#! /usr/bin/perl -w

use strict;
use CGI;
use RecReader;

my $cgi = new CGI;

my $recfile = $cgi->param('game');
$recfile = "gg.0000" unless defined $recfile;
$recfile =~ s/[^A-Za-z0-9\/\.\-]+//g;
$recfile =~ s/\/+/\//g;
$recfile =~ s/^\///;

my $player = $cgi->param('player');
$player = "" unless defined $player;
$player = "" unless $player =~ /[0-9]+/;

my $players;

my $reader = new RecReader("/projects/quadra/stats/games/$recfile.rec", \&processEvent);
if(!defined $reader) {
	print $cgi->header('text/plain');
	print "Can't read '$recfile.rec'\n";
	exit;
}

if(!defined $players->{$player}) {
	$player = "";
}

my %blockPics = (0 => 'Cube', 1 => 'S', 2 => 'Z', 3 => 'L', 4 => 'I_L', 5 => 'I', 6 => 'T');
my %teamColors = (
	orange => '#F06000',
	cyan => '#00D0D0',
	red => '#D00000',
	purple => '#D000D0',
	yellow => '#D0D000',
	green => '#00D000',
	blue => '#0020FF',
	gray => '#707070'
);

sub evaluateMove {
	my $params = shift;
	my $moveType = shift;

	if(!defined $params) {
		return 0;
	}

	my $ret = 0;

	if($moveType eq 'lines') {
		$ret = $params->{lines} * 256 + $params->{combo};
	}
	elsif($moveType eq 'combo') {
		$ret = $params->{combo} * 256 + $params->{lines};
	}
	elsif($moveType eq 'clean' && $params->{clean} eq 'true') {
		$ret = $params->{lines} * 256 + $params->{combo};
	}

	return $ret;
}

sub processEvent {
	my $frame = shift;
	my $event = shift;
	my $params = shift;

	if($event eq "player_join") {
		my $id = $params->{id};
		$players->{$id}->{id} = $id;
		$players->{$id}->{name} = $params->{name};
		$players->{$id}->{team} = $params->{team};
	}
	elsif($event eq "player_snapshot") {
		my $id = $params->{id};
		for my $moveType ('lines', 'combo', 'clean') {
			my $thisValue = &evaluateMove($params, $moveType);
			my $oldValue = &evaluateMove($players->{$id}->{"best_$moveType"}, $moveType);
			if($thisValue > $oldValue) {
				$players->{$id}->{"best_$moveType"} = $params;
			}
		}
	}
	elsif($event eq "player_stampblock") {
		my $id = $params->{id};
		my $block = $params->{block};
		$players->{$id}->{blocks} += 1;
		$players->{$id}->{"blocks_$block"} += 1;

		$players->{$id}->{points} += $params->{points};
		$players->{$id}->{"points_$block"} += $params->{points};

		$players->{$id}->{times_rotated} += $params->{times_rotated};
		$players->{$id}->{"times_rotated_$block"} += $params->{times_rotated};

		$players->{$id}->{playing_time} += $params->{time_held};
		$players->{$id}->{"time_held_$block"} += $params->{time_held};
	}
	elsif($event eq "player_lines_cleared") {
		my $id = $params->{id};
		$players->{$id}->{lines} += $params->{lines};
		$players->{$id}->{points} += $params->{points};
	}
	elsif($event eq "player_dead") {
		my $id = $params->{id};
		my $fragger_id = $params->{fragger_id};
		my $type = $params->{type};
		$players->{$id}->{deaths} += 1;
		$players->{$id}->{"deaths_$type"} += 1;
		if($fragger_id != 0) {
			$players->{$fragger_id}->{frags} += 1;
			$players->{$fragger_id}->{"frags_$type"} += 1;
		}
	}
	elsif($event eq "playing_end") {
		my @reaplist;
		while(my ($key, $val) = each %$players) {
			push @reaplist, $key unless defined $val->{playing_time};
		}
		for(@reaplist) {
			delete $players->{$_};
		}
	}
}


print $cgi->header('text/html');


if($player eq "") {
	print $cgi->start_html(-title => "Stats");
}
else {
	print $cgi->start_html(-title => "Stats for $players->{$player}->{name}");
}

print '<center>';
print &pic("Quadra.gif");
print '<table border=0>';
print '<tr bgcolor="#000080">';
print '<th>&nbsp;</th>';
print '<th><font color="#FFFFFF">Frags</font></th>';
print '<th><font color="#FFFFFF">Deaths</font></th>';
print '<th><font color="#FFFFFF">Points</font></th>';
print '<th><font color="#FFFFFF">Lines</font></th>';
print '<th><font color="#FFFFFF">Blocks</font></th>';
print '<th><font color="#FFFFFF">PPM</font></th>';
print '<th><font color="#FFFFFF">BPM</font></th>';
print '</tr>', "\n";
my @players;
if($player eq "") {
	@players = sort {
		$::b->{frags} <=> $::a->{frags} ||
		$::a->{deaths} <=> $::b->{deaths} ||
		$::b->{points} <=> $::a->{points} ||
		$::b->{lines} <=> $::a->{lines};
	} values %$players;
}
else {
	@players = $players->{$player};
}
for my $player (@players) {
	print &playerRow($player);
}
print '</table><br><br>', "\n";

