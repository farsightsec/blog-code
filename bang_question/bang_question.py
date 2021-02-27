#!/usr/local/bin/python3
# You MUST install Python 3.9.1 (or later) on macOS Big Sur
# (see https://www.python.org/downloads/release/python-391/ )

import asyncio
import errno
import json
import os
from pathlib import Path
import socket
import sys

from datetime import datetime
import datetime

import tkinter as tk
from tkinter import NW, TOP, LEFT, END
from tkinter.messagebox import *

from tkinter.ttk import *
import tkinter.ttk as ttk

import PIL
from PIL import Image, ImageTk

# https://pypi.org/project/pyppeteer2/
# https://miyakogi.github.io/pyppeteer/reference.html
# One time, download Chromium: $ pyppeteer-install
# from pyppeteer import *

### This site or product includes IP2Location LITE data available
### from https://lite.ip2location.com
### We assume that a local copy of IP2LOCATION-LITE-DB9.IPV6.BIN
### has been downloaded and installed. (That file is for both V4 & V6)
import IP2Location

import plotly.express as px
import kaleido

import pandas as pd

import pyasn

from ipwhois import IPWhois

# https://github.com/DannyCork/python-whois
import whois

# https://pypi.org/project/PyMuPDF/
# https://pymupdf.readthedocs.io/en/latest/
import fitz

# make sure you don't include the .py as part of the name of the imports
from scrapePage import scrapeAFQDN
from asnWhois import myAsnWhois
from dnsdbRun import doQuery
from draw_network_graphviz import draw_the_graph
# from draw_network_graph import draw_the_graph

################################# Globals ###################################

global root, mywindow, mynotebook, mymenubarbox, mymenubarlabel, my_log_box
global fqdn, FQDN, myip, buttona, buttonb, buttonc, mymapname9
global t1, t2, t3, t4, t5, t6, t7, t8, t9, t10
global background2, img4, t10img

################################ Functions ##################################

# https://stackoverflow.com/questions/56043767/show-large-image-using-scrollbar-in-python/56043976
class ScrollableImage(tk.Frame):
    def __init__(self, master=None, **kw):
        self.image = kw.pop('image', None)
        sw = kw.pop('scrollbarwidth', 10)
        super(ScrollableImage, self).__init__(master=master, **kw)
        self.cnvs = tk.Canvas(self, highlightthickness=0, **kw)
        self.cnvs.create_image(0, 0, anchor='nw', image=self.image)
        # Vertical and Horizontal scrollbars
        self.v_scroll = tk.Scrollbar(self, orient='vertical', width=sw)
        self.h_scroll = tk.Scrollbar(self, orient='horizontal', width=sw)
        # Grid and configure weight.
        self.cnvs.grid(row=0, column=0,  sticky='nsew')
        self.h_scroll.grid(row=1, column=0, sticky='ew')
        self.v_scroll.grid(row=0, column=1, sticky='ns')
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        # Set the scrollbars to the canvas
        self.cnvs.config(xscrollcommand=self.h_scroll.set, 
                           yscrollcommand=self.v_scroll.set)
        # Set canvas view to the scrollbars
        self.v_scroll.config(command=self.cnvs.yview)
        self.h_scroll.config(command=self.cnvs.xview)
        # Assign the region to be scrolled 
        self.cnvs.config(scrollregion=self.cnvs.bbox('all'))
        self.cnvs.bind_class(self.cnvs, "<MouseWheel>", self.mouse_scroll)

    def mouse_scroll(self, evt):
        if evt.state == 0 :
            self.cnvs.yview_scroll(-1*(evt.delta), 'units') # For MacOS
            self.cnvs.yview_scroll(int(-1*(evt.delta/120)), 'units') # For windows
        if evt.state == 1:
            self.cnvs.xview_scroll(-1*(evt.delta), 'units') # For MacOS
            self.cnvs.xview_scroll(int(-1*(evt.delta/120)), 'units') # For windows

