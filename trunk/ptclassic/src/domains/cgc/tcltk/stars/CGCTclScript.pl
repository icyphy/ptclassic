defstar {
	name {TclScript}
	domain {CGC}
	desc {
Sends input values to a Tcl script.  Gets output values from a Tcl script.
The star can communicate with Tcl either synchronously or asynchronously.
	}
	explanation {
This star reads a file containing Tcl commands and communicates with Tcl
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
is "${uniqueName}setOutputs", "${uniqueName}grabInputs", and
"${uniqueName}callTcl".  The first two of these are defined internally
by the star.  The third should be defined by the user in the Tcl file
that the star reads.
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
	author { E. A. Lee and D. Niehaus }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC tcltk library }
	outmulti {
		name {output}
		type {float}
	}
	inmulti {
		name {input}
		type {float}
	}
	defstate {
		name {tcl_file}
		type {string}
		default {"$PTOLEMY/src/domains/cgc/tcltk/stars/tkScript.tcl"}
		desc {The file from which to read the tcl script}
	}
	defstate {
		name {synchronous}
		type{int}
		default { TRUE }
		desc {If TRUE, tcl procedure '${uniqueSymbol}callTcl' is called on every firing }
	}
	defstate {
		name {numOutputs}
		type {int}
		default {0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name {numInputs}
		type {int}
		default {0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name {temp}
		type {int}
		default {0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	initCode {
	    addCode(setup1,"tkSetup");
	    if(((const char*)tcl_file)[0] == '$') {
	        addCode(sourceTclwEnv, "tkSetup");
	    } else {
	        addCode(sourceTclwoEnv, "tkSetup");
	    }
	    numOutputs = output.numberPorts();
	    if (numOutputs > 0) {
		addCode(setup2,"tkSetup");
		addCode(setOutputsDef, "procedure");
	    }
	    numInputs = input.numberPorts();
	    if (numInputs > 0) {
		addCode(setup3,"tkSetup");
		addCode(grabInputsDef, "procedure");
	    }
	    StringList out;
	    if (int(numOutputs) > 0)
	        out << "\tfloat $starSymbol(outs)[" << int(numOutputs) << "];\n";
	    if (int(numInputs) > 0)
	        out << "\tfloat $starSymbol(ins)[" << int(numInputs) << "];\n";
	    addGlobal((const char*) out);
	    if(!int(synchronous)) {
		addCode(callTcl);
	    }
	}
	go {
	    StringList out;
	    for (int i = 1; i <= input.numberPorts(); i++) {
		temp = i;
		out << "\t$starSymbol(ins)[" << i-1 << "] = $ref(input#temp);\n";
	        addCode((const char*) out);
		out.initialize();
	    }
	    if(int(synchronous)) {
		addCode(callTcl);
	    }
	    for (i = 1; i <= output.numberPorts(); i++) {
		temp = i;
		out << "\t$ref(output#temp) = $starSymbol(outs)[" << i-1 << "];\n";
	        addCode((const char*) out);
		out.initialize();
	    }
	}
	codeblock (setup1) {
	    if(Tcl_Eval(interp, "set uniqueSymbol $starSymbol(tkScript)", 0,
		(char **) NULL) != TCL_OK)
		errorReport("Error accessing tcl");
	}
	codeblock (setup2) {
	    Tcl_CreateCommand(interp, "$starSymbol(tkScript)setOutputs",
		$starSymbol(setOutputs), (ClientData) 0, NULL);
	}
	codeblock (setup3) {
	    Tcl_CreateCommand(interp, "$starSymbol(tkScript)grabInputs",
		$starSymbol(grabInputs), (ClientData) 0, NULL);
	}
	codeblock (sourceTclwEnv) {
	    if(Tcl_Eval(interp, "source [expandEnvVar \\$val(tcl_file)]", 0,
		(char **) NULL) != TCL_OK)
		errorReport("Cannot source tcl script for TclScript star");
	}
	codeblock (sourceTclwoEnv) {
	    if(Tcl_Eval(interp, "source [expandEnvVar $val(tcl_file)]", 0,
		(char **) NULL) != TCL_OK)
		errorReport("Cannot source tcl script for TclScript star");
	}
	codeblock (callTcl) {
	    if(Tcl_Eval(interp, "$starSymbol(tkScript)callTcl",
		0, (char **) NULL) != TCL_OK)
		errorReport("Error invoking callTcl");
	}

	// Define the callback procedure used by Tcl to set the value
	// of the outputs.
	codeblock (setOutputsDef) {
	    static int
            $starSymbol(setOutputs)(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		int i;
		if(argc != $ref(numOutputs)+1) {
                    errorReport("Invalid number of arguments");
                    return TCL_ERROR;
		}
		for(i=0; i<$ref(numOutputs); i++) {
                    if(sscanf(argv[i+1], "%f", &$starSymbol(outs)[i]) != 1) {
                        errorReport("Invalid value");
                        return TCL_ERROR;
                    }
		}
                return TCL_OK;
            }
	}

	// Define the callback procedure used by Tcl to get the value of
	// the inputs.
	codeblock (grabInputsDef) {
	    static int
            $starSymbol(grabInputs)(dummy, interp, argc, argv)
                ClientData dummy;                   /* Not used. */
                Tcl_Interp *interp;                 /* Current interpreter. */
                int argc;                           /* Number of arguments. */
                char **argv;                        /* Argument strings. */
            {
		/* FIX ME: Is 32 always enough? */
		static char* temp[32];
		int i;
		for(i=0; i<$val(numInputs); i++) {
                    sprintf(temp, "%f", $starSymbol(ins)[i]);
		    Tcl_AppendElement(interp,temp,0);
		}
                return TCL_OK;
            }
	}
}
