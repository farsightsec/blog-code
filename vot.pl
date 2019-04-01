#!/usr/bin/perl
use strict;
use warnings;

use Date::Manip;
use DateTime;
use File::Temp qw/ tempfile tempdir /;
use Getopt::Long qw(:config no_ignore_case);
use List::MoreUtils;
use List::Util qw(pairs);

my $bailiwick = ''; 
my $cmd = ''; 
my $cutoff = ''; 
my $daysback = '';
my $dt = '';
my $dt1 = '';
my $dur = '';
my $file = ''; 
my $filecount = '';
my $filedate = '';
my $myfiledate = '';
my $fqdn = ''; 
my $fullpattern = '';
my $granularity = 'D';
my $help = ''; 
my $jsonl = ''; 
my $key = '';
my $minutesseconds = '';
my $mtbldir = '/export/dnsdb/mtbl/';
my $noplotraw = '';
my $notable = ''; 
my $now_string = '';
my $now_string_mm_ss = '';
my $plot = ''; 
my $plotdev = 'postscript';
my $plotdir = '.';
my $plotout = ''; 
my $plotsmoothed = ''; 
my $plottype = 'line';
my $rectype = ''; 
my $roughcutoff = '';
my $roughstart = ''; 
my $roughstop = ''; 
my $sizeofresults = '';
my $smooth = ''; 
my $start = ''; 
my $start2 = ''; 
my $stop = ''; 
my $stop2 = '';
my $tabledir = '.'; 
my $tableextension ='txt'; 
my $tableout = ''; 
my $timefencecheck1 = '';
my $timefencecheck2 = '';
my $timefencecheck3 = '';
my $timefencecheck4 = '';
my $timefencecheck5 = '';
my $title = ''; 
my $totalresultsfound = '';
my $value = '';

my @keepermtblfiles;
my @mtblfiles;  
my @results = ''; 
my @unsortedfiles;

my %hash = (); 
my %smoothedvalues = ();

################### GET AND PROCESS THE ARGUMENTS ##################
 
GetOptions ('fqdn|name|rrset|rrname|r=s' => \$fqdn,
            'rectype|rrtype|t=s' => \$rectype,
            'bailiwick|b=s' => \$bailiwick,
            'granularity|unit|timeperiod|u=s' => \$granularity,
            'smooth|ma|m=s' => \$smooth,
            'notable|quiet|q' => \$notable,
            'tableout|output|outfile=s' => \$tableout,
            'jsonl|json|j' => \$jsonl,
            'tabledir|reportdir|directory|dir|d=s' => \$tabledir,
            'plot|graph|chart|p' => \$plot,
	    'plottype|graphtype|charttype=s' => \$plottype,
            'plotdev|device=s' => \$plotdev,
            'plotout=s' => \$plotout,
            'plotdir|graphdir=s' => \$plotdir,
            'title|plottitle=s' => \$title,
            'noplotraw' => \$noplotraw,
            'plotsmoothed|plotma' => \$plotsmoothed,
	    'start|first|begin=s' => \$start,
	    'stop|last|end=s' => \$stop,
	    'daysback|days|window=s' => \$daysback,
            'mtbldir|datadir|mtblfiles|mtbl' => \$mtbldir,
	    'help|info|man|manual|usage|h' => \$help,
           );
           # b d h j m p q r t u v

