 #########################################################################
 #                                                                       #
 # Copyright (C) 1993 by General Electric company.  All rights reserved. #
 #                                                                       #
 # Permission to use, copy, modify, and distribute this                  #
 # software and its documentation for any purpose and without            #
 # fee is hereby granted, provided that the above copyright              #
 # notice appear in all copies and that both that copyright              #
 # notice and this permission notice appear in supporting                #
 # documentation, and that the name of General Electric not be used in   #
 # advertising or publicity pertaining to distribution of the            #
 # software without specific, written prior permission.                  #
 #                                                                       #
 # General Electric makes no representations about the suitability of    #
 # this software for any purpose.  It is provided ``as is''              #
 # without express or implied warranty.                                  #
 #                                                                       #
 # This work was supported in part by the DARPA Initiative in Concurrent #
 # Engineering (DICE) through DARPA Contracts MDA972-88-C-0047 and       #
 # MDA972-92-C-0027.                                                     #
 #                                                                       #
 # This work was supported in part by the Tri-Services Microwave and     #
 # Millimeter-Wave Advanced Computational Environment (MMACE) program    #
 # under Naval Research Laboratory contract N00014-92-C-2044.            #
 #                                                                       #
 #########################################################################


# File: fileselect.tcl
#
# Description:
#	fileselect -- display a file selection dialog and get the user's reply
#
# Global variables:
#c	fileselect_priv(command,$w)
#		Command to run when a file is selected in the dialog box $w
#c	fileselect_priv(dirname,$w)
#		Directory that is displayed in the dialog box $w
#c	fileselect_priv(dirvariable,$w)
#		Global variable in which $w's current directory name is
#		to be stored.
#c	fileselect_priv(filter,$w)
#		Function used to select only certain files for display in
#		dialog box $w.
#c	fileselect_priv(new,$w)
#		Boolean variable == 1 if new files may be created by file
#		selection in $w, 0 of only existing files are acceptable.
#c	fileselect_priv(pattern.$w)
#		Pattern restricting the files to select in $w.
#c	fileselect_priv(textvariable,$w)
#		Variable in which to store the name of the file selected in $w.

# $Id$
# $Source$

 # orig Id: fileselect.tcl,v 1.11 1993/11/01 18:20:46 kennykb Exp $
 # orig Source: /homedisk/julius/u0/kennykb/src/tkauxlib_ship/RCS/fileselect.tcl,v $
 # orig Log: fileselect.tcl,v $
 # Revision 1.11  1993/11/01  18:20:46  kennykb
 # Beta release to be announced on comp.lang.tcl
 #
 # Revision 1.10  1993/10/27  15:52:49  kennykb
 # Package for alpha release to the Net, and for MMACE 931101 release.
 #
 # Revision 1.9  1993/10/20  19:10:47  kennykb
 # Alpha release #1 was thawed for bug fixes in tk 3.3.  Now frozen again at this
 # point.
 #
 # Revision 1.8  1993/10/20  18:43:20  kennykb
 # Repaired copyright notice so that it doesn't look like structured commentary.
 #
 # Revision 1.7  1993/10/14  18:15:42  kennykb
 # Cleaned up alignment of log messages, to avoid problems extracting
 # structured commentary.
 #
 # Revision 1.6  1993/10/14  18:06:59  kennykb
 # Added GE legal notice to head of file in preparation for release.
 #
 # Revision 1.5  1993/10/14  14:02:02  kennykb
 # Alpha release #1 frozen at this point.
 #
 # Revision 1.4  1993/10/14  13:34:40  kennykb
 # Changed to use widget_waitVariable to avoid hangs when the application
 # is destroyed.
 #
 # Revision 1.3  1993/07/20  19:17:12  kennykb
 # Improved structured comments.
 # Changed modules through `g' in the alphabet to follow `:' and `_' naming
 # conventions.
 #
 # Revision 1.2  1993/07/16  15:58:00  kennykb
 # Renamed all commands that start with `wiget.' to either `widget_' or
 # `widget:'.  Added the code for creating composite widgets.
 #
 # Revision 1.1  1993/06/03  15:27:42  kennykb
 # Initial revision
 #

