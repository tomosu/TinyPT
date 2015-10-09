$infile=$ARGV[0];

open(IN, $infile);
@line=<IN>;
close(IN);

# open(OUT, "> $infile");
$i=1;
foreach $tok (@line){
    chomp $tok;
    @val = split(/,/, $tok); 
    #$tok =~ s/^\s+//;
    #$tok =~ s/\s+$//;
    #$tok =~ s/\s+/,/g;
    #print OUT "$tok\n";
    if($i%2 == 1){
	print "$val[0]\n";
    }
    if($i%2 == 0){
	@energy =split(/\(/, $val[0]);
	print "$energy[0]\n";
    }
    $i++;
}
