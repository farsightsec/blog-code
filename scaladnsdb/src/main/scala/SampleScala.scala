package dnsdbexample

import java.io._
import java.net._
import java.text._
import java.util._
import javax.crypto.Cipher
import javax.swing._
import javax.swing.JFrame._
import scala._
import scala.io._
import scala.swing._
import swing._
import swing.event._
import Swing._

// -----------------

class UIoutput(var frameTitle: String, fileLocation: String, MyBoxColor: Color)
    extends Frame {

  // Create a new wide and short window for posting one-line log-like info
  // (can be closed without affecting the main window)

  preferredSize = new Dimension(760, 60)
  setDefaultLookAndFeelDecorated(true)

  title = frameTitle // passed in as a parameter

  contents = new BoxPanel(Orientation.Horizontal) {
    // Adding the BoxPanel lets me left-justify the label
    background = MyBoxColor // passed in as a parameter
    contents += Swing.HStrut(10) // preserve a left margin

    contents += new Label {
      text = fileLocation // passed in as a parameter
      val myFont = "Monospaced"
      val myFontSize = 16
      font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
    }

    contents += Swing.HGlue // left justify
    contents += Swing.HStrut(10) // preserve a right margin
  }
}

// -----------------

class UI extends MainFrame {
  // Our main window. Closing it will kill the application

  title = "DNSDB API DEMO CLIENT IN Scala"

  setDefaultLookAndFeelDecorated(true)

  val myFont = "Monospaced"
  System.setProperty("awt.font", myFont)

  val myFontSize = 16

  var myFontTitleSize = (myFontSize * 1.5)

  // this is a light grayish color for the background
  val c537 = new Color(187, 199, 214)

  // this is a light green color for suggesting success
  val greenTint = new Color(204, 255, 179)

  // this is a light orange color for suggesting potential problems
  val orangeTint = new Color(255, 165, 0)

  // this is a light red color for suggesting a problem
  val redTint = new Color(255, 64, 0)

  // GUI spacing factors
  val borderMargin = 20
  val verticalSpaceBetweenItems = 10
  val bigVerticalSpaceBetweenItems = 30

  // popup window's inital location -- toward the top, over a ways
  var xcoord: Int = 400
  var ycoord: Int = 50

  // save these values for when we wrap around eventually
  val resetxcoord = xcoord
  val resetycoord = ycoord

  // set limits for max x and y position
  val xcoordMax: Int = 600
  val ycoordMax: Int = 600

  // bump per iteration
  val bumpIt: Int = 25

  // End of line in Unix = \n
  // End of line in Windows = \r\n
  // MS Word and WordPad can cope with \n, but the default Windows text
  // file editor, Notepad, fails to cope, so let's do the right thing here
  val nl = System.getProperty("line.separator").toString();

  var limit0: String = ""
  var remaining0: String = ""

  // DNSDB domain or IP
  object myinput extends swing.TextField {
    columns = 24
    maximumSize = new Dimension(32, 24)
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  // Bailiwick (used for RRname only)
  object myinput2 extends swing.TextField {
    columns = 24
    maximumSize = new Dimension(32, 24)
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  // Record data (used for Rdata only)
  object myinput3 extends swing.TextField {
    columns = 24
    maximumSize = new Dimension(32, 24)
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  // Max results to return
  object maxresults extends swing.TextField {
    columns = 7
    maximumSize = new Dimension(5, 24)
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  // Timefence
  object gobackhowmanydays extends swing.TextField {
    columns = 5
    maximumSize = new Dimension(5, 24)
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  // .... Radio button group

