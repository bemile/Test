#!/usr/bin/perl

use strict;
use warnings;
use POSIX;

my $prefix_string = "";

#Check arguments
my $all_as_file;
my $peer_file;
if ($#ARGV != 1) {
	usage();
}
else {
	$all_as_file = shift;
	$peer_file = shift;
}


open(PEER, "$peer_file") or die "Cannot open input file to read: $!\n";
my $line;
while ($line = <PEER>) {
	$line =~ /(\S+)\s+(\S+)\s+(\S+)\s+(\S+)/;
	generate_report($1, $4);
}
close PEER;


#
sub generate_report {
	my ($as_name, $as_num) = @_; 
	print "Generating report for: $as_name\n";
	open(ALLAS, "$all_as_file") or die "Cannot open input file to read: $!\n";
	my $line;
	LOOP: while ($line = <ALLAS>) {
		my @pieces = split(/\t\t/, $line);
		if ($pieces[0] eq $as_num) {
			$prefix_string = $pieces[1];
			last LOOP;
		}
	}
	close ALLAS;


	my @prefixes = split(/;/, $prefix_string);
	my %prefix_hash;
	foreach (@prefixes) {
		if ($prefix_hash{$_}) {
			$prefix_hash{$_}++;
		}
		else {
			$prefix_hash{$_} = 1;
		}
	}

	my $file_name = "transit_as_prefixes\\${as_name}_AS${as_num}_prefixes.csv";
	open(OUTPUT, ">$file_name") or die "Cannot open file to write: $!";
	print OUTPUT "#Prefix, #IP Addresses\n";

	for my $key (sort keys %prefix_hash) {
		if ($key =~ /.*\/.*/) {
			my @parts = split(/\//, $key);
			my $mask = $parts[1];
			my $ip_num = POSIX::pow(2, 32 - $mask);
			print OUTPUT "$key,$ip_num\n";
		}
	}
	close OUTPUT;
	
	print "Report for $as_name generated.\n";
}



#Usage info
sub usage {
	die("Usage: perl all-as-file peer-file\n");
}