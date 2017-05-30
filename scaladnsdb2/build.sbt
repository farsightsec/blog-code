name := "CheckASN"

version := "1.0"

scalaVersion := "2.12.2"

cancelable in Global := true

fork in run := true

connectInput in run := true

enablePlugins(JavaAppPackaging)

enablePlugins(WindowsPlugin)

maintainer := "Joe St Sauver <stsauver@fsi.io>"
packageSummary := "QueryDNSbyASN"
packageDescription := """Demonstrate querying DNSDB by ASN with Scala"""

wixProductId := "bcadded4-ab1d-40c6-a4b8-53afa01ccc54"
wixProductUpgradeId := "c90c7414-05c2-4c94-a8aa-302d49bf7894"

// javaOptions in run += "-Djavax.net.debug=ssl:handshake"

// https://mvnrepository.com/artifact/org.scala-lang.modules/scala-swing_2.12
libraryDependencies += "org.scala-lang.modules" % "scala-swing_2.12" % "2.0.0"

// https://mvnrepository.com/artifact/com.typesafe.play/play-json_2.12
libraryDependencies += "com.typesafe.play" % "play-json_2.12" % "2.6.0-M6"
