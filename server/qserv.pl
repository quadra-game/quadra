#! /usr/bin/perl -Tw

package QServ;

use strict;
#use lib '/srv/www/perl';

use CGI;
use CGI::Carp;
use Data::Dumper;
#use QServ::HTML;
#use QServ::Message;

our $debug = 1;

our %commands = (
		 postgame => \&do_postgame,
		 deletegame => \&do_deletegame,
		 postdemo => \&do_postdemo,
		 gethighscores => \&do_gethighscores,
		 getgames => \&do_getgames,
		 %commands
		);

our %cleanups = (
		 games => \&cleanup_games,
		 scores => \&cleanup_scores,
		 %cleanups
		);

our $datadir = '/srv/www/data/qserv';
our $gamesdir = $datadir . '/games';
our $scoresdir = $datadir . '/scores';
our $timeout = 180;
our $scores_to_keep = 100;
our $default_demos = 5;

sub set_param {
  my($params, $key, $value) = @_;
  my @path = split('/', $key);
  my $dir = $params;
  my $file = pop(@path);

  while(@path) {
    my $path = shift(@path);
    if(!exists($dir->{$path})) {
      $dir->{$path} = {};
    }
    $dir = $dir->{$path};
  }

  $dir->{$file} = $value;
}

sub parse {
  my $output = {};

  foreach (@_) {
    my($key, $value) = split(/\s/, $_, 2);
    set_param($output, $key, $value);
  }

  return $output;
}

sub format_params {
  my($root, $params) = @_;
  my $output = '';

  foreach my $key (keys(%$params)) {
    if(ref($params->{$key}) eq 'HASH') {
      $output .= format_params($root.'/'.$key, $params->{$key});
    } else {
      $output .= "$root/$key $params->{$key}\n";
    }
  }

  return $output;
}

sub tweak {
  my($cgi, $params) = @_;

  set_param($params, 'info/remoteaddr', $cgi->remote_host());

  $params->{port} = $params->{port} || 3456;
  $params->{port} =~ s/\D//g;
  if($params->{port} > 65535) {
    $params->{port} = 3456;
  }
}

sub is_game {
  my($gamefile) = @_;

  if($gamefile !~ m/(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}:\d{1,5})/) {
    return undef;
  }
  $gamefile = $1;

  return undef unless -f $gamesdir . '/' . $gamefile;
  return undef unless -r $gamesdir . '/' . $gamefile;

  return $gamefile;
}

sub is_score {
  my($scorefile) = @_;

  if($scorefile !~ m/(\d+)/) {
    return undef;
  }
  $scorefile = $1;

  return undef unless -f $scoresdir . '/' . $scorefile;
  return undef unless -r $scoresdir . '/' . $scorefile;

  return $scorefile;
}

sub expire_game {
  my($gamefile) = @_;

  my @statinfo = stat($gamesdir . '/' . $gamefile);
  my $mtime = $statinfo[9];

  if(time() - $mtime >= $timeout) {
    warn("deleting $gamefile ($mtime)") if $debug >= 3;
    unlink($gamesdir.'/'.$gamefile);
    return 1;
  }

  warn("NOT deleting $gamefile ($mtime)") if $debug >= 3;

  return 0;
}

sub default {
  my($cgi, $params) = @_;

  print $cgi->header('text/plain');
  print "Hi, I'm the NEW Quadra game server.\nYou should use Quadra to talk to me :).\n";
}

sub get_games {
  my %games;
  my @gamefiles;

  opendir(DIR, $gamesdir);
  rewinddir(DIR);
  while(my $gamefile = readdir(DIR)) {
    push(@gamefiles, $gamefile);
  }
  closedir(DIR);

  foreach my $gamefile (@gamefiles) {
    my $gameinfo;

    $gamefile = is_game($gamefile);
    next unless defined($gamefile);
    next if expire_game($gamefile);

    open(GAME, "<$gamesdir/$gamefile");
    $gameinfo = do {
      local $/;
      <GAME>;
    };
    close(GAME);

    $gameinfo =~ m/(.*)/s;
    $gameinfo = $1;
    $gameinfo = eval('my '.$gameinfo);

    $games{$gamefile} = $gameinfo;
  }

  delete($cleanups{games});

  return \%games;
}

sub get_scores {
  my %scores;
  my @scores;
  my @old;

  opendir(DIR, $scoresdir);
  rewinddir(DIR);
  while(my $scorefile = readdir(DIR)) {
    $scorefile = is_score($scorefile);
    push(@scores, $scorefile) if defined $scorefile;
  }
  closedir(DIR);

  @scores = sort {$b <=> $a} @scores;

  if(scalar(@scores) > $scores_to_keep) {
    @old = splice(@scores, $scores_to_keep);
  }

  foreach my $scorefile (@old) {
    warn("deleting score $scorefile") if $debug >= 2;
    unlink($scoresdir.'/'.$scorefile);
  }

  foreach my $scorefile (@scores) {
    my $scoreinfo;

    open(SCORE, "<$scoresdir/$scorefile") || next;
    $scoreinfo = do {
      local $/;
      <SCORE>;
    };
    close(SCORE);

    $scoreinfo =~ m/(.*)/s;
    $scoreinfo = $1;
    $scoreinfo = eval('my '.$scoreinfo);

    $scores{$scorefile} = $scoreinfo;
  }

  delete($cleanups{scores});

  return \%scores;
}

