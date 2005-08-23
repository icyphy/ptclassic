# $Header$

#
# SearchBox mega widget
#   incremental and regular expression searching in a text widget
#   for examples of use, see TkMan and tkinfo
#
#   Copyright (C) 1993-1996 Tom Phelps (phelps@cs.Berkeley.EDU)
#
# extracted from TkMan, and then used by TkMan and NBT 6-Aug-93
#
# 19-Aug  made more robust (Kennard White)
#
# 1994
# 19-Apr  single character "|" as search string fixed (Greg McFarlane)
# 26-Aug  incremental search highlights match, use pickplace instead of top line of screen
# 25-Dec  updated for Tk 4.0
# 30      case insensitive iff all lowercase
#
# 1995
#  1-Jul  patches for change in search command in b4/final Tk 4.0
#
# 1996
#  6-Feb  Tk 4.1 can search for tags backward
#  3-Jun  wraparound behavior gives warnings (Rick Macdonald)

# name space use: prefixes searchbox, sb, sbx
# does NOT require taputils.tcl


set sbx(boilerplate) {
Searchbox
Copyright (c) 1993-1996  T.A. Phelps

Permission to use, copy, modify, and distribute this software and its
documentation for educational, research and non-profit purposes, 
without fee, and without a written agreement is hereby granted, 
provided that the above copyright notice and the following three 
paragraphs appear in all copies.  

Permission to incorporate this software into commercial products may 
be obtained from the Office of Technology Licensing, 2150 Shattuck 
Avenue, Suite 510, Berkeley, CA  94704. 

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE 
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF 
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, 
ENHANCEMENTS, OR MODIFICATIONS.
}


### default key bindings
# ""=unmodified, A=alt, C=control, M=meta
# (Shift should not be used as a modifier.  Different keyboards
#  require shift for different characters, as for instance the
#  less than and greater than signs.  Any shift modifier is discarded
#  before trying to match a binding.  Thus, to make a binding for
#  capital A, just write "sb(key,-A)" or "sb(key,M-A)" or sb(key,C-A)"--
#  ignoring the shift altogether.)

set sb(key,*) "add modifiers in this order: M, C, A"

# Emacs-like

set sb(key,C-x) exchangepointandmark
set sb(key,C-space) setmark
set sb(key,-Delete) pageup
set sb(key,C-h) pageup
set sb(key,M-v) pageup
set sb(key,-BackSpace) pageup
set sb(key,-space) pagedown
set sb(key,C-v) pagedown
set sb(key,-Escape) searchkill
set sb(key,C-g) searchkill
set sb(key,C-n) nextline
set sb(key,C-p) prevline
set sb(key,-Up) prevline
set sb(key,-Down) nextline
set sb(key,-Left) prevline
set sb(key,-Right) nextline
set sb(key,C-s) incrsearch
set sb(key,C-r) revincrsearch
set sb(key,M-question) help
set sb(key,-Help) help
set sb(key,C-l) clearscreen


# vi, when don't conflict with above

set sb(key,C-f) pagedown
set sb(key,C-b) pageup


# for Sun keyboards(?)

set sb(key,-F27) pagestart
set sb(key,-F29) pageup
set sb(key,-F35) pagedown
set sb(key,-R13) pageend


set sbx(wrap-msg) "No more matches; try again to wrap around"

# cursor keys -- built into Tk 4.0

#set sb(key,-Prior) pageup
#set sb(key,-Next) pagedown
#set sb(key,-Home) pagestart
#set sb(key,-End) pageend
set sb(key,M-less) pagestart
set sb(key,M-greater) pageend

foreach e [array names sb] {
	set sbdef($e) $sb($e)
}


### environment checks
if {[info tclversion]<7.4 || $tk_version<4.0} {
	puts stderr "Tcl 7.4/Tk 4.0 minimum versions required"
	puts stderr "You have Tcl [info tclversion]/Tk $tk_version"
	exit 1
}




