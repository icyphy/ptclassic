# Procedure to make a graph describing a class hierarchy.
#
# @Authors: Edward A. Lee, Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1995-%Q% The Regents of the University of California.
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

# Pick up ::tycho::expandPath
source [ file join $TYCHO kernel Path.tcl]

set classpackagelist { \
::tycho::BuilderDialog    guikernel \
::tycho::ButtonBox        guikernel \
::tycho::CacheManager     guikernel \
::tycho::ColorBrowser     guikernel \
::tycho::ColorManager     guikernel \
::tycho::ColorPicker      guikernel \
::tycho::ControlPanel     guikernel \
::tycho::Dialog           guikernel \
::tycho::DirSearch        guikernel \
::tycho::Displayer        guikernel \
::tycho::Edit             guikernel \
::tycho::EditMail         guikernel \
::tycho::EditSearch       guikernel \
::tycho::EditSpell        guikernel \
::tycho::ErrorMessage     guikernel \
::tycho::File             guikernel \
::tycho::FileBrowser      guikernel \
::tycho::FontDialog       guikernel \
::tycho::FontManager      guikernel \
::tycho::HTML             guikernel \
::tycho::HTMLMessage      guikernel \
::tycho::IndexBrowser     guikernel \
::tycho::ListBrowser      guikernel \
::tycho::MenuBar          guikernel \
::tycho::MenuSupport      guikernel \
::tycho::Message          guikernel \
::tycho::ModelViewer      guikernel \
::tycho::MultiPanel       guikernel \
::tycho::PopupMenu        guikernel \
::tycho::Query            guikernel \
::tycho::Registry         guikernel \
::tycho::Resource         guikernel \
::tycho::ResourceFTP      guikernel \
::tycho::ResourceFile     guikernel \
::tycho::ResourceHTTP     guikernel \
::tycho::ResourceMail     guikernel \
::tycho::ResourceRemote   guikernel \
::tycho::ResourceWELD     guikernel \
::tycho::SimpleView       guikernel \
::tycho::StatusBar        guikernel \
::tycho::StyleChooser     guikernel \
::tycho::StyleEditor      guikernel \
::tycho::StyleSheet       guikernel \
::tycho::Subpanel         guikernel \
::tycho::TWidget          guikernel \
::tycho::ToolBar          guikernel \
::tycho::TopLevel         guikernel \
::tycho::UserProfile      guikernel \
::tycho::View             guikernel \
::tycho::WelcomeMessage   guikernel \
::tycho::YesNoCancel      guikernel \
::tycho::YesNoQuery       guikernel \
::tycho::Compile          ide \
::tycho::EditDiff         ide \
::tycho::EditMake         ide \
::tycho::EditProgram      ide \
::tycho::Exec             ide \
::tycho::Glimpse          ide \
::tycho::Monitor          ide \
::tycho::RevControl       ide \
::tycho::RevControlRCS    ide \
::tycho::RevControlSCCS   ide \
::tycho::EditC            idecpp \
::tycho::EditCpp          idecpp \
HTMLMap2DAG               idehtml \
::tycho::EditHTML         idehtml \
::tycho::CompileCTcl      ideitcl \
::tycho::EditItcl         ideitcl \
::tycho::EditTcl          ideitcl \
::tycho::ItclClassList    ideitcl \
::tycho::TclShell         ideitcl \
::tycho::CompileJava      idejava \
::tycho::EditJava         idejava \
::tycho::EditEsterel      idemisc \
::tycho::EditSDL          idemisc \
::tycho::EditPtcl         ideptolemy \
::tycho::EditPtlang       ideptolemy \
List                      kernel \
Loader                    kernel \
Math                      kernel \
Misc                      kernel \
Path                      kernel \
String                    kernel \
Tycho                     kernel \
URLPath                   kernel \
::tycho::AbstractGraph    kernel \
::tycho::Builder          kernel \
::tycho::CircularList     kernel \
::tycho::DataModel        kernel \
::tycho::Digraph          kernel \
::tycho::DirectedAcyclicGraph kernel \
::tycho::EditStack        kernel \
::tycho::Forest           kernel \
::tycho::Graph            kernel \
::tycho::IconLibrary      kernel \
::tycho::Library          kernel \
::tycho::MetaData         kernel \
::tycho::Model            kernel \
::tycho::Object           kernel \
::tycho::PackageData      kernel \
::tycho::PortedDigraph    kernel \
::tycho::SimpleModel      kernel \
::tycho::Stack            kernel \
::tycho::StarLibrary      kernel \
::tycho::Uninstantiable   kernel \
::tycho::Animator         slate \
::tycho::Axis             slate \
::tycho::Bounder          slate \
::tycho::ComplexItem      slate \
::tycho::DragDropper      slate \
::tycho::Follower         slate \
::tycho::Frame            slate \
::tycho::IconFrame        slate \
::tycho::IconItem         slate \
::tycho::IconOval         slate \
::tycho::IconRect         slate \
::tycho::IncrCanvas       slate \
::tycho::Interactor       slate \
::tycho::LabeledFrame     slate \
::tycho::LabeledItem      slate \
::tycho::LabeledLine      slate \
::tycho::LabeledOval      slate \
::tycho::LabeledRect      slate \
::tycho::Layout           slate \
::tycho::Line             slate \
::tycho::Oval             slate \
::tycho::Point            slate \
::tycho::Polygon          slate \
::tycho::Rectangle        slate \
::tycho::Repeater         slate \
::tycho::Selector         slate \
::tycho::Shades           slate \
::tycho::Shape            slate \
::tycho::Slate            slate \
::tycho::SlateView        slate \
::tycho::Slider           slate \
::tycho::SmartLine        slate \
::tycho::Solid            slate \
::tycho::Stepper          slate \
::tycho::Terminal         slate \
::tycho::TextItemEdit     slate \
::tycho::ControlSubpanel  tycgc \
::tycho::Scheduler        tycgc \
::tycho::Class            tydoc \
::tycho::ClassModel       tydoc \
::tycho::ClassParser      tydoc \
::tycho::ClassViewer      tydoc \
::tycho::DocClass         tydoc \
::tycho::DocComment       tydoc \
::tycho::DocSys           tydoc \
::tycho::HTMLDocSys       tydoc \
::tycho::HierModel        tydoc \
::tycho::ItclParser       tydoc \
::tycho::ItclViewer       tydoc \
::tycho::JavaParser       tydoc \
::tycho::JavaViewer       tydoc \
::tycho::ListDocSys       tydoc \
::tycho::TyDoc            tydoc \
Profile                   typt \
::ptolemy::Block          typt \
::ptolemy::Galaxy         typt \
::ptolemy::Universe       typt \
::tycho::AudioIO          typt \
::tycho::DomainInfo       typt \
::tycho::GraphicEq        typt \
::tycho::Mathematica      typt \
::tycho::Matlab           typt \
::tycho::Oct              typt \
::tycho::ParametricEq     typt \
::tycho::PtWelcomeMessage typt \
::tycho::Retarget         typt \
::tycho::ToneControl      typt \
tychoMakeClassDAG         util \
tychoMakeIndex            util \
tychoMakePackageDAG       util \
::tycho::EditBubbleAndArc vis \
::tycho::EditDAG          vis \
::tycho::EditForest       vis \
::tycho::EditPalette      vis \
::tycho::EditSTD          vis \
::tycho::Graphics         vis \
::tycho::HTMLExport       vis \
::tycho::ProfileTcl       vis \
}

