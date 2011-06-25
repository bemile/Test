#!/usr/bin/perl

use strict;
use warnings;
use Net::FTP;
use DBD::mysql;
  

my $ftp = Net::FTP->new( "archive.routeviews.org", Debug => 0 )
  or die "Cannot connect to archive.routeviews.org: $@";
  
$ftp->login( "anonymous", '-anonymous@' )
  or die "Cannot login ", $ftp->message;

print("Login successful.\n");
  
$ftp->cwd("/bgpdata/2010.01/RIBS/")
  or die "Cannot change working directory ", $ftp->message;

my @file_list = $ftp->ls();
my $num_files = @file_list;

$ftp->binary();

print("Download file $file_list[0]...\n");  
#$ftp->get("$file_list[0]")
#  or die "File download failed ", $ftp->message;
print("File downloaded!\n");

$ftp->quit;


##Unzip the file
print("Unzip the file...\n");
#system("7za920\\7za.exe x $file_list[0]");
print("File unzipped!\n");

print("Extract text info...\n");
my $file_name = $file_list[0];
$file_name =~ s/.bz2//;
#system("perl zebra-dump-parser.pl $file_name");
print("Text extracted!\n");


create_table($file_name);

print("Parsing RIB file...\n");
system("perl rib_parser.pl $file_name.out");
print("Parsing finished!\n");


exit;



################## Sub Routines #######################
sub create_table {
	my ($table_name) = @_;
	$table_name =~ s/\./_/g;
	print("Table Name: $table_name\n");
	my $dsn = 'dbi:mysql:route_views_rib:localhost:3306';
	# set the user and password 
	my $user = 'root'; 
	my $pass = 'jacklee1985'; 
	
	# now connect and get a database handle
	print("Connecting to the database...\n");  
	my $dbh = DBI->connect($dsn, $user, $pass) 
	 or die "Can’t connect to the DB: $DBI::errstr\n"; 
	 
	my $sth;
	my $query_string = "create table $table_name (ASN varchar(10), Upstreams varchar(4096), 
							NumUpstreams int, Prefixes text, NumPrefixes int, StubOrTransit varchar(10), PAOrPI varchar(5) )";
	$sth = $dbh->prepare($query_string);
	$sth->execute()
		or die "Can't create table\n";
		
	$dbh->disconnect();
}



################## Database Operation ###################
my $dsn = 'dbi:mysql:jie_test:localhost:3306';
# set the user and password 
my $user = 'root'; 
my $pass = 'jacklee1985'; 
 
# now connect and get a database handle
print("Connecting to the database...\n");  
my $dbh = DBI->connect($dsn, $user, $pass) 
 or die "Can’t connect to the DB: $DBI::errstr\n"; 
 
my $sth;
#$sth = $dbh->prepare('create table test (FirstName varchar(30), LastName varchar(30), Age int)');
#$sth->execute()
#	or die "Can't create table\n";
 
$sth = $dbh->prepare('insert into Test(FirstName, LastName, Age) values ("Jie", "Li", "26")');
$sth->execute()
 or die "Can't execute the clause\n";
 

$sth = $dbh->prepare("select FirstName, LastName from Test"); 
$sth->execute(); 
 
my @row;
while(@row = $sth->fetchrow_array()) { 
 print "$row[0]: $row[1]\n"; 
} 
