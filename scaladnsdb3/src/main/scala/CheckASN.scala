package checkasn

import java.awt._
import java.awt.Component._
import java.io._
import java.lang._
import java.net._
import java.text._
import java.util._
import javax.swing.JFrame._
import org.json4s._
import org.json4s.jackson.JsonMethods.parse
import org.json4s.DefaultFormats
import play.api.libs.json._
import scala._
import scala.io._
import scala.swing._
import swing._
import Swing._

class UI extends MainFrame {
  // Our main window. Closing it will kill the application

  // =============================================================
  // Definitions

  title = "Checking DNSDB by ASN with Scala"

  setDefaultLookAndFeelDecorated(true)

  val myFont = "Monospaced"
  System.setProperty("awt.font", myFont)

  val myFontSize = 16

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

  // DNSDB quota values
  var limit0: String = ""
  var remaining0: String = ""

  // the pivotal item itself, the ASN
  var myasn: String = ""

  // for consolidated output, we need to make sure we don't make new
  // filename; we just want to use the first one we used
  var FirstFileName: String = ""
  var FirstDirName: String = ""

  // =====================================================================
  // Elements used in our GUI form

  // ASN of interest
  object myasn2 extends swing.TextField {
    columns = 7
    maximumSize = new Dimension(7, 24)
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }

  // Max results to return per prefix
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
  val both_v4_and_v6 = new RadioButton {
    text = "IPv4+IPv6"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
    selected = true
  }
  val v4_only = new RadioButton {
    text = "IPv4"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }
  val v6_only = new RadioButton {
    text = "IPv6"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }
  val mutex0 = new swing.ButtonGroup(both_v4_and_v6, v4_only, v6_only)
  // .... Radio button group ends

  // .... Radio button group
  val per_prefix_output = new RadioButton {
    text = "Per-Prefix"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
    selected = true
  }
  val consolidated_output = new RadioButton {
    text = "Per-ASN"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }
  val mutex1 = new swing.ButtonGroup(per_prefix_output, consolidated_output)
  // .... Radio button group ends

  // .... Radio button group
  val textformat = new RadioButton {
    text = "Text      "
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
    selected = true
  }
  val jsonformat = new RadioButton {
    text = "JSON"
    font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
  }
  val mutex2 = new swing.ButtonGroup(textformat, jsonformat)
  // .... Radio button group ends ....

  // now do the drop down box to allow us to select the recordtype we want
  val rectype = new swing.ComboBox(
    scala.List("ANY",
               "A",
               "AAAA"))

  // ======================================================================

  // PUT UP THE BLANK GRAPHIC FORM

  // When we start, make sure we have quota left...
  var quota_structure = CheckRemainingQuota.check_Remaining_quota
  limit0 = quota_structure._1
  remaining0 = quota_structure._2

