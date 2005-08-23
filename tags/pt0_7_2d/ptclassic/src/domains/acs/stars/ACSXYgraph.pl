defcorona {
    name { XYgraph }
    domain { ACS }
    desc {
Generates an X-Y plot with the pxgraph program.
The X data is on "xInput" and the Y data is on "input".
    }
    version { @(#)ACSXYgraph.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
	input {
		name { input }
		type { anytype }
	}
	input {
		name { xInput }
		type { anytype }
	} 
	defstate {
		name {title}
		type {string}
		default {"Ptolemy XYgraph"}
		desc { Title for the plot. }
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc { File name for saving plottable data. }
	}
	defstate {
		name {options}
		type {string}
		default {"-bb -tk =800x400"}
		desc {Command line options for the xgraph program.}
	}
	defstate {
		name {ignore}
		type {int}
		default { 0 }
		desc { Number of initial values to ignore.}
	}
	protected {
		XGraph graph;
	}
	hinclude { "Display.h" }

}
