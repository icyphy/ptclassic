# 
# Copyright (c) 1995-1996 The Regents of the University of California.
# All rights reserved.
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
#
#
# mkPtolemyTree: A shell script to build custom Ptolemy trees
#
# Version: @(#)mkPtolemyTree.tcl	1.26 09/26/96 
#
# Author: Jose Luis Pino
# Date: 10/20/95
#
# Port to Tcl by Matt Tavis and Brian L. Evans.
#
# We rely on 'mkdir -p' to create all the parent directories for
# a given path name.  The '-p' option is supported on the platforms
# we build for Ptolemy 0.6, and we use it in $PTOLEMY/src/kernel/Target.cc 
# and $PTOLEMY/src/domains/cg/kernel/CGUtilities.cc.  However, the
# '-p' option is not support on, for example, Ultrix 4.1.  See
# $PTOLEMY/src/gnu/src/make/make-stds.texi.  -cxh

proc replaceLinkWithDir {pathname linkspec} {
    if { [file type $pathname] == "link" } {
	puts "Creating $pathname"
	exec sh -c "rm $pathname ; mkdir -p $pathname ; ln -s $linkspec $pathname"
    } elseif { [file isdirectory $pathname] } {
	puts "The directory $pathname already exists"
    } else {
	puts "$pathname is neither a link nor a directory"
    }
}

