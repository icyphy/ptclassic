# $Header$

#
# Bird?  Plane?  TkMan!  (TkPerson?)
#
#   by Tom Phelps (phelps@CS.Berkeley.EDU)
#      March 24-25, 1993
#
#   Copyright (c) 1993-1996  Thomas A. Phelps
#

# ability to add directory to MANPATH dynamically (external script that kills current TkMan, and restarts new picking up new value)
# add more anchors to help pages and add context help?
#	maybe link to clicking on label in Preferences...
# put short UNIX names in preferences(?) => belongs in Makefile, where it is now
# in addition to history list, back and forward buttons (Robert Withrow)
# if run by tclsh, null out Tk commands, assume -rebuildandquit?
#    nah, quick enough to build database that don't need to relegate this to an nighttime cron job
# add assertions.
# zap assertions, debugging with optimized Makefile target
# if empty search string set to man page name (Ian Darwin)

# split screen with slider for names and page contents?

# option for TkMan autoraise or leave that to window manager?

# should rewrite Preferences so that entries are specified descriptively
# and a general interpretation proc builds the gui -- like exmh does
# embed Preferences widgets into help page?



###
### proc's (only--no variables, proc calls)
###

wm withdraw .; update idletasks
option add *Menubutton.relief raised
option add *padX 2
option add *padY 2
option add *Button.padX 2
option add *Button.padY 2
option add *Menubutton.padX 2
option add *Menubutton.padY 2
option add *Radiobutton.padX 2
option add *Radiobutton.padY 2


#--------------------------------------------------
#
# TkMan -- make the gui
#
#--------------------------------------------------

proc TkMan {} {
	global man manx mani env

	# determine instance name
	set dup [expr $manx(uid)>1]
	if {!$dup} {
		# .man is guaranteed to exist
		set w .man
	} else {
		set w .man$manx(uid)
		toplevel $w -class TkMan
	}
	set t $w.show; set wi $w.info

	bind $w <Unmap> "if \$man(updateicon) { wm iconname $w \$manx(name$w) }"


	# initialize per-instance variables
	set manx(man$w) ""
	set manx(manfull$w) ""
	set manx(catfull$w) ""
	set manx(name$w) ""
	set manx(num$w) ""
	set manx(cursect$w) 1
	set manx(lastvol$w) 1
	set manx(hv$w) [set manx(oldmode$w) [set manx(mode$w) help]]


	# make the gui
	wm minsize $w 200 200
	# temporarily, to get around twm bug (yuck!)
	wm iconname $w "TkMan"

	if {!$dup} {
		set manx(title$w) $manx(title)
		wm geometry $w $manx(geom)
		# aborts without saving .tkman
		wm protocol $w WM_DELETE_WINDOW {exit 0}

		if {$manx(iconify)} {
			wm iconify $w
			update idletasks
		}
	} else {
		set manx(title$w) "$manx(title) #$manx(uid)"
		wm geometry $w [lfirst [split $manx(geom) "+-"]]
		#wm group $w .man -- don't want this even if it did work
	}
	wm title $w $manx(title$w)


	### information bar
	label $wi -anchor w
#	label $w.volnow -anchor e

	### man or section
	frame $w.kind
	button $w.man -text "man" -command "incr stat(man-button); manShowMan \$manx(typein$w) {} $w"
	button $w.apropos -text "apropos" -command "manApropos \$manx(typein$w) $w"
	if {$man(glimpse)!=""} {button $w.glimpse -text "glimpse" -command "manGlimpse \$manx(typein$w) $w"}
	entry $w.mantypein -relief sunken -textvariable manx(typein$w) -width 25
	# should make this a Preference
	bind Entry <Key-Delete> [bind Entry <Control-Key-h>]
	bind Entry <Control-KeyPress-w> {%W delete 0 end}
	bind $w.mantypein <KeyPress-Return> "$w.man invoke"
	# ha! meta information
	bind $w.mantypein <Shift-KeyPress-Return> "$w.apropos invoke"
	if {$man(glimpse)!=""} {
		foreach m {"Meta" "Alt"} {
			bind $w.mantypein <$m-KeyPress-Return> "$w.glimpse invoke"
		}
	}
	menubutton [set mb $w.dups] -text "\337" -menu [set m $mb.m]; menu $m
# -font $font(guisymbol)

	pack $w.man -in $w.kind -side left -padx 2 -anchor e
	pack $w.apropos -in $w.kind -side left -padx 3
	if {$man(glimpse)!=""} { pack $w.glimpse -in $w.kind -side left -padx 3	}
	pack $w.mantypein -fill x -expand yes -in $w.kind -side left -ipadx 5 -anchor w

	menubutton [set mb $w.paths] -text "Paths" -menu [set m $mb.m]; menu $m
	if {[llength $manx(paths)]>2} {
		$m add command -label "All Paths On" -command {
			foreach i $manx(paths) {set man($i) 1}
			manResetEnv
		}
		$m add command -label "All Paths Off" -command {
			foreach i $manx(paths) {set man($i) 0}
			manResetEnv
		}
		$m add command -label "Save Paths Selections" -command {
			set manx(pathstat) ""
			foreach i $manx(paths) {lappend manx(pathstat) $man($i)}
		}
		$m add command -label "Restore Paths Selections" -command {
			foreach i $manx(paths) j $manx(pathstat) {set man($i) $j}
			manResetEnv
		}
		$m add separator
	}
	foreach i $manx(paths) {
		$m add checkbutton -label $i -variable man($i) -command {manResetEnv}
	}
	manMenuFit $m


	menubutton [set mb $w.vols] -text "Volumes" -menu [set m $mb.m]; menu $m -tearoff no

	pack [frame $w.filler] -in $w.kind -side left -fill x -expand yes
	pack $w.vols $w.paths -in $w.kind -side left -padx 4


	### navigation
	frame $w.nav
	menubutton [set mb $w.sections] -text "Sections" -menu [set m $mb.m]; menu $m -tearoff no
	menubutton [set mb $w.links] -text "Links" -menu [set m $mb.m]; menu $m
# -tearoff no

	frame $w.hlf
	menubutton [set mb $w.high] -text "Highlights" -menu [set m $mb.m]; menu $m -tearoff no
	bind $mb <Shift-Button-1> "manHighlights $w get 1; break"
	button $w.hadd -text "+" -padx 4 -command "
		if \[llength \[$t tag nextrange sel 1.0\]\]==0 {
			manWinstderr $w {Select a range of characters to highlight.}
			return
		}
		incr stat(page-highlight-add)
		set mani(high,form) {}
		$t tag add highlight sel.first sel.last
		selection clear $t
		manHighlights $w
	"
	button $w.hsub -text "-" -padx 4 -command "
		if \[llength \[$t tag nextrange sel 1.0\]\]==0 {
			manWinstderr $w {Select a range of highlighted characters to unhighlight.}
			return
		}
		$t tag remove highlight sel.first sel.last
		incr stat(page-highlight-sub)
		set mani(high,form) {}
		selection clear $t
		manHighlights $w
	"
	bind $w.hsub <Shift-Button-1> "$t tag remove highlight 1.0 end; manHighlights $w"
	pack $w.high $w.hadd $w.hsub -in $w.hlf -side left -padx 1


	frame $w.scf
	menubutton [set mb $w.shortcuts] -text "Shortcuts" -menu [set m $mb.m]; menu $m -tearoff no
	button $w.sadd -text "+" -padx 4 -command "incr stat(page-shortcut-add); manShortcuts $w add"
	button $w.ssub -text "-" -padx 4 -command "incr stat(page-shortcut-sub); manShortcuts $w sub"
	bind $w.ssub <Shift-Button-1> "set man(shortcuts) {}; manShortcuts $w init"
	pack $w.shortcuts $w.sadd $w.ssub -in $w.scf -side left -padx 1
	manShortcuts $w init


	menubutton [set mb $w.history] -text "History" -menu [set m $mb.m] -state disabled; menu $m -tearoff no
	set manx(history$w) ""
#	$m add command -label "(none)"

	menubutton [set mb $w.output] -text "Output" -menu [set m $mb.m]; menu $m -tearoff no
	set manx(out$w) $w


	### commands not used as frequently
	menubutton [set mb $w.occ] -text "Occasionals" -menu [set m $mb.m]; menu $m
	$m add command -label "Help" -command "manHelp $w"
	$m add command -label "Statistics and Information" -command "manInfo $w"
	$m add command -label "Instantiate New View" -command manInstantiate
	$m add command -label "Rebuild Database" -command "manReadSects $w 1 {Rebuilding database ...}" \
		-state [expr [file writable [file dirname $manx(database)]]?"normal":"disabled"]

# used to have entry to rebuild only if existed at least one writable directory
	if {$man(glimpseindex)!=""} {
		$m add command -label "Rebuild Glimpse Index" -command "manGlimpseIndex $w"
		if {$man(time-lastglimpse)!=-1} {$m entryconfigure last -label "Rebuild Glimpse Index (last $man(time-lastglimpse))"}
	}
	$m add cascade -label "Kill Trees" -menu [set m2 $m.kill]; menu $m2 -tearoff no
	$m add cascade -label "Incremental Search Case Sensitive" -menu [set m2 $m.m2]
		menu $m2 -tearoff no
		$m2 add radiobutton -label "yes" -variable man(incr,case) -value 1
		$m2 add radiobutton -label "no" -variable man(incr,case) -value 0
		$m2 add radiobutton -label "iff upper" -variable man(incr,case) -value -1
	$m add cascade -label "Regexp Search Case Sensitive" -menu [set m2 $m.m3]
		menu $m2 -tearoff no
		$m2 add radiobutton -label "yes" -variable man(regexp,case) -value 1
		$m2 add radiobutton -label "no" -variable man(regexp,case) -value 0
		$m2 add radiobutton -label "iff upper" -variable man(regexp,case) -value -1
	if {$man(glimpse)!=""} { $m entryconfigure end -label "Regexp and Glimpse Case Sensitive" }	
	$m add checkbutton -label "Changebars on Left" -variable man(changeleft) -onvalue "-c" -offvalue ""
	$m add command -label "Preferences..." -command manPreferences
	$m add command -label "Checkpoint state to $manx(startup-short)" \
		 -command "manSave; manWinstdout $w {[bolg $manx(startup) ~] updated}"
	$m add separator
	$m add command -label "Quit, don't update $manx(startup-short)" -command "exit 0"


	foreach i {sections hlf links history scf occ} {pack $w.$i -in $w.nav -side left}
	foreach i {hlf history occ} {pack configure $w.$i -padx 6}
	pack configure $w.links -expand yes -anchor e
	pack configure $w.scf -expand yes -anchor w


	### view box
	frame $w.vf
	text $t \
#-font $man(currentfont) \
		-relief sunken -borderwidth 2 \
# -padx $man(textboxmargin) -pady $man(textboxmargin) \
		-yscrollcommand "$w.v set" -exportselection yes -wrap word -cursor $manx(cursor) \
		-height 10 -width 5 -insertwidth 0
	$t tag configure info -lmargin2 0.5i
	bind $w <Configure> "
		set tmp \[expr \[winfo width $t\]-2*\[$t cget -padx\]-10\]
		$t tag configure info -tabs \[list 0.3i \$tmp right\]
		$t tag configure high -tabs \[list \$tmp right\]
	"
	foreach b {Double-Button-1 Shift-Button-1} { bind Text <$b> {} }
	$t tag configure apropos -wrap word

	# bind letters to jump to that part of list
	bind $t <Any-KeyPress> "if \[manKeyNav $w \[key_state2mnemon %s\] %K\] break"
	bind $t <Control-KeyPress-d> "manShowSection $w \$manx(lastvol$w)"
	bind $t <Control-KeyPress-m> "manShowMan \$manx(lastman) {} $w"

	bind $t <Button1-Motion> "manHotSpot clear $t 1.0"
	bind $t <Button-1> "manHotSpot show %W @%x,%y"
	set manx(hotman$t) ""

	scrollbar $w.v -orient vertical -command "$t yview"
	pack $w.v -in $w.vf -side $man(scrollbarside) -fill y
	pack $t -in $w.vf -side $man(scrollbarside) -fill both -expand yes

	# shift on various menus, buttons instantiates new viewer too
	foreach b [list $w.man $w.apropos] {
		bind $b <Shift-ButtonRelease-1> {set manx(shift) 1 }
	}
	if {$man(glimpse)!=""} {bind $w.glimpse <Shift-ButtonRelease-1> {set manx(shift) 1} }
	foreach mb [list $w.vols $w.links $w.history $w.shortcuts] {
		bind $mb.m <Shift-ButtonRelease-1> {set manx(shift) 1 }
	}



	### search (uses searchbox--wow, code reuse!)
	frame $w.search
	button $w.search.s -text "Search" -command "
		incr stat(page-regexp-next)
		set manx(search,cnt$w) \[searchboxSearch \$manx(search,string$w) 1 \$man(regexp,case) search $t $wi $w.search.cnt\]
	"
	button $w.search.next -text "\xdf" -command "incr stat(page-regexp-next); searchboxNext search $t $wi"
	button $w.search.prev -text "\xdd" -command "incr stat(page-regexp-prev); searchboxPrev search $t $wi"
	label $w.search.cnt
	entry $w.search.t -relief sunken -textvariable manx(search,string$w)
	set manx(search,cnt$w) 0
	set manx(search,oldstring$w) ""
	bind $w.search.t <KeyPress-Return> "
		if {\$manx(search,oldstring$w)!=\$manx(search,string$w) || !\$manx(search,cnt$w)} {
			set manx(search,oldstring$w) \$manx(search,string$w)
			$w.search.s invoke
		} else {$w.search.next invoke}"
	bind $w.search.t <Control-KeyPress-s> "incr stat(page-incr-next); $w.search.next invoke"
	bind $w.search.t <Control-KeyPress-r> "incr stat(page-incr-prev); $w.search.prev invoke"
	bind $w.search.t <Control-KeyPress-n> "manKeyNav $w C n"
	bind $w.search.t <Control-KeyPress-p> "manKeyNav $w C p"
	pack $w.search.s -side left
	pack $w.search.next $w.search.prev -side left -padx 4
	pack $w.search.t -side left -fill x -expand yes -ipadx 10 -anchor w
	pack $w.search.cnt -side left

#	bind $t <KeyPress-slash> "focus $w.search.t"

	### font
	# mostly mono (Mozart?)
	checkbutton $w.mono -text "Mono" -variable man(currentfont) \
# -onvalue and -offvalue set elsewhere
#		-onvalue $font(mono) -offvalue $font(pro) \
#		-font $font(guimono) \
		-command "
			incr stat(page-mono)
			$t configure -font \$man(currentfont)
			manManTabSet $w
#			$t tag configure italics -underline \
#				\[expr 1-\[lfifth \[$t tag configure italics -underline\]\]\]
		"

	### quit
	button $w.quit -text "Quit" -command "manSave; exit 0" -padx 4
	if !$manx(quit) {$w.quit configure -command "exit 0"}
	if $dup {$w.quit configure -text "Close" -command "destroy $w; incr manx(outcnt) -1; manOutput"}
	bind all <Meta-KeyPress-q> "$w.quit invoke"

	pack $w.mono -in $w.search -side left -padx 3 -anchor e
	pack $w.quit -in $w.search -side left -padx 3


	pack $wi $w.kind -fill x -pady 4
	pack $w.nav -fill x -pady 6
	pack $w.vf -fill both -expand yes
	pack $w.search -fill x -pady 6


	# generous hit regions, tab between
	foreach i {info kind nav} {bind $w.$i <Enter> "focus $w.mantypein"}
	foreach i {vf show v} {bind $w.$i <Enter> "focus $t"}
	bind $w.search <Enter> "focus $w.search.t"
	tabgroup $w.mantypein $t $w.search.t
	foreach i {mantypein show search.t} {
		foreach k {KeyPress-Escape Control-KeyPress-g} {
			bind $w.$i <$k> {+ set STOP 1 }
		}
	}
	# fixups
	foreach k {KeyPress-Escape Control-KeyPress-g} {
		bind $t <$k> "+ if \[manKeyNav $w \[key_state2mnemon %s\] %K\] break"
	}
	bind $w.mantypein <KeyPress-Escape> "+
		if \[regexp {^\[<|.~/$\]} \$manx(typein$w)\] {manFilecomplete $w}
	"

	manMakeVolList $w
	manPreferencesSetMain $w

	manHelp $w
#	update idletasks

	return $w
}

# update winstdout/winstderr use by other modules
rename winstdout winstdout-default
rename winstderr winstderr-default
proc winstdout {w {msg AnUnLiKeMeSsAgE} {update 0}} { return [manWinstdout [winfo parent $w] $msg $update] }
proc winstderr {w msg {type "bell & flash"}} { return [manWinstderr [winfo parent $w] $msg] }

proc manWinstderr {w msg} {
	global man
	return [winstderr-default $w.info $msg $man(error-effect)]
}

proc manWinstdout {w {msg AnUnLiKeMeSsAgE} {update 0}} {
	return [winstdout-default $w.info $msg $update]
}

proc manManTabSet {w} {
	set t $w.show

	set state [$t cget -state]; $t configure -state normal
	$t insert @0,0 "     \n"
	$t configure -tabs [lindex [$t dlineinfo @0,0] 2]
	$t delete @0,0 @0,0+6c
	$t configure -state $state
}

proc manMakeVolList {w} {
	global man manx

	set m $w.vols.m
	if ![winfo exists $m] return
	destroy $m
	#delete 0 last
	menu $m -tearoff no

	$m add command -label "Last volume viewed" -accelerator "C-d" -state disabled \
		-command "manShowSection $w \$manx(lastvol$w)"
	$m add separator

	set iapropos [lsearch -exact $manx(manList) "apropos"]

	set ctr 0
	foreach i $manx(manList) j $manx(manTitleList) {

		set menu $m; set label "($i) $j"

		if $man(subvols) {
			set l1 [string range $i 0 0]
			set p [lsearch -exact $manx(manList) $l1]
			set c [lsearch -glob $manx(manList) "$l1?*"]; if {$c>=$iapropos} {set c -1}

			if {$p!=-1 && $c!=-1} {
				# part of a group => add entry to cascade menu
				set menu $m.$p
				if {$ctr==$p} {set label "general"}

				# if that cascade menu doesn't exist yet, create it now
				# (so submenus need not precede their root)
				if ![winfo exists $menu] {
					menu $menu -tearoff no
					$m add cascade -label "($i) [lindex $manx(manTitleList) $p]" -menu $menu
				}
			}
		}

		# everybody gets added somewhere, under some name
		$menu add command -label $label -command "manShowSection $w $i"
		incr ctr
	}

	manMenuFit $m
}



#--------------------------------------------------
#
# Shortcuts
#
#--------------------------------------------------

proc manShortcuts {w cmd} {
	global man manx

	set me $manx(name$w)
	if {![string match "<*" $me]} { append me ".$manx(cursect$w)" }
#puts "$manx(man$w) => $manx(name$w).$manx(cursect$w)"

	set modeok [lmatch {man txt} $manx(mode$w)]
	if {$cmd!="init" && (!$modeok || $manx(man$w)=="")} return

	set present [expr [lsearch -exact $man(shortcuts) $me]!=-1]
	if {$cmd=="add" && !$present} {lappend man(shortcuts) $me} \
	elseif {$cmd=="sub" && $present} {set man(shortcuts) [lfilter $me $man(shortcuts)]}

	# for each instantiation, update shortcuts list
	foreach win [lmatches ".man*" [winfo children .]] {
		set m [set mb $win.shortcuts].m
		$m delete 0 last
		set len [llength $man(shortcuts)]
		if {$len} {
			foreach i $man(shortcuts) {
				if {![regexp {^[<|]} $i]} {
					#  usual man page
					if {[lsearch $manx(mandot) $i]>-1} {set name $i} {set name [file rootname $i]}
				} else {set name $i}
				$m add command -label $name -command "incr stat(man-shortcut); manShowMan [list $i] {} $win"
			}
		}

		configurestate $mb
# $len
		manShortcutsStatus $win
		manMenuFit $m
	}
}


proc manShortcutsStatus {w} {
	global man manx

	set me $manx(name$w)
	if {![string match "<*" $me]} { append me ".$manx(cursect$w)" }

	set modeok [lmatch {man txt} $manx(mode$w)]
	set present [expr [lsearch -exact $man(shortcuts) $me]!=-1]
	configurestate $w.sadd [expr $modeok&&!$present]
	configurestate $w.ssub [expr $modeok&&$present]
}


# compensate for Tk's nonalphnum-bounded wordstart and wordend
# should have companions to word{start,end} that are bounded by whitespace, not nonalpha
# (in this case analysis too complex for an extended wordstart)

proc manHotSpot {cmd t xy} {
	global man manx

	set manchars {[ a-z0-9_.~/$+()-:<]}

	# click again in hot spot without time considerations
#	if {!catch{$t index hot.first} && [$t compare hot.first <= $xy] && [$t compare $xy >= hot.last]} {
#		set cmd get
#	}

	# act on command
	if {$cmd=="clear"} {
		$t tag remove hot 1.0 end
		set manx(hotman$t) ""
		return
	}

	# else $cmd=="show"
	$t tag remove hot 1.0 end
	set manx(hotman$t) ""

	scan [$t index $xy] "%d.%d" line char
	scan [$t index "$line.$char lineend"] "%d.%d" bozo lineend

	# special cases when clicking on parentheses
	set c [$t get $line.$char]
	if {$c=="("} {
		if {$char>0 && [$t get $line.$char-1c]!=" "} {incr char -1} {incr char}
	} elseif {$c==")"} {
		if {$char>0} {incr char -1}
	}


	set lparen 0; set rparen 0
	# single space between name and parenthesized volume ok (found mainly in apropos but some page too)
	set fspace 0

	# gobble characters forwards
	for {set cn $char} {$cn<=$lineend && [regexp -nocase $manchars [set c [$t get $line.$cn]]]} {incr cn} {
		if {$c=="("} {
			if {!$lparen} {set lparen $cn} else break
		} elseif {$c==")"} {
			if {!$rparen} {set rparen $cn; incr cn}
			break
		} elseif {$c==":" && ([$t get $line.$cn+1c]==" " || [$t get $line.$cn-1c]==" ")} {
			break
		} elseif {$c==" "} {
			if {!$lparen && !$fspace && $cn<$lineend && [$t get $line.$cn+1c]=="("} {set fspace 1} else break
		}
	}
	incr cn -1

	# gobble characters backwards
	for {set c0 $char} {$c0>=0 && [regexp -nocase $manchars [set c [$t get $line.$c0]]]} {incr c0 -1} {
		if {$c=="("} {
			if {!$lparen} {set lparen $c0} else break
		} elseif {$c==")"} {
			break
		} elseif {$c==":" && ([$t get $line.$c0+1c]==" " || [$t get $line.$c0-1c]==" ")} {
			break
		} elseif {$c==" "} {
			if {$lparen==[expr $c0+1] && !$fspace} {set fspace 1} else break
		}
	}
	incr c0

	# adjust if parentheses unbalanced
	if {!$lparen^!$rparen} {
		if {$lparen} {
			if {$char>$lparen} {set c0 [expr $lparen+1]} {set cn [expr $lparen-1]}
			set lparen 0
		} else {
			# trim off right paren
			incr cn -1
		}

	# check for valid section number
	} elseif {$lparen && [lsearch $man(manList) [$t get $line.$lparen+1c]]==-1} {
		if {$char>$lparen} {set c0 [expr $lparen+1]} {set cn [expr $lparen-1]}
		set lparen 0

	# nothing between parentheses
	} elseif {$lparen==[expr $rparen-1] && $lparen>0} {
		set cn [expr $lparen-1]

	# if have good parens, can't start with a "/"
	} elseif {$lparen && $rparen && [$t get $line.$c0]=="/"} {
		incr c0
	} 

	# clean up sides
	while {$c0>0 && [lsearch {" "} [$t get $line.$c0]]>=0} {incr c0}
	while {$cn>$c0 && [lsearch {. - , " "} [$t get $line.$cn]]>=0} {incr cn -1}


	$t tag add hot $line.$c0 $line.$cn+1c
	set manx(hotman$t) [string trim [$t get $line.$c0 $line.$cn+1c]]
}


proc manInstantiate {} {
	global manx

	incr manx(uid)
	incr manx(outcnt)
	set w [TkMan]
	manOutput

	return $w
}


proc manOutput {} {
	global manx

	set wins [lmatches ".man*" [winfo children .]]

	# make list of output choices
	foreach i [lsort -command manWinSort $wins] {
		set title "#[string range $i 4 end]"
		# special case for main window
		if {$title=="#"} {append title 1}
		lappend titleList [list $title $i]
	}

	# update list on each instantiation
	foreach w $wins {
		set m $w.output.m
		$m delete 0 last
		foreach i $titleList {
			$m add radiobutton -label [lfirst $i] \
				-variable manx(out$w) -value [lsecond $i]
		}
	}
	manMenuFit $m

	# if pointing to a zapped instance, reset to self
	foreach w $wins {
		if ![winfo exists $manx(out$w)] {set manx(out$w) $w}
	}

	# if multiple choices, show menu
	if {$manx(outcnt)==1} {
		pack forget .man.output
	} else {
		foreach w $wins {pack $w.output -before $w.occ -padx 2 -side left -expand yes}
	}
}

