#!/usr/bin/env python3
"""This script demonstrates use of the SIE-Batch API from Python3"""

# Lint with $ pylint sie_get_py.py      (assumes .pylintrc file in dir)

import calendar
import datetime
from datetime import datetime
from io import BytesIO
import json
from os import path
from pathlib import Path
import re
import sys
import time
from time import strftime
import pycurl

endpoint = 'batch.sie-remote.net'
useproxy = False    # note: 'false' will not work

# -----------------------------------------------------------------------------
#
def getkeyfromlocalfile():
    """Retrieves the SIE-Batch API key"""

    filepath = str(Path.home()) + "/.sie-get-key.txt"

    if not path.exists(filepath):
        print("\nERROR:\n\n  No SIE-Batch API keyfile at "+filepath)
        sys.exit(1)

    with open(filepath) as stream:
        myapikey = stream.read().rstrip()

    return myapikey

# -----------------------------------------------------------------------------
#
def make_query(url, useproxy, params, outputfilename):
    """make query"""

    if outputfilename != '-999':
        try:
            f = open(outputfilename, "wb")
        except IOError:
            sys.exit("error opening output file for results")
    else:
        buffer = BytesIO()

    c = pycurl.Curl()
    c.setopt(pycurl.URL, url)
    c.setopt(pycurl.HTTPHEADER, ['Content-Type: application/json'])
    c.setopt(pycurl.POST, True)
    c.setopt(pycurl.POSTFIELDS, params)
    c.setopt(pycurl.FOLLOWLOCATION, 1)
    c.setopt(pycurl.CONNECTTIMEOUT, 300)
    c.setopt(pycurl.TIMEOUT, 86400)
    c.setopt(pycurl.USERAGENT, 'sie_get_py/1.0')

    # we're going to write the actual data files directly to the outputfile
    # other stuff (apikey check, channel listing, etc.) we're just going
    # to write to a buffer (and then read that))
    if outputfilename == '-999':
        c.setopt(pycurl.WRITEDATA, buffer)
    else:
        c.setopt(pycurl.WRITEDATA, f)

    if useproxy:
        c.setopt(pycurl.PROXY, '127.0.0.1')
        c.setopt(pycurl.PROXYPORT, 1080)
        c.setopt(pycurl.PROXYTYPE, pycurl.PROXYTYPE_SOCKS5)

    tries = 3
    rc = ''

    while tries >= 1:
        c.perform()
        rc = c.getinfo(c.RESPONSE_CODE)

        # if writing to a buffer, we need to extract results and change
        # from a bytestring to a string
        if outputfilename == '-999':
            body = buffer.getvalue()
            content = body.decode('iso-8859-1')

        # successful transfer? if so, break out of the loop
        # if not, try it again
        #pylint: disable=no-else-break
        if rc == 200:
            break
        else:
            print('Problem in make_query: response code='+str(rc))

    #pylint: disable=no-else-return
    if outputfilename == '-999':
        return content
    else:
        sys.exit(0)
    #pylint: enable=no-else-break

# -----------------------------------------------------------------------------
def validateapikeyonline(endpoint, useproxy):
    """ check the API key for validity on the live SIE-Batch API server """
    myapikeyval = getkeyfromlocalfile()
    params = {'apikey' : myapikeyval}
    params2 = json.dumps(params)
    queryURL = 'https://' + endpoint + '/siebatchd/v1/validate'
    returned_content = make_query(queryURL, useproxy, params2, '-999')
    returned_content_json_format = json.loads(returned_content)
    status = returned_content_json_format['_status']
    return status

# -----------------------------------------------------------------------------
#
def format_and_printout_the_chan_list(chan_list):
    """ we have the channel data, now format and print it out in a report """
    chan_list_json_format = json.loads(chan_list)

    new_hash = {}
    #pylint: disable=unused-variable
    for k, v in chan_list_json_format.items():
        #pylint: enable=unused-variable
        keystring = k.replace('ch', '')
        keystring = keystring.rjust(3, ' ')
        actual_val = chan_list_json_format[k]['description']
        new_hash[keystring] = actual_val

    for k, v in sorted(new_hash.items()):
        print(k, new_hash[k])

    sys.exit(0)