print '<table border=0>';
print '<tr bgcolor="#000080">';
print '<th>&nbsp;</th>';
print '<th><font color="#FFFFFF">Count</font></th>';
print '<th><font color="#FFFFFF">Avg. Rotations</font></th>';
print '<th><font color="#FFFFFF">Avg. Time</font></th>';
print '</tr>', "\n";
for my $block (0..6) {
	print &blockRow($block);
}
print '</table><br><br>', "\n";


my ($bestLines, $bestCombo, $bestClean);
if($player eq "") {
	my $thisValue;
	my $oldValue;
	while(my ($key, $val) = each %$players) {
		$thisValue = &evaluateMove($val->{best_lines}, 'lines');
		$oldValue = &evaluateMove($bestLines, 'lines');
		if($thisValue > $oldValue) {
			$bestLines = $val->{best_lines};
		}

		$thisValue = &evaluateMove($val->{best_combo}, 'combo');
		$oldValue = &evaluateMove($bestCombo, 'combo');
		if($thisValue > $oldValue) {
			$bestCombo = $val->{best_combo};
		}

		$thisValue = &evaluateMove($val->{best_clean}, 'clean');
		$oldValue = &evaluateMove($bestClean, 'clean');
		if($thisValue > $oldValue) {
			$bestClean = $val->{best_clean};
		}
	}
}
else {
	$bestLines = $players->{$player}->{best_lines};
	$bestCombo = $players->{$player}->{best_combo};
	$bestClean = $players->{$player}->{best_clean};
}
print '<table border=0 cellpadding=8>';
print '<tr bgcolor="#000080">';
print '<th align=center width="33%"><font size=+1 color="#FFFFFF">Best move</font></th>';
print '<th align=center width="33%"><font size=+1 color="#FFFFFF">Best combo</font></th>';
print '<th align=center width="33%"><font size=+1 color="#FFFFFF">Best clean</font></th>';
print '</tr>', "\n";
print '<tr>';
print '<td align=center valign=bottom bgcolor="#A0A0A0">', &snapshot($bestLines), '</td>';
print '<td align=center valign=bottom bgcolor="#A0A0A0">', &snapshot($bestCombo), '</td>';
print '<td align=center valign=bottom bgcolor="#A0A0A0">', &snapshot($bestClean), '</td>';
print '</tr>', "\n";
print '</table>', "\n";
print '</center>';


print $cgi->end_html();


sub playerLink {
	my $id = shift;
	return
		'<a href="' .
		$cgi->url(-relative => 1) .
		"?game=$recfile&player=$id\">" .
		&playerName($id) .
		'</a>';
}

sub playerName {
	my $id = shift;
	return &teamColored($players->{$id}->{name}, $players->{$id}->{team});
}

sub teamColored {
	my $text = shift;
	my $team = shift;

	return '<font color="' .
		$teamColors{$team} .
		'">' .
		$text .
		'</font>';
}

sub playerRow {
	my $val = shift;
	return
		'<tr>' .
		'<td bgcolor="#000000">' . 
		&playerLink($val->{id}) .
		'</td>' .
		&column($val->{frags}) .
		&column($val->{deaths}) .
		&column($val->{points}) .
		&column($val->{lines}) .
		&column($val->{blocks}) .
		&column(sprintf "%.0f", $val->{points}/$val->{playing_time}*100*60) .
		&column(sprintf "%.1f", $val->{blocks}/$val->{playing_time}*100*60) .
		'</tr>' .
		"\n";
}

sub blockRow {
	my $block = shift;

	my $ret = '<tr>';
	$ret .= '<td align=center bgcolor="#000000">';
	$ret .= &pic("$blockPics{$block}.gif");
	$ret .= '</td>';

	my $count = &sum("blocks_$block");
	my $rotations = &sum("times_rotated_$block");
	my $time = &sum("time_held_$block");
	if($count > 0) {
		$ret .= &column($count);
		$ret .= &column(sprintf "%.2f", $rotations/$count);
		$ret .= &column(sprintf "%.2f", $time/$count/100);
	}
	else {
		$ret .= &column();
		$ret .= &column();
		$ret .= &column();
	}
	$ret .= '</tr>';
	$ret .= "\n";
	return $ret;
}

sub column {
	my $text = shift;
	if(defined $text) {
		return '<td align=right bgcolor="#A0A0A0">' . $text . '</td>';
	}
	else {
		return '<td bgcolor="#A0A0A0">&nbsp;</td>';
	}
}

sub sum {
	my $item = shift;

	if($player eq "") {
		my $ret = 0;
		while(my ($key, $val) = each %$players) {
			my $value = $val->{$item};
			$ret += $value if defined $value;
		}
		return $ret;
	}
	else {
		return $players->{$player}->{$item};
	}
}

sub snapshot {
	my $params = shift;

	return '&nbsp;' unless defined $params;

	my $ret = &playerName($params->{id});
	$ret .= " ($params->{lines}";
	if($params->{clean} eq 'true') {
		$ret .= "-clean";
	}
	else {
		$ret .= " lines";
	}
	$ret .= ", $params->{combo} combo)<br>\n";
	my $snapshot = "e0e0e0e0e0e0e0e0e0e0$params->{snapshot}";
	my $pos = 0;
	my $x = 0;
	while($pos < length $snapshot) {
		my $block = substr $snapshot, $pos, 2;
		$ret .= &pic("$block.gif");

		$x++;
		if($x == 10) {
			$x = 0;
			$ret .= "<br>\n";
		}

		$pos += 2;
	}

	return $ret;
}

sub pic {
	my $pic = shift;
	return '<img src="html/' . $pic . '">';
}
