package checkasn

import scala._
import scala.swing._
import swing._

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

