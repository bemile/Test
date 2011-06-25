#!/usr/bin/perl

use strict;
use warnings;
use DBD::mysql;

#Hash tables for AS information
my %transit_as;
my %all_as;
my %all_prefix;
my %as_upstreams;

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
	#if ($count > 1000) {
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
		while ($i < $last_index) {
			add_trans_as($as_list[$i]);
			add_as($as_list[$i], "");
			$i++;
		}
		add_as($as_list[$i], $prefix);
		add_prefix($prefix, $as_list[$i]);
		if ($last_index > 0) {
			add_upstream($as_list[$last_index], $as_list[$last_index-1]);
		}
		

		$count++;
		if ($count % 100000 == 0) {
			print "$count entries have peen processed.\n";
		}
	}
}

close INPUT;
build_table();

#mkdir $report_dir;
#chdir $report_dir;
#print_all_as();
#print_stub_as();
#print_transit_as();
#print_prefix_to_as();
#chdir "..";



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
			$all_as{$num}->{$prefix} = 1;
		}
	}
	else {
		my %temp_prefixes;
		if ($prefix) {
			$temp_prefixes{$prefix} = 1;
		}
		
		$all_as{$num} = \%temp_prefixes;
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


#Add an upstream AS for a target AS
sub add_upstream {
	my ($as, $up_as) = @_;
	if (!$as_upstreams{$as}) {
		my %upstreams;
		$upstreams{$up_as} = 1;
		$as_upstreams{$as} = \%upstreams;
	}
	else {
		$as_upstreams{$as}->{$up_as} = 1;
	}
}




# Build the table
sub build_table {
	my $dsn = 'dbi:mysql:route_views_rib:localhost:3306';
	# set the user and password 
	my $user = 'root'; 
	my $pass = 'jacklee1985'; 
	 
	# now connect and get a database handle
	print("Connecting to the database...\n");  
	my $dbh = DBI->connect($dsn, $user, $pass) 
	 or die "Can’t connect to the DB: $DBI::errstr\n"; 
	
	my $table_name = $input_file;
	$table_name =~ s/.out//;
	$table_name =~ s/\./_/g; 
	my $sth;
	
	foreach my $key (keys(%all_as)) {
		my $asn = $key;
		my %upstreams;
		my $up_string = "";
		my $num_up = 0;
		if ($as_upstreams{$key}) {
			%upstreams = %{$as_upstreams{$key}};
			if (%upstreams) {
				foreach my $key (keys(%upstreams)) {
					$up_string .= "$key;";
					$num_up++;
				}
			}
		}
		
		my %prefixes;
		my $prefix_string = "";
		my $num_prefix = 0;
		if ($all_as{$key}) {
			%prefixes = %{$all_as{$key}};
			if (%prefixes) {
				foreach my $key (keys(%prefixes)) {
					$prefix_string .= "$key;";
					$num_prefix++;
				}
			}
		}
		
		my $stub_transit;
		if ($transit_as{$key}) {
			$stub_transit = "Transit";
		}
		else {
			$stub_transit = "Stub";
		}
		
		$sth = $dbh->prepare("insert into $table_name(ASN, Upstreams, NumUpstreams, Prefixes, NumPrefixes, StubOrTransit) 
								values (\"$asn\", \"$up_string\", \"$num_up\", \"$prefix_string\", \"$num_prefix\",
								\"$stub_transit\")");
		$sth->execute()
		 or die "Can't execute the clause\n";
	}
	
	$dbh->disconnect();
}






#Generate a report file about all AS entities
sub print_all_as {
	open(OUTPUT, ">all_as") or die ("Cannot open file to write: $!\n");
	foreach my $key (keys(%all_as)) {
		print OUTPUT "$key\t\t$all_as{$key}\n";
	}
	close OUTPUT;
}

#Generate a report file about transit AS entities
sub print_transit_as {
	open(OUTPUT, ">transit_as.csv") or die ("Cannot open file to write: $!\n");
	print OUTPUT "#ASN,Prefix Number\n";
	foreach my $key (keys(%transit_as)) {
		my $prefixes = $all_as{$key};
		my $num = count_unique_prefixes($prefixes);
		print OUTPUT "$key,$num\n";
	}
	close OUTPUT;
}

#Generate a report file about stub AS entities
sub print_stub_as {
	open(OUTPUT, ">stub_as.csv") or die ("Cannot open file to write: $!\n");
	print OUTPUT "#ASN,Prefix Number\n";
	foreach my $key (keys(%all_as)) {
		if (!$transit_as{$key}) { #This is a stub AS
			my $prefixes = $all_as{$key};
			my $num = count_unique_prefixes($prefixes);
			print OUTPUT "$key,$num\n";
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

#Usage info
sub usage {
	die("Usage: perl rib_parser.pl input-file-name\n");
}

## END OF SUB ROUTINES ##