  val rrname = new RadioButton {
    text = "RRname"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  val rdata = new RadioButton {
    text = "Rdata"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  // only one or the other button can be checked
  val mutex = new swing.ButtonGroup(rrname, rdata)

  // .... Radio button group ends ....

  // .... Radio button group

  val textformat = new RadioButton {
    text = "Text"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
    selected = true
  }

  val jsonformat = new RadioButton {
    text = "JSON"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  val mutex2 = new swing.ButtonGroup(textformat, jsonformat)

  // .... Radio button group ends ....

  // .... Radio button group

  val nameinputmode = new RadioButton {
    text = "Name"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  val ipOrNetworkMode = new RadioButton {
    text = "IP, CIDR or Range"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  val notSet = new RadioButton {
    text = "(neither)"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
    selected = true
  }

  val mutex4 = new swing.ButtonGroup(nameinputmode, ipOrNetworkMode, notSet)

  // .... Radio button group ends ....

  // now do the drop down box to allow us to select the recordtype we want
  val rectype = new swing.ComboBox(
    scala.List("ANY",
               "ANY-DNSSEC",
               "A",
               "AAAA",
               "NS",
               "CNAME",
               "DNAME",
               "PTR",
               "MX",
               "SRV",
               "TXT",
               "DS",
               "RRSIG",
               "NSEC",
               "DNSKEY",
               "NSEC3",
               "TLSA",
               "URI"))

// -----------------

  // When we start, make sure we have quota left...
  check_Remaining_quota

  // put up the main GUI form now
  do_GUI_form

// -----------------

  reactions += {
    case ButtonClicked(button) => {
      mutex.selected.get match {
        case `rrname` => {
          rrname.selected = true
          rdata.selected = false
          notSet.selected = true
          repaint()
        }
        case `rdata` => {
          rdata.selected = true
          notSet.selected = true
          repaint()
        }
      }
    }
  } // end of reactions

// -----------------

  def myClose() {
    val res = scala.swing.Dialog.showConfirmation(
      contents.head,
      "OK to quit now?",
      optionType = scala.swing.Dialog.Options.YesNo,
      title = title)
    if (res == scala.swing.Dialog.Result.Ok) sys.exit(0)
  } // end of myClose

// -----------------

  def APIfileDoesntExist() {
    contents = new BoxPanel(Orientation.Vertical) {
      background = c537
      // val res = scala.swing.Dialog.showConfirmation(
      scala.swing.Dialog.showConfirmation(
        contents.head,
        ".dnsdb-apikey.txt isn't in default home directory. Must Exit Now",
        optionType = scala.swing.Dialog.Options.YesNo,
        title = title)
      sys.exit(0)
    }
  } // end  of APIfileDoesntExist

// -----------------

  def reset_vals() {
    rectype.selection.item = "ANY"
    myinput.text = ""
    myinput2.text = ""
    myinput3.text = ""
    maxresults.text = ""
    gobackhowmanydays.text = ""
    mutex.select(`rrname`)
    mutex2.select(`textformat`)
    mutex4.select(`notSet`)
  }

// -----------------

  def check_Remaining_quota() {

    System.setProperty("https.cipherSuites",
                       "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384")
    System.setProperty("jdk.tls.ephemeralDHKeySize", "4096")

    val homeDir0 = System.getProperty("user.home")
    val apiKey0 = ".dnsdb-apikey.txt"
    val apiPath0 = homeDir0 + "/" + apiKey0

    val filename0 = apiPath0
    var myLine0 = ""
    for (line0 <- Source.fromFile(filename0).getLines()) {
      myLine0 = line0
    }

    val checkURL0: String = "https://api.dnsdb.info/lookup/rate_limit"
    val myurl0 = new URL(checkURL0)
    val con0 = myurl0.openConnection.asInstanceOf[HttpURLConnection]

    con0.setRequestProperty("X-API-Key", myLine0)
    con0.setRequestProperty("User-Agent",
                            "Demo Scala DNSDB API Client v1.0 Quota Check")
    con0.setRequestProperty("Accept", "text/plain")
    con0.setRequestMethod("GET")
    con0.setConnectTimeout(3000)
    con0.setReadTimeout(3000)
    con0.connect()
    val responseCode0 = con0.getResponseCode()

    if (responseCode0 == 404) {
      // println("Quota check URL not found" + nl)
    } else if (responseCode0 == 400) {
      // println("Quota check URL formatted incorrectly" + nl)
    } else if (responseCode0 == 403) {
      // println("X-API-Key header not present or wrong" + nl)
      limit0 = "DNSDB API key (in .dnsdb-apikey.txt) is missing or invalid"
    } else if (responseCode0 == 500) {
      println("Error processing quota check request" + nl)
    } else if (responseCode0 == 200) {
      val ins0: InputStream = con0.getInputStream()
      val isr0: InputStreamReader = new InputStreamReader(ins0)
      val in0: BufferedReader = new BufferedReader(isr0)

      // ignore three lines
      var lines0 = in0.readLine.mkString
      lines0 = in0.readLine.mkString
      lines0 = in0.readLine.mkString

      // fourth line = limit (we want this one)
      lines0 = in0.readLine.mkString
      lines0 = lines0.replace("\"limit\":","")
      lines0 = lines0.replace(" ","")
      lines0 = lines0.replace(",","")
      limit0 = lines0

      // fifth line = remaining (we want that one, too)
      lines0 = in0.readLine.mkString
      lines0 = lines0.replace("\"remaining\":","")
      lines0 = lines0.replace(" ","")
      remaining0 = lines0

      // dump six and seventh lines
      lines0 = in0.readLine.mkString
      lines0 = in0.readLine.mkString

      in0.close()
    }
  }

// -----------------

  def do_GUI_form() {

    System.setProperty("apple.laf.useScreenMenuBar", "true");
    System.setProperty("com.apple.mrj.application.apple.menu.about.name", "SampleScala");
    UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());

    contents = new BoxPanel(Orientation.Vertical) {
      background = c537
      contents += Swing.VStrut(borderMargin)

      // Title
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += Swing.HGlue
        contents += new swing.Label {
          text = "DNSDB API DEMO CLIENT IN SCALA"
          font = new Font(myFont, java.awt.Font.BOLD, myFontSize)
        }
        contents += Swing.HGlue
        contents += Swing.HStrut(borderMargin)
      }
      contents += Swing.VStrut(bigVerticalSpaceBetweenItems)

      // Quota Report
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += new swing.Label {

          if (limit0 == "unlimited") {
            text = "Queries/day: Unlimited"
            // this is a positive green color
            foreground = new Color(0, 102, 0)
          } else if (limit0 == "DNSDB API key (in .dnsdb-apikey.txt) is missing or invalid") {
            text = "DNSDB API key (in .dnsdb-apikey.txt) is missing or invalid"
            // this is a negative red color
            foreground = new Color(179, 0, 0)
          } else {
            text = "Queries/day: " + limit0 + "   " +
              "Remaining: " + remaining0
            if (remaining0 == "0") {
              // this is a negative red color
              foreground = new Color(179, 0, 0)
            } else {
              // this is a positive green color
              foreground = new Color(0, 102, 0)
            }
          }

          font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)

        }
        contents += Swing.HGlue
        contents += Swing.HStrut(borderMargin)
      }
      contents += Swing.VStrut(bigVerticalSpaceBetweenItems)

      // Search Mode
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += new swing.Label {
          text = "Search: "
          font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
        }
        contents += new BoxPanel(Orientation.Horizontal) {
          contents += rrname
          // this is a light blue color
          background = new Color(204, 255, 255)
        }
        contents += Swing.HGlue
        contents += Swing.HStrut(borderMargin)
      }
      rrname.selected = true
      contents += Swing.VStrut(10)
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(100)
        contents += new BoxPanel(Orientation.Horizontal) {
          contents += rdata
          contents += Swing.HStrut(5)
          contents += new swing.Label {
            text = "=>"
            font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
          }
          contents += nameinputmode
          contents += Swing.HStrut(5)
          contents += ipOrNetworkMode
          contents += Swing.HStrut(5)
          contents += notSet
          contents += Swing.HStrut(5)
          // this is a light purple color
          background = new Color(229, 204, 255)
        }
        contents += Swing.HStrut(borderMargin)
        contents += Swing.HGlue
        background = c537
      }
      contents += Swing.VStrut(bigVerticalSpaceBetweenItems)

      // Record Type
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += new swing.Label {
          text = "Record Type:"
          font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
        }
        contents += Swing.HStrut(5)
        contents += rectype
        contents += Swing.HGlue
      }
      contents += Swing.VStrut(verticalSpaceBetweenItems)

