defstar {
	name {TclScript}
	domain {SDF}
	desc {
Sends input values to a Tcl script.  Gets output values from a Tcl script.
The star can communicate with Tcl either synchronously or asynchronously.
	}
	explanation {
The star reads a file containing Tcl commands and communicates with Tcl
via procedures defined in that file.  Those procedures can read the inputs
to the star and set its outputs.
It can be used in a huge
variety of ways, including using Tk to animate or control a simulation.
The Tcl file must define three Tcl procedures to communicate
between the star and the Tcl code.  One of these reads the values of the
inputs to the star (if any), another writes new values to the outputs (if
any), and the third is called by Ptolemy either on every firing of
the star (if \fIsynchronous\fR is TRUE) or on starting the simulation
(if \fIsynchronous\fR is FALSE).
.pp
The names of the three procedures are different for each instance of this star.
This allows sharing of Tcl code without name conflicts.
These unique names are constructed by prepending a unique string to a
constant suffix.  The suffixes for the three procedures mentioned
above are "setOutputs", "grabInputs", and "callTcl".
The first thing the star does is to define the Tcl variable "uniqueSymbol"
to equal the unique string.  This string specifies the prefix that makes
the name unique.  Thus the full name for the three procedures
is "{uniqueSymbol}setOutputs", "{uniqueSymbol}grabInputs", and
"{uniqueSymbol}callTcl".  The first two of these are defined internally
by the star.  The third should be defined by the user in the Tcl file
that the star reads.  In this star, the "uniqueSymbol" is "tclScriptN",
where "N" is an integer.
.pp
Two basic mechanisms can be used to control the behavior of the star.
In the first, X events are bound to Tcl/Tk commands that read or write
data to the star.  These Tcl commands use
${uniqueSymbol}grabInputs, which returns
a list containing the current value of each of the inputs, and
${uniqueSymbol}setOutputs, which sets the value of the outputs.
The argument list for ${uniqueSymbol}setOutputs should contain a
floating point value for each output of the star.
This mechanism is entirely asychronous, in that the Tcl/Tk script
decides when these actions should be performed on the basis of X events.
.pp
In addition, the Tcl procedure ${uniqueSymbol}callTcl will be called
by the star.  If the parameter "synchronous" is TRUE, this procedure
will be called every time the star fires.
The procedure could,
for example, grab input values and computes output values,
although it can do anything the designer wishes, even ignoring the input
and output values.  If the parameter "synchronous" is FALSE, then the
Tcl script "${uniqueSymbol}callTcl" is called only once during the
initialization phase.  At that
time, it can, for example,
set up periodic calls to poll the inputs and set the outputs.
.pp
If the procedure "${uniqueSymbol}callTcl" is not defined in the given
tcl_file, an error message results.
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF tcltk library }
	hinclude { "ptk.h" }
	outmulti {
		name {output}
		type {float}
		desc { Any outputs obtained from tcl }
	}
	inmulti {
		name {input}
		type {float}
		desc { Any number of intputs to feed to tcl }
	}
	defstate {
		name {tcl_file}
		type {string}
		default {"$PTOLEMY/src/domains/sdf/tcltk/stars/tkScript.tcl"}
		desc {The file from which to read the tcl script}
	}
	defstate {
		name {synchronous}
		type{int}
		default { TRUE }
		desc {If TRUE, tcl procedure '${uniqueSymbol}callTcl' is called on every firing }
	}
	protected {
		// A global variable used to create names that are
		// guaranteed to be unique.  After each use, it should
		// be incremented.
		static int unique;

		// Store the value of "unique" when tcl file is loaded
		int myUnique;

		// Used to store the latest output values supplied by Tcl
		double *outputValues;

		// Use for constructing tcl commands
		char buf[128];
	}
	code {
		// The following line initializes the member "unique"
		// when the .o file is loaded.
		int SDFTclScript::unique = 0;
	}
	constructor {
	    // doing this here ensures that for each instance of the star,
	    // rather than for each run, the name will be unique.
	    myUnique = unique++;
	}
	setup {
	    sprintf(buf,"tclScript%d", myUnique);
	    if(Tcl_SetVar(ptkInterp, "uniqueSymbol", buf, TCL_GLOBAL_ONLY)
			== NULL) {
		Error::abortRun(*this, "Failed to set uniqueSymbol");
		return;
	    }

	    if (input.numberPorts() > 0) {
		sprintf(buf,"tclScript%dgrabInputs",myUnique);
	        Tcl_CreateCommand(ptkInterp, buf,
		    grabInputs, (ClientData) this, NULL);
	    }

	    if (output.numberPorts() > 0) {
		sprintf(buf,"tclScript%dsetOutputs",myUnique);
	        Tcl_CreateCommand(ptkInterp, buf,
		    setOutputs, (ClientData) this, NULL);
		LOG_DEL; delete [] outputValues;
		LOG_NEW; outputValues = new double[output.numberPorts()];
	    }

	    if(((const char*)tcl_file)[0] == '$') {
	        sprintf(buf,"source [ptkExpandEnvVar \\%s]",
			    (const char*)tcl_file);
	        if(Tcl_GlobalEval(ptkInterp, buf) != TCL_OK) {
		    Tcl_Eval(ptkInterp, "ptkDisplayErrorInfo",
					0, (char **) NULL);
		    Error::abortRun(*this, "Cannot source tcl script");
		    return;
		}
	    } else {
	        sprintf(buf,"source %s",(const char*)tcl_file);
	        if(Tcl_GlobalEval(ptkInterp, buf) != TCL_OK) {
		    Tcl_Eval(ptkInterp, "ptkDisplayErrorInfo",
					0, (char **) NULL);
		    Error::abortRun(*this, "Cannot source tcl script");
		    return;
	        }
	    }

	    // Call Tcl if the communication is asynchronous, to start it
	    // running.
	    if(!int(synchronous)) {
		sprintf(buf,"tclScript%dcallTcl",myUnique);
	        if(Tcl_Eval(ptkInterp, buf, 0, (char **) NULL) != TCL_OK) {
		    Tcl_Eval(ptkInterp, "ptkDisplayErrorInfo",
					0, (char **) NULL);
		    Error::abortRun(*this, "Failed to run callTcl procedure");
		    return;
	        }
	    }
	}

	go {
	    // If synchronous == TRUE, callTcl
	    if(int(synchronous)) {
		sprintf(buf,"tclScript%dcallTcl",myUnique);
	        if(Tcl_Eval(ptkInterp, buf, 0, (char **) NULL) != TCL_OK) {
		    Tcl_Eval(ptkInterp, "ptkDisplayErrorInfo",
					0, (char **) NULL);
		    Error::abortRun(*this, "Failed to run callTcl procedure");
	        }
	    }
	    // Send output values from the local buffer
	    MPHIter nexto(output);
	    PortHole *p;
	    int i = 0;
	    while ((p = nexto++) != 0) {
		(*p)%0 << outputValues[i++];
	    }
	}

	method {
	    name { getInputs }
	    // Would like this to be char*, but this fails
	    type { StringList }
	    access { public }
	    arglist { "()" }
	    code {
		MPHIter nexti(input);
		PortHole *p;
		StringList result;
		while ((p = nexti++) != 0) {
		    result += double((*p)%0);
		    result += " ";
		}
		return result;
	    }
	}

	// Define the callback procedure used by Tcl to get the value of
	// the inputs.
	code {
	    extern "C" static int
            grabInputs(
                ClientData star,                    // Pointer to the star
                Tcl_Interp *interp,                 // Current interpreter
                int argc,                           // Number of arguments
                char **argv                         // Argument strings
	    ) {
		Tcl_SetResult(ptkInterp,
			((SDFTclScript*)star)->getInputs(),0);
                return TCL_OK;
            }
	}

	// Load the local buffer outputValues[] with values supplied by Tcl
	method {
	    name { setOneOutput }
	    type { void }
	    access { public }
	    arglist { "(int outNum, double outValue)" }
	    code {
		if(outNum >= output.numberPorts()) {
		    Error::warn(*this,"Too many outputs supplied by Tcl");
		} else {
		    outputValues[outNum] = outValue;
		}
	    }
	}

	// Define the callback procedure used by Tcl to get the value of
	// the inputs.
	code {
	    extern "C" static int
            setOutputs(
                ClientData star,                    // Pointer to the star
                Tcl_Interp *interp,                 // Current interpreter
                int argc,                           // Number of arguments
                char **argv                         // Argument strings
	    ) {
		float temp;
		for(int i=0; i < (argc-1); i++) {
		    if(sscanf(argv[i+1], "%f", &temp) != 1) {
		        Error::abortRun(*(SDFTclScript*)star,
			    "Invalid output value from Tcl to star");
			return TCL_ERROR;
		    }
		    ((SDFTclScript*)star)->setOneOutput(i,temp);
		}
                return TCL_OK;
            }
	}
}