# help report
if (($fqdn eq '') || ($help eq 1)) 
{
   die "Usage:

       \$ $0 --fqdn FQDN [--rectype RECTYPE] [--bailiwick BAILIWICK] 
       [--granularity {Y|M|D|H}] [--smooth INTEGER]
       [--notable] [--tableout FILENAME] [--jsonl] [--tabledir DIR]]
       [--plot [--plottype {POINT|LINE|VBAR}]
         [--plotdev GNUPLOTDEVICE] [--plotout FILENAME] [--plotdir DIR] 
         [--title 'TITLE'] [--noplotraw] [--plotsmoothed]]
       [--start DATE] [--stop DATE] [--daysback INTEGER]
       [--mtbldir] [--help]

       fqdn: fully qualified domain name to be graphed (REQUIRED)
             aliases: name|rrset|rrname|r
       rectype: ONE rectype (a, aaaa, cname, etc.) (def: non-DNSSEC types)
             aliases: rrtype|t
       bailiwick: for www.abc.com, either abc.com or com (def: use both)
	     alias: b
       granularity: Y(ear), M(onth), D(ay), H(our) (def: D)
             aliases: unit|timeperiod|u
       smooth: moving average period (def: no smoothing done)
             aliases: ma|m
       notable: supress tabular output (def: tabular output)
             aliases: quiet|q
       tableout: file for tabular output (def: generated filename)
             aliases: output|outfile
       jsonl: produce table in json lines format (def: CSV)
             aliases: json|j
       tabledir: directory for tabular output files (def: current dir)
             aliases: reportdir|directory|dir|d

       plot: request plots (def: no plots output)
             aliases: graph|chart|p
       plottype: type of plot to make? lines, dots, steps, impulses (def: lines)
	     aliases: graphtype|charttype
       plotdev: gnuplot output format (def: postscript)
             aliases: device
       plotout: graphic output filename (def: generated filename )
       plotdir: directory for plot output files (def: current dir)
	     aliases: graphdir
       title: plot title (def: command line string options)
             aliases: plottitle
       noplotraw: no plotting of raw data (def: display raw data)
       plotsmoothed: plot smoothed data (def: omit smoothed data)
             aliases: plotma

       start: show data starting from this date forward (def: all dates)
             aliases: first|begin
       stop: display no data after this date (def: all dates)
	     aliases: last|end
       daysback: only display data for the last N days (def: all dates)
             aliases: days|window

       mtbldir: location of MTBL files to be used (def: /export/dnsdb/mtbl/)
	     aliases: datadir|mtblfiles|mtbl

       help: show this then exit
             aliases: info|man|manual|usage|h

       Examples: \$ $0 --fqdn \"www.reed.edu\"
                 \$ $0 --fqdn \"powells.com\" --smooth 3 --notable\\
                    --plot --graphformat jpg --graphdir mygraphs\\
                    --start 20180101 --stop 20180630\n\n";
}

if ($fqdn eq '') { die "Specify a fully qualified domain name with --fqdn";}

# need current time to construct default filenames for output if not specified
# and for relative time ("daysback") options

# get today's date for the starting time
$dt = DateTime->today;

# convert the date to YYYYMMDD format with no separator between elements
$now_string = $dt->ymd('');

# get the local time and convert it to the component chunks
my ($dsec,$dmin,$dhour,$dmday,$dmon,$dyear,$dwday,$dyday,$disdst) = 
localtime(time);

# we're going to use this for the generated filenames (. = concatenate)
$minutesseconds = $dmin . $dsec;
$now_string_mm_ss = $now_string . $minutesseconds;

# need to add one to get correct behavior
if ($daysback ne '') {
$daysback++;
$dur = DateTime::Duration->new( days => $daysback );
$dt1 = $dt - $dur;
$cutoff = $dt1->ymd('');
}

# check for illegal time fencing -- can do daysback or start/stop, not both
if ((($daysback ne '') && ($start ne '')) ||
    (($daysback ne '') && ($stop ne '')))
    { die "Cannot use --daysback AND --start or --stop"; }

# want to plot but no filename: synthasize an output filename for the plot
# example name: www.facebook.com.2018021029.postscript
if (($plot ne '') && ($plotout eq ''))
{ $plotout = "$plotdir/$fqdn\.$now_string_mm_ss\.$plotdev"; } 

# if user wants json output format, set the file extension appropriately
if ($jsonl) { $tableextension='jsonl'; }