  // Build the GUI interface to display
  contents = new BoxPanel(Orientation.Vertical) {
    background = c537
    contents += Swing.VStrut(borderMargin)

    // Title
    contents += new BoxPanel(Orientation.Horizontal) {
      background = c537
      contents += Swing.HStrut(borderMargin)
      contents += Swing.HGlue
      contents += new swing.Label {
        text = "Checking DNSDB by ASN with Scala"
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
        if (limit0 == "unlimited") {
	  contents += new swing.Label {
          text = "Queries/day: Unlimited"
          // this is a positive green color
          foreground = new Color(0, 102, 0)
	  }
        } else if (limit0 == "DNSDB API key (in .dnsdb-apikey.txt) is missing or invalid") {
	  contents += new swing.Label {
          text = "DNSDB API key (in .dnsdb-apikey.txt) is missing or invalid"
          // this is a negative red color
          foreground = new Color(179, 0, 0)
	  }
        } else {
	  contents += new swing.Label {
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
        }
      contents += new swing.Label {
        font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
      }
      contents += Swing.HGlue
      contents += Swing.HStrut(borderMargin)
    }
    contents += Swing.VStrut(bigVerticalSpaceBetweenItems)

    // Specify the ASN
    contents += new BoxPanel(Orientation.Horizontal) {
      background = c537
      contents += Swing.HStrut(borderMargin)
      contents += new swing.Label {
        text = "Autonomous System Number:"
        font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
      }
      contents += Swing.HStrut(5)
      contents += myasn2
      contents += Swing.HGlue
      contents += Swing.HStrut(borderMargin)
    }
    contents += Swing.VStrut(verticalSpaceBetweenItems)

    // Do we want to check for IPv4 and IPv6 prefixes, or just one or the
    // other?
    contents += new BoxPanel(Orientation.Horizontal) {
      background = c537
      contents += Swing.HStrut(15)
      contents += both_v4_and_v6
      contents += Swing.HStrut(5)
      contents += v4_only
      contents += Swing.HStrut(5)
      contents += v6_only
      contents += Swing.HStrut(borderMargin)
      contents += Swing.HGlue
    }
    contents += Swing.VStrut(verticalSpaceBetweenItems)

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

    // Max results
    contents += new BoxPanel(Orientation.Horizontal) {
      background = c537
      contents += Swing.HStrut(borderMargin)
      contents += new swing.Label {
        text = "Results/Prefix:"
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
        text = "Days Back:     "
        font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
      }
      contents += Swing.HStrut(5)
      contents += gobackhowmanydays
      contents += Swing.HGlue
    }
    contents += Swing.VStrut(bigVerticalSpaceBetweenItems)

    // Maintain seperate files for each prefix, or consolidate into one?
    contents += new BoxPanel(Orientation.Horizontal) {
      background = c537
      contents += Swing.HStrut(borderMargin)
      contents += new swing.Label {
        text = "Output:"
        font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
      }
      contents += Swing.HStrut(5)
      contents += per_prefix_output
      contents += Swing.HStrut(5)
      contents += consolidated_output
      contents += Swing.HGlue
    }
    contents += Swing.VStrut(verticalSpaceBetweenItems)

    // Output format: Text or JSON?
    contents += new BoxPanel(Orientation.Horizontal) {
      background = c537
      contents += Swing.HStrut(borderMargin)
      contents += new swing.Label {
        text = "Format:"
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
        get_prefixes_from_ripe(myasn2.text)
      }
      contents += swing.Button("Reset") {
        myasn2.text = ""
        maxresults.text = ""
        gobackhowmanydays.text = ""
        mutex1.select(`per_prefix_output`)
        mutex2.select(`textformat`)
      }
      contents += swing.Button("Close") {
        sys.exit(0)
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
  }

// =======================================================================

// GET THE PREFIXES ASSOCIATED WITH THE ASN

  def get_prefixes_from_ripe(mytempasn: String) {

    implicit val formats = DefaultFormats
    var rawJsonLines: String = ""

    // ASN should be just a number, but some people may prefix it with "as"
    // so we'll strip that if present
    var myasn = myasn2.text
    myasn = myasn.replace("AS", "")
    myasn = myasn.replace("aS", "")
    myasn = myasn.replace("As", "")
    myasn = myasn.replace("as", "")

    // build the URL for ripe...
    var checkURL: String = "https://stat.ripe.net/data/announced-prefixes/data.json?resource=" + myasn

    // process the JSON lines into a string
    rawJsonLines = scala.io.Source.fromURL(checkURL).mkString

    // extract just the prefixes of interest
    var json = (parse(rawJsonLines) \ "data" \ "prefixes" \ "prefix")

    // convert that array of objects into an array of strings
    var jsonArrayTemp = ((json).extract[Array[String]])

    // sort and uniquify the prefixes
    var jsonArray = jsonArrayTemp.sorted
    jsonArrayTemp = jsonArray.distinct
    jsonArray = jsonArrayTemp

    var i=0
    var e=""

    while (i < jsonArray.length) {    // OUTER PREFIX PROCESSING LOOP

       var myOutputFormat: String = ""
       var myPrefix: String = ""

       if (textformat.selected == true) { myOutputFormat = "textformat"
       } else if (jsonformat.selected == true) { myOutputFormat = "jsonformat"
       }

       e=jsonArray(i)

       // process v4 queries, if needed
       if (((v4_only.selected == true) || (both_v4_and_v6.selected == true))
          && (e.indexOf(".") != -1) ) {

          // if json prefix strings include literal quotes, remove them
          myPrefix = e.toString.replace("\"", "")

          // flip the slash in a prefix to a comma
          // need to fix it here for the dnsdb query and for output file usage
          myPrefix = myPrefix.replace("/", ",")

          var myOutputGoesTo: String = ""
          if (per_prefix_output.selected == true) {
            myOutputGoesTo = myPrefix
          } else if (consolidated_output.selected == true) {
            myOutputGoesTo = myasn
          }

          // do the v4 query
          sendQueryToDNSDB(myPrefix, myOutputGoesTo)
       }

      // process v6 queries, if needed
      if (((v6_only.selected == true) || (both_v4_and_v6.selected == true))
        && (e.indexOf(":") != -1)) {

          // json prefix strings include literal quotes; remove them
          myPrefix = e.toString.replace("\"", "")

          // now flip the slash to a comma in the prefix
          // need to fix it here for output file usage
          myPrefix = myPrefix.replace("/", ",")

          // can't set where output goes until we know the relevant
          // prefix (just in case if per_prefix_output is selected)

          var myOutputGoesTo: String = ""
          if (per_prefix_output.selected == true) {
            myOutputGoesTo = myPrefix
          } else if (consolidated_output.selected == true) {
            myOutputGoesTo = myasn
          }

          // do the v6 query
          sendQueryToDNSDB(myPrefix, myOutputGoesTo)
      } // end of v6 processing block
      i=i+1
    } // end of ASN processing block

    // Reset the FirstFileName for next time
    FirstFileName = ""
    FirstDirName = ""
    myasn2.text = ""
    maxresults.text = ""
    gobackhowmanydays.text = ""
    mutex1.select(`per_prefix_output`)
    mutex2.select(`textformat`)
    quota_structure = CheckRemainingQuota.check_Remaining_quota
    limit0 = quota_structure._1
    remaining0 = quota_structure._2

    println(nl + "-------------------------------")
  }

// -------------------------------------------------------------

  def setBoxLoc() {

    xcoord += bumpIt
    if (xcoord >= xcoordMax) { xcoord = resetxcoord }

    ycoord += bumpIt
    if (ycoord >= ycoordMax) { ycoord = resetycoord }

  }

// -----------------

  def sendQueryToDNSDB(myprefix: String, OutputGoesTo: String) {

    var httpsURL: String = "https://api.dnsdb.info/lookup/rdata/ip/"

    val myasn3: String = myasn2.text

    // need a copy of this that we can modify
    val myPrefix = myprefix

    httpsURL = httpsURL + myPrefix + "/" + rectype.selection.item

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
      val nowUnixSeconds: scala.Long = (gobackhowmanydays.text).toLong *
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
    // two options exist...
    // per prefix files, one per query
    // per ASN files, one only, results **appended** to it for each query
    //
    // format 1: $HOME/dnsdb-output/$DATE/$TIME-$QUERY.txt (like normal)
    // format 2: $HOME/dnsdb-output/$DATE/$TIME-$ASN.txt (time doesn't update)

    // this is user's home diretory + /dnsdb-output
    var homeDir = System.getProperty("user.home")
    homeDir = homeDir + "/dnsdb-output"
    // now add a subdirectory, y/m/d format
    val dateFormat = new SimpleDateFormat("yyyy-MM-dd")
    homeDir = homeDir + "/" + dateFormat.format(
      Calendar.getInstance().getTime())

    // make sure a directory exists with today's date
    val f: File = new File(homeDir)
    f.mkdirs()

    if ((myprefix == OutputGoesTo) && (FirstDirName == "")) {
      // consolidate output will go into a subdirectory time-ASnumber
      val timeFormat2 = new SimpleDateFormat("HHmmss")
      val myTime2 = timeFormat2.format(Calendar.getInstance().getTime())
      homeDir = homeDir + "/" + myTime2 + "-AS" + myasn3
      println("Making subdiredctory" + homeDir)
      val f: File = new File(homeDir)
      f.mkdirs()
      FirstDirName = homeDir
    } else if ((myprefix == OutputGoesTo) && (FirstDirName != "")) {
      homeDir = FirstDirName
    }

    // now we need to handle making the output filename in that directory
    var outputPath: String = ""
    val timeFormat = new SimpleDateFormat("HHmmss")
    val myTime = timeFormat.format(Calendar.getInstance().getTime())
    var tempstring2: String = OutputGoesTo // OutputGoesTo is passed in...

    var file_extension: String = ""
    if (jsonformat.selected == true) {
      file_extension = ".jsonl"
    } else if (textformat.selected == true) {
      file_extension = ".text"
    } else {
      file_extension = ".???"
    }

    if (myprefix == OutputGoesTo) {
      // found a prefix for output, not doing consolidated output...
      // put all the prefixes in a single timestamped directory

      // fix awkwardness in potential filenames
      tempstring2 = tempstring2.replace("/", ",")
      tempstring2 = tempstring2.replace(":", "#")
      tempstring2 = tempstring2.replace("*", "STAR")
      outputPath = homeDir + "/" + myTime + "-" + tempstring2 + "-" +
        rectype.selection.item + file_extension

    } else {

      // consolidated output... we want to open an ASN file, and then
      // append to it, so that all output is in one file

      if (FirstFileName == "") {
        outputPath = homeDir + "/" + myTime + "-AS" + OutputGoesTo + "-" +
          rectype.selection.item + file_extension
        FirstFileName = outputPath
      } else {
        outputPath = FirstFileName
      }
    }

    val f2: File = new File(outputPath)

    val pw: PrintWriter = new PrintWriter(
      new FileOutputStream(f2, true /* true ==> append */ ))

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
    con.setRequestProperty("User-Agent", "DNSDB Check-By-ASN Demo Client")

    if (mutex2.selected.get == `jsonformat`) {
      con.setRequestProperty("Accept", "application/json")
    } else if (mutex2.selected.get == `textformat`) {
      con.setRequestProperty("Accept", "text/plain")
    }

    con.setRequestMethod("GET")
    con.setConnectTimeout(3000)
    con.setReadTimeout(90000)

    var responseCode = 9999 // assume we couldn't do the query by default

    scala.concurrent.blocking {
      con.connect()
      responseCode = con.getResponseCode()

      if (responseCode == 404) {
        // no records found
      } else if (responseCode == 400) {
        val myNewWindow =
          new UIoutput("Query Error...", "URL formatted incorectly", redTint)
        setBoxLoc()
        myNewWindow.location = new Point(xcoord, ycoord)
        myNewWindow.pack()
        myNewWindow.visible = true
      } else if (responseCode == 403) {
        val myNewWindow = new UIoutput("Query Error...",
                                       "X-API-Key header not present or wrong",
                                       redTint)
        setBoxLoc()
        myNewWindow.location = new Point(xcoord, ycoord)
        myNewWindow.pack()
        myNewWindow.visible = true
      } else if (responseCode == 429) {
        val myNewWindow =
          new UIoutput("Query Error...",
                       "API daily key quota exceeded",
                       redTint)
        setBoxLoc()
        myNewWindow.location = new Point(xcoord, ycoord)
        myNewWindow.pack()
        myNewWindow.visible = true
      } else if (responseCode == 429) {} else if (responseCode == 500) {
        println("Error processing request" + nl)
      } else if (responseCode == 503) {
        val myNewWindow =
          new UIoutput("Query Error...",
                       "Too many concurrent queries",
                       redTint)
        setBoxLoc()
        myNewWindow.location = new Point(xcoord, ycoord)
        myNewWindow.pack()
        myNewWindow.visible = true

      } else if (responseCode == 200) {
        println("Output is going to = " + outputPath)
        /* val myNewWindow =
        new UIoutput(
          "Output is being sent to... (please be patient for large queries!)",
          outputPath,
          greenTint)
      setBoxLoc()
      myNewWindow.location = new Point(xcoord, ycoord)
      myNewWindow.pack()
      myNewWindow.visible = true
         */

        val ins: InputStream = con.getInputStream()
        val isr: InputStreamReader = new InputStreamReader(ins)
        val in: BufferedReader = new BufferedReader(isr)
        var ok = true

        while (ok) {
          val lines = in.readLine.mkString
          ok = lines != null
          if (ok) {
            pw.write(lines + nl)
          }
        }

        pw.write(nl)

        in.close()
        pw.close()

      }
    }

    // update available quota
    val quota_structure = CheckRemainingQuota.check_Remaining_quota
    limit0 = quota_structure._1
    remaining0 = quota_structure._2
  } // end of UI
}

object CheckASN {

  def main(args: Array[String]) = {

    if (CheckAPI.CheckAPIFile.toString == "OK") {

      // all's okay, proceed as normally
      val ui = new UI
      ui.location = new Point(50, 50)
      ui.pack()
      ui.visible = true

      while (System.in.read() != -1) {}
    } // APIFileIsOK block
  } // main
} // CheckASN

// -----------------

class UIoutput(frameTitle: String, fileLocation: String, MyBoxColor: Color)
    extends swing.Frame {

  // Create a new wide and short window for posting one-line log-like info
  // (can be closed without affecting the main window)

  preferredSize = new Dimension(760, 60)
  setDefaultLookAndFeelDecorated(true)

  title = frameTitle // passed in as a parameter

  contents = new BoxPanel(Orientation.Horizontal) {
    // Adding the BoxPanel lets me left-justify the label
    background = MyBoxColor // passed in as a parameter
    contents += Swing.HStrut(10) // preserve a left margin

    contents += new swing.Label {
      text = fileLocation // passed in as a parameter
      val myFont = "Monospaced"
      val myFontSize = 16
      font = new Font(myFont, java.awt.Font.PLAIN, myFontSize)
    }

    contents += Swing.HGlue // left justify
    contents += Swing.HStrut(10) // preserve a right margin
  }
}
