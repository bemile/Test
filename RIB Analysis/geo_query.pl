#!/usr/bin/perl

use strict;
use warnings;


#Array of IP-to-Country entries
my @ip_country;
my $ip_entry_count = 0;

#Hash table of Prefix-to-AS entries
my %prefix_to_as;

# Variables used in formatted output
my $country_code;
my $country_name;
my $as_num;
my $as_type;
my $prefix;
  

#IP-to-Country database file path
my $db_file;
#Prefix-to-AS file path	
my $prefix_file;

#Check arguments
if ($#ARGV != 1) {
	usage();
}
else {
	$db_file = shift;
	$prefix_file = shift;
}

init_ip_array();


#Read input file
open(INPUT, "$prefix_file") or die("Cannot open file $prefix_file: $!\n");
#Open output file to write the GeoLocation information
open(OUTPUT, ">prefix_geoinfo.csv") or die ("Cannot open file to write: $!\n");
print OUTPUT "Country Code,Country Name,ASN,AS Type,Prefix\n";

my $line;
my $count = 0;
#Eliminate the header line of the csv file
$line = <INPUT>;
while ($line = <INPUT>) {
	$count++;
	if ($count % 10000 == 0) {
		print "$count prefixes have been processed.\n";
	}
	
	chomp($line);	
	my @pieces = split(/,/, $line);
	$prefix = $pieces[0];
	$as_num = $pieces[1];
	$as_type = $pieces[2];
	my @addr_parts = split(/\//, $prefix);
	my $ip = $addr_parts[0];
	if ($ip) {
		my @res = find_country($ip);
		if (@res) {
			$country_code = $res[0];
			$country_name = $res[1];
		}
		else {
			$country_code = "N/A";
			$country_name = "N/A";
		}
		#write(OUTPUT);
		print OUTPUT "${country_code},${country_name},${as_num},${as_type},${prefix}\n";
	}
}

close INPUT;
close OUTPUT;


format OUTPUT = 
@<<<<<<<<<<    @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    @<<<<<<<<<<    @<<<<<<<<    @<<<<<<<<<<<<<<<<<<<<
$country_code, $country_name, $as_num, $as_type, $prefix
.

#Find the country location for given a prefix string
sub find_country {
	my ($ip) = @_;
	my @parts = split(/\./, $ip);
	my $num = $parts[0] * 16777216 + $parts[1] * 65536 + $parts[2] * 256 + $parts[3];
	#print "Looking up: $num\n";
	return find_country_from_number($num, 0, $ip_entry_count - 1);
}

#Find the country code from the 32-bit integer ip number using binary search
sub find_country_from_number {
	my ($num, $start, $end) = @_;
	if ($start == $end) {
		if (($num >= $ip_country[$start][0]) && ($num <= $ip_country[$start][1])) {
			return ($ip_country[$start][4], $ip_country[$start][6]);
		}
		else {
			return my @temp;
		}
	}
	
	my $mid_num = ($start + $end) / 2;
	if (($num >= $ip_country[$mid_num][0]) && ($num <= $ip_country[$mid_num][1])) {
		my @res = ($ip_country[$mid_num][4], $ip_country[$mid_num][6]);
		return @res;
	}
	elsif ($num < $ip_country[$mid_num][0]) {
		return find_country_from_number($num, $start, $mid_num);
	}
	else {
		return find_country_from_number($num, $mid_num + 1, $end);
	}
}



sub init_ip_array {
	#Read the ip-to-country database file
	open(IPCOUNTRY, "$db_file") or die("Cannot open the ip-to-country database file: $!\n");
	my $line;
	print "Constructing IP-to-Country array...\n";
	while ($line = <IPCOUNTRY>) {
		if ($line =~ /^#/) {
			print $line;
			next;
		}
			
		$ip_entry_count++;
		chomp($line);
		my @record = split(/,/, $line);
		push(@ip_country, \@record);
	}
	close IPCOUNTRY;
	print "Totally $ip_entry_count ip range entries in the array.\n";
}


#Usage info
sub usage {
	die("Usage: perl geo_query.pl ip-to-country-file prefix-to-as-file\n");
}