# -----------------------------------------------------------------------------
#
def list_channels(endpoint, useproxy):
    """ retrieve a list of channels from the server """
    myapikeyval = getkeyfromlocalfile()
    params = {'apikey' : myapikeyval}
    params2 = json.dumps(params)
    queryURL = 'https://' + endpoint + '/siebatchd/v1/validate'
    returned_content = make_query(queryURL, useproxy, params2, '-999')
    returned_content_json_format = json.loads(returned_content)
    extract_bit = returned_content_json_format['profile']['siebatch']
    json_query_object = json.dumps(extract_bit)
    format_and_printout_the_chan_list(json_query_object)
    sys.exit(0)

# -----------------------------------------------------------------------------
#
def format_and_printout_chan_time_limits(chan, earliest_time_string, \
    latest_time_string, volume):
    """ print a summary of available channel date range and volume """

    # take the channel status parameters and print them out in a little report
    chan = chan.rjust(4)
    if int(volume) >= 4:
        volume = '{:,d}'.format(int(volume))
        volume = volume.rjust(16)

    # could add a header, but it's pretty self-obvious, right?
    # printf('chan  earliest datetime    latest datetime       octets\n')
    print(chan+'  "'+earliest_time_string+'"  '+\
                 '"'+latest_time_string+'"  '+volume)
    sys.exit(0)

# -----------------------------------------------------------------------------
#
def show_intervals(endpoint, useproxy, chan_to_check):
    """ get the starting and stopping date range and volume """
    # with square brackets
    newchan_to_check = '[' + chan_to_check + ']'
    # no brackets, but with ch literal prefix
    chan_with_prefix = 'ch' + chan_to_check

    myapikeyval = getkeyfromlocalfile()
    params = {'apikey': myapikeyval, 'channels': newchan_to_check}
    params2 = json.dumps(params)

    # {"channels":"[212]","apikey":"blah"}  needs to become
    # {"channels":[212],"apikey":"blah"}    (e.g., no quotes around [chan])
    params2 = params2.replace('"[', '[')
    params2 = params2.replace(']"', ']')

    url = 'https://' + endpoint + '/siebatchd/v1/siebatch/chdetails'
    response = make_query(url, useproxy, params2, '-999')
    decoded_results = json.loads(response)

    earliest_time_string = \
        decoded_results['channels'][chan_with_prefix]['earliest']
    latest_time_string = \
        decoded_results['channels'][chan_with_prefix]['latest']
    size_string = \
        decoded_results['channels'][chan_with_prefix]['size']

    return (earliest_time_string, latest_time_string, size_string)

# ----------------------------------------------------------------------------
#
def fixup_ending_datetime_in_the_future():
    """ if the ending date is in the future, reel it back in! """
    # Replace future times with the current GMT time.
    # The following returns a datetime structure
    epochseconds = time.gmtime()
    enddatetime = time.strftime('%Y-%m-%d %H:%M:%S', epochseconds)
    enddatetime2 = re.sub(r'..$', '00', enddatetime)
    return enddatetime2

# ----------------------------------------------------------------------------
#
def string_fmt_time_to_seconds(string_format_time):
    """ utility function to convert a string format time to epoch seconds """
    dt = datetime.strptime(string_format_time, "%Y-%m-%d %H:%M:%S")
    epoch_seconds = calendar.timegm(dt.utctimetuple())
    return epoch_seconds

# -----------------------------------------------------------------------------
#
def check_channel(endpoint, useproxy, chanflagstring, startdatetime, \
    enddatetime):
    """ make sure that the channel is available and the dates are in-range """

    # get the available datetime range for this channel
    #pylint:disable=unused-variable
    (earliest_time_string, latest_time_string, chan_to_check) = \
        show_intervals(endpoint, useproxy, chanflagstring)
    #pylint:enable=unused-variable

    # convert the requested and available start datetimes into Un*x seconds
    requested_start_seconds = string_fmt_time_to_seconds(startdatetime)
    earliest_date_seconds = string_fmt_time_to_seconds(earliest_time_string)
    requested_stop_seconds = string_fmt_time_to_seconds(enddatetime)
    latest_date_seconds = string_fmt_time_to_seconds(latest_time_string)

    # start datetime must be earlier than stop date time
    if (requested_stop_seconds - requested_start_seconds) < 0:
        sys.exit('Start datetime must be earlier than stop datetime')

    # start datetime may not be earlier than earliest data available
    if (requested_start_seconds - earliest_date_seconds) < 0:
        sys.exit('Start datetime out of range. Must be no earlier than ' + \
            earliest_time_string)

    # end datetime may not be in the future
    if (requested_stop_seconds - latest_date_seconds) > 0:
        enddatetime = fixup_ending_datetime_in_the_future()

    return (startdatetime, enddatetime)

