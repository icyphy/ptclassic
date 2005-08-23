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
	version {@(#)SDFChop.pl	1.11	06 Oct 1996}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
This star reads a sequence of input particles of any type, and writes a
sequence of particles constructed from the input sequence (and possibly
zeros).  The number of input particles consumed is given by <i>nread</i>,
and the number of output particles produced is given by <i>nwrite</i>.
The <i>offset</i> parameter (default 0) specifies where in the output
block the first (oldest) input should go.
<p>
A simple use of this star is to pad a block of inputs with zeros.
Set <i>offset</i> to zero and use <i>nwrite &gt; nread</i>.
<a name="zero-padding"></a>
<a name="padding"></a>
Another simple use is to obtain overlapping windows from
an input stream.
Set <i>use_past_inputs</i> to YES, use <i>nwrite &gt; nread</i>,
and set <i>offset</i> equal to <i>nwrite - nread</i>.
<a name="overlapping windows"></a>
<a name="windowing"></a>
<p>
The general operation is illustrated with the following examples.
If offset is positive,
there two possible scenarios, illustrated by the following examples:
<p>
<pre>
<pre>
     iiiiii                  nread = 6
      \    \                 offset = 2
     ppiiiiii00              nwrite = 10
<p>
     iiiiii                  nread = 6
      \ \  \                 offset = 2
     ppiii                   nwrite = 5
</pre>
</pre>
<p>
The symbol "i" refers to any input particle.
The leftmost symbol refers to the oldest input particle of the ones
consumed in a given firing.
The symbol "p" refers to a particle that is either zero
(if <i>use_past_inputs</i> is NO) or is equal to a previously
consumed input particle (if <i>use_past_inputs</i> is YES).
The symbol "0" refers to a zero-valued particle.
In the first of the above examples, the entire input block is
copied to the output, and then filled out with zeros.
In the second example, only a portion of the input block fits.
The remaining input particles are discarded.
<p>
When the <i>offset</i> is negative, the corresponding scenarios are
shown below:
<p>
<pre>
<pre>
     iiiiii                  nread = 6
    / /  /                   offset = -2
     iiii000000              nwrite = 10
<p>
     iiiiii                  nread = 6
    / / //                   offset = -2
     iii                     nwrite = 3
</pre>
</pre>
<p>
In the first of these examples, the first two input particles are
discarded.  In the second example, the first two and the last input
particle are discarded.
<p>
The zero-valued particles are constructed using the
<tt>initialize()</tt>
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
		desc { If offset &gt 0, specify whether to use previously read inputs (otherwise use zeros). }
	}
	protected {
		int hiLim, inidx, loLim;
	}
        method {
	    name { computeRange }
	    type { "void" }
	    arglist { "()" }
	    access { protected }
	    code {
		// Compute the range of output indexes that come from inputs
		// This method is called in the setup() method for the Chop
		// star, and in the go method for ChopVarOffset because
		// it resets the offset parameter
		hiLim = int(nwrite) - int(offset) - 1;
		if (hiLim >= int(nwrite)) hiLim = int(nwrite) - 1;
		else if (int(use_past_inputs)) hiLim = int(nwrite) - 1;

		loLim = int(nwrite) - int(nread) - int(offset);
		if (loLim < 0) loLim = 0;

		inidx = int(nread) - int(nwrite) + int(offset);
		if (inidx < 0) inidx = 0;
	    }
	}
	setup {
		if (int(nread) <= 0) {
		    Error::abortRun(*this, "The number of samples to read ",
				    "must be positive");
		    return;
		}
		if (int(nwrite) <= 0) {
		    Error::abortRun(*this, "The number of samples to write ",
				    "must be positive");
		    return;
		}

		if (int(use_past_inputs))
		    input.setSDFParams(int(nread),int(nread)+int(offset)-1);
		else
		    input.setSDFParams(int(nread),int(nread)-1);
		output.setSDFParams(int(nwrite),int(nwrite)-1);

		computeRange();
	}
	go {
		int inputIndex = inidx;
		for (int i = 0; i < int(nwrite); i++) {
		    if (i > hiLim || i < loLim) {
			(output%i).initialize();
		    }
		    else {
			output%i = input%inputIndex;
			inputIndex++;
		    }
		}
	}
}
