import asyncio
import datetime
import errno
import os
from pathlib import Path
import socket

# https://pypi.org/project/pyppeteer2/
# https://miyakogi.github.io/pyppeteer/reference.html
# One time, download Chromium: $ pyppeteer-install
from pyppeteer import *

import tkinter as tk
from tkinter.messagebox import *

def display_error():
    tk.messagebox.showerror(message=\
        "Couldn't get FQDN.\nTypo in the FQDN entered?",
        title="Message Box", icon="error")

async def scrapeAFQDN(fqdn, my_log_box, url_or_file):
    # the snapshot goes to this filespec
    # if on something non-Un*x-ish, remember os.path.join(dir, f)
    my_log_box.insert(tk.END, "Making sure archive directory exists...\n")
    my_log_box.update()
    home = str(Path.home())
    subdir = "snapshots"
    myfilename = fqdn
    utcdatetime = datetime.datetime.utcnow().isoformat() + 'Z'
    myextension = "jpeg"
    mydir = home + "/" + subdir 
    fullfilenamepart = fqdn + "_" + utcdatetime + "." + myextension
    latestversion = fqdn + "_" + "latest" + "." + myextension

    # ensure the directory exists
    try:
        os.makedirs(mydir)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

    # ensure the timestamped file exists
    if (url_or_file == "url"):
        myfilespec = mydir + "/" + fullfilenamepart
        # ensure the directory exists
        try:
            os.makedirs(mydir)
        except OSError as e:
            if e.errno != errno.EEXIST:
                raise
    elif (url_or_file == "file"):
        myfilespec = fullfilenamepart
    try: 
        open(myfilespec, 'a').close()
    except:
        raise

    my_log_box.insert(tk.END, "Confirming page is reachable on 443 or 80...\n")
    my_log_box.update()

    domain_resolve_ok = False
    if (url_or_file == "url"):
        # verify page exists, and figure out if https is supported
        try:
            # note: you do NOT need to explicitly permit inbound traffic 
            # on macOS (even if it asks you to allow it!)
            myaddrinfo = socket.getaddrinfo(fqdn, 443)
            url = "https://"+fqdn
            domain_resolves_ok = True
        except socket.gaierror:
            # couldn't connect on default SSL/TLS port
            # fall back to regular http
            try:
                myaddrinfo = socket.getaddrinfo(fqdn, 80)
                url = "http://"+fqdn
                domain_resolves_ok = True
            except:
                # couldn't even connect on standard port
                display_error()
            return(1)
    elif (url_or_file == "file"):
        domain_resolves_ok = True
        url = "file://"+fqdn
        myfilespec = fqdn + ".jpeg"

    if domain_resolves_ok == True:
        browser = await launch({'headless': True, 'ignoreHTTPSErrors': True,\
            'defaultViewport': None, 'viewport_width': 800})
        context = await browser.createIncognitoBrowserContext()
        page = await browser.newPage()
        await page._client.send('Animation.disable')
        await page.goto(url)
        await page.screenshot({'path': myfilespec, 'type': 'jpeg', \
            'quality': 80, 'fullPage': True})
        await browser.close()

        # ensure the "latest" version is updated for this URL
        if (url_or_file == "url"):
            mylatest = mydir + "/" + latestversion
            # print("mylatest =" + mylatest)
        elif (url_or_file == "file"):
            mylatest = myfilespec + "_" + "latest" + "." + myextension
        try: 
            os.unlink(mylatest)
        except:
            pass
        os.symlink(myfilespec, mylatest)
    
        return(myfilespec)
