
use Tk;
use Tk::Tree;
use LWP::Simple;

sub getGames;

sub updateGames {
	scalar keys %$games && $listboxGames->delete(0, end);
	while(my($key, $val) = each(%$games)) {
		$listboxGames->insert(end, $val->{'name'});
		$listboxGamesKey{$listboxGames->size-1} = $key;
	}

	foreach (@rawGames) {
		my $text = $_;
		my $path;
		while(length $text) {
			my($left, $right) = split "/", $text, 2;
			$path .= $left;
			$hlist->add($path, -text => $left) unless $hlist->info('exists', $path);
			$path .= '/';
			$text = $right;
		}
	}
	$hlist->autosetmode;
}

sub updatePlayers {
	$listboxPlayers->delete(0, end);
	my $gameKey = $listboxGamesKey{$listboxGames->curselection};
	my $players = $games->{$gameKey}->{'players'};
	while(my($key, $val) = each(%$players)) {
		$listboxPlayers->insert(end, $val->{'name'});
	}
}

my $main = MainWindow->new;

$refreshButton = $main->Button(-text => 'Refresh', -command => sub {
	$games = getGames;
	updateGames;
})->pack;


$hlist = $main->ScrlTree(
	-scrollbars => oe,
	-width => 100,
	-itemtype => text,
	-separator => '/',
	-selectmode => single
)->pack;

$listboxGames = $main->Listbox(-width => 100)->pack;
$listboxGames->bind('<ButtonRelease-1>', sub {
	updatePlayers;
});

$listboxPlayers = $main->Listbox(-width => 100)->pack;

MainLoop;

sub parseResponse;

sub getGames {
	my $response = get('http://quadra.sourceforge.net/cgi-bin/qserv.pl?data=getgames%0ainfo/quadra_version%201.1.7');
	return {} unless defined $response;

	my($result, $data) = split "\n", $response, 2;
	return {} unless $result == "Current games";

	@rawGames = split "\n", $data;
	return parseResponse(@rawGames);
}

sub parseResponse {
	my($ret);
	$ret = {};
	foreach(@_) {
		my($path, $val) = split(/\s/, $_, 2);
		my($curr) = $ret;
		while($path) {
			$path =~ /([^\s\/]+)\/?(.*)/;
			my($rep) = $1;
			$path = $2;
			if($path eq '/') {
				$path = '';
			}
			if(!exists($curr->{$rep})) {
				if($path ne '') {
					$curr->{$rep} = {};
					$curr = $curr->{$rep};
				}
				else {
					$curr->{$rep} = $val;
				}
			}
			else {
				$curr = $curr->{$rep};
			}
		}
	}
	return $ret;
}
