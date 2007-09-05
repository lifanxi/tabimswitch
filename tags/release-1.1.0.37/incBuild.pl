#! /usr/bin/perl -w
#
use strict;

open FILE, "install.rdf" or die "no install.rdf: $!\n";
my @file = <FILE>;
close FILE;

my $fileContent = join "", @file;

$fileContent =~ s[<em:version>(\d+\.\d+\.\d+)\.(\d+)</em:version>]["<em:version>$1.".($2+1)."</em:version>"]ei;

open OUTPUT, ">install.rdf" or die "cannot open install.rdf: $!\n";
print OUTPUT $fileContent;
close OUTPUT;
