#!perl -w

use strict;
use CGI;
use RecReader;

my $cgi = new CGI;

my $dir = "games/";
opendir DIR, $dir || &error("Can't read directory.");
my @files = readdir DIR;
closedir DIR;

print $cgi->header('text/html');
print $cgi->start_html(-title => "Games");
for my $file (@files) {
	if($file =~ /^(.*)\.rec$/) {
		my $reader = new RecReader("$dir$file");
		next unless defined $reader;
		my $summary = $reader->summary();
		print "<a href=\"snapshot.pl?game=$1\">$summary->{name}</a> ($1)<br>\n";
	}
}
print $cgi->end_html();

sub error {
	my $error = shift;
	print $cgi->header('text/plain');
	print "$error\n";
	exit;
}
