#! /usr/bin/perl -w

use strict;
use RecReader;

my $reader = new RecReader("/projects/quadra/stats/games/bob.rec", \&processEvent);
print $reader->raw_summary if defined $reader;

sub processEvent {
	my $frame = shift;
	my $event = shift;
	my $params = shift;
	printf "%08x $event ", $frame;
	print join " ", %$params;
	print "\n";
}
