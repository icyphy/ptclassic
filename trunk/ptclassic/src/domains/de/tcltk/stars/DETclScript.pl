defstar {
	name {TclScript}
	domain {DE}
	version { $Id$ }
	author { Brian Evans and Edward Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE tcltk library }
	desc {
Invoke a Tcl script that can optionally define a procedure that is
invoked every time the star fires.
	}
	explanation {
.EQ
nodelim
.EN
The star reads a file containing Tcl commands.
It can be used in a large variety of ways, including using Tk to animate
or control a simulation.
A number of procedures and global variables will have been defined for
use by the Tcl script by the time it is sourced.
These enable the script to read the inputs to the star or set output values.
The Tcl script can optionally define a procedure to be called by Ptolemy
on every firing of the star.
.pp
Much of the complexity in using this star is due to the need to use unique
names for each instance of the star.
These unique names are constructed using a unique string defined by the star.
That string is made available to the Tcl script in the form of a global
Tcl variable \fIstarID\fR.
The procedure used by the Tcl script to set output values is called
"setOutputs_$starID", while the procedure used to read input values is
called "grabInputs_$starID".
The setOutputs procedure takes as many arguments as there are output ports.
The grabInputs procedure returns a string with as many values as there are
inputs, separated by spaces.
The Tcl script is sourced during the startup phase of the star execution,
so it does not make sense at this time to read inputs.
However, it may make sense to set output values (in order to initialize them).
.pp
The Tcl script can optionally define a Tcl procedure called "goTcl_$starID".
If this procedure is defined in the script, then it will be invoked every
time the star fires.
It takes one argument, the starID, and returns no values.
If the goTcl procedure is defined, then the communication with Tcl is said
to be \fIsynchronous\fR (it is synchronized to the firing of the star).
Otherwise, it is \fIasynchronous\fR (the Tcl script is responsible for setting
up procedures that will interact with the star only when Tcl invokes them).
.pp
For asynchronous operation, typically X events are bound to Tcl/Tk commands
that read or write data to the star.
These Tcl commands use grabInputs_$starID, which returns a list containing
the current value of each of the inputs, and setOutputs_$starID, which sets
the value of the outputs.
The argument list for setOutputs_$starID should contain a floating-point
value for each output of the star.
The inputs can be of any type.
The print() method of the particle is used to construct a string passed to Tcl.
This mechanism is entirely asynchronous, in that the Tcl/Tk script
decides when these actions should be performed on the basis of X events.
.pp
In synchronous operation, the Tcl procedure goTcl_$starID will be called
by the star every time it fires.
The procedure could, for example, grab input values and compute output values,
although it can do anything the designer wishes, even ignoring the input
and output values.
.pp
For the DE domain, this star checks to see which inputs are new and
sets flags accordingly.
Then, the star calls the Tcl/Tk script which writes new output values to
a buffer in this star.
Finally, after the Tcl/Tk script is finished, the star writes out the
new output values (identified by Tcl) onto the appropriate output ports.
The time stamp on the affected output ports will be the same
as the time stamp on the input port(s) that caused this star to fire.
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
		desc {The file from which to read the Tcl script}
	}
	protected {
		// Standardized interface to Tcl
		TclStarIfc tcl;
	}
	setup {
		tcl.setup( this,
			   input.numberPorts(),
			   output.numberPorts(),
			   (const char*) tcl_file );
		if ( input.numberPorts() == 0 ) {
		  Error::warn( *this, "Tcl/Tk source stars are not yet ",
			       "supported yet in the DE domain." );
		}
	}
	method {
		name{ processInputs }
		type{ void }
		access{ public }
		arglist{ "()" }
		code {
		  // Reset newInputFlags and newOutputFlags in the Tcl object
		  tcl.setAllNewInputFlags(FALSE);
		  tcl.setAllNewOutputFlags(FALSE);

		  // Check the input values to see which ones are new
		  // and set the flags for the affected output ports
		  InDEMPHIter nextp(input);
		  InDEPort* iportp;
		  int portnum = 0;
		  while ((iportp = nextp++) != 0) {
		    if ( iportp->dataNew ) {
		      // set proper entry of newInputFlags of the Tcl object
		      tcl.setOneNewInputFlag(portnum, TRUE);
		    }
		    portnum++;
		  }
		}
	}
	method {
		name{ processOutputs }
		type{ void }
		access{ public }
		arglist{ "()" }
		code {
		  // Output values (computed by Tcl) from the local tcl buffer
		  OutDEMPHIter nextp(output);
		  OutDEPort *oportp;
		  int portnum = 0;
		  while ((oportp = nextp++) != 0) {
		    if ( tcl.outputNewFlags[portnum] ) {
		      oportp->put(completionTime) << tcl.outputValues[portnum];
		    }
		    portnum++;
		  }
		}
	}
	go {
	   completionTime = arrivalTime;

	   // Process the inputs to see which ones are new
	   processInputs();

	   // Invoke the Tcl script (Tcl will call back to get input values)
	   tcl.go();

	   // Send the outputs which are new through the output portholes
	   processOutputs();
	}
}
