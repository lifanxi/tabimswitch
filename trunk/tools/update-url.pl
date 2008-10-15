#! /usr/bin/perl -w
#
use strict;

my $usage="update_url.pl [remove|restore]";
my $command = shift or die $usage;

if ( $command eq 'remove' ) {
  open FILE, "install.rdf" or die "no install.rdf: $!\n";
  my @file = <FILE>;
  close FILE;

  my $fileContent = join "", @file;

  open BAK, ">install.rdf.bak" or die "cannot backup install.rdf:$!\n";
  print BAK $fileContent;
  close BAK;

  $fileContent =~ s[<em:updateURL>.+</em:updateURL>][]i;

  open OUTPUT, ">install.rdf" or die "cannot open install.rdf: $!\n";
  print OUTPUT $fileContent;
  close OUTPUT;
}
elsif ( $command eq 'restore' ) {
  unlink("install.rdf");
  rename("install.rdf.bak", "install.rdf") or die ("Cannot restore install.rdf.bak to install.rdf: $!\n");
}
else {
  die $usage;
}

exit 1;