      // Domain text box
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += new swing.Label {
          text = "Query Term: "
          font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
        }
        contents += Swing.HStrut(5)
        contents += myinput
        contents += Swing.HGlue
        contents += Swing.HStrut(borderMargin)
      }
      contents += Swing.VStrut(bigVerticalSpaceBetweenItems)

      // Separate the optional stuff
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += new swing.Label {
          text = "Optional... "
          font = new Font(myFont, java.awt.Font.ITALIC, myFontSize)
        }
        contents += Swing.HGlue
        contents += Swing.HStrut(borderMargin)
      }
      contents += Swing.VStrut(verticalSpaceBetweenItems)

      // Bailiwick text box
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += new swing.Label {
          text = "Bailiwick:  "
          font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
        }
        contents += Swing.HStrut(5)
        contents += myinput2
        contents += Swing.HStrut(borderMargin)
        contents += Swing.HGlue
      }
      contents += Swing.VStrut(verticalSpaceBetweenItems)

      // Max results
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += new swing.Label {
          text = "Max Results:"
          font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
        }
        contents += Swing.HStrut(5)
        contents += maxresults
        contents += Swing.HGlue
      }
      contents += Swing.VStrut(verticalSpaceBetweenItems)

      // Time window
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += new swing.Label {
          text = "Days Back?  "
          font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
        }
        contents += Swing.HStrut(5)
        contents += gobackhowmanydays
        contents += Swing.HGlue
      }
      contents += Swing.VStrut(verticalSpaceBetweenItems)

