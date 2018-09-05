package checkasn

import java.io._
import java.lang._
import java.net._
import scala.io._

object CheckRemainingQuota {

  def check_Remaining_quota:(String, String) = {

    var limit0: String = ""
    var remaining0: String = ""

    // End of line in Unix = \n
    // End of line in Windows = \r\n
    // MS Word and WordPad can cope with \n, but the default Windows text
    // file editor, Notepad, fails to cope, so let's do the right thing here
    val nl = System.getProperty("line.separator").toString();

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
    con0.setRequestProperty("User-Agent", "DNSDB Check-By-ASN Demo Client")
    con0.setRequestProperty("Accept", "text/plain")
    con0.setRequestMethod("GET")
    con0.setConnectTimeout(3000)
    con0.setReadTimeout(3000)
    con0.connect()
    val MyresponseCode0 = con0.getResponseCode()

    if (MyresponseCode0 == 404) {
      // println("Quota check URL not found" + nl)
    } else if (MyresponseCode0 == 400) {
      // println("Quota check URL formatted incorrectly" + nl)
    } else if (MyresponseCode0 == 403) {
      // println("X-API-Key header not present or wrong" + nl)
      limit0 = "DNSDB API key (in .dnsdb-apikey.txt) is missing or invalid"
    } else if (MyresponseCode0 == 500) {
      println("Error processing quota check request" + nl)
    } else if (MyresponseCode0 == 200) {
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
    return (limit0, remaining0)
  }
}
