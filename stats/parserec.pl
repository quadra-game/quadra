#!perl -w

use strict;
use RecReader;

my $game = shift;
my $reader = new RecReader("$game", \&processEvent);
print $reader->raw_summary if defined $reader;

sub processEvent {
	my $frame = shift;
	my $event = shift;
	my $params = shift;
	printf "%08x $event ", $frame;
	print join " ", %$params;
	print "\n";
}