### Callback for submit button
def funca(event=None):
    global fqdn, mymapname9, myip, my_log_box, FQDN
    global background2, img, img2, img4, t10img
    global t1, t2, t3, t4, t5, t6, t7, t8, t9, t10

    # we log events as we have them to report...
    # clear any existing log entries
    my_log_box.delete('1.0', END)

    my_log_box.insert(tk.END, "New run initiated\n")
    my_log_box.update()

    # callback for Submit button -- FQDN field should be filled in now
    fqdn = FQDN.get()

    my_log_box.insert(tk.END, "The FQDN entered was "+fqdn+"\n")
    my_log_box.update()

    # ------------------------------------------------------------------

    my_log_box.insert(tk.END, "About to do screen grab of FQDN...\n")
    my_log_box.update()

    # check the supplied domain, and try to grab a copy of the specified page
    mypage = asyncio.get_event_loop().run_until_complete\
        (scrapeAFQDN(fqdn, my_log_box, "url"))

    # there was a problem scraping the page
    if (str(mypage) == "1"):
        return(1)

    my_log_box.insert(tk.END, "  Screen grab done. Grabbed page is\n  "+\
        str(mypage)+"\n")
    my_log_box.update()

    try:
        t2.destroy()
    except:
        pass

    t2 = tk.Frame()
    mynotebook.add(t2, text="Screen Grab")
    img2 = ImageTk.PhotoImage(Image.open(mypage))
    image_window = ScrollableImage(t2, image=img2, 
        width=800, height=600)
    image_window.pack(anchor=NW)
    image_window.update()

    my_log_box.insert(tk.END, "  Screen grab added to tab.\n")
    my_log_box.update()

    # ------------------------------------------------------------------

    my_log_box.insert(tk.END, "Running DNSDB RRname query now...\n")
    my_log_box.update()

    my_fqdn_results_json = doQuery(fqdn, "full")

    try:
        t3.destroy()
    except:
        pass

    t3 = tk.Frame()
    mynotebook.add(t3, text="DNSDB RRnames")

    # Text Widget height and width are in characters
    mytext_widget_pdns = tk.Text(t3,height=40,width=132)
    mytext_widget_pdns.pack(side=TOP, anchor=NW)
    mytext_widget_pdns.update()

    mytext_widget_pdns.insert(tk.END, my_fqdn_results_json)
    my_log_box.insert(tk.END, "  Passive DNS results added to tab.\n")
    my_log_box.update()

 # ------------------------------------------------------------------

    my_log_box.insert(tk.END, "Running DNSDB Rdata IP query now...\n")
    my_log_box.update()

    myip = socket.gethostbyname(fqdn)
    my_ip_results_json = doQuery(myip, "RdataIP")

    try:
        t4.destroy()
    except:
        pass

    t4 = tk.Frame()
    mynotebook.add(t4, text="DNSDB IP Rdata")

    # Text Widget height and width are in characters
    mytext_widget_pdns_2 = tk.Text(t4,height=40,width=132)
    mytext_widget_pdns_2.pack(side=TOP, anchor=NW)
    mytext_widget_pdns_2.update()

    mytext_widget_pdns_2.insert(tk.END, my_ip_results_json)
    my_log_box.insert(tk.END, "  Passive DNS Rdata IP results added to tab.\n")
    my_log_box.update()

    # ------------------------------------------------------------------

    my_log_box.insert(tk.END, "Now working on Domain WHOIS...\n")
    my_log_box.update()

    domain = whois.query(fqdn)
    domain_json = json.dumps(domain.__dict__, indent=4, default=str)

    try:
        t5.destroy()
    except:
        pass

    t5 = tk.Frame()
    mynotebook.add(t5, text="DomWhois")
    mytext_widget_5 = tk.Text(t5,height=40,width=132)
    mytext_widget_5.pack(side=TOP, anchor=NW)
    mytext_widget_5.insert(tk.END, domain_json)

    my_log_box.insert(tk.END, "  Domain WHOIS results added to tab.\n")
    my_log_box.update()

    # ------------------------------------------------------------------

    my_log_box.insert(tk.END, "Now working on IP WHOIS...\n")
    my_log_box.update()

    myip = socket.gethostbyname(fqdn)
    obj = IPWhois(myip)
    res=obj.lookup_whois(get_referral=True)
    pretty_printed_text = json.dumps(res, indent=4)

    try:
        t6.destroy()
    except:
        pass

    t6 = tk.Frame()
    mynotebook.add(t6, text="IPWhois")
    mytext_widget_6 = tk.Text(t6,height=40,width=132)
    mytext_widget_6.pack(side=TOP, anchor=NW)
    mytext_widget_6.insert(tk.END, pretty_printed_text)

    my_log_box.insert(tk.END, "  IP WHOIS results added to tab.\n")
    my_log_box.update()

    # ------------------------------------------------------------------

    # get the basic ASN whois info
    my_log_box.insert(tk.END, "Now working on ASN WHOIS results...\n")
    my_log_box.update()

    # scraping the ASN Whois 
    my_asn_info_file = myAsnWhois(fqdn)
    myasnpage = asyncio.get_event_loop().run_until_complete\
        (scrapeAFQDN(my_asn_info_file, my_log_box, "file"))

    try:
        t7.destroy()
    except:
        pass

    t7 = tk.Frame()
    mynotebook.add(t7, text="ASNWhois")
    asnimg7 = ImageTk.PhotoImage(Image.open(myasnpage))
    image_window = ScrollableImage(t7, image=asnimg7,
        width=1000, height=600)
    image_window.pack(anchor=NW)
    image_window.update()

    my_log_box.insert(tk.END, "  ASN Whois info added to tab."+"\n")
    my_log_box.update()

    # --------------------------------------------------------------

    my_log_box.insert(tk.END, "Now working on Adding prefixes...\n")
    my_log_box.update()

    # we need the ASN for some of the filenames, so we'll get that first
    myip2asnfilspec = str(Path.home()) + "/.bang_question_files/" + \
        "my_ip2asn_db_file"
    asndb = pyasn.pyasn(myip2asnfilspec)
    asn = asndb.lookup(myip)

    # get the prefixes associated with the ASN
    prefixes = asndb.get_as_prefixes(asn[0])
    prefixes = sorted(prefixes)

    combined_text=""
    for pre in prefixes:
        combined_text=combined_text+"\n"+pre

    try:
        t8.destroy()
    except:
        pass

    t8 = tk.Frame()
    mynotebook.add(t8, text="Prefixes")
    mytext_widget_8_prefixes = tk.Text(t8,height=40,width=132)
    mytext_widget_8_prefixes.pack(side=TOP, anchor=NW)
    mytext_widget_8_prefixes.insert(tk.END, "AS"+str(asn[0])+" Prefixes:\n")
    mytext_widget_8_prefixes.insert(tk.END, combined_text)

    my_log_box.insert(tk.END, "  ASN Prefixes added to tab.\n")
    my_log_box.update()

    # ------------------------------------------------------------------

    my_log_box.insert(tk.END, "Now working on geolocation..\n")
    my_log_box.update()

    home = str(Path.home())
    subdir9 = "geolocation-maps"
    myfilename9 = fqdn
    utcdatetime9 = datetime.datetime.utcnow().isoformat() + 'Z'
    myextension9 = "jpeg"
    mymapextension9 = "png"
    mydir9 = home + "/" + subdir9
    fullfilenamepart9 = fqdn + "_" + utcdatetime9 + "." + myextension9
    latestversion9 = fqdn + "_" + "latest" + "." + myextension9
    mymapname9 = mydir9 + "/" + fqdn + "_" + utcdatetime9 + "." + \
        mymapextension9
    myfilespec9 = mydir9 + "/" + fullfilenamepart9

    # ensure the directory exists
    try:
        os.makedirs(mydir9)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise
    try:
        open(myfilespec9, 'a').close()
    except:
        raise

    try:
        open(mymapname9, 'a').close()
    except:
        raise

    geoipfilespec = str(Path.home()) + "/.bang_question_files/" + \
        "IP2LOCATION-LITE-DB9.IPV6.BIN"
    database = IP2Location.IP2Location(geoipfilespec, "SHARED_MEMORY")
    rec = database.get_all(myip)
    lat = rec.latitude
    lon = rec.longitude
    cit = rec.city

    # map it (we need to make it into a DataFrame first
    d=[{"txt":cit,"lat":lat,"lon":lon,"siz":10}]
    df=pd.DataFrame(d,columns=["txt","lat","lon","siz"])

    if ((lon >= -180) and (lon <= -52) and
        (lat >= 36)   and (lat <=  83)):
        fig=px.scatter_geo(df,lat="lat",lon="lon",text="txt",\
            size="siz",projection="albers usa",width=800,height=600)
    else:
        fig=px.scatter_geo(df,lat="lat",lon="lon",text="txt",\
            size="siz",projection="mercator",width=800,height=600)

    fig.write_image(mymapname9)

    # ensure the image is appropriately sized
    map_img=Image.open(mymapname9).resize((800, 600),Image.ANTIALIAS)
    map_img.load()

    # BUG: https://stackoverflow.com/questions/42099914/imagetk-photoimage-doesnt-show-up-on-osx-but-does-on-windows
    # See https://stackoverflow.com/questions/41576637/are-rgba-pngs-unsupported-in-python-3-5-pillow
    # See Yuji Tomita's post at https://stackoverflow.com/questions/9166400/convert-rgba-png-to-rgb-with-pil
    background2 = Image.new("RGB", map_img.size, (255,255,255))
    background2.paste(map_img, mask=map_img.split()[3]) # 3 is the alpha channel
    background2.save(myfilespec9, 'JPEG', quality=80)
    img4 = ImageTk.PhotoImage(background2)

    try:
        t9.destroy()
    except:
        pass

    t9 = tk.Frame()
    mynotebook.add(t9, text="GeoIP")
    panel2 = tk.Label(t9, image=img4)
    panel2.pack(side=TOP, anchor=NW)
    mynotebook.add(t9)

    my_log_box.insert(tk.END, "  Geolocation map added to tab.\n")
    my_log_box.update()

    # ------------------------------------------------------------------

    my_log_box.insert(tk.END, "Working on network graph...\n")
    my_log_box.update()

    subdir10 = "network-graphs"
    myfilename10 = fqdn
    utcdatetime10 = datetime.datetime.utcnow().isoformat() + 'Z'
    myextension10 = "pdf"
    mydir10 = home + "/" + subdir9
    # fullfilenamepart10 = fqdn + "_" + utcdatetime10 + "." + myextension10
    fullfilenamepart10 = fqdn + "_" + utcdatetime10 
    fullfilenamepart10=fullfilenamepart10.rstrip()
    # fullfilenamepart10=fullfilenamepart10.replace(" ","")
    # fullfilenamepart10=fullfilenamepart10.replace(":","H",1)
    # fullfilenamepart10=fullfilenamepart10.replace(":","M",1)
    # fullfilenamepart10=fullfilenamepart10.replace("-","_")
    # fullfilenamepart10=fullfilenamepart10.replace("[","")
    # fullfilenamepart10=fullfilenamepart10.replace("]","")

    myfilespec10 = mydir10 + "/" + fullfilenamepart10
    latestversion10 = fqdn + "_" + "latest" + "." + myextension10

    # ensure the directory exists
    try:
        os.makedirs(mydir10)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

    try:
        open(myfilespec10, 'a').close()
    except:
        raise

    draw_the_graph(doQuery(fqdn, "limited"), myfilespec10)
    fitzname=myfilespec10+".pdf"
    doc=fitz.open(fitzname)
    page=doc.loadPage(0)
    pix=page.getPixmap()
    mode="RGB"

    try:
        t10.destroy()
    except:
        pass

    t10 = tk.Frame()
    mynotebook.add(t10, text="Graph")
    t10img=Image.frombytes(mode,[pix.width, pix.height], pix.samples)
    t10tkimg=ImageTk.PhotoImage(t10img)
    t10_image_window = ScrollableImage(t10, image=t10tkimg,
        width=800, height=600)
    t10_image_window.pack(anchor=NW)

    my_log_box.insert(tk.END, "  Network graph added to tab.\n")
    my_log_box.update()

    my_log_box.insert(tk.END, "--DONE--\n")
    my_log_box.update()

