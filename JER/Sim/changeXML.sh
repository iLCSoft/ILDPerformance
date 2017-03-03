#!/usr/local/bin/perl
#
# change all occurances of a string in a file to another string
#

#print "Start\n";

$oldfile1 = $ARGV[0];
$newfile1 = $ARGV[1];
$oldfile2 = $ARGV[2];
$newfile2 = $ARGV[3];

$particle = $ARGV[4];
$energy = $ARGV[5];
$nevents = $ARGV[6];
$randomSeed = $ARGV[7];

$ilcsoft = $ARGV[8];
$detector = $ARGV[9];

#print "$oldfile\n";
#print "$par\n";
#print "$size\n";
#print "$nevents2skip\n";

$cmd1 = "cp $oldfile1 $newfile1";
system($cmd1);

$cmd2 = "cp $oldfile2 $newfile2";
system($cmd2);

open(OF1, $oldfile1);
open(NF1, ">$newfile1");
#read file
while ($line= <OF1>){
    #print "Reading file...\n";
    #print $line;

    $line =~ s/PARTICLE/${particle}/g;
    $line =~ s/ENERGY/${energy}/g;
    $line =~ s/NEVENTS/${nevents}/g;
    $line =~ s/RANDOMSEED/${randomSeed}/g;
    $line =~ s/ILCSOFT/${ilcsoft}/g;
    $line =~ s/DETECTOR/${detector}/g;

    print NF1 $line;
}
close(OF1);
close(NF1);

open(OF2, $oldfile2);
open(NF2, ">$newfile2");
#read file
while ($line= <OF2>){
    #print "Reading file...\n";
    #print $line;

    $line =~ s/PARTICLE/${particle}/g;
    $line =~ s/ENERGY/${energy}/g;
    $line =~ s/NEVENTS/${nevents}/g;
    $line =~ s/RANDOMSEED/${randomSeed}/g;
    $line =~ s/ILCSOFT/${ilcsoft}/g;
    $line =~ s/DETECTOR/${detector}/g;

    print NF2 $line;
}
close(OF2);
close(NF2);

