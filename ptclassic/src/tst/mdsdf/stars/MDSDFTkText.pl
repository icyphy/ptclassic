defstar {
	name { TkText }
	domain { MDSDF }
	desc {
	}
	version {$Id$}
	author { D. G. Messerschmitt, J. Buck, Christopher Hylands }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { MDSDF test library }
	explanation {
	}
	inmulti {
		name { input }
		type { ANYTYPE }
	}
	defstate {
		name { fileName }
		type { string }
		default { "<stdout>" }
		desc { Filename for output. }
	}
	defstate {
	  name {number_of_past_values}
	  type {string}
	  default { "100" }
	  desc {Specify how many past values you would like saved}
	}
	defstate {
	  name {label}
	  type {string}
	  default {"Inputs to the TkShowValues star:"}
	  desc {A label to put on the display}
	}
	defstate {
	  name {put_in_control_panel}
	  type {int}
	  default { "YES" }
	  desc {Specify to put bars in the control panel (vs. their own window)}
	}
	defstate {
	  name {wait_between_outputs}
	  type {int}
	  default { "NO" }
	  desc {Specify whether to wait for user input between output values}
	}

	defstate {
		name {tcl_file}
		type {string}
		default {"$PTOLEMY/src/domains/sdf/tcltk/stars/tkScript.tcl"}
		desc {The file from which to read the tcl script}
	}
        defstate {
                name { numRows }
                type { int }
                default { 2 }
                desc { The number of rows in the matrix. }
        }
        defstate {
                name { numCols }
                type { int }
                default { 2 }
                desc { The number of columns in the matrix. }
        }

	hinclude { "pt_fstream.h" }
	protected {
		pt_ofstream *p_out;
	}
	constructor {
		p_out = 0;
	}
	setup {
		// in case file was open from previous run w/o wrapup call
		LOG_DEL; delete p_out;
		LOG_NEW; p_out = new pt_ofstream(fileName);
            // set the dimensions of the porthole
            //  temporary, would prefer to define some kind of ANY_SIZE
            //  dimensionality
	    input.setMDSDFParams(int(numRows), int(numCols));
            //output.setMDSDFParams(int(numRows), int(numCols));

	}
	go {
		pt_ofstream& output = *p_out;
		MPHIter nexti(input);
		PortHole* p;
		while ((p = nexti++) != 0)
		 	output << ((*p)%0).print() << "\t";
		output << "\n";
		fflush(stdout);
	}
	wrapup {
		LOG_DEL; delete p_out;		// flush output
		p_out = 0;
	}
	destructor {
		LOG_DEL; delete p_out;		// flush output
	}
}