# Procedure: fileselect
#
# Synopsis:
#	Display a file selection dialog box.
#
# Usage:
#c	fileselect pathName ?-option value?...
#
# Parameters:
#c	pathName
#		Path name of the file selection dialog box, which will be
#		created as a transient window for its parent.
#
# Options:
#	Name:			action
#	Class:			Action
#	Command-line Switch:	-a, -action
#	Default:		`OK'
#		Name of the button that selects the file.
#	Name:			command
#	Class:			Command
#	Command-line Switch:	-c, -command
#	Default:		None.
#		If nonempty, `command' is the text of a Tcl command to run
#		when a file is selected.  The file name is appended to the
#		command.
#	Name:			directory
#	Class:			Directory
#	Command-line Switch:	-d, -directory
#	Default:		`.'
#		Directory at which to begin the search.
#	Name:			dirVariable
#	Class:			DirVariable
#	Command-line switch:	-dv, -dirvariable
#	Default:		None.
#		Name of a global variable in which to store the name of the
#		working directory of the dialog.  If null, no global variable
#		is used.  If this option and `-directory' are both present,
#		and the variable is already set, the variable's value takes
#		precedence over the `-directory' option.  In this way, a user
#		can maintain a `working directory' across several invocations
#		of `fileselect'.
#	Name:			filter
#	Class:			Filter
#	Command-line switch:	-filter, -f
#	Default:		`fileselect:filter'
#		Tcl function that selects a subset of files to display.
#		The dialog will display only those files that cause the
#		specified function to return a true value when passed
#		the file name.  The default is `fileselect:filter', which
#		returns a true value if a file exists, and a false value
#		otherwise.
#	Name:			message
#	Class:			Message
#	Command-line switch:	-message, -m
#	Default:		`Select a file:'
#		Message to be displayed on the top line of the dialog box.
#	Name:			new
#	Class:			New
#	Command-line switch:	-new, -n
#	Default:		0
#		Boolean value. 1 if a new file is acceptable, and 0
#		if only a pre-existing file is OK.
#	Name:			pattern
#	Class:			Pattern
#	Command-line switch:	-pattern, -p
#	Default:		{*}
#		List of items to match against file names.  Only file names
#		that match at least one of the specified items will be
#		displayed.  The items are matched according to the rules
#		for `string match'.
#	Name:			textVariable
#	Class:			TextVariable
#	Command-line switch:	-textvariable, -textv
#	Default:		None.
#		Global variable in which to store the name of the selected
#		file.  If null, the file name is not stored.
#
# Return value:
#	Path name of the dialog box, suitable for packing, use with
#	`transient', and so on.
#
# Description:
#	`fileselect' displays a file selection dialog box, with a Motif-like
#	look and feel.
#
# Bugs:
#	- There is no way to configure a fileselect dialog to select
#	  a directory.  This should be changed.
#	- There should be control over whether the selection is exported.
#	- There should be a `Rescan' operation.
#	- The `fileselect' is not a first class widget; it does not
#	  respond to the `config' widget command.

 # Default option settings:

option add *Fileselect.action OK widgetDefault
option add *Fileselect.command {} widgetDefault
option add *Fileselect.directory . widgetDefault
option add *Fileselect.dirVariable {} widgetDefault
option add *Fileselect.filter fileselect:filter widgetDefault
option add *Fileselect.message "Select a file:" widgetDefault
option add *Fileselect.new 0 widgetDefault
option add *Fileselect.pattern "*" widgetDefault
option add *Fileselect.textVariable {} widgetDefault
	# Place button text in resources so that the user can customize it.