proc manWinSort {a b} {
	set an [string range $a 4 end]
	set bn [string range $b 4 end]
	if {$an==""} {return -1} elseif {$bn==""} {return 1}

	if {$an<$bn} {return -1} elseif {$an==$bn} {return 0} else {return 1}
}


#--------------------------------------------------
#
# Highlights
#
#--------------------------------------------------

# get old highlights and yview, if any

proc manHighView {w} {
	global manx

	set t $w.show
	if ![catch {set y $manx(yview,$manx(hv$w))}] {
		$t yview moveto $y
		$t mark set xmark [$t index @0,0]
	}
	manHighlights $w get
}


# highlighting commands: update (menu), get (all from database)

proc manHighlights {w {cmd update} {auto 0}} {
	global high man manx mani stat

	set m [set mb $w.high].m
	$m delete 0 last
	set t $w.show
	set status ""
	set res ""

	# get/save tags
	set f $manx(hv$w)
#	if [string match <* $f] {set f [string range $f 1 end]}
	# canonical file name
	if ![catch {set sf [file readlink $f]}] {
		if [string match /* $sf] {
			set f $sf
		} else {
			set f [file dirname $f]
			set strip 1
			while {$strip} {
				switch -glob $f {
					../* {set f [file dirname $f]; set sf [string range $sf 3 end]}
					./* {set sf [string range $sf 2 end]}
					default {set strip 0}
				}
			}
			append f /$sf
		}
	}
	set var high($manx(manfull$w))
	set tags ""
	if {$cmd=="update"} {
		# new style annotations store date (latest) annotation made
		set tags [clock seconds]
		# save some context with tag in hopes of reattaching if the man page changes 
		# (or just reinstalled)
		set rawtags [$t tag ranges highlight]
		for {set i 0} \$i<[llength $rawtags] {incr i 2} {
			set first [lindex $rawtags $i]
			set last [lindex $rawtags [expr $i+1]]
			eval lappend tags [manHighCompose $t $first $last]
		}

		# clear volume list because updated time of last highlight
		set mani(high,form) {}
		DEBUG {puts "updating highlights to $tags"}
		if {[llength $tags]>1} {set $var $tags} {catch {unset $var}}
		set status "updated"

	} elseif {[info exists $var]} {
		set tags [set $var]

		if {[llength [lsecond $tags]]>1} {
			set losertxt ""; set dicey 0
			set res "new style"

			# new style highlights -- reattach given initial position and context; date irrelevant
			set newtags [lfirst $tags]
			for {set i 1} \$i<[llength $tags] {incr i 2} {
				set ftuple [lindex $tags $i]
				set first [lfirst $ftuple]; set firsttxt [lsecond $ftuple]
				set predelta [lthird $ftuple]; set fhooklen [lfourth $ftuple]

				set ltuple [lindex $tags [expr $i+1]]
				set ldelta [lfirst $ltuple]; set lasttxt [lsecond $ltuple]
				set postdelta [lthird $ltuple]; set lhooklen [lfourth $ltuple]

				set new [manHighlightAttach $t  $first $firsttxt $predelta $fhooklen   $ldelta $lasttxt $postdelta $lhooklen]
				set status [lfirst $new]; set firstnew [lsecond $new]; set ldeltanew [lthird $new]
				if {$status=="LOSE"} {
					incr stat(high-lose)
					append losertxt "[string range $firsttxt 0 [expr $predelta-1]] / [string range $firsttxt $predelta end] ($first)   ...   [string range $lasttxt 0 [expr [string length $lasttxt]-$postdelta]] / [string range $lasttxt [expr [string length $lasttxt]-$postdelta+1] end] (+$ldelta)\n"
					append res ", lost one"
				} elseif {$firstnew!=$first || $ldeltanew!=$ldelta} {
					# had to move one, save repositioned information
					incr stat(high-move)
					eval lappend newtags [manHighCompose $t $firstnew "$firstnew+${ldeltanew}c"]
					# dicey move
					if {$status=="DICEY"} { set dicey 1; append res ", dicey move" }
				} else {
					incr stat(high-exact)
					# old info is still good
					lappend newtags $ftuple $ltuple
					append res ", exact"
				}
			}

			set warningtxt ""

			# report losers at bottom
			if {$losertxt!=""} {
				manTextPlug $t end "\n\n\nCouldn't attach these highlights:\n" {} $losertxt {} "\n"
				append warningtxt "Some highlights could not be repositioned.  See the bottom of the page for a list.  They will be forgotten unless they are reapplied manually now.   "
			}
			if {$dicey} {
				append warningtxt "Some highlights may have been repositioned incorrectly.  You may want to verify them now.   "
			}
			if {$warningtxt!="" && !$auto} {
				tk_dialog .dialog "Warning" $warningtxt "" 0 " OK "
			}

			# update persistent info
			set $var $newtags
			set tags $newtags

		} else {
			# old style highlights -- up to date or die
			set res "old style"
			if {![file isfile $f] || [file mtime $f]<=[lfirst [set $var]]} {
				# man page hasn't been changed (according to mtime) -- assume everything OK
				append res ", OK"
			} else {
				# old style highlights lose
				if {!$auto && ![tk_dialog .dialog "Warning" "Highlights out of date for $f.  Delete them?" "" 1 "No" "Yes"]} {
					set $var [set tags "[file mtime $f] [lrange [set $var] 1 end]"]
				}
				append res ", out of date"
			}

			# lazily convert from old style to new
			# use after because have to draw highlights below before making new style ones
			after 1 manHighlights $w update
		}
	}
#puts stdout "v = $var, f = $f"


	# always redraw highlights (good check)
	$t tag remove highlight 1.0 end

	# update highlighting in text, menu
	for {set i 1} \$i<[set len [llength $tags]] {incr i 2} {
		set first [lindex $tags $i]; set last [lindex $tags [expr $i+1]]
		if {[llength $first]>1} { set first [lfirst $first]; set last "$first+[lfirst $last]c" }
		$t tag add highlight $first $last
		if {$auto} { $t yview -pickplace $last; update idletasks; after 1000 }
		set label \
			[string range [manHighNormalize [$t get $first $last]] 0 $man(high,hcontext)]
		$m add command -label $label \
			-command "incr stat(page-highlight-go); $t yview \[$t index \"$first - \$man(high,vcontext) lines\"\]"
	}

	configurestate [list $mb $w.hsub] "$len>1"
	manMenuFit $m

	return $res
}


# update highlights: old to new, throw out nonexistant files
proc manHighlightsUpdate {{w .man}} {
	global high mani

	if ![llength [array names high]] return

	set t $w.show

	# do you need to update your highlights?
	# give opportunity iff no new style highlights, so most people only asked once
	set highup 1
	foreach i [array names high] {
#		if {$i=="*"} continue
		if [llength [lsecond $high($i)]]>1 { set highup 0; break }
	}
	if {!$highup || ![tk_dialog .dialog "Update TkMan Highlights?" "Do you want to update your [llength [array names high]] highlighted pages to use new style highlights that are robust to changes in the man page, automatically and in batch now?  Otherwise they can be updated manually one by one at any time." "" 1 " Manual " " Automatic "]} {
		return
	}


	wm deiconify $w
	manShowSection $w high

	rename manHighView manHighViewX
	proc manHighView {w} {}

	set newform "The results below will become permanent unless you exit via \"Occasionals / Quit, don't update $manx(startup-short)\"\n\n\n"

	foreach man [array names high] {
#		if {$i=="*"}  continue

		append newform "$man\t"

		if {[llength [lsecond $high($man)]]>1} { append newform "new style\n"; continue }

		set stat [manShowMan $man]; update idletasks
#puts "|$stat|"
		if {$stat==""} {
			catch {unset high($man)}
			set r "doesn't exist"
# all pages supplied as full paths
#		} elseif {[string match "ERROR*" $man]} {
#			#catch {unset high($man)}
#			set r "not found"
		} else {
			set r [manHighlights $w get 1]
			# flush after call that resets highlights
			update

			$t yview 0
		}

		append newform "$r\n"
	}

	rename manHighView {}
	rename manHighViewX manHighView

	set mani(high,form) $newform
	manShowSection $w high
}


# compose highlight data record from region of text
# want excerpt of text 
# and hook of reasonable length to search for (so may need to augment excerpt on that line)
# record format :== first-tag last-tag
# first-tag :== index excerpt pre-augment-length hook-length
# last-tag :== unnormalize-highlight-length excerpt post-augment-length hook-length
# when/if search across lines, hook-length worthless

proc manHighCompose {t first last} {
	set excerptmax 30; set hookmax 20

	scan [$t index $first] "%d.%d" fline fchar
	scan [$t index $last] "%d.%d" lline lchar
	set rlen [string length [$t get $first $last]]
	set elen [min $excerptmax $rlen]
DEBUG {puts "EXCERPTING first=$first, last=$last, rlen=$rlen, elen=$elen"}

	# compute start 
	scan [$t index [set fsi "$first linestart"]] "%d.%d" junk fstartchar
	scan [$t index [set fei "$first lineend"]] "%d.%d" junk fendchar
	set exhooklen [min $elen [expr $fendchar-$fchar+1]]
DEBUG {puts "first\tfstartchar=$fstartchar, fendchar=$fendchar, exhooklen = $exhooklen"}
	if {$exhooklen>=$hookmax} {
		# excerpted characters form substantial enough hook
		set hooklen [string length [manHighNormalize [$t get $first $first+${exhooklen}c]]]
		set prelen 0
		set excerpttxt [manHighNormalize [$t get $first $first+${elen}c]]
	} else {
		# augment excerpt if possible, at start only (for now?)
#puts "augmented"
		# when/if search across lines, don't be limited by start of line, end of line
		set prei "$first-[expr $hookmax-$exhooklen]c"; if [$t compare $prei < $fsi] { set prei $fsi }
		set excerpttxt [manHighNormalize [$t get $prei $first+${elen}c]]
		set posti "$first+${elen}c"; if [$t compare $posti > $fei] { set posti $fei }
		set hooklen [string length [manHighNormalize [$t get $prei $posti]]]
		set prelen [expr $hooklen-[string length [manHighNormalize [$t get $first $posti]]]]
	}
DEBUG {puts "|$excerpttxt|, $prelen, $hooklen"}
	set firsttag [list $first $excerpttxt $prelen $hooklen]


	# in updating from old to new style, sometimes can get empty line
	if {!$hooklen} { return "" }


	# compute end
	scan [$t index [set esi "$last linestart"]] "%d.%d" junk lstartchar
	scan [$t index [set eei "$last lineend"]] "%d.%d" junk lendchar
	set exhooklen [min $elen [expr $lchar-$lstartchar+1]]
DEBUG {puts "end\tlstartchar=$lstartchar, lendchar=$lendchar, exhooklen = $exhooklen"}
	if {$exhooklen>=$hookmax} {
		# excerpt characters form substantial enough hook
		set hooklen [string length [manHighNormalize [$t get $last-${exhooklen}c $last]]]
		set postlen 0
		set excerpttxt [manHighNormalize [$t get $last-${elen}c $last]]
	} else {
		# augment excerpt if possible
#puts "augmented"
		# when/if search across lines, don't be limited by start of line, end of line
		set posti "$last+[expr $hookmax-$exhooklen]c"; if [$t compare $posti > $eei] { set posti $eei }
		set excerpttxt [manHighNormalize [$t get $last-${elen}c $posti]]
		set prei "$last-${elen}c"; if [$t compare $prei < $esi] { set prei $esi }
		set hooklen [string length [manHighNormalize [$t get $prei $posti]]]
		set postlen [expr $hooklen-[string length [manHighNormalize [$t get $prei $last]]]]
	}
	set lasttag [list $rlen $excerpttxt $postlen $hooklen]
DEBUG {puts "|$excerpttxt|, $hooklen, $postlen"}

	return [list $firsttag $lasttag]
}


# hyphens ignored, any whitespace matches any whitespace (space matches tab, e.g.)
# essential for searching across reformats, but also good for storing in ~/.tkman

proc manHighNormalize {raw {maxlen 0}} {
	set ws "\[\t\n \]"
	set new [string trim $raw]

	# zap changebars
	regsub -all "\n\\|" $new "\n" new
	regsub "^\\|$ws" $new "\n" new

	# linebreaks (hyphens and whitespace) ignored and word spacing ignored
	regsub -all -- "-\n" $new "\n" new
	regsub -all "$ws+" $new " " new

	if {$maxlen} { set new [string range $new 0 $maxlen] }

	return $new
}

proc manHighRegexp {normal} {
	set regexp [stringregexpesc $normal]
	regsub -all "\[\t\n \]+" $regexp "\[\t\n |-\]*" regexp
	return $regexp
}


# try to reattach new style highlights
proc manHighlightAttach {t first firsttxt predelta fhooklen  ldelta lasttxt postdelta lhooklen   {status "GOOD"}} {
	DEBUG {puts "ATTACH: $first $firsttxt $predelta $fhooklen   $ldelta $lasttxt $postdelta $lhooklen  $status"}

	if {!$fhooklen} { set fhooktxt $firsttxt } else {
		set len [string length $firsttxt]
		set fhooktxt [string range $firsttxt 0 [expr $fhooklen-1]]
		set fextxt [string range $firsttxt $predelta end]
		set fpretxt [string range $firsttxt 0 [expr $predelta-1]]
#puts "fhooktxt=|$fhooktxt|, fextxt=|$fextxt|, fpretxt=|$fpretxt|"
	}
	if {!$lhooklen} { set lhooktxt $lasttxt } else {
		set len [string length $lasttxt]
		set lhooktxt [string range $lasttxt [expr $len-$lhooklen] end]
		set lextxt [string range $lasttxt 0 [expr $len-$postdelta-1]]
		set lhookextxt [string range $lhooktxt 0 [expr [string length $lhooktxt]-$postdelta-1]]
		set lposttxt [string range $lasttxt [expr $len-$postdelta] end]
#puts "lhooktxt=|$lhooktxt| / |$lhookextxt|, lextxt=|$lextxt|, lposttxt=|$lposttxt|"
	}

	DEBUG {puts "first = $first, |$fhooktxt|\nlast = +$ldelta, |$lhooktxt|"}


	### attach start of range

	if [$t compare $first >= end] { set first [$t index end-1l] }
	set flen [string length $fhooktxt]
	set fpt ""

	# first check for exact match for first part
	set found 0
	set fregexp [manHighRegexp $fhooktxt]
#puts "searching forward for $fregexp"
	set ffw [$t search -forwards -regexp $fregexp $first end]
	set fbk [$t search -backwards -regexp $fregexp $first 1.0]
	if {$ffw=="" && $fbk==""} {
		# nothing yet
	} elseif {$ffw==""} {
		set fpt $fbk
		set found 1
		DEBUG {puts "only found backward from $first at $fbk"}
	} elseif {$fbk==""} {
		set fpt $ffw
		set found 1
		DEBUG {puts "only found foward from $first at $ffw"}
	} else {
		# matches forward and backward.  pick closer one
		scan $first "%d.%d" line char
		scan $ffw "%d.%d" fline fchar; set difffw [expr $fline-$line]; set dcfw [expr abs($fchar-$char)]
		scan $fbk "%d.%d" bline bchar; set diffbk [expr $line-$bline]; set dcbk [expr abs($char-$bchar)]
		if {$diffbk<$difffw} {set fpt $fbk} elseif {$difffw<$diffbk} {set fpt $ffw} {
			# tie, go to characters
			if {$dcbk<$dcfw} {set fpt $fbk} else {set fpt $ffw}
		}
		set found 1
		DEBUG {puts "found point $first forward ($ffw) and back ($fbk), closer is $fpt"}
	}
	scan $fpt "%d.%d" fline fchar


	# adjustments to search: disqualifications and tweaks
	if {$found} {
		if {$fhooklen} {
			# searching by hooks: verify match on excerpt
			# and get real start of highlight (controlling for spaces)

			# bump over hook context
			# this had better match!
			set must [$t search -forwards -regexp -count delta [manHighRegexp $fpretxt] $fpt end]
			set fpt [$t index "$fpt+${delta}c"]

			set txt [$t get $fpt "$fpt+1000c"]
			if ![regexp -indices -- [manHighRegexp $fextxt] $txt all] { set found 0 }

		} elseif {$flen>=20} {
			# searching by long excerpt
			# if match far away, axe it, in favor of possible closer if shorter match
			if {[expr abs($line-$fline)>200]} { set found 0 }
		}
	}

	if {!$found} {
		# back off strategies:
		# if searching by hooks, dump hooks for excerpt text
		if {$fhooklen} {
			return [manHighlightAttach $t $first $fextxt 0 0  $ldelta $lasttxt $postdelta $lhooklen  $status]

		# if searching by long excerpt text, chop it and try again
		} elseif {$flen>10} {
			set chop [max 9 [expr int($flen/2)]]
			return [manHighlightAttach $t $first [string range $fhooktxt 0 $chop] 0 0  $ldelta $lasttxt $postdelta $lhooklen "DICEY"]
		} else {
			return "LOSE"
		}
	}


	### attach end of range

	# now search forward from first match to find end
	# at approximately the same number of characters forward as old one
	set found 0
	set llen [string length $lhooktxt]
	set last "$fpt+${ldelta}c-${llen}c"
	set lregexp [manHighRegexp $lhooktxt]
#puts "searching backward for $lregexp"
	set lfw [$t search -forwards -regexp -count lfwcnt $lregexp $last end]
	set lbk [$t search -backwards -regexp -count lbkcnt $lregexp $last $fpt]
	if {$lfw=="" && $lbk==""} {
	} elseif {$lfw==""} {
		set lpt $lbk
		set llen $lbkcnt
		set found 1
		DEBUG {puts "end only found backward from $fpt at $lbk"}
	} elseif {$lbk==""} {
		set lpt $lfw
		set llen $lfwcnt
		set found 1
		DEBUG {puts "end only found foward from $fpt at $lfw"}
	} else {
		# match forward and backward.  pick closer one -- need to adjust for length of match
		scan $fpt "%d.%d" line char
		scan $lfw "%d.%d" fline fchar; set difffw [expr $fline-$line]; set dcfw [expr abs($fchar-$char)]
		scan $lbk "%d.%d" bline bchar; set diffbk [expr $line-$bline]; set dcbk [expr abs($char-$bchar)]
		if {$diffbk<$difffw} {set lpt $lbk; set llen $lbkcnt} elseif {$difffw<$diffbk} {set lpt $lfw; set llen $lfwcnt} {
			# tie, go to characters
			if {$dcbk<$dcfw} {set lpt $lbk; set llen $lbkcnt} else {set lpt $lfw; set llen $lfwcnt}
		}
		set found 1
		DEBUG {puts "found end point $fpt forward ($lfw) and back ($lbk), closer is $lpt"}
	}
	

	if {$found} {
		if {$lhooklen} {
			# should make this check when do $t search ...
			# match only as far as excerpt (not hook) -- chop off trailing context
			set e [manHighRegexp $lhookextxt]
			# this had better match!
			set posti [$t search -count llen -regexp $e $lpt end]
			DEBUG {puts "$t search -count llen -regexp $e $lpt end"}
			# off by one error in searching backwards
#			DEBUG {puts "backward must = $must, delta=$delta, llen=$llen"}
#			set llen [string length [$t get $fpt "$lpt+${len}c"]]
#			incr llen -$delta


			set e [manHighRegexp $lextxt]; append e "\$"
			set txt [$t get "$lpt-1000c" "$lpt+${llen}c"]
#puts "backward regexp [regexp $e $txt]: find |$lextxt| in |[string range $txt [expr [string length $txt]-100] end]|"
			if ![regexp -indices -- $e $txt all] { set found 0 }
#puts "*** no match to excerpt"; #
		}

		set nldelta [string length [$t get $fpt "$lpt+${llen}c"]]

		if {$llen>=20} {
			# if new end too far down, assume it was searching for common text 
			# and latched onto an unsuspecting host
			if {$nldelta>[expr 10*$ldelta]} { set found 0 }
		}
	}

	if {!$found} {
		if {$lhooklen} {
			return [manHighlightAttach $t $fpt $firsttxt $predelta $fhooklen   $ldelta $lextxt 0 0  $status]

		} elseif {$llen>10} {
			set chop [max 9 [expr int($llen/2)]]
			# keep end $chop characters
			set d [expr $llen-$chop]; set lhooktxt [string range $lhooktxt $d end]
			return [manHighlightAttach $t $fpt $firsttxt $predelta $fhooklen  $ldelta $lhooktxt 0 0  "DICEY"]

		} else {
			#return "LOSE"
			# if didn't find the end, just assume it's the same distance away that it used to be
			set nldelta $ldelta
			set status DICEY
		}
	}


	# got a match
	return [list $status $fpt $nldelta]
}


#--------------------------------------------------
#
# file completion
#
#--------------------------------------------------

proc manFilecomplete {w} {
	global manx

	set t $w.show; set wi $w.info
	set line $manx(typein$w)
	set file [string trim [llast $line] "<"]
	set posn [string last $file $line]

	set ll [llength [set fc [filecomplete $file]]]
#puts stdout "$line, $file, $posn, $fc"

	# check for no matches or more than one match
	if {!$ll} {
		manWinstderr $w "no matches"
		return
	} elseif {$ll>=2} {
		foreach i $fc {lappend matches [file tail $i]}
		if {$ll<10} {
			manWinstderr $w [lrest $matches]
		} else {
			manTextOpen $w
			$t insert end [lrest $matches]
			manTextClose $w
		}
		set fc [lfirst $fc]
	}

	# show new file name
	set manx(typein$w) [string range $line 0 [expr $posn-1]]$fc

	$w.mantypein icursor end
	$w.mantypein xview moveto 1
}


#--------------------------------------------------
#
# manNewMode -- collect mode change inits in single place
#	 types of modes: section, man, txt, apropos, super, help
#
#--------------------------------------------------

proc manNewMode {w mode {n {""}}} {
	global man manx stat

	set t $w.show

	# save old highlights --> done continuously
#	manHighlights $w
	# save yview
	set manx(yview,$manx(hv$w)) [lfirst [$t yview]]

	set manx(oldmode$w) $manx(mode$w)
	set manx(mode$w) $mode
	set manx(manfull$w) ""
	set manx(catfull$w) ""
	set manx(man$w) ""
	set manx(name$w) ""
	set manx(num$w) ""

	# reset searching
#	set manx(search,string$w) ""
	set manx(search,oldstring$w) ""
	$w.search.cnt configure -text ""
	searchboxKeyNav "" Escape 0 $t $w.info 0
#	if {$mode=="section"} {searchboxKeyNav C s 0 $t "" 1}
	after 5 selection clear $t

#	set sbx(lastkeys$w) ""
	set manx(vect) 1
	set manx(try) 0
	destroy $w.sections.m; menu $w.sections.m -tearoff no
	#$w.sections.m delete 0 last; # doesn't destroy cascade menus too, which is the right thing
	$w.links.m delete 0 last


	# disable various ui buttons for different types
	#   (paths, volumes, shortcuts, history always available)
	#
	#               man help txt info section apropos super
	# yview          x   x    x   x      ?
	# sections       x   x        x
	# highlights     x   x    x
	# links          x   x
	# shortcuts +/-  x        x
	# print          x

	set high(0) disabled; set high(1) normal

	set h $high([lmatch {man help info} $mode]); $w.sections configure -state $h
	set h $high([lmatch {man txt help} $mode])
		# $w.high state controlled by highlighting code
		foreach i {hadd} {$w.$i configure -state $h}
	if {![lmatch {man txt help} $mode]} {
		foreach i {high hsub} {$w.$i configure -state disabled}
	}
	set h $high([lmatch {man help} $mode]); $w.links configure -state $h
	if {![lmatch {man txt} $mode]} {
		foreach i {sadd ssub} {$w.$i configure -state disabled}
	}
	set h $high([lmatch man $mode]); $w.occ.m entryconfigure "Kill Trees" -state $h

	manHyper $w $n
}


# set mouse clicks for hyperlinking
proc manHyper {w {n ""}} {
	global man manx

	set t $w.show

	# clear old settings
	foreach i {"Double-" ""} {
		foreach j {"" "Alt-" "Shift-"} {
			bind $t <${j}${i}ButtonRelease-1> "format nothing"
		}
	}

	if {$man(hyperclick)=="double"} {set mod "Double-" } { set mod "" }
	bind $t <ButtonRelease-1> "
		if {\$manx(hotman$t)!={}} { set manx(typein$w) \$manx(hotman$t) }
	"
	bind $t <${mod}ButtonRelease-1> "+
		if {\$manx(hotman$t)!={}} { incr stat(man-hyper); manShowMan \$manx(hotman$t) $n $w }
	"
	bind $t <Shift-${mod}ButtonRelease-1> "
		if {\$manx(hotman$t)!={}} { incr stat(man-hyper); set manx(shift) 1; manShowMan \$manx(hotman$t) $n $w }
	"
	bind $t <Alt-${mod}ButtonRelease-1> "
		if {\$manx(hotman$t)!={}} { set manx(typein$w) \$manx(hotman$t); manShowMan <\$manx(hotman$t) $w }
	"

	# (<Button-1> and <Button1-Motion> don't change)
}



#--------------------------------------------------
#
# manXXX - miscellaneous medium-level commands
#
#--------------------------------------------------

# just jab some text into the widget regardless of state
proc manTextPlug {t posn args} {
	set state [$t cget -state]
	$t configure -state normal
	eval $t insert $posn $args
	$t configure -state $state
}

proc manTextOpen {w} {
	global man

	#raise $w
	cursorBusy
	set t $w.show
	$t configure -state normal
	$t delete 1.0 end
	# delete all the marks
	foreach i [$t mark names] {if {$i!="insert"&&$i!="current"} {$t mark unset $i}}
	$t configure -font $man(currentfont)
}

proc manTextClose {w} {
	set t $w.show

	# zap final newline so last line is flush with bottom
	if {[$t get "end -1c"]=="\n"} {$t delete "end -1c"}

	$t configure -state disabled
	cursorUnset
	$t mark set xmark 1.0
}

proc manResetEnv {} {
	global env man manx mani

	set manpath {}
	foreach i $manx(paths) {if {$man($i)} {append manpath :$i}}
	set env(MANPATH) [string range $manpath 1 end]

	# also updates contents of volume listings
	foreach i $manx(manList) {
		if {[lsearch $manx(specialvols) $i]==-1 || $i=="all"} {set mani($i,form) ""}
	}
}

proc manSetSect {w n} {
	global manx mani

	set manx(cursect$w) *

	# passed full pathname, search for section
	if [regexp {^/} $n] {
		set dir [file dirname $n]
		foreach vol $mani(manList) {
			if [lsearch -exact $mani($vol,dirs) $dir]!=-1 {
				set manx(cursect$w) $vol
#				set f [lsearch -exact $mani(manList) $vol]
				break
			}
		}

	# passed alphanum, use if valid
	} elseif {[set f [lsearch $manx(manList) $n]]!=-1} {
		set manx(cursect$w) $n
	}
#		$w.volnow configure -text "([lindex $manx(manList) $f]) [lindex $manx(manTitleList) $f]"
#$w.volnow configure -text ""
}

# put entire pulldown menu on screen, using progressively smaller fonts as necessary
proc manMenuFit {m} {
	global man manx

	if {[winfo class $m]!="Menu"} {puts stderr "$m not of Menu class"; exit 1}
	if {[$m index last]=="none"} return

	set sh [winfo screenheight $m]

	# starting with current gui font size, pick smaller font as needed
	# until all entries fit on screen or out of smaller fonts
	set ok 0
	for {set i [expr [lsearch $manx(sizes) $man(gui-points)]+1]} {$i>=0} {incr i -1} {
		set p [lindex $manx(pts) $i]
		set f [spec2font $man(gui-family) $man(gui-style) $p]
		$m configure -font $f; update idletasks
		set mh [winfo reqheight $m]
#DEBUG {puts stderr "manMenuFit $m: $f, $mh, $sh"}
		if {$mh<$sh} {set ok 1; break}
	}

	# if menu still too big for screen, clip entries from bottom until it is
	if {!$ok} {
		set ctr 0
		# kind of heavyweight but don't need to do this often
		while {[winfo reqheight $m]>=$sh} {
			$m delete last; incr ctr; update idletasks
		}
		# make space for "too many" at bottom
		$m delete last; incr ctr
		$m add command -label "($ctr too many to show)" -state disabled
	}
}


#--------------------------------------------------
#
# manInit -- determine which sections exist,
#
#--------------------------------------------------

proc manInit {} {
	global man manx mani

	if {$manx(init)} return

	manReadSects
	# MANPATH may have shifted during startup
	manResetEnv
	set manx(init) 1
}


proc manReadSects {{w .man} {force 0} {msg "Building database ..."}} {
	global man manx mani env

	set wi $w.info
	zapZ! manx(database)

	# find name of database.  create if necessary
	set dbdir [file dirname $manx(database)]
	set database [lfirst [glob -nocomplain $manx(database).$manx(zglob)]]

	if {$database=="" || $force || $manx(rebuildandquit)} {
		### build database
		cursorBusy
		manWinstdout $w $msg 1

		## make database
		catch {eval exec $man(rm) -f [glob $manx(database).$manx(zglob)]}
		set tmpfile $dbdir/.tkmantmp
		if [catch {set fid [open $tmpfile w]} info] {
			puts stderr "Cannot write temporary information to $tmpfile\n=> $info"
			exit 1
		}
		set manx(manList) ""; set manx(manTitleList) ""; set manx(mandot) ""
		set manx(newmen) {}
		set buildtime [time {
		foreach i $mani(manList) j $mani(manTitleList) {
			manWinstdout $w "[manWinstdout $w] $i" 1
			if {[manReadSection $i $fid]!=0} {
				lappend manx(manList) $i; lappend manx(manTitleList) $j
			}
		}}]
		close $fid
		set man(time-lastdatabase) [expr [lfirst $buildtime]/1000000]

		# identify it with summary information
		if [catch {set fid [open $manx(database) w]} info] {
			puts stderr "Cannot write database to $manx(database)\n=> $info"
			exit 1
		}
		puts $fid "#\n# TkMan v$manx(version) database\n#"
		puts $fid "# created at [clock format [clock seconds]]"
		puts $fid "# build time $man(time-lastdatabase) seconds"
		puts $fid "#"

		# database providence
		puts $fid "# set manx(db-manpath) $manx(MANPATH0)"
		puts $fid "# set manx(db-signature) [manDBsignature]"

		puts $fid "# set manx(newmen) [list $manx(newmen)]"

		foreach i {manList manTitleList mandot} {
			puts $fid "# set manx($i) {$manx($i)}"
		}

		# append listings from $tmpfile
# close $fid, then eval exec $man(mv) $tmpfile >> $manx(database) ?
		set fid2 [open $tmpfile]
		puts -nonewline $fid [read $fid2]
		close $fid2
		close $fid

		## compress it
		# compression programs sometimes spit out statistics that exec interprets as errors
		# ==> better to know if compress has a problem, say no space left on device
		# ==> can't continue without the database, so crash if error
		if [catch {eval exec $man(compress) $manx(database)} info] {
			# assume messages reporting "%" compression are benign
			if ![string match "*%*" $info] {
				puts stderr "Problem compressing database\n=> $info"
				exit 1
			}
		}
		eval exec $man(rm) -f $tmpfile
		# should get exactly one glob match here
		set database [glob $manx(database).$manx(zglob)]

		cursorUnset
		manWinstdout $w "Database created in file [bolg $database ~]"

		# update MANPATH warnings
		#manManpathCheck
		#manHelp $w
	}
	if $manx(rebuildandquit) {exit 0}



	### read in key information from database
	# give manx(....file) the actual name, including compression suffix
	set manx(database) $database

	# read in database meta-information
	set fid [open "|$man(zcat) $manx(database)"]
	while {[gets $fid line]!=-1 && [regexp {^#} $line]} {
		if {[regexp {^# (set .*)} $line all set]} {eval $set}
	}
	# catch because pipe not empty
	catch {close $fid}



	### database validity checks

	set maxdir ""; set maxdirtime 0
	foreach i $mani(manList) {
		scan [manLatest $mani($i,dirs)] "%d %s" mstime msdir
		set mani($i,update) $mstime
		if {$maxdirtime<$mstime} {set maxdirtime $mstime; set maxdir $msdir}
	}
	set manx($manx(glimpsestrays),latest) $maxdirtime

	ASSERT file exists $database
	set rmsg ""
	if {$manx(now)} {
		# clear it for next time
		set manx(now) 0
	} elseif {[file mtime $database]<$maxdirtime} {
		set rmsg "Database out of date"
	} elseif {$manx(MANPATH0)!=$manx(db-manpath)} {
		set rmsg "MANPATH changed"
DEBUG {puts "$manx(db-manpath) => $manx(MANPATH0)"}
	} elseif {[manDBsignature]!=$manx(db-signature)} {
		set rmsg "New tkmandesc configuration"
DEBUG {puts "$manx(db-signature) =>\n[manDBsignature]"}
	}
	if {[string length $rmsg]} {
		if ![file writable $dbdir] {
			after 2 "manWinstderr $w \"$rmsg ... but database directory not writable\""
		} else {
			manReadSects $w 1 "$rmsg, rebuilding ..."
			# check for misset clocks (leaving some man directory with later time than new database)
			set newupdate [file mtime $manx(database)]
			if {$newupdate<$maxdirtime} {
				set errmsg "
					Problem with file and directory times...\a\a
					The newly created database ($manx(database)) has its time set
					earlier than the $maxdir directory and perhaps others.
					Unless corrected, the database will be rebuilt every time
					TkMan is started.  Perhaps you need to reset your system clock.
				"
				puts stderr [linebreak $errmsg 70]
			}

			# important return below
			return
		}
	}

	if ![llength $manx(manList)] {
		puts stderr "Can't find any man pages!"
		puts stderr "MANPATH = $env(MANPATH)"
		exit 1
	}

	# dynamically formatted
	foreach i $manx(manList) {set mani($i,form) ""}

	# add standard pseudo-volumes
	foreach i $manx(extravols) {
		set letter [lfirst $i]; set title [lsecond $i]; set msg [lthird $i]
		lappend manx(manList) $letter; lappend manx(manTitleList) $title
		set mani($letter,form) $msg; set mani($letter,cnt) 0; set mani($letter,update) ""
	}


	### also need to rebuild section pulldown here
	foreach win [lmatches ".man*" [winfo children .]] {
		manMakeVolList $win
		$win.occ.m entryconfigure "Rebuild Database*" -label "Rebuild Database (last took $man(time-lastdatabase) sec)"
	}
}


proc manLatestMan {root} {
	# compare index date against dates of SUBDIRECTORES
	# SGI sub-subdirectories lose out here... but these mostly just the preinstalled ones anyway
	#	if [catch {set men [exec $man(find) $root -type d -name 'man?*' -print]} info] { set men $info }

	set men [glob -nocomplain "$root/man?*"]
	# in cat-only situation?
	if {$men==""} {
		set men [glob -nocomplain "$root/*"]
		#if [catch {set men [exec $man(find) $root -type d -print]} info] { set men $info }
	}

	return [manLatest $men]
}

proc manLatest {dirs} {
	global mani

	set max 0; set maxdir ""
	foreach sub $dirs {
		if {[file isdirectory $sub]} {
			set mtime [file mtime $sub]
			set mani($sub,update) $mtime
			if {$mtime>$max} { set max $mtime; set maxdir $sub }
		}
	}

	return [list $max $maxdir]
}



# calculate signature on paths used to build database,
# so can identify changes in MANPATH and with tkmandesc, and rebuild automatically

proc manDBsignature {} {
	global mani

	# set first character of sig to letter so Tcl won't try to interpret string as a number
	set sig "X"
	foreach i $mani(manList) {
		if [llength $mani($i,dirs)] {append sig $i[llength $mani($i,dirs)]}
	}
	return $sig
}



#--------------------------------------------------
#
# manReadSection
#
#--------------------------------------------------

proc manReadSection {n fid} {
	global man manx mani env

	# follow all paths here, even if search presently turned off for that dir
#	if ![info exists mani($n,dirs)] {return 0}
	if ![llength $mani($n,dirs)] {return 0}

	set title [lindex $mani(manTitleList) [lsearch $mani(manList) $n]]
DEBUG { puts -nonewline stderr $n }
#	set mani($n) ""
	set first 1
	set cnt 0

	# collect recent additions/changes
DEBUG {puts stderr ": $man(find) $mani($n,dirs) -type f -mtime -$man(recentdays) -print"}
	if {![catch {set lines [eval exec $man(find) $mani($n,dirs) -type f -mtime -$man(recentdays) -print]}] && $lines!=""} {
DEBUG {puts -nonewline "F"}
		set short {}
		foreach k [lsort $lines] {
			set f [zapZ [file tail $k]]
			if {![string match "*~" $f]} {lappend short $f}
		}
		# guard against directory change being deleted files only
		if {$short!=""} {lappend manx(newmen) [list $n $short]}
	}


	# collect names from directory
	foreach i $mani($n,dirs) {
		foreach k [glob -nocomplain $i/*] {
#DEBUG { puts stderr $k }
			if {$first} {puts $fid "$n [list $title]"; set first 0}
			# normalize names in stripping compression suffixes
			set sgi [string match "*/cat*/*.z" $k]
			zapZ! k
			set f [file tail $k]
			# no Emacs backup files (~) and simple check for manual page-ness (contains a ".")
			# ==> SGI doesn't have any dots after chopping off .z!
			if {![string match "*~" $f] && ![string match "*.bak" $f]} {
				if {[string match "*.*" $f] || $sgi} {
#					puts $fid [file root $k]
					puts $fid $k
					incr cnt

					# amass mandots
					if {[regexp {\..*\.} $f] || ($sgi && [regexp {\.} $f])} {
						lappend manx(mandot) [file rootname $f]
					}
				}
			}
		}
	}

	return $cnt
}


proc manFormatSect {n} {
	global man manx mani high

	set form ""

	if {$n=="high"} {
		set mani(high,form) ""
		set ltmp [lsort -command manSortByName [array names high]]
		foreach k $ltmp {
			if {[llength $k]>1} continue
# || $k=="*"
			append mani(high,form) $k
			if {[llength [lsecond $high($k)]]>1} { append mani(high,form) "\t[recently [lfirst $high($k)]]" }
			append mani(high,form) "\n"
		}
		set mani(high,cnt) [llength $mani(high,form)]
		if {$mani(high,cnt)==0} {set mani(high,form) "No manual pages have been annotated.\n"}

		return

	} elseif {$n=="recent"} {
		set first 1; set form ""; set cnt 0
		foreach i $manx(newmen) {
			set vol [lfirst $i]; set names [lsecond $i]
			if {$first} {set first 0} {append form "\n\n\n"}
			if {[set index [lsearch $mani(manList) $vol]]!=-1} {
				set voltxt "Volume $vol, \"[lindex $mani(manTitleList) $index]\", updated at [recently $mani($vol,update)]"
			} else { set voltxt $vol }
			append form "$voltxt\n\n"
			foreach j $names {append form $j "\t"; incr cnt}
		}
		set mani(recent,cnt) $cnt
		if {$mani(recent,cnt)==0} {
			set form "There are no manual pages less than $man(recentdays) day"
			if {$man(recentdays)>1} {append form "s"}
			append form " old.\n"
			append form "You can change the definition of `recent' in Occasionals/Preferences/Misc.\n"
		}
		set mani(recent,form) $form
		return
	}


	# get all the names -- wow, sed is actually useful,
	#    though it could use the regular expressions it claims it has, like the '|' operator
	# show only pages in current volumes
	# (only volume header lines have spaces)
	# sed varies: sed on SunOS starts checking for end of range at next line, gsed on same line
	# => so find exact end pattern
	set nextn [lindex $manx(manList) [expr [lsearch -exact $manx(manList) $n]+1]]
	if {$n=="all"} {set sect ""} {set sect "-e {/^$n /,/^$nextn /!d}"}
	set sed "$man(sed) $sect -e {/ /d}"
	# exclude pages from disabled paths
	foreach i $manx(paths) {
		if {[info exists man($i)] && !$man($i)} {
			append sed " -e {\\?^$i?d}"
		}
	}
	# delete the directory, leaving just the filename
	append sed " -e {s/.*\\///g}"
#-e {s/\\.z\$//}  -e {s/\\.gz\$//}  -e {s/\\.Z\$//}
DEBUG {puts "sed = $sed"}
	set ltmp [eval exec "$man(zcat) $manx(database) | $sed | $man(sort)"]
	#set ltmp [lsort [eval exec "$man(zcat) $manx(database) | $sed"]]}]

	if {$man(volcol)=="0"} {set sep "  "} {set sep "\t"}
#	set cnt [llength $ltmp]
	set cnt 0

	# specialized version of uniqlist
	set pr ""; set pe ""; set pl ""
	set online 0; set skip 0
#set xtmp [lfirst $ltmp]; set xdir [file dirname [file dirname $xtmp]]
#puts "$xtmp => $xdir, exists [info exists man($xdir)], ok $man($xdir)"
	foreach i $ltmp {
		set ir [file rootname $i]; set ie [file extension $i]; set il [string tolower [string range $i 0 0]]

		if {$pl!=$il && $online>0} {
# && ($n==3 || ![string match {[A-Z]} [string range $i 0 0]])} {
			append form "\n\n"
			set online 0
		}

		if {$pr!=$ir} {
			if {$online!=0} {append form $sep}
			append form $ir; incr online
			set skip 0
		} elseif {$pe==$ie} {
#			append form +
#			set skip 0
		} else {
			if {$online!=0} {
				if {!$skip} {append form $pe}
				append form $sep
			}
			append form $i; incr online
			set skip 1
		}

		set pr $ir; set pe $ie; set pl $il
		incr cnt
	}

	if {$cnt==0} {set form "No man pages in currently enabled paths.\nTry turning on some under the Paths pulldown menu."}
	set mani($n,form) $form
	set mani($n,cnt) $cnt
}


#--------------------------------------------------
#
# mandesc-like features
#
# mani(<single letter>) => name of full title main index
# mani(<letter>) => listing of full dir & list of names
# mani(<letter>,form) => formatted?
# mani(<letter>,cnt) => # of entries
# mani(<letter>,dirs) => paths x sections cross product, w/mandesc diddling
# --> everybody has a letter and letters aren't repeated
#
#--------------------------------------------------


# cross product of paths by sections
# if LANG environment variable is set, first try to read directories with that name

proc manDescDefaults {} {
	global man manx mani env

	# don't check manx(defaults) here
	manDescManiCheck return

	if [info exists env(LANG)] {set langs [split $env(LANG) ":"]} {set langs ""}
	foreach i $mani(manList) { set mani($i,dirs) {} }
	foreach i $manx(paths) { set mani($i,dirs) {} }
	set mani($man(glimpsestrays),dirs) {}
	set curdir [pwd]

	foreach i $manx(paths) {
		cd $i
		set alldirs [glob -nocomplain $manx(subdirs)]
		foreach l $langs {
			set alldirs [concat $alldirs [glob -nocomplain $l/$manx(subdirs)]]
		}
#DEBUG {puts "    alldirs = $alldirs"}
DEBUG {puts "    sorted alldirs = [lsort -command bytypenum $alldirs]"}
		foreach d [lsort -command bytypenum $alldirs] {
			if {[string match "*/*" $d]} {
				set lang "[file dirname $d]/"; set dir [file tail $d]
			} else {set lang ""; set dir $d }
			# keep both manN and manN.Z, with manN.Z preferred
#			set dir [file rootname $d]; set type [string range $dir 0 2]
			set dirsig [string range $dir $manx(dirnameindex) end]
			# purpose of following line?
			if {[string match "cat*" $dir]} {set dirsig [file rootname $dirsig]}
			set num [file rootname $dirsig]; set num1 [string index $num 0]

			# what section group does it belong to?
			if {[lsearch -exact $mani(manList) $num]!=-1} {set n $num} {set n $num1}

			# check for conflicts with other abbreviations starting with same letter
			set pat "^[stringregexpesc $i]/${lang}(man|cat)[stringregexpesc $dirsig]\$"
			set dir $i/$d
			if {[lsearch -regexp $mani($i,dirs) $pat]==-1 && [manPathCheckDir $dir]==""} {
# following line would be a good check, but takes too long to start up -- actually, part of manPathCheckDir
#				 && [glob -nocomplain $dir/*]!=""} {
				lappend mani($i,dirs) $dir
				lappend mani($n,dirs) $dir
			}
		}
DEBUG {puts "mani($i,dirs) = $mani($i,dirs)"}
	}