array set classpackagearray $classpackagelist

array set packagecolor [list \
	guikernel red4 \
	ide aquamarine \
	idecpp aquamarine1 \
	idehtml aquamarine2 \
	ideitcl aquamarine3 \
	idejava aquamarine4 \
	idemisc mediumaquamarine \
	ideptolemy seagreen1 \
	kernel red1 \
	slate slateblue \
	tycgc darkseagreen3 \
	tydoc yellow1 \
        typt seagreen2 \
	vis midnightblue \
	{} black \
	]

#### packageColor
# Return the color that corresponds with the package that the nm class
# is in
proc packageColor {nm} {
    global classpackagearray packagecolor
    if [catch {set package $classpackagearray($nm)}] {
	set package $nm
    }
    if [catch {set color $packagecolor($package)}] {
	puts "packageColor: $package"
	return black
    }
    return $color
}

proc dumpPackageKey {fd} {
    puts $fd "\
{add ::typack::kernel {label {kernel Package}\n\
 color [packageColor kernel]} {}}\n\
{add ::typack::guikernel {label {guikernelPackage}\n\
 color [packageColor guikernel]} {::typack::kernel}}\n\
\n\
{add ::typack::ide {label {ide Package}\n\
 color [packageColor ide]} {::typack::kernel}}\n\
{add ::typack::idehtml {label {idehtml Package}\n\
 color [packageColor idehtml]} {::typack::html ::typack::ide}}\n\
{add ::typack::ideitcl {label {ideitcl Package}\n\
 color [packageColor ideitcl]} {::typack::tclshell ::typack::ide}}
{add ::typack::idejava {label {idejava Package}\n\
 color [packageColor idejava]} {::typack::ide}}\n\
{add ::typack::ideptolemy {label {ideptolemy Package}\n\
 color [packageColor ideptolemy]} {::typack::ide}}\n\
\n\
{add ::typack::vis {label {vis Package}\n\
 color [packageColor vis]} {::typack::kernel}}\n\
{add ::typack::textedit {label {textedit Package}\n\
 color [packageColor textedit]} {::typack::kernel}}\n\
{add ::typack::html {label {html Package}\n\
 color [packageColor html]} {::typack::kernel}}\n\
{add ::typack::ptolemy {label {ptolemy Package}\n\
 color [packageColor ptolemy]} {::typack::ideptolemy}}\n\
{add ::typack::tycgc {label {tycgc Package}\n\
 color [packageColor tycgc]} {::typack::ptolemy}}\n\
{add ::typack::tydoc {label {tydoc Package}\n\
 color [packageColor tydoc]} {::typack::kernel}}\n\
{add ::typack::idoc {label {idoc Package}\n\
 color [packageColor idoc]} {::typack::tydoc}}\n\
{add ::typack::slate {label {slate Package}\n\
 color [packageColor slate]} {::typack::kernel}}\n\
{add ::typack::misc {label {misc Package}\n\
 color [packageColor misc]} {::typack::kernel}}\n\
{add ::typack::weld {label {weld Package}\n\
 color [packageColor weld]} {::typack::kernel}}\n\
"

}
#### tychoMkClassGraph
# Make a class graph of the type displayed by the Tycho EditDAG class.
# The first argument is the name of the graph. The second argument is
# the name of the graph file to create. The rest of the arguments are
# any number of file names from which the graph should be created.
# If any of these files is not readable, it is ignored.
# These file names should be absolute (or relative to an environment
# variable like TYCHO) so that the resulting hyperlinks work from any
# directory. To create a graph for the entire Tycho tree, use the
# procedure <code>tychoStandardClasses</code>.
#
# We could have tydoc generate this information, but it would be much slower.
#
proc tychoMkClassGraph {title filename args} {
    #puts "tychoMkClassGraph '$title' '$filename' "
    set entries {}
    set retval {}
    set classexp "\n\[ \t\]*class\[ \t\]+(\[^\{ \t\n\]*)"
    set inheritexp "\n\[ \t\]*inherit\[ \t\]+(\[^\n\]*)"
    set procexp "\n\[ \t\]*proc\[ \t\]+"
    set nonClassFileList {}

    foreach file $args {
        #puts $file
        update
	if {$file == {} } {
	    continue
	}
	set ffn [::tycho::expandPath $file]
        if {![file readable $ffn] || [file isdirectory $ffn]} {
            continue
        }
	set fd [open $ffn r]
 	if [catch {set contents [read $fd]} errMsg] {
            error "Error while reading $file:\n$errMsg"
        }
	close $fd

        set nm {}
	set classEnd 0

	# Read through the file, looking for the start of classes.
	# Each time we find the start of a class, we save the location
	# and then the next time through, start the search after
	# the start of the last class so that we pick up the next class
	
	while {[regexp -indices $classexp \
		[string range $contents $classEnd end] \
		classStartIndices nm] != 0} {
	    # FIXME: it is just easier to get the name by running regexp
	    # again.
	    regexp $classexp [string range $contents \
		    $classEnd end] \
		    matchvar nm
	    set classEnd [expr {[lindex $classStartIndices 1] + \
		    $classEnd}]
            set classfile($nm) $file
	    set htmlfile($nm) [file join [file dirname $file] \
		    doc codeDoc [info namespace tail $nm].html]
	    #puts "$nm $classStartIndices $classEnd \
	    #    [string range $contents $classEnd [expr {$classEnd+40}]]"

	    # Look for inheritance only if a class definition was found.
	    if {$nm != {}} {
		if {[regexp -indices $inheritexp \
			[string range $contents $classEnd end] \
			matchvar inheritIndices] != 0} {
		    # Check for the case where we have multiple classes
		    # in a file, but the first class has no parents.
		    # If the indices of the inherits clause are greater
		    # than the indices of the next class clause, then
		    # the current class has no parents, as the inherit
		    # clause we found belongs to the next class clause.
		    
		    if {[regexp -indices $classexp \
			    [string range $contents $classEnd end] \
			    classStartIndices tnm] != 0 } {
			if {[lindex $classEnd 0] < \
				[lindex $inheritIndices 0]} {
			    #puts "continuing: $nm $classEnd"
			    set parent($nm) {}
			    continue
			}
		    }

		    # FIXME: it is just easier to get the name by running
		    # regexp again.
		    regexp $inheritexp \
			[string range $contents $classEnd end] \
			matchvar pnm
		    if {$nm == $pnm} {
			#puts "In $file, $nm cannot be its own parent"
			set parent($nm) {}

		    } else {
			#puts "nm=$nm, pnm=$pnm"
			set parent($nm) {}
			set parentList [split $pnm]
			foreach parentElement $parentList {
			    if {$parentElement != {}} {
				lappend parent($nm) $parentElement
			    }
			}
		    }

		} {
		    set parent($nm) {}
		}
	    }
        }
	if {$classEnd == 0 } {
	    # If we are here, then this file does not define a class,
	    # so we check to see if it defines any procs.  If it does
	    # we add it to the list of files that define procs but not classes.
	    
	    if {[regexp $procexp $contents] != 0} {
		lappend nonClassFileList $file
	    }
	}
    }
    dumpClassDAG $title $filename classfile parent htmlfile $nonClassFileList
    return $retval
}

