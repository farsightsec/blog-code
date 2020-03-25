#!/usr/bin/env ruby

require 'Typhoeus'
require 'json'
require 'date'
require 'set'

# the SIE-Batch API endpoint is NOT routinely globally accessible
#
# your IP must be explicitly whitelisted (or use a proxy server that
# has been authorized access). If using a proxy server, ssh into that
# bastionhost before running this script:
#
#    $ ssh -D 127.0.0.1:1080 bastionhostname

useproxy = false
endpoint = 'batch.sie-remote.net'

# FUNCTIONS
#
# functions appear in order by dependency (to-be called fn precedes calling fn)

# ----------------------------------------------------------------------------
def getkeyfromlocalfile
  # build the filepath pointing at the SIE-Batch API key in the user's homedir
  filepath = Dir.home + '/.sie-get-key.txt'

  # make sure the SIE-Batch API key file exists
  unless File.exist?(filepath)
    abort('\nERROR:\n\n  No SIE-Batch API keyfile at ' + filepath)
  end

  # open that key file and read the SIE-Batch API key
  file = File.open(filepath)
  myapikeyval = file.read.chomp
  file.close

  return myapikeyval
end

# ----------------------------------------------------------------------------
def make_query(useproxy, queryUrl, queryParams)
  # actually handle making the http/https query against the endpoint

  myqueryURL = queryUrl.dup.to_s
  myqueryParams = queryParams.dup.to_s

  # (other Typhoeus options defined:
  # https://github.com/typhoeus/ethon/blob/master/lib/ethon/curls/options.rb )

  if useproxy == true
    request = Typhoeus::Request.new( \
      myqueryURL, \
      method: :post, \
      headers: { 'Application' => 'application/json', \
                 'User-Agent' => 'sie_get_rb/1.0' }, \
      body: myqueryParams, \
      connecttimeout: 10, \
      ipresolve: :v4, \
      proxy: 'http://127.0.0.1:1080', \
      proxytype: 'socks5' \
    )
  else
    request = Typhoeus::Request.new( \
      myqueryURL, \
      method: :post, \
      headers: { 'Application' => 'application/json', \
                 'User-Agent' => 'sie_get_rb/1.0' }, \
      body: myqueryParams, \
      connecttimeout: 10, \
      use_ssl: :all, \
      ssl_verifypeer: true \
    )
  end

  # try validating the API key up to 3 times
  response = nil
  tries = 3
  while tries >= 1
    response = request.run

    # if successful, break out of the attempt loop and proceed
    if response.code == 200
      break
    elsif response.code == 403
      abort("Response Code 403: Bad SIE-Batch API Key?")
    end

    # if not, try again at least two more times
    tries -= 1
    if response.code.zero? || tries.zero?
      abort('HTTP error (retried three times)')
    end

  end
  return response.body
end

# ----------------------------------------------------------------------------
def validateapikeyonline(endpoint, useproxy)
  # after getting the API key from the local key file, is it valid? We'll check
  # online

  myapikeyval = getkeyfromlocalfile

  queryURL = 'https://' + endpoint + '/siebatchd/v1/validate'
  queryParams = { 'apikey' => myapikeyval }.to_json
  response = make_query(useproxy, queryURL, queryParams)
  json_query_object = JSON.parse(response)
  status = json_query_object['_status']
  return status
end

# -----------------------------------------------------------------------------
def format_and_printout_the_chan_list(chan_list)
  # this routine actually formats and writes out the channel listing
  new_hash = {}
  chan_list.each do |key, value|
    tempstring = (key.to_s + value.to_s)
    tempstring = tempstring.sub('ch', '')
    tempstring = tempstring.sub('{"description"=>"', ' ')
    tempstring = tempstring.sub('"}', '')
    delim = tempstring.index(' ')
    strleng = tempstring.length
    # left pad the channel number to a width of 3
    keystring = tempstring[0, delim].rjust(3, ' ')
    valstring = tempstring[delim + 1, strleng]
    new_hash[keystring] = valstring
  end

  new_hash = new_hash.sort
  new_hash.each do |keystring, valstring|
    printf("%s  %s\n", keystring, valstring)
  end
end

