#
# some functions which should be part of Tcl, but aren't
#
# Tom Phelps (phelps@cs.Berkeley.EDU)
#


#
# UNIXish
#


# pipeexp - expand file names in a pipe
proc pipeexp {p} {
	set p [string trim $p]

	set expp ""
	foreach i $p {
		if {[regexp {^[.~/$]} $i]} {lappend expp [fileexp $i]} \
		else {lappend expp $i}
	}
	return $expp
}

proc assert {bool msg {boom 0}} {
	if {!$bool} {
		puts stderr $msg
		if $boom {exit 1}
	}
}


# fileexp perform file spec expansion: ~ . .. $
proc fileexp {f} {
	global env

	set f [string trim $f]
	set l [string length $f]
	set expf ""

	set dir [pwd]
	foreach i [split $f "/"] {
		switch -glob $i {
			"" {set dir ""}
			~  {set dir $env(HOME)}
	 $* {set val $env([string trim [string range $i 1 end] ()])
				 if {[string match /* $val]} {set dir $val} {append expf /$val)}}
			.  {set dir $dir}
	 .. {set dir [file dirname $dir]}
	 default {append expf /$i}
		}
	}

	return $dir$expf
}


# in:  f = (partial) file name
# out:   "" (NULL) if no matches
#        full name if exactly one match
#        list      w/first==longest match, if multiple matches

# on /usr/sww/, Tcl's file tail returns sww -- want ""
proc filetail {f} {
	set tail [file tail $f]
	if [string match */ $f] {set tail ""}
	return $tail
}
proc filedirname {f} {
	set dirname [file dirname $f]
	if {[string match ?*/ $f]} {set dirname [string range $f 0 [expr [string length $f]-1]]}
	return $dirname
}

proc filecomplete {f} {
	set expf [fileexp [filedirname $f]]/[filetail $f]
	set posn [string last [filetail $f] $f]; if [string match */ $f] { set posn [string length $f] }
	#if [string match */ $f] {set expf $f; set tail ""; set posn [string length $f]}
#puts "$posn, expf=$expf"
	set l [glob -nocomplain $expf*]
	set ll [llength $l]

	if {!$ll} {
		# maybe indicate that partial name not good
		set tail ""
	} elseif {$ll==1} {
		set tail [file tail $l]
		if {[file isdirectory $l]} {append tail /}
	} else {
		# determine the longest common prefix
		set lf [lfirst $l]; set lfl [string length $lf]
		set last $lfl
		set ni [expr [string last / $lf]+1]
		foreach i $l {
			set il [string length $i]
			for {set j $ni} {$j<=$last} {incr j} {
				if {[string range $lf $j $j]!=[string range $i $j $j]} break
			}
			set last [min $last [expr $j-1]]
		}
		set tail [filetail [string range [lfirst $l] 0 $last]]
	}

#puts "$ll, $tail"
	# compose original directory specification with (possibly) new tail
	if {$posn>0 && $ll} {
		# can't use dirname because it expands ~'s
		set tail [string range $f 0 [expr $posn-1]]$tail
	}

	if {$ll<2} {return $tail} {return "$tail $l"}
}


#return [expr [string first $c "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"]!=-1]

proc isalpha {c} {return [regexp -nocase {[a-z]} $c]}
proc isnum {c} {return [expr [string first $c "123456790"]!=-1]}
proc isalphanum {c} {return [expr [isalpha $c]||[isnum $c]]}

proc stringicap {s} {return [string toupper [string range $s 0 0]][string range $s 1 end]}

proc tr {s c1 c2} {
#	regsub -all \\$c1 $s $c2 s2
	regsub -all "\[$c1\]" $s $c2 s2
	return $s2
}

# reverse glob
#    pass expanded filename, list of shortenings
proc bolg {f {l ""}} {
	if {$l==""} {global file; set l $file(globList)}

	foreach i $l {
		if [regsub ([glob -nocomplain $i])(.*) $f "$i\\2" short] {return $short}
	}
	return $f
}


proc setinsert {l i e} {
	return [linsert [lfilter $e $l] $i $e]
}
# short enough to just inline: if [lsearch $l $e]==-1 {lappend $l $e}
#proc setinsert {l e} {
#   if {[lsearch $l $e]==-1} {
#      return [lappend $l $e]
#   } else {
#      return $l
#   }
#}


proc unsplit {l c} {
	foreach i $l {
		append l2 $i $c
	}
#   return [string trimright $l2 $c]
	return [string range $l2 0 [expr [string length $l2]-2]]
}

proc bytes2prefix {x} {
	set pfx {bytes KB MB GB TB QB}
	set bp 20
	set k 1024.0
	set sz $k

	set y BIG
	for {set i 0} {$i<[llength $pfx]} {incr i} {
		if {$x<$sz} {
			set y [format " %0.0f [lindex $pfx $i]" [expr $x/($sz/$k)]]
			break
		} elseif {$x<[expr $sz*$bp]} {
			set y [format " %0.1f [lindex $pfx [expr $i+1]]" [expr ($x+0.0)/$sz]]
			break
		}

		set sz [expr $sz*$k]
	}

	return $y
}



#
# Lispish
#

# unfortunately, no way to have more-convenient single quote form
proc quote {x} {return $x}

# should sort beforehand
proc uniqlist {l} {
	set e ""
	set l2 ""
	foreach i $l {
		if {$e!=$i} {
			lappend l2 $i
			set e $i
		}
	}
	return $l2
}


proc min {args} {
	if {[llength $args]==1} {set args [lindex $args 0]}
	set x [lindex $args 0]
	foreach i $args {
		if {$i<$x} {set x $i}
	}
	return $x
}

proc avg {args} {
	set sum 0.0

	if {$args==""} return
	
	foreach i $args {set sum [expr $sum+$i]}
	return [expr ($sum+0.0)/[llength $args]]
}

proc max {args} {
	if {[llength $args]==1} {set args [lindex $args 0]}
	set x [lindex $args 0]
	foreach i $args {
		if {$i>$x} {set x $i}
	}
	return $x
}


proc lfirst {l} {return [lindex $l 0]}
proc lsecond {l} {return [lindex $l 1]}
proc lthird {l} {return [lindex $l 2]}
proc lfourth {l} {return [lindex $l 3]}
# five is enough to get all pieces of `configure' records
proc lfifth {l} {return [lindex $l 4]}
proc lsixth {l} {return [lindex $l 5]}
proc lseventh {l} {return [lindex $l 6]}
proc lrest {l} {return [lrange $l 1 end]}

proc llast {l} {
	set end [llength $l]
	if {!$end} {return ""}
	return [lindex $l [expr $end-1]]
}

proc setappend {l e} {
	return "[lfilter $e $l] $e"
}

# filter out elements matching pattern p from list l
proc lfilter {p l} {
	set l2 ""

	foreach i $l {
		if ![string match $p $i] "lappend l2 [list $i]"
	}
	return $l2
}

# keep elements matching pattern p in list l
proc lmatches {p l} {
	set l2 ""
	foreach i $l { if [string match $p $i] {lappend l2 [list $i]} }
	return $l2
}

proc lassoc {l k} {
	foreach i $l {
		if {[lindex $i 0]==$k} {return [lrange $i 1 end]}
	}
	return ""
}

proc lset {l args} {
	foreach val $l var $args {
		upvar $var x
		set x $val
	}
}

# like lassoc, but search on second element, returns first
proc lbssoc {l k} {

	foreach i $l {
		if {[lindex $i 1]==$k} {return [lindex $i 0]}
	}
	return ""
}

proc lreverse {l} {
	set l2 ""
	for {set i [expr [llength $l]-1]} {$i>=0} {incr i -1} {
		lappend l2 [lindex $l $i]
	}
	return $l2
}


#
# X-ish
#

proc geom2posn {g} {
	if [regexp {(=?[0-9]+x[0-9]+)([-+]+[0-9]+[-+]+[0-9]+)} $g both d p] {
		return $p
	} else { return $g }
}

proc geom2size {g} {
	if [regexp {(=?[0-9]+x[0-9]+)([-+]+[0-9]+[-+]+[0-9]+)} $g both d p] {
		return $d
	} else { return $g }
}



#
# Tcl-ish
#


# translate ascii names into single character versions
# this should be a bind option

set name2charList {
	minus plus percent ampersand asciitilde at less greater equal
	numbersign dollar asciicircum asterisk quoteleft quoteright
	parenleft parenright bracketleft bracketright braceleft braceright
	semicolon colon question slash bar period underscore backslash
	exclam comma
}

proc name2char {c} {
	global name2charList

	if {[set x [lsearch $name2charList $c]]!=-1} {
		 return [string index "-+%&~@<>=#$^*`'()\[\]{};:?/|._\\!," $x]
	} else {return $c}
}

# 0=none="", 1=Shift=S, 2=Alt?, 4=Ctrl=C, 8=meta=M, 16=Alt?
proc key_state2mnemon {n} {
	set mod ""

	if {$n>=16} {append mod A; set n [expr $n-16]}
	if {$n>=8} {append mod M; set n [expr $n-8]}
	if {$n>=4} {append mod C; set n [expr $n-4]}
	if {$n>=2} {append mod A; set n [expr $n-2]}
	if {$n} {append mod S}

	return $mod
}

proc lmatch {mode list {pattern ""}} {
	if {$pattern==""} {set pattern $list; set list $mode; set mode "-glob"}
	return [expr [lsearch $mode $list $pattern]!=-1]
}


# remove all char c from string s

proc stringremove {s {c " "}} {
	regsub -all -- \\$c $s "" s2
	return $s2
}


# backquote all regular expression meta-characters
proc stringregexpesc {s} { 
	return [stringesc $s {\||\*|\+|\?|\.|\^|\$|\\|\[|\]|\(|\)|\-}]
}
# backquote Tcl meta-characters
proc stringesc {s {c {\\|\$|\[|\{|\}|\]|\"}}} {
	regsub -all -- $c $s {\\&} s2
	return $s2
}


proc tk_listboxNoSelect args {
	 foreach w $args {
		  bind $w <Button-1> {format x}
	bind $w <B1-Motion> {format x}
	bind $w <Shift-1> {format x}
	bind $w <Shift-B1-Motion> {format x}
	 }
}

# could do with "listbox select&highlight pattern"

proc listboxshowS {lb s {first 0} {cnstr yes}} {
	set sz [$lb size]

	for {set i $first} {$i<$sz} {incr i} {
		if [string match $s [$lb get $i]] {
			listboxshowI $lb $i $cnstr
			return $i
		}
	}
	return -1
}

proc listboxshowI {lb high {cnstr yes}} {
#   if {$high>=[$lb size] || $high<0} return
	set high [max 0 [min $high [expr [$lb size]-1]]]

	set hb [lindex [split [$lb cget -geometry] x] 1]
	set hx [max 0 [expr [$lb size]-$hb]]
	if {$cnstr=="yes"} {set hl [expr $high<$hb?0:[min $high $hx]]} {set hl $high}
	$lb select from $high
	$lb yview $hl
}

proc listboxreplace {lb index new} {
	$lb delete $index
	$lb insert $index $new
	# don't lose selection
	$lb select from $index
}


# preserves selection, yview

proc listboxmove {l1 l2} {
	listboxcopy $l1 $l2
	$l1 delete 0 end
}

proc listboxcopy {l1 l2} {

	$l2 delete 0 end
	listboxappend $l1 $l2
	catch {$l2 select from [$l1 curselection]}
# use NEW yview to keep same yview position
#   catch {$l2 yview [$l1 yview]}
}

proc listboxappend {l1 l2} {

	set size [$l1 size]

	for {set i 0} {$i<$size} {incr i} {
		$l2 insert end [$l1 get $i]
	}
}

###

#option add *Entry.relief sunken
#option add *Text.relief sunken
option add *Text.borderwidth 2
#option add *Menubutton.relief raised
#option add *Radiobutton.relief ridge
#option add *Radiobutton.borderwidth 3
#option add *Checkbutton.relief ridge
#option add *Button.relief ridge
#option add *Button.borderwidth 3

###


proc tabgroup {args} {
	if [llength $args]==1 {set wins [lindex $args 0]} {set wins $args}

	set l [llength $wins]
	for {set i 0} {$i<$l} {incr i} {
		set w [lindex $wins $i]
		set pw [lindex $wins [expr ($i-1)%$l]]
		set nw [lindex $wins [expr ($i+1)%$l]]

		bind $w <KeyPress-Tab> "focus $nw; break"
		bind $w <Shift-KeyPress-Tab> "focus $pw; break"
	}
}


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
	if {$update=="update"} {set update 1}

	if {$msg!="AnUnLiKeMeSsAgE"} {
		$w configure -text $msg
		if {$update} { update idletasks }
	}
	return [$w cget -text]
}

proc yviewcontext {w l c} {
	if {$l=="sel"} {
		set cnt [scan [$w tag ranges sel] %d l]
		if {$cnt<=0} return
	}

	incr l -1; # 0-based!

	scan [$w index end] %d n
	set prev [expr $l-$c]; set next [expr $l+$c]

	if {$prev>=0} {$w yview -pickplace $prev}
	if {$next<=$n} {$w yview -pickplace $next}
	$w yview -pickplace $l
}


proc screencenter {xy wh} {
	if {$xy=="x"} {
		return [expr ([winfo screenwidth .]-$wh)/2]
	} {
		return [expr ([winfo screenheight .]-$wh)/2]
	}
}

# this doesn't work
#button .a -cursor watch
#proc cursorBusy {} {
#   focus .a; grab .a
#}
#
#proc cursorUnBusy {} {
#   global win
#   grab release .a; focus \$win.list
#}

proc cursorBusy {{up 1}} {
	if {[. cget -cursor]!="watch"} {
		cursorSet watch; if $up {update idletasks}
	}
}
proc cursorSet {c {w .}} {
	global cursor
#if {$w=="."} {puts "cursorSet"}
	set cursor($w) [$w cget -cursor]
#puts "cursor($w) = $cursor($w)"
	$w configure -cursor $c
	foreach child [winfo children $w] {cursorSet $c $child}
}
proc cursorUnset {{w .}} {
	global cursor
#if {$w=="."} {puts "cursorUnset"}
	catch {$w configure -cursor $cursor($w)}
	foreach child [winfo children $w] {cursorUnset $child}
}

proc mon2Month {m} {
	set mons {jan feb mar apr may jun jul aug sep oct nov dec}
	set Months {January February March April May June July August September October November December}
	set ml [string tolower $m]
	if {[set x [lsearch -exact $mons $ml]]!=-1} {
		set m [lindex $Months $x]
	}

	return $m
}

proc configurestate {wins {flag "menu"}} {
	set flag0 $flag
	foreach w $wins {
		set flag $flag0
		if {$flag=="menu"} {
			if {[winfo class $w]=="Menubutton"} {
				set m [$w cget -menu]; set end [$m index end]
				set flag [expr {$end!="none" && (![$m cget -tearoff] || $end>0)} ]
			} else { set flag 0 }
		}
		$w configure -state [expr $flag?"normal":"disabled"]
	}
}

proc linebreak {string {breakat 70}} {
	set ch 0; set lastw ""
	set broke ""

	foreach word $string {
		# double space after periods
		if {[string match "*." $lastw]} {append broke " "}

		set wlen [string length $word]
		if {$ch+$wlen<$breakat} {
			if {$ch>0} {append broke " "; incr ch}
			append broke $word; incr ch $wlen
		} else {
			append broke "\n" $word
			set ch $wlen
		}

		set lastw $word
	}

	return $broke
}


# return dates like ls: the more recent the more information, in roughly same number of characters
proc recently {then} {
	set datainfo "%Y %B %d %H %M %S"
	set format "%d %s %d %d %s %s"
	set now [clock seconds]
	scan [clock format $now -format $datainfo] $format year month day hour min sec
	set midnight [clock scan "$day $month"]
	scan [clock format $then -format $datainfo] $format oyear omonth oday ohour omin osec

	set secday [expr 24*60*60]
	set secmonth [expr 30*$secday]
	set secyear [expr 365*$secday]

	set age [expr $now-$then]
	if {$age>=$secyear} {
		set r "$oday $omonth $oyear"
	} else {
		if {$age>=$secmonth} {
			set r "$ohour:$omin, $oday $omonth"
		} else {
			set r "$ohour:$omin"
			if {[expr $midnight-$secday]>=$then} {
				append r ", $oday $omonth"
			} else {
				append r ":$osec"
				if {$day!=$oday} {
					append r " yesterday"
				} else { append r " today" }
			}
		}
	}

	return $r
}
