#------------------------------------------------------------------------------
# File Name: rib_parser.pl
# Date: April 16, 2011
# Author: Jie Li
# Email: jieli@virginia.edu
#
# Info: 
# The program takes an ascii RIB file as input and parse out the following
# information:
#    1. The list of all AS numbers present in the RIB file messages
#    2. The list of Stub ASes, each with its prefixes, upstream AS list
#    3. The list of Transit ASes, each with its prefixes and 
#       upstream/downstream AS lists
#    4. A complete list of Prefix-to-AS mappings
#
# The above 4 report files will be output as 4 separate csv files (under the 
# "/reports" folder)
#------------------------------------------------------------------------------

#!/usr/bin/perl

use strict;
use warnings;

#Hash tables for AS information
my %all_as;
my %transit_as;
my %stub_as;
my %all_prefix;
my %upstream_list;
my %downstream_list;

#The directory name for generated report files
my $report_dir = "reports";

#Check arguments
my $input_file;
if ($#ARGV != 0) {
	usage();
}
else {
	$input_file = shift;
}

#Read input file
open(INPUT, "$input_file") or die("Cannot open file $input_file: $!\n");

my $line;
my $count = 0;
while ($line = <INPUT>) {
	#if ($count > 100) {
	#	last;
	#}
	
	if (index($line, "PREFIX:") ne -1) {
		$line =~ /.*PREFIX:\s+(\d+\.\d+\.\d+\.\d+\/\d+)/;
		my $prefix = $1;
		while (index($line = <INPUT>, "AS_PATH:") eq -1) {
		}
		
		chomp($line);
		$line =~ /.*AS_PATH:\s+(.*)/;
		my $as_path = $1;
		my @as_list = split(/ /, $as_path);
		my $last_index = $#as_list;
		my $i = 0;
		while ($i <= $last_index) {
			my $asn = $as_list[$i];
			if ($i < $last_index) {
				add_as($as_list[$i], "");
			}
			else {
				add_as($as_list[$i], $prefix);
			}
			
			if ($i > 0 && $asn ne $as_list[$i - 1]) {
				add_upstream($asn, $as_list[$i - 1]);
			}
			
			if ($i < $last_index) {
				if ($as_list[$i] ne $as_list[$i + 1]) {
					add_trans_as($asn);
					add_downstream($asn, $as_list[$i + 1]);
				}
			}
			
			$i++;
		}
		#add_as($as_list[$i], $prefix);
		add_prefix($prefix, $as_list[$last_index]);

		$count++;
		if ($count % 100000 == 0) {
			print "$count entries have peen processed.\n";
		}
	}
}

close INPUT;
mkdir $report_dir;
chdir $report_dir;
print_all_as();
print_stub_as();
print_transit_as();
print_prefix_to_as();
chdir "..";

## END OF MAIN PROGRAM ##




##################################    SUB ROUTINES    ##################################

# Add a transit AS number to the unique transit AS hash table
sub add_trans_as {
	my ($num) = @_;
	if ($transit_as{$num}) {
		$transit_as{$num}++;
	}
	else {
		$transit_as{$num} = 1;
	}
}

# Add a AS-to-prefix entry to the AS hash table
sub add_as {
	my ($num, $prefix) = @_;
	if ($all_as{$num}) {
		if ($prefix) {
			$all_as{$num} .= ($prefix.";");
		}
	}
	else {
		if ($prefix) {
			$all_as{$num} = ($prefix.";");
		}
		else {
			$all_as{$num} = "";
		}
	}
}

#Add an Prefix-to-AS entry
sub add_prefix {
	my ($prefix, $num) = @_;
	if (!$all_prefix{$prefix}) {
		$all_prefix{$prefix} = $num;
	}
	else {
	}
}

#Add an upstream AS
sub add_upstream {
	my ($asn, $upstream) = @_;
	if (!$upstream_list{$asn}) {
		$upstream_list{$asn} = ($upstream."/");
	}
	else {
		$upstream_list{$asn} .= ($upstream."/");
	}
}

#Add an downstream AS
sub add_downstream {
	my ($asn, $downstream) = @_;
	if (!$downstream_list{$asn}) {
		$downstream_list{$asn} = ($downstream."/");
	}
	else {
		$downstream_list{$asn} .= ($downstream."/");
	}
}



#Generate a report file about all AS entities
sub print_all_as {
	open(OUTPUT, ">all_as") or die ("Cannot open file to write: $!\n");
	foreach my $key (keys(%all_as)) {
		my $prefixes = $all_as{$key};
		my $num = count_unique_prefixes($prefixes);
		print OUTPUT "$key\t\t$num\n";
	}
	close OUTPUT;
}

#Generate a report file about transit AS entities
sub print_transit_as {
	open(OUTPUT, ">transit_as.csv") or die ("Cannot open file to write: $!\n");
	print OUTPUT "#ASN,Prefixes,UpstreamList,DownstreamList\n";
	foreach my $key (keys(%transit_as)) {
		my $prefixes = get_unique_identifiers($all_as{$key}, ";");
		my $upstreams = get_unique_identifiers($upstream_list{$key}, "/");
		my $downstreams = get_unique_identifiers($downstream_list{$key}, "/");
		print OUTPUT "$key,$prefixes,$upstreams,$downstreams\n";
	}
	close OUTPUT;
}

#Generate a report file about stub AS entities
sub print_stub_as {
	open(OUTPUT, ">stub_as.csv") or die ("Cannot open file to write: $!\n");
	print OUTPUT "#ASN,Prefixes,UpstreamList\n";
	foreach my $key (keys(%all_as)) {
		if (!$transit_as{$key}) { #This is a stub AS
			my $prefixes = get_unique_identifiers($all_as{$key}, ";");
			my $upstreams = get_unique_identifiers($upstream_list{$key}, "/");
			print OUTPUT "$key,$prefixes,$upstreams\n";
		}
	}
	close OUTPUT;
}

#Generate a report file about Prefix-to-AS mappings
sub print_prefix_to_as {
	open(OUTPUT, ">prefix_to_as.csv") or die ("Cannot open file to write: $!\n");
	print OUTPUT "#Prefix Number,ASN,AS Type\n";
	foreach my $key (keys(%all_prefix)) {
			my $as_num = $all_prefix{$key};
			my $as_type;
			if ($transit_as{$as_num}) {
				$as_type = "Transit";
			}
			else {
				$as_type = "Stub";
			}
			print OUTPUT "$key,$as_num,$as_type\n";
	}
	close OUTPUT;
}

#Get the number of unique prefixes in a prefix string (separated by ';')
sub count_unique_prefixes {
	my ($prefixes) = @_;
	if (!$prefixes) {
		return 0;
	}
	else {
		my %uniq_ent;
		my $count = 0;
		my @ent = split(/;/, $prefixes);
		foreach (@ent) {
			if ($_ && !$uniq_ent{$_}) {
				$uniq_ent{$_} = 1;
				$count++;
			}
		}
		return $count;
	}
}

#Get the string of unique identifiers
sub get_unique_identifiers {
	my ($string, $separator) = @_;
	if (!$string) {
		return "";
	} else {
		my %uniq_ent;
		my $count = 0;
		my @ent = split(/$separator/, $string);
		foreach (@ent) {
			if ($_ && !$uniq_ent{$_}) {
				$uniq_ent{$_} = 1;
			}
		}
		
		my $res = "";
		foreach my $key (keys(%uniq_ent)) {
			$res .= ($key.$separator);
		}
		
		return $res;
	}		
}


#Usage info
sub usage {
	die("Usage: perl rib_parser.pl input-file-name\n");
}

## END OF SUB ROUTINES ##