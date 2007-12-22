#! /usr/bin/perl -w
#
use strict;

my $input = shift or die "no input file";
my $output = shift or die "no output file";

open FILE, $input or die "open $input for read failed: $!\n";
my @file = <FILE>;
close FILE;

my $fileContent = join "", @file;

$fileContent =~ s[(\d+\.\d+\.\d+)\.(\d+)]["$1.".($2+1)]ei;

open OUTPUT, ">$output" or die "open $output for write failed: $!\n";
print OUTPUT $fileContent;
close OUTPUT;
