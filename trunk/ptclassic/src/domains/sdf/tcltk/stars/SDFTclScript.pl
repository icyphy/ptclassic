defstar {
	name {TclScript}
	domain {SDF}
	desc {
Invoke a Tcl script that can optionally define a procedure that is
invoked every time the star fires.
	}
	explanation {
.EQ
nodelim
.EN
The star reads a file containing Tcl commands.
It can be used in a huge
variety of ways, including using Tk to animate or control a simulation.
A number of procedures and global variables will have been defined for
use by the Tcl script by the time it is sourced.  These enable the script
to read the inputs to the star or set output values.
The Tcl script can optionally define a procedure to be
called by Ptolemy on every firing of the star.
.pp
Much of the complexity in using this star is due to the need to use unique
names for each instance of the star.
These unique names are constructed using a unique string defined by
the star.  That string is made available to the Tcl script in the form
of a global Tcl variable \fIuniqueSymbol\fR.
The procedure used by the Tcl script to set output values is called
"${uniqueSymbol}setOutputs", while the procedure used to read
input values is called "${uniqueSymbol}grabInputs".  The setOutputs procedure
takes as many arguments as there are output ports.  The grabInputs
procedure returns a string with as many values as there are inputs,
separated by spaces.  The Tcl script is sourced during the startup
phase of the star execution, so it does not make sense at this time to
read inputs.  However, it may make sense to set output values (in order
to initialize them).
.pp
The Tcl script can optionally define a Tcl procedure called
"${uniqueSymbol}callTcl".  If this procedure is defined in the script,
then it will be invoked every time the star fires.
It takes no arguments are returns no values.
If the callTcl procedure is defined, then the communication with
Tcl is said to be \fIsynchronous\fR (it is synchronized to the firing
of the star).  Otherwise, it is \fIasynchronous\fR (the Tcl script is
responsible for setting up procedures that will interact with the star
only when Tcl invokes them).
.pp
For asynchronous operation, typically
X events are bound to Tcl/Tk commands that read or write
data to the star.  These Tcl commands use
${uniqueSymbol}grabInputs, which returns
a list containing the current value of each of the inputs, and
${uniqueSymbol}setOutputs, which sets the value of the outputs.
The argument list for ${uniqueSymbol}setOutputs should contain a
floating point value for each output of the star.
The inputs can be of any type.  The print() method of the particle
is used to construct a string passed to Tcl.
This mechanism is entirely asychronous, in that the Tcl/Tk script
decides when these actions should be performed on the basis of X events.
.pp
In synchronous operation,
the Tcl procedure ${uniqueSymbol}callTcl will be called
by the star every time it fires.
The procedure could,
for example, grab input values and compute output values,
although it can do anything the designer wishes, even ignoring the input
and output values.
.EQ
delim $$
.EN
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
		type {anytype}
		desc { Any number of inputs to feed to tcl }
	}
	defstate {
		name {tcl_file}
		type {string}
		default {"$PTOLEMY/src/domains/sdf/tcltk/stars/tkScript.tcl"}
		desc {The file from which to read the tcl script}
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
		char uniqStr[32];

		// Mark whether the star is synchronous or not
		int synchronous;
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
	    sprintf(uniqStr,"tclScript%d", myUnique);
	    if(Tcl_SetVar(ptkInterp, "uniqueSymbol", uniqStr, TCL_GLOBAL_ONLY)
			== NULL) {
		Error::abortRun(*this, "Failed to set uniqueSymbol");
		return;
	    }

	    if (input.numberPorts() > 0) {
		sprintf(buf,"%sgrabInputs",uniqStr);
	        Tcl_CreateCommand(ptkInterp, buf,
		    grabInputs, (ClientData) this, NULL);
	    }

	    if (output.numberPorts() > 0) {
		sprintf(buf,"%ssetOutputs",uniqStr);
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

	    // Determine whether the star is synchronous by checking to
	    // see whether the callTcl procedure has been defined by the
	    // Tcl script.
	    sprintf(buf,"info procs %scallTcl",uniqStr);
	    if(Tcl_Eval(ptkInterp, buf, 0, (char **) NULL) != TCL_OK ||
	       strlen(ptkInterp->result) == 0) synchronous = 0;
	    else synchronous = 1;
	}

	go {
	    // If synchronous == TRUE, callTcl
	    if(synchronous) {
	        if(Tcl_SetVar(ptkInterp, "uniqueSymbol", uniqStr,
			TCL_GLOBAL_ONLY) == NULL) {
		    Error::abortRun(*this, "Failed to set uniqueSymbol");
		    return;
	        }
		sprintf(buf,"%scallTcl",uniqStr);
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
		    // return a quoted string for tcl consumption
		    result += "{";
		    result += ((*p)%0).print();
		    result += "} ";
		}
		return result;
	    }
	}

	// Define the callback procedure used by Tcl to get the value of
	// the inputs.
	code {
	    static int grabInputs(
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
	    static int setOutputs(
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
	destructor {
	    // Delete Tcl commands created for this star
	    sprintf(buf,"%sgrabInputs",uniqStr);
	    Tcl_DeleteCommand(ptkInterp, buf);
	    sprintf(buf,"%ssetOutputs",uniqStr);
	    Tcl_DeleteCommand(ptkInterp, buf);
	    sprintf(buf,"%scallTcl",uniqStr);
	    Tcl_DeleteCommand(ptkInterp, buf);
	}
}
