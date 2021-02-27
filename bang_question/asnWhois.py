import datetime
import errno
import socket
import os
from pathlib import Path
import dns.resolver
from lxml import etree
import requests

def confirmDirExists(mydir):
    try:
        os.makedirs(mydir)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

def confirmFileExists(myfilespec):
    try:
        open(myfilespec, 'a').close()
    except:
        raise

def makeLink(myfilespec, mylatest):
    try:
        os.unlink(mylatest)
    except:
        pass
    os.symlink(myfilespec, mylatest)

def fqdnToIP(fqdn):
    myip = socket.gethostbyname(fqdn)
    return(myip)

def reverseIPforRouteviews(myip):
    reversed_ip = ".".join(reversed(myip.split('.')))+".asn.routeviews.org"
    return(reversed_ip)

def getASNfromRouteviews(reversed_ip):
    answers = dns.resolver.resolve(reversed_ip, 'TXT')
    split_answers=answers.response.answer[0].to_text().split(" ")
    myasn=split_answers[4]
    myasn=myasn.replace('"','')
    return(myasn)

def makeOutputFile(fqdn, outputfiletype):
    home = str(Path.home())
    subdir = "asnwhois_output"
    utcdatetime = datetime.datetime.utcnow().isoformat() + 'Z'

    if (outputfiletype == "raw"):
        myextension = "xml"
    elif (outputfiletype == "cooked"):
        myextension = "html"

    mydir = home + "/" + subdir
    # if on something non-Un*x-ish, remember os.path.join(dir, f)
    fullfilenamepart = fqdn + "_" + utcdatetime + "." + myextension
    latestversion = fqdn + "_" + "latest" + "." + myextension

    # esure the directory exists
    confirmDirExists(mydir)

    # ensure the timestamped file exists
    myfilespec = mydir + "/" + fullfilenamepart
    confirmFileExists(myfilespec)

    # set up a convenience link to the latest version
    mylatest = mydir + "/" + latestversion
    makeLink(myfilespec, mylatest)

    return(myfilespec)

def myAsnWhois(fqdn):
    # we need the IP of the FQDN to map to an ASN
    myip = fqdnToIP(fqdn)

    # get the domain we need to get the ASN from Routeviews
    reversed_ip = reverseIPforRouteviews(myip)

    # do IP-->ASN using Routeviews
    myasn = getASNfromRouteviews(reversed_ip)

    # Joint Whois Project allows all queries to go to a single
    # whois server which will redirect as appropriate, see
    # https://www.lacnic.net/1040/2/lacnic/lacnics-whois (we'll use ARIN)

    # now assemble the query URL
    myurl = "https://whois.arin.net/rest/asn/" + myasn + "/pft?s=" + myasn

    # create the output file to hold the ASN Whois Information
    myfilespec = makeOutputFile(fqdn, "raw")

    # If using requests instead of pycurl
    headers = {'Accept' : 'application/xml'}
    response = requests.get(myurl, headers=headers)

    # Throw an error for bad status codes
    response.raise_for_status()

    # if we want the response in unicode, use response.text below
    # if we want the response in bytes, use response.content instead

    # write the results to a file
    with open(myfilespec, "wb") as my_file:
        my_file.write(response.content)

    # IMPORTANT NOTE: we're using a saved copy of XSLT because ARIN shows
    # https://www.w3.org/1999/XSL/Transform BUT THERE SHOULD BE NO "s" there
    # (e.g., the URI MUST BE regular http not https). If this isn't fixed, 
    # etree.XSLT will indicated that no stylesheet exists. A subtle bug...
    # FWIW, the Oxygen XML Editor immediately found the issue, very impressive!

    XSLT_file = str(Path.home()) + "/.bang_question_files/" + "./website.xsl"
    transform = etree.XSLT(etree.parse(XSLT_file))
    result = transform(etree.parse(myfilespec))
    my_transformed_results = etree.tostring(result, pretty_print=True)

    cooked_file = makeOutputFile(fqdn, "cooked")

    with open(cooked_file, "wb") as outfile:
        outfile.write(my_transformed_results)

    return(cooked_file)
