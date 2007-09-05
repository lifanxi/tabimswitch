#! perl -w

use strict;

sub getVersion() {
  open FILE, "install.rdf" or die "no install.rdf: $!\n";
  my @file = <FILE>;
  close FILE;

  my $fileContent = join "", @file;

  my $version;
  $fileContent =~ s[<em:version>(\d+\.\d+\.\d+\.\d+)</em:version>][$version=$1]ei;
  
  die "unable to get version number." unless $version;

  return $version;
}

my @oldFileNames = (
  'tabimswitch',
  'tabimswitch-mozilla'
);

for my $oldFile (@oldFileNames) {
  if (-e "$oldFile.xpi") {
    print "rename $oldFile.xpi...\n";
    my $version = getVersion();
    rename("$oldFile.xpi", "$oldFile-$version.xpi") or die "Unable to rename files $oldFile.xpi: $!\n";
  }
}