      // Output format: Text or JSON?
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += new swing.Label {
          text = "Format:     "
          font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
        }
        contents += Swing.HStrut(5)
        contents += textformat
        contents += Swing.HStrut(5)
        contents += jsonformat
        contents += Swing.HGlue
      }
      contents += Swing.VStrut(bigVerticalSpaceBetweenItems)

      // search/reset/close
      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += swing.Button("Search") {
          restExample()
        }
        contents += swing.Button("Reset") {
          reset_vals()
        }
        contents += swing.Button("Close") {
          myClose()
        }
        contents += Swing.HGlue
        contents += Swing.HStrut(borderMargin)
      }
      contents += Swing.VStrut(bigVerticalSpaceBetweenItems)

      contents += new BoxPanel(Orientation.Horizontal) {
        background = c537
        contents += Swing.HStrut(borderMargin)
        contents += Swing.VStrut(10)

        contents += Swing.VStrut(verticalSpaceBetweenItems)
      } // end of vertical Frame

// -----------------

      def setBoxLoc() {

        xcoord += bumpIt
        if (xcoord >= xcoordMax) { xcoord = resetxcoord }

        ycoord += bumpIt
        if (ycoord >= ycoordMax) { ycoord = resetycoord }

      }

      // http://alvinalexander.com/blog/post/java/simple-https-example
      def restExample() {
        System.setProperty("https.cipherSuites",
                           "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384")
        System.setProperty("jdk.tls.ephemeralDHKeySize", "4096")

        // assume we're good to proceed; we'll then check...
        var goodToOutput = true

        var httpsURL: String = "https://api.dnsdb.info/lookup/"

        // We need a query term. Did they specify one?
        if (myinput.text.length == 0) {
          val myNewWindow = new UIoutput("Query missing...",
                                         "Please supply a Query Term...",
                                         redTint)
          setBoxLoc()
          goodToOutput = false
          myNewWindow.location = new Point(xcoord, ycoord)
          myNewWindow.pack()
          myNewWindow.visible = true
        } else {
          // continue processing...

          // base RESTFUL URL
          if (rrname.selected == true) {
            httpsURL = httpsURL + "rrset/name/"
            // add on domain and recordtype
            httpsURL = httpsURL + myinput.text + "/" + rectype.selection.item
            // do we have a bailiwick? If so, add it on as well
            if (myinput2.text.length > 0) {
              httpsURL = httpsURL + "/" + myinput2.text
            }
          } else if ((rdata.selected == true) &&
                     (nameinputmode.selected == true)) {
            httpsURL = httpsURL + "rdata/name/"
            // add on domain and recordtype
            httpsURL = httpsURL + myinput.text + "/" + rectype.selection.item
          } else if ((rdata.selected == true) &&
                     (ipOrNetworkMode.selected == true)) {
            httpsURL = httpsURL + "rdata/ip/"
            var tempstring = myinput.text
            tempstring = tempstring.replace("/", ",")
            // add on IP or CIDR and recordtype
            httpsURL = httpsURL + tempstring + "/" + rectype.selection.item
          } else if ((rdata.selected == true) &&
                     (notSet.selected == true)) {
            val myNewWindow = new UIoutput(
              "Rdata query type missing...",
              "Rdata query requires selecting either 'Name' OR 'IP, CIDR or range'...",
              redTint)
            goodToOutput = false
            setBoxLoc()
            myNewWindow.location = new Point(xcoord, ycoord)
            myNewWindow.pack()
            myNewWindow.visible = true
          }

          var delimiter: String = "?" // first param

          // are we limiting results returned?
          if ((maxresults.text.length > 0) &&
              (maxresults.text.toInt > 0) &&
              (maxresults.text.toInt <= 1000000)) {
            httpsURL = httpsURL + delimiter + "limit=" + maxresults.text
            delimiter = "&" // ampersand
          } else if (maxresults.text.length == 0) {
            // not specifying max results
          } else {
            val myNewWindow = new UIoutput(
              "Max Results Error...",
              "maxresults must be 1 <= maxresults <= 1000000; using default of 10000",
              orangeTint)
            setBoxLoc()
            myNewWindow.location = new Point(xcoord, ycoord)
            myNewWindow.pack()
            myNewWindow.visible = true
          }

          // are we time fencing?
          if ((gobackhowmanydays.text.length > 0) &&
              (gobackhowmanydays.text.toLong > 0) &&
              (gobackhowmanydays.text.toLong < 3650)) {
            // convert days to seconds
            val nowUnixSeconds: Long = (gobackhowmanydays.text).toLong *
              (24 * 60 * 60)
            // time fence parameter is "time_last_after="
            httpsURL = httpsURL + delimiter + "time_last_after=-" + nowUnixSeconds
            delimiter = "&"
          } else if (gobackhowmanydays.text.length == 0) {
            // not time fencing
          } else {
            val myNewWindow = new UIoutput(
              "Time Fencing Error...",
              "maxdaysback must be 1 to 3650 integer days; doing NO time fencing",
              orangeTint)
            setBoxLoc()
            myNewWindow.location = new Point(xcoord, ycoord)
            myNewWindow.pack()
            myNewWindow.visible = true
          }

          val myurl = new URL(httpsURL)

          // handle creating the output file
          // target $HOME/dnsdb-output/$DATE/$TIME-$QUERY.txt

          // this is user's home diretory + /dnsdb-output
          var homeDir = System.getProperty("user.home")
          homeDir = homeDir + "/dnsdb-output"

          // now add a subdirectory, y/m/d format
          val now = Calendar.getInstance().getTime()
          val dateFormat = new SimpleDateFormat("yyyy-MM-dd")
          val myDate = dateFormat.format(now)
          homeDir = homeDir + "/" + myDate
          val f: File = new File(homeDir)
          f.mkdirs()

          // now build the filename (time + querystring + rectype + .txt)
          val timeFormat = new SimpleDateFormat("HHmmss")
          val myTime = timeFormat.format(now)
          var tempstring2 = myinput.text
          // fix awkwardness in potential filenames
          tempstring2 = tempstring2.replace("/", ",")
          tempstring2 = tempstring2.replace(":", "#")
          tempstring2 = tempstring2.replace("*", "STAR")
          val outputPath = homeDir + "/" + myTime + "-" + tempstring2 + ".txt"
          val f2: File = new File(outputPath)

          val pw: PrintWriter = new PrintWriter(
            new FileOutputStream(f2, false /* append = true */ ))

          val con = myurl.openConnection.asInstanceOf[HttpURLConnection]

          homeDir = System.getProperty("user.home")
          val apiKey = ".dnsdb-apikey.txt"
          val apiPath = homeDir + "/" + apiKey

          val filename = apiPath
          var myLine = ""
          for (line <- Source.fromFile(filename).getLines()) {
            myLine = line
          }

          con.setRequestProperty("X-API-Key", myLine)
          con.setRequestProperty("User-Agent",
                                 "Demo Scala DNSDB API Client v1.0")

          if (mutex2.selected.get == `jsonformat`) {
            con.setRequestProperty("Accept", "application/json")
          } else if (mutex2.selected.get == `textformat`) {
            con.setRequestProperty("Accept", "text/plain")
          }

          con.setRequestMethod("GET")
          con.setConnectTimeout(3000)
          con.setReadTimeout(3000)

          var responseCode = 9999 // assume we couldn't do the query by default

          if (goodToOutput == true) {
            con.connect()
            responseCode = con.getResponseCode()
          }

          if (responseCode == 404) {
            val myNewWindow =
              new UIoutput("Query Error...", "No records found", redTint)
            setBoxLoc()
            myNewWindow.location = new Point(xcoord, ycoord)
            myNewWindow.pack()
            myNewWindow.visible = true

          } else if (responseCode == 400) {
            val myNewWindow = new UIoutput("Query Error...",
                                           "URL formatted incorectly",
                                           redTint)
            setBoxLoc()
            myNewWindow.location = new Point(xcoord, ycoord)
            myNewWindow.pack()
            myNewWindow.visible = true
          } else if (responseCode == 403) {
            val myNewWindow = new UIoutput(
              "Query Error...",
              "X-API-Key header not present or wrong",
              redTint)
            setBoxLoc()
            myNewWindow.location = new Point(xcoord, ycoord)
            myNewWindow.pack()
            myNewWindow.visible = true
          } else if (responseCode == 429) {
            val myNewWindow = new UIoutput("Query Error...",
                                           "API daily key quota exceeded",
                                           redTint)
            setBoxLoc()
            myNewWindow.location = new Point(xcoord, ycoord)
            myNewWindow.pack()
            myNewWindow.visible = true
          } else if (responseCode == 429) {} else if (responseCode == 500) {
            println("Error processing request" + nl)
          } else if (responseCode == 503) {
            val myNewWindow = new UIoutput("Query Error...",
                                           "Too many concurrent queries",
                                           redTint)
            setBoxLoc()
            myNewWindow.location = new Point(xcoord, ycoord)
            myNewWindow.pack()
            myNewWindow.visible = true

          } else if ((responseCode == 200) && (goodToOutput == true)) {
            val myNewWindow =
              new UIoutput("Output is being sent to... (please be patient for large queries!)", outputPath, greenTint)
            setBoxLoc()
            myNewWindow.location = new Point(xcoord, ycoord)
            myNewWindow.pack()
            myNewWindow.visible = true

            val ins: InputStream = con.getInputStream()
            val isr: InputStreamReader = new InputStreamReader(ins)
            val in: BufferedReader = new BufferedReader(isr)
            var ok = true

            while (ok) {
              val lines = in.readLine.mkString
              ok = lines != null
              if (ok) {
                // println(lines + nl)
                pw.write(lines + nl)
              }
            }

            // println(nl)
            pw.write(nl)

            in.close()
            pw.close()

          }
          // update available quota
          check_Remaining_quota
        } // end of UI
      }
    }
  }
}

