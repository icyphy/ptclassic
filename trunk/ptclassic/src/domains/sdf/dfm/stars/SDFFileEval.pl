defstar {
    name { FileEval }
    domain { SDF }
    derivedFrom { TclScript }
    version { $Id$ }
    author { E. A. Lee }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { SDF dfm library }
    desc {
Evaluate the "command" parameter in Tcl. Inputs are referenced in
the comand as "input#1", "input#2", etc.  Outputs are file names,
and are referenced in the command as "output#1", "output#2", etc.
For example, if "command" is "exec sed -e /foo/s//bar/ input#1 &gt output#1",
then the program "sed" will be used to replace all instances of "foo"
in the input file given by the first input to yield the output file
on the first output.  
    }
	htmldoc {
The command may or may not be executed.  It will be executed
if any of the following conditions holds:
<OL>
<LI> There are files in the inputs and these are newer than the outputs.
<LI> The "conditional" parameter is "NO" (this is the default).
<LI> The "command" is different from the previous invocation.
</OL>
For the purposes of testing condition (3), the command is compared
to the previous invocation after replacing all input and output references
with the values of the inputs and the names of the input files.
The latest command executed is stored in the top-level facet,
although you must save the facet for this information to persist
to the next time you run Ptolemy.
<p>
The files produced by this star may or may not be persistent.
If the parameter "output_filenames" is given, then it must be a
list of file names to use for the output.  In this case, the
output files will persist.  If "output_filenames" is the empty string,
then unique temporary file names are generated. In this latter case,
normally the temporary files will be deleted after they have been read
at their destination.  Deletion of these files can be prevented
by setting "save_output_files" to "YES" (the default is "NO").
<p>
Although it is expected that most uses of this star will have
inputs of type File, any type of input is acceptable. The string
returned by the "print" method of the input particles replaces
the strings "input#1", etc., before the command is evaluated.
Regrettably, because of the current overly restrictive type-resolution
mechanism in Ptolemy, all inputs must be of the same type.
<p>
The "command" parameter can be any Tcl command.  A common application
will use the Tcl "exec" command to invoke some other program on the
input files, producing output files. Note that when using exec in this way,
the program should not be put in the background.  It is up to you
to ensure that the output files are written to before "command" returns.
Otherwise, downstream actors may complain about not receiving the
files they expect.
<p>
If "show_evaluation" is "YES", then the icon associated with the
star will be highlighted in green whenever the command is actually
executed.  Since this star is meant to be used to invoke large,
complex, external programs, this parameter is "YES" by default.
A useful visualization tool is to turn on graphical animation
as well.  In this case, the star will be highlighted in red
when it fires, and the color will change to green if (and only if)
the command is evaluated.
<p>
If there is a delay on the arc feeding one of the inputs, then
there will be no file name for that input on the first firing.
In this case, the star replaces references to the input in the
command with the special string "/dev/null".
Although it is probably rare to have an application that needs this,
if you have such an application, your command should be able to
accept this string.
<p>
Note that in the current implementation, is not advisable to use
the same file name for an output as an input.  The results will
be unpredictable, and there is no error checking.
<p>
This star is designed to make it easy to derive more specialized
stars from it.  Thus, if you have some external program that you use
frequently, you should consider deriving a star from this one.
The mechanism for checking whether the command should be evaluated
is implemented in a virtual protected method called "evalneeded".
This method also performs the substitutions in the command for
references to the inputs and outputs.  Thus, in a derived star,
you may only need to redefine the tcl_file parameter.  This parameter
gives the name of a Tcl file that defines the "goTcl_$starID" procedure
that is invoked when it has been determined that "command" should
be evaluated.
<p>
For convenience, another method is provided for conditionally sourcing
a Tcl file containing support code.  This method is called
"sourceIfNeeded" and it takes two arguments.  The first argument is
the name of Tcl procedure defined in the file.  If this Tcl procedure
already exists in the interpreter, then it is assumed that the file
need not be sourced again.  If the procedure does not exist, then
the name of the file to source is given by the second argument.
    }
    ccinclude { "FileMessage.h" <iostream.h> }
    outmulti {
	name {out}
	type {filemsg}
	desc {Output files for the results}
    }
    defstate {
	name {command}
	type {string}
	default {"exec cat input#1 > output#1"}
	desc {The command line to execute.}
    }
    defstate {
	name {conditional}
	type {int}
	default {"NO"}
	desc {If YES, evaluate only if something has changed. }
    }
    defstate {
	name {output_filenames}
	type {stringarray}
	default {""}
	desc {If no filenames are given, temporary files will be used.}
    }
    defstate {
	name {save_output_files}
	type {int}
	default {"NO"}
	desc {If temporary files are used, this specifies whether they persist.}
    }
    defstate {
	name {show_evaluation}
	type {int}
	default {"YES"}
	desc {Show evaluation by highlighting the block in green}
    }
    protected {
	int useTmpFiles;
    }
    setup {
	if (output_filenames.size() == 0) {
	    useTmpFiles = 1;
	} else {
	    useTmpFiles = 0;
	    if (output_filenames.size() != out.numberPorts()) {
		Error::abortRun(*this,
		"Number of output filenames does not match "
		"the number of outputs");
	    }
	}
    }
    begin {
        tcl_file = "$PTOLEMY/src/domains/sdf/dfm/stars/fileeval.tcl";

	if (sourceIfNeeded("fileeval_evalneeded",
	        "$PTOLEMY/src/domains/sdf/dfm/stars/fileevalsupport.tcl")
		== 0) {
	    return;
	}

        SDFTclScript::begin();
    }
    constructor {
        tcl_file.clearAttributes(A_SETTABLE);
	output.setAttributes(P_HIDDEN);
    }
    code {
extern "C" {
#include "ptk.h"
}
    }
    // Return TRUE if evaluation is needed.
    // This occurs if:
    //   (1) There are files in the inputs and these are newer than the outputs.
    //   (2) The conditional parameter is NO.
    //   (3) The (edited) command is different from the previous invocation.
    //
    // As a side effect, this procedure sets a Tcl variable
    // $starID(editedcommand) equal to an edited version of the command
    // parameter.  The editing done is:
    //   (1) "input#i" is replaced by whatever is returned by the print
    //       method of the input particle at the i-th input.
    //   (2) "output#i" is replaced by the filename of the i-th output.
    // 
    // As a second side effect, the Tcl variable $starID(outputfilenames)
    // is set to a list of output file names.
    //
    // This method should be invoked by the go method.
    virtual method {
	name { evalneeded }
	access { protected }
	arglist { "()" }
	type { int }
	code {
	    // Construct a list of output filenames for Tcl.
	    InfString outnames;
	    MPHIter nextp(out);
	    for (int i = 0; i < out.numberPorts(); i++) {
		FileMessage* fm;
		if (useTmpFiles) {
		    fm = new FileMessage();
		} else {
		    fm = new FileMessage((char*)output_filenames[i]);
		}
		if (int(save_output_files)) {
		    fm->setTransient(0);
		}
		outnames += fm->fileName();
		Envelope pkt(*fm);
		
		// Now send the filename to the output
		PortHole* p = nextp++;
		(*p)%0 << pkt;
	    }
	    char* buf = (char*)outnames;
	    if (!buf) {buf = ""; }
	    Tcl_SetVar2(ptkInterp, tcl.id(), "outputfilenames",
	    buf, TCL_GLOBAL_ONLY);

	    InfString cmd = "[fileeval_evalneeded ";
	    cmd << tcl.id() << "]";
	    int result;
	    if (Tcl_ExprBoolean(ptkInterp, (char*)cmd, &result) != TCL_OK) {
		Error::abortRun(*this, ptkInterp->result);
		return 0;
	    } else {
		return result;
	    }
	}
    }
    // If the Tcl procedure given by the first argument exists already
    // in the interpreter, do nothing.  Otherwise, source the file given
    // by the second argument.  Return 1 if no errors occured, 0 otherwise.
    // Error::abortRun is also called if an error occurs.
    method {
	name { sourceIfNeeded }
	access { protected }
	arglist { "(const char* proc, const char* filename)" }
	type { int }
	code {
	    InfString cmd;
	    cmd << "if {[info commands " << proc << "] == {}} {\n";
	    cmd << "    source " << filename << "\n";
	    cmd << "}";
	    if (Tcl_GlobalEval(ptkInterp, (char*)cmd) != TCL_OK) {
		Error::abortRun(*this, ptkInterp->result);
		return 0;
	    }
	    return 1;
	}
    }
    go {
	if (evalneeded()) {
	    if (int(show_evaluation)) {
		InfString cmd = "ptkHighlight ";
		cmd << fullName();
		// The following specifies a color in the arcane way 
		// required by vem: RGB. NOTE: Tycho incompatibility.
		cmd << " 0 65535 0";
		Tcl_GlobalEval(ptkInterp, (char*)cmd);
	    }
	    SDFTclScript::go();
	    if (int(show_evaluation)) {
		InfString cmd = "ptkClearHighlights";
		Tcl_GlobalEval(ptkInterp, (char*)cmd);
	    }	    
	}
    }
}