#### dumpClassDAG
# Dump the class DAG to filename as a Tycho DAG format file
#
proc dumpClassDAG {title filename \
        classfileArray parentArray htmlfileArray nonClassFileList} {

    upvar $classfileArray classfile $parentArray parent htmlfileArray htmlfile
    set fd [open $filename w]
    #puts "dumpClassDAG: [pwd] $filename '$title'"

    # Put in titles and a reasonable default size.
    puts $fd "\{configure -canvasheight 600\} \{configure -canvaswidth 800\}"
    puts $fd "\{titleSet title \{$title\}\}"
    puts $fd "\{titleSet subtitle \{created:\
            [clock format [clock seconds]]\}\}"
    dumpPackageKey $fd

    foreach nm [array names classfile] {
        set pnt $parent($nm)
        set rxp "\[ \t\]*class\[ \t\]+$nm"
        # Remove namespace information.
        # FIXME: This should be encoded to show up somehow.
        set sname $nm
        regexp "::(\[^: \t\]*)\$" $nm match sname
	set color "color [packageColor $nm]"
        if [info exists htmlfile($nm)] {
            puts $fd "\{add $nm \{label \{$sname\} altlink \{$classfile($nm)\
                \{$rxp\}\} link \{$htmlfile($nm) \{\}\} $color\} \{$pnt\}\}"
        } else {
            puts $fd "\{add $nm \{label \{$sname\} link \{$classfile($nm) \{\}\} $color\} \{$pnt\}\}"
        }
    }
    if {$nonClassFileList != {} } {
        # Dump the files that don't define classes, but do define procs

        # Build the parent node
        #puts "nonClassFileList: $nonClassFileList"
        puts $fd "\{add nonClassFilesNode \{label \{Non-Class Files\} \} \{\}\}"
        set pnt "nonClassFilesNode"

        # Traverse the list, and determine what directories have non-class files
        # in them.  Then create child nodes with the directory names.

        set nonClassDirectory(dummy) dummy
        foreach nonClassFile [lsort $nonClassFileList] {
            set dirname [file dirname $nonClassFile]
            if ![info exists nonClassDirectory($dirname)] {
                set label [::tycho::simplifyPath $dirname]
                # If the label is more than 32 chars, only print out the last32
                if {[string length $label] > 32} {
                    set label "...[string range $label \
                            [expr {[string length $label]-32}] end]"
                }
                puts $fd "\{add \{$label\} \{label \{$label\} \} \{$pnt\}\}"
                set nonClassDirectory($dirname) "$label"
            }
        }

        foreach nonClassFile [lsort $nonClassFileList] {
            set nm [file tail [file rootname $nonClassFile]]
            set sname $nm
            set basefile [file rootname [file tail $nonClassFile]]
            set dirname [file dirname $nonClassFile]
            set html [file join $dirname doc codeDoc $basefile.html]
	    set color "color [packageColor $nm]"
            puts $fd "\{add $nm \{label \{$sname\} altlink \{$nonClassFile\
                \{\} $color\} link \{$html \{\}\}\}\
            \{$nonClassDirectory($dirname)\}\}"
            # Save the current node in the slot for the current directory.
	    # This makes the class diagram much wider
	    #set nonClassDirectory($dirname) $nm
        }
    }
    close $fd
}

