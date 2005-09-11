#!/usr/bin/perl

if ($#ARGV != 0) {
  die "Specifiy the name of the file to clean (without filesuffix)!"
}

$BASENAME=$ARGV[0];
$HPPNAME=$BASENAME . ".h";
$CPPNAME=$BASENAME . ".cpp";
$OBJNAME=$BASENAME . ".lo";

# backup the cpp file
system "cp $CPPNAME $CPPNAME-ORIGINAL";

# read the whole cpp file
open(INFILE,  "$CPPNAME") or die "Can't open $CPPNAME: $!";
@FILECONTENT=<INFILE>;
close INFILE;


my %foundIncludes = ();
my $firstInclude = 1;

my $lineNr = 0;
foreach (@FILECONTENT) {

  # this is not an include line --> ignore
  if ( ! m/^\s*#include\ ["<].*[">]/ ) {
    $lineNr++;
    next;
  }

  # never remove the first include in a file
  # the first include is the header file for this cpp file!
  if ($firstInclude == 1) {
    $firstInclude = 0;
    $lineNr++;
    next;
  }

  # test if this is a duplicate include... --> comment it
  if ($foundIncludes{"$_"} ne "") {
    print "duplicate include found: $_ @ line $lineNr\n";
    $FILECONTENT[$lineNr] = "//" . $FILECONTENT[$lineNr];
    $lineNr++;
    next;
  }

  # remember all seen includes
  $foundIncludes{"$_"} = $lineNr;

  $lineNr++;
}


foreach (sort {$a <=> $b} values %foundIncludes) {
  $includesLineNr = $_;

  # comment the include directive on the current line
  $FILECONTENT[$includesLineNr] = "//" . $FILECONTENT[$includesLineNr];
  
  # write the modified file
  open(OUTFILE, ">$CPPNAME") or die "Can't open $CPPNAME: $!";
  foreach (@FILECONTENT) {
    print OUTFILE;
  }
  close OUTFILE;

  # remove compiled object to force recompilation!
  system "rm $OBJNAME";

  # try to compile
  system "gmake $OBJNAME";

  # if there was an error: uncomment the commented include directive
  if ($? != 0) {
    $FILECONTENT[$includesLineNr] =~ m/^\s*\/\/(.*)/;
    $FILECONTENT[$includesLineNr] = "$1\n";
  }
}


# write down the resulting file
open(OUTFILE, ">$CPPNAME") or die "Can't open $CPPNAME: $!";
print OUTFILE <<"END";
@FILECONTENT
END
close OUTFILE;