# set a default title
if ($title eq '') {
  $title = "\$ vot \-\-fqdn $fqdn";
  if ($rectype ne '') { $title = $title   . "\/$rectype"; }
  if ($bailiwick ne '') { $title = $title . "\/$bailiwick"; }
  if ($smooth ne '') { $title = $title    . " \-\-ma $smooth"; }
}

# build the output filename for the table
# example name: www.facebook.com.2018021029.txt 
if (($notable eq '') && ($tableout eq ''))
{ $tableout = "$tabledir/$fqdn\.$now_string_mm_ss\.$tableextension"; 
} 

# user wants to produce SOMETHING, right?
if (($notable ne '') && ($plot eq ''))
{ die "No table output? No plot output? Nothing to do!"; }

# typical input MTBL filename: dns.20181125.D.mtbl
# granularity is a single letter: (Y, M, D, H)

if ($granularity eq "D") {
    $fullpattern = $granularity.'.mtbl';
} else {die "granularity must be D only for now, sorry (case sensitive!)";}

# let's get the list of MTBL files (we'll exclude out-of-scope ones later)
opendir DIR, $mtbldir or die "Cannot open mtbldirectory: $mtbldir $!";
@unsortedfiles = readdir DIR;

# the MTBL file array is unsorted, let's tidy that up
@mtblfiles = sort @unsortedfiles;


# do we have at least 1 file to analyze?
$filecount = @mtblfiles;
if ($filecount == 0) {die "no files of requested granularity in $mtbldir $!";} 

# we've loaded the MTBL files into the @mtblfiles array, so we can close
# the filehandle
closedir DIR;

# loop over the file array, and just keep the ones (roughly) in scope
# we'll add a one day grace period around the actual time period

# compute the adjusted dates
if ($start ne '')    { $roughstart = DateCalc(ParseDate($start), ParseDateDelta('- 1 days')); }

if ($stop ne '')     { $roughstop = DateCalc(ParseDate($stop), ParseDateDelta('+ 1 days')); 
                     }

if ($daysback ne '') { $roughcutoff = $dt1->ymd(''); $roughcutoff = DateCalc(ParseDate($roughcutoff), ParseDateDelta('+ 1 days')); }

foreach $file (@mtblfiles) {

    # make sure the files have the right granularity and aren't an "in process file" starting with a dot
    if ((index($file, $fullpattern) != -1) && (index($file, '^\.') == -1)) {

	# trim the junk from the filename
        $filedate = $file;
	$filedate =~ s/^dns\.//;
	$filedate =~ s/\.D\.mtbl//;

	# convert the date string from the filename to a real DateTime

	$myfiledate = ParseDate($filedate);

        # no time fencing
        $timefencecheck1 =
        (($start eq '') && ($stop eq '') && ($daysback eq ''));

        # just after start
        $timefencecheck2 =
        (($stop  eq '') && ($start ne '') && ($myfiledate ge $roughstart));

        # just before stop
        $timefencecheck3 =
        (($start eq '') && ($stop ne '') && ($myfiledate le $roughstop));

        # after start and before stop
        $timefencecheck4 =
        (($start ne '') && ($stop ne '') &&
        ($filedate ge $roughstart) && ($myfiledate le $roughstop));

        # relative time check...
        $timefencecheck5 =
        (($daysback ne '') && ($myfiledate ge $cutoff));

        if ($timefencecheck1 || $timefencecheck2 || $timefencecheck3 
           || $timefencecheck4 || $timefencecheck5) {
 	    push (@keepermtblfiles, $file); 
           }
      } # end of granularity check

   } # end for each potential MTBL file

# plan is to tally the counts in a perl hash, keyed by the first seen date
# total results found initially? zero, of course
$totalresultsfound = 0;

