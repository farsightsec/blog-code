name := "SampleScala"

version := "1.0"

scalaVersion := "2.12.1"

cancelable in Global := true

fork in run := true

connectInput in run := true

enablePlugins(JavaAppPackaging)

// If you want an icon other than the default Java icon...
// javaOptions in run += "-Xdock:icon=/yourPathHere/SampleScala/scala.jpg"

// https://mvnrepository.com/artifact/org.scala-lang.modules/scala-swing_2.12
libraryDependencies += "org.scala-lang.modules" % "scala-swing_2.12" % "2.0.0"