#### tychoStandardClasses
# Update the Tycho classes graph.
#
proc tychoStandardDAG {} {
    global TYCHO
    set olddir [pwd]
    cd $TYCHO

    # If you update this list, update
    # $TYCHO/editors/textedit/ItclClassList.itcl too.
    set dirs [list \
            kernel \
            [file join editors textedit] \
            [file join editors visedit] \
            [file join editors slate] \
            [file join typt editors ] \
            [file join typt kernel ] \
            [file join typt controls ] \
            [file join lib idx ] \
            [file join lib tydoc ]
    ]

    foreach dir $dirs {
	if [file isdirectory "$dir"] {
	    eval lappend files [glob \
		    [file join $dir {{*.itcl,*.tcl,*.tk,*.itk}}]]
	}
    }
    foreach file $files {
        lappend filesenv [file join "\$TYCHO" $file]
    }
    set retval [eval tychoMkClassGraph {{Tycho Class Hierarchy}} [file join doc tychoPackage.dag] $filesenv ]
    cd $olddir
    return $retval
}
#### cppMkClassGraph
# Make a class graph of the type displayed by the Tycho EditDAG class.
# The first argument is the name of the graph. The second argument is
# the title of the graph file to create. The rest of the arguments are
# any number of file names from which the graph should be created.
# If any of these files is not readable, it is ignored.
# These file names should be absolute (or relative to an environment
# variable like TYCHO) so that the resulting hyperlinks work from any
# directory. To create a graph for the entire Tycho tree, use the
# procedure <code>tychoStandardClasses</code>.
#
# We could have tydoc generate this information, but it would be much slower.
#
proc cppMkClassGraph {title filename args} {
    #puts "cppMkClassGraph: '$title' '$filename' '$args'"
    set entries {}
    set retval {}
    # Don't allow leading whitespace before 'class', or we pick up
    # C style comments, such as the comment in Wormhole.h '    class with'
    set classexp "\nclass\[ \t\]+(\[^ \t\n:;\{\]*)"
    set inheritexp "^\[ \t\]*:\[ \t]*(\[^\{\]*)" 
    set procexp "\n\[ \t\]*proc\[ \t\]+"
    set nonClassFileList {}

    foreach file $args {
        update
        #puts "ptolemyMkClasGraph file==$file"
	if {$file == {} } {
	    continue
	}
	set ffn [::tycho::expandPath $file]
        if {![file readable $ffn] || [file isdirectory $ffn]} {
            continue
        }
	set fd [open $ffn r]
 	if [catch {set contents [read $fd]} errMsg] {
            error "Error while reading $file:\n$errMsg"
        }
	close $fd

        set nm {}
	set classEnd 0

	# Read through the file, looking for the start of classes.
	# Each time we find the start of a class, we save the location
	# and then the next time through, start the search after
	# the start of the last class so that we pick up the next class
	
	while {[regexp -indices $classexp \
		[string range $contents $classEnd end] \
		classStartIndices nm] != 0} {
	    # FIXME: it is just easier to get the name by running regexp
	    # again.
	    regexp $classexp [string range $contents \
		    $classEnd end] \
		    matchvar nm
	    set classEnd [expr {[lindex $classStartIndices 1] + \
		    $classEnd+1} ]
            if {[string range $contents $classEnd $classEnd] != ";"} {
                #puts "ptolemyMkClassGraph nm==$nm '[string range $contents $classEnd $classEnd]'"

                set classfile($nm) $file
            } else {
                set nm {}
            }
	    #set htmlfile($nm) [file join [file dirname $file] \
	    #doc codeDoc [info namespace tail $nm].html]
	    #puts "$nm $classStartIndices $classEnd \
	    #    [string range $contents $classEnd [expr {$classEnd+40}]]"

	    # Look for inheritance only if a class definition was found.
	    if {$nm != {}} {
		#puts "`[string range $contents $classEnd [expr {$classEnd +20}]]'"
		if {[regexp -indices $inheritexp \
			[string range $contents $classEnd end] \
			matchvar inheritIndices] != 0} {
		    # Check for the case where we have multiple classes
		    # in a file, but the first class has no parents.
		    # If the indices of the inherits clause are greater
		    # than the indices of the next class clause, then
		    # the current class has no parents, as the inherit
		    # clause we found belongs to the next class clause.
		    
		    if {[regexp -indices $classexp \
			    [string range $contents $classEnd end] \
			    classStartIndices tnm] != 0 } {
			if {[lindex $classEnd 0] < \
				[lindex $inheritIndices 0]} {
			    #puts "continuing: $nm $classEnd"
			    set parent($nm) {}
			    continue
			}
		    }

		    # FIXME: it is just easier to get the name by running
		    # regexp again.
		    regexp $inheritexp \
			[string range $contents $classEnd end] \
			matchvar pnm
		    if {$nm == $pnm} {
			#puts "In $file, $nm cannot be its own parent"
			set parent($nm) {}

		    } else {
			#puts "nm=$nm, pnm=$pnm"
			set parent($nm) {}
			set parentList [split $pnm ","]
			foreach parentElement $parentList {
			    if {$parentElement != {}} {
                                set parentElement [string trim $parentElement]
                                if [regexp {(public|protected|private)[ 	]+([^ 	]+)} $parentElement matchvar protection parentClass] {
                                    #puts "parentClass=$parentClass"
                                    lappend parent($nm) $parentClass
                                }
			    }
			}
		    }

		} {
		    set parent($nm) {}
		}
	    }
        }
	if {$classEnd == 0 } {
	    # If we are here, then this file does not define a class,
	    # so we check to see if it defines any procs.  If it does
	    # we add it to the list of files that define procs but not classes.
	    
	    if {[regexp $procexp $contents] != 0} {
		lappend nonClassFileList $file
	    }
	}
    }
        
    dumpClassDAG $title $filename classfile parent htmlfile $nonClassFileList
    return $retval
}