foreach $file (@keepermtblfiles) {
        # build the command we need to run...
        $cmd = "export DNSTABLE_FNAME=$mtbldir$file ; dnstable_lookup -j rrset $fqdn $rectype $bailiwick\| jq -r \'\"\\(.time_first\|todate\) \\(.count\)\"\' \| sed \'s\/T\.\* \/ \/\' \| sed \'s\/\-/\/g\' \| sort";

        @results = `$cmd`;
	my $sizeofresults = @results;

        $totalresultsfound = $totalresultsfound + $sizeofresults;

	# now load the results from dnstable_lookup into a perl hash
	my $i = 0;
        while ($i<$sizeofresults) 
           {
	       # split out a pair of values (a datestamp and a count)
               ($key, $value) = split(/\s+/,$results[$i]);

	       # double check the time fencing

               $start2 = DateCalc(ParseDate($start), ParseDateDelta('- 2 days')); 
	       $stop2 = ParseDate($stop);

	       # no time fencing
	       my $timefencecheck1 = 
                 (($start2 eq '') && ($stop2 eq '') && ($daysback eq ''));

	       # just after start
	       my $timefencecheck2 =
                   (($stop2  eq '') && ($start2 ne '') && ($key ge $start2));

	       # just before stop
	       my $timefencecheck3 =
                   (($start2 eq '') && ($stop2 ne '') && ($key le $stop2));

               # after start and before stop
	       my $timefencecheck4 =
		   (($start2 ne '') && ($stop2 ne '') &&
                    ($key ge $start2) && ($key le $stop2));

	       # relative time check...
               my $timefencecheck5 =
                   (($daysback ne '') && ($key ge $cutoff));

               if ($timefencecheck1 || $timefencecheck2 || $timefencecheck3 || $timefencecheck4 || $timefencecheck5)
                   {
                       # print periodic status reports
			 print "Processing: $file\n"; 
                       # perl doesn't set initial hash values to zero 
                       # by default, so we need to avoid doing arithmetic 
                       # with UNDEF hash values
  	               if (defined ($hash{$key})) 
                       { $hash{$key}  += $value; } 
                       else 
                       { $hash{$key} = $value; }

                       my $temptally=0;
		       $smoothedvalues{$key} = 'NaN';
                       if ($smooth ne '') {
			  my $validhashvalues=0;
                          for (my $kk=0; $kk < $smooth; $kk++) {
                              my $offset ='- '.$kk.' days';
			      my $checkkey = DateCalc(ParseDate($key), ParseDateDelta($offset));
			      $checkkey =~ s/00:00:00//;

			    if (defined ($hash{$checkkey})) {
                               $validhashvalues++;
		               $temptally=$temptally+$hash{$checkkey};
                               } # end of the valid hash check+tally
			  } # end of the for loop
			  if ($validhashvalues == $smooth){
                                $smoothedvalues{$key} = $temptally/$validhashvalues;
                             } # end of smoothing and value is valid
			   } # end of the "are we smoothing?"
                       } # if in the time fence
	$i++;
        } # while results loop
} # looping over all keeper filenames

if ($totalresultsfound == 0) 
   {die "no results found -- typo in FQDN? wrong rectype or bailiwick?\n";}

# we now have a date=>count hash, let's do something with it

########################### TABLE ###############################

