# Script to check out incremental linking
# Version identification:
# $Id$
# Copyright (c) %Q% The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
# 
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# 
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
# Programmer: Christopher Hylands
# Date of creation: 5/24/96

proc runAllIncLinkTests {} {
  global env

  if [catch {set arch $env(PTARCH)}] {
    set env(PTARCH) $env(ARCH)
    set arch $env(ARCH)
  }

  exec make realclean

  loadSyntax
  loadUndefined
  reset
  loadReload
  reset
  twoUniverses
  reset
  puts "*** Skipping running loadLoadReset, it dumps core"
  #loadLoadReset

  multilinkTest
  reset
  permlinkLink
  reset
  permlinkPerm
  
  reset
  linkSysDerived
  reset
  linkSysDerived permlink
  reset

  puts "linkCompileSDF should fail, there is a known bug"
  puts "see sdf/298: compile-sdf fails on incrementally linked stars"
  linkCompileSDF

  #loadReload permlink
  #twoUniverses permlink
  #loadLoadReset permlink


}


#####
# The actual tests


# Test out the syntax of the commands
proc loadSyntax {} {
  set procName "loadSyntax"
  puts "================ $procName start"
  t_syntax link "incorrect usage: should be \"link <objfile>\""
  t_syntax permlink \
    "incorrect usage: should be \"permlink <file1> ?<file2> ...?\""
  t_syntax multilink \
    "incorrect usage: should be \"multilink <file1> ?<file2> ...?\""

  t_syntax "link foo bar" "incorrect usage: should be \"link <objfile>\""
  puts "================ $procName done"
}

#
# Try loading a star that depends on a permlinked star that has not
# been loaded yet.  On sun4 with dlopen() style linking, this results in
# ptcl exiting.
proc loadUndefined { {linkCommand link} } {

  set procName "loadUndefined \{ $linkCommand \}"
  puts "================ $procName start"

  global env
  set arch $env(PTARCH)
  if { $arch == "sun4" } {
    oneError;
    puts " $procName: error PTARCH = $arch, with dlopen() this test would exit ptcl"
  }
  puts "$procName: Load a star that is has undefined references in it"

  set errVar [catch {print MyRamp}]
  if {$errVar != 1 } {
    oneError;
    puts "$procName: Warning this test should be run before MyRamp is linked in"
    puts "$procName: If MyRamp was permlinked in, this test will not behave"
    puts "$procName: as expected."
  }

  set errVar [catch {t_compileLoadMyRamp main $linkCommand MyRampDerived} \
	      errMsg]
  if {$errVar == 0 } {
    oneError;
    puts " $procName: That command should have failed!"
  } {
    puts "$procName: Ok, that command failed as expected"
    puts "$procName: message was: $errMsg"
  }
  puts "================ $procName done"
}

#
# Load a star into one universe, run it, modify it, reload it, run it.
proc loadReload { {linkCommand link} } {

  set procName "loadReload \{ $linkCommand \}"
  puts "================ $procName start"

  puts "$procName: Set up the initial star with step = 1.0"
  t_cpSDFMyRampDist

  set step1 [t_compileLoadMyRamp main $linkCommand]
  puts $step1

  puts "$procName: Run sed on the star to set up step = 2.0"
  t_sedSDFMyRampDist 2.0

  #delstar printer
  #delstar rampit

  reset
  set step2 [t_compileLoadMyRamp main $linkCommand]
  puts $step2
  t_shouldBeDifferent "$procName" "$step1" "$step2"

  puts "================ $procName done"
}

#
# Load many stars by taking one star and modifying it
proc loadMany { {linkCommand link} } {

  set procName "loadMany \{ $linkCommand \}"
  puts "================ $procName start"

  set outFile myrun10
  set objname "SDFMyRamp.o"

  for {set i 0} {$i < 30} {incr i} {
    # Change the name of the star and the step
    puts "$procName: Create MyRamp${i}"
    exec sed s\/MyRamp\/MyRamp$i\/ SDFMyRamp-dist.pl | sed s\/1.0\/$i.0\/ > SDFMyRamp$i.pl

    #catch {exec diff SDFMyRamp-dist.pl SDFMyRamp$i.pl} msg
    #puts $msg

    # Create the .o file
    if [catch {exec make SDFMyRamp$i.o > /tmp/MyRamp.make} errVal] {
      oneError;
      puts " Make returned [t_getResults /tmp/MyRamp.make] returned $errVal"
    }

    puts "About to $linkCommand SDFMyRamp$i.o"
    $linkCommand SDFMyRamp$i.o
    puts "Linking successful"

    # Create a Universe and run it
    star rampit MyRamp$i
    star printer Printer
    connect rampit output printer input
    setstate printer fileName $outFile
    run 10
    wrapup
    reset
    puts "Results: [t_getResults $outFile]"
  }

  puts "================ $procName done"
}



