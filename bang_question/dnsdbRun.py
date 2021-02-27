from pathlib import Path
from io import BytesIO
import json
import re
import sys
from time import strftime, gmtime

# import pycurl
import requests

# See stackoverflow.com/questions/26924812/python-sort-list-of-json-by-value
def extract_time(myrecord):
    json_format=eval(myrecord)

    try:
        extracted_bit = json_format['obj']['time_last']
    except:
        extracted_bit = json_format['obj']['zone_time_last']

    return extracted_bit

def print_bits(myrecord):
    myformat = '%Y-%m-%d %H:%M:%S'
    myrecord_json_format = json.loads(myrecord)
    extract_bit = myrecord_json_format['obj']['rrname']

    extract_bit_2 = myrecord_json_format['obj']['rrtype']
    extract_bit_2 = str('{0:<5}'.format(extract_bit_2))

    temp_bit_3 = myrecord_json_format['obj']['rdata']
    extract_bit_3 = json.dumps(temp_bit_3)

    try:
        extract_tl = myrecord_json_format['obj']['time_last']
    except:
        extract_tl = myrecord_json_format['obj']['zone_time_last']

    tl_datetime = gmtime(extract_tl)
    enddatetime = strftime(myformat, tl_datetime)

    try:
        extract_tf = myrecord_json_format['obj']['time_first']
    except:
        extract_tf = myrecord_json_format['obj']['zone_time_first']

    tf_datetime = gmtime(extract_tf)
    startdatetime = strftime(myformat, tf_datetime)

    extract_count = myrecord_json_format['obj']['count']
    formatted_count = str('{:>11,d}'.format(extract_count))
    results = extract_bit + " " + extract_bit_2 + " \"" + enddatetime + \
        "\" \"" + startdatetime + "\" " + formatted_count + \
        " " + extract_bit_3

    if (results.find("SOA") == -1):
        return results
    else:
        return ""

def print_rdata_bits(myrecord):
    myformat = '%Y-%m-%d %H:%M:%S'
    myrecord_json_format = json.loads(myrecord)
    extract_bit = myrecord_json_format['obj']['rrname']
    extract_bit = str('{0:<50}'.format(extract_bit))

    extract_bit_2 = myrecord_json_format['obj']['rrtype']
    extract_bit_2 = str('{0:<5}'.format(extract_bit_2))

    temp_bit_3 = myrecord_json_format['obj']['rdata']
    extract_bit_3 = json.dumps(temp_bit_3)

    try:
        extract_tl = myrecord_json_format['obj']['time_last']
    except:
        extract_tl = myrecord_json_format['obj']['zone_time_last']

    tl_datetime = gmtime(extract_tl)
    enddatetime = strftime(myformat, tl_datetime)

    try:
        extract_tf = myrecord_json_format['obj']['time_first']
    except:
        extract_tf = myrecord_json_format['obj']['zone_time_first']

    tf_datetime = gmtime(extract_tf)
    startdatetime = strftime(myformat, tf_datetime)

    extract_count = myrecord_json_format['obj']['count']
    formatted_count = str('{:>11,d}'.format(extract_count))
    results = extract_bit + " " + extract_bit_2 + " \"" + enddatetime + \
        "\" \"" + startdatetime + "\" " + formatted_count + \
        " " + extract_bit_3

    unwanted_name_found = False
    unwanted_rrnames = r'(.*\.verteiltesysteme.net\.$|.*\.eslared\.org\.ve\.$|.*\.usac\.edu\.gt\.$)'
    if re.match(unwanted_rrnames, extract_bit):
        unwanted_name_found = True

    if ((results.find("SOA") == -1) and (unwanted_name_found == False)):
        return results
    else:
        return ""

def print_limited_bits(myrecord):

    myrecord_json_format = json.loads(myrecord)
    extract_bit = myrecord_json_format['obj']['rrname']

    extract_bit_2 = myrecord_json_format['obj']['rrtype']
    # extract_bit_2 = str('{0:<5}'.format(extract_bit_2))

    temp_bit_3 = myrecord_json_format['obj']['rdata']
    extract_bit_3 = json.dumps(temp_bit_3)
    extract_bit_3 = extract_bit_3.replace(' ','')

    results = extract_bit + " " + extract_bit_2 + " " + extract_bit_3

    rrtypes = r'^(A|AAAA|CNAME|NS)$'
 
    if re.match(rrtypes,extract_bit_2):
        return results
    else:
        return ""

def make_query(fqdn, query_type):
    # get the DNSDB API key
    filepath = str(Path.home()) + "/.dnsdb-apikey.txt"
    with open(filepath) as stream:
        myapikey = stream.read().rstrip()

    url = ""

    if (query_type == "RRname"):
        url = "https://api.dnsdb.info/dnsdb/v2/lookup/rrset/name/" + fqdn
    elif (query_type == "RdataIP"):
        url = "https://api.dnsdb.info/dnsdb/v2/lookup/rdata/ip/" + fqdn
    elif (query_type == "RdataName"):
        url = "https://api.dnsdb.info/dnsdb/v2/lookup/rdata/name/" + fqdn

    headers = {'X-API-Key' : myapikey, 'Accept' : 'application/jsonl'}

    response = requests.get(url, headers=headers)

    if response.status_code == 200:
        return response.text
    else:
        return response.status_code

def doQuery(fqdn, full_or_limited):

    if ((full_or_limited == "limited") or (full_or_limited == "full")):
        content = make_query(fqdn, "RRname")
    elif (full_or_limited == "RdataIP"):
        content = make_query(fqdn, "RdataIP")
    else:
        sys.exit(0)

    try:
        test = int(content)
        print("Error making dnsdb query! Return code = "+str(test))
        sys.exit(0)
    except:
        sList = list(line for line in content.strip().split("\n"))

        # we want to dump the first line in that output 
        if sList[0] == '{"cond":"begin"}': 
            sList.pop(0)
        else: 
            print("SOMETHING ODD HAPPENED POPPING THE FIRST ELEMENT")
    
        if  ((sList[-1] == '{"cond":"succeeded"}') or 
            (sList[-1] == '{"cond":"limited","msg":"Result limit reached"}')):
            sList.pop()
        else:
            print("SOMETHING ODD HAPPENED POPPING THE LAST ELEMENT")

        sList2 = sorted(sList, key=extract_time, reverse=True)

        formatted_output=""
        results=""
        for line in sList2:
            if (full_or_limited == "full"):
                results=print_bits(line)
            elif (full_or_limited == "limited"):
                results=print_limited_bits(line)
            elif (full_or_limited == "RdataIP"):
                 results=print_rdata_bits(line)

            if results != "":
                result_with_nl=results+"\n"
                formatted_output=formatted_output+result_with_nl

        if len(formatted_output) == 0:
            formatted_output = "No results found\n"

        return(formatted_output)

