# util.tcl: various utility procs defined in the tutorial text
#
# Copyright (c) 1998 The Regents of the University of California.
#       All Rights Reserved.  See the COPYRIGHT file for details.
#

# Toggle the label of a java button
proc toggleButton {b} {
    if { [java::prop $b label] == "Push me!" } {
	java::prop $b label "Again!"
    } else {
	java::prop $b label "Push me!"
    }
}

# Display a Java widget in a new top-level frame
proc showWidget {widget} {
    set window [java::new java.awt.Frame]
    $window setLocation 100 100
    $window {add java.awt.Component} $widget
    $window pack
    $window show
    return $widget
}

# Print the static fields of a class
proc showStaticFields {class} {
    foreach f [java::info fields -static java.util.Calendar] {
	puts "$f = [java::field java.util.Calendar $f]"
    }
}

# Print the JDK property list
proc jdkProperties {} {
    set props [java::call System getProperties]
    set names [$props propertyNames]
    while { [$names hasMoreElements] } {
	set name [$names nextElement]
	puts "$name=[$props getProperty $name]"
    }
}

# Print JDK version info
proc jdkVersion {} {
    global tcl_version tcl_patchLevel env
    puts "env(CLASSPATH):   $env(CLASSPATH)\n"
    puts "java.class.path property:\
            [java::call System getProperty java.class.path]\n"
    puts -nonewline "JDK version: [java::call System getProperty \
	    java.version]"
    if [info exists ::java::patchLevel] {
	puts ", Tcl Blend patch level: $::java::patchLevel"
    } else {
	puts ""
    }
    puts "Tcl version: $tcl_version, \
	    Tcl patch level: $tcl_patchLevel"
    puts "Java package: [package versions java] \
	    info loaded: [info loaded]"   
}

# Capture output to System.out
proc jdkCapture {script varName} {
    upvar $varName output
    set stream [java::new java.io.ByteArrayOutputStream]
    set printStream [java::new \
	    {java.io.PrintStream java.io.OutputStream} $stream]
    set stdout [java::field System out]
    java::call System setOut $printStream
    set result [uplevel $script]
    java::call System setOut $stdout
    $printStream flush
    set output [$stream toString]
    return $result
}

# Print the most recent Java stack trace
proc jdkStackTrace {} {
    global errorCode errorInfo
    if { [string match {JAVA*} $errorCode] } {
	set exception [lindex $errorCode 1]
	set stream [java::new java.io.ByteArrayOutputStream]
	set printWriter [java::new \
		{java.io.PrintWriter java.io.OutputStream} $stream]
	$exception {printStackTrace java.io.PrintWriter} $printWriter
	$printWriter flush

	puts "[$exception getMessage]"
	puts "    while executing"
	puts "[$stream toString]"
	puts "    while executing"
    }
    puts $errorInfo
}

