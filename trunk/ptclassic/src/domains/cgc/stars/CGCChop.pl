defstar {
	name {Chop}
	domain {CGC}
	desc {
On each execution, this star reads a block of "nread" samples (default 128)
and writes "nwrite" of these samples (default 64), skipping the first
"offset" samples (default 0).  It is an error if nwrite + offset > nread.
If nwrite > nread, then the output consists of overlapping windows,
and hence "offset" must be negative.
	}
	version {$Id$}
	author { S. Ha }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	htmldoc {
This star reads a block of particles of any type, and writes a block
of particles that somehow overlaps the input block.
The number of input particles consumed is given by <i>nread</i>,
and the number of output particles produced is given by <i>nwrite</i>.
The <i>offset</i> parameter (default 0) specifies where the output
block should start, relative to the beginning of the input block.
To avoid trying to read samples that have not yet been consumed,
it is required that <i>nwrite  +  offset  &lt;</i>=<i>  nread</i>.
Hence, if <i>nwrite  &gt;  nread</i>, <i>offset</i> must be negative,
and the output consists of overlapping blocks input particles.
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
		desc { Position of output block relative to input block.}
	}
        defstate {
		name {use_past_inputs}
		type {int}
		default {YES}
		desc {
If offset > 0, specify whether to use previously read inputs
(otherwise use zeros).
		}
	}
	defstate {
		name {hiLim}
		type {int}
		default {0}
		desc {
Lower limit on the indexing from the input to the output buffer
		}
		attributes{A_NONSETTABLE|A_NONCONSTANT}
	}
	defstate {
		name {inidx}
		type {int}
		default {0}
		desc {Current index into the output buffer}
		attributes{A_NONSETTABLE|A_NONCONSTANT}
	}
	defstate {
		name {loLim}
		type {int}
		default {0}
		desc {
Lower limit on the indexing from the input to the output buffer
		}
		attributes{A_NONSETTABLE|A_NONCONSTANT}
	}
	constructor {
		noInternalState();
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
                if (int(hiLim) >= int(nwrite)) hiLim = int(nwrite) - 1;
                else if (int(use_past_inputs)) hiLim = int(nwrite) - 1;

                loLim = int(nwrite) - int(nread) - int(offset);
                if (int(loLim) < 0) loLim = 0;

                inidx = int(nread) - int(nwrite) + int(offset);
                if (int(inidx) < 0) inidx = 0;
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
	codeblock(decl) {
	int i;
	}
	codeblock(nonComplexOut) {
	for (i = 0; i < $val(nwrite); i++) {
	    if (i > hiLim || i < loLim) {
		$ref(output,i) = 0;
	    }
	    else {
		$ref(output,i) = $ref(input,inputIndex);
		inputIndex++;
            }
        }
	}
	codeblock(complexOut) {
	for (i = 0; i < $val(nwrite); i++) {
	    if (i > hiLim || i < loLim) {
		$ref(output,i).real = 0;
		$ref(output,i).imag = 0;
	    }
	    else {
		$ref(output,i) = $ref(input,inputIndex);
		inputIndex++;
            }
        }
	}
	go {
		StringList moreDeclarations = "\tint ";
		moreDeclarations << "hiLim = " << int(hiLim) << ", "
				 << "inputIndex = " << int(inidx) << ", "
				 << "loLim = " << int(loLim) << ";\n";

		addCode(decl);
		addCode(moreDeclarations);
		if (strcmp(input.resolvedType(), "COMPLEX") == 0) 
		  addCode(complexOut);
		else
		  addCode(nonComplexOut);
	}
	exectime {
		computeRange();
		return int(nwrite) + 2 * (int(hiLim) - int(loLim));
	}
}
