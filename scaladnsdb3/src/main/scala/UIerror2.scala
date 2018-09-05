package checkasn

import scala._
import scala.swing._
import swing._

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