#
# load a star into one universe, change the star,
# load the star into another universe
#
proc twoUniverses { { linkCommand link } } {
  set procName "twoUniverses \{ $linkCommand \}"
  puts "================ $procName start"

  puts "$procName: Set up the initial star with step = 1.0"
  t_cpSDFMyRampDist
  puts "$procName: Creating step1_0 universe"
  set step1 [t_compileLoadMyRamp step1_0 $linkCommand]
  puts "$step1"
  set expectedResults1 "{0.0	} {1.0	} {2.0	} {3.0	} {4.0	} {5.0	} {6.0	} {7.0	} {8.0	} {9.0	} {}"
  t_shouldBeTheSame "twoUniverses, step1_0 against expected" "$step1" \
    "$expectedResults1"

  puts "$procName: Run sed on the star to set up step = 2.5"
  t_sedSDFMyRampDist 2.5

  puts "$procName: Creating step2_5 universe"
  set step2 [t_compileLoadMyRamp step2_5 $linkCommand]
  puts "$step2"

 set expectedResults2 "{0.0	} {2.5	} {5.0	} {7.5	} {10.0	} {12.5	} {15.0	} {17.5	} {20.0	} {22.5	} {}"
  t_shouldBeTheSame "twoUniverses, step2_5 against expected" "$step2" \
    "$expectedResults2"

  t_shouldBeDifferent "twoUniverses, step2_5 section" "$step1" "$step2"


  puts "$procName: Running step1_0 again"
  curuniverse step1_0
  run 10

  wrapup
  set step1a [t_getResults myrun10]

  t_shouldBeTheSame "twoUniverses, step1_0 section" "$step1" "$step1a"

  puts "$procName: Running step2_5 again"
  curuniverse step2_5
  run 10
  wrapup
  set step2a [t_getResults myrun10]

  t_shouldBeTheSame "twoUniverses, step2_5 section" "$step2" "$step2a"

  puts "================ $procName done"
}

proc loadLoadReset { {linkCommand link} } {
  
  set procName "loadLoadReset \{ $linkCommand \}"
  puts "================ $procName start"

  puts [t_compileLoadMyRamp main $linkCommand]
  puts [t_compileLoadMyRamp main $linkCommand]

  # This reset will trigger a core dump in all ptcls if you run it in
  # a fresh ptcl.  If there are a few universes, you may need to delete
  # them first
  # If there is a reset between concurrent links, the the core dump does
  # not happen
  # The core dump happens in Geodesic::~Geodesic
  # with the call to Geodesic::disconnect because destinationPort is 0x0
  
  puts "$procName: About to reset"
  reset
  puts "================ $procName done"

}


proc multilinkTest {} {
  set procName "mulilinkTest"
  puts "================ $procName start"

  puts "$procName: Set up the initial star with step = 1.0"
  t_cpSDFMyRampDist

  set obj1 SDFMyRamp.o
  set obj2 SDFMyRampDerived.o
  puts "About to make $obj1 $obj2"
  set errVal [catch {exec make $obj1 $obj2 > /tmp/MyRamp.make} ]
  puts "[t_getResults /tmp/MyRamp.make] returned $errVal"

  puts "About to   multilink $obj1 $obj2"
  multilink $obj1 $obj2
  set outFile myrun10
  exec rm -f $outFile

  star rampit MyRampDerived
  star printer Printer
  connect rampit output printer input
  setstate printer fileName $outFile
  run 10
  wrapup
  set results "[t_getResults $outFile]"
  puts $results
  set expectedResults2 "{0.0	} {3.0	} {6.0	} {9.0	} {12.0	} {15.0	} {18.0	} {21.0	} {24.0	} {27.0	} {}"
  t_shouldBeTheSame "$procName" "$results" "$expectedResults2"

  puts "================ $procName done"
}