option add *Fileselect.b.p.b.text "Pop" widgetDefault
option add *Fileselect.b.c.b.text "Cancel" widgetDefault
option add *Fileselect.d.head.text "Directory:" widgetDefault
option add *Fileselect.f.head.text "File name:" widgetDefault
option add *Fileselect.b.o.borderWidth 2 widgetDefault
option add *Fileselect.b.o.relief sunken widgetDefault
option add *Fileselect.f.entry.Font -*-courier-medium-r-*-120-* widgetDefault
option add *Fileselect.f.entry.relief sunken widgetDefault
option add *Fileselect.l.list.relief sunken widgetDefault
	# The indented geometry options are in the following list because the
	# packer appears to override the non-indented ones.
option add *Fileselect.e.height 20 widgetDefault
	option add *Fileselect.e.geometry 1x20 widgetDefault
option add *Fileselect.f.r.width 40 widgetDefault
	option add *Fileselect.f.r.geometry 40x1 widgetDefault
option add *Fileselect.l.l.width 40 widgetDefault
	option add *Fileselect.l.l.geometry 40x1 widgetDefault
option add *Fileselect.l.r.width 40 widgetDefault
	option add *Fileselect.l.r.geometry 40x1 widgetDefault
option add *Fileselect.x.l.width 40 widgetDefault
	option add *Fileselect.x.l.geometry 40x1 widgetDefault
option add *Fileselect.x.r.width 60 widgetDefault
	option add *Fileselect.x.r.geometry 40x1 widgetDefault
	# Following option will be replaced someday with `width' and `height'
option add *Fileselect.l.list.geometry 32x10 widgetDefault
option add *Fileselect*info*background #87cefa widgetDefault
option add *Fileselect*info*foreground #af0000 widgetDefault