#--------------------------------------------------
#
# searchboxSearch -- initiate an exact or regular expression search
#
# params
#    str = string to search for
#    regexp = boolean - regular expression search? (otherwise exact)
#    casesen = case sensitive?
#    tag = tag to associate with matches
#       (do a `tag bind' in the text widget for this tag)
#    w = text widget
#    wmsg = (optional) window to show status messages
#    wcnt = (optional) widget to show number of matches
#
#
#--------------------------------------------------

proc searchboxSearch {pat regexp casesen tag t {wmsg ""} {wcnt ""}} {
	global sbx

	if {$casesen==-1} {set casesen [expr {[string tolower $pat]!=$pat}]}
	set caseopt [expr !$casesen?"-nocase":""]
	if {$regexp} {set type "regexp"} {set type "exact"}

	if {$pat==""} {
		winstderr $wmsg "Nothing to search for!  Type in a [expr $regexp?{regular expression}:{string}]."
		return 0
	}

	# check for bad regular expression
	if {$type=="regexp"&&[catch {regexp $pat bozomaniac}]} {
		winstderr $wmsg "Malformed regular expression."
		return 0
	}

	$t tag remove $tag 1.0 end

	# wow, heaps faster with Tk 4.0's built-in searching
	set cnt 0
	set index 1.0
	set len 0
	while {[set index [eval "$t search -forwards -$type $caseopt -count len -- {$pat} {$index+$len chars} end"]]!=""} {
		$t tag add $tag $index "$index + $len chars"
		incr cnt
	}


	if {$cnt==0} {set txt "no matches"} elseif {$cnt==1} {set txt "$cnt match"} {set txt "$cnt matches"}
#	winstdout $wcnt $txt
	$wcnt configure -text $txt

	# show the first match
	set sbx(ratend$t) 0; set sbx(ratstart$t) 0
	searchboxNext $tag $t $wmsg 0

	return $cnt
}



#--------------------------------------------------
#
# searchboxNext -- show the next match
#
# params
#    tag = tag to search for (see searchboxRegexpSearch)
#    w = text widget
#    wmsg = window to show status messages
#    next/prev = fractional position at which to search looking for next match
#
#--------------------------------------------------

proc searchboxNext {tag t {wmsg ""} {yoff ""}} {
	global sbx

	if {[$t tag ranges $tag]==""} { winstderr $wmsg "No matches!"; return }

	if {$yoff==""} {set yoff [winfo height $t]}
	set hit [$t tag nextrange $tag [$t index @0,$yoff]]
	if {$hit==""} {
		if {$sbx(ratend$t)} {
			set hit [$t tag nextrange $tag 1.0]
			set sbx(ratend$t) 0
		} else {
			winstderr $wmsg $sbx(wrap-msg)
			set sbx(ratend$t) 1
		}
	}
	if {$hit!=""} {
		$t see [lindex $hit 0]
#		winstdout $wmsg ""; # clear wraparound messages
	}
}

proc searchboxPrev {tag t {wmsg ""}} {
	global sbx

	if {[$t tag ranges $tag]==""} { winstderr $wmsg "No matches!"; return }

	set hit [$t tag prevrange $tag [$t index @0,0]]
	if {$hit==""} {
		if {$sbx(ratstart$t)} {
			set hit [$t tag prevrange $tag end]
			set sbx(ratstart$t) 0
		} else {
			winstderr $wmsg $sbx(wrap-msg)
			set sbx(ratstart$t) 1
		}
	}

	if {$hit!=""} {
		$t see [lindex $hit 0]
#		winstdout $wmsg ""; # clear wraparound messages
	}
}


#--------------------------------------------------
#
# searchboxKeyNav -- keyboard-based navigation and searching
#
#   maybe separate out some commands so incrsearch doesn't have to wade through
#
# params:
#   m = modifier key
#   k = key
#   casesen = case sensitive?
#   t = text widget
#   wmsg = label in which to show incremental search string
#   mode = 1=>match on first character of line, 0=>match anywhere
#
#--------------------------------------------------

