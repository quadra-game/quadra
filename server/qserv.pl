#! /usr/bin/perl -w -T

use CGI;
use Data::Dumper;
use DB_File;
use Fcntl qw(:flock);

$TIMEOUT = 180;
$DEFAULTDEMOS = 5;
$MAXDEMOS = 100;

sub superuser {
	my($params) = shift;
	if($params->{info}{remoteaddr} eq '127.0.0.1') {
		return 1;
	}
	return 0;
}

%commands = (
	'ping' =>
	sub {
		my($params) = shift;
		my($ret) = "ping\n" . &format($params);
		return $ret;
	},

	'getgames' =>
	sub {
		my($params) = shift;
		my($ret);
		my($db) = &allocdb("games.dbm", LOCK_SH);
		my(%sent);
		$ret = "Current games\n";
		while(($key, $val) = each(%$db)) {
			my($data) = eval($val);
			if(time - $data->{info}{lastupdate} <= $TIMEOUT) {
				if(defined $params->{info}{quadra_version}) {
					if($params->{info}{quadra_version} eq "1.1.2") {
						delete $data->{players};
					}
				}
				else {
					if(!defined $params->{info}{qsnoop_version}) {
						$key =~ s/:.*//;
					}
				}
				if(!defined $sent{$key}) {
					$ret .= &format($data, $key);
				}
				$sent{$key} = 1;
			}
		}
		&freedb($db);
		return $ret;
	},

	'getgameshtml' =>
	sub {
		my($params) = shift;
		my($ret);
		my(%colors) = (
			'0' => '#F06000',
			'1' => '#00D0D0',
			'2' => '#D00000',
			'3' => '#D000D0',
			'4' => '#D0D000',
			'5' => '#00D000',
			'6' => '#0000D0',
		);
		my($db) = &allocdb("games.dbm", LOCK_SH);
		$ret = "<table border=1>\n";
		$ret .= "<th>Game name</th>\n";
		$ret .= "<th>IP</th>\n";
		$ret .= "<th>Players</th>\n";
		while(($key, $val) = each(%$db)) {
			my($data) = eval($val);
			if(time - $data->{info}{lastupdate} <= $TIMEOUT) {
				$ret .= "<tr valign=top>\n";
				$ret .= "<td>$data->{name}</td>\n";
				$ret .= "<td>$data->{info}{remoteaddr}</td>\n";
				$ret .= "<td>\n";
				while(($key, $val) = each(%{$data->{players}})) {
					$ret .= "<font color=\"$colors{$val->{team}}\">\n";
					$ret .= "$val->{name}\n</font>\n";
					$ret .= "<br>\n";
				}
				$ret .= "</td>\n";
				$ret .= "</tr>\n";
			}
		}
		&freedb($db);
		$ret .= "</table>\n";
		return $ret;
	},

	'postgame' =>
	sub {
		my($params) = shift;
		my($ret);
		my($db) = &allocdb("games.dbm", LOCK_EX);
		if(!defined $params->{port}) {
			$params->{port} = 3456;
		}
		my($addr) = $params->{info}{remoteaddr};
		$addr = $addr . ":" . $params->{port};
		$params->{info}{lastupdate} = time;
		if(exists $db->{$addr}) {
			$ret = "Game updated\n";
		}
		else {
			$ret = "Game added\n";
		}
		$db->{$addr} = Dumper($params);
		&freedb($db, 1);
		return $ret;
	},

	'deletegame' =>
	sub {
		my($params) = shift;
		my($ret);
		my($db) = &allocdb("games.dbm", LOCK_EX);
		if(!defined $params->{port}) {
			$params->{port} = 3456;
		}
		my($addr) = $params->{info}{remoteaddr} . ":" . $params->{port};
		if(exists $db->{$addr}) {
			delete $db->{$addr};
			$ret = "Game deleted\n";
		}
		else {
			$ret = "Game not found\n";
		}
		&freedb($db, 1);
		return $ret;
	},

	'deleteoldgames' => 
	sub {
		my($params) = shift;
		my($ret);
		if(&superuser($params)) {
			my($db) = &allocdb("games.dbm", LOCK_EX);
			$ret = "Deleded games\n";
			my(@reaplist);
			while(($key, $val) = each(%$db)) {
				my($game) = eval($val);
				if(time - $game->{info}{lastupdate} > $TIMEOUT) {
					push @reaplist, $key;
					$ret .= &format($game, $key);
				}
			}
			foreach $key (@reaplist) {
				delete $db->{$key};
			}
			&freedb($db, 1);
		}
		else {
			$ret = "Permission denied\n";
		}
		return $ret;
	},

	'gethighscores' =>
	sub {
		my($params) = shift;
		my($db) = &allocdb("highscores.dbm", LOCK_SH);
		my($ret) = "Ok\n";
		$ret .= &format(&getscores($db, $params->{num}));
		&freedb($db);
		return $ret;
	},

	'gethighscoreshtml' =>
	sub {
		my($params) = shift;
		my($db) = &allocdb("highscores.dbm", LOCK_SH);
		$scores = &getscores($db, $params->{num});
		&freedb($db);
		$ret = "<table border=1>\n";
		$ret .= "<th>Position</th>\n";
		$ret .= "<th>Player name</th>\n";
		$ret .= "<th>Score</th>\n";
		$ret .= "<th>Lines</th>\n";
		$ret .= "<th>Level</th>\n";
		$ret .= "<th>Date</th>\n";
		my($i);
		for($i=0; ; $i++) {
			my($high) = sprintf "high%03i", $i;
			last unless exists $scores->{$high};
			my($data) = $scores->{$high};
			$ret .= "<tr valign=top>\n";
			$ret .= "<td align=right>";
			$ret .= $i+1;
			$ret .= "</td>\n";
			$ret .= "<td>$data->{name}</td>\n";
			$ret .= "<td align=right>$data->{score}</td>\n";
			$ret .= "<td align=right>$data->{lines}</td>\n";
			$ret .= "<td align=right>$data->{level}</td>\n";
			$ret .= "<td align=right>";
			my($sec, $min, $hour, $mday, $mon, $year,
			   $wday, $yday, $isdst) = localtime $data->{date};
			$ret .= sprintf "%i-%02i-%02i %02i:%02i:%02i",
			        $year+1900, $mon+1, $mday, $hour, $min, $sec;
			$ret .= "</td>\n";
			$ret .= "</tr>\n";
		}
		$ret .= "</table>\n";
		return $ret;
	},

	'postdemo' =>
	sub {
		my($params) = shift;
		my($ret) = "Ok\nresponse Not a highscore\n";
		my($db) = &allocdb("highscores.dbm", LOCK_EX);
		my($key) = &getuniquekey($db);
		my($candidate) = {};
		$candidate->{key} = $key;
		$candidate->{score} = $params->{score};
		my($i);
		for($i=0; $i<$MAXDEMOS; $i++) {
			my($highkey) = sprintf "high%03i", $i;
			if(!exists $db->{$highkey}) {
				$ret = "Ok\nresponse Demo added\n";
				$ret .= "position $i\n";
				$ret .= "key $key\n";
				$db->{$key} = Dumper($params);;
				$db->{$highkey} = Dumper($candidate);
				last;
			}
			else {
				my($high) = eval($db->{$highkey});
				if($candidate->{score} > $high->{score}) {
					my($j);
					for($j=$MAXDEMOS-1; $j>$i; $j--) {
						my($key1) = sprintf "high%03i", $j;
						my($key2) = sprintf "high%03i", $j-1;
						if(exists $db->{$key2}) {
							$db->{$key1} = $db->{$key2};
						}
					}
					$ret = "Ok\nresponse Demo added\n";
					$ret .= "position $i\n";
					$ret .= "key $key\n";
					$db->{$key} = Dumper($params);;
					$db->{$highkey} = Dumper($candidate);
					last;
				}
				if($candidate->{score} == $high->{score}) {
					my($highdetails) = eval($db->{$high->{key}});
					if($params->{rec} eq $highdetails->{rec}) {
						$ret = "Ok\nresponse Demo already here\n";
						$ret .= "position $i\n";
						$ret .= "key $high->{key}\n";
						last;
					}
				}
			}
		}
		$ret .= &format(&getscores($db, $params->{num}));
		&freedb($db, 1);
		return $ret;
	},

	'rebuildhighscores' =>
	sub {
		my($params) = shift;
		my($ret);
		if(&superuser($params)) {
			$ret = "Ok\n";
			my($db) = &allocdb("highscores.dbm", LOCK_EX);
			my(@reaplist);
			my(@highkeys);
			my(@highscores);
			my(@highnames);
			my(%name_to_score);
			while(($key, $val) = each(%$db)) {
				if($key =~ 'high') {
					push @reaplist, $key;
					next;
				}
				my($debugging) = 0;
				my($i);
				my($data) = eval $val;
				if(!defined $data->{name} || !defined $data->{score}) {
					push @reaplist, $key;
					next;
				}
				if($data->{score} < 150000) {
					push @reaplist, $key;
					next;
				}
				if($data->{name} eq "Hi Normy!!!") {
					push @reaplist, $key;
					next;
				}
				if($data->{name} eq "Norm") {
					$debugging = 1;
				}
				my($inserted)=0;
				if(defined $name_to_score{$data->{name}}) {
					if($name_to_score{$data->{name}} > $data->{score}) {
						next;
					}
				}
				$name_to_score{$data->{name}} = $data->{score};
				for($i=0; $i<(scalar @highkeys); $i++) {
					if($data->{score} > $highscores[$i] || ($data->{score} == $highscores[$i] && $key < $highkeys[$i])) {
						my($j) = $i;
						while($j < (scalar @highkeys)) {
							if($highnames[$j] eq $data->{name}) {
								last;
							}
							$j++;
						}
						for(; $j>$i; $j--) {
							$highkeys[$j] = $highkeys[$j-1];
							$highscores[$j] = $highscores[$j-1];
							$highnames[$j] = $highnames[$j-1];
						}
						$highkeys[$i] = $key;
						$highscores[$i] = $data->{score};
						$highnames[$i] = $data->{name};
						$inserted=1;
						if($debugging) {
							$ret .= "debug inserted $data->{score} at $i\n";
						}
						last;
					}
				}
				if(!$inserted) {
					if($debugging) {
						$ret .= "debug appending $data->{score}\n";
					}
					push @highkeys, $key;
					push @highscores, $data->{score};
					push @highnames, $data->{name};
				}
			}
			$ret .= 'debug scalar @highkeys==';
			$ret .= scalar @highkeys;
			$ret .= "\n";
			foreach $key (@reaplist) {
				delete $db->{$key};
			}
			my($i);
			for($i=0; $i<$MAXDEMOS; $i++) {
				my($highkey) = sprintf "high%03i", $i;
				my($data) = eval $db->{$highkeys[$i]};
				my($infos) = {};
				$infos->{name} = $data->{name};
				$infos->{score} = $data->{score};
				$infos->{lines} = $data->{lines};
				$infos->{level} = $data->{level};
				$infos->{key} = $highkeys[$i];
				$db->{$highkey} = Dumper($infos);
			}

			&freedb($db, 1);
		}
		else {
			$ret = "Permission denied\n";
		}
		return $ret;
	},

	'deletedemo' =>
	sub {
		my($params) = shift;
		my($ret);
		if(&superuser($params)) {
			my($db) = &allocdb("highscores.dbm", LOCK_EX);
			$ret = "Deleded demos\n";
			my(@reaplist);
			while(($key, $val) = each(%$db)) {
				if($key =~ "high.*") {
					next;
				}
				else {
					my($demo) = eval($val);
					if($demo->{score} == $params->{score}) {
						push @reaplist, $key;
						$ret .= &format($demo, $key);
					}
				}
			}
			foreach $key (@reaplist) {
				delete $db->{$key};
			}
			&freedb($db, 1);
		}
		else {
			$ret = "Permission denied\n";
		}
		return $ret;
	},
);