proc fileselect {w args} {
	global fileselect_priv

	# Make the window

	frame $w -class Fileselect
	widget_addBinding $w Destroy "fileselect:destroy $w"

	# Defaults

	set action [option get $w action Action]
	set command [option get $w command Command]
	set dirname [option get $w directory Directory]
	set dirvar [option get $w dirVariable DirVariable]
	set filter  [option get $w filter Filter]
	set message [option get $w message Message]
	set newfile [option get $w new New]
	set pattern [option get $w pattern Pattern]
	set textvar [option get $w textVariable TextVariable]

	# Parse command line

	while {[llength $args] > 0} {
		set string [lindex $args 0]
		set args [lrange $args 1 end]
		case $string in {
			{-a -action} {
				set action [lindex $args 0]
				set args [lrange $args 1 end]
			}
			{-c -command} {
				set command [lindex $args 0]
				set args [lrange $args 1 end]
			}
			{-d -directory} {
				set dirname [lindex $args 0]
				set args [lrange $args 1 end]
			}
			{-dv -dirvariable} {
				set dirvar [lindex $args 0]
				set args [lrange $args 1 end]
			}
			{-f -filter} {
				set filter [lindex $args 0]
				set args [lrange $args 1 end]
			}
			{-m -message} {
				set message [lindex $args 0]
				set args [lrange $args 1 end]
			}
			{-n -new} {
				set newfile [lindex $args 0]
				set args [lrange $args 1 end]
			}
			{-p -pattern} {
				set pattern [lindex $args 0]
				set args [lrange $args 1 end]
			}
			{-textv -textvariable} {
				set textvar [lindex $args 0]
				set args [lrange $args 1 end]
			}
			default {
				destroy $w
				error "$string: unknown option"
			}
		}
	}

	# Store options in globals

	set fileselect_priv(command,$w) $command
	set fileselect_priv(dirname,$w) $dirname
	set fileselect_priv(dirvariable,$w) $dirvar
	set fileselect_priv(filter,$w) $filter
	set fileselect_priv(new,$w) $newfile
	set fileselect_priv(pattern,$w) $pattern
	set fileselect_priv(textvariable,$w) $textvar

	# Create the subwindows

	# Prompt message

	label $w.m -text $message

	# Directory name

	frame $w.d
	label $w.d.head
	label $w.d.name
	pack append $w.d $w.d.head {left} $w.d.name {left}

	# Entry for file name

	frame $w.f
	label $w.f.head
	entry $w.f.entry
	frame $w.f.r

	pack append $w.f \
		$w.f.head {left} \
		$w.f.entry {left expand fill} \
		$w.f.r {left}

	# Pad

	frame $w.e

	# Scrollbar for the file list

	frame $w.x
	frame $w.x.l
	scrollbar $w.x.s -command "$w.l.list xview" -orient horizontal
	frame $w.x.r
	pack append $w.x $w.x.l {left} $w.x.s {left expand fillx} $w.x.r {left}

	# Listbox to show files

	frame $w.l
	frame $w.l.l
	scrollbar $w.l.scroll -command "$w.l.list yview"
	listbox $w.l.list -yscroll "$w.l.scroll set" -xscroll "$w.x.s set"
	frame $w.l.r
	pack append $w.l \
		$w.l.l {left} \
		$w.l.list {left expand fill} \
		$w.l.scroll {left filly} \
		$w.l.r {left}

	tk_listboxSingleSelect $w.l.list

	bind $w.l.list <Any-ButtonRelease-1> {
		fileselect:copyEntry [winfo parent [winfo parent %W]] \
				[fileselect_slash [%W get [%W nearest %y]]]
	}
	bind $w.l.list <Double-Button-1> {
		fileselect:select [winfo parent [winfo parent %W]] \
				[fileselect_slash [%W get [%W nearest %y]]]
	}

	# Buttons

	frame $w.b
	focusable button $w.b.o -command "fileselect:ok $w" \
		-text $action
	focusable button $w.b.p -command "fileselect:pop $w"
	focusable button $w.b.c -command "fileselect:cancel $w"
	pack append $w.b $w.b.o {left expand frame center} \
		$w.b.p {left expand frame center} \
		$w.b.c {left expand frame center}

	pack append $w \
			$w.m {top expand frame w} \
			$w.d {top expand fillx} \
			$w.f {top expand fillx} \
			$w.e {top} \
			$w.l {top expand fill} \
			$w.x {top expand fillx} \
			$w.b {top expand fillx pady 10 frame center}

	# Fill in the default file and directory name
	if [catch {$filter $dirname} dirinfo] {
		set dirinfo "directory ./"
	}

	if {$textvar != ""} {
		upvar #0 $textvar tv
		if {[info exists tv]} {
			$w.f.entry delete 0 end
			$w.f.entry insert 0 [file tail $tv]
			entry_cursor $w.f.entry
		}
	}
	if {$dirvar != ""} {
		upvar #0 $dirvar dv
		if {[info exists dv] && $dv != ""} {
			if {![catch {$filter $dv} dirinfo2]
			  && [lindex $dirinfo2 0]=="directory"] } {
				set dirinfo dirinfo2
			}
		}
	}

	# Move into the specified directory

	after 1 fileselect:dir $w [list $dirinfo]
}

# Procedure:	fileselect:cancel
#
# Synopsis:
#	Cancel a file selection request.
#
# Usage:
#c	fileselect:cancel pathName
#
# Parameters:
#c	pathName
#		Path name of a file selecction dialog
#
# Return value:
#	None.
#
# Description:
#	`fileselect:cancel' is invoked when the user presses the `Cancel'
#	button in a file selection dialog.  It sets the text variable to
#	the null string, executes the selection command giving the null string
#	as an argument, and returns.

proc fileselect:cancel {w} {
	global fileselect_priv
	if {$fileselect_priv(textvariable,$w) != ""} {
		upvar #0 $fileselect_priv(textvariable,$w) tv
		set tv {}
	}
	if {$fileselect_priv(command,$w) != ""} {
		uplevel #0 $fileselect_priv(command,$w) {}
	}
}