# -----------------------------------------------------------------------------
def list_channels(endpoint, useproxy)
  # this routine gets the list of live channels from the API server

  myapikeyval = getkeyfromlocalfile

  queryUrl = 'https://' + endpoint + '/siebatchd/v1/validate'
  queryParams = { 'apikey' => myapikeyval }.to_json
  response = make_query(useproxy, queryUrl, queryParams)
  json_query_object = JSON.parse(response)
  obj = json_query_object['profile']['siebatch']
  format_and_printout_the_chan_list(obj)
  exit(0)
end

# -----------------------------------------------------------------------------
# https://stackoverflow.com/questions/23169510/adding-commas-to-numbers-in-ruby
#
def add_commas(num_string)
  # utility routine to add commas to numeric strings for readability
  num_string.reverse.scan(/\d{3}|.+/).join(',').reverse
end

# -----------------------------------------------------------------------------
#
def format_and_printout_chan_time_limits(chan, earliest_time_string,
  latest_time_string, volume)
  # take those channel status parameters and print them out in a little report

  chan = chan.rjust(4, ' ')
  volume = add_commas(volume.to_s)
  # could add a header, but it's pretty self-obvious, right?
  # printf('chan  earliest datetime    latest datetime       octets\n')
  printf("%s  \"%s\"  \"%s\"  %s\n", chan, earliest_time_string,
    latest_time_string, volume)
end

# -----------------------------------------------------------------------------
def show_intervals(endpoint, useproxy, chan_to_check)
  # each channel is available for a range of dates, and has an associated
  # data volume (so you'll know if you're potentially startng a huge download)

  params = '{"apikey":"' +
    getkeyfromlocalfile +
    '","channels":[' +
    chan_to_check +
    ']}'
  url = 'https://' + endpoint + '/siebatchd/v1/siebatch/chdetails'

  response = make_query(useproxy, url, params)

  json_query_object = JSON.parse(response)

  chan_to_check = 'ch' + chan_to_check
  earliest_time_string =
    json_query_object['channels'][chan_to_check]['earliest']
  latest_time_string = json_query_object['channels'][chan_to_check]['latest']
  chan_size = json_query_object['channels'][chan_to_check]['size']

  # we return results as an array since Ruby is a return-one-arg-only language
  timearray = Array.new(3)
  timearray[0] = earliest_time_string
  timearray[1] = latest_time_string
  timearray[2] = chan_size
  return timearray
end

# ----------------------------------------------------------------------------
def fixup_ending_datetime_in_the_future
  # Stop datetime from being out of range. Replacing with current GMT time.

  format = '%Y-%m-%d %H:%M:%S'

  epochseconds = Time.now
  extraseconds = epochseconds.strftime('%S')
  endingtime_seconds = epochseconds.gmtime.to_i - extraseconds.to_i
  endingtime = Time.at(endingtime_seconds).gmtime.to_datetime
  enddatetime = endingtime.strftime(format)

  return enddatetime
end

# -----------------------------------------------------------------------------
def check_channel(endpoint, useproxy, chanflagstring, startdatetime, \
  enddatetime)
  # make sure that the channel is available and the dates are in-range

  # get the available datetime range for this channel
  (earliest_time_string, latest_time_string) = show_intervals(endpoint, \
    useproxy, chanflagstring)

  # convert the requested and available START datetimes into Un*x seconds
  requested_start_seconds = DateTime.parse(startdatetime).to_time.to_i
  earliest_date_seconds = DateTime.parse(earliest_time_string).to_time.to_i

  # convert the requested and available END datetimes into Un*x seconds
  requested_stop_seconds = DateTime.parse(enddatetime).to_time.to_i
  latest_date_seconds = DateTime.parse(latest_time_string).to_time.to_i

  # stop datetime must be later than start date time
  if (requested_stop_seconds - requested_start_seconds).negative?
    abort('Start datetime must be earlier than stop datetime')
  end

  # requested start datetime must be >= the earliest data available
  if (requested_start_seconds - earliest_date_seconds).negative?
    abort('Start datetime out of range. Must be no earlier than ' + \
      earliest_time_string)
  end

  # end datetime may not be in the future; we fix it up by clamping
  # the end time to now if it is asking for something in the future
  if (requested_stop_seconds - latest_date_seconds).positive?
    enddatetime = fixup_ending_datetime_in_the_future
  end

  # handle return of the (potentially updated) datetimes here
  # another case of needing to pack multiple returns into an array
  timearray = Array.new(2)
  timearray[0] = startdatetime
  timearray[1] = enddatetime
  return timearray
end