sub getscores {
	my($db) = shift;
	my($num) = shift;
	$num = $DEFAULTDEMOS unless defined $num;
	if($num > $MAXDEMOS) {
		$num = $MAXDEMOS;
	}
	else {
		if($num < 0) {
			$num = 0;
		}
	}
	my($ret) = {};
	my($i) = 0;
	for($i=0; $i<$num; $i++) {
		my($key) = sprintf "high%03i", $i;
		if(exists $db->{$key}) {
			my($key2) = eval($db->{$key});
			my($data) = eval($db->{$key2->{key}});
			$data->{date} = $key2->{key};
			$ret->{$key} = $data;
		}
		else {
			last;
		}
	}
	return $ret;
}

&main;

sub main {
	my($q) = new CGI;
	my($data) = $q->param('data');
	$data = "" unless defined $data;
	my(@req) = split("\n", $data);

	my($cmd) = shift(@req);
	$cmd = "nothing" unless defined $cmd;
	my($params) = parse(@req);

	#Ajoute des params
	if(defined $params->{address}) {
		my(@addrs) = split " ", $params->{address};
		$params->{info}{remoteaddr} = $addrs[0];
		foreach $ad (@addrs) {
			if($ad == $q->remote_addr()) {
				$params->{info}{remoteaddr} = $ad;
			}
		}
		delete $params->{address};
	}
	else {
		$params->{info}{remoteaddr} = $q->remote_addr();
	}

	if(exists $commands{$cmd}) {
		$reply = &{$commands{$cmd}}($params);
		$reply = "\n" unless defined $reply;
	}
	else {
		$reply = "Hi, I'm the Quadra game server.\n" .
		         "You should use Quadra to talk to me :).\n";
	}
	if($cmd =~ /html$/) {
		print $q->header('text/html');
	}
	else {
		print $q->header('text/plain');
	}
	print $reply;
}