# Procedure:	fileselect:copyEntry
#
# Synopsis:
#	Select an entry from the listbox in a file selection dialog.
#
# Usage:
#c	fileselect:copyEntry pathName text
#
# Parameters:
#c	pathName
#		Path name of a file selection dialog box
#c	text
#		File name.
#
# Return value:
#	None.
#
# Description:
#	`fileselect:copyEntry' is invoked in response to a change of the
#	selection within a file selection dialog box.  It copies
#	the current selection (given as the second parameter) to the
#	entry box so that the user can edit it if desired.

proc fileselect:copyEntry {w text} {
	catch {$w.f.entry delete 0 end}
	catch {$w.f.entry insert 0 $text ; entry_cursor $w}
}

# Procedure:	fileselect:destroy
#
# Synopsis:
#	Clean up when a file selection dialog is destroyed.
#
# Usage:
#c	fileselect:destroy pathName
#
# Parameters:
#c	pathName
#		Path name of a file selection dialog box.
#
# Return value:
#	None.
#
# Description:
#	`fileselect:destroy' is invoked when a file selection dialog box is
#	destroyed.  It removes all the private variables associated with the
#	dialog.

proc fileselect:destroy w {
	global fileselect_priv
	set status [catch {array names fileselect_priv} names]
	if {$status == 0} {
		foreach name $names {
			if [string match *,$w $name] {
				unset fileselect_priv($name)
			}
		}
	}
}

# Procedure:	fileselect:dir
#
# Synopsis:
#	Traverse to a new directory in a file selection dialog.
#
# Usage:
#c	fileselect:dir pathName directory
#
# Parameters:
#c	w
#		Path name of a file selection dialog box.
#c	dirinfo
#		A list of the form {directory userdirname osdirname}
#		where "directory" is literal, "userdirname" is the
#		name of the directory suitable for user consuption,
#		and "osdirname" is a name suitable for the operating system.
#		dirinfo may be either relative or absolute to the
#		current directory of the fileselect box.
#
# Return value:
#	None.
#
# Description:
#	`fileselect:dir' is invoked when the directory on display in a file
#	selection dialog box changes.  It scans the files in the new directory,
#	matches them against the pattern and filter, and rebuilds the list
#	box.  It locks the application in a modal dialog while scanning,
#	in order to avoid spurious selections.

proc fileselect:dir {w dirinfo} {
	global fileselect_priv

	set olddirname [fileselect_slash $fileselect_priv(dirname,$w)]
	set dirname [fileselect_slash [lindex $dirinfo 1]]
	set osdirname [fileselect_slash [lindex $dirinfo 3]]

	if 0 {
		set status [catch {exec /bin/csh -cef "cd $osdirname. && pwd"} dirname]
		if {$status} {
			errormessage $w.error $dirname
			return
		}
	} elseif 0 {
		set oldpwd [pwd]
		if [catch {cd $osdirname; pwd} dirname] {
			errormessage $w.error $dirname
			return
		}
		if [catch {cd $oldpwd} foo] {
			errormessage $w.error \
			  "Couldn't switch back to old dir ``$oldpwd'': $foo"
		}
	}
	
	modalDialog transient label $w.info -text "Scanning $dirname"
	set fileselect_priv(dirname,$w) $dirname
	set dirvar $fileselect_priv(dirvariable,$w)
	if {$dirvar != ""} {
		upvar #0 $dirvar dv
		set dv $dirname
	}

	$w.d.name config -text $dirname
	catch {$w.f.entry delete 0 end}
	catch {$w.l.list delete 0 end}
	set status [catch {split [exec ls -a $osdirname] \n} fileList]
	if {$status == 0} {
		eval fileselect:format {$w} {$w.l.list} {$dirname} $fileList
	}
	modalDialog.end $w.info
}		



