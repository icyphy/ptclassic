defstar {
	name {TclScript}
	domain {SDF}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF tcltk library }
	desc {
Invoke a Tcl script that can optionally define a procedure that is
invoked every time the star fires. That procedure can read the star's
inputs and update the value of the outputs.
	}
	explanation {
.EQ
delim off
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
of a global Tcl variable \fIstarID\fR.
The procedure used by the Tcl script to set output values is called
"setOutputs_$starID", while the procedure used to read
input values is called "grabInputs_$starID".  The setOutputs procedure
takes as many arguments as there are output ports.  The grabInputs
procedure returns a string with as many values as there are inputs,
separated by spaces.  The Tcl script is sourced during the startup
phase of the star execution, so it does not make sense at this time to
read inputs.  However, it may make sense to set output values (in order
to initialize them).
.pp
The Tcl script can optionally define a Tcl procedure called
"goTcl_$starID".  If this procedure is defined in the script,
then it will be invoked every time the star fires.
It takes one argument, the starID, and returns no values.
If the goTcl procedure is defined, then the communication with
Tcl is said to be \fIsynchronous\fR (it is synchronized to the firing
of the star).  Otherwise, it is \fIasynchronous\fR (the Tcl script is
responsible for setting up procedures that will interact with the star
only when Tcl invokes them).
.pp
For asynchronous operation, typically
X events are bound to Tcl/Tk commands that read or write
data to the star.  These Tcl commands use
grabInputs_$starID, which returns
a list containing the current value of each of the inputs, and
setOutputs_$starID, which sets the value of the outputs.
The argument list for setOutputs_$starID should contain a
floating point value for each output of the star.
The inputs can be of any type.  The print() method of the particle
is used to construct a string passed to Tcl.
This mechanism is entirely asynchronous, in that the Tcl/Tk script
decides when these actions should be performed on the basis of X events.
.pp
In synchronous operation,
the Tcl procedure goTcl_$starID will be called
by the star every time it fires.
The procedure could,
for example, grab input values and compute output values,
although it can do anything the designer wishes, even ignoring the input
and output values.
.EQ
delim $$
.EN
	}
	hinclude { "TclStarIfc.h" }
	outmulti {
		name {output}
		type {float}
		desc { Any outputs obtained from Tcl }
	}
	inmulti {
		name {input}
		type {anytype}
		desc { Any number of inputs to feed to Tcl }
	}
	defstate {
		name {tcl_file}
		type {string}
		default {"$PTOLEMY/src/domains/sdf/tcltk/stars/tkScript.tcl"}
		desc {The file from which to read the tcl script}
	}
	protected {
		// Standardized interface to Tcl
		TclStarIfc tcl;
	}
	begin {
	    tcl.setup(this,input.numberPorts(),
			output.numberPorts(), (const char*)tcl_file);
	}

	go {
	    tcl.go();

	    // Send output values from the local buffer
	    MPHIter nexto(output);
	    PortHole *p;
	    int i = 0;
	    while ((p = nexto++) != 0) {
		(*p)%0 << tcl.outputValues[i++];
	    }
	}

	wrapup {
	    tcl.wrapup();
        }
}