# ----------------------------------------------------------------------------
#
def validate_input_time_date_format(mydatetime):
    """ make sure the user has followed the required datetime format """
    # parameter is datetime to format check. if invalid, abort run.
    # if valid, return the validated (but unchanged) datetime (could skip
    # doing this for now, but at some point we might decide to fix up bad
    # string formatting as a convenience to the user, so...)

    # check the format with a regex
    if not(re.match(r'/\A\d{4}-\d{2}-\d{2}\ \d{2}:\d{2}:\d{2}\Z/', \
        mydatetime), mydatetime):
        print("bad starting time format -- must be \"YYYY-MM-DD HH:MM:SS\"\n")
        sys.exit(1)

    return mydatetime

# -----------------------------------------------------------------------------
#
def zero_unused_seconds(mydatetime):
    """ if seconds are non-zero in the time stamps, zero them out """
    # since SIE-Batch API does not care about seconds, we set them to zero
    mydatetime2 = re.sub(r'..$', '00', mydatetime)
    return mydatetime2

# -----------------------------------------------------------------------------
#
def convert_relative_times_to_real_datetimes(minutesback):
    """ one option is relative times; if we get one, make it a real time """
    # in relative format, the initial "ending time" is actually the minutes
    # worth of data we want to retrieve
    # the "real" ending datetime will be created from the current GMT time
    # we will be doing math on the epoch seconds

    myformat = '%Y-%m-%d %H:%M:%S'

    endingtime = time.gmtime()
    epochseconds = calendar.timegm(endingtime)

    # now compute the formatted ending date time in standard YYYY-MM-DD HH:MM:SS
    enddatetime = strftime(myformat, endingtime)

    # find just the seconds from that string
    extraseconds = int(enddatetime[-2:])
    # subtract the seconds from the full datetime to end up with 00 seconds
    endingtime_seconds = int(epochseconds) - extraseconds

    # let's now work on the starting time
    # we compute the "real" starting datetime by offsetting backwards
    # our to-be-modified datetime is in epoch seconds, so convert min to seconds
    mysecondsback = int(minutesback) * 60
    startseconds = endingtime_seconds - mysecondsback
    startdatetime = strftime(myformat, time.gmtime(startseconds))
    enddatetime = strftime(myformat, time.gmtime(endingtime_seconds))

    return (startdatetime, enddatetime)

# -----------------------------------------------------------------------------
#
def fix_times():
    """ handles calling the rest of the routines to fix up times """
    # arguments come in from the command line so we don't pass them in

    # chanflagstring = str(sys.argv[1])
    startdatetime = str(sys.argv[2])
    enddatetime = str(sys.argv[3])

    # if relative times, replace the ending time with the current GMT time
    # set the starting time back by the specified number of minutes
    if startdatetime == 'now':
        (startdatetime, enddatetime) = \
            convert_relative_times_to_real_datetimes(enddatetime)
    else:
        # we have real timedate stamps for starting and ending datetimes
        # process the starting datetime value...
        # correctly written datetime value?
        # also zero the seconds if present (SIE-Batch API doesn't use them)
        validate_input_time_date_format(startdatetime)
        startdatetime = zero_unused_seconds(startdatetime)

        # repeat for the ending datetime value...
        validate_input_time_date_format(enddatetime)
        enddatetime = zero_unused_seconds(enddatetime)
    return (startdatetime, enddatetime)

# ----------------------------------------------------------------------------
#
# https://stackoverflow.com/questions/1265665/how-can-i-check-if-a-string-represents-an-int-without-using-try-except
#
def isInt_try(v):
    """ convenience function to see if a string might be integer-ish """
    # pylint: disable=unused-variable,multiple-statements,bare-except
    try: i = int(v)
    except: return False
    return True

