# Tests for the Class model.
#
# @Author: Cliff Cordeiro
#
# @Version: $Id$
#
# @Copyright (c) 1997 The Regents of the University of California.
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
#######################################################################

# Tycho test bed, see $TYCHO/doc/coding/testing.html for more information.

# Load up the test definitions.
if {[string compare test [info procs test]] == 1} then { 
    source [file join $TYCHO kernel test testDefs.tcl]
} {}

# Uncomment this to get a full report, or set in your Tcl shell window.
# set VERBOSE 1

# If a file contains non-graphical tests, then it should be named .tcl
# If a file contains graphical tests, then it should be called .itcl
# 
# It would be nice if the tests would work in a vanilla itkwish binary.
# Check for necessary classes and adjust the auto_path accordingly.
#
if {[namespace tycho {info classes Class}] == {} } {
    uplevel #0 {
	set ::auto_path [linsert $auto_path 0 [file join $TYCHO kernel]] 
    }
}

######################################################################
#### Create a simple class
# 
test Class-1.1 {A simple class, check the description} {
    catch {delete object boo}
    ::tycho::Class boo
    boo method pooh
    boo variable chew
    boo describe
} {
method pooh
variable chew
}

######################################################################
#### Test toplevel Class attributes
# 
test Class-1.2 {Test toplevel Class attributes} {
    catch {delete object boo}
    ::tycho::Class boo
    boo modelconfigure -source "c.itcl"
    boo modelconfigure -language "COBOL"
    boo modelconfigure -name "::tycho::Glass"
    boo modelconfigure -package "brown paper"
    boo modelconfigure -protection "public"
    boo modelconfigure -modifiers "other"
    boo modelcget -source
} {c.itcl}

######################################################################
#### Test parents
# 
test Class-1.3 {Test parents} {
    catch {delete object boo}
    ::tycho::Class boo
    boo parent mom -inheritance implementation
    boo parent dad -inheritance interface
    set x [boo parents]
    lsort $x
} {dad mom}

######################################################################
#### Test Constructor
# 
test Class-1.4 {Test Constructor} {
    catch {delete object boo}
    ::tycho::Class boo
    boo Constructor Tinker -protection private -modifiers duh -exceptions but
    boo Constructor Toy -protection protected
    set x [boo Constructors]
    lsort $x
} {Tinker Toy}

######################################################################
#### Test Destructor
# 
test Class-1.5 {Test Destructor} {
    catch {delete object boo}
    ::tycho::Class boo
    boo Destructor Tango -protection private -modifiers adjective \
            -exceptions irrelevant
    boo Destructor Cash -protection package
    set x [boo Destructors]
    lsort -decreasing $x
} {Tango Cash}


######################################################################
#### Test method
# 
test Class-1.6 {Test method} {
    catch {delete object boo}
    ::tycho::Class boo
    boo method rhythm -protection private -modifiers green -type Int \
            -scope class -exceptions overruled
    boo method is -scope instance -type "O positive"
    boo method a -protection private -exceptions "because I said so"
    boo method dancer -type "B negative"
    set x [boo methods -protection public]
    lsort -decreasing $x
} {is dancer}

######################################################################
#### Test variable
# 
test Class-1.7 {Test variable} {
    catch {delete object boo}
    ::tycho::Class boo
    boo variable speed -protection protected -scope instance -type o \
            -modifiers louder -default faster
    boo variable racer -protection public -type hype -default renault
    boo variable go -scope class
    set x [boo variables -scope instance]
    lsort -decreasing $x
} {speed racer}

######################################################################
#### Test annotation
# 
test Class-1.8 {Test annotation} {
    catch {delete object boo}
    ::tycho::Class boo
    boo annotation description "This is."
    boo annotation explanation "This is it."
    set retval ""
    lappend retval [boo get annotation description]
    lappend retval [boo get annotation explanation]
} {{This is.} {This is it.}}

######################################################################
#### Test nested annotations
# 
test Class-1.9 {Test nested annotations} {
    catch {delete object boo}
    ::tycho::Class boo
    boo method heimlich
    boo annotation heimlich.manuever "Is that how you spell it?"
    boo variable temperature
    boo annotation temperature.hot "Lows in mid 60s."
    set retval ""
    lappend retval [boo get annotation heimlich.manuever]
    lappend retval [boo get annotation temperature.hot]
} {{Is that how you spell it?} {Lows in mid 60s.}}

######################################################################
#### Test nested parameters
# 
test Class-1.10 {Test nested parameters} {
    catch {delete object boo}
    ::tycho::Class boo
    boo method heimlich
    boo parameter heimlich manuever -type int -optional 1 -default "foot"
    boo variable weather
    boo parameter weather temperature -type double
    boo method rhythm -protection private
    boo parameter rhythm frequency -optional 0
    set retval ""
    lappend retval [boo parameters heimlich]
    lappend retval [boo parameters weather]
    lappend retval [boo parameters rhythm]
    lappend retval [boo itemcget parameter heimlich.manuever -type method]
} {manuever temperature frequency int}

