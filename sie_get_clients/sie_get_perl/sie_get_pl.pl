#!/usr/local/bin/perl

# Check with:  perlcritic --profile ./.perlcritic sie_get_pl.pl

use strict;
use warnings;

use Carp qw( croak );
use Data::Printer;
use Data::Structure::Util qw( unbless );
use DateTime;
use DateTime::Format::Strptime;
use File::HomeDir;
use JSON;
use LWP;
use LWP::UserAgent;
use TimeDate;
use Time::ParseDate;
use POSIX::strftime::GNU;
use POSIX qw(strftime);

my $NL = "\n";

my $endpoint = 'batch.sie-remote.net';
my $useproxy = '0';   # '0' is false

# FUNCTIONS
#
# functions appear in order by dependency (to-be called precedes calling)

# ----------------------------------------------------------------------------
sub getkeyfromlocalfile
  {
  # build the filepath pointing at the SIE-Batch API key
  my $filepath = File::HomeDir->my_home . '/.sie-get-key.txt';

  # make sure the SIE-Batch API key file exists
  if ( !-e $filepath )
  {
    croak( "ERROR:\n\n  No SIE-Batch API keyfile at \n", $filepath );
  }

  # open that key file and read the SIE-Batch API key
  my $fh;
  open ($fh, '<:encoding(UTF-8)', $filepath) or
    croak ("Could not open SIE-Batch API keyfile $fh\n");
  my $myapikey = <$fh>;
  chomp $myapikey;
  close($fh) or croak("Could not close SIE-Batch API keyfile $fh\n");

  return ($myapikey);
  }

# ----------------------------------------------------------------------------
sub make_query
  {
  my $url = $_[0];
  my $useproxy = $_[1];  # we get $useproxy as a global
  my $params = $_[2];
  my $outputfilename = $_[3];

  my $content;
  my $ua;

  $ua = LWP::UserAgent->new();
    $ua->requests_redirectable(['POST',]);
    $ua->default_header('Content-Type' => 'application/json');
    $ua->default_header('Accept' => 'application/json');
    $ua->timeout(86_400);
    $ua->agent('sie_get_pl/1.0');

  if ($useproxy) {
    $ua->proxy([qw(http https)] => 'socks://127.0.0.1:1080');
    $ua->protocols_allowed(['https','http',]);
  } else {
    $ua->protocols_allowed(['https',]);
    $ua->ssl_opts( verify_hostname => 'true');
  }

  # try validating the API key up to 3 times
  # my @response;

  my $tries = 3;
  my $rc;
  my $msg;

  while ($tries >= 1) {
    my $response = $ua->post( $url, content => $params);
    unbless($response);
    $rc = $response->{'_rc'};
    $msg = $response->{'_msg'};
    $content = $response->{'_content'};

    if ( $rc == 200 ) {
      last;
    } else {
      print "Problem in make_query: $rc\n";
      print "Message: $msg\n";
    }

    # if not, try again at least two more times
    $tries = $tries - 1;
    if ($tries == 0) {
      croak('HTTP error (retried three times)');
    }
  }

  return($content);
  }

# ----------------------------------------------------------------------------
sub validateapikeyonline
  {
  # query the live SIE-Batch API server to confirm the key status

  $endpoint = $_[0];
  $useproxy = $_[1];

  my $queryURL = 'https://' . $endpoint . '/siebatchd/v1/validate';

  my $myapikeyval = getkeyfromlocalfile;
  my $queryParams = { 'apikey' => $myapikeyval };
  my $json = encode_json $queryParams;
  my $response = make_query($queryURL, $useproxy, $json, '-999');
  unbless($response);
  my $decoded_response = decode_json $response;
  my $status = $decoded_response->{'_status'};
  return($status);
  }

# -----------------------------------------------------------------------------
# left pad a value with spaces
# https://www.perlmonks.org/?node_id=7462
sub pad
  {
        my ($string, $length)=@_;
        my $padlength=$length-length($string);
## no critic
        return " "x$padlength.$string;
## use critic
  }

# -----------------------------------------------------------------------------
sub format_and_printout_the_chan_list
  {
  my $chan_list = $_[0];
  # my %json_decoded_stuff;
  my %new_hash;

  my $json_decoded_stuff = decode_json $chan_list;
  foreach my $key (keys %{$json_decoded_stuff}) {

    my $actual_val=%$json_decoded_stuff{$key}->{'description'};

    # left pad the channel number to a width of 3
    my $key2 = $key;
    $key2 =~ s/ch//s;
    my $keystring = pad($key2,3);
    $new_hash{$keystring} = $actual_val;
  }

  foreach my $keystring (sort keys %new_hash) {
    printf("%s  %s\n", $keystring, $new_hash{$keystring});
  }
  return;
  }

