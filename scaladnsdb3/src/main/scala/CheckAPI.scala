package checkasn

import java.io._
import java.lang._
import scala._
import scala.io._

// -----------------

object CheckAPI {

  def CheckAPIFile: String = {

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
      val uiError2 = new UIerror2
      uiError2.visible = true
      // we kill the main window after showing the error dialog
    }

    val fileContentsLength = myLine0.length()

    if (fileContentsLength == 64) {
      // file contents length is correct
    } else {
      // println("Wrong size : " + fileContentsLength)
      val uiError = new UIerror
      uiError.visible = true
      // we kill the main window after showing the error dialog
    }

    // made it this far? all's superficially okay with the API key file,
    // so proceed as normal
    return "OK"
  }
}