#### cppDAG
# Generate a Tycho DAG from C++ files.
# The first argument is the title, which should be of the form
# {{{Foo Class Hierarchy}}}.  The triple nesting is necessary
# because of the various evals that happen while creating the DAG
# The second argument is the filename.
# The rest of the arguments are either files or directories.
# If the argument is a directory, then all the .h files in that
# directory are searched.
#
proc cppDAG {title filename args} {
    #puts "cppDAG '$title' '$filename' $args"
    set olddir [pwd]

    foreach dir $args {
	if [file isdirectory "$dir"] {
	    eval lappend files [glob \
		    [file join $dir {{*.h}}]]
	} else {
            lappend files $dir
        }
    }

    set retval [eval cppMkClassGraph $title $filename $files ]
    cd $olddir
    return $retval
}

#### ptolemyStandardClasses
# Update the Ptolemy classes graph.
#
proc ptolemyStandardDAG {} {
    global PTOLEMY
    set olddir [pwd]
    cd $PTOLEMY

    set dirs [list [list [file join src kernel] ]]

    foreach dir $dirs {
	if [file isdirectory "$dir"] {
            foreach file [glob [file join $dir {{*.h}}]] {
                lappend files [file join "\$PTOLEMY" $file]
            }
	}
    }

#     set files [list [file join "\$PTOLEMY" src kernel Block.h]  \
#              [file join "\$PTOLEMY" src kernel Universe.h] \
#              [file join "\$PTOLEMY" src kernel Galaxy.h] \
             [file join "\$PTOLEMY" src kernel Runnable.h]]
    set retval [eval cppDAG {{{Ptolemy Class Hierarchy}}} \
            [file join doc ptolemyClasses.dag] \
            $files]
    cd $olddir
    return $retval
}
