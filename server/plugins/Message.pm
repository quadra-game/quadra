
package QServ::Message;

use strict;

use CGI::Carp;
use Data::Dumper;

my $send_games = 1;
my $separator = '---------------------------------------------';
my @message = (
	       'IMPORTANT MESSAGE',
	       ' ',
	       'The Quadra game server',
	       'has moved. See the',
	       'following web page for',
	       'more information:',
	       ' ',
	       ' ',
	       'http://quadra.sf.net/move',
	      );

$QServ::commands{getgames} = \&send_message;

sub remove_old_msg {
  opendir(DIR, $QServ::gamesdir);
  rewinddir(DIR);
  while(my $gamefile = readdir(DIR)) {
    if($gamefile =~ m/^127\.0\.0/) {
      unlink($QServ::gamesdir.'/'.QServ::is_game($gamefile));
    }
  }
  closedir(DIR);
}

sub send_message {
  my($cgi, $params) = @_;
  my $count = 1;

  if($send_games) {
    my $tmpfile = $QServ::gamesdir."/$$.tmp";
    remove_old_msg($cgi);

    push(@message, $separator) if defined($separator);

    foreach my $line (@message) {
      my $fakegame = {
                       name => $line,
                     };
      my $fakefile = sprintf("127.0.0.%03i:1", $count);
      open(TMP, ">$tmpfile") or die $!;
      print TMP Dumper($fakegame);
      close(TMP);

      rename($tmpfile, $QServ::gamesdir.'/'.$fakefile) or die $!;

      ++$count;
    }

    QServ::do_getgames->(@_);
  } else {
    print $cgi->header('text/plain');
    print "Current games\n";

    foreach my $line (@message) {
      print QServ::format_params('127.0.0.'.$count,
			         {
				  name => $line,
			         });
      ++$count;
    }
  }
}

1;

