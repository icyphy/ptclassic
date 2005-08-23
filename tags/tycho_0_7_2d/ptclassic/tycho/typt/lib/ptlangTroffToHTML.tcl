# Procedure for converting the explanation section of a ptlang file
# from troff to HTML
#
# @Authors: Edward A. Lee
#
# @Version: @(#)ptlangTroffToHTML.tcl	1.7 10/03/96
#
# @Copyright (c) 1995-1998 The Regents of the University of California.
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

#### ptlangTroffToHTML
# Convert the "explanation" field of a ptlang file to a "htmldoc"
# section. Only rudimentary troff formatting is converted, so manual
# editing will often be required after the conversion. The arguments
# are filenames. A backup of the original file is made by appending the
# extension .bak to each specified file.
# NOTE: This procedure only works with very specifically formatted files.
# It is designed to expedite the conversion of the files in the ptolemy
# tree, not as a general utility.
#
proc ptlangTroffToHTML {args} {
    foreach file $args {
	set infile [open [::tycho::expandPath $file] r]
	set tmpfile [open "/tmp/$file" w]
        set delim 1
        set ineq 0
        while {[gets $infile lineIn] >= 0} {
	    # We put these replacements outside the explanation regexp
	    # because we did not find out about them until we ran this
	    # on all the stars and converted 'explanation' to 'html'
            regsub -all {\\fI} $lineIn {<i>} lineIn
            regsub -all {\\fP} $lineIn {</i>} lineIn
            regsub -all {\\fR} $lineIn {</i>} lineIn
	    regsub -all {\\fB} $lineIn {</b>} lineIn
            if [regexp {^[ 	]*explanation[ 	]*\{[ 	]*$} $lineIn] {
                # Replace with htmldoc
                puts $tmpfile "\thtmldoc \{"
                while {[gets $infile lineIn] >= 0} {
                    # Assume a lone close bracket is the end of the section.
                    if [regexp {^[ 	]*\}[ 	]*$} $lineIn] {break}

                    # Replace certain formatting controls
                    regsub -all ">" $lineIn {\&gt;} lineIn
                    regsub -all "<" $lineIn {\&lt;} lineIn
                    regsub -all {\\\\} $lineIn {\\} lineIn
                    regsub -all {REFERENCES} $lineIn {References} lineIn

                    # Handle some in-line equations
                    set line $lineIn
                    set newline {}
                    set end 0
                    set count 0
                    if $delim {
                        while {[regexp -indices {\$([^\$]*)\$} \
                                $line dummy eqn] != 0} {
                            puts "CHECK: Inline equations in $file"
                            set start [lindex $eqn 0]
                            set end [lindex $eqn 1]
                            if {$start > 1} {
                                append newline \
                                        [string range $line 0 [expr $start-2]]
                            }
                            set eqnt [string range $line $start $end]
                            regsub -all {~} $eqnt { } eqnt
                            regsub -all {[\(\)=]} $eqnt {</i>&<i>} eqnt
                            regsub -all {log|sin|cos|min|max} $eqnt {</i>&<i>} eqnt
                            regsub -all {[0-9]+} $eqnt {</i>&<i>} eqnt
                            regsub -all {sub[ 	]+([^ 	]*)} \
                                    $eqnt {<sub>\1</sub>} eqnt
                            regsub -all {sup[ 	]+([^ 	]*)} \
                                    $eqnt {<sup>\1</sup>} eqnt
                            append newline "<i>$eqnt</i>"
                            set line [string range $line [expr $end+2] end]
                            if {[incr count] > 10} {
                                puts "Warning: unprocessed equations"
                                break
                            }
                        }
                    }
                    append newline $line
                    set lineIn $newline

                    # Search for various common troff macros
                    switch -regexp -- $lineIn {
                        {^\.br} {
                            puts $tmpfile "<br>"
                        }
                        {^\.c([ 	]+|$)} {
                            if [regexp {.c[ 	]*(.*)$} $lineIn dummy entry] {
                                puts $tmpfile "<tt>$entry</tt>"
                            } {
                                if {[gets $infile lineIn] >= 0} {
                                    puts $tmpfile "<tt>$lineIn</tt>"
                                }
                            }
                        }
                        {^\.\(c} -
                        {^\.na} -
                        {^\.nf} {
                            puts $tmpfile "<pre>"
                        }
                        {^\.\)c} -
                        {^\.ad} -
                        {^\.fi} {
                            puts $tmpfile "</pre>"
                        }
                        {^\.[Hh]1} {
                            if [regexp {^\.[hH]1[ 	]*\"?([^\"]*)\"?} \
                                    $lineIn dummy entry] {
                                puts $tmpfile "<h1>$entry</h1>"
                            }
                        }
                        {^\.[Hh]2} {
                            if [regexp {^\.[hH]2[ 	]*\"?([^\"]*)\"?} \
                                    $lineIn dummy entry] {
                                puts $tmpfile "<h2>$entry</h2>"
                            }
                        }
                        {^\.[Hh]3} {
                            if [regexp {^\.[hH]3[ 	]*\"?([^\"]*)\"?} \
                                    $lineIn dummy entry] {
                                puts $tmpfile "<h3>$entry</h3>"
                            }
                        }
                        {^\.[Hh]4} {
                            if [regexp {^\.[hH]4[ 	]*\"?([^\"]*)\"?} \
                                    $lineIn dummy entry] {
                                puts $tmpfile "<h4>$entry</h4>"
                            }
                        }
                        {^\.I[dDeErR]} {
                            if [regexp {^\.I[deEr][ 	]*\"?([^\"]*)\"?} \
                                    $lineIn dummy entry] {
                                puts $tmpfile "<a name=\"$entry\"></a>"
                            }
                        }
                        {^\.[iS]r} {
                            if [regexp {^\.[iS]r][ 	]*\"?([^\"]*)\"?} \
                                    $lineIn dummy entry] {
                                puts $tmpfile "<a name=\"$entry\"></a>"
                            }
                        }
                        {^\.\(l} {
                            puts $tmpfile "<ul>"
                            while {[gets $infile lineIn] >= 0} {
                                if [regexp {^\.\)l} $lineIn] {break}
                                puts $tmpfile "<li> $lineIn"
                            }
                            puts $tmpfile "</ul>"
                        }
                        {^\.(ip|IP)} {
                            if [regexp {^\.(ip|IP)[ 	]*\"?([^\"]*)\"?} \
                                    $lineIn dummy dummy2 entry] {
                                puts $tmpfile "<p>$entry  "
                            } {
                                puts $tmpfile "<p>"
                            }
                        }
                        {^\.sp} -
                        {^\.lp} -
                        {^\.LP} -
                        {^\.PP} -
                        {^\.pp} {
                            puts $tmpfile {<p>}
                        }
                        {^\.[uU][Hh]} {
                            if [regexp {^\.[uU][hH][ 	]*\"?([^\"]*)\"?} \
                                    $lineIn dummy entry] {
                                puts $tmpfile "<h3>$entry</h3>"
                            }
                        }
                        {^\.cs} {}
                        {^\.ti} {
                            if {[gets $infile lineIn] >= 0} {
                                puts $tmpfile "<blockquote>$lineIn</blockquote>"
                            }
                        }
                        {^\.ul} {
                            if {[gets $infile lineIn] >= 0} {
                                puts $tmpfile "<em>$lineIn</em>"
                            }
                        }
                        {^\.(EQ)} {
                            puts $tmpfile "<pre>"
                            set ineq 1
                            puts "WARNING: Equation in $file"
                        }
                        {^\.(EN)} {
                            puts $tmpfile "</pre>"
                            set ineq 0
                        }
                        {^\delim off} {
                            if $ineq {
                                set delim 0
                            }
                        }
                        {^\delim \$\$} {
                            if $ineq {
                                set delim 1
                            }
                        }
                        {^\.(TS)} {
                            puts $tmpfile "<pre>"
                            puts "WARNING: Table in $file"
                        }
                        {^\.(TE)} {
                            puts $tmpfile "</pre>"
                        }
                        {^\.} {
                            puts "TROFF IGNORED: $lineIn"
                        }
                        default {
                            puts $tmpfile $lineIn
                        }
                    }
                }
            }
            puts $tmpfile $lineIn
        }
	close $infile
	close $tmpfile
#        exec rm -f $file.bak
#        exec mv $file $file.bak
#        exec mv "/tmp/$file" $file
    }
}
