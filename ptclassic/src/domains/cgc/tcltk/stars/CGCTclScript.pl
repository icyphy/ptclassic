defstar {
	name {TclScript}
	domain {CGC}
	desc {
Sends input values to a Tcl script.  Gets output values from a Tcl script.
The star can communicate with Tcl either synchronously or asynchronously.
	}
	htmldoc {
This star reads a file containing Tcl commands and communicates with Tcl
via procedures defined in that file.  Those procedures can read the inputs
to the star and set its outputs.
It can be used in a huge
variety of ways, including using Tk to animate or control a simulation.
The Tcl file must define three Tcl procedures to communicate
between the star and the Tcl code.  One of these reads the values of the
inputs to the star (if any), another writes new values to the outputs (if
any), and the third is called by Ptolemy either on every firing of
the star (if <i>synchronous</i> is TRUE) or on starting the simulation
(if <i>synchronous</i> is FALSE).
<p>
The names of the three procedures are different for each instance of this star.
This allows sharing of Tcl code without name conflicts.
These unique names are constructed by prepending a unique string to a
constant suffix.  The suffixes for the three procedures mentioned
above are "setOutputs", "grabInputs", and "callTcl".
The first thing the star does is to define the Tcl variable "uniqueSymbol"
to equal the unique string.  This string specifies the prefix that makes
the name unique.  Thus the full name for the three procedures
is "${uniqueSymbol}setOutputs", "${uniqueSymbol}grabInputs", and
"${uniqueSymbol}callTcl".  The first two of these are defined internally
by the star.  The third should be defined by the user in the Tcl file
that the star reads.
<p>
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
<p>
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
<p>
If the procedure "${uniqueSymbol}callTcl" is not defined in the given
tcl_file, an error message results.
<p>
Finally, this star can be used as a based class for other Tcl/Tk CGC
stars.  In this manner, you can also have tcl/tk code which uses custom
states.  The states are stored in a array named ${uniqueSymbol}
indexed by the state name.  For example the tcl command:
<p>
set foo [set ${uniqueSymbol}(foo)]
<p>
will set the variable "foo" to the value of the state named "foo".
	}
	version { @(#)CGCTclScript.pl	1.25	12/08/97 }
	author { E. A. Lee, D. Niehaus and J. L. Pino }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Tcl/Tk library }
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
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_GLOBAL }
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
	    addCode(setUniqueSymbol,"tkSetup");

            // Export the environment variables
            BlockStateIter nextState(*this);
            State* state;
            while ((state = nextState++) !=NULL ) {
		// It would be good to be able to use the Tokenizer
		// class here - but we can because it strips out " characters
		// FIXME

		// We escape $ with $$ and " with \".
		// $ is escaped so that the expandMacro
		// code will ignore embedded $ in state values.
		// " is escaped so that we can embed the entire string
	        // in "" in CGCTclScript::exportState

                StringList currentValue;
		currentValue = state->currentValue();
		const char* p = currentValue;
		StringList escapedCurrentValue;
		while (*p) {
		    if (*p == '\"') escapedCurrentValue << '\\';
                    else if (*p == '$') escapedCurrentValue << '$';
                    escapedCurrentValue << *p;
                    p++;
                }

                addCode(exportState(state->name(),escapedCurrentValue),
                        "tkSetup");
            }

            // Create setOutputs tcl command
	    numOutputs = output.numberPorts();
	    if (numOutputs > 0) {
		addCode(declareSetOutputs,"tkSetup");
		addCode(setOutputsDef, "procedure");
	    }

            // Create setInputs tcl command
	    numInputs = input.numberPorts();
	    if (numInputs > 0) {
		addCode(declareGrabInputs,"tkSetup");
		addCode(grabInputsDef, "procedure");
	    }

            // Source the specified shell script
	    if(((const char*)tcl_file)[0] == '$') {
	        addCode(sourceTclwEnv, "tkSetup");
	    } else {
	        addCode(sourceTclwoEnv, "tkSetup");
	    }

	    StringList out;
	    if (int(numOutputs) > 0)
	        out << "\tfloat $starSymbol(outs)[" << int(numOutputs)
                    << "];\n";
	    if (int(numInputs) > 0)
	        out << "\tfloat $starSymbol(ins)[" << int(numInputs) << "];\n";
	    addGlobal((const char*) out);
	    if(!int(synchronous)) {
		addCode(setUniqueSymbol);
		addCode(callTcl);
	    }
	}
	go {
	    StringList out;
	    int i;
	    for (i = 1; i <= input.numberPorts(); i++) {
		temp = i;
		out << "\t$starSymbol(ins)[" << i-1 << "] = $ref(input#temp);\n";
	        addCode((const char*) out);
		out.initialize();
	    }
	    if(int(synchronous)) {
		addCode(setUniqueSymbol);
		addCode(callTcl);
	    }
	    for (i = 1; i <= output.numberPorts(); i++) {
		temp = i;
		out << "\t$ref(output#temp) = $starSymbol(outs)[" << i-1 << "];\n";
	        addCode((const char*) out);
		out.initialize();
	    }
	}
	codeblock (setUniqueSymbol) {
	    if(Tcl_Eval(interp, "global uniqueSymbol; set uniqueSymbol $starSymbol(tkScript)")
		!= TCL_OK)
		errorReport("Error accessing tcl");
	}
	codeblock (declareSetOutputs) {
	    Tcl_CreateCommand(interp, "$starSymbol(tkScript)setOutputs",
		$starSymbol(setOutputs), (ClientData) 0, NULL);
	}
	codeblock (declareGrabInputs) {
	    Tcl_CreateCommand(interp, "$starSymbol(tkScript)grabInputs",
		$starSymbol(grabInputs), (ClientData) 0, NULL);
	}
        codeblock (exportState,"const char* name, const char* value") {
	    if(Tcl_Eval(interp, 
                "set $starSymbol(tkScript)(@name) {@value}") != TCL_OK)
		errorReport("Cannot initialize state @name");
	}
	codeblock (sourceTclwEnv) {
	    if(Tcl_Eval(interp, "source [expandEnvVars \\$val(tcl_file)]")
		!= TCL_OK)
		errorReport("Cannot source tcl script for TclScript star");
	}
	codeblock (sourceTclwoEnv) {
	    if(Tcl_Eval(interp, "source [expandEnvVars $val(tcl_file)]")
		!= TCL_OK)
		errorReport("Cannot source tcl script for TclScript star");
	}
	codeblock (callTcl) {
	    if(Tcl_Eval(interp, "$starSymbol(tkScript)callTcl") != TCL_OK)
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
                    /* Ignore -- probably premature */
                    return TCL_OK;
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
		static char temp[32];
		int i;
		for(i=0; i<$val(numInputs); i++) {
                    sprintf(temp, "%f", $starSymbol(ins)[i]);
		    Tcl_AppendElement(interp,temp);
		}
                return TCL_OK;
            }
	}
}
