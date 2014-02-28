#! /usr/bin/perl -Tw

use strict;
use MD5;

my($input, $hash, $i, $item);

chomp($input = <STDIN>);

$hash = MD5->hash($input);

for($i = 0; $i < 16; $i++) {
	$item = unpack("C", substr($hash, $i, 1));
	$item = (($item ^ 255) + (53 + $i * 7)) % 256;
	substr($hash, $i, 1) = pack("C", $item);
}

print unpack("H*", $hash);