// -----------------

class UIerror extends MainFrame {
  contents = new BoxPanel(Orientation.Vertical) {
    contents += new Label {
      text = " "
    }
  }
  val res = Dialog.showConfirmation(
    contents.head,
    ".dnsdb-apikey.txt file in the default home directory should be\nexactly 64 characters long (disregarding end-of-line characters).\n\nIT'S NOT. Cannot continue.\n\nHit either button to exit. Fix the file, then try again.",
    optionType = Dialog.Options.OkCancel,
    title = "ERROR!"
  )
  if (res == Dialog.Result.Ok)
    sys.exit(0)
  else
    sys.exit(0)
} // end of UIerror

// -----------------

class UIerror2 extends MainFrame {
  contents = new BoxPanel(Orientation.Vertical) {
    contents += new Label {
      text = " "
    }
  }
  val res = Dialog.showConfirmation(
    contents.head,
    "Your DNSDB API key MUST be in a plain text file called .dnsdb-apikey.txt in the default home directory.\n\nWE CAN'T FIND IT.\n\nDid you forget to create it?\n\nOr perhaps you misnamed it by forgetting the leading dot in the file name? \n\nMaybe it's in the wrong directory?\n\nAnyhow, without it, WE CANNOT CONTINUE.\n\nHit either button to exit. Get the file in the right spot, then try again.",
    optionType = Dialog.Options.OkCancel,
    title = "ERROR!"
  )
  if (res == Dialog.Result.Ok)
    sys.exit(0)
  else
    sys.exit(0)
} // end of UIerror2

