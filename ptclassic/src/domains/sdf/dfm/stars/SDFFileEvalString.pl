defstar {
    name { FileEvalString }
    domain { SDF }
    derivedFrom { FileEval }
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
This command is just like the FileEval command, but with the addition
of another set of outputs "strout".  This set of outputs is treated
a bit differently.  If the execution of the "command" parameter
sets the variables "strout#1", "strout#2", etc., then the value
given to those variables becomes the output filename.
For example, if "command" is "set strout#1 foo", then the output
filename is "foo".  If the "command" script does not set "strout#i"
for a connected output "i", then a temporary filename is generated.
    }
	htmldoc {
The idea of this star is to give the user more control over the
produced filenames.  In particular, the filenames can be constructed
on the fly by the "command" script.  But there are also trickier uses
of this capability.
Since the File datatype simply manages strings that it assumes are file
names and does not enforce that a file by the given name exists, this
datatype can be used to pass around ordinary strings that are not
associated with files.
The strout outputs have no effect on whether the command will be
executed.
    }
    outmulti {
	name {strout}
	type {filemsg}
	desc {Output names set by the command}
    }
    ccinclude { "FileMessage.h" }
    code {
extern "C" {
#include "ptk.h"
}
    }
    go {
	SDFFileEval::go();
	// Now step through the strout outputs and generate filenames.
	MPHIter nextp(strout);
	for (int i = 0; i < strout.numberPorts(); i++) {
	    FileMessage* fm;
	    // FIXME: The variables should not really be global.
	    // They should be in the the starID array.
	    InfString var = "strout#";
	    var << i;
	    if (Tcl_GetVar(ptkInterp,(char*)var,TCL_GLOBAL_ONLY) == TCL_OK) {
		fm = new FileMessage(ptkInterp->result);
	    } else {
		fm = new FileMessage();
	    }
	    Envelope pkt(*fm);
		
	    // Now send the filename to the output
	    PortHole* p = nextp++;
	    (*p)%0 << pkt;
	}
    }
}
