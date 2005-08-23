defstar {
	name { Xscope }
	domain { CGC }
	desc { Generate a multi-trace plot with the pxgraph program. }
	derivedFrom { Xgraph }
	version {@(#)CGCXscope.pl	1.11	06 Oct 1996}
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	htmldoc {
This star is an enhanced version of Xgraph.  It is identical
except that it can plot multiple traces, like an oscilloscope.
As for Xgraph,
the <i>title</i> parameter specifies a title for the plot.
The <i>saveFile</i> parameter optionally specifies a file for
storing the data in a syntax acceptable to pxgraph.
A null string prevents any such storage.
The <i>options</i> string is passed directly to the pxgraph program
as command-line options.  See the manual section describing pxgraph
for a complete explanation of the options.
<p>
Multiple traces may be plotted by setting the <i>traceLength</i>
state to a nonzero value.  In this case, a new plot (starting at
x value zero) is started every <i>traceLength</i> samples.  The
first <i>ignore</i> samples are not plotted; this is useful for letting
transients die away.
<a name="pxgraph program"></a>
<a name="oscilloscope, X window"></a>
<a name="graph, X window, multi-trace"></a>
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
