$infile=$ARGV[0];

open(IN, $infile);
@line=<IN>;
close(IN);

open(OUT, "> $infile");

foreach $tok (@line){
    chomp $tok;
#    $tok =~ s/^\s+//;
#    $tok =~ s/\s+$//;
#    $tok =~ s/\s+/,/g;
    $tok =~ s/,$//;
    print OUT "$tok\n";
}