DEBUG {foreach i $mani(manList) { puts "mani($i,dirs) = $mani($i,dirs)" }}

	cd $curdir
	set manx(defaults) 1
}

proc bytypenum {a b} {
	if [string match "*/*" $a] { set al [file dirname $a]; set a [file tail $a] } {set al ""}
	if [string match "*.Z" $a] { set as [file extension $a]; set a [file rootname $a] } {set as ""}
	set at [string range $a 0 2]
	if [string match "*/*" $b] { set bl [file dirname $b]; set b [file tail $b] } {set bl ""}
	if [string match "*.Z" $b] { set bs [file extension $b]; set b [file rootname $b] } {set bs ""}
	set bt [string range $b 0 2]

	# priority: language, type (man or cat), number, .Z suffix
	if {$al!=$bl} {
		if {$al==""} {return 1} elseif {$bl==""} {return -1} elseif {$al<$bl} {return -1} {return 1}
	} elseif {$at!=$bt} {
		if {$at=="man"} {return -1} {return 1}
	} elseif {$as!=$bs} {
		# prefer .Z
		if {$as==""} {return 1} {return -1}
	} else {
		if {$a<$b} {return -1} {return 1}
	}

	return 0
}


# commands: move, copy, delete
# from, to *list* of source, target dirs
# list is list of directory *patterns*

proc manDescMove {from to dirs} {manDesc move $from $to $dirs}
proc manDescDelete {from dirs} {manDesc delete $from "" $dirs}
proc manDescCopy {from to dirs} {manDesc copy $from $to $dirs}
set manx(mandesc-warnings) ""
proc manDescAdd {to dirs} {
	global mani manx man

	set manx(mandesc-warnings) ""
	set warnings ""

	manDescManiCheck
	foreach d $dirs {
		if {[set warnmsg [manPathCheckDir $d]]!=""} {
			append warnings $warnmsg
		} else {
			foreach t $to {lappend mani($t,dirs) $d}

			# try to attach these oddball directories to some MANPATH
			# in order to make them available for Glimpse indexing
DEBUG {puts "MANPATH for $d?"}
			set mp $d
			while {[string match "/*" $mp] && $mp!="/"} {
				if {[lsearch -exact $manx(paths) $mp]>=0} {
DEBUG {puts "\tyes, in $mp"}
					lappend mani($mp,dirs) $d; break
				} else {set mp [file dirname $mp]}
			}
			if {$mp=="/"} { lappend mani($man(glimpsestrays),dirs) $d
DEBUG {puts "\tno, added to strays\n\t\tnow mani($man(glimpsestrays),dirs) =  $mani($man(glimpsestrays),dirs)"}
			}
		}
	}


	if {$warnings!=""} {
		if ![string match *manDescAdd* $manx(mandesc-warnings)] {
			append manx(mandesc-warnings) "Problems with manDescAdd's...\n"
		}
		append manx(mandesc-warnings) $warnings
	}
}


proc manDesc {cmd from to dirs} {
	global man manx mani

	manDescManiCheck
	if {$from=="*"} {set from $mani(manList)}
	if {$to=="*"} {set to $mani(manList)}
	foreach n [concat $from $to] {
		if [lsearch $mani(manList) $n]==-1 {
			puts stderr "$cmd: Section letter `$n' doesn't exist."
			exit 1
		}
	}

DEBUG {puts stdout "$cmd {$from} {$to} {$dirs}"}
	foreach d $dirs {
		foreach f $from {
			set newdir {}
			foreach fi $mani($f,dirs) {
				if [string match $d $fi] {
					if {$cmd=="copy"} {lappend newdir $fi}
					if {$cmd=="copy"||$cmd=="move"} {
						foreach t $to {if {$f!=$t} {lappend mani($t,dirs) $fi} {lappend newdir $fi}}
					}
				} else {lappend newdir $fi}
			}
			set mani($f,dirs) $newdir
DEBUG {puts stdout $f:$mani($f,dirs)}
		}
	}
}

proc manDescAddSects {l {posn "end"} {what "n"}} {
	global man mani

	manDescManiCheck
	if {$posn=="before"||$posn=="after"} {set l [lreverse $l]}
	foreach i $l {
		set n [lfirst $i]; set tit [lsecond $i]
		if [lsearch $mani(manList) $n]!=-1 {
			puts stderr "Section letter `$n' already in use; request ignored."
			continue
		}

		if {$posn=="end"} {
			lappend mani(manList) $n
			lappend mani(manTitleList) $tit

		} elseif {$posn=="before"||$posn=="after"} {
			if [set ndx [lsearch $mani(manList) $what]]==-1 {
				puts stderr "Requested $posn $what, but $what doesn't exist; request ignored"
				continue
			}
			if {$posn=="after"} {incr ndx}
			set mani(manList) [linsert $mani(manList) $ndx $n]
			set mani(manTitleList) [linsert $mani(manTitleList) $ndx $tit]

		} elseif {$posn=="sort"} {
			lappend mani(manList) $n
			set mani(manList) [lsort $mani(manList)]
			set ndx [lsearch $mani(manList) $n]
			set mani(manTitleList) [linsert $mani(manTitleList) $ndx $tit]
		}

		set mani($n,dirs) {}
	}
}


