package RecReader;
require 5.004;

use strict;
use Compress::Zlib;

sub new {
	my $class = shift;
	my $filename = shift;
	my $eventCB = shift;

	(open FILE, $filename) || return;
	binmode FILE; local $/;

	# skip size
	(read FILE, $_, 4) || return;

	# read rest and uncompress
	(my $hunks = uncompress <FILE>) || return;
	close FILE;

	my $self = {};
	bless $self, ref $class || $class;

	$self->{raw_summary} = &_parse($hunks, $eventCB);

	return $self;
}

sub DESTROY {
}

sub summary {
	my $self = shift;

	$self->{summary} = &_parseSummary($self->{raw_summary}) unless exists $self->{summary};
	return $self->{summary};
}

sub raw_summary {
	my $self = shift;
	
	return $self->{raw_summary};
}

sub _parse {
	my $hunks = shift;
	my $cb = shift;

	my $summary;
	my $pos = 0;
	while(length $hunks > $pos) {
		my $hunktype = unpack "C", substr $hunks, $pos, 1;

		if($hunktype == 3) {
			my ($name, $score, $lines, $level) = unpack "Z40VVV", substr $hunks, $pos+1, 52;
			$pos += 53;
		}
		elsif($hunktype == 11) {
			my ($frame, $len) = unpack "Vv", substr $hunks, $pos+1, 6;
			my $packet = substr $hunks, $pos+7, $len;
			my $packetid = unpack "C", $packet;
			if($packetid == 0) {
				my ($team, $text) = unpack "xCZ256", $packet;
			}
			elsif($packetid == 49) {
				my $event = unpack "xZ64", $packet;
				my ($paramcount, $rest) = unpack "Na*", substr $packet, (2+length $event);
				my %params;
				my $pos = 0;
				while($paramcount) {
					(substr $rest, $pos) =~ /([^\0]{0,128})\0([^\0]{0,1024})\0/;
					$params{$1} = $2;
					$paramcount--;
					$pos += length $1;
					$pos += length $2;
					$pos += 2;
				}
				&{$cb}($frame, $event, \%params) if defined $cb;
			}
			$pos += (7 + $len);
		}
		elsif($hunktype == 13) {
			my $len = unpack "V", substr $hunks, $pos+1, 4;
			$summary = substr $hunks, $pos+5, $len;
			$pos += (5 + $len);
		}
		else {
			last;
		}
	}
	return $summary;
}

sub _parseSummary {
	my $raw_summary = shift;
	my $ret = {};

	my @lines = split /\n/, $raw_summary;
	foreach(@lines) {
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

1;