proc searchboxKeyNav {m k casesen t {wmsg ""} {firstmode 0}} {
#puts "$m $k $casesen $t $wmsg $firstmode"
	global sb sbx

	if {[regexp {(Shift|Control|Meta|Alt)_.} $k]} {return 0}
	if {![info exists sbx(try$t)]} {
		set sbx(try$t) 0
		set sbx(vect$t) 1
		set sbx(lastkeys$t) ""
		set sbx(iatstart$t) 0; set sbx(iatend$t) 0
		# other apps may wish to seed default search string
		if ![info exists sbx(lastkeys-old$t)] {set sbx(lastkeys-old$t) ""}
	}


	# some translations
	if {!$firstmode && ($sbx(try$t) || $sbx(lastkeys$t)!="")} {
		switch -exact -- $k {
			space {set k " "}
			BackSpace -
			Delete {
				set k ""
				set last [expr [string length $sbx(lastkeys$t)]-2]
				set sbx(lastkeys$t) [string range $sbx(lastkeys$t) 0 $last]
				set sbx(try$t) 1
			}
			default { if {$m==""||$m=="S"} {set k [name2char $k]} }
		}
	}


	# commands
	set m [string trimright $m "S"]; # strip shift as a modifier
	set mk $m-$k
#puts "searchbox: mk = $mk"
	if {$m=="literal"} {set op $k} \
	elseif {[info exists sb(key,$mk)]} {set op $sb(key,$mk)} \
	elseif {$m=="" && [string length $k]<=1} {set op default} \
	else {return 0}

#puts stdout "trying for a match on $mk"
	switch -exact -- $op {
# help not application independent
#		help {$t.occ.m invoke Help; return}
		exchangepointandmark {
			# yview w/o parameter should return current value
			set tmp [$t index @0,0]
			$t yview xmark
			update
			$t mark set xmark $tmp
		}
		setmark {$t mark set xmark [$t index @0,0]}
		pageup {$t yview scroll -1 pages}
		pagedown {$t yview scroll 1 pages}
		pagestart {$t yview moveto 0}
		pageend {$t yview moveto 1}
		searchkill {
#puts stderr "killing search"
			if {$sbx(lastkeys$t)!=""} {set sbx(lastkeys-old$t) $sbx(lastkeys$t)}
			set sbx(lastkeys$t) ""; set sbx(try$t) 0
			winstdout $wmsg ""
		}
		clearscreen {winstdout $wmsg ""}
		nextline {$t yview scroll 1 units}
		prevline {$t yview scroll -1 units}
		default {
#puts stderr "incr search...  op=$op"
			# incremental search
			if {$op=="incrsearch"} {
				# C-s C-s retrieves last search pattern
				if {$sbx(try$t)&&$sbx(lastkeys$t)==""} {set sbx(lastkeys$t) $sbx(lastkeys-old$t)}
#				if {$sbx(lastkeys$t)==""} {$t tag remove isearch 1.0 end}
				set sbx(vect$t) 1; set sbx(try$t) 1
			} elseif {$op=="revincrsearch"} {
				if {$sbx(try$t)&&$sbx(lastkeys$t)==""} {set sbx(lastkeys$t) $sbx(lastkeys-old$t)}
#				if {$sbx(lastkeys$t)==""} {$t tag remove isearch 1.0 end}
				set sbx(vect$t) -1; set sbx(try$t) 1
			} elseif {$firstmode} {
#				$t tag remove isearch 1.0 end
				set sbx(lastkeys$t) $k
				set sbx(try$t) 1
			} elseif {$sbx(try$t)} {
				append sbx(lastkeys$t) $k
			}

			set keys $sbx(lastkeys$t)
			if {$casesen==-1} {set casesen [expr {[string tolower $keys]!=$keys}]}
			set caseopt [expr !$casesen?"-nocase":""]
			if {$sbx(try$t)==0 && $keys==""} {return 0}
			winstdout $wmsg "Searching for \"$keys\" ..."
#			if {$sbx(try$t)==0 || $keys==""} {return 1}


			if {$firstmode} {
				set start 1.0; set end end
			} elseif {[set ranges [$t tag ranges isearch]]!=""} {
				set start [lindex $ranges 0]; set end 1.0
				if {$op=="incrsearch"} {set start [$t index "$start+1c"]}
				if {$sbx(vect$t)==1} {set end "end"}
			} else {
				if {$sbx(vect$t)==1} {
					set start [$t index @0,0]; set end "end"
				} else {
					set start [$t index @0,[winfo height $t]]; set end "1.0"
				}
			}

			# need case sensitive switch here
			set dir [expr {$sbx(vect$t)==1?"-forwards":"-backwards"}]
			set type [expr $firstmode?"-regexp":"-exact"]
			set pfx [expr $firstmode?"^":""]
			set search "$t search $dir $type $caseopt -count klen -- {$pfx$keys} $start"
#DEBUG {puts "$search ... $end"}
			set found [eval "$search $end"]
			set anywhere [eval $search]
#puts "searching ... $t search $sbx(vect$t) -exact -- $keys $start klen = $found"
#puts "$op... $t search $dir $type $caseopt -count klen -- {$pfx$keys} $start"

			# wraparound behavior
			if {$anywhere!=""} {
				if {$found!=""} {
					$t tag remove isearch 1.0 end
					$t tag add isearch $found "$found + $klen c"
					$t see $found
#					winstdout $wmsg ""; # clear wraparound messages(?)
					set sbx(iatstart$t) 0; set sbx(iatend$t) 0
				} else {
					if {$sbx(vect$t)==1} {
						if {$sbx(iatstart$t)} {
							$t yview moveto 0; $t tag remove isearch 1.0 end
							return [searchboxKeyNav $m $k $casesen $t $wmsg $firstmode]
						} else {
							winstderr $wmsg $sbx(wrap-msg)
							set sbx(iatstart$t) 1
						}
					} else {
						if {$sbx(iatend$t)} {
							$t yview moveto 1; $t tag remove isearch 1.0 end
							return [searchboxKeyNav $m $k $casesen $t $wmsg $firstmode]
						} else {
							winstderr $wmsg $sbx(wrap-msg)
							set sbx(iatend$t) 1
						}
					}
				}
			} else {
				$t tag remove isearch 1.0 end
				winstderr $wmsg "\"$keys\" not found"
				# turn off searching once can't match what you already have
				set sbx(try$t) 0
			}
		}
	}

	return 1
}



