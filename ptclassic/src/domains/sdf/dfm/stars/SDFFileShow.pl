defstar {
    name {FileShow}
    domain {SDF}
    derivedFrom { TclScript }
    desc {
Display the input files using Tycho.
The extension on the file name determines what editor is used
for the display.
    }
    version { @(#)SDFFileShow.pl	1.3	01 Oct 1996 }
    author { E. A. Lee }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { SDF dfm library }
	htmldoc {
This star checks to see whether Tycho has been loaded.
If not, it loads it and invokes ::tycho::File::openContext on the input
files.
    }
    hinclude { "ptk.h" }
    setup {
        if (output.numberPorts() > 0) {
            Error::abortRun(*this, "Outputs are not supported");
            return;
        }
    }
    begin {
        tcl_file = "$PTOLEMY/src/domains/sdf/dfm/stars/fileshow.tcl";

        SDFTclScript::begin();
    }
    constructor {
        output.setAttributes(P_HIDDEN);
        tcl_file.clearAttributes(A_SETTABLE);
    }
}
