defstar {
	name { Xscope }
	domain { CGC }
	desc { Generate a multi-trace plot with the pxgraph program. }
	derivedFrom { Xgraph }
	version {$Id$}
	author { S. Ha }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
This star is an enhanced version of Xgraph.  It is identical
except that it can plot multiple traces, like an oscilloscope.
As for Xgraph,
the \fItitle\fR parameter specifies a title for the plot.
The \fIsaveFile\fR parameter optionally specifies a file for
storing the data in a syntax acceptable to pxgraph.
A null string prevents any such storage.
The \fIoptions\fR string is passed directly to the pxgraph program
as command-line options.  See the manual section describing pxgraph
for a complete explanation of the options.
.pp
Multiple traces may be plotted by setting the \fItraceLength\fR
state to a nonzero value.  In this case, a new plot (starting at
x value zero) is started every \fItraceLength\fR samples.  The
first \fIignore\fR samples are not plotted; this is useful for letting
transients die away.
.Ir "pxgraph program"
.Id "oscilloscope, X window"
.Id "graph, X window, multi-trace"
	}
	defstate {
		name {traceLength}
		type {int}
		default {"0"}
		desc { Number of samples per trace.  If 0, only one trace. }
	}
	defstate {
		name {traceCount}
		type {int}
		default {"0"}
		desc { Counter for samples in trace interval }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name {nTracesSoFar}
		type {int}
		default {"0"}
		desc { Counter for trace intervals }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}

	setup {
	    traceCount= 0-(int)ignore;
	    CGCXgraph::setup();
	}
	go {
		if (int(traceLength) > 0) {
@	if ($ref(traceCount) >= $val(traceLength)) {
@		$ref(traceCount) = 0;
@		fprintf($starSymbol(fp), "move ");
@		$ref(index) = $val(xInit);
@		$ref(nTracesSoFar)++;
@	}
@	$ref(traceCount)++;
@	if (!$ref(traceCount)) $ref(index) = 0;
		}

		CGCXgraph::go();
	}

	exectime {
		return 8;
	}
}