proc processDirectory {mydir croot root} {
    set stddir "$mydir"
    regsub -all $croot "$stddir" $root stddir
    puts "Expanding $mydir"
    set tarFiles ""
    set files [glob -nocomplain "$stddir/*" "$stddir/.*"]
    foreach pathname $files {
	# extract the file name from the path name
	set file [file tail $pathname]

	# do not process junk files
	# -- skip core and TAGS files
	# -- skip files beginning with '#' or ',' and
	# -- skip files ending with ~
	if { [regexp {core|TAGS|(^#)|(^,)|(~$)} $file] } continue

	# if the file is not readable, then tell user and get next file
	if { ! [file readable "$pathname"] } {
	    puts "$pathname is not readable (skipped)"
	    continue
	}

	# if the file is a directory, then set a symbolic link to it;
	# otherwise, copy file unless it's executable (actually, we want
	# to avoid copying binaries but we do want to copy shell scripts;
	# however, Tcl does not have a convenient way of telling the
	# difference between an ascii and binary file).
	if { [file isdirectory "$stddir/$file"] } {
	    if {! [file exists "$mydir/$file"]} {		# prevent error
		exec ln -s "$stddir/$file" "$mydir/$file"
	    }
	} elseif {! [file executable "$stddir/$file"] } {
	    lappend tarFiles $file
	}
    }

    if { "$tarFiles" != "" } {
	exec sh -c "cd $stddir ; tar cf - $tarFiles | (cd $mydir ; tar xf -)"
    }
}


# The heart of the script
# We use a proc here rather than putting it all at the top level
# so that we can easily debug
proc mkPtolemyTree {override croot root ptarch} {
    # tell the user what is going on
    puts "Making a customized Ptolemy development tree using the version of"
    puts "Ptolemy installed in the directory $root"
    puts "The new customized Ptolemy tree will go in $croot"
    puts "Using $override for $ptarch"

    # here are some libraries which are in lib.$PTACH but do not get built
    # often so, we just make a sym link to them
    set LIBDIR "$root/lib.$ptarch"
    set AUX_LIBRARIES "$LIBDIR/libCGCrtlib.* $LIBDIR/libcephes.*"

    ### The next few lines are all part of MAKEFILE value
    # We can't use echo -n here, as it is not portable.
    # Under solaris2.4 /usr/bin/echo does not recognize -n
    # Note that the text below needs to start at the left margin.
    # Make will barf if there is white space before the rules.
    set MAKEFILE "
TK=1
OCT=1
ROOT=$root
CROOT=$croot
PTARCH=$ptarch
# Include config-\$PTARCH.mk so that we get platform/domain inter dependencies
include $root/mk/config-\$(PTARCH).mk
include $override
include $root/mk/stars.mk

TREE:
	echo \$(CUSTOM_DIRS)
"

    ### Here ends the setting of MAKEFILE value

   
    if [catch {exec echo "$MAKEFILE" | make -s -f - } errMsg] {
	puts stderr "mkPtolemyTree Error: make failed: $errMsg\n Makefile was:"
	puts stderr "$MAKEFILE"
	exit 
    } else {
	set srcdirs $errMsg
    }

    set objdirs $srcdirs
    regsub -all "/src/" $objdirs "/obj.$ptarch/" objdirs


    puts "Creating obj directories"
    foreach mdir $objdirs {
	if [catch {exec mkdir -p "$mdir"} errMsg] {
	    puts "mkPtolemyTree Error: mkdir -p $mdir failed: $errMsg"
	    puts $objdirs
	    exit
	}
    }
    puts "Creating src directories"
    foreach mdir $srcdirs {
	if [catch {exec mkdir -p "$mdir"} errMsg] {
	    puts "mkPtolemyTree Error: mkdir -p $mdir failed: $errMsg"
	    puts $objdirs
	    exit
	}
    }

    
    # process each directory, src directories before obj directories so that
    # the make.template and makefile symlinks are done correctly
    foreach dir [exec find $croot -type d -print | sort -r] {
	set dir [string trim $dir]
	if { [regexp "/obj\.$ptarch/" $dir] } {
	    set srcdir $dir
	    regsub -all "/obj\.$ptarch/" $srcdir "/src/" srcdir
	    if { [file exists "$srcdir/makefile"] } {
		exec ln -s "$srcdir/makefile" "$dir/makefile"
	    }
	    if { [file exists "$srcdir/make.template"] } {
		exec ln -s "$srcdir/make.template" "$dir/make.template"
	    }
	}
	processDirectory $dir $croot $root
    }

    exec rm -f $croot/obj.$ptarch/makefile
    exec ln -s $root/src/makefile $croot/obj.$ptarch/makefile

    exec rm -f $croot/MAKEARCH
    exec cp $root/MAKEARCH $croot/MAKEARCH

    puts "Copying $override to $croot/mk/override.mk"
    exec cp $override $croot/mk/override.mk
    
    set over [open "$croot/obj.$ptarch/pigiRpc/override.mk" w]
    puts $over \
	    "include \$(ROOT)/mk/override.mk\nPTOLEMY=$croot\nPIGI=pigiRpc\n"
    close $over

    set over [open "$croot/obj.$ptarch/ptcl/override.mk" w]
    puts $over "include \$(ROOT)/mk/override.mk\nPTOLEMY=$croot\nPIGI=ptcl\n"
    close $over

    set over [open "$croot/obj.$ptarch/tysh/override.mk" w]
    puts $over "include \$(ROOT)/mk/override.mk\nPTOLEMY=$croot\nPIGI=tysh\n"
    close $over

    set over [open "$croot/obj.$ptarch/pigiExample/override.mk" w]
    puts $over "include \$(ROOT)/mk/override.mk\nPTOLEMY=$croot\nPIGI=tysh\n"
    close $over

    # Remove the symbolic links for bin, bin.$ptarch, and lib.$ptarch,
    # create these directories, and set up initial symbolic links
    replaceLinkWithDir "$croot/bin" "$root/bin/*"
    replaceLinkWithDir "$croot/bin.$ptarch" "$root/bin.$ptarch/*"
    replaceLinkWithDir "$croot/lib" "$root/lib/*"
    replaceLinkWithDir "$croot/lib.$ptarch" "$AUX_LIBRARIES"

    # We need to make sources to build any makefiles that are out of date
    # and to make install to compile any out-of-date object files
    puts "Building ptcl, pigiRpc, and tycho in your customized Ptolemy tree."
    puts "Please wait for a few minutes ..."
    exec sh -c "cd $croot/obj.$ptarch ; make sources install"
    puts "Before using your new pigiRpc, be sure to set \$PTOLEMY to $croot" 

}


# Process the args we were called with and then call mkPtolemyTree
proc processArgs {} {
    global argc argv env
    set error 0
    if {$argc != 2} {
	puts "mkPtolemyTree: wrong number of arguments"
	set error 1
    } elseif {! ([file readable [lindex $argv 0]] && \
	    ("file" == [file type [lindex $argv 0]]))} {
	puts "mkPtolemyTree: override.mk file unreadable"
	set error 1
    } elseif {[file exists [lindex $argv 1]]} {
	puts "mkPtolemyTree: Custom root [lindex $argv 1] already exists"
	set error 1
    } elseif {[string range [lindex $argv 1] 0 0] != "/"} {
	puts "mkPtolemyTree: [lindex $argv 1] must be expressed as an absolute path"
	set error 1
    }    
    if {$error} {
	puts "Usage: mkPtolemyTree <override.mk file> <root path of new tree>"
	puts " The override file is a set of make directives.  By convention,"
	puts " the override file is named \"override.mk\".  The override file"
	puts " gets copied into the new tree.  Note that the final"
	puts " sub-directory in the absolute path name must not already"
	puts " exist.  It will be created by the script."
	puts "\n A sample override.mk file that will build a VHDL only"
	puts " pigi is below."
	puts "VHDL=1"
	puts "DEFAULT_DOMAIN=VHDL"
	puts "VERSION_DESC=\"VHDL only\""
	puts "\n For a complete list of domains that can be included,"
	puts " see \$PTOLEMY/mk/ptbin.mk."
	puts "\n Chapter 1 of the Ptolemy Programmer's manual has"
	puts " full documentation for mkPtolemyTree"
	exit
    }
    mkPtolemyTree [lindex $argv 0] [lindex $argv 1] $env(PTOLEMY) $env(PTARCH) 
}


# If we are not in an interactive tclsh, then call processArgs
# By doing this, we make it possible to source this file and then
# call mkPtolemyTree by hand for testing
if {$tcl_interactive != 1 } {
    processArgs
}