# -----------------------------------------------------------------------------
sub list_channels
  {

  my $endpoint = $_[0];
  my $useproxy = $_[1];

  my $myapikeyval = getkeyfromlocalfile;
  my $queryURL = 'https://' . $endpoint . '/siebatchd/v1/validate';
  my $queryParams = to_json{ 'apikey' => $myapikeyval };
  my $response = make_query($queryURL, $useproxy, $queryParams, '-999');

  unbless($response);
  my $decoded_response = decode_json $response;
  my $extract_bit = $decoded_response->{'profile'}->{'siebatch'};
  my $json_query_object = to_json($extract_bit);
  format_and_printout_the_chan_list($json_query_object);
  return;
  }

# -----------------------------------------------------------------------------
# https://www.oreilly.com/library/view/perl-cookbook/1565922433/ch02s18.html
#
sub add_commas
  {
  my $num_string = $_[0];
  my $text = reverse $num_string;
  $text =~ s/(\d\d\d)(?=\d)(?!\d*\.)/$1,/gsmx;
  return scalar reverse $text;
  }

# -----------------------------------------------------------------------------
#
sub format_and_printout_chan_time_limits
  {
  my $chan = $_[0];
  my $earliest_time_string = $_[1];
  my $latest_time_string = $_[2];
  my $volume = $_[3];

  my $chan_with_prefix = 'ch' . $chan;  # needed to correspond to the API :-(

  # take the channel status parameters and print them out in a little report
  $chan = sprintf '%4s', $chan;
  if (int($volume) >= 4) {
    $volume = add_commas($volume);
  }

  # could add a header, but it's pretty self-obvious, right?
  # printf('chan  earliest datetime    latest datetime       octets\n');
  printf("%s  \"%s\"  \"%s\"  %s\n", $chan, $earliest_time_string,
    $latest_time_string, $volume);
  return;
  }

# -----------------------------------------------------------------------------
sub show_intervals
  {
  my $endpoint = $_[0];
  my $useproxy = $_[1];
  my $chan_to_check = $_[2];

  # with square brackets
  my $newchan_to_check = '[' . $chan_to_check . ']';
  # no brackets, but with ch literal prefix
  my $chan_with_prefix = 'ch' . $chan_to_check;

  my $myapikeyval = getkeyfromlocalfile;
  my $params  = encode_json{ 'apikey'   => $myapikeyval,
                             'channels' => $newchan_to_check };

  # fix up a bit of awkwardness
  # {"channels":"[212]","apikey":"blah"}  needs to become
  # {"channels":[212],"apikey":"blah"}    (e.g., no quotes around [chan])
  $params =~ s/"\[/[/s;
  $params =~ s/]"/]/s;

  my $url = 'https://' . $endpoint . '/siebatchd/v1/siebatch/chdetails';
  my $response = make_query($url, $useproxy, $params, '-999');
  my $decoded_results=from_json($response);
  unbless($decoded_results);

  my $earliest_time_string =
    $decoded_results->{'channels'}{$chan_with_prefix}{'earliest'};
  my $latest_time_string =
    $decoded_results->{'channels'}{$chan_with_prefix}{'latest'};
  my $size_string =
    $decoded_results->{'channels'}{$chan_with_prefix}{'size'};

  return ($earliest_time_string, $latest_time_string, $size_string);
  }

# ----------------------------------------------------------------------------
sub fixup_ending_datetime_in_the_future
  {
  # Stop datetime out of range. Replacing with current GMT time.

  my $epochseconds = parsedate(gmtime());
  my $enddatetime = strftime('%Y-%m-%d %H:%M:%S', gmtime());
  $enddatetime =~ s/..$/00/s;
  return($enddatetime);
  }

# -----------------------------------------------------------------------------
sub check_channel
  {
  # make sure that the channel is available and the dates are in-range

  my $endpoint = $_[0];
  my $useproxy = $_[1];
  my $chanflagstring = $_[2];
  my $startdatetime = $_[3];
  my $enddatetime = $_[4];

  # get the available datetime range for this channel
  (my $earliest_time_string, my $latest_time_string, my $channel_octers) =
     show_intervals($endpoint, $useproxy, $chanflagstring);

  # convert the requested and available start datetimes into Un*x seconds
  my $requested_start_seconds = parsedate($startdatetime);
  my $earliest_date_seconds =   parsedate($earliest_time_string);

  # convert the requested and available end datetimes into Un*x seconds
  my $requested_stop_seconds = parsedate($enddatetime);
  my $latest_date_seconds =    parsedate($latest_time_string);

  # start datetime must be earlier than stop date time
  if (($requested_stop_seconds - $requested_start_seconds) < 0) {
    croak('Start datetime must be earlier than stop datetime');
  }

  # start datetime may not be earlier than earliest data available
  if (($requested_start_seconds - $earliest_date_seconds) < 0) {
    croak('Start datetime out of range. Must be no earlier than ' . \
      $earliest_time_string);
  }

  # end datetime may not be in the future
  if (($requested_stop_seconds - $latest_date_seconds) > 0) {
    $enddatetime = fixup_ending_datetime_in_the_future();
  }

  # handle return of the (potentially updated) datetimes here
  my @timearray;
  $timearray[0] = $startdatetime;
  $timearray[1] = $enddatetime;
  return (@timearray);
  }