# if mani array, matrix not already created, do it now

proc manDescManiCheck {{action "exit"}} {
	global man mani manx env

	# check for missing call to manDot
	if {!$manx(manDot)} manDot

	if ![info exists mani(manList)] {
		set mani(manList) $man(manList)
		set mani(manTitleList) $man(manTitleList)

		# have to be done playing with MANPATH before start playing with results
		manManpathSet
		manManpathCheck

		if {$action=="return"} return
		manDescDefaults
	}
}


# place SGI's magically-appearing catman subdirectories into volumes

proc manDescSGI {patterns} {
	global man manx mani

	# start up checks for patterns
	# (=> maybe go with (vol name pats) tuples)

	set paterrs 0
	foreach pat $patterns {
		set mapto [lfirst $pat]; set patlist [lsecond $pat]

		# make sure each mapping exists as a volume that's been added
		if [lsearch -regexp $mani(manList) ".?$mapto"]==-1 {
			puts stderr "no volume corresponding to $mapto mapping (patterns: $patlist)"
			incr paterrs
		}

		# make sure patterns don't overlap
		#    can't have identical patterns (ftn =>! 2f and 3ftn.)
		#    either: 2/ftn=>2f and 3/ftn=>3ftn)  *or*  merge with ftn => 2ftn and 3ftn
		foreach p $patlist {
			foreach pat2 $patterns {
				if {$pat==$pat2} break
				set mapto2 [lfirst $pat2]; set patlist2 [lsecond $pat2]
				foreach p2 $patlist2 {
					if [string match $p2 $p] {
						puts stderr "pattern $p never reached -- $mapto2's $p2 precludes it"
						incr paterrs
					}
				}
			}
		}
	}

	# add a guaranteed match, that's placed in single-letter volume
	lappend patterns {"" {""}}
	DEBUG {puts "mani(manList) => $mani(manList)"}

	# collect .../catman directories (know they're readable by this point)
	set catmen {}
	foreach d $manx(paths) {
#[split $manx(MANPATH0) ":"] {
		if [string match "*/catman" $d] { lappend catmen $d }
	}
	if ![llength $catmen] {
		puts stderr "No sneaky catman directories found in MANPATH:\n\t$manx(MANPATH0)"
		return
		#exit 1
	}


	if [catch {set catmandirs [eval exec $man(find) $catmen -type d -print]} info] {
		puts stderr "Trouble reading $catmen -- $info"
		set catmandirs {}
	}

	foreach dir $catmandirs {
		if [regexp {(catman|_man)$} $dir] continue
		set tail [file tail $dir]
		set vol [file tail [file dirname $dir]]
		if ![regexp "^(man|cat)" $vol] { set vol $tail; set tail "" }

		set volnum [string range $vol 3 3]

		DEBUG {puts -nonewline "$dir ($vol:$tail ($volnum)) => "}
		set matched 0
		foreach pat $patterns {
			set mapto [lfirst $pat]; set patlist [lsecond $pat]
			foreach dp $patlist {
				if [string match "*$dp" $dir] {
					DEBUG {puts -nonewline "match on $dp => "}
					set matched 1
					# first try to place in mapto name in current volume
					if [lsearch -exact $mani(manList) "$volnum$mapto"]!=-1 {
						DEBUG {puts $volnum$mapto}
						manDescAdd "$volnum$mapto" $dir

					# then try mapto name in any volume
					} elseif [lsearch -exact $mani(manList) $mapto]!=-1 {
						DEBUG {puts $mapto}
						manDescAdd $mapto $dir

			 		# else place in a parent volume
					} elseif [lsearch -exact $mani(manList) $volnum]!=-1 {
						DEBUG {puts $volnum}
						manDescAdd $volnum $dir

					} else {
						DEBUG {puts "can't place"}
					}

					break
				}
			}
			if {$matched} break
		}
		DEBUG {if {!$matched} {puts "CAN'T MATCH\a\a"}}
	}
}



# for debugging
proc manDescShow {} {
	global man manx mani

	manDescManiCheck
	puts stdout "*** manDescShow"
	foreach i $mani(manList) {
#		if [info exists mani($i,dirs)] {
		puts stdout $i:$mani($i,dirs)
#}
	}
}


#--------------------------------------------------
#
# manBug -- maybe not
#
#--------------------------------------------------

proc manBug {w} {
	global man manx mani high default stat env argv0

	if $manx(shift) { set manx(shift) 0; set w [manInstantiate] }
	set t $w.show; set wi $w.info; set m $w.sections.m

	manNewMode $w bug; incr stat(bug)
	set manx(hv$w) bug
	set head "Submit Bug Report"
	wm title $w $head
	manWinstdout $w $head
	$w.search.cnt configure -text ""

	manTextOpen $w

	$t insert end "Select all the text below and paste into your e-mail program.\n"
	$t insert end "Before submitting a bug report, first check the home ftp site (ftp://ftp.cs.berkeley.edu/ucb/people/phelps/tcltk) to make sure it hasn't already been fixed in a later version." i "\n\n"


	# maybe link up with exmh
	$t insert end "To: phelps@cs.berkeley.edu\n"
	$t insert end "Subject: TkMan bug report\n\n"

	$t insert end "Describe the problem:\n\n\n\n" h1


	$t insert end "System description information\n\n" h1

	$t insert end "X Windows version: _____\n"
	$t insert end "window manager: ______\n"

	global tk_patchLevel
	$t insert end "TkMan version $manx(version)\n"
	$t insert end "MANPATH = $manx(MANPATH0)\n"
	$t insert end "Tcl [info patchlevel]/Tk $tk_patchLevel\n"
	catch { $t insert end "Hardware: [exec uname -a]\n" }

	if {[file readable $manx(startup)]} {
		$t insert end "\n\nVariables overridden in [bolg $manx(startup) ~]\n\n" h1
		set allold 1
		foreach i [lsort [array names default]] {
			set new [tr $man($i) \n " "]; if {$new==""} {set new "(null)"}
			set old [tr $default($i) \n " "]; if {$old==""} {set old "(null)"}
			if {$new!=$old} {
				$t insert end "man(" {} $i b ") = " {} $new tt ", " {} "formerly " i $old tt "\n"
				set allold 0
			}
		}
		if {$allold} { $t insert end "None\n" }
		$t insert end "\n\n\n"
	}

	# executable and Makefile probably OK if got to this point
if 0 {
	# first few lines
	if [file readable $argv0] {
		$t insert end "First few lines of `tkman' executable\n\n" h1
		set fid [open $argv0]
		for {set i 0} \$i<6 {incr i} {$t insert end [gets $fid] {} \n}
		catch {close $fid}
	}

	# include 400+ line file here?
#	$t insert end $manx(Makefile)
}

	manTextClose $w
	manHighView $w
}


#--------------------------------------------------
#
# manInfo
#
#--------------------------------------------------

proc manInfo {w} {
	global man manx mani high default stat env

	if $manx(shift) { set manx(shift) 0; set w [manInstantiate] }
	set t $w.show; set wi $w.info; set m $w.sections.m

	manNewMode $w info; incr stat(info)
	set manx(hv$w) info
	set head "Statistics and Information"
	wm title $w $head
	manWinstdout $w $head
#	$w.search.cnt configure -text ""

	manTextOpen $w

	$m add command -label "statistics" -command "incr stat(page-section); $t yview 1.0"
	$t insert end "Statistics  " h1

	$t insert end "since startup at [clock format $manx(startuptime) -format $man(date-format)]" i
	$t insert end "\t\[cumulative since [clock format $stat(cum-time) -format $man(date-format)]\]\n\n" i
	manStatsSet

	foreach i $manx(modes) desc $manx(mode-desc) {
		$t insert end "$stat($i)\t$desc " "" "\[$stat(cum-$i)\]\n" i
		if {$i=="man"} {
# && $stat(man)} {
#			if $stat(man-no) {
				$t delete end-1c
				$t insert end ", $stat(man-no) not found " "" "\[$stat(cum-man-no)\]\n" i
#			}
			$t insert end "\t"
			set first 1
			foreach j $manx(stats) desc $manx(stats-desc) {
				if [string match "man-*" $j] {
					#if {!$stat($j)} continue
					if {!$first} {$t insert end ",  "}
					$t insert end "$stat($j)  $desc " "" "\[$stat(cum-$j)\]" i
					set first 0
				}
			}
			$t insert end "\n"
		}
	}

	set pageops 0; set cpageops 0
	foreach i [array names stat page-*] {
		incr pageops $stat($i)
		incr cpageops $stat(cum-$i)
	}
	$t insert end "$pageops\tpage navigation operations " "" "\[$cpageops\]\n" i
#	if {$pageops>0} {
		$t insert end "\t"
		set first 1
		foreach j $manx(stats) desc $manx(stats-desc) {
			if [string match "page-*" $j] {
				#if {!$stat($j)} continue
				if {!$first} {$t insert end ",  "}
				$t insert end "$stat($j)  $desc " "" "\[$stat(cum-$j)\]" i
				set first 0
			}
		}
		$t insert end "\n"
#	}

	$t insert end "$stat(high-move)\thighlights repositioned automatically  " "" "\[$stat(cum-high-move)\]\n" i
	$t insert end "$stat(high-lose)\thighlights unattachable  " "" "\[$stat(cum-high-lose)\]\n" i

	$t insert end "$stat(print)\tpages printed  " "" "\[$stat(cum-print)\]\n" i

	$t insert end "TkMan version $manx(version)\n"
	global tk_patchLevel
	$t insert end "\nRunning under Tcl [info patchlevel]/Tk $tk_patchLevel\n"
	$t insert end "[info cmdcount] Tcl commands executed\n"
	catch { $t insert end "Hardware: [exec uname -a]\n" }


	if {[file readable $manx(startup)]} {
		$m add command -label "variables" -command "incr stat(page-section); $t yview [$t index end]"
		$t insert end "\nVariables overridden in [bolg $manx(startup) ~]\n\n" h1
		set allold 1
		foreach i [lsort [array names default]] {
			set new [tr $man($i) \n " "]; if {$new==""} {set new "(null)"}
			set old [tr $default($i) \n " "]; if {$old==""} {set old "(null)"}
			if {$new!=$old} {
				$t insert end "man(" {} $i b ") = " {} $new tt ", " {} "formerly " i $old tt "\n"
				set allold 0
			}
		}
		if {$allold} { $t insert end "None\n" }
	}

	$m add command -label "full paths" -command "incr stat(page-section); $t yview [$t index end]"
	$t insert end "\nFull paths of supporting executables\n\n" h1
	set allfull 1
	foreach i $manx(binvars) {
		set val [set $i]
		$t insert end "$i = $val.   "
		foreach j [split $val "|"] {
			set bin [lfirst $j]
			if ![string match "/*" $bin] {
				$t insert end "  $bin" b " => " "" $stat(bin-$bin) tt
				set allfull 0
			}
			if [info exists stat(bin-$bin-vers)] { $t insert end "  (v$stat(bin-$bin-vers))" }
		}
		$t insert end "\n"
	}
	if {!$allfull} {
		$t insert end "\n" "" "PATH" tt "  environment variable is " "" $env(PATH) tt "\n"
	}

	
	$m add command -label "dates" -command "incr stat(page-section); $t yview [$t index end]"
	$t insert end "\nDates\n\n" h1
	if {[file readable $manx(startup)]} {
		$t insert end "Startup " b "file last save\n\t" {} $manx(startup) tt "\t" {} [recently [file mtime $manx(startup)]] i "\n"
#  last saved at [clock format [file mtime $manx(startup)] -format $man(date-format)]\n"
	}
	$t insert end "Man page " {} "database" b " last update\n\t" {} $manx(database) tt "\t" {} [recently [file mtime $manx(database)]] i "\n"
#"  last updated at [clock format [file mtime $manx(database)] -format $man(date-format)]\n"
	# iterate through all glimpse directories?
	if {$man(glimpse)!=""} {
		if {$man(indexglimpse)=="distributed"} {
			$t insert end "Distributed Glimpse " {} "indexes" b " latest updates\n"
			set paths $manx(paths)
		} else { 
			$t insert end "Unified Glimpse " {} "index" b " latest update\n"
			set paths $man(glimpsestrays)
		}

		foreach i $paths {
			set db $i/.glimpse_index
			$t insert end "\t" {} $i tt "\t"
			if [file exists $db] {
#				set date [clock format [file mtime $db] -format $man(date-format)]
				$t insert end [recently [file mtime $db]] i "\n"
			} else { $t insert end "no index" b "\n" }
		}
	}


	$m add command -label "volume mappings" -command "incr stat(page-section); $t yview [$t index end]"
	$t insert end "\nVolume-to-directory mappings\n\n" h1

	set manpath0 [join $manx(paths) ":"]
	$t insert end "MANPATH" sc ": " "" $manpath0 tt "\n"
	if {$manx(MANPATH0)!=$manpath0} {
		$t insert end "As cleaned up from original " "" "MANPATH" sc ": "
		foreach path [split $manx(MANPATH0) ":"] {
			$t insert end $path [expr [lsearch -exact $manx(paths) $path]==-1?"i":"tt"] ":" tt
		}
		$t delete end-2c; $t insert end "\n"
	}
	$t insert end "\n"

	foreach i $mani(manList) {
		if {![llength $mani($i,dirs)]} continue
		$t insert end "Volume $i, [lindex $mani(manTitleList) [lsearch $mani(manList) $i]]\n" b
		foreach j $mani($i,dirs) { 
			$t insert end "\t" {} $j tt "\t" {} [recently $mani($j,update)] i "\n"
		}
	}
	# also show stray cats, if any


	$m add command -label "change log" -command "incr stat(page-section); $t yview [$t index end]"
	$t insert end "\nLog of Some Less-trivial Changes\n" h1
	$t insert end $manx(changes)

	$t tag add info 1.0 end
	manTextClose $w

	scan [$t index end] %d eot
	$w.search.cnt configure -text "$eot lines"

	manHighView $w
}


#--------------------------------------------------
#
# manShowSection -- show listing
#
#--------------------------------------------------

proc manShowSection {w n} {
	global man manx mani high font stat

	if {[lsearch $manx(manList) $n]==-1} { manWinstderr $w "Volume $n not found"; return }
	if $manx(shift) { set manx(shift) 0; set w [manInstantiate] }
	set t $w.show; set wi $w.info

	manNewMode $w section $n; incr stat(section)
	wm title $w "$manx(title$w): Volume $n"
	set manx(lastvol$w) $n
	set manx(hv$w) $n

	set head [lindex $manx(manTitleList) [lsearch $manx(manList) $n]]

	if {$mani($n,form)==""} {
		cursorBusy
		manWinstdout $w "Formatting $head ..." 1
		manFormatSect $n
		cursorUnset
	}

	set info $head
#	if {$mani($n,update)!=""} { append info "   (last update [clock format $mani($n,update) -format $man(date-format)])" }
	manWinstdout $w $info
	manSetSect $w $n

	manTextOpen $w
	$t insert end $mani($n,form)
	$w.search.cnt configure -text "$mani($n,cnt) entries"
	manTextClose $w

	manHighView $w
#	focus $t
	# apropos needs special formatting tags
	if {$n=="apropos" || $n=="high"} { set tag $n } else { set tag volume }
	$t tag add $tag 1.0 end 

	# rewrite "Last volume viewed" to show what volume that will be
	set lastvol [$w.vols.m cget -tearoff]
	$w.vols.m entryconfigure $lastvol -label "($n) $head" -state normal
	set manx(name$w) $head
}


proc manSortByName {a b} {
	set c [string tolower [file tail $a]]; set d [string tolower [file tail $b]]
	if {$c<$d} {set cmp -1} elseif {$c==$d} {set cmp 0} {set cmp 1}
	return $cmp
}



#--------------------------------------------------
#
# manShowMan -- given various formats of names,
#	 search for that man page.  if successful, call manShowManFound
#
# don't use `man' to show because may want to *always*
#	 format with my own macros
#
#--------------------------------------------------