sub getuniquekey {
	my($db) = shift;
	my($ret) = time . '.00';
	while(exists $db->{$ret}) {
		$ret += 0.01;
	}
	return $ret;
}

sub allocdb {
	my($fn) = shift;
	my($lock) = shift;
	my(%db);
	my($db) = tie(%db, "DB_File", $fn, O_CREAT|O_RDWR, 0666, $DB_HASH);
	my($fd) = $db->fd;
	undef $db;
	open(DB_FH, "+<&=$fd");
	if($lock) {
		flock(DB_FH, $lock);
	}
	return \%db;
}

sub freedb {
	my($db) = shift;
	my($dbobj) = tied(%$db);
	my($unlock) = shift;
	$dbobj->sync;
	if($unlock) {
		flock(DB_FH, LOCK_UN);
	}
	undef $dbobj;
	untie %$db;
	close(DB_FH);
}

sub parse {
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

sub format {
	my($params) = shift;
	my($root) = shift;
	my($ret);
	my($key, $val);
	while(($key, $val) = each(%$params)) {
		if(ref($val) ne 'HASH') {
			if($root) {
				$ret .= "$root/$key $val\n";
			}
			else {
				$ret .= "$key $val\n";
			}
		}
		else {
			if(defined $root) {
				$ret .= &format($val, "$root/$key");
			}
			else {
				$ret .= &format($val, "$key");
			}
		}
	}
	return $ret;
}