# ----------------------------------------------------------------------------
sub validate_input_time_date_format
  {
  # parameter is datetime to format check
  # if invalid, abort run
  # if valid, return the validated (but unchanged) datetime (could skip
  # doing this for now, but at some point we might decide to fix up bad
  # string formatting as a convenience to the user, so...)

  my $mydatetime = $_[0];

  # check the format with a regex
  if (($mydatetime =~ /\A\d{4}-\d{2}-\d{2}\ \d{2}:\d{2}:\d{2}\Z/smx)){
    # good starting time format
  } else {
    print("bad starting time format -- must be \"YYYY-MM-DD HH:MM:SS\"\n");
    exit(1);
  }

  return ($mydatetime);
  }

# -----------------------------------------------------------------------------
sub zero_unused_seconds
  {
  # since SIE-Batch API does not care about seconds, we set them to zero

  my $mydatetime = $_[0];
  $mydatetime =~ s/..$/00/s;
  return ($mydatetime);
  }

# -----------------------------------------------------------------------------
sub convert_relative_times_to_real_datetimes
  {
  my $minutesback = $ARGV[2];
  my $format = '%Y-%m-%d %H:%M:%S';

  # in relative format, the initial "ending time" is actually the minutes
  # worth of data we want to retrieve
  # the "real" ending datetime will be created from the current GMT time
  # we will be doing math on the epoch seconds

  my $endingtime = gmtime();
  my $epochseconds = parsedate($endingtime);

  # now compute the formatted ending date time in standard YYYY-MM-DD HH:MM:SS
  my $enddatetime = strftime $format, localtime $epochseconds;
  # find just the seconds from that string
  my $extraseconds = strftime '%S', localtime $epochseconds;
  # subtract the seconds from the full datetime to end up with 00 seconds
  my $endingtime_seconds = int($epochseconds) - int($extraseconds);

  # let's now work on the starting time
  # we compute the "real" starting datetime by offsetting backwards
  # our to-be-modified datetime is in epoch seconds, so convert min to seconds
  my $mysecondsback = int($minutesback) * 60;
  my $startseconds = $endingtime_seconds - int($mysecondsback);
  my $startdatetime = strftime $format, localtime $startseconds;

  $enddatetime = strftime $format, localtime $endingtime_seconds;

  my @timearray;
  $timearray[0] = $startdatetime;
  $timearray[1] = $enddatetime;

  return(@timearray);
  }

# -----------------------------------------------------------------------------
sub fix_times
  {
  # arguments come in from the command line so we don't pass them in

  # standardize the starting and ending times from the command line
  (my $chanflagstring, my $startdatetime, my $enddatetime) = @ARGV;

  # if relative times, replace the ending time with the current GMT time
  # set the starting time back by the specified number of minutes
  my @timearray;
  if ($startdatetime eq 'now') {
    @timearray = convert_relative_times_to_real_datetimes($enddatetime);
  } else {
    # we have real timedate stamps for starting and ending datetimes

    # process the starting datetime value...
    # correctly written datetime value?
    # also zero the seconds if present (SIE-Batch API doesn't use them)
    validate_input_time_date_format($startdatetime);
    $startdatetime = zero_unused_seconds($startdatetime);

    # repeat for the ending datetime value...
    validate_input_time_date_format($enddatetime);
    $enddatetime = zero_unused_seconds($enddatetime);

    $timearray[0] = $startdatetime;
    $timearray[1] = $enddatetime;
  }
  return (@timearray);
  }

# -----------------------------------------------------------------------------
sub check_if_chan_is_an_nmsg_chan
  {
  my $chanflagstring = $_[0];

  my %keys = (204 => 1,
              206 => 1,
              207 => 1,
              208 => 1,
              221 => 1,);

  my $filetype = undef;
  # my $keys;
  if ($keys{$chanflagstring}) {
    $filetype = '.nmsg';
  } else {
    $filetype = '.jsonl';
  }

  return($filetype);
  }

# -----------------------------------------------------------------------------
sub build_filename
  {
  # construct the filename and return it
  my $string1 = $_[1];
  my $string2 = $_[2];
  my $chanflagstring = $ARGV[0];

  $string1 =~ s/ /@/s;
  $string2 =~ s/ /@/s;

  my $filetype = check_if_chan_is_an_nmsg_chan($chanflagstring);
  my $outputfilename = 'sie-ch' . $chanflagstring . '-{' . $string1 .
    '}-{' . $string2 . '}' . $filetype;

  return($outputfilename);
  }