# Test out permalink against a star
# Then we link a derived star against the first star
proc permlinkLink {} {
  set procName "permlinkLink"
  puts "================ $procName start"
  puts "After running this test, MyRamp will be permanently linked"
  puts "so further links will fail"


  puts "$procName: Set up the initial star with step = 1.0"
  t_cpSDFMyRampDist

  set step1 [t_compileLoadMyRamp main permlink MyRamp]
  puts "$step1"
  set expectedResults1 "{0.0	} {1.0	} {2.0	} {3.0	} {4.0	} {5.0	} {6.0	} {7.0	} {8.0	} {9.0	} {}"
  t_shouldBeTheSame "$procName" "$step1" "$expectedResults1"

  # If this reset is not there, we will dump core
  reset

  set step2 [t_compileLoadMyRamp main link MyRampDerived]
  puts "$step2"
  set expectedResults2 "{0.0	} {3.0	} {6.0	} {9.0	} {12.0	} {15.0	} {18.0	} {21.0	} {24.0	} {27.0	} {}"
  t_shouldBeTheSame "$procName" "$step2" "$expectedResults2"

  t_shouldBeDifferent "$procName" "$step1" "$step2"
  puts "================ $procName done"
}


# Test out permalink against a star
# Then we permlink a derived star against the first star
proc permlinkPerm {} {
  set procName "permlinkPerm"
  puts "================ $procName start"
  puts "After running this test, MyRamp and MyRampDerived will be"
  puts "permanently linked so further non perm links of these stars may fail"
  puts "----This test will fail on Ptolemy ports that use ld -A style linking"
  puts "$procName: Set up the initial star with step = 1.0"
  t_cpSDFMyRampDist

  reset
  set errVar [catch { t_compileLoadMyRamp main permlink MyRamp } step1]
  puts "$step1"
  set expectedResults1 "{0.0	} {1.0	} {2.0	} {3.0	} {4.0	} {5.0	} {6.0	} {7.0	} {8.0	} {9.0	} {}"
  t_shouldBeTheSame "$procName #1" $step1 "$expectedResults1"

  # If this reset is not there, we will dump core
  reset

  set errVar [catch {t_compileLoadMyRamp main permlink MyRampDerived} step2]
  puts "$step2"
  set expectedResults2 "{0.0	} {3.0	} {6.0	} {9.0	} {12.0	} {15.0	} {18.0	} {21.0	} {24.0	} {27.0	} {}"
  t_shouldBeTheSame "$procName" "$step2" "$expectedResults2"

  t_shouldBeDifferent "$procName" "$step1" "$step2"

  reset

  puts "$procName: Now try relinking in MyRamp (This will fail on sun4, hppa, mips)"
  puts "$procName: Run sed on the MyRamp star to set up step = 2.5"
  t_sedSDFMyRampDist 2.5

  set errVar [catch {t_compileLoadMyRamp main link MyRamp} step3 ]
  puts "$step3"

  set expectedResults3 "{0.0	} {2.5	} {5.0	} {7.5	} {10.0	} {12.5	} {15.0	} {17.5	} {20.0	} {22.5	} {}"
  t_shouldBeTheSame "$procName, re link against expected" "$step3" \
    "$expectedResults3"

  puts "================ $procName done"


}



proc linkSysDerived { {linkCommand link} } {
  set procName "linkSysDerived \{ $linkCommand \}"
  puts "================ $procName start"
  puts "$linkCommand a star derived against a system star"

  set step1 [t_compileLoadMyRamp main $linkCommand MyRampSysDerived]
  puts "$step1"
  set expectedResults1 "{0.0	} {4.0	} {8.0	} {12.0	} {16.0	} {20.0	} {24.0	} {28.0	} {32.0	} {36.0	} {}"
  t_shouldBeTheSame "$procName" "$step1" "$expectedResults1"
  puts "================ $procName done"
}

proc linkCGC { {linkCommand link} } {
  set procName "linkCGC \{ $linkCommand \}"
  puts "================ $procName start"
  puts "$linkCommand a cgc star"

  set step1 [t_compileLoadMyRamp main $linkCommand MyRamp CGC]
  puts "$step1"

  puts "================ $procName done"
}


# Link a star, try to produce Compile SDF
# This won't work because compile-SDF can't find SDFMyRamp.h while it is
# compiling code.cc in ~/PTOLEMY_SYSTEMS