#--------------------------------------------------
#
# searchboxSaveConfig -- dump persistent variables into passed file id
#
#--------------------------------------------------

proc searchboxSaveConfig {fid} {
	global sb sbx sbdef

	puts $fid "#\n# SearchBox\n#\n"
	foreach i [lsort [array names sb]] {
		set co ""
		if {[info exists sbdef($i)] && $sbdef($i)==$sb($i)} {set co "#"}
		puts $fid "${co}set sb($i) [list $sb($i)]"
	}
	puts $fid "\n"
}



# instantiate standard mechanisms if don't already exist
if {[info procs winstdout]==""} {

proc winstderr {w msg {type "bell & flash"}} {
	if {![winfo exists $w]} return
	set bell [string match "*bell*" $type]
	set flash [string match "*flash*" $type]

	set fg [$w cget -foreground]; set bg [$w cget -background]

	set msgout [string range $msg 0 500]
	if {[string length $msg]>500} {
		append msgout " ... (truncated; full message sent to stdout)"
		puts stderr $msg
	}
	winstdout $w $msgout
	if {$flash} {$w configure -foreground $bg -background $fg}
	if {$bell} bell
	if {$flash} {
		update idletasks; after 500
		$w configure -foreground $fg -background $bg
	}
}

proc winstdout {w {msg AnUnLiKeMeSsAgE} {update 0}} {
	if {![winfo exists $w]} return
	if {$msg!="AnUnLiKeMeSsAgE"} {
		$w configure -text $msg
		if {$update} { update idletasks }
	}
	return [$w cget -text]
}

}