# ----------------------------------------------------------------------------
sub print_usage_info
  {
  print <<'FOO';
Usage:

  sie_get_pl channel "now" minutesBack
  Example: sie_get_pl 212 now 15

    OR

  sie_get_pl channel "startdatetime" "enddatetime"
  Example: sie_get_pl 212 "2020-01-07 00:13:00" "2020-01-07 00:28:00"

Convenience functions:

  Check SIE-Batch API key:                      sie_get_pl checkkey
  Get a listing of channels:                    sie_get_pl channels
  Get datetime range and volume for a channel:  sie_get_pl 212

Notes:

  Datetimes are UTC and must be quoted. (Current UTC datetime: $ date -u )
  Zero pad any single digit months, days, hours, minutes or seconds.
  Seconds must be entered as part of the UTC datetimes (but are ignored)
  Ending datetime in the future? It will be clamped to current datetime.

  minutesBack must be >= 1 and a whole number
FOO
  exit(0);
  }

# -----------------------------------------------------------------------------
sub one_real_arg
  {
  # because users can ask for channel info by specifying a channel
  # number, we need to know the list of defined channel numbers

  my $first_arg = $_[2];
  my $chanflagstring = $ARGV[0];

  my %defined_channels = (24 =>  'true',
                          25 =>  'true',
                          27 =>  'true',
                          42 =>  'true',
                          80 =>  'true',
                          114 => 'true',
                          115 => 'true',
                          204 => 'true',
                          206 => 'true',
                          207 => 'true',
                          208 => 'true',
                          211 => 'true',
                          212 => 'true',
                          213 => 'true',
                          214 => 'true',
                          221 => 'true',
                         );

    my $first_arg_looks_numeric = ($first_arg =~ /^\d+$/s);
    my $status;

    if ($first_arg eq 'channels') {
        # list channels for the user
        list_channels ($endpoint, $useproxy);
        exit(0);
    } elsif ($first_arg eq 'checkkey') {
        # check the user's key for validity
        $status = validateapikeyonline($endpoint, $useproxy);
        print('API key status is ' . $status . "\n");
        exit(0);
    } elsif ($defined_channels{$first_arg}) {
        # list details about the specified channel
        (my $earliest, my $latest, my $datasize) =
          show_intervals($endpoint, $useproxy, $chanflagstring);
        format_and_printout_chan_time_limits($chanflagstring, $earliest,
          $latest, $datasize);
        exit(0)
    } elsif ((not($defined_channels{$chanflagstring})) &&
         ($first_arg_looks_numeric)) {
        ### the requested channel is not one we offer, so...
        print("Channel not offered via this script\n");
        exit(0);
    } else {
        print_usage_info();
        exit(0);
    }
}

# ----------------------------------------------------------------------------
sub three_real_args
    {
    my $first_arg = $ARGV[0];
    my $startdatetime = $ARGV[1];
    my $enddatetime = $ARGV[2];

    my $myapikey = getkeyfromlocalfile;

    ($startdatetime, $enddatetime) = fix_times();

    ($startdatetime, $enddatetime) = check_channel($endpoint, $useproxy,
        $first_arg, $startdatetime, $enddatetime);

    my $outputfilename = build_filename($first_arg, $startdatetime,
        $enddatetime);

    my %params = ('apikey' => getkeyfromlocalfile,
       'channel' => int($first_arg),
       'start_time' => $startdatetime,
       'end_time' => $enddatetime,);

    # the backslash in the following is critical; w/o it, $params2 becomes 4
    my $params2 = encode_json \%params;

    my $queryURL = 'https://' . $endpoint . '/siebatchd/v1/siebatch/chfetch';

    my $response = make_query($queryURL, $useproxy, $params2,
        $outputfilename);

    # write the SIE-Batch data out to our file
    open my $fh, '>:raw', $outputfilename or croak();
    print $fh $response;
    close($fh) or croak ("Could not successfully close output file $fh\n");
    exit(0);
}

# =============================================================================
# main

my $first_arg;
my $status;

if (@ARGV) {
  $first_arg = $ARGV[0];
}

my $command_line_arg_count = $#ARGV + 1;

if ($command_line_arg_count == 1)
{
  one_real_arg($endpoint, $useproxy, $first_arg);
  exit(0);
}
elsif ($command_line_arg_count == 3)
{
  three_real_args($endpoint, $useproxy, $first_arg);
  exit(0);
}
elsif (($command_line_arg_count <= 0) ||
       ($command_line_arg_count == 2) ||
       ($command_line_arg_count >= 4))
{
      print_usage_info();
      exit(0);
}