proc manShowMan {fname {goodnum ""} {w .man}} {
	global man manx mani env stat
#DEBUG {puts stdout "manShowMan: $fname $goodnum $w"}

	if $manx(shift) { set manx(shift) 0; set w [manInstantiate] }
	set wi $w.info

	# establish valid variable values
	if {[string trim $fname]==""} return
	# goodnum comes from double clicks from sections or previously parsed section number
	if {[lsearch $manx(specialvols) $goodnum]!=-1} {set goodnum ""}

	# given full path?  if so, don't need to search
	if {[regexp {^(\.\./|\./|[~/$])} $fname]} {
#puts "full path match on $fname"
		return [manShowManFound [fileexp [lfirst $fname]] 0 $w]
	}

	# `|command' for shell command, whose text is sucked into search box,
	#    to use with lman, say
	# `< file' syntax for file to suck up raw text
	# for more sophisticated file reading, use NBT

	if {[regexp {^[|<]} $fname]} {manShowText $fname $w; return}


	### need to search for name

	# protect backslashes
	regsub "\\\\" $fname "\a" fname

	# construct stem, section, extension
	set oname [string trimright [string trim [lfirst $fname] { ,?!;"'}] .]
	if {$goodnum!=""} {set tmp "($goodnum)"} {set tmp ""}
	manWinstdout $w "Searching for \"$oname$tmp\" ..." 1
	set fname [string tolower $oname]; # lc w/  () for regexp
	set sname [string trim $fname ()]; # lc w/o ()
	set oname [string trim $oname ()]; # original case w/o ()

	set name $sname; set num ""; set ext ""
# set sect {\..};

	# extract section number in `man(sect)' format
	if {[regexp -nocase {([a-z0-9_.+-]+)([\t ]*)\(([^)]?)(.*)\)} $fname \
		 all name spc num ext]} {
#		set sed "| $man(sed) /^$num /,/^\[^$num\] /p;d"
#		set sect "\\\..$ext"

	# if you have a dot, assume it's the section number
	} elseif {[regexp {(.+)\.(.)([^.]*)$} $oname all namex sectx extx]
# bad sections fixed up later--just check for mandots here
		&& [lsearch -exact $manx(manList) $sectx]!=-1
		&& [lsearch -exact $manx(mandot) $oname]==-1} {
#		set sect {\.}
		set name $namex; set num $sectx; set ext $extx
#		set num [string trimleft [file extension $oname] .]
#		set name [file rootname $oname]
#		set sed "| $man(sed) /^$num\$/,/^\[^$num\]\$/ p; d"
DEBUG {puts "num = $num"}
	}


	# given section number
	if {$goodnum!=""} {
#		set sect "\\\.$goodnum"
#		set sect {\.}
#		set sed "| $man(sed) /^$goodnum /,/^\[^$goodnum\] /p;d"
		set num $goodnum
	}

	# check for multicharacter section "letters"
	if {[lsearch -exact $manx(manList) "$num$ext"]!=-1} {
		set num "$num$ext"; set ext ""
	}

# get section number in `man.sect' format
# no .z's here

	set ext [string tolower $ext]
	if $man(shortnames) {set name [string range $name 0 10].*}
	if [catch {regexp $name validregexpcheck}] { set name [stringregexpesc $name] }
	# restore backslashes
	foreach v {fname sname oname name} { regsub "\a" [set $v] "\\" $v }

	# search!
	cursorBusy
DEBUG {puts stdout "$name : $num : $ext"}
	set foundList [manShowManSearch $name $num $ext]
	cursorUnset

	# if no matches with extension, look for one without
	set found [llength $foundList]
	if {!$found} {
DEBUG {puts stdout "$name not found: $ext, $num, [file rootname $name], [stringregexpesc $name]"}
		if {$ext!=""} {
			manShowMan $name $num $w; return
		} elseif {$num!="" || [file rootname $name]!=$name} {
			manShowMan [file rootname $name] {} $w
			return
		} elseif {$name!=[stringregexpesc $name] && ![string match {*\\*} $name]} {
			# case when name has embedded regexp meta characters which are part of valid regexp
			manShowMan [stringregexpesc $name] {} $w
			return
		}
	}

	# act on results of search
	if {!$found} {
		manWinstderr $w "$sname not found"
		incr stat(man-no)
		set r "ERROR: manual page not found"
	} else {
		set orname [file rootname $oname]
#puts "sorting $foundList"
		set prioritylist [lsort -command {manShowManSort $orname $manx(cursect$w) $num $ext} $foundList]
		return [manShowManFound $prioritylist 0 $w]
	}
}


proc manShowManSort {orname onum num ext a b} {
	global manx

	set pa [manShowManPriority $a $num $ext $orname $onum]
	set pb [manShowManPriority $b $num $ext $orname $onum]

	# greater priority placed earlier
	if {$pa>$pb} {set r -1} elseif {$pa<$pb} {set r 1} else {set r 0}
	return $r
}

proc manShowManPriority {m num ext orname onum} {
	global man manx mani

	# locals: d=directory, t=tail, n=number, e=extention
	set d [file dirname $m]; set t [file tail $m]
	set r [file rootname $t]; set n [file extension $t]
	set e [string range $n 2 end]; set n [string range $n 1 1]

	# determine priority value
	# levels: name, lc names, num, ext, num==onum
	set pri 0
# I think $n==$num is true for all or none
	if {$n==$num} {incr pri 16} elseif {$n==$onum} {incr pri 1}
	if {$r==$orname} {incr pri 8} elseif {[string tolower $r]==[string tolower $orname]} {incr pri 4}
	if {$e==$ext} {incr pri 2}
	set pri [expr $pri<<7]

	# tie breaker #1: MANPATH order
	set p -1; set l [llength $manx(paths)]
	while {$d!="/" && [set p [lsearch -glob $manx(paths) $d*]]==-1} {
		set d [file dirname $d]
	}

	# should manDescAdd directories be first or last?
	if {$p==-1} {set p $l}
	incr pri [expr $l-$p]
	set pri [expr $pri<<7]

	# tie breaker #2: section number order
	set l [llength $mani(manList)]
	set p [lsearch -exact $mani(manList) $n]; if {$p==-1} {set p $l}
	incr pri [expr $l-$p]

	return $pri
}


# given name, number and extension of manual page,
# return a list of matches as full pathnames

proc manShowManSearch {name num ext} {
	global man manx

	set sed $man(sed)
	if [lsearch $manx(manList) $num]==-1 {
		set num ""
	} else {
		# want to be permissive here, finding all pages that match, including subsections,
		# so don't find next volume name--^$num is what we want
		append sed " -e {/^$num /,/^\[^$num\] /!d}"
	}
	append sed " -e {/^. /d}"
DEBUG {puts "sed = $sed"}


	# search for man page
	set foundList ""
# sect handled with sed
#	set sect [if {$num!=""} {format "\.$num"} {format ""}]

	# get all potential matches, then filter against active MANPATH components
	# no $num in setting pat below as $num used in sed filter
	# (need optional piece at end for normalized SGI names)
#	set pat "/$name\\(\\..$ext\[^/\]*\\)\\{0,1\\}\$"
	set pat "/${name}(\\..$ext\[^/\]*)?\$"
#	set pat "/${name}\$"
DEBUG {puts stderr "searching for $pat"}
DEBUG {puts stderr "set fid \[open \"|$man(zcat) $manx(database) | $sed | $man(egrep) -i $pat\"\]"}
	set fid [open "|$man(zcat) $manx(database) | $sed | $man(egrep) -i {$pat}"]
	while {[gets $fid f]!=-1} {
#		gets $fid f; if {$f==""} continue
		set d [file dirname [file dirname $f]]
#puts -nonewline stderr "considering $f"
		if {(![info exists man($d)] || $man($d)) && [lsearch $foundList $f]==-1 } {
			lappend foundList $f
		}
	}
	catch {close $fid}

	return $foundList
}



#--------------------------------------------------
#
# manShowManFound -- display man page and update gui with parse info
#
# invariants:
#	 manx(catfull$w)!="" iff loaded from .../cat. directory
#--------------------------------------------------

proc manShowManFound {f {keep 0} {w .man}} {
	global man manx stat

	set t $manx(out$w).show; set wi $w.info
	# usually get name from database which doesn't have .z's, but just in case (and for transition time)
	set f [zapZ [string trim $f]]

	# update dups arrow
	set flen [llength $f]
	if {$flen>1} {
		# multiple matches
		pack $w.dups -before $w.mantypein -side left -anchor e; $w.dups configure -state active
		set m $w.dups.m
		$m delete 0 last
		foreach i $f {$m add command -label $i -command "incr stat(man-dups); manShowManFound $i 1 $w"}
		manMenuFit $m

		set f [lfirst $f]
	} elseif {!$keep} {pack forget $w.dups}

	manNewMode $w man; incr stat(man)


	# only redirect text of man pages
	if {[winfo exists $manx(out$w)]} {set w $manx(out$w)} {set manx(out$w) $w}
	# passed filename may or may not have (i.e., need) compression suffix
#	if {[set fg [lfirst [glob -nocomplain $f*$manx(zoptglob)]]]!=""} {
	if {[set fg [lfirst [glob -nocomplain $f$manx(zoptglob)]]]!=""} {
		set f $fg
	} else { manWinstderr $w "$f doesn't exist"; return }
	if [file isdirectory $f] {
		manWinstderr $w "$f is a directory"
		return
	}
	set f0 $f

	# for relative names w/. and ..
	set tmpdir [file dirname $f]
#	set f [stringesc $f $manx(regexpmetachars)]
#puts "$f => $f"

	# on first line, check for single-line .so file pointing to a compressed destination
	# on second line, check for "  Purpose" as sign of IBM AIX manual page
	set so 0
#	catch {
	set line1 [set line2 ""]
	if {[file readable $f]} {
		set fid [open "|[manManPipe $f]"]
		while {([string trim $line1]=="" || [regexp {^[.']\\"} $line1]) && ![eof $fid]} {gets $fid line1}
		while {[string trim $line2]=="" && ![eof $fid]} {gets $fid line2}
		catch {close $fid}
#puts stderr "***$line1***\n***$line2***"
		# don't be fooled by simple inclusion of macro file
		if {[regexp {^\.so (man.+/.+)} $line1 all newman]} {
DEBUG {puts stderr "*** single-line .so => $manx(manfull$w): $line1"}
			# glob here as redirected file may be compressed
			# (catch in case destination of .so doesn't exist)
			if [catch {set f [lfirst [glob [file dirname $tmpdir]/$newman*$manx(zoptglob)]]}] return
			set tmpdir [file dirname $f]
			set so 1
DEBUG {puts stderr "*** new f => $f"}
		}
	}
#	}

	# set variables
	set manx(manfull$w) $f
	set manx(man$w) [zapZ [file tail $f]]
	set manx(name$w) [string trimright [file rootname $manx(man$w)] "\\"]

	set fdir [zapZ [file dirname $manx(manfull$w)]]
	set topdir [file dirname $fdir]
	# strip trailing Z's
	set manx(num$w) [string range [file tail $fdir] $manx(dirnameindex) end]
	if {[lsearch $man(manList) $manx(num$w)]==-1} {set manx(num$w) [string index $manx(num$w) 0]}
#puts "manx(num\$w) = $manx(num$w)"
#	set manx(num$w) [string range [file extension $manx(man$w)] 1 1]
#	if {[file extension $manx(man$w)]==".man"} {set manx(num$w) [string range [file dirname $f] 1 1]}

	set fsstnd ""
	# if cat-only, then set manfull is cat already
	if {[regexp $man(catsig) $fdir]} {
#[string match */cat?*/* $f]} {
# $tmpdir] || [string match */cat?* [file dirname $tmpdir]]} {
		set manx(catfull$w) $manx(manfull$w)

		if {$line2=="  Purpose"} {
			manShowText $f $w 1
			set manx(typein$w) [set manx(name$w) [file rootname [set manx(man$w) [file tail $f]]]]
			return
		}
	} else {
		# /usr/<blah/>man/manN/<name> => /var/catman/<blah>/catN/<name>
DEBUG {puts "regexp on $topdir"}
		if {[regexp {^/usr/(.*)man$} $topdir all blah]} {
			set fsstnd "/var/catman/${blah}cat$manx(num$w)/$manx(man$w)$manx(zoptglob)"
DEBUG {puts "*** fsstnd = $fsstnd"}
		}
		set manx(catfull$w) "$topdir/cat$manx(num$w)*/$manx(man$w)$manx(zoptglob)"
DEBUG {puts "manx(man\$w) = $manx(man$w), catfull = $manx(catfull$w)"}
	}
	set manx(cat) "$topdir/cat$manx(num$w)"


    # for case of source a link to nowhere but formatted version OK (yeesh)
	if [catch {set manfullmtime [file mtime $manx(manfull$w)]}] {set manfullmtime 0}

	# check for already-formatted man page that's up to date vis-a-vis source code
	if {[set path [lfirst [glob -nocomplain $manx(catfull$w)]]]!=""
		&& [file readable $path] && [file mtime $path]>=$manfullmtime} {
		set manx(catfull$w) $path
		set pipe [manManPipe $path]

	# check for cat version in /var/catman... according to Linux FSSTND
	} elseif {$fsstnd!="" && [set path [lfirst [glob -nocomplain $fsstnd]]]!="" &&
		[file readable $path] && [file mtime $path]>=$manfullmtime} {
		set manx(catfull$w) $path
		set manx(cat) [file dirname $path]
		set pipe [manManPipe $path]

	# need to format from roff source
	} elseif {[file exists $manx(manfull$w)]} {
		# cd into top of man hierarchy in case get .so's
		if {[string match */man?* $tmpdir]} {
			set topdir [file dirname $tmpdir]
		} else {set topdir $tmpdir}
		if [catch {cd $topdir}] {
			manWinstderr $w "Can't cd into $topdir.  This is bad."
			return
		}
#puts stdout "\n\n\tcurrend dir [pwd]"
#puts stdout "manShowManFound\n\tcatfull = $manx(cat), manfull = $manx(manfull$w)"
		# try to save a nroff-formatted version?
		# check for H-P save directory
		if {[string match "*compress" $man(nroffsave)] && [file writable "$manx(cat).Z"]} {
			append manx(cat) ".Z"
		}
		if {[string match "on*" $man(nroffsave)]} {
			set saveerr ""

			# if .../catN unavailable, check Linux /var/catman alternative
			if {![file writable $manx(cat)] && [file writable "/var/catman"] && $fsstnd!=""} {
				set manx(cat) [file dirname $fsstnd]
			}

			# may need to create intermediate directories
			set idir ""
#puts "manx(cat) = $manx(cat)"
			foreach dir [split [string range $manx(cat) 1 end] "/"] {
#puts "idir = $idir, dir = $dir"
				if {![file exists $idir/$dir]} {
DEBUG {puts "\tmaking $idir/$dir"}
#					if {![file writable $idir]} {
#						manWinstderr $w "$idir not writable when trying to mkdir $dir"
#						break
#					}
					if [catch "exec $man(mkdir) $idir/$dir" info] {
DEBUG {puts "\t  ERROR: $info"}
#						manWinstderr $w $info 1
						set saveerr $info
						break
					} else {
						# permissions: if dir=.../catN and exists .../manN, make same as .../manN
						# otherwise make same a parent directory (assume never have to create /)
						catch {
							if {"$idir/$dir"!=$manx(cat) || ![string match cat* $dir] ||
							![file isdirectory [set permme "$idir/man[string range $dir 3 end]"]]} {
								set permme $idir
							}
							file stat $permme dirstat
							set perm [format "%o" [expr $dirstat(mode)&0777]]
DEBUG { puts "\tsetting permission of directory $idir/$dir to $perm, from $permme" }
							eval exec $man(chmod) $perm "$idir/$dir"
						}
					}
				}
				append idir "/$dir"
			}

			if {$saveerr!=""} {
				# nothing
			} elseif {![file writable $manx(cat)]} {
				set saveerr "CAN'T SAVE: $manx(cat) not writable"
			} else {
				# same name as source file, different directory
				set path [set manx(catfull$w) $manx(cat)/$manx(man$w)]

				manWinstdout $w "Saving copy formatted by nroff ..." 1
				if [catch {eval "exec [manManPipe $manx(manfull$w)] | $man(format) > $path"} info] {
					set saveerr "CAN'T SAVE: $info"
				} else {
					catch {
						file stat [file dirname $manx(manfull$w)] dirstat
						set perm [format "%o" [expr $dirstat(mode)&0666]]
DEBUG { puts "\tsetting permission of $path to $perm, from $manx(manfull$w)" }
						eval exec $man(chmod) $perm $path
 					}

					# if successfully saved formatted version, check on compressing it
					if {[string match "*compress" $man(nroffsave)]} {
						manWinstdout $w "Compressing ..." 1
						if [catch "exec $man(compress) $manx(catfull$w)" info] {
							set saveerr "CAN'T COMPRESS:  $info"
							# set path -- keep same path
							# return -- ok to fall through
						} elseif {[file extension $manx(cat)]==".Z"} {	# H-P
							eval exec $man(mv) [glob $manx(catfull$w).$manx(zglob)] $manx(catfull$w)
							set path $manx(catfull$w)
						} else {
							set path [set manx(catfull$w) [lfirst [glob $manx(catfull$w).$manx(zglob)]]]
						}
					}
					set pipe [manManPipe $path]
				}
			}

			# if problem making nroff version, fall back
			if {$saveerr!=""} {
				after 20 manWinstdout $w "{$saveerr}"
				set path $manx(manfull$w)
				set manx(catfull$w) ""
				set pipe "[manManPipe $path] | $man(format)"
			}

		# don't save formatted version
		} else {
			set path $manx(manfull$w)
			set manx(catfull$w) ""
			set pipe "[manManPipe $path] | $man(format)"
		}

	} elseif [catch {[file readlink $manx(manfull$w)]}] {
		manWinstderr $w "$manx(manfull$w) is a symbolic link that points nowhere"
		return
	} else {
		manWinstderr $w "$manx(manfull$w) not found"
		return
	}


	# got full file name, now show it
	set errflag 0
	set msg [expr [string match "*/*roff*" $pipe]?"Formatting and filtering":"Filtering"]
	manWinstdout $w "$msg $manx(name$w) ..." 1


	append pipe " | $manx(rman) -f TkMan $man(subsect) $man(headfoot) $man(changeleft) $man(zaphy)"
# $man(tables)
DEBUG {puts stderr "pipe = $pipe"}

	# source should take a pipe argument: "source |$pipe"
	if [catch {set fid [open "|$pipe"]} info] {
#		manWinstderr $w "Deep weirdness with $path.  Tell your sys admin."
		manWinstderr $w "ERROR: $info"
DEBUG {puts "can't open pipe: $info"}
		return
	}

	manTextOpen $w
	# want line at a time so can see first n lines quickly
	set time [time {
# plan A -- significately slower than plan B
#	while {[gets $fid line]!=-1} {eval $line}
# plan B -- shows first page, but slower than plan C
	while {![eof $fid]} {eval [gets $fid]}
# plan C - doesn't show first page quickly
#	eval [read $fid]
# plan D - quicker, but Perl 4 page uses 200K more memory! => so decided not quick enough!
#	set ctr 0
#	while {![eof $fid] && [incr ctr]<100} {eval [gets $fid]}
	# ... gobble rest as fast as possible
#	if {![eof $fid]} {eval [read $fid]}
	}]
DEBUG {puts "formatting time: $time"}
	if {$man(headfoot)!=""} {
		if {$manx(catfull$w)!=""} {
			set f $manx(catfull$w); set info "formatted"
		} else {set f $manx(manfull$w); set info "installed"}

		set fileinfo [clock format [file mtime $f] -format $man(date-format)]
		$t insert end "( $f $info at $fileinfo )\n" sc
	}
	manTextClose $w
	manWinstdout $w ""

	if [catch {close $fid} info] {
#		manWinstderr $w "Man page not installed properly, probably."
		manWinstderr $w "ERROR: $info"
DEBUG {puts "can't close pipe: $info"}
		return
	}

	# if followed a .so link, restore initial man page name for history and shortcuts, but not highlights
	# (is this the right policy decision?)
	if {$so} {
		set manx(manfull$w) $f0
		set manx(man$w) [zapZ [file tail $f0]]
		set manx(name$w) [file rootname $manx(man$w)]
#puts stderr "new status: $manx(manfull$w), $manx(man$w), $manx(name$w)"
	}

	manShowManFoundSetText $w $manx(out$w).show [bolg [zapZ $manx(manfull$w)] ~]
	# whatis information -- find by scanning text
	manWinstdout $w ""
	if {$man(whatwhere)} {
		manWinstdout $w $manx(manfull$w)
	} else {
		scan [$t index end] %d n
		set n [max $n 20]
		for {set i 1} {$i<=$n} {incr i} {
			if {[regexp -- {- (.*)} [$t get $i.0 "$i.0 lineend"] all info]} {
				incr i
				while {[regexp { +(.*)} [$t get $i.0 "$i.0 lineend"] all spaces more]} {
					append info " $more"
					incr i
				}
				manWinstdout $w "$info"
				break
			}
		}
	}


	cd $tmpdir
	manShowManStatus $w
	wm title $w "$manx(title$w): $manx(man$w)"
	set manx(lastman) $manx(manfull$w)

	# for remote calls(?)
	return $manx(manfull$w)
}


# man page-only (including help, a special one)
proc manShowManFoundSetText {w t key} {
	global man manx

	### after reading in text, build sections menu, count lines, show highlights
	# make sections menu
	set m [set mb $w.sections].m
	set ml {}; foreach i [$t mark names] { if [string match "js*" $i] { lappend ml $i } }
	set ml [lsort -command "bytextposn $t " $ml]
#[$t mark names]]
	set ctr 0
	foreach i $ml {
#		if ![string match "js*" $i] continue
		set pfx [expr {[string match "jss*" $i]?"   ":""}]
		set txt [$t get "$i linestart" "$i lineend"]
		if {![regexp {[a-z]} $txt] && [string length $txt]>3} { set txt [string tolower $txt] }

		set nextsect [lindex $ml [expr [lsearch -exact $ml $i]+1]]; if {$nextsect==""} {set nextsect end}
		set opts {}
		set optposn $i
		set expr "^\\|*\[ \t\]+(-.*)"; set expr2 "^\\|*\[ \t\]+(\[^-\].*)"
#puts "optposn = $optposn+1l, nextsect = $nextsect"
		while {[set optposn [$t search -forwards -regexp $expr $optposn+1l $nextsect]]!=""} {
			set opttxt [$t get $optposn "$optposn lineend"]
			regexp $expr $opttxt all opttxt
			for {set more 1} {$more<=3 && [$t compare [set optposn2 $optposn+${more}l] < $nextsect]} {incr more} {
				set opttxt2 [$t get $optposn2 "$optposn2 lineend"]
				if [regexp $expr2 $opttxt2 all opttxt2] { append opttxt " " $opttxt2 } else break
			}
			lappend opts [list $optposn [manHighNormalize $opttxt $man(high,hcontext)]]
#puts "got opt => |$opttxt| @ $optposn"
		}
#		puts "opts = |$opts|"

		if {[llength $opts]<=2 || !$man(optmenus)} {
			$m add command -label "$pfx$txt" -command "incr stat(page-section); $t yview $i"
		} else {
			$m add cascade -label "$pfx$txt" -menu [set m2 $m.opts$ctr]; incr ctr
			menu $m2 -tearoff no
			$m2 add command -label "(top of section)" -command "incr stat(page-section); $t yview $i"
			foreach j $opts {
				set optposn [lfirst $j]; set opttxt [lsecond $j]
				$m2 add command -label $opttxt -command "incr stat(page-section); $t yview $optposn"
			}
			manMenuFit $m2
		}
	}
	if {![string match "_*" $key] && $man(headfoot)!=""} {
		$m add separator
		$m add command -label "Header and Footer" -command "incr stat(page-section); $t yview headfoot"
	}
	configurestate $mb
# [llength $ml]
	manMenuFit $w.sections.m


	# hyperlinks -- collect from manref tag sets by RosettaMan
	set refs [$t tag ranges manref]
	set reflist {}
	set m [set mb $w.links].m
	for {set i 0} \$i<[llength $refs] {incr i 2} {
		lappend reflist [$t get [lindex $refs $i] [lindex $refs [expr $i+1]]]
	}
	foreach ref [uniqlist [lsort $reflist]] {
		$m add command -label $ref -command "incr stat(man-link); manShowMan $ref {} $w"
	}

	configurestate $mb
# [llength $reflist]
	manMenuFit $m


#	if {$manx(catfull$w)!=""} {set manx(hv$w) $manx(catfull$w)} {set manx(hv$w) $manx(manfull$w)}
	# always key highlights to name of source
	set manx(hv$w) $key

	manHighView $w

	focus $t
}


proc manManPipe {f} {
	global man manx

	if {[regexp $manx(zregexp) $f] || [regexp $manx(zregexp) [file dirname $f]]} {
		set pipe $man(zcat)
	} else {set pipe "cat"}

	return "$pipe $f"
}

# (proper) man page and text file finishings
proc manShowManStatus {w} {
	global man manx

	set t $manx(out$w).show; set wi $w.info

	### update status information

	# show section
	manSetSect $w $manx(manfull$w)

	# typein field
	set manx(typein$w) $manx(name$w)
	$w.mantypein icursor end

	# history - save entire path, including any compression suffix
	set manx(history$w) \
		[lrange [setinsert $manx(history$w) 0 [list $manx(manfull$w)]] 0 [expr $man(maxhistory)-1]]
	# should do this as a postcommand, but that's too messy
	set m [set mb $w.history].m
	$m delete 0 last
	foreach i $manx(history$w) {
		if {[llength [lfirst $i]]>=2} {set l [lfirst $i]} {set l $i}
		if {![regexp {^[|<]} $l]} {
			set l [zapZ [file tail $l]]
		}
		$m add command -label $l -command "incr stat(man-history); manShowMan $i {} $w"
	}
	configurestate $mb
# [llength $manx(history$w)]
	manMenuFit $m


	# shortcuts
	manShortcutsStatus $w


	# line count
	scan [$t index end] "%d" linecnt 
	$w.search.cnt configure -text "$linecnt lines"
}


proc zapZ! {f} {
	uplevel 1 set $f \[zapZ \[set $f\]\]
}
proc zapZ {f} {
	global manx
	return [expr {[regexp $manx(zregexp) $f]?[file rootname $f]:$f}]
}


#--------------------------------------------------
#
# manShowText -- use searching, etc. facilities for non-man page text
#	 to use via `send', pass full path name of text file to show
#
#--------------------------------------------------

proc manShowText {f0 {w .man} {keep 0}} {
	global man manx stat

	set wi $w.info

# maybe want to use `file' to check for ASCII files
#	if {![file readable $f] || [file isdirectory $f]} {
#		manWinstdout $w "Can't read $f"
#		return
#	}

	set t $manx(out$w).show

	if {[string match <* $f0]} {
		set f [fileexp [string range $f0 1 end]]
		if {[regexp $manx(zregexp) $f]} {set f "|$man(zcat) $f"}
# {set f "|cat $f"}
	} else {set f [pipeexp $f0]}

	# strain out control characters and show nroff files
#	append f " | rman"

	manNewMode $w txt; incr stat(txt)

	manTextOpen $w
	if {[catch {
		set fid [open $f]
#		while {![eof $fid]} {eval [gets $fid]}
		while {![eof $fid]} {$t insert end [gets $fid]\n}
		close $fid
	}]} {manTextClose $w; manWinstderr $w "Trouble reading $f0"; return}
	manTextClose $w
	if {!$keep} {pack forget $w.dups}

	if [file isfile $f] {cd [file dirname [glob $f]]}


	### update status information

	# could always save full path here, but that occupies too much screen real estate
	set manx(man$w) $f0
	set manx(num$w) X
	set manx(hv$w) [bolg [zapZ $f] ~]
	set manx(manfull$w) $f0
	set manx(catfull$w) $f
	set manx(name$w) $f0
	set tmp $man(whatwhere)
	set man(whatwhere) 1
	manShowManStatus $w
	set man(whatwhere) $tmp

	manWinstdout $w $manx(manfull$w)
	manHighView $w
	focus $t

	return $f
}



#--------------------------------------------------
#
# manApropos -- show `apropos' (`man -k') information, w/dups filtered out
#
#--------------------------------------------------

proc manApropos {name {w .man}} {
	global man manx mani stat

	if $manx(shift) { set manx(shift) 0; set w [manInstantiate] }
	set wi $w.info; set t $w.show

	if {$name==""} {set name $manx(man$w)}
	if {$name==""} {
		manWinstderr $w "Type in keywords to search for"
		return
	}

	set mani(apropos,form) "Apropos search for \"$name\"\n\n"
	manWinstdout $w "Apropos search for \"$name\" ..." 1

DEBUG {puts "manApropos: exec $man(apropos) $name $man(aproposfilter) 2>/dev/null"}
	if {[catch {set tmp [eval exec "$man(apropos) $name $man(aproposfilter) 2>/dev/null"]} info] || $tmp==""} {
		if {$info!=""} {
			append mani(apropos,form)
		} else {
			append mani(apropos,form) "Nothing related found.\n"
		}
		if {$man(glimpse)!=""} { append mani(apropos,form) "Try a full text search with Glimpse.\n" }
		set mani(apropos,cnt) 0
	} else {
		manNewMode $w aprops; incr stat(apropos)
		set mani(apropos,update) [clock seconds]
		set cnt 0
		foreach line [split $tmp "\n"] {
			# zap formatting codes if erroneously given roff source
			regsub "^\\.\[^ \t\]+" $line "" line
			# normalize tabs
			regsub -all "\[\t \]+" $line " " line
			# normalize spacing between names and descriptions to single tab
			regsub " - " $line "\t - " line
			# Solaris gives name of file, name of function within page
			if {[lfirst $line]==[lsecond $line]} { regsub {^[ ]*[^ ]+[ ]*} $line "" line }

			append mani(apropos,form) $line "\n"
			incr cnt
		}

		set mani(apropos,cnt) $cnt
		set manx(yview,apropos) 0
	}

	manShowSection $w apropos
}



#--------------------------------------------------
#
# manGlimpse -- full text search
#
#--------------------------------------------------

proc manGlimpse {name {w .man}} {
	global man manx

	# index over currently selected paths, and (always) stray cats
	set dirs $man(glimpsestrays)

	set auxopts ""
	if {$man(indexglimpse)=="distributed"} {
		foreach d $manx(paths) {
			if {$man($d)} {	lappend dirs $d }
		}
	} else {
# can't restrict unified glimpse searches to directories in Paths
# because list of directories can exceed a buffer in agrep
#		set first "-F "
#		foreach d $manx(paths) {
#			append auxopts $first [stringregexpesc $d]
#			set first ","
#		}
	}

	manGlimpse2 $name $dirs $auxopts $w
}

proc manGlimpse2 {name dirs {auxopts ""} {w .man}} {
	global man manx mani sbx env stat STOP

	if $manx(shift) {set manx(shift) 0; set w [manInstantiate]}
	set t $manx(out$w).show; set wi $w.info

	# set name to search for and name to show
	if {$name==""} {set name $manx(man$w)}
	if {$name==""} {
		manWinstderr $w "Type in regular expression for full text search"
		return
	}
	set showname $name
	# escape out semicolons meant for Glimpse so Tcl doesn't parse them as commands separators
	regsub -all {([^\]);} $name {\1\;} name

	# set options
	# X DO NOT put -z option here.  unneeded and causes errors when pages are compressed
	# => au contraire, DO put -z in as Glimpse interrogates compressed files to verify matches
	# => tackle this in the Makefile
	#set opts "-lyz"
	set opts "-ly"
	set casesen $man(regexp,case); if {$casesen==-1 && [string tolower $name]!=$name} {set casesen 1}
	if {!$casesen} {append opts "i"}
	if {$man(maxglimpse)!="none"} {append opts " -L $man(maxglimpse)"}
	set redirect ""
	# HACK: -N spits out blocks matches to stderr, which is an error to Tcl
	if [regexp { -[^ ]*N[^ ]* } "$man(glimpse) $name"] {append redirect "2>/dev/null"}

	set STOP 0
	cursorBusy

	set foundList ""
	set errorList ""
	# would be considerably(?) more efficient if Glimpse could handle a list of directories
	# to search for matches, rather than multiple exec's
	foreach d $dirs {
		if ![file readable $d/.glimpse_index] {
# this reported at statup
#			append errorList "$d: no Glimpse index files\n"
			continue
		}
		manWinstdout $w "Glimpsing for \"$showname\" in $d ..."; update
DEBUG {puts "$man(glimpse) $auxopts $opts -H $d $name"}
		if {$STOP} {set STOP 0; break}
		# silently skip over problems (like no .glimpse_* files)
		if ![catch {set matches [eval exec -keepnewline "$man(glimpse) $auxopts $opts -H $d {$name} $redirect"]} info] {
			 append foundList $matches
		} else {
			append errorList "can't glimpse $d: $info\n"
			manWinstderr $w "error with glimpsing: $info"
		}
	}
	manWinstdout $w ""
	cursorUnset

	set found [llength $foundList]; set error [string length $errorList]
	if {!$found && !$error} {
		manWinstderr $w "$name not found in full text search"
	} else {
		manNewMode $w super; incr stat(super)
		set mani(glimpse,form) "Glimpse full text search for \"$name\"\n\n"
		set mani(glimpse,update) [clock seconds]

		if $error {
			append mani(glimpse,form) "Errors while Glimpsing\n\n" $errorList "\n\n"
		}

		foreach i [lsort -command manSortByName $foundList] {
			append mani(glimpse,form) "$i\n"
		}
		set mani(glimpse,cnt) $found
		set manx(yview,glimpse) 0

		# seed regexp and isearch strings
#		set manx(search,string$w) [tr [tr [llast $name] ";" ".*"] "," "|"]
		set manx(search,string$w) [tr [tr [llast $name] ";" "|"] "," "|"]
		set sbx(lastkeys-old$t) [llast $name]

		manShowSection $w glimpse
	}
}



proc manGlimpseIndex {{w .man}} {
	global man manx mani

	# index over all paths, whether currently on or not

	# pairs of (dest-dir-of-index list-of-dirs-to-index)
	if {$man(indexglimpse)=="distributed"} {
		set dirpairs {}
		if [llength $mani($man(glimpsestrays),dirs)] {
			lappend dirpairs [list $man(glimpsestrays) $mani($man(glimpsestrays),dirs)]
		}
		foreach dir $manx(paths) {
			lappend dirpairs [list $dir $mani($dir,dirs)]
		}
	} else {
		set dirs $mani($man(glimpsestrays),dirs)
		foreach dir $manx(paths) {
			set dirs [concat $dirs $mani($dir,dirs)]
		}
		set dirpairs [list [list $man(glimpsestrays) $dirs]]
	}

	set buildsec [time {manGlimpseIndex2 $dirpairs $w}]

	if {$buildsec<[expr 60*60]} {set buildfmt "%M:%S"} {set buildfmt "%T"}
	if {$buildsec>30 || $man(time-lastglimpse)==-1} {
		set man(time-lastglimpse) [clock format $buildsec $buildfmt]
	}

	foreach win [lmatches ".man*" [winfo children .]] {
		$w.occ.m entryconfigure "Rebuild Glimpse*" -label "Rebuild Glimpse Index (last $man(time-lastglimpse))"
	}

	# now update Glimpse warnings -- done at every Help
	#manManpathCheck
}


proc manGlimpseIndex2 {dirpairs {w .man}} {
	global man manx mani env

	manNewMode $w super
	set t $manx(out$w).show; set wi $w.info

	manWinstdout $w "Rebuilding Glimpse database ... "
	manShowSection $w glimpse
	manTextOpen $w; update idletasks
	set cnt [llength $dirpairs]; set cur 1
	set foneup 0

	foreach pair $dirpairs {
		set dir [lfirst $pair]; set dirs [lsecond $pair]
		$t insert end "Working on $dir" b " ($cur of $cnt), "
		set dircnt [llength $dirs]; set dirtxt [expr $dircnt==1?"directory":"directories"]
		$t insert end "$dircnt $dirtxt"
		$t insert end "\n\n"
		$t see end; update idletasks

		if {!$dircnt} {
			$t insert end "Nothing to index.\n"
			incr cur; $t insert end "\n\n"
			continue
		}

		set gzt ".glimpse_filters"
		set gf "$dir/.glimpse_filenames"
		set gz "$dir/$gzt"
		set gfe [expr [llength [glob -nocomplain $dir/.glimpse_{filenames,index}]]==2]


		# see if index is out of date
		set outofdate [expr {!$gfe || [file size $gf]==0 || ([file exists $gz] && [file mtime $gz]>[file mtime $gf]) || [file mtime $gf]<$manx($dir,latest)}]

		if {!$outofdate} {
			$t insert end "Glimpse index still current.\n" indent

			set foneup 1
			# could use perl-style continue expression here
			incr cur; $t insert end "\n\n"
			continue
		}


		# directory writable?
		if {![file writable $dir]} {
			$t insert end "Glimpse index out of date but directory not writable" indent
			if {$gfe} {
				$t insert end " ... but old Glimpse files found\n" indent
				$t insert end "Full text seaching available here using existing files.\n" indent
			} else {
				$t insert end " ... and Glimpse files not found\n" indent
				$t insert end "No full text searching available here.\n" {indent bi}
			}

			incr cur; $t insert end "\n\n"
			continue
		}


		set gex "$dir/.glimpse_exclude"
		if ![file exists $gex] {
			set fid [open $gex "w"]
			puts $fid {.glimpse_exclude$}
			puts $fid {~$}
			puts $fid {.bak$}
			close $fid
		}

		# see if .glimpse_filters file needed, and if so make one
		# (but don't overwrite any existing .glimpse_filters)
		if ![file exists $gz] {
			set fcat [expr [lsearch -regexp $dirs {/cat[^/]*$}]!=-1]
#|/catman/
			set fhp [expr [lsearch -regexp $dirs {\.Z$}]!=-1]
			set fz 0
			foreach d $dirs {
				# cd into directory so get the short file names (important for /usr/man!)
				cd $d
				if {[lsearch -regexp [glob -nocomplain *] $manx(zregexp)]!=-1} {set fz 1; break}
			}

#			$t insert end "* * * create $gzt file here: fcat=$fcat, fhp=$fhp, fz=$fz\n"
			set fid [open $gz "w"]
			# # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
			# create file according to man(compress) and manx(zglob)  #
			# # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
			if $fhp {puts $fid "*.Z/*\tzcat <"}
			if {$fz} {
				set zcat [file tail [lfirst $man(zcat)]]
				switch -glob -- $zcat {
					gz* {puts $fid "*.z\t$man(zcat)\n*.Z\t$man(zcat)\n*.gz\t$man(zcat)"}
# would like to do this
#					gz* {puts $fid "*.{z,Z,gz}\t$man(zcat)"}
					default {
						# works for zcat, pcat and (one hopes) anything else to come
						# (string trimright because of HP "zcat < ")
						puts $fid "*.$manx(zglob)\t[string trimright $man(zcat) { <}]"
					}
				}
			}
			# strip AFTER decompression
			if $fcat {puts $fid "*/cat*/*\trman <"}
			close $fid
		}


		### try to index or re-index directory
		if [catch {set fid [open "|$man(glimpseindex) -z -H $dir $dirs"]} info] {
			# other problems ... like what?
DEBUG {puts "error on: $man(glimpseindex) -z -H $dir $dirs]: $info"}
			$t insert end "$info\n" bi
			catch {close $fid}
		} else {
DEBUG {puts "$man(glimpseindex) -z -H $dir $dirs"}
			set blankok 0
			while {![eof $fid]} {
				gets $fid line
				if {![regexp {(^This is)} $line] && ($line!="" || $blankok)} {
					$t insert end "$line\n" tt; $t see end; update idletasks
					set blankok 1
				}
			}
			if [catch {close $fid} info] { $t insert end "ERRORS\n" {indent bi} $info indent2 "\n" }

			if {[file size $gf]==0} {
				$t insert end "No files could be indexed.  No full text searching available here.\n" {indent bi}
				if [file exists $gz] {
					$t insert end "Try checking your $gzt file in $dir.  If $gzt wasn't created by TkMan, try deleting it and letting TkMan create one of its own.\n" indent
				}
			} else {
				# give glimpse files same permissions as directory
				catch {
					file stat $dir dirstat
					set perm [format "%o" [expr $dirstat(mode)&0666]]
					eval exec $man(chmod) $perm [glob $dir/.glimpse_*]
				}
			}
		}

		incr cur
		$t insert end "\n\n"
	}

	if {$foneup} {
		$t insert end "\nTo force re-indexing of directories that TkMan claims are current, remove all Glimpse index files in that directory, as with `rm <dir> .glimpse_*'.\n" i
	}

	$t see end
	manTextClose $w

	set mani(glimpse,form) [$t get 1.0 end]

	manWinstdout $w ""
}


#--------------------------------------------------
#
# manPrint -- kill trees
#
#--------------------------------------------------

proc manPrint {w {printer ""}} {
	global man manx stat env

	set t $w.show; set wi $w.info
# can't get here unless man page
#	if {$manx(mode$w)!="man" && $manx(mode$w)!="txt"} {
#		manWinstderr $w "TkMan only prints man pages."
#		return
#	}

	set f [string trim $manx(manfull$w)]
	if {$f=="" || ![file exists $f]} return
	set tail [zapZ [file tail $f]]; set name [file rootname $tail]; set sect [file extension $tail]
	if {$sect==""} {set sect [string index [file tail [file dirname $f]] $manx(dirnameindex)]}

	set tmp [manWinstdout $w]
	manWinstdout $w "Printing $f ..." 1
	# need to cd in case of trying to print .so pages
	set curdir [pwd]
	set topdir [file dirname $f]
	set printpipe $man(print)
	if {[regexp -- $man(catsig) $topdir]} {
		set printpipe $man(catprint)
		if {[tk_dialog .dialog "NO GUARANTEES" "No troff source.  Try to reverse compile cat-only page?" \
				"" 1 No Yes]} {
			set printpipe "$manx(rman) -f roff $man(changeleft) -n $name -s $sect $man(subsect) | $man(print)"
# $man(tables)
		}
	}
	if {[regexp $man(catsig) $topdir] || [string match */man?* $topdir]} {
		set topdir [file dirname $topdir]
	}

	# try to move into $topdir, but don't complain if can't because it may not be necessary
	catch {cd $topdir}
DEBUG {puts stderr "print pipe = [manManPipe $f] | $printpipe"}

	set env(PRINTER) $printer; set env(LPDEST) $printer
	eval exec [manManPipe $f] | $printpipe
	manWinstdout $w $tmp
	cd $curdir
	incr stat(print)
}



#--------------------------------------------------
#
# manHelp -- dump help text that's "compiled" offline
#
#--------------------------------------------------

proc manHelp {w} {
	global man manx stat

	set t $w.show; set wi $w.info

	manNewMode $w help; incr stat(help)
	wm title $w "$manx(title$w) v$manx(version)"
	set manx(manfull$w) "_help"
	set manx(name$w) "_help"

	manTextOpen $w
	# put in help first so marks set correctly (they stay correct subsequently as they float)
	manHelpDump $t
	# fixups
	$t tag remove h1 1.0 "1.0 lineend"; $t tag add title 1.0 "1.0 lineend"
	$t delete 2.0
	$t delete 7.10 7.14; $t insert 7.10 "\251"; $t tag add symbol 7.10
	foreach bm {icon face} {
		$bm configure -foreground black -background white
		label $t.$bm -image $bm
	}
	$t window create 2.0 -window $t.icon -padx 8 -pady 8
	$t mark set insert "author1+2lines lineend"
	$t insert insert "\t"
	$t window create insert -window $t.face -align bottom -padx 8 -pady 8

	# now add to opening screen
	# update warnings
	manManpathCheck
	set fWarn 0
	foreach warn {manpathset manpath mandesc expire} {
		if {$manx($warn-warnings)!=""} {
			$t insert 1.0 $manx($warn-warnings)\n\n
			set fWarn 1
		}
	}
	if $fWarn {	$t insert 1.0 "Warnings\n\n" b }

	$t insert 1.0 $manx(updateinfo)

	manTextClose $w


	# translate SGML names to Tk marks
	set cnt 0
	foreach i [lsort -command "bytextposn $t " [$t mark names]] {
		if {[string match "*1" $i]} { $t mark set js$cnt $i; incr cnt }
		if {[string match "*2" $i]} { $t mark set jss$cnt $i; incr cnt }
	}

	manShowManFoundSetText $w $t "_help"


	# use update and after for good interaction with rebuilding database
	update idletasks
	after 1 "manWinstdout $w \"TkMan v$manx(version) of $manx(date)\""
}

proc bytextposn {t a b} {
	set ap [$t index $a]; set bp [$t index $b]
	if {$ap<$bp} {set cmp -1} elseif {$ap>$bp} {set cmp 1} else {set cmp 0}
	return $cmp
}


#--------------------------------------------------
#
# manKeyNav -- keyboard-based navigation and searching
#    calls out to searchbox module
#
#--------------------------------------------------

proc manKeyNav {w m k} {
	global man manx

	set t $w.show; set wi $w.info

	set firstmode [expr {$manx(mode$w)=="section" || $manx(mode$w)=="apropos"}]
	set casesen [expr $firstmode?1:$man(incr,case)]

	set fFound [searchboxKeyNav $m $k $casesen $t $wi $firstmode]
#	if {!$fFound && $firstmode} {set fFound [searchboxKeyNav $m $k 0 $t $wi $firstmode]}

	return $fFound
}



#--------------------------------------------------
#
# manPreferences -- graphical interface to many values stored in startup file
#
#--------------------------------------------------

proc manPreferencesMake {{w0 ""}} {
	global prefedit manx man bozo
# don't do global man here

	set w .prefs
	if {[winfo exists $w]||$w0==""} {return $w}

	toplevel $w
	wm geometry $w $prefedit(geom-prefs)
	wm title $w "Preferences"
	wm withdraw $w

	set f [frame $w.pages]
	foreach i {Fonts Colors See Icon Misc} {
		set il [string tolower $i]
		radiobutton $f.$il -text $i -command manPreferences -variable manx(prefscreen) -value $il
	}
	pack $f.fonts $f.colors $f.see $f.icon $f.misc -side left -fill x -padx 4

	frame $w.sep1 -height 2 -background $prefedit(guifg)


	### fonts
	set g [frame $w.[set group "fonts"]]

	# fonts: gui, file, errors -- family, size
	foreach i {{gui Interface} {text "Text display"} {vol "Volume listings"}} {
		set var [lfirst $i]; set txt [lsecond $i]
		lappend manx(vars-$group) $var-family $var-style $var-points
		set f [frame $g.font$var]
		label $f.l -text $txt
		[eval tk_optionMenu $f.family prefedit($var-family) $man(families)] configure -tearoff no
		[eval tk_optionMenu $f.style prefedit($var-style) $manx(styles)] configure -tearoff no
		[eval tk_optionMenu $f.points prefedit($var-points) $manx(sizes)] configure -tearoff no
		pack $f.l -side left -fill x
		pack $f.style $f.points $f.family -side right -padx 2
	}

#	if {[llength $man(dpi-values)]>1}
	# screen dpi: *, 75, 100
	set f [frame $g.dpi]
	label $f.l -text "Screen dots per inch (DPI)"
	[eval tk_optionMenu $f.dpi prefedit(dpi) $prefedit(dpis)] configure -tearoff no
	lappend manx(vars-$group) "dpi"
	pack $f.l -side left -fill x
	pack $f.dpi -side right

	pack $g.fontgui $g.fonttext $g.fontvol $g.dpi -fill x -pady 3 -padx 4


	### colors
	set g [frame $w.[set group "colors"]]

	foreach i {{text "Text"} {gui "Interface"} {butt "Buttons"} {active "Active Regions"}} {
		set var [lfirst $i]; set txt [lsecond $i]
		lappend manx(vars-$group) ${var}fg ${var}bg
		set f [frame $g.$var]
		label $f.l -text $txt
		foreach j {{fg "foreground"} {bg "background"}} {
			set jvar [lfirst $j]; set jtxt [lsecond $j]
			if {$jvar=="fg"} {set jopp "bg"} {set jopp "fg"}
			set mb $f.$var$jvar
			[set m [eval tk_optionMenu $mb prefedit($var$jvar) $man(colors)]] configure -tearoff no
			# when monochrome, keep foreground and background colors opposites!
			if {$manx(mono)} {
				foreach k $man(colors) {
					set copp [lindex $prefedit(colors) [expr 1-[lsearch $prefedit(colors) $k]]]
					$m entryconfigure $k -command "set prefedit($var$jopp) $copp; set prefedit($var$jvar) $k"
				}
			}
		}
		pack $f.l -side left -fill x
		pack $f.${var}bg [label $f.${var}on -text "on"] $f.${var}fg -side right -padx 4
	}

	foreach i {{manref f "Man page references"} {isearch b "Incremental search hits"}
				{search b "Regexp search hits"} {highlight b "Highlights"}} {
		set var [lfirst $i]; set fb [lsecond $i]; set txt [lthird $i]
		lappend manx(vars-$group) $var
		set f [frame $g.$var]
		label $f.l -text $txt

		[set m [eval tk_optionMenu [set mb $f.$var] prefedit($var) $manx(highs)]] configure -tearoff no
			foreach k $manx(highs) {
				if {[lsearch $man(colors) $k]==-1} {set val $k} \
				else {if {$fb=="f"} { set val "-foreground [list $k]"} { set val "-background [list $k]" } }
				$m entryconfigure $k -command "set prefedit($var) [list $val]"
			}
		pack $f.l -side left -fill x
		pack $mb -side right -padx 4
	}

	pack $g.text $g.gui $g.butt $g.active $g.manref $g.isearch $g.search $g.highlight \
		-fill x -expand yes -pady 3 -padx 4


	### See
	set g [frame $w.[set group "see"]]

	foreach i {{headfoot "Show header, footer, date at bottom"} {scrollbarside "Scrollbar side"}
				{whatwhere "Information bar shows"} {textboxmargin "Text box internal margin"}
				{volcol "Width of columns in Volumes list"}
				{high,vcontext "Back context for Highlights jump"}
				{strictmotif "Strict Motif behavior"} 
				{error-effect "Status line error/warning effect"}
				{optmenus "Sections submenus for command line options"}
				{subvols "Subvolumes as submenus in Volumes"} 
				{apropostab "Tab stop in apropos list"}} {
		set var [lfirst $i]; set txt [lsecond $i]
		lappend manx(vars-$group) $var
		set f [frame $g.$var]
		label $f.l -text $txt
		[set m [eval tk_optionMenu $f.high bozo($var) $manx(${var}-t)]] configure -tearoff no
			set j 0
			foreach mv $manx(${var}-v) {
				$m entryconfigure $j -command "set prefedit($var) [list $mv]"
				incr j
			}
			trace variable prefedit($var) w manPrefBozo
		pack $f.l -side left -fill x
		pack $f.high -side right
	}

	pack $g.headfoot $g.subvols $g.optmenus $g.high,vcontext $g.volcol $g.apropostab $g.whatwhere $g.scrollbarside $g.textboxmargin $g.error-effect $g.strictmotif \
		-fill x -pady 3 -padx 4



	### icon-related
	set g [frame $w.[set group "icon"]]

	foreach i {{iconify "Iconify on startup"} {updateicon "... or reflect current page in iconname"}} {
		set var [lfirst $i]; set txt [lsecond $i]
		lappend manx(vars-$group) $var
		set f [frame $g.$var]
		label $f.l -text $txt
		[set m [eval tk_optionMenu $f.high bozo($var) $manx(${var}-t)]] configure -tearoff no
			set j 0
			foreach mv $manx(${var}-v) {
				$m entryconfigure $j -command "set prefedit($var) [list $mv]"
				incr j
			}
			trace variable prefedit($var) w manPrefBozo
		pack $f.l -side left -fill x
		pack $f.high -side right
	}

	foreach i {{iconname "Name when iconified..."}
				{iconbitmap "Path name of icon bitmap"} {iconmask "Path name of icon mask"}
				{iconposition "Icon position (+|-)x(+|-)y"}} {
		set var [lfirst $i]; set txt [lsecond $i]
		lappend manx(vars-$group) $var
		set f [frame $g.$var]
		label $f.l -text $txt
		entry $f.e -textvariable prefedit($var) -relief sunken

		pack $f.l -side left -fill x
		pack $f.e -side right
	}
	set ring {iconname iconbitmap iconmask iconposition}; set ringl [llength $ring]
	for {set i 0} {$i<$ringl} {incr i} {
		set wig $g.[lindex $ring $i].e
		foreach k {Tab Return} { bind $wig <KeyPress-$k> "focus $g.[lindex $ring [expr ($i+1)%$ringl]].e"; break }
		bind $wig <Shift-KeyPress-Tab> "focus $g.[lindex $ring [expr ($i-1)%$ringl]].e; break"
	}

	# file completion for bitmap names
	foreach wig {iconbitmap iconposition} {
		set e $g.$wig.e
		bind $e <KeyPress-Escape> "manFilecompleteLocal $e"
	}

	pack $g.iconify $g.iconname $g.updateicon $g.iconbitmap $g.iconmask $g.iconposition \
		-fill x -pady 3 -padx 4


	### miscellaneous
	set g [frame $w.[set group "misc"]]

	foreach i {{nroffsave "Cache formatted (nroff'ed) pages"}
				{hyperclick "Mouse click to activate hyperlink"}
				{subsect "Parse man page subsections"}
#				{tables "Aggressive table parsing"}
				{recentdays {"Recent" volume age, in days}}
				{maxhistory "Maximum length of history list"}
				{indexglimpse "Glimpse indexing"}
				{maxglimpse "Maximum Glimpse hits (per man hierarchy)"}
				{zaphy "Prevent hyphenation (silly)"}} {
		set var [lfirst $i]; set txt [lsecond $i]
		lappend manx(vars-$group) $var
		set f [frame $g.$var]
		label $f.l -text $txt
		[set m [eval tk_optionMenu $f.high bozo($var) $manx(${var}-t)]] configure -tearoff no
			set j 0
			foreach mv $manx(${var}-v) {
				$m entryconfigure $j -command "set prefedit($var) [list $mv]"
				incr j
			}
			trace variable prefedit($var) w manPrefBozo
		pack $f.l -side left -fill x
		pack $f.high -side right
	}


	foreach i {{glimpsestrays "Distributed stray cats / unified index"}
				{printers "Printer list (space separated)"}} {
		set var [lfirst $i]; set txt [lsecond $i]

		set f [frame $g.$var]
		label $f.l -text $txt
		entry $f.e -width 25 -textvariable prefedit($var) -relief sunken

		pack $f.l -side left -fill x
		pack $f.e -side right
	}


	pack $g.nroffsave $g.printers $g.hyperclick $g.subsect $g.maxhistory $g.recentdays \
		$g.maxglimpse $g.indexglimpse $g.glimpsestrays \
#		$g.tables \
		$g.zaphy \
		-fill x -pady 3 -padx 4


	# buttons
	frame $w.bsep -relief sunken -height 2 -background $prefedit(guifg)
	set f [frame $w.butts]
	button $f.ok -text "OK" -padx 6 -command "grab release $w; wm withdraw $w; manPreferencesSet"
	button $f.apply -text "Apply" -command "manPreferencesSet"
	button $f.cancel -text "Cancel" -command "
		grab release $w; wm withdraw $w
		manPreferencesGet cancel; manPreferencesSet
	"
	button $f.default -text "Defaults" -command "manPreferencesGet default"
	pack $f.ok $f.apply $f.default $f.cancel -side right -padx 4


	pack $w.pages $w.sep1 $w.bsep $w.butts \
		-side top -fill x -pady 3 -padx 4

	return $w
}

proc manPrefBozo {array var op} {
	global prefedit manx bozo
	set bozo($var) [lindex $manx($var-t) [lsearch $manx($var-v) $prefedit($var)]]
}


proc manFilecompleteLocal {t} {
	set line [$t get]
	set fc [filecomplete $line]
	set ll [llength $fc]

	# no matches returns
	if {!$ll} {
		bell
		return
	}

	# otherwise show longest valid name (longest common prefix determined by filecomplete)
	$t delete 0 end
	$t insert 0 [lfirst $fc]
	$t icursor end
	$t xview moveto 1
}


proc manPreferencesGet {{cmd "fill"}} {
	global man manx default prefedit cancel curedit

	if {$cmd=="fill"} {
		foreach i [array names default] {set prefedit($i) [set curedit($i) [set cancel($i) $man($i)]]}
	} elseif {$cmd=="default"} {
		foreach i $manx(vars-$manx(prefscreen)) {set prefedit($i) $default($i)}
	} elseif {$cmd=="cancel"} {
		foreach i [array names default] {set prefedit($i) $cancel($i)}
	} elseif {$cmd=="man"} {
		foreach i [array names default] {set man($i) $prefedit($i)}
	} elseif {$cmd=="curedit"} {
		foreach i [array names default] {set curedit($i) $prefedit($i)}
	}
}

set manx(prefscreen) "fonts"
set manx(oldprefscreen) ""
proc manPreferences {{screen ""}} {
	global manx

	# show right grouping
	set w [manPreferencesMake bozo]
	raise $w
	if {$screen==""} {set screen $manx(prefscreen)} {set manx(prefscreen) $screen}
	set prev $manx(oldprefscreen)

	if {$screen!=$prev} {
		if {$prev!=""} {
			pack forget $w.$prev
#			$w.pages.$prev configure -relief raised
		}

		pack $w.$screen -after $w.sep1 -fill x
#		$w.pages.$screen configure -relief sunken

		set manx(oldprefscreen) $screen
	}

	if ![winfo ismapped $w] {
		manPreferencesGet fill
		wm deiconify $w; grab set $w
	}
}

proc manPresDefaultsSet {} {
	global man font

	# fonts & colors
	foreach i {{gui ""} {butt "Button."} {butt "Menubutton."} {butt "Radiobutton."} \
				{butt "Checkbutton."} {text "Text."}} {
# {gui "Scrollbar."}} {
# {txt "Entry."}
		set var [lfirst $i]; set txt [lsecond $i]
		if [info exists man(${var}-family)] {
			option add Tkman*${txt}Font \
				[spec2font $man(${var}-family) $man(${var}-style) $man(${var}-points)] 61
		}
		option add Tkman*${txt}Foreground $man(${var}fg) 61
		option add Tkman*${txt}Background $man(${var}bg) 61
#puts "option add ...$txt... $man(${var}fg) on $man(${var}bg)"
	}

	option add Tkman*activeForeground $man(activefg) 61
	option add Tkman*activeBackground $man(activebg) 61
	option add Tkman*highlightBackground $man(activebg) 61

	option add Tkman*selectColor $man(buttfg) 61

	set font(pro) [spec2font $man(text-family) $man(text-style) $man(text-points)]
	set font(mono) [spec2font "Courier" $man(text-style) $man(text-points) "s"]
	set font(vol) [spec2font $man(vol-family) $man(vol-style) $man(vol-points)]
	if {[string match "*Courier*" $man(currentfont)]} {
		set man(currentfont) $font(mono)
	} else {set man(currentfont) $font(pro)}
}


# reconfigure screen based on new preferences information
proc manPreferencesSet {} {
	global man manx mani prefedit curedit default

	set change 0
	foreach i [array names default] {
		if {$curedit($i)!=$prefedit($i)} {
			set change 1
			break
		}
	}
	if !$change return

	# random updates
	set mani($prefedit(glimpsestrays),dirs) $mani($man(glimpsestrays),dirs)

	# make temporary permanent
	manPreferencesGet man
#	manGeometry get -- done just before quit

	# # # set keybindings--later

	# fonts -- text, gui (main and preferences windows)
	manPresDefaultsSet
	resetfonts [spec2font $man(gui-family) $man(gui-style) $man(gui-points)]
	resetcolors

	# update text tags and other .man* bits
	foreach w [lmatches ".man*" [winfo children .]] {manPreferencesSetMain $w}

#	manGeometry set
	manPreferencesGet curedit
}


proc manPreferencesSetMain {w} {
	global man manx font curedit tk_strictMotif

	if ![string match ".man*" $w] return

	# icon-related
	# ... need to reference man() to get new values, but manx() has command line options
	foreach i {iconname iconposition iconbitmap iconmask} {
		if {$man($i)!=$curedit($i)} {set manx($i) $man($i)}
	}
	set num [string range $w 4 end]
	# have option to show current man page name instead? -- would need to constantly update this
	if {$num==""} {set name $manx(iconname)} {set name "$manx(iconname) #$num"}
	wm iconname $w $name
	if {[regexp $manx(posnregexp) $manx(iconposition) all x y]} {
		wm iconposition $w $x $y
	}
	foreach i {iconbitmap iconmask iconwindow} {wm $i $w ""}

	if {$manx(iconbitmap)=="(default)" || $manx(iconbitmap)=="default"} {
		set iw .iconwindow
		if ![winfo exists $iw] {toplevel $iw; label $iw.l -image icon; pack $iw.l}
		wm iconwindow $w $iw
	} else {
		# everybody shares same icons
		# (null string for name means no icon)
		foreach i {iconbitmap iconmask} {
			if {$manx($i)!=""} {
				if [catch {wm $i $w @$manx($i)} info] {
					puts stderr "Bad $i: $manx($i).  Fix in Preferences/Icon."
#					tk_dialog .dialog "BAD [string toupper $i]" "Bad $i: $manx($i).  Fix in Preferences/Icon." "" 0 " OK "
				}
			}
		}
	}


	# random bits

	$w.mono configure -onvalue $font(mono) -offvalue $font(pro)
	pack $w.v -side $man(scrollbarside)
	set tk_strictMotif $man(strictmotif)
	set symbolfont [spec2font "symbol" "normal" $man(gui-points)]
	$w.dups configure -font $symbolfont
	$w.search.next configure -font $symbolfont; $w.search.prev configure -font $symbolfont
	$w.mono configure -font [spec2font "Courier" $man(gui-style) $man(gui-points)]
	foreach i [list $w.occ.m $w.paths.m $w.occ.m.m2 $w.occ.m.m3] {
		$i configure -selectcolor $man(guifg)
	}

	# the scrollbar is an oddball
	$w.v configure -troughcolor $man(guibg)
	$w.v configure -background [expr {$man(buttbg)!=$man(guibg)?$man(buttbg):$man(guibg)}]

	# text widget fonts and tags
	set t $w.show
	$t configure -padx $man(textboxmargin) -pady $man(textboxmargin)
	$t configure -font $man(currentfont)
	manManTabSet $w
# -tabs 0.2i -- tabs set to 5 spaces in current font
	$t tag configure volume -font $font(vol) -tabs $man(volcol)
	$t tag configure apropos -tabs $man(apropostab) -lmargin2 $man(apropostab)

	$t tag configure sel -foreground "$man(textbg)" -background "$man(textfg)"

	foreach v $manx(tags) {
#puts "$t => [winfo class $t]"
		$t tag configure $v -font "" -foreground "" -background "" -underline no
		set change ""; set newfont 0
		set fam $man(text-family); set sty $man(text-style); set poi $man(text-points); set poi2 "m"
		foreach g $man($v) {
			switch -exact -- $g {
				normal {}
				underline {append change " -underline yes"}
				reverse {append change " -foreground {$man(textbg)} -background {$man(textfg)}"}
				italics { set sty $g; set newfont 1 }
				bold -
				bold-italics {
					set sty $g; set poi2 "s"; set newfont 1
				}
				mono { set fam "Courier"; set poi2 "s"; set newfont 1 }
				symbol { set fam "Symbol"; set sty "normal"; set newfont 1 }
				small -
				medium -
				large {
					set poi $g; set newfont 1
				}
				s -
				m -
				l {
					set poi2 $g; set newfont 1
				}
				left -
				right -
				center {
					append change " -justify $g"
				}
				default {append change " " [list $g]}
			}
		}
		if {$newfont} {append change " -font \"[spec2font $fam $sty $poi $poi2]\""}
#puts "change = $change"
		eval $t tag configure $v $change
	}
	$t tag raise sel


	if {[[set m2 $w.occ.m.kill] index end]=="none" || $curedit(printers)!=$man(printers) } {
		set plist [string trim $man(printers)]
		if [lsearch $plist $manx(printer)]==-1 { set plist [concat $manx(printer) $plist] }

		#if {[llength $plist]==1} => always want level of indirection for printing
		$m2 delete 0 end
		foreach p $plist { $m2 add command -label $p -command "manPrint $w $p" }
		if ![llength $plist] { $m2 add command -label "(default)" -command "manPrint $w" }
	}

	if {$curedit(hyperclick)!=$man(hyperclick)} {manHyper $w ""}
	if {$curedit(subvols)!=$man(subvols)} {manMakeVolList $w}
}


proc resetfonts {font {w .}} {
	# do current window first--may cancel reconfig without examining children
	if ![catch {set oldfont [$w configure -font]}] {
		if {$oldfont==$font} return
		$w configure -font $font
	}

	# recurse
	foreach c [winfo children $w] {
		resetfonts $font $c
	}
}

proc resetcolors {{w .}} {
	global man curedit

	set c [winfo class $w]
	set g "gui"; if {$c=="Text"} {set g "text"} elseif {[string match "*utton" $c]} {set g "butt"}
#||$c=="Entry"
	set foreground [set selector [set insertbackground $man(${g}fg)]]
	set background $man(${g}bg)
	set ofg $curedit(${g}fg)
	set obg $curedit(${g}bg)

#puts "checking $w, class $c"
	foreach i {foreground background insertbackground selector} {
		if ![catch {set color [$w cget -$i]}] {
			 if {$color==$ofg} {$w configure -$i $foreground} \
			 elseif {$color==$obg} {$w configure -$i $background}
		}
	}

	set activeforeground $man(activefg)
	set activebackground $man(activebg)
	set highlightbackground $activebackground
	foreach i {activeforeground activebackground highlightbackground} {
		catch {$w configure -$i [set $i]}
	}

	# recurse
	foreach c [winfo children $w] {
		resetcolors $c
	}
}


proc spec2font {{family "times"} {style "normal"} {points "medium"} {size "m"}} {
	global man manx

	set dpi $man(dpi)
#puts -nonewline "$family + $style + $points ( $size ) + $dpi => "
	switch -exact -- $style {
		normal {set style "medium-r"}
		bold {set style "bold-r"}
		italics {
			if [regexp -nocase $manx(xmono) $family] {set style "medium-o"} {set style "medium-i"}
		}
		bold-italics {
			if [regexp -nocase $manx(xmono) $family] {set style "bold-o"} {set style "bold-i"}
		}
		default {puts stderr "nonexistent style: $style"; exit 1}
	}

	# specify s,m,l within small,medium,large; or set absolute point size
	if {[set pts [lsearch $manx(sizes) $points]]!=-1} {
		set p "[lindex [lrange $manx(pts) $pts end] [lsearch {s m l} $size]]"
	} else {set p $points}
	append p "0"

	set font "-*-$family-$style-normal-*-*-$p-$dpi-$dpi-*"
#puts $font

	return $font
}


#--------------------------------------------------
#
# manSave -- manage merging with old config file
# if passed name of save file, could put this into utils
#
#--------------------------------------------------

proc manSave {} {
	global man manx env

	if {$manx(savegeom)} {set man(geom) [wm geometry .man]}
	set w [manPreferencesMake]
	if [winfo exists $w] {set man(geom-prefs) [geom2posn [wm geometry $w]]}

	set nfn $manx(startup)
	set ofn $manx(startup)-bkup

	if {![file exists $nfn] || [file writable $nfn]} {
		if {[file exists $nfn]} {eval exec $man(cp) $nfn $ofn}
		if [catch {set fid [open $nfn w]}] {
			manWinstderr .man "$nfn is probably on a read-only filesystem"
			return
		}
		foreach p [info procs *SaveConfig] {eval $p $fid}
		puts $fid "manDot\n"
		puts $fid $manx(userconfig)

		if {[file exists $ofn]} {
			set ofid [open $ofn]
			set p 0
			while {[gets $ofid line]!=-1} {
				if {$p} {puts $fid $line} \
				elseif {$manx(userconfig)=="$line"} {set p 1}
			}
		}
		close $fid
	}

	# don't delete ~/.oldtkman
}



#--------------------------------------------------
#
# manSaveConfig -- dump persistent variables into passed file id
#
#--------------------------------------------------

proc manSaveConfig {fid} {
	global man manx high default

# if only persistent stuff in man array,
# could have general SaveConfig in utils.tcl

	# write out preamble
	puts $fid "#\n# TkMan v$manx(version)"
# insert $env(USER)?
	puts $fid "# configuration saved on [clock format [clock seconds]]"
	puts $fid "#\n"

	set preamble {
		Elements of the man array control many aspects of operation.
		Most, but not all, user-controllable parameters are available
		in the Preferences panel.  All parameters are listed below.
		Those that are identical to their default values are commented
		out (preceded by \"#\") so that changes in the defaults will propagate nicely.
		If you want to override the default, uncomment it and change the value
		to its new, persistent setting.
	}
	foreach line [split [linebreak $preamble] "\n"] {
		puts $fid "# $line"
	}
	puts $fid ""


	# save miscellaneous variables

	manStatsSet; # update its value
	# write out man(), commenting out if same as corresponding default()
	# maybe iterate through default array ==> no!  won't pick up man(/<directory>)'s
	foreach i [lsort [array names man]] {
		if {[info exists default($i)]} {
			if {$default($i)==$man($i)} {set co "#"} {set co ""}
			puts $fid "${co}set [list man($i)] [tr [list $man($i)] \n \n$co]"
		} elseif {[string match "/*" $i]} {puts $fid "set man($i) $man($i)"}
	}

	puts $fid "\n\n#\n# Highlights\n#\n"
#	puts $fid "# format: time, start/end pairs for use by text widget"
	foreach i [lsort [array names high]] {
		puts $fid "set [list high($i)] [list $high($i)]"
	}

	puts $fid "\n"
}



#--------------------------------------------------
#
# manStats* -- cumulative statistics
#
#--------------------------------------------------

# write out new stats line
proc manStatsSaveFile {} {
	global man manx

DEBUG {puts -nonewline "manStatsSaveFile"}
	# if flag not set, don't bother
	if {$manx(statsdirty) && [file readable $manx(startup)] && [file writable $manx(startup)]} {
		# read in file
		set fid [open $manx(startup)]; set startup [read $fid]; close $fid

		# update and insert man(stats)
		set stats "set man(stats) [list [manStatsSet]]\n"

		# rewrite variable or instantiate new variable before userconfig, if such a line exists
		if [regsub "set man\\(stats\\)\[^\n\]+\n" $startup $stats nstartup] {
			# problem copying from and into same variable, though seems to work in other cases
			set startup $nstartup
#puts "\n$startup\n"
DEBUG {puts -nonewline "\tfound existing variable"}
		} elseif [regsub "\n\n$manx(userconfig)\n" $startup "\n$stats\n$manx(userconfig)\n" startup] {
DEBUG {puts -nonewline "\tinserted before userconfig"}
		} else {
			set startup [concat $startup $stats]
DEBUG {puts -nonewline "\tappended"}
		}

		# write it out
		set fid [open $manx(startup) "w"]; puts -nonewline $fid $startup; close $fid
DEBUG {puts -nonewline "\tsaved to file"}
	}
DEBUG {puts ""}

	after [expr 5*60*1000] manStatsSaveFile
#	after [expr 5*1000] manStatsSaveFile
}


# save as name-value pairs so can extend list without positional dependencies leading to madness
proc manStatsSet {} {
	global man manx stat

DEBUG {puts "manStatsSet"}
	trace vdelete stat w manStatsDirty
	scan $man(stats) "%d" stat(cum-time)
	set newstats $stat(cum-time)

	# bump up monolithic cumulative
	foreach i $manx(all-stats) {
		# get cumulative value
		if {[set val [lassoc $man(stats) $i]]!=""} {
			set stat(cum-$i) $val
		} else {
			set stat(cum-$i) $stat($i)
		}

		# bump up cumulative totals
		incr stat(cum-$i) [expr $stat($i)-$stat(cur-$i)]
		set stat(cur-$i) $stat($i)

		lappend newstats [list $i $stat(cum-$i)]
	}

	set man(stats) $newstats
	trace variable stat w manStatsDirty
	set manx(statsdirty) 0

	return $man(stats)
}

# set trigger of writes to stat array to set flag
proc manStatsDirty {name1 name2 op} {
	global manx
DEBUG {puts "manStatsDirty: $name2"}
	set manx(statsdirty) 1
}


#--------------------------------------------------
#
# Startup initialization and validity checks
#
#--------------------------------------------------


# after reading .tkman variables but before executing code

proc manDot {} {
	global manx

	manManManx

	manPresDefaultsSet
	# make window here so ~/.tkman commands can manipulate it without `after'
	toplevel .man -class TkMan
	manPreferencesGet fill

	set manx(manDot) 1
}

proc manManManx {} {
	global man manx
	# these get set to man counterparts, unless overwritten by command line options
	foreach i {
			iconify iconname iconbitmap iconmask iconposition
			geom database glimpsestrays
		} {
		if {![info exists manx($i)]} {set manx($i) $man($i)}
	}
}


# check for MANPATH in various places
# set manx(MANPATH0), maybe add to warnings

set manx(manpathset-warnings) ""
proc manManpathSet {} {
	global manx env

	set manx(manpathset-warnings) ""
	set ws "\[ \t\]"; set nws "\[^ \t\]"

	# if user has MANPATH set, use it
	if {![info exists env(MANPATH)] || $env(MANPATH)==""} {
		set manpath ""; set manpathsrc ""; set def [string trim $manx(manpathdef)]

		# using GNU's gmanpath?
		if ![catch {set gmanpath [exec gmanpath -q]}] {
			set manpathsrc "given by gmanpath"
			set manpath $gmanpath


		# BSDI: locations of man pages stored in "man.conf"?
		} elseif [file readable [set manconf "/etc/man.conf"]] {
			set manpathsrc "read from $manconf"

			set fid [open $manconf]
			while {![eof $fid]} {
				gets $fid line
				if {[regexp "^_default$ws+(.*)" $line all dirs]} {
					foreach globdir $dirs {
						foreach dir [glob -nocomplain [string trimright $globdir "/"]] {
							append manpath "$dir:"
						}
					}
				}
			}
			close $fid


		# Linux: locations of man pages stored in "man.config"?
		} elseif {[file readable [set manconf "/etc/man.config"]] || [file readable [set manconf "/etc/manpath.config"]]} {
			set manpathsrc "read from $manconf"
			set manpathlist ""

			set fid [open $manconf]
			while {![eof $fid]} {
				gets $fid line
				if {[regexp "^(MANDATORY_)?MANPATH$ws+(.*)" $line all junk man]} {
					append manpath ":$man"; lappend manpathlist $man
				} elseif {[regexp "^MANPATH_MAP$ws+($nws+)$ws+($nws+)" all bin man]} {
					if {[lsearch $manx(bin-paths) $bin]!=-1 && [lsearch $manpathlist $man]==-1} {
						append manpath ":$man"; lappend manpathlist $man
					}
				}
			}
			close $fid


		# SCO has /etc/default/man
		} elseif [file readable [set manconf "/etc/default/man"]] {
			set fid [open $manconf]
			while {![eof $fid]} {
				if {[regexp "^\[ \t]*MANPATH\[ \t]*=\[ \t]*(.*)" [gets $fid] all tmp]} {
					regsub "scohelp:" $tmp "" man
					# take last MANPATH= line so don't break
				}
			}
			close $fid

			set manpathsrc "set to system default in $manconf"
			set manpath $man

		# on an SGI, go with its default
		} elseif {[file readable /usr/catman] || [file readable /usr/share/catman]} {
			set manpathsrc "set to the IRIX default"
			set manpath "/usr/share/catman:/usr/share/man:/usr/catman:/usr/man"


		# set to Makefile's default setting
		} elseif {$def!=""} {
			set manpathsrc "set to local default"
			set manpath $def


		# calculate MANPATH from PATH -- your seventh--eighth if you count env(MANPATH)--and final chance
		# (if no MANPATH and no PATH set initially, MANPATH set to /usr/local/man:/usr/man)
		} else {
			set manpathsrc "calculated from your PATH"

			foreach i $manx(bin-paths) {
				if {$i==""} continue
				if [regexp {(.*)/bin(/.*)?} $i all dirtop] {
					set i $dirtop/man
					if {[manPathCheckDir $i]==""} {
						append manpath "$i:"
					}
				}
			}
		}


		# all alternative settings finally set MANPATH here
		set manpath [string trim $manpath ":"]
		set env(MANPATH) $manpath

		regsub -all ":" $manpath ":\n    " manpathshow
		append manx(manpathset-warnings) "You don't have a MANPATH environment variable set, but you should.\n"
		append manx(manpathset-warnings) "Assuming a MANPATH, $manpathsrc, of:\n    $manpathshow\n\n"
	}

	set manx(MANPATH0) $env(MANPATH)
DEBUG {puts stderr "env(MANPATH) => $env(MANPATH)"}
}


# check validity of MANPATH and set manx(paths)

set manx(manpath-warnings) ""
proc manManpathCheck {} {
	global man manx env

	set manx(manpath-warnings) ""
	set manx(paths) {}
	set manpatherr ""
	set whatiserr 0
	set glimpseerr 0
	set glimpseuptodate 1
	set pass 0

	# dumb BSDI concatenates all whatis information into single file
	set fBSDI [file readable [set whatis "/usr/share/man/whatis.db"]]
	# would like to generate validity information for it, but
	# if it does exist, then that's OK (it's BSDI) and if it
	# doesn't that's OK (it's got a good whatis organization)

	# dumb HPUX concatenates all whatis information into single file
	set fHPUX 0; if {!$fBSDI} { set fHPUX [file readable [set whatis "/usr/lib/whatis"]] }


	foreach root [split $manx(MANPATH0) ":"] {
		# not a general solution, but expand some abbreviations
		if {[string match "./*" $root] || [string match "../*" $root]} {
			# could expand relative paths, but that's not a good fit with the database
			append manpatherr "$root ignored -- relative paths are incompatible with TkMan\n"
			continue
		}
#		if {$root=="."} {set $root [pwd]}
		if [string match "~*" $root] {set root [glob -nocomplain $root]}
		if {[string trim $root]==""} continue

		if {[string match "/?*/" $root ]} {
			append manpatherr "$root -- spurious trailing slash character (\"/\")\n"
			# clean this one up and keep on going
			set root [string trimright $root "/"]
		}

		if {$root=="/"} {
			append manpatherr "$root -- root directory not allowed\n"
		} elseif {[lsearch $manx(paths) $root]>=0} {
			append manpatherr "$root -- duplicated path\n"
		} elseif {[set tmp [manPathCheckDir $root]]!=""} {
			append manpatherr $tmp
		} elseif {![string match "*/catman" $root] && [glob -nocomplain $root/$manx(subdirs)]==""} {
			# if nothing in that directory, something's probably wrong
			append manpatherr "$root -- no subdirectories matching $manx(subdirs) glob pattern\n"
			# directory too specific: a subdirectory?
			if {![string match "*/man" $root] && [glob -nocomplain "[file dirname $root]/$manx(subdirs)"]!=""} {
				append manpatherr "    => try changing it to [file dirname $root]\n"
			# or not specific enough?
			} elseif {[file exists $root/man]} {
				append manpatherr "    => try changing it to $root/man\n"
			}
		} else {
			# directory looks good, add it to list of valids
			lappend manx(paths) $root
			if {![info exists man($root)]} {set man($root) 1}
			lappend manx(pathstat) $man($root)
			set manx($root,latest) [lfirst [manLatestMan $root]]

			# check for apropos index (called windex on Solaris)
			if {!$fBSDI && !$fHPUX} {
				if ![file exists [set whatis $root/windex]] {
					set whatis $root/whatis
				}
			}

			if {![file exists $whatis]} {
				append manpatherr "$root -- no `whatis' file for apropos\n"
				if {!$whatiserr} {
					append manpatherr "    => generate `whatis' with mkwhatis\n"
					set whatiserr 1
				}
			} elseif {![file readable $whatis]} {
				# whatis set above
				append manpatherr "$whatis not readable\n"
			} elseif {[file mtime $whatis]<$manx($root,latest)} {
				append manpatherr "$whatis out of date\n"
			}

			# now check for Glimpse files
			if {$man(glimpse)!=""} {
				set g $root; if {$man(indexglimpse)!="distributed"} {set g $man(glimpsestrays)}
				set gi $g/.glimpse_index

				if {$man(indexglimpse)=="distributed" || $pass==0} {
					if {[glob -nocomplain $g/.glimpse*]==""} {
						append manpatherr "$g -- no Glimpse support\n"
						if {!$glimpseerr} {
							append manpatherr "    => try building Glimpse database (under Occasionals)\n"
							set glimpseerr 1; set glimpseuptodate 0
						}
					} elseif {![file readable $gi]} {
						append manpatherr "$g -- Glimpse index exists but not readable\n"
					}
				}

				if {[file readable $gi] && [file mtime $gi]<$manx($root,latest)} {
						append manpatherr "$root -- Glimpse index out of date\n"
				}
			}
		}

		incr pass
	}

	if {$manpatherr!=""} {
		append manx(manpath-warnings) "Problems in paths of MANPATH environment variable...\n" $manpatherr "\n"
	}

	if ![llength $manx(paths)] {
		puts stderr "NO VALID DIRECTORIES IN MANPATH!\a"
		exit 1
	}
}

proc manPathCheckDir {dir} {
	set warning ""

	if {![file exists $dir]} {
		set warning "doesn't exist"
	} elseif {![file isdirectory $dir]} {
		set warning "not a directory"
	} elseif {![file readable $dir]} {
		set warning "not readable\n    => check permissions"
	} elseif {![file executable $dir]} {
		set warning "not searchable (executable)\n    => check permissions"
	} elseif {[glob -nocomplain $dir/*]==""} {
		set warning "is empty"
	}

	if {$warning!=""} {set warning "$dir -- $warning\n"}
	return $warning
}

proc manBinCheck {} {
	global man manx env stat

	# binaries checks for rman, sed, ...
	set err 0
	foreach var $manx(binvars) {
		set val [set $var]
		if {$val==""} continue

		# first check for existence and executability...
		foreach pipe [split $val "|"] {
			set bin [lfirst $pipe]
			set found 0
#; set exe 0

#puts stderr "checking $var's $bin"
			if {[string match "/*" $bin]} {set pathlist [file dirname $bin]; set tail [file tail $bin]
			} else {set pathlist $manx(bin-paths); set tail $bin}

			foreach dir $pathlist {
				if {$dir=="."} continue
				set fullpath $dir/$tail
				# ignore file if not executable, just like shells do
				if {[file isfile $fullpath] && [file executable $fullpath]} {
					set found 1
#					if {[file executable $fullpath]} {set exe 1}
#puts stderr "\tfound @ $dir"
					set stat(bin-$bin) $fullpath
					break
				}
			}

			if {!$found} {
				puts stderr "$bin not found--check the $var variable in $manx(startup) or the Makefile."
				incr err
#			} elseif {!$exe} {
#				puts stderr "$bin found but not executable--check permissions."
#				incr err
			} else {
				# special check for egrep (must have extended/full regular expressions)
				if {$var=="man(egrep)"} {
					set ename "tkman"; set evol "1"; set eext "tk"
					set eman "/usr/man/man1/$ename.$evol$eext"
					set eregexp "/${ename}(\\..$eext\[^/\]*)?$"
					set ematch ""
					set egreperr [catch {set ematch [exec echo $eman | $fullpath $eregexp]} info]
					if {$egreperr || $ematch!=$eman} {
						puts stderr "Change `egrep' in Makefile or `man(egrep)' in $manx(startup),\nfrom $man(egrep), which maps to $fullpath,\nto something that can handle extended or full regular expressions."
						incr err
						continue
					}
				}

				# ... then check for proper versions of selected executables
				if {[set info [lassoc $manx(bin-versioned) $tail]]!=""} {
					lset $info flag minvers
					set execerr [catch {set lines [exec $fullpath $flag < /dev/null]} info]
				} elseif {[string match "g*" $tail]} {
					# could be a GNU -- maybe take this out since it lengthens startup for all in exchange for small benefit for few
					set minvers 0.0
					#foreach flag {"--version" "-V" "-v"} -- takes to long to start up
					# GNU programs print version information to stderr
					set execerr [catch {set lines [exec $fullpath $flag < /dev/null]} info]
					set execerr 0; set lines $info
				} else continue

				if {$execerr && $minvers==0.0} {
					# nothing
				} elseif {$execerr} {
					puts "ERROR executing \"$fullpath $flag\": $info\a"
					incr err
				} else {
					set line ""; foreach line [split $lines "\n"] { if {$line!=""} break }
					# grok version number
					set vers unknown
					if {$line=="" || ![regexp $manx(bin-versregexp) $line vers cmpvers] || $cmpvers<$minvers} {
						if {$minvers!=0.0} {
							puts stderr "$bin is version $vers--must be at least $minvers."
							incr err
						}
					} else { set stat(bin-$bin-vers) $vers }
				}
			}
		}
	}
	if {$err} {exit 1}
}

proc manParseCommandline {} {
	global manx argv argv0 env

	for {set i 0} \$i<[llength $argv] {incr i} {
		set arg [lindex $argv $i]; set val [lindex $argv [expr $i+1]]
		switch -glob -- $arg {
			-help -
			-h* -
			--help {
				set helptxt {
					[-M <MANPATH>] [-M+ <paths to append to MANPATH>] [-+M <paths to prepend to MANPATH>]
					[-[!]now] [-rebuildandquit] [-[!]iconify] [-version]
					[-title <string>] [-startup <file>] [-database <file>] [-[!]debug] <man page>
				}
				puts -nonewline "tkman"
				foreach line [split [linebreak $helptxt 70] "\n"] { puts "\t$line" }
				exit 0
			}
			-M {set env(MANPATH) $val; incr i}
			-M+ {append env(MANPATH) ":$val"; incr i}
			-+M {set env(MANPATH) "$val:$env(MANPATH)"; incr i}
			-now {set manx(now) 1}
			-!now {set manx(now) 0}
			-iconname {set manx(iconname) $val; incr i}
			-iconmask {set manx(iconmask) $val; incr i}
			-iconposition {set manx(iconposition) $val; incr i}
			-iconbitmap {set manx(iconbitmap) $val; incr i}
			-icon* {set manx(iconify) 1}
			-!icon* {set manx(iconify) 0}
			-rebuild* {set manx(rebuildandquit) 1}
			-quit {if [string match no* $val] {set manx(quit) 0}; incr i}
			# put the more permissive option name patterns down below
			--v* -
			-v* {puts stdout "TkMan v$manx(version) of $manx(date)"; exit 0}
			-t* {set manx(title) $val; incr i}
			-s* {set manx(startup) $val; incr i}
			-database {set manx(database) $val; incr i}
			-d* {set manx(debug) 1; set manx(quit) 0; set manx(iconify) 0}
			-!d* {set manx(debug) 0}
			-* {puts stdout "[file tail $argv0]: unrecognized option: $arg"; exit 1}
			default {
				after 2000 manShowMan $arg {{}} .man
				# permit several???  add extras to History?
				break
			}
		}
	}
	# grr, special case
	if {[info exists geometry]} {set manx(geom) $val}
}


proc ASSERT {args} {
	if {![uplevel 1 eval $args]} {
		puts "ASSERTION VIOLATED: $args"
		exit 1
	}
}

proc DEBUG {args} {
	global manx
	if $manx(debug) {uplevel 1 eval $args}
}


##################################################
###
### start up
###
##################################################

### environment checks
if {[info tclversion]<$manx(mintcl) || $tk_version<$manx(mintk)} {
	puts -nonewline stderr "Tcl $manx(mintcl)/Tk $manx(mintk) minimum versions required.  "
	puts stderr "You have Tcl [info tclversion]/Tk $tk_version"
	exit 1
} elseif {int([info tclversion])-int($manx(mintcl))>=1 || int($tk_version)-int($manx(mintk))>=1} {
	puts stderr "New major versions of Tcl and/or Tk may have introduced\nincompatibilies in TkMan.\nCheck the TkMan home ftp site for a possible new version.\n"
}


#--------------------------------------------------
#
# set defaults for shared global variables
#
#--------------------------------------------------

# # # # # # # # # # # # # # # # # # # # # # # # # #
# DON'T EDIT THE DEFAULTS HERE--DO IT IN ~/.tkman #
# # # # # # # # # # # # # # # # # # # # # # # # # #

# man() = persistent variables
# manx() = x-pire, system use, command line overrides

set w .man

#
# man
#

set man(manList) {1 2 3 4 5 6 7 8 9 l o n p D}
set man(manTitleList) {
	"User Commands" "System Calls" Subroutines
	Devices "File Formats"
	Games "Miscellaneous" "System Administration" "*** check contrib/*_bindings ***"
	Local Old New Public "*** check contrib/*_bindings ***"
}
set man(stats) [clock seconds]
set man(time-lastglimpse) -1
set man(time-lastdatabase) -1
set man(families) {Times "New Century Schoolbook" Lucida Courier Helvetica}
set manx(styles) {normal bold italics bold-italics}
set manx(pts) {10 12 14 18 24}
set manx(sizes) {small medium large}
set man(gui-family) "Times"
set man(gui-style) "bold"
set man(gui-points) "medium"
set man(text-family) "New Century Schoolbook"
set man(text-style) "normal"
set man(text-points) "medium"
set man(vol-family) "Times"
set man(vol-style) "normal"
set man(vol-points) "small"
set man(dpi) [lfirst $man(dpis)]
# try to determine if screen is 75dpi or 100dpi
if {$man(dpis)=={* 75 100}} {
    button .x
	if {[winfo fpixels . 1i]<100 && [set man(dpi) 75] && ![catch {.x configure -font [spec2font]}]} {
    } elseif {[set man(dpi) 100] && ![catch {.x configure -font [spec2font]}]} {
	} else {set man(dpi) *}
	destroy .x
}
set man(currentfont) [spec2font $man(text-family) $man(text-style) $man(text-points)]

set manx(tags) {h1 h2 h3 tt sc y b bi i search isearch manref title highlight hot indent indent2}
set man(tt) mono
set man(sc) s
set man(y) symbol
set man(b) bold
set man(bi) bold-italics
set man(i) italics
set man(h1) {bold l}
set man(h2) {bold m}
set man(h3) italics
set man(isearch) {-background gray}
set man(search) reverse
set man(manref) {mono -foreground blue}
set man(title) {bold large l}
set man(hot) {-underline yes}
set man(highlight) {-background #ffd8ffffb332}; # a pale yellow
set man(indent) {-lmargin1 5m -lmargin2 10m}
set man(indent2) {-lmargin1 10m -lmargin2 15m}

set man(maxhistory) 15; set manx(maxhistory-v) [set manx(maxhistory-t) {5 10 15 20 30 40 50}]
set man(recentdays) 14; set manx(recentdays-v) [set manx(recentdays-t) {1 2 7 14 30 60 90 180}]
set man(shortcuts) {}
set man(indexglimpse) "distributed"; set manx(indexglimpse-v) [set manx(indexglimpse-t) {"distributed" "unified"}]
set man(maxglimpse) 200; set manx(maxglimpse-v) [set manx(maxglimpse-t) {25 50 100 200 500 1000 "none"}]
set man(whatwhere) 1; set manx(whatwhere-v) {1 0}; set manx(whatwhere-t) {"pathname" "description"}
set man(iconify) 0; set manx(iconify-v) {1 0}; set manx(iconify-t) {"yes" "no"}
set man(subsect) ""; set manx(subsect-v) {"-b" ""}; set manx(subsect-t) {"yes" "no"}
set man(nroffsave) "off"; set manx(nroffsave-v) [set manx(nroffsave-t) {"off" "on" "on & compress"}]
set man(headfoot) "-k"; set manx(headfoot-v) {"-k" ""}; set manx(headfoot-t) {"yes" "no"}
set man(hyperclick) "single"; set manx(hyperclick-v) [set manx(hyperclick-t) {"double" "single"}]
set man(incr,case) -1
set man(regexp,case) -1
set man(aproposfilter) {| sort | uniq}
set man(scrollbarside) right; set manx(scrollbarside-v) [set manx(scrollbarside-t) {"left" "right"}]
set man(zaphy) ""; set manx(zaphy-v) {"-y" ""}; set manx(zaphy-t) {"yes" "no"}
set man(updateicon) 0; set manx(updateicon-v) {1 0}; set manx(updateicon-t) {"yes" "no"}
set man(strictmotif) 0; set manx(strictmotif-v) {1 0}; set manx(strictmotif-t) {"yes" "no"}
set man(subvols) 1; set manx(subvols-v) {1 0}; set manx(subvols-t) {"yes" "no"}
set man(optmenus) 0; set manx(optmenus-v) {1 0}; set manx(optmenus-t) {"yes" "no"}
set man(textboxmargin) 5; set manx(textboxmargin-v) [set manx(textboxmargin-t) {0 1 2 3 4 5 7 10}]
set man(error-effect) "bell & flash"; set manx(error-effect-v) [set manx(error-effect-t) {"bell & flash" "bell" "flash" "none"}]
set man(volcol) 4.0c; set manx(volcol-v) {0 1.5c 2.0c 2.5c 3.0c 3.5c 4.0c 4.5c 5.0c 7.5c 10.0c}; set manx(volcol-t) {"no columns" "1.5 cm" "2 cm" "2.5 cm/~1 inch" "3 cm" "3.5 cm" "4 cm" "4.5 cm" "5.0 cm/~2 inches" "7.5 cm" "10 cm"}
set man(apropostab) "4.5c"; set manx(apropostab-v) {0 3.0c 4.0c 4.5c 5.0c 5.5c 6.0c 7.5c 10.0c}; set manx(apropostab-t) {"none" "3 cm" "4 cm" "4.5 cm" "5 cm" "5.5 cm" "6 cm" "7.5 cm" "10 cm"}
set man(changeleft) "-c"
#set man(tables) ""; set manx(tables-v) {"-T" ""}; set manx(tables-t) {"on" "off"}
set man(high,hcontext) 50
set man(high,vcontext) 10; set manx(high,vcontext-v) [set manx(high,vcontext-t) {0 2 5 7 10 15 20}]
set man(geom) 570x800+150+10
set man(geom-prefs) +300+300
set man(iconname) "TkMan"
#set man(iconbitmap) "" -- moved to Makefile
set man(iconbitmap) "(default)"
set man(iconmask) ""
set man(iconposition) ""
#set man(database) $env(HOME)/.tkmandatabase -- set in Makefile
set man(startup) $env(HOME)/.tkman
#set man(hypertext-binding) <Double-Button-1>

# hard to have bulletproof UI for this
set man(date-format) "%H:%M, %e %B %Y"

# colors
set man(colors) {black white red "light yellow" yellow orange green blue beige SlateGray4 gray75 gray90}

# set preferred colors
#set man(textfg) "black"
#set man(textbg) "white"
#set man(buttfg) [set man(activefg) [set man(guifg) "gray"]]
#set man(buttbg) [set man(guibg) "beige"]
#set man(activebg) #eed5b7

# colors are Tk's defaults
checkbutton .a
set man(textfg) [set man(buttfg) [set man(guifg)  [.a cget -foreground]]]
set man(textbg) [set man(guibg) [set man(buttbg) [.a cget -background]]]
set man(activefg) [.a cget -activeforeground]
set man(activebg) [.a cget -activebackground]
destroy .a

set manx(mono) [expr [winfo depth .]==1]
# [expr {[tk colormodel .]=="monochrome"}]
#set manx(mono) 1
if $manx(mono) {
	set man(foreground) "black"
	set man(background) "white"
	set man(colors) {black white}
	set man(textfg) [set man(activebg) [set man(buttfg) [set man(guifg) "black"]]]
	set man(textbg) [set man(activefg) [set man(buttbg) [set man(guibg) "white"]]]

	set man(search) [set man(isearch) "reverse"]
	set man(highlight) "bold-italics"
	set man(manref) "mono underline"

	# any more modifications for monochrome?
}


### collect all man's to this point as the defaults
# used for Defaults button in Preferences
# and to "comment out" unchanged parameters, so that subsequent
# changes (corrections), usually in the Makefile, propagate as expected
# (i.e., should make for fewer cases of "delete ~/.tkman")
# CAREFUL!  Only man()'s recorded here in defaults are saved to ~/.tkman
foreach i [array names man] {set default($i) $man($i)}


#
# man()-independent manx()
#

set manx(title) "TkMan"
regexp "..../../.." {$Date$} manx(date)
set manx(date-seconds) [clock scan [string range $manx(date) 5 end]/[string range $manx(date) 0 3]]
set manx(expire-warnings) ""
if {[clock seconds] > [clock scan "2 years" -base $manx(date-seconds)]} {
	set manx(expire-warnings) "This is a very old copy of TkMan.  Please check ftp://ftp.cs.berkeley.edu/people/phelps/ucb/tcltk for the latest and greatest."
}
set manx(manList) $man(manList)
set manx(manTitleList) $man(manTitleList)
set manx(userconfig) "### your additions go below"
set manx(posnregexp) {([-+]?[0-9]+)([-+][0-9]+)}
set manx(init) 0
set manx(manDot) 0
set manx(cursor) left_ptr
set manx(yview,help) 0
set manx(paths) ""
set manx(pathstat) ""
set manx(uid) 1
set manx(outcnt) 1
set manx(debug) 0
set manx(defaults) 0
set manx(startup) $man(startup)
set manx(savegeom) 1
# can't be sure volume 1 exists(!)
#set manx(lastvol$w) 1
set manx(lastman) TkMan
set manx(quit) 1
set manx(mandot) ""
set manx(db-manpath) ""
set manx(db-signature) ""
set manx(rebuildandquit) 0
set manx(now) 0
set manx(newmen) ""
set manx(xmono) {courier|helvetica}
set manx(subdirs) "{man,cat}?*"
set manx(shift) 0
# default printer
if [info exists env(PRINTER)] { set manx(printer) $env(PRINTER) } \
elseif [info exists env(LPDEST)] { set manx(printer) $env(LPDEST) } \
else { set manx(printer) "" }

# versions of supporting binaries, if those binaries happen to be used
set manx(bin-versregexp) {([0-9]+\.[0-9]+)([^ ,;]+)?} ;#(\.[0-9]+)?
set manx(bin-versioned) {{rman "-v" 2.4} {glimpse "-V" 2.1} {glimpseindex "-V" 2.1}}
set manx(modes) {man help section txt apropos super info}
# bug}
set manx(mode-desc) {"manual pages seen" "references to help page" "volume listings invoked" "text files seen" "apropos listings" "Glimpse full-text searches" "ganders at this page"}
DEBUG { assert [llength $manx(modes)]!=[llength $manx(mode-desc)] "mismatched modes lists" 1 }
set manx(stats) {man-hyper man-dups man-button man-link man-history man-shortcut page-section page-highlight-go page-highlight-add page-highlight-sub page-shortcut-add page-shortcut-sub page-regexp-next page-regexp-prev page-incr-next page-incr-prev page-mono high-exact high-move high-lose print}
set manx(stats-desc) {"via hyperlink" "via multiple matches pulldown" "via man button or typein box" "via links menu" "via history menu" "via shortcut menu" "jumps to section header" "jump to highlight" "highlight additions" "highlight deletions" "shortcut additions" "shortcut deletions" "regular expression searches forward" "regular expression searches backward" "incremental searches forward" "incremental searches backward" "swaps between proportional font and monospaced" "exact" "repositioned" "lost" "pages printed"}
DEBUG { assert [llength $manx(stats)]!=[llength $manx(stats-desc)] "mismatched stats lists" 1 }
set stat(man-no) 0
set manx(all-stats) [concat $manx(modes) $manx(stats) "man-no"]
foreach i $manx(all-stats) {set stat(cur-$i) [set stat($i) 0]}


#source ~/spine/tkman/contrib/local-man.tcl
#set high(*) "format: time annotation made, start position and context, end position and context"

# TKMAN environment variable gives standard options
# env(TKMAN) goes first so options given on command-line override
if [info exists env(TKMAN)] {set argv "$env(TKMAN) $argv"}
if {![info exists env(PATH)] || $env(PATH)==""} { set env(PATH) "/usr/local/bin:/usr/bin" }
# don't pick up changes to env(PATH) in startup file (just so you know)
set manx(bin-paths) [split [string trim $env(PATH) ":"] ":"]

#puts "*** manParseCommandline"
# usually want to do this after startup file, but need to get -M here, before manx(startup) (?)
manParseCommandline
set manx(startup-short) [file tail $manx(startup)]

# execute some arbitrary Tcl code for new users (it may create a startup file)
if {![file exists $manx(startup)]} {eval $manx(newuser)}

# read in startup file after proc/vars/ui, so can modify them
set manx(savefilevers) "unknown"
set manx(updateinfo) ""
#puts "*** before startup"

# if on special case OS and don't have startup file, instantiate right startup code
# uname -s -r: SunOS 5.5 on ecstasy, IRIX 5.2 on bohr, SCO_SV on SCO OpenServer Release 5
# (for future references: OSF1 V3.2 on tumtum, HP-UX A.09.05 on euca)
# Put this code after eval newuser.

if {$manx(startup)!="" && ![file exists $manx(startup)] && [file writable [file dirname $manx(startup)]]} {
	catch {
	set os [string tolower [exec uname -s]]; set osvers [exec uname -r]
	set setup ""; # most OSes work without configuration file

	# only three trouble makers
	if {$os=="sunos" && $osvers>=5.0} { set setup solaris } \
	elseif {$os=="irix" || $os=="sco_sv"} { set setup $os }
#puts stderr "\afound $setup"

	if {$setup!=""} {
		set fid [open $manx(startup) "w"]
		puts $fid $manx($setup-bindings)
		close $fid
	}
   }
}



if {$manx(startup)!="" && [file readable $manx(startup)]} {
	set fid [open $manx(startup)]

	# I don't know how this happens, but it has, apparently
	if [string match "#!*" [gets $fid line]] {
		puts stderr "$manx(startup) looks like an executable."
		puts stderr "You should delete it, probably."
		exit 1
	}

	while {![eof $fid]} {
		if {[regexp {^# TkMan v([0-9.]+.*)} $line all manx(savefilevers)]} {
			break
		}
		gets $fid line
	}
	catch {close $fid}
DEBUG {puts "*** savefilevers = $manx(savefilevers)"}

	source $manx(startup)
#puts "*** read $manx(startup)"

	# update several variables from old save files
	#if {$manx(savefilevers)!=$manx(version)} {
	if {$manx(savefilevers)=="unknown" || [string range $manx(savefilevers) 0 2]<1.6} {
	#if {[string range $manx(savefilevers) 0 2]<[string range $manx(version) 0 2]} {
	#if {[string range $manx(savefilevers) 0 2]<1.6} {
		set manx(updateinfo) "Startup file information updated from version $manx(savefilevers) to version $manx(version).\n"

		### changes for < 1.6
		# update these variables
		foreach var {catsig compress zcat} {
#	 detailed list of changes more information than user wants
#			append manx(updateinfo) "    Changed man($var) from $man($var) to $default($var)\n"
			set man($var) $default($var)
		}

		# zap problem shifted sb keys
		foreach k {greater less question} {
			set var "sb(key,MS-$k)"
			if [info exists $var] {unset $var}
#			append manx(updateinfo) "    Deleted $k keybinding (use M-$k)\n"
		}

		# call manDot before user code
		append manx(updatedinfo) "    Added call to manDot\n"
		# (if no problem before, no problem now)

		# zap old man() variables ==> done every time save file updated
#		foreach v [array names man] {
#			if {![info exists default($v)] && ![string match "/*" $v]} {
#				append manx(updateinfo) "    Deleted obsolete variable man($v) (was set to $man($v))\n"; 
#				unset man($v)
#			}
#		}

		append manx(updateinfo) "Save updates via the Quit button or Occasionals/Checkpoint, or cancel updates via \"Occasionals / Quit, don't update\".\n"
		# backup old startup file
		if ![catch "exec $man(cp) $manx(startup) [set ofn $manx(startup)-$manx(savefilevers)]"] {
			append manx(updateinfo) "Old startup file saved as $ofn\n"
		}
		append manx(updateinfo) "\n\n"
	}
	if {!$manx(manDot)} {
		#puts stderr "no mandot" -- clean this up silently
		manDot
	}
}
catch {unset high(*)}


# manx(highs) needs to get latest list of colors
set manx(highs) [concat $manx(styles) reverse underline mono $man(colors)]

set manx(extravols) [list {apropos "apropos list" "No apropos list"}]
if {$man(glimpse)!=""} { lappend manx(extravols) {glimpse "glimpse list" "No glimpse list"} }
lappend manx(extravols) {recent "Recently added/changed" {}} {high "All with Highlights" {}} {all "All Enabled Volumes" {}}
foreach i $manx(extravols) { lappend manx(specialvols) [lfirst $i] }

# calculate compression comparison expressions
set manx(zregexp) "\\.("
set manx(zglob) "{"
set manx(zoptglob) "{,"
foreach z $man(zlist) {
	append manx(zregexp) "$z|"
	append manx(zglob) "$z,"
	append manx(zoptglob) ".$z,"
}
foreach z {zregexp zglob zoptglob} { set manx($z) [string trimright $manx($z) ",|"] }
append manx(zregexp) ")\$"
append manx(zglob) "}"
append manx(zoptglob) "}"



# do this after source of ~/.tkman has chance to change some of these
set manx(binvars) {
	manx(rman) man(glimpse) man(glimpseindex)
	man(format) man(print) man(catprint) man(apropos) man(zcat) man(compress) 
	man(sed) man(egrep) man(cp) man(mv) man(rm) man(mkdir) man(find) man(chmod) man(sort)
}
manBinCheck

# assertions

if {[llength $man(manList)]!=[llength $man(manTitleList)]} {
	puts stderr "Length of section abbreviations differs from length of section titles:\n\nlength [llength $man(manList)]:\t$man(manList)\n\nlength [llength $man(manTitleList)]:\t$man(manTitleList)"
	exit 1
}

# no sense to tease glimpseindex if can't write anything
set manx(glimpseindex) ""
if {$man(glimpse)!="" && $man(glimpseindex)!=""} {
	foreach p $manx(paths) {
		if {![file writable $p]} {set manx(glimpseindex) $man(glimpseindex); break}
	}
}


# almost always you'll want to make the cross product
#	 if you don't, be tricky and `set manx(defaults) 1'
if !$manx(defaults) manDescDefaults

# set up cumulative statistics watchdog
set manx(statsdirty) 0
manStatsSet
manStatsSaveFile;	# should just set timer this time through, not save to a file

# if no stray dirs for glimpse, don't glimpse there
if {[llength $mani($manx(glimpsestrays),dirs)]==0} {set manx(glimpsestrays) ""}

set STOP 0
TkMan
#image create bitmap wmicon -data [icon cget -data]

set manx(startuptime) [clock seconds]
set starttime [time manInit]

DEBUG {
	puts stdout "init takes $starttime"

	# convenience variables
#	set w .man
	set t $w.show

	# debug box
	entry $w.in -relief sunken -textvariable manx(debugcmd)
	bind $w.in <KeyPress-Return> {manWinstdout .man "[eval $manx(debugcmd)]"}
	pack $w.in -fill x

	proc manStatsSaveFile args {}
}

manHighlightsUpdate