# ----------------------------------------------------------------------------
def validate_input_time_date_format(mydatetime)
  # parameter is a datetime that we want to format check
  # if invalid, abort run
  # if valid, return the validated (but unchanged) datetime (could skip
  # doing this for now, but at some point we might decide to fix up bad
  # string formatting as a convenience to the user, so...)

  # check the format with a regex
  if !(mydatetime =~ /\A\d{4}-\d{2}-\d{2}\ \d{2}:\d{2}:\d{2}\Z/).nil?
    # good starting time format
  else
    puts('bad starting time format -- must be "YYYY-MM-DD HH:MM:SS"')
    abort('')
  end

  return mydatetime
end

# -----------------------------------------------------------------------------
def zero_unused_seconds(mydatetime)
  # since SIE-Batch API does not care about seconds, we force them to zero
  mynewdatetime = mydatetime.sub!(/..$/, "00")
  return mynewdatetime
end

# -----------------------------------------------------------------------------
def convert_relative_times_to_real_datetimes(enddatetime)
  format = '%Y-%m-%d %H:%M:%S'

  # the new "real" ending datetime comes from the current GMT time
  epochseconds = Time.now
  extraseconds = epochseconds.strftime('%S')
  # remove any "extra" seconds that would make the formatted time
  # non-zero
  endingtime_seconds = epochseconds.gmtime.to_i - extraseconds.to_i

  # we compute the "real" starting datetime by offsetting backwards
  # we get minutes from the user, but need seconds
  mysecondsback = enddatetime.to_i * 60
  startseconds = endingtime_seconds - mysecondsback.to_i
  # convert from epoch seconds back to datetime object
  stageddatetime = Time.at(startseconds).gmtime.to_datetime
  # format the date time object as a time string
  startdatetime = stageddatetime.strftime(format)

  # repeat conversion and formatting for the ending time
  endingtime = Time.at(endingtime_seconds).gmtime.to_datetime
  enddatetime = endingtime.strftime(format)

  # pass as an array
  timearray = Array.new(2)
  timearray[0] = startdatetime
  timearray[1] = enddatetime
  return timearray
end

# -----------------------------------------------------------------------------
def fix_times
  # starting and ending time arguments come in from the command line
  # so we don't pass them in as parameters when calling fix_times

  # standardize the starting and ending times from the command line
  startdatetime = ARGV[1].dup
  enddatetime   = ARGV[2].dup

  # if relative times, replace the ending time with the current GMT time
  # set the starting time back by the specified number of minutes

  # ensure results returned from convert_relative_times_to_real_datetimes
  # are correctly scoped/available routine-wide
  timearray = Array.new(2)

  if startdatetime == 'now'

    timearray = convert_relative_times_to_real_datetimes(enddatetime)

  else

    # we have real timedate stamps for starting and ending datetimes

    # process the starting datetime value...
    # is the datetime value written in the right format?
    # also zero the seconds if present (SIE-Batch API doesn't use them)
    validate_input_time_date_format(startdatetime)
    startdatetime = zero_unused_seconds(startdatetime)

    # repeat for the ending datetime value...
    validate_input_time_date_format(enddatetime)
    enddatetime = zero_unused_seconds(enddatetime)

    timearray[0] = startdatetime
    timearray[1] = enddatetime
  end

  return timearray
end

# -----------------------------------------------------------------------------
def check_if_chan_is_an_nmsg_chan(chanflagstring)
  # some channels use JSON lines format, some use nmsg
  # this routine defines which is which
  nmsg_channels = ['204', '206', '207', '208', '221'].to_set

  if nmsg_channels.include?(chanflagstring)
    filetype = '.nmsg'
  else
    filetype = '.jsonl'
  end

  return filetype
end

# -----------------------------------------------------------------------------
def build_filename(startdatetime, enddatetime)
  # construct the filename from the command line arguments and return it

  chanflagstring = ARGV[0].dup
  string1 = startdatetime.dup
  string2 = enddatetime.dup

  # don't want spaces in the filename; could use something else, like 'T'
  # here instead, I suppose
  string1.sub!(' ', '@')
  string2.sub!(' ', '@')

  filetype = check_if_chan_is_an_nmsg_chan(chanflagstring)
  outputfilename = 'sie-ch' +
    chanflagstring.to_s +
    '-{' +
    string1.to_s +
    '}-{' +
    string2.to_s +
    '}' +
    filetype.to_s
  return outputfilename