# handle the tabular output case, if tabular output hasn't been supressed
if ($notable eq '') 
{
   # user wants tabular output, so let's open that table output file

   open (my $tfh, '>', $tableout) 
      or die "$0: open $tableout: $!";

   # in printing the following, we have some stuff that's always printed
   # and some stuff that's optionally printed
   # always printed: "$j" (the date) and $hash{$j} (the count)
   # printed if $smooth > 0: $smoothedvalues{$j} (the moving average)
   # printed if $jsonl selected: jsonl formatting cruft
   # The trailing comma is NOT printed if doing $jsonl and it's the last record

   if ($jsonl) {print $tfh '['; }

   my $left= keys %hash;

   foreach my $j (sort keys %hash) 
         {
         if ($jsonl) {print $tfh '{"date":"'; }
         print $tfh "$j";
         print "$j ";
         if ($jsonl eq '') {print $tfh ' ';}
         if ($jsonl) {print $tfh '"},{"count":"';}
         print $tfh "$hash{$j}";
         print "$hash{$j}";
         if ($jsonl) {print $tfh '"}';}
         # also print smoothed values?
         if ($smooth ne '') {
            if ($jsonl) {print $tfh ',{"ma":"';} 
            if ($jsonl eq '') {print $tfh ' ';
            }
            print $tfh "$smoothedvalues{$j}";
            print " $smoothedvalues{$j}";
            if ($jsonl) {print $tfh '"}';}
         } # end smoothed block

         # no comma on the last obs, need right square bracket instead
         if (($jsonl) && ($left >= 1)) {print $tfh ',';}
         elsif ($jsonl) {print $tfh ']';}

         #everybody gets the newline
         print $tfh "\n";
         print "\n";
	 $left--;
         }

print "Table output file: $tableout\n";

close ($tfh);
} # end of table processing block

####################### PLOT #############################

if ($plot ne '') 
{
   $plottype = lc($plottype);

   # three files: one for the plot output, one for the gnuplot commands, and
   # one for a temporary copy of the data

   open (my $pfh, '>', "$plotout")
      || die "$0: open $plotout $!";

   my $tempfilename1 = '';
   my $tempfilename2 = '';

   # temporary file #1 for the gnuplot commands
   (my $tempfh1, $tempfilename1) = tempfile();

   # temporary file #2 for a copy of the data
   (my $tempfh2, $tempfilename2) = tempfile();

   # build the graphic code we'll be running

   my $gnuplotcode = "set term $plotdev size 10in,7in monochrome font 'Helvetica,14'\n";
   print $tempfh1 "$gnuplotcode";
   $gnuplotcode = "set output \"$plotout\"\nset title \"$title\"\n";
   print $tempfh1 "$gnuplotcode";
   $gnuplotcode ="set xdata time\nset timefmt \"\%Y\%m\%d\"\nset format x \"\%Y\%m\%d\"\nset format y '%.0f'\n";
   print $tempfh1 "$gnuplotcode";
   $gnuplotcode ="set xtics rotate by 90 offset 0,-4 out nomirror\nset mxtics\nset style data $plottype\n";
   print $tempfh1 "$gnuplotcode";
   $gnuplotcode ="set datafile missing \"NaN\"\nset xlabel offset 2\nset bmargin 7\nset rmargin 5\nset tmargin 3\n";
   print $tempfh1 "$gnuplotcode";

   if (($noplotraw eq '') && ($plotsmoothed)) {
     # plot raw and smoothed
     my $gnuplotcode2 ="plot '$tempfilename2' using 1:2 with $plottype t 'raw' lt black dt 3, '$tempfilename2' using 1:3 with $plottype t 'smoothed' lt black dt 1\n";
     print $tempfh1 "$gnuplotcode2\n";
   } elsif (($noplotraw ne '') && ($plotsmoothed)) {
     # plot smoothed only
     my $gnuplotcode2 ="plot '$tempfilename2' using 1:3 with $plottype t 'smoothed' lt black dt 1\n";
     print $tempfh1 "$gnuplotcode2\n";
   } else {
     # plot raw only
     my $gnuplotcode2 ="plot '$tempfilename2' using 1:2 with $plottype t 'raw' lt black dt 1\n ";
     print $tempfh1 "$gnuplotcode2\n";
   }

foreach my $j (sort keys %hash)
   {
    print $tempfh2 "$j, $hash{$j}";

    if ($smooth ne '') { print $tempfh2 ", $smoothedvalues{$j}"; }

    print $tempfh2 "\n";
   }
      
   my $tempcommandline = "gnuplot $tempfilename1 < $tempfilename2\n";
   my $ploterrors = `$tempcommandline`;

   print "$ploterrors";

   close $pfh;

print "Plot output file:  $plotout\n";
}