# Procedure:	fileselect:format
#
# Synopsis:
#	Format and filter the list of dirs&files and insert them into the
#	list box.
#
# Usage:
#c	fileselect:format pathName listBoxPath dirname files ...
#
# Parameters:
#c	pathName
#		Path name of a file selection dialog box.
#	listBoxPath
#		Path name of list box widget to update.
#	dirname
#		Name of directory that we are processing, the "files" must
#		be in this directory.
#	files
#		Zero or more files and directories that are the contents
#		of "dirname".
# Description:
#	`fileselect:format' processes the files "files", filters them
#	according to the fileselect box's pattern and filter, and stores
#	them into the listbox.  Some future version may allow the application
#	to override this function; this would allow custom sorting.
# Return value:
#	None.
#
# Description:
proc fileselect:format {w listbox dirname args} {
	global fileselect_priv
	set pattern $fileselect_priv(pattern,$w)
	set filter $fileselect_priv(filter,$w)

	set files ""
	foreach filename $args {
		if [catch {$filter $dirname/$filename} fileinfo] {
			continue
		}
		switch [lindex $fileinfo 0] {
		    directory {
			$listbox insert end [lindex $fileinfo 2]
		    }
		    file {
			foreach p $pattern {
				if [string match $p $filename] {
					lappend files [lindex $fileinfo 2]
					break
				}
			}
		    }
		}
	}
	foreach filename $files {
		$listbox insert end $filename
	}
}


# Procedure:	fileselect:filter
#
# Synopsis:
#	Default filter for a file selection dialog.
#
# Usage:
#c	fileselect:filter fileName
#
# Parameters:
#c	fileName
#		Path name of a file
#
# Return value:
#	A list {fileType formatName} where "fileType" is either "file"
#	or "directory" and "formatName" is how the file or directory
#	should be entered into the listbox.  An error is generated if
#	the file or directory should not be selectable.
#
# Description:
#	fileselect:filter is the default filter function in a file selection
#	dialog box if the user doesn't supply one.  It returns a list
#	as described above.  Directories will be suffixed by a slash.

proc fileselect:filter {filename} {
	if ![file exists $filename] {
		error "No such file ``$filename''."
	} 
	if [file isdirectory $filename] {
		return [list directory $filename [file tail $filename]/ $filename]"
	}
	return [list file $filename [file tail $filename] $filename]
}

# Procedure:	fileselect:ok
#
# Synopsis:
#	Handle the `OK' button in a file selection dialog.
#
# Usage:
#c	fileselect:ok pathName
#
# Parameters:
#c	pathName
#		Path name of a file selection dialog box.
#
# Return value:
#	None
#
# Description:
#	`fileselect:ok' is invoked when the user presses `OK' or double
#	clicks in a file selection dialog box.  It retrieves the file name
#	from the listbox or entry as appropriate, makes sure that a file
#	has been specified, and calls `fileselect:select' to select it.

proc fileselect:ok w {
	set f [$w.f.entry get]
	if {$f == ""} {
		set s [$w.l.list curselection]
		if {[llength $s] == 0} {
			errormessage $w.error "Please select a file name"
			return
		}
		if {[llength $s] > 1} {
			errormessage $w.error "Please select only one file."
			return
		}
		set f [$w.l.list get $s]
	}
	fileselect:select $w $f
}

# Procedure:	fileselect:select
#
# Synopsis:
#	Select a file or directory in a file selection dialog.
#
# Usage:
#c	fileselect:select pathName fileName
#
# Parameters:
#c	pathName
#		Path name of a file selection dialog box
#c	fileName
#		Name of a selected file or directory.
#
# Description:
#	`fileselect:select' is the general function that responds to the
#	`OK' button or to a double click in a file selection dialog box.
#	It is passed the file name being selected.  If the file is a
#	directory, it is scanned and opened.  If it is a plain file,
#	it is selected, the text variable is set, and the selection command
#	is executed.