end

# ----------------------------------------------------------------------------
def print_usage_info
  puts(
    '''
Usage:

  sie_get_rb channel "now" minutesBack
  Example: sie_get_rb 212 now 15

    OR

  sie_get_rb channel "startdatetime" "enddatetime"
  Example: sie_get_rb 212 "2020-01-07 00:13:00" "2020-01-07 00:28:00"

Convenience functions:

  Check SIE-Batch API key:                      sie_get_rb checkkey
  Get a listing of channels:                    sie_get_rb channels
  Get datetime range and volume for a channel:  sie_get_rb 212

Notes:

  Datetimes are UTC and must be quoted. (Current UTC datetime: $ date -u )
  Zero pad any single digit months, days, hours, minutes or seconds.
  Seconds must be entered as part of the UTC datetimes (but are ignored)
  Ending datetime in the future? It will be clamped to current datetime.

  minutesBack must be >= 1 and a whole number
'''
  )
  exit(1)
end

# -----------------------------------------------------------------------------
# https://stackoverflow.com/questions/1235863/how-to-test-if-a-string-is-basically-an-integer-in-quotes-using-ruby
# note: rubocop complains, but since I've basically taken this code verbatim,
# I'm leaving it as is

# rubocop:disable Naming/PredicateName
# rubocop:disable Style/RedundantSelf
class String
  def is_integer?
    self.to_i.to_s == self
  end
end
# rubocop:enable Naming/PredicateName
# rubocop:enable Style/RedundantSelf

# -----------------------------------------------------------------------------
def one_real_arg(endpoint, useproxy, first_arg)
  # because users can ask for channel info by specifying a channel
  # number, we need to know the list of defined channel numbers
  defined_channels = ['24', '25', '27', '42', '80', '114', '115', \
    '204', '206', '207', '208', '211', '212', '213', '214', '221'].to_set

  if first_arg == 'channels'
    # list channels for the user
    list_channels(endpoint, useproxy)
    exit(0)
  elsif first_arg == 'checkkey'
    # check the user's key for validity
    status = validateapikeyonline(endpoint, useproxy)
    puts('API key status is ' + status)
    exit(0)
  elsif defined_channels.include?(first_arg)
    # list details about the specified channel
    (earliest, latest, datasize) = show_intervals(endpoint, \
      useproxy, first_arg)
    format_and_printout_chan_time_limits(first_arg, earliest, \
      latest, datasize)
    exit(0)
  elsif !defined_channels.include?(first_arg) && first_arg.is_integer?
    ### the requested channel is not one we offer, so...
    print("Channel not offered via this script\n")
    exit(0)
  else
    print_usage_info
  end
  exit(0)
end

# -----------------------------------------------------------------------------
def three_real_args(endpoint, useproxy)
  # We appear to actually be working on retrieving data....
  chanflagstring = ARGV[0]

  # beat the datetimes into shape, if need be
  (startdatetime, enddatetime) = fix_times

  # be sure the requested channel and requested time range are good to go
  # (check the online server to make sure the channel exists, and the
  # start and end datetimes are sane)
  (startdatetime, enddatetime) = check_channel(endpoint, useproxy, \
  chanflagstring, startdatetime, enddatetime)

  # get the output filename
  outputfilename = build_filename(startdatetime, enddatetime)

  # actually get the SIE-Batch data
  param = JSON.generate\
    ({ 'apikey' => getkeyfromlocalfile, \
       'channel' => chanflagstring.to_i, \
       'start_time' => startdatetime, \
       'end_time' => enddatetime })

  url = 'https://' + endpoint + '/siebatchd/v1/siebatch/chfetch'
  response2 = make_query(useproxy, url, param)

  # write the SIE-Batch data out to our file
  File.open(outputfilename, 'wb') do |file|
    file.write(response2)
  end

  exit(0)
end

# =============================================================================
# main

command_line_arg_count = ARGV.length
if command_line_arg_count >= 1
  first_arg = (ARGV[0]).dup
end

if command_line_arg_count == 1
  one_real_arg(endpoint, useproxy, first_arg)
  exit(0)
elsif command_line_arg_count == 3
  three_real_args(endpoint, useproxy)
  exit(0)
elsif command_line_arg_count.zero? || (command_line_arg_count == 2) || \
  (command_line_arg_count >= 4)
  print_usage_info
  exit(0)
end