# see sdf/298: compile-sdf fails on incrementally linked stars
proc linkCompileSDF { {linkCommand link} } {

  set procName "linkCompileSDF"
  puts "================ $procName start"

  puts "$procName: Set up the initial star with step = 1.0"
  t_cpSDFMyRampDist

  target compile-SDF
  targetparam directory "./"
  puts "This should return an error, but there is a another bug somewhere"
  puts "  that fails to return an error code if compile-SDF fails"
  set step1 [t_compileLoadMyRamp main $linkCommand MyRamp SDF compile-SDF]
  puts "$step1"

  puts "================ $procName done"
}


#
##############################################################################
# House keeping functions below here
# They should all start with t_

proc t_getResults { outFile } {

  set results ""
  if {[file exists $outFile]} {
    set fileHandle [open $outFile r]
    while {![eof $fileHandle]} {
      gets $fileHandle line
      #puts $line
      lappend results $line	
    }
    #puts $results
    close $fileHandle
  }
  return "$results"
}

proc t_shouldBeDifferent { procName string1 string2 } {
  if { "$string1" != "$string2"} {
    puts "$procName: ok, the two results were not the same"
  } else {
    oneError;
    puts " error $procName test failed!  The results:"
    puts $string1
    puts "and the results"
    puts $string2
    puts "should not be equal"
  }
}

proc t_shouldBeTheSame { procName string1 string2 } {
  if { "$string1" == "$string2"} {
    puts "$procName: ok, the two results were the same" 
  } else {
    oneError;
    puts " $procName test failed!  The results:"
    puts $string1
    puts "and the results"
    puts $string2
    puts "should be equal"
  }
}

# Compile and Load a star
# This procedure is used by other tests
proc t_compileLoadMyRamp { {univName main} {linkCommand link} \
	{starName MyRamp} {domain SDF} {target default-SDF} } {

  set objname "$domain$starName.o"
  set outFile myrun10
  exec rm -f $outFile

  # Compile the star if necessary
  puts "About to make $objname"
  catch {exec make VPATH=. $objname > /tmp/MyRamp.make} errVal 
  puts "[t_getResults /tmp/MyRamp.make] returned $errVal"

  puts "About to $linkCommand $objname"
  $linkCommand $objname
  puts "Linking successful"


  if { $univName == "main" } {
    puts "Universe name = main"
  } else {
    puts "Universe name = $univName"
    newuniverse $univName
  }

  target $target

  if [catch {star rampit $starName} errMsg] {
      oneError;
      puts " (t_compileLoadRamp):\n {star rampit $starName} failed: $errMsg"
  }

  star printer Printer
  connect rampit output printer input
  setstate printer fileName $outFile
  if [catch {run 10} errMsg] {
      puts " (t_compileLoadRamp) run failed: $errMsg"
      oneError;
  }
  if [catch {wrapup} errMsg] {
      oneError;
      puts " (t_compileLoadRamp) wrapup failed: $errMsg"

  }

  #setstate printer fileName myrun100
  #run 100
  #setstate printer fileName myrun1000
  #run 1000

  return "[t_getResults $outFile]"

}


proc t_syntax { command expectedMsg } {
  puts -nonewline "Testing syntax of \"$command\""
  set retval [catch {eval $command} errMsg]
  if { $retval != 0 } {
    if { "$errMsg" != "$expectedMsg" } {
      puts "\n*** Error (t_syntax): The command \"$command\" resulted in"
      puts "    $errMsg"
      puts "    is not equal to"
      puts "    $expectedMsg"
      oneError;
    } {
      puts " \tError message was as expected"
    }
  } {
    puts " \tCommand was successful"
  }
}

proc t_cpSDFMyRampDist {} {
  # Need to fix the version file names here, or ptlang barfs
  exec sed -e s\/MyRamp-dist\/MyRamp\/ SDFMyRamp-dist.pl > SDFMyRamp.pl
}

proc t_sedSDFMyRampDist {newValue} {
  # Need to fix the version file names here
  exec sed -e s\/\ 1.0\ \/\ $newValue\ \/ -e s\/MyRamp-dist\/MyRamp\/ SDFMyRamp-dist.pl > SDFMyRamp.pl

}

# Count errors
proc oneError {} { global errorCount;  incr errorCount; puts "***ERROR***" }
set errorCount 0

# Now run the tests
runAllIncLinkTests

puts "$errorCount errors, 1 expected"
