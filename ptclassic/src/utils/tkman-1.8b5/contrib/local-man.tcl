# Insert this file just before the "environment checks" lines in tkman.tcl 
# (search for "### environment checks", last seen as line 3473)
# before `make'ing the executable in order to use the local `man' command
# to search for man pages.  This is useful for cases when the MANPATH 
# changes frequently.
#
# Features lost if you use the local man as the search engine:
# * volume listings, including list of recently added/changed
# * all tkmandesc commands
# * regular expression searches for man page names

# Note that the syntax for specifying sections is still
# <name>(<section>) or <name>.<section> -- not <section> <name>

# Between SGI and Tom Christiansen's Perl-based man pager, `man -w' 
# seems to be the standard for returning matches as a list of pathnames.
#
# N.B.  I HAD TO PATCH PERL MAN to add the following line
#	&get_section;
# to the whereis sub, right after declaring the local variables

# This module has not been extensively tested and I would be unsurprised 
# if there were problems.  If so, try to solve them yourself.
# I'll incorporate any fixes I'm sent, however.
# Fixes to:  phelps@cs.berkeley.edu

# 10-Jan-95  written (Perl man only)
# 19-Feb     interface for `man' commands without -w option


# if your man pager can take a -w option that returns a list
# pathnames of matching pages, set manx(man-w) to 1.
# Try to determine existence of -w dynamically:
set manx(man-w) [expr 1-[catch {exec man -w ls}]]
#set manx(man-w) 0
#set manx(man-w) 1


# if you're using AIX with all the man pages on CD-ROM, I recommend that you
# copy them onto the hard disk as described in the Makefile.  You can keep them 
# on CD-ROM by setting manx(ibm) to 1.
set manx(ibm) 0
#set manx(ibm) 1



### some proc's no longer needed
set killprocs {}
append killprocs {
	manReadSects
	manReadSection
	manDBsignature
}

### some features no longer available
# volume listings
proc manMakeVolList {w} {
	if [winfo exists $w.vols] {destroy $w.vols}
	if [winfo exists $w.occ] {$w.occ.m entryconfigure "Rebuild Database" -state disabled}
}
append killprocs {
	manShowSection
	manSortByName
	manFormatSect
}

# tkmandesc commands
append killprocs {
	manDescDefaults
	bytypenum
	manDescMove
	manDescDelete
	manDescCopy
	manDescAdd
	manDesc
	manDescAddSects
	manDescManiCheck
	manDescShow
}

proc manSetSect {w n} {
	global man manx

	if [regexp {^/} $n] {
		set name [file tail $n]
		if [regexp $man(zregexp) $name] {set name [file rootname $n]}
		set manx(cursect$w) [string range [file extension $n] 1 end]
	} elseif [lsearch $manx(manList) $n]!=-1 {
		set $manx(cursect$w) $n
	} else {
		set manx(cursect$w) *
	}
}


if $manx(man-w) {

### key proc: use local man -w to get filenames of hits
proc manShowManSearch {name {sect ""} {ext ""}} {
	global man manx env

	# reorder and concatenate section and extension
	# no need for -M$env(MANPATH) since that's the current environment anyway
	# (in fact, explicitly specifying one incites an error in Perl man 
    # if dbm files are lacking)
	set matches ""
	catch {set matches [eval exec "man -w $sect$ext $name"]}

	set matches [string trim $matches]
DEBUG {puts "\nRESULTS:   $matches"}
	return $matches
}

} else {

# no -w, so just pass on necessary information
proc manShowManSearch {name {sect ""} {ext ""}} {
	return "$name{$sect}"
}

### key proc: use local man to generate text
proc manShowManFound {f {keep 0} {w .man}} {
	global man manx

	set t $w.show; set wi $w.info
	manNewMode $w man
	set manx(manfull$w) ""
	set manx(links) ""

	if [regexp {(.*){(.*)}} $f all name sect] {
		winstdout $wi ""
		set pipe "|man $sect $name"
		if !$manx(ibm) {append pipe " | $manx(rman) -f TkMan $man(subsect) $man(tables) $man(headfoot) $man(changeleft)"}
		if [catch {set fid [open $pipe]} errmsg] {
			winstderr $wi $errmsg
		} else {
			manOpenText $w
			while {![eof $fid]} {eval [gets $fid]}
			manCloseText $w
			if [catch {close $fid} errmsg] {
				winstderr $wi $errmsg
			} else {
				set key $name; if {$sect!=""} {append key "($sect)"}
				set manx(hv$w) [set manx(manfull$w) $key]
				set manx(name$w) $name
				set manx(section) $sect
				manShowManFoundSetText $w
				manShowManStatus $w
			}
		}
	}

}

}



foreach kill $killprocs {
	proc $kill args {}
}


### miscellaneous fixups
manManpathSet
manManpathCheck
set mani(manList) $man(manList)
set mani(manTitleList) $man(manTitleList)
set mani([set manx(glimpsestrays) ""],dirs) {}
