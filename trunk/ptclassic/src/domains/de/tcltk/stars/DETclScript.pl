defstar {
	name {TclScript}
	domain {DE}
	version { $Id$ }
	author { Brian Evans and Edward Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
delim off
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
To set all output values at once from Tcl, use
"setOutputs_$starID" with one argument for each output port (giving the new value).
To set one output only, use "setOutput_$starID" with two arguments,
the port number (starting with one) and the new output value.
To read input values use "grabInputs_$starID".
This procedure returns a string with as many values as there are
inputs, separated by spaces.
The Tcl script is sourced in the begin method of the star execution,
so it does not make sense at this time to read inputs.
However, you might want to set outputs at this time.
These outputs will have time stamp zero.
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
the current value of each of the inputs, and setOutputs_$starID or
setOutput_$starID, which sets
the value of the outputs.
The inputs can be of any type;
the print() method of the particle is used to construct a string passed to Tcl.
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
sets flags accordingly.  The Tcl script can read these flags by calling
grabInputsState_$starID; this procedure returns a list of zeros and ones,
as many as there are inputs, indicating whether the corresponding input
value is new.
The Tcl script can write to the outputs either all at once using
setOutputs_$starID, or one by one, using setOutput_$starID.
The time stamp on the affected output ports will be the current time
of the scheduler.  If writing to the outputs occurs in goTcl_$starID
procedure, then the time stamp on the outputs will be the same as
the invocation time of the star.  If it ocurrs asynchronously, then
the time stamp is somewhat arbitrary, in that the current time of the scheduler,
whenever the procedure happens to be invoked, will be used.
.EQ
delim $$
.EN
	}
	hinclude { "TclStarIfc.h" }
	ccinclude { "ptk.h" }
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
	ccinclude { "Scheduler.h" }
	header {
	  class DETclScript;

	  // Overload TclStarIfc for DE-specific functions
	  class TclStarIfcDE : public TclStarIfc {
	  public:
	    /* virtual */ void setOneOutput(int outNum, double outValue);
	    void setParent(DETclScript* p) {parent = p;}
	  private:
	     DETclScript *parent;
	  };
	}
	code {
	  void TclStarIfcDE::setOneOutput (int outNum, double outValue) {
	    if ( outNum >= outputArraySize ) {
	      Error::warn(*myStar, "Too many outputs supplied by Tcl");
	    } else if ( outNum < 0 ) {
	      Error::warn(*myStar, "Negative output port number");
	      return;
	    } else {
	      // Is there a procedure to get the port by number?
	      OutDEMPHIter nextp(parent->output);
	      OutDEPort *oportp = nextp++;
	      for (int portnum=0; portnum < outNum; portnum++) oportp = nextp++;
	      oportp->put(parent->scheduler()->now()) << outValue;
	      oportp->sendData();
	    }
	  }
	}
	protected {
		// Standardized interface to Tcl
		TclStarIfcDE tcl;
	}
	constructor {
	  tcl.setParent(this);
	}
	begin {
		tcl.setup( this,
			   input.numberPorts(),
			   output.numberPorts(),
			   (const char*) tcl_file );
	}
	method {
		name{ processInputs }
		type{ void }
		access{ public }
		arglist{ "()" }
		code {
		  // Reset newInputFlags and newOutputFlags in the Tcl object
		  tcl.setAllNewInputFlags(FALSE);

		  // Check the input values to see which ones are new
		  // and set the flags for the affected output ports
		  InDEMPHIter nextp(input);
		  InDEPort* iportp;
		  int portnum = 0;
		  while ((iportp = nextp++) != 0) {
		    if (iportp->isItOutput()) continue;
		    if ( iportp->dataNew ) {
		      // set proper entry of newInputFlags of the Tcl object
			tcl.setOneNewInputFlag(portnum, TRUE);
			iportp->dataNew = FALSE;
		    }
		    portnum++;
		  }
		}
	}
	go {
	   completionTime = arrivalTime;
	   InfString time = arrivalTime;
	   Tcl_SetVar2(ptkInterp,tcl.id(),"arrivalTime",
		       (char*)time,TCL_GLOBAL_ONLY);
	   
	   // Process the inputs to see which ones are new
	   processInputs();

	   // Invoke the Tcl script (Tcl will call back to get input values)
	   tcl.go();
	}
	wrapup {
	   tcl.wrapup();
        }
}
