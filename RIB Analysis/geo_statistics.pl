#!/usr/bin/perl

use strict;
use warnings;

#variables used in geo statistics
my %country_hash;
my @country_entries = ();
my $current_country = "N/A";

#Check arguments
my $geo_file;
if ($#ARGV != 0) {
	usage();
}
else {
	$geo_file = shift;
}

#Read the input geo-ip file
open(INPUT, "$geo_file") or die("Cannot open file $geo_file: $!\n");
#Open output file to write the GeoLocation information
open(OUTPUT, ">final_statistics.csv") or die ("Cannot open file to write: $!\n");
print OUTPUT "Country Code,Country Name,#AS,#Transit AS,#Stub AS,#Total Prefixes,#Transit AS Prefixes,#Stub AS Prefixes\n";

my $line;
my $count = 0;
#Eliminate the header line of the .csv file
$line = <INPUT>;
while ($line = <INPUT>) {
	$count++;
	if ($count % 10000 == 0) {
		print "$count records have been processed.\n";
	}
	
	chomp($line);
	my @record = split(/,/, $line);
	
	if ($country_hash{$record[0]}) {
		push(@{$country_hash{$record[0]}}, \@record);
	}
	else {
		my @array = (\@record);
		$country_hash{$record[0]} = \@array; 
	}
}

foreach my $key (sort keys(%country_hash)) {
	my $new_row = generate_country_info(@{$country_hash{$key}});
	print OUTPUT "$new_row";
}

close INPUT;
close OUTPUT;


sub process_record {
	my @record;
	foreach	(@_) {
		push(@record, $_);
	}
	push(@country_entries, \@record);
}

sub generate_country_info {
	my @country_entries = @_;
	if ($#country_entries < 0) {
		return "";
	}
	else {
		my $country_code = $country_entries[0][0];
		my $country_name = $country_entries[0][1];
		#Statistics numbers
		my $as_num = 0;
		my $transit_as_num = 0;
		my $stub_as_num = 0;
		my $prefix_num = 0;
		my $transit_prefix_num = 0;
		my $stub_prefix_num = 0;
		
		my %unique_as;
		my $i = 0;
		my $last_index = $#country_entries;
		while ($i <= $last_index) {
			$prefix_num++;
			if ($unique_as{$country_entries[$i][2]}) {			
				if ($country_entries[$i][3] eq "Transit") {
					$transit_prefix_num++;
				}
				else {
					$stub_prefix_num++;
				}
			}
			else {
				$unique_as{$country_entries[$i][2]} = 1;
				$as_num++;
				if ($country_entries[$i][3] eq "Transit") {
					$transit_as_num++;
					$transit_prefix_num++;
				}
				else {
					$stub_as_num++;
					$stub_prefix_num++;
				}
			}
			
			$i++;
		}
		
		@country_entries = ();
		return "$country_code,$country_name,$as_num,$transit_as_num,$stub_as_num,$prefix_num,$transit_prefix_num,$stub_prefix_num\n";
	}	
}


#Usage info
sub usage {
	die("Usage: perl geo_statistics.pl geo-ip-file\n");
}