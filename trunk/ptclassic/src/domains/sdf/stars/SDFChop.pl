defstar {
	name {Chop}
	domain {SDF}
	desc {
On each execution, this star reads a block of "nread" particles
and writes them to the output with the given offset.  The number of
particles written is given by "nwrite".  The output block contains
all or part of the input block, depending on "offset" and "nwrite".
The "offset" specifies where in the output block the first (oldest)
particle in the input block will lie.  If "offset" is positive,
then the first "offset" output particles will be either particles
consumed on previous firings (if "use_past_inputs" parameter is YES),
or zero (otherwise).  If "offset" is negative, then the first "offset"
input particles will be discarded.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
This star reads a sequence of input particles of any type, and writes a
sequence of particles constructed from the input sequence (and possibly
zeros).  The number of input particles consumed is given by \fInread\fR,
and the number of output particles produced is given by \fInwrite\fR.
The \fIoffset\fR parameter (default 0) specifies where in the output
block the first (oldest) input should go.
.pp
A simple use of this star is to pad a block of inputs with zeros.
Set \fIoffset\fR to zero and use \fInwrite > nread\fR.
.IE "zero-padding"
.IE "padding"
Another simple use is to obtain overlapping windows from
an input stream.
Set \fIuse_past_inputs\fR to YES, use \fInwrite > nread\fR,
and set \fIoffset\fR equal to \fInwrite - nread\fR.
.IE "overlapping windows"
.IE "windowing"
.pp
The general operation is illustrated with the following examples.
If offset is positive,
there two possible scenarios, illustrated by the following examples:
.lp
.nf
.na
.cs R 18
     iiiiii                  nread = 6
      \\    \\                 offset = 2
     ppiiiiii00              nwrite = 10
.sp
     iiiiii                  nread = 6
      \\ \\  \\                 offset = 2
     ppiii                   nwrite = 5
.cs R
.fi
.ad
.lp
The symbol "i" refers to any input particle.
The leftmost symbol refers to the oldest input particle of the ones
consumed in a given firing.
The symbol "p" refers to a particle that is either zero
(if \fIuse_past_inputs\fR is NO) or is equal to a previously
consumed input particle (if \fIuse_past_inputs\fR is YES).
The symbol "0" refers to a zero-valued particle.
In the first of the above examples, the entire input block is
copied to the output, and then filled out with zeros.
In the second example, only a portion of the input block fits.
The remaining input particles are discarded.
.pp
When the \fIoffset\fR is negative, the corresponding scenarios are
shown below:
.lp
.nf
.na
.cs R 18
     iiiiii                  nread = 6
    / /  /                   offset = -2
     iiii000000              nwrite = 10
.sp
     iiiiii                  nread = 6
    / / //                   offset = -2
     iii                     nwrite = 3
.cs R
.fi
.ad
.lp
In the first of these examples, the first two input particles are
discarded.  In the second example, the first two and the last input
particle are discarded.
.pp
The zero-valued particles are constructed using the
.c initialize()
method of the appropriate particle class.  This returns a
floating point zero, an integer zero, a complex zero, and so forth,
for numerically valued particles.  However, if the particle
is a message particle, then messages of type DUMMY are
used for zeros.
	}
	input {
		name{input}
		type{ANYTYPE}
	}
	output {
		name{output}
		type{=input}
	}
	defstate {
		name {nread}
		type {int}
		default {128}
		desc {Number of particles read.}
	}
	defstate {
		name {nwrite}
		type {int}
		default {64}
		desc {Number of particles written.}
	}
	defstate {
		name {offset}
		type {int}
		default {0}
		desc { Start of output block relative to start of input block. }
	}
	defstate {
		name {use_past_inputs}
		type {int}
		default {YES}
		desc { If offset > 0, specify whether to use previously read inputs (otherwise use zeros). }
	}
	private {
		int hiLim, loLim;
	}
	setup {
		if(int(use_past_inputs))
		    input.setSDFParams(int(nread),int(nread)+int(offset)-1);
		else
		    input.setSDFParams(int(nread),int(nread)-1);
		output.setSDFParams(int(nwrite),int(nwrite)-1);

		// Compute the range of output indexes that come from inputs
		hiLim = int(nwrite)-int(offset)-1;
		if(hiLim >= int(nwrite)) hiLim = int(nwrite)-1;
		else if(int(use_past_inputs)) hiLim = int(nwrite)-1;

		loLim = int(nwrite) - int(nread) - int(offset);
		if(loLim < 0) loLim = 0;
	}
	go {
		int inidx = int(nread) - int(nwrite) + int(offset);
		if(inidx < 0) inidx = 0;

		for (int i = 0; i < int(nwrite); i++) {
		    if (i > hiLim || i < loLim)
			(output%i).initialize();
		    else {
			output%i = input%inidx;
			inidx++;
		    }
		}
	}
}
