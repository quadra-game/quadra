
package QServ::HTML;

use strict;

use Socket;

my $resolve_ips = 0;

$QServ::commands{getgameshtml} = \&do_getgameshtml;
$QServ::commands{gethighscoreshtml} = \&do_getscoreshtml;

my @team_colors = (
		   '#F06000',
		   '#00D0D0',
		   '#D00000',
		   '#D000D0',
		   '#D0D000',
		   '#00D000',
		   '#0000D0'
		  );
my $default_quadra_port = 3456;

my %ip_cache;

sub resolve_ip {
  my($ip) = @_;
  my $name;

  if($resolve_ips && !exists($ip_cache{$ip})) {
    ($name) = gethostbyaddr(inet_aton($ip), AF_INET);
    if(defined($name)) {
      $ip_cache{$ip} = "$name ($ip)";
    } else {
      $ip_cache{$ip} = $ip;
    }
  }

  return $ip_cache{$ip} || $ip;
}

sub do_getgameshtml {
  my($cgi, $params) = @_;

  my $games = QServ::get_games();

  print $cgi->header('text/html');

  print "<table border=1>\n<th>Game name</th><th>IP</th><th>Players</th>\n";

  foreach my $game (sort(keys(%$games))) {
    my $port = '';

    if(defined($games->{$game}{port}) && $games->{$game}{port} != $default_quadra_port) {
      $port = ':' . $games->{$game}{port};
    }

    print "<tr valign=\"top\">";
    print "<td>$games->{$game}{name}</td>";
    print "<td>", resolve_ip($games->{$game}{info}{remoteaddr}), $port, "</td><td>\n";
    foreach my $player (values(%{$games->{$game}{players}})) {
      print "    <font color=\"$team_colors[$player->{team}]\">";
      print "$player->{name}";
      print "</font><br>\n";
    }
    print "</td></tr>\n";
  }

  print "</table>\n";
}

sub do_getscoreshtml {
  my($cgi, $params) = @_;
  my $scores = QServ::get_scores();
  my @scores = keys(%$scores);
  my $pos = 1;
  my $num = $params->{num} || $QServ::default_demos;

  @scores = sort {$b <=> $a} @scores;
  if(scalar(@scores) > $num) {
    splice(@scores, $num);
  }

  print $cgi->header('text/html');

  print "<table border=1>\n<th>Position</th><th>Player name</th><th>Score</th><th>Lines</th><th>Level</th><th>Date</th>\n";

  foreach my $score (@scores) {
    my($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime($scores->{$score}{date});

    print "<tr valign=\"top\">";
    print "<td align=\"right\">", $pos++, "</td>";
    print "<td>", $scores->{$score}{name}, "</td>";
    print "<td align=\"right\">", $scores->{$score}{score}, "</td>";
    print "<td align=\"right\">", $scores->{$score}{lines}, "</td>";
    print "<td align=\"right\">", $scores->{$score}{level}, "</td>";
    print "<td align=\"right\">", sprintf("%i-%02i-%02i %02i:%02i:%02i", $year + 1900, $mon + 1, $mday, $hour, $min, $sec), "</td>";
    print "</tr>\n";
  }

  print "</table>\n";
}

1;