// -----------------

class UIerror3 extends MainFrame {
  contents = new BoxPanel(Orientation.Vertical) {
    contents += new Label {
      text = " "
    }
  }
  val res = Dialog.showConfirmation(
    contents.head,
    "Java has two cryptographic modes: the default (limited strength) mode, and enhanced\n(unlimited strength) mode. This program intentionally requires unlimited strength\nmode.\n\nYOU DO NOT CURRENTLY HAVE JAVA UNLIMITED STRENGTH CRYPTOGRAPHY ENABLED.\n\nTo correct this, see\n\n   \'Java Cryptography Extension (JCE) Unlimited Strength Jurisdiction Policy Files 8 Download,\'\n   http://www.oracle.com/technetwork/java/javase/downloads/jce8-download-2133166.html\n\nOnce you have downloaded and unzipped the unlimited strength cryptographic policy files,\ncopy them into your JRE\'s lib/security directory, replacing the files of the same name that\nare currently there. NOTE: You will only need to do this ONE TIME for any given JRE installation.\n\nThe exact location of your JRE lib/security directory may vary, and if you have multiple copies\nof Java installed you'll need to make sure you enable strong crypto on the RIGHT copy of Java,\nbut on a typical Mac, see\n\n     /Library/Java/JavaVirtualMachines/jdk1.8.0_121.jdk/Contents/Home/jre/lib/security\n\nOn a typical Windows 10 PC, see\n\n     C:\\Program Files (x86)\\Java\\jre1.8.0_121\\lib\\security\n\nUntil you successfully enable strong cryptography for Java, WE CANNOT CONTINUE.\n\nHit either button to exit. Get the strong crypto policy files installed, then try again.",
    optionType = Dialog.Options.OkCancel,
    title = "ERROR!"
  )
  if (res == Dialog.Result.Ok)
    sys.exit(0)
  else
    sys.exit(0)
} // end of UIerror2