proc fileselect:select {w filename} {
	global fileselect_priv
	set command $fileselect_priv(command,$w)
	set dir $fileselect_priv(dirname,$w)
	if [catch {fileselect_merge $w $dir $filename} fileinfo] {
		errormessage $w.error $fileinfo
		return
	}
	catch {$w.f.entry delete 0 end}
	switch [lindex $fileinfo 0] {
	    directory {
		fileselect:dir $w $fileinfo
	    }
	    file {
		set osfilename [lindex $fileinfo 3]
		if {!$fileselect_priv(new,$w) && ![file exists $osfilename]} {
			errormessage $w.error "Can't open ``$osfilename''."
			return
		}
		set filename [lindex $fileinfo 1]
		if {$fileselect_priv(textvariable,$w) != ""} {
			upvar #0 $fileselect_priv(textvariable,$w) tv
			set tv $filename
		}
		if {$command != ""} {
			uplevel #0 $command $filename
		}
	    }
	}
}

proc fileselect:pop {w} {
	global fileselect_priv
	set dir $fileselect_priv(dirname,$w)
	set dir [file dirname [fileselect_slash $dir]]
	set fileselect_priv(dirname,$w) $dir
	fileselect:select $w .
}

proc fileselect_merge {w dir1 dir2} {
	global fileselect_priv
	set filter $fileselect_priv(filter,$w)

	if { ![catch "$filter {$dir2}" fileinfo] \
	  && [regexp {^[/~]} [lindex $fileinfo 3]] } {
		# Just discard dir1 completely
		return $fileinfo
	}
	while {"$dir2"!=""} {
		if [regsub {^\.(/|$)} $dir2 {} dir2] {
			# kill leading ``.'' in dir2
			continue
		}
		if [regsub {^\.\.(/|$)} $dir2 {} dir2] {
			switch [file tail $dir1] {
			    .		{ set dir1 [file dirname $dir1]/.. }
			    ..		{ set dir1 $dir1/.. }
			    default	{ set dir1 [file dirname $dir1] }
			}
			continue
		}
		break
	}
#puts stdout "merge: combine ``$dir1'' ``$dir2''"
	if { "$dir2"!="" } {
		append dir1 [expr {"$dir1"=="/" ? "" : "/" }] $dir2
	}
	set dirinfo [eval $filter {$dir1}]
#puts stdout "dirinfo: $dirinfo"
	return $dirinfo
}

# Procedure:	fileselect_slash
#
# Synopsis:
#	Strip a trailing slash from a directory name
#
# Usage:
#c	fileselect_slash fileName
#
# Parameters:
#c	fileName
#		Name of a file or directory, optionally with a trailing slash.
#
# Return value:
#	File name, with the trailing slash removed.
#
# Description:
#	fileselect_slash strips trailing slashes from file names.
#	Note that we must be careful to not strip the slash when its just
#	the root dir ``/''.

proc fileselect_slash f {
	if [regexp {^(.+)/$} $f rubbish leader] {
		set f $leader
	}
	return $f
}

# Procedure: selectfile
#
# Synopsis:
#	Modal file selection dialog.
#
# Usage:
#c	selectFile ?argument...?
#
# Parameters:
#	Parameters are the same as for fileselect, except that the
#	widget path name should NOT be supplied, and the `-command' and
#	`-textvariable' options should not be used.
#
# Return value:
#	Name of the selected file.
#
# Errors:
#c	operation cancelled
#		Self-explanatory.
#
# Description:
#	selectfile provides a simple interface to fileselect, performing the
#	selection in a transient modal dialog.
#
# See also:
#c	fileselect

proc selectfile args {
	global selectfile_priv
	set w [eval modalDialog transient fileselect .fileselect $args \
			-textvariable selectfile_priv]
	widget_waitVariable selectfile_priv
	set selection $selectfile_priv
	unset selectfile_priv
	modalDialog.end $w
	if {$selection != ""} {
		return $selection
	} else {
		error "operation cancelled"
	}
}
