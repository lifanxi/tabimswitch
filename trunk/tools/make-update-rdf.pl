#! perl -w
# script to make update menifest.
#

use strict;

my $version;
my $xpi_file;
my $update_rdf_src_file;
my $output_rdf_file;

for (@ARGV) {
  if (/^--version=(\d+\.\d+\.\d+\.\d+)$/si) { $version = $1; }
  elsif (/^--xpi=(.+)$/si) { $xpi_file = $1; }
  elsif (/^--output=(.+)$/si) { $output_rdf_file = $1; }
  else {
    die "Already specified source file $update_rdf_src_file.\n" if $update_rdf_src_file;
    $update_rdf_src_file = $_;
  }
}

die "No source file specified.\n" unless $update_rdf_src_file;

unless ($version) {
  $version = `cat buildversion` or die "No version file buildversion, please create one or use --version=version.\n" 
}

$xpi_file = "output/tabimswitch-$version.xpi" unless $xpi_file;  
die "$xpi_file does not exist.\n" unless -e $xpi_file;

$output_rdf_file="update/update.rdf" unless $output_rdf_file;

my $openssl_hash=`openssl dgst -sha1 $xpi_file` or die "Unable to cacluate hash of $xpi_file.\n";
my @hash_parts = split(/\s/, $openssl_hash);
my $hash = $hash_parts[1];

open SRC, $update_rdf_src_file or die "open $update_rdf_src_file for read failed: $!\n";
my @source = <SRC>;
close SRC;

my $source = join "", @source;

$source =~ s[\$\(VERSION\)][$version]eg;
$source =~ s[\$\(HASH\)][$hash]eg;

open OUTPUT, ">$output_rdf_file" or die "open $output_rdf_file for write failed: $!\n";
print OUTPUT $source;
close OUTPUT;

print "Please sign the $output_rdf_file with your key...\n";
print "File path: $output_rdf_file\n";

system 'D:/Tools/mccoy/mccoy.exe';

exit 0;