// -----------------

object SampleScala {

  var apiKeyFileStatus = "APIFileIsOK"

  def main(args: Array[String]) = {

    // CHECK SANITY OF CONFIGURATION, FIRST...

    // Does the API Key File exist, and is it of at least the right size?

    val homeDir0 = System.getProperty("user.home")
    val apiKey0 = ".dnsdb-apikey.txt"
    val apiPath0 = homeDir0 + "/" + apiKey0

    val f = new File(apiPath0)
    val filename0 = apiPath0
    var myLine0 = ""

    if (f.exists()) {
      for (line0 <- Source.fromFile(filename0).getLines()) {
        myLine0 = line0
        myLine0 = myLine0.replaceAll("(\\r|\\n)", "")
      }
    } else {
      // println("API Key File doesn't exist at .dnsdb-apikey.txt in default home directory")
      val uiError2 = new UIerror2
      uiError2.visible = true
    }

    val fileContentsLength = myLine0.length()

    if (fileContentsLength == 64) {
      // file contents length is correct
    } else {
      // println("Wrong size : " + fileContentsLength)
      val uiError = new UIerror
      uiError.visible = true
    }

    // now check for the existence of strong crypto policy files
    val unlimited = Cipher.getMaxAllowedKeyLength("RC5")
    if (unlimited <= 256) {
      val uiError3 = new UIerror3
      uiError3.visible = true
    }

    // all's okay, proceed as normally

    val ui = new UI
    ui.location = new Point(50,50)
    ui.pack()
    ui.visible = true

    while (System.in.read() != -1) {}
  }

}