sub cleanup_games {
  opendir(DIR, $gamesdir);
  rewinddir(DIR);
  while(my $gamefile = readdir(DIR)) {
    $gamefile = is_game($gamefile);
    expire_game($gamefile) if defined $gamefile;
  }
  closedir(DIR);
  warn("cleaned up games\n") if $debug >= 3;
}

sub cleanup_scores {
  get_scores();
  warn("cleaned up scores\n") if $debug >= 3;
}

sub main {
  my $cgi = CGI->new();
  my $data = $cgi->param('data') || '';
  my @req = split('\n', $data);
  my $cmd = shift(@req) || 'nothing';
  my $params = parse(@req);

  warn("$cmd\n") if $debug >= 2;
  warn(format_params("foo", $params), "\n") if $debug >= 3;

  tweak($cgi, $params);

  if(defined($commands{$cmd})) {
    $commands{$cmd}->($cgi, $params);
  } else {
    warn("unknown command \"$cmd\"\n");
    default($cgi, $params);
  }

  if(scalar(keys(%cleanups)) && (($debug >= 2) || !fork())) {
    if($debug < 2) {
      close(STDIN);
      close(STDOUT);
      close(STDERR);
    }

    foreach my $cleanup (values(%cleanups)) {
      $cleanup->();
    }
  }
}

sub do_postgame {
  my($cgi, $params) = @_;
  my $addr;
  # FIXME: probably a security hole here
  my $tmpfile = $gamesdir."/$$.tmp";
  my $gamefile = $gamesdir.'/';

  $addr = $params->{info}{remoteaddr} . ':' . $params->{port};
  if($addr =~ /^([-\@\w.:]+)$/) {
    $addr = $1;
  } else {
    die("bad data in $addr");
  }
  $gamefile .= $addr;
  set_param($params, 'info/lastupdate', time);

  open(TMP, ">$tmpfile") or die $!;
  print TMP Dumper($params);
  close(TMP);

  print $cgi->header('text/plain');

  if(-e $gamefile) {
    rename($tmpfile, $gamefile) or die $!;
    print "Game updated\n";
  } else {
    rename($tmpfile, $gamefile) or die $!;
    print "Game added\n";
  }
}

sub do_deletegame {
  my($cgi, $params) = @_;
  my $game = $params->{info}{remoteaddr}.":".$params->{port};

  $game =~ s/\.{2,}/./;

  if($game =~ /^([-\@\w.:]+)$/) {
    $game = $1;
  } else {
    die("bad data in $game");
  }

  print $cgi->header('text/plain');
  if(unlink($gamesdir . '/' . $game)) {
    print "Game deleted\n";
  } else {
    print "Game not found\n";
  }
}

sub do_postdemo {
  my($cgi, $params) = @_;
  my $tmpfile = $scoresdir."/$$.tmp";
  my $scorefile = $scoresdir.'/';
  my $score = $params->{score};

  if(defined($score)) {
    if($score =~ /^(\d+)$/) {
      $score = $1;
    } else {
      die("bad data in $score");
    }
    $scorefile .= $score;

    open(TMP, ">$tmpfile") or die $!;
    print TMP Dumper($params);
    close(TMP);

    rename($tmpfile, $scorefile) or die $!;
  }

  do_gethighscores($cgi, $params);
}

sub do_gethighscores {
  my($cgi, $params) = @_;
  my $scores = get_scores();
  my @scores = keys(%$scores);
  my $high = 0;
  my $num = $params->{num} || $default_demos;

  @scores = sort {$b <=> $a} @scores;
  if(scalar(@scores) > $num) {
    splice(@scores, $num);
  }

  print $cgi->header('text/plain');
  print "Ok\n";

  foreach my $score (@scores) {
    print format_params(sprintf("high%03i", $high++), $scores->{$score});
  }
}

sub do_getgames {
  my($cgi, $params) = @_;
  my $games = get_games();
  my %sent;

  print $cgi->header('text/plain');
  print "Current games\n";

  foreach my $game (sort(keys(%$games))) {
    my($host, $port) = split(/:/, $game);
    my $key = $game;

    if(defined($params->{info}{quadra_version})) {
      if($params->{info}{quadra_version} eq '1.1.2') {
	delete($games->{$game}->{players});
      }
    } else {
      if(!defined($params->{info}{qsnoop_version})) {
	$key = $host;
      }
    }

    if(!exists($sent{$key})) {
      print format_params($key, $games->{$game});
      $sent{$key} = 1;
    }
  }
}

main();

