defstar {
	name {TkBreakPt}
	domain {SDF}
	derivedFrom { TclScript }
	desc {
A conditional break point.  
Each time this star executes, it evaluates its conditional expression.  
If the expression evaluates to true, it causes the run to pause.
	}
	version { $Id$ }
	author { Alan Kamas }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF tcltk library }
	explanation {
.EQ
delim off
.EN
This star evaluates a tcl expression each time it is fired.  If the expression
returns a value of "true" then the run is paused.
The expression can be any valid tcl expression.  For details of valid tcl
expressions, see the tcl command "expr."
.pp
This star takes multiple inputs.  To make it easier to write a conditional
expression, the inputs are numbered: input(1), input(2), input(3), etc.
Thus if you only had one input connected to a TkBreakPt star, you would
use input(1) to refer to it.
.pp
Remember that Tcl uses a dollar sign ($) to reference the value of a 
variable.  Thus, the expression
.br
$input(1) < 0
.br
will be true if the value
of the first input is negative.  Similarly, the expression
.br
$input(1) < $input(2)
.br
will be true whenever the value of the first input is less than the value 
of the second input.
.pp
It may be difficult to distinguish which input is which if they are
all connected directly to the multiport input.  One solution is to
use the "Bus create" icons in the HOF functions palette.  Connect
the bus create icon of your choice to the input of the TkBreakPt star.
Then connect the inputs to the bus create icon.  The top input is 
input(1), the next is input(2), etc.
.pp
The second parameter, Optional_Alternate_Script is the script to source if the 
condition evaluates to "true".  If this parameter is left blank (the default)
then the default script is executed which pauses the run and 
puts up a message in the Run Control block.  This should be fine for
most applications of this star.
.pp
The Optional_Alternate_Script is sourced from the goTcl_$starID proceedure.  It
has access to the following variables:
.br
$starID() : The $starID associative array.  See the programmer's manual for
details.
.br
input(1), input(2), etc. : The inputs to this star.  See above for the details
of the numbering.
.br
$ptkControlPanel : The current Run Control window.
} 

	hinclude { "ptk.h" }
	defstate {
	        name {condition}
	        type {string}
	        default { "$input(1) < 0" }
	        desc {Condition on which to pause the run}
	}
	defstate {
	        name {Optional_Alternate_Script}
	        type {string}
	        default{""}
	        desc {Script to run instead of the default when the condition is true.  The default script pauses the run.}
	}
	setup {
	    if (output.numberPorts() > 0) {
		Error::abortRun(*this, "Outputs not supported");
		return;
	    }
	}
	begin {
	    // Set parameter values that are not user settable.
	    tcl_file =
		"$PTOLEMY/src/domains/sdf/tcltk/stars/tkBreakPt.tcl";

	    SDFTclScript::begin();
	}
	constructor {
	    // Hide irrelevant outputs and states.
	    output.setAttributes(P_HIDDEN);
	    tcl_file.clearAttributes(A_SETTABLE);
	}
	go {
	  // call the go method in the parent.
	  SDFTclScript::go();
	  
	  // make sure that the break point is processed NOW
          while (Tk_DoOneEvent(TK_DONT_WAIT|TK_ALL_EVENTS));
	}
}