# -----------------------------------------------------------------------------
#
def build_filename(chanflagstring, startdatetime, enddatetime):
    """construct the filename from the command line arguments and return it"""

    string1 = startdatetime.replace(' ', '@')
    string2 = enddatetime.replace(' ', '@')

    nmsgchannels = ["204", "206", "207", "208", "221"]
    if chanflagstring in nmsgchannels:
        filetype = ".nmsg"
    else:
        filetype = ".jsonl"

    outputfilename = "sie-ch" + chanflagstring + "-{" + string1 + \
        "}-{" + string2 + "}" + filetype

    return outputfilename

# -----------------------------------------------------------------------------
#
def print_usage_info():
    """ deliver a succinct usage summary if needed """
    print('''
Usage:

  sie_get_py channel "now" minutesBack
  Example: sie_get_py 212 now 15

OR

  sie_get_py channel "startdatetime" "enddatetime"
  Example: sie_get_py 212 "2020-01-07 00:13:00" "2020-01-07 00:28:00"

Convenience functions:

  Check SIE-Batch API key:                      sie_get_py checkkey
  Get a listing of channels:                    sie_get_py channels
  Get datetime range and volume for a channel:  sie_get_py 212

Notes:

  Datetimes are UTC and must be quoted. (Current UTC datetime: $ date -u )
  Zero pad any single digit months, days, hours, minutes or seconds.
  Seconds must be entered as part of the UTC datetimes (but are ignored)
  Ending datetime in the future? It will be clamped to current datetime.
         ''')
    sys.exit(1)

# ----------------------------------------------------------------------------
#
def one_real_arg(endpoint, useproxy, first_arg):
    """ sometimes we only see one option on the command line; process it """
    defined_channels = {'24', '25', '27', '42', '80', '114', '115', \
        '204', '206', '207', '208', '211', '212', '213', '214', '221'}

    if first_arg == 'channels':
        # list channels for the user
        list_channels(endpoint, useproxy)
        sys.exit(0)

    elif first_arg == 'checkkey':
        # check the user's key for validity
        status = validateapikeyonline(endpoint, useproxy)
        print("API key status is "+status)
        sys.exit(0)

    elif (isInt_try(first_arg) and (first_arg in defined_channels)):
        # list details about the specified channel
        (earliest, latest, datasize) = show_intervals(endpoint, \
            useproxy, first_arg)
        format_and_printout_chan_time_limits(first_arg, earliest, \
            latest, datasize)
        sys.exit(0)

    elif (not(first_arg in defined_channels) and (isInt_try(first_arg))):
        # the requested channel is not one we offer, so...
        print("Channel not offered via this script")
        sys.exit(0)

    else:
        print_usage_info()
        sys.exit(0)

# ----------------------------------------------------------------------------
#
def three_real_args(endpoint, useproxy):
    """ other times we may see three arguments on the command line... """
    chanflagstring = str(sys.argv[1])
    (startdatetime, enddatetime) = fix_times()
    (startdatetime, enddatetime) = check_channel(endpoint, useproxy, \
        chanflagstring, startdatetime, enddatetime)

    outputfilename = build_filename(chanflagstring, startdatetime, enddatetime)
    myapikey = getkeyfromlocalfile()
    params = {"apikey": myapikey, "channel": int(chanflagstring), \
        "start_time": startdatetime, "end_time": enddatetime}
    params2 = json.dumps(params)
    queryURL = "https://" + endpoint + "/siebatchd/v1/siebatch/chfetch"
    make_query(queryURL, useproxy, params2, outputfilename)
    sys.exit(0)

# ============================================================================
# main

if len(sys.argv) == 1:
    print_usage_info()
    sys.exit(0)

elif len(sys.argv) >= 2:
    first_arg = sys.argv[1]

command_line_arg_count = len(sys.argv)-1

if command_line_arg_count == 1:
    one_real_arg(endpoint, useproxy, first_arg)
    sys.exit(0)

elif command_line_arg_count == 3:
    three_real_args(endpoint, useproxy)
    sys.exit(0)

elif (command_line_arg_count <= 0) or (command_line_arg_count >= 4) or \
    (command_line_arg_count == 2):
    print_usage_info()
    sys.exit(0)