def cleanup_tab(tab_to_cleanup):
    try:
        tab_to_cleanup.destroy()
    except:
        pass

def funcb(event=None):
    global FQDN, my_log_box
    global t1, t2, t3, t4, t5, t6, t7, t8, t9, t10

    FQDN.delete(0, 'end')

    my_log_box.delete('1.0', END)
    my_log_box.update()

    # we intentionally clean these up in reverse order and leave t1 alone
    my_tabs = [t10, t9, t8, t7, t6, t5, t4, t3, t2]
    for i in my_tabs:
        cleanup_tab(i)

### Callback for quit button
def funcc(event=None):
    sys.exit()

#################################### MAIN #####################################

def main():
    root = tk.Tk()
    root.title("bang_question")
    iconfilespec = str(Path.home()) + "/.bang_question_files/" + \
        "exclamationquestion.gif"
    root.tk.call('wm','iconphoto',root._w,ImageTk.PhotoImage(file=iconfilespec))
    s = ttk.Style()
    s.configure('TNotebook', tabposition='nw')
    s.theme_use('clam')
    root.bind('<Return>', funca)
    mywindow = tk.Frame(root)
    
    # create notebook
    global mynotebook
    mynotebook = ttk.Notebook(mywindow)
    mynotebook.pack(side=TOP, anchor=NW)
    
    t1 = tk.Frame()
    mynotebook.add(t1, text="MAIN")
    
    # build menu box
    mymenubarbox = ttk.Frame(t1)
    mymenubarbox.pack(side=TOP, anchor=NW)
    mymenubarlabel = ttk.Label(mymenubarbox, text="    Enter FQDN:")
    mymenubarlabel.pack(side=LEFT)
    global FQDN
    FQDN = ttk.Entry(mymenubarbox)
    FQDN.pack(side=LEFT)
    buttona = tk.Button(mymenubarbox, text='Submit', command = funca)
    buttona.pack(side=LEFT)
    buttonb = tk.Button(mymenubarbox, text='Clear',  command = funcb)
    buttonb.pack(side=LEFT)
    buttonc = tk.Button(mymenubarbox, text='Quit',   command = funcc)
    buttonc.pack(side=LEFT)
    mymenubarbox.pack(side=TOP, anchor=NW)
    
    # build log box
    global my_log_box
    my_log_box = tk.Text(t1,height=40,width=132)
    my_log_box.pack(side=TOP, anchor=NW)
    
    # add tab #1 to notebook
    mynotebook.add(t1, text="MAIN")
    mynotebook.pack(side=TOP, anchor=NW, expand=True)
    
    mywindow.pack(side=TOP, anchor=NW)
    mywindow.mainloop()

if __name__ == "__main__":
    # execute only if run as a script
    main()
