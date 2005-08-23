defstar {
	name { Xscope }
	domain { SDF }
	desc {
Generate a multi-trace plot with the xgraph program.
Successive traces are overlaid on one another.
	}
	derivedFrom { Xgraph }
	version {@(#)SDFXscope.pl	2.13 06 Oct 1996}
	author { J. T. Buck, N. Becker }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
This star is an enhanced version of Xgraph.  It is identical
except that it can plot multiple traces, like an oscilloscope.
As for Xgraph,
the <i>title</i> parameter specifies a title for the plot.
The <i>saveFile</i> parameter optionally specifies a file for
storing the data in a syntax acceptable to xgraph.
A null string prevents any such storage.
The <i>options</i> string is passed directly to the xgraph program
as command-line options.  See the manual section describing xgraph
for a complete explanation of the options.
<p>
Multiple traces may be plotted by setting the <i>traceLength</i>
state to a nonzero value.  In this case, a new plot (starting at
x value zero) is started every <i>traceLength</i> samples.  The
first <i>ignore</i> samples are not plotted; this is useful for letting
transients die away.  The <i>initCount</i> parameter can be used to
offset the traces to control their alignment with respect to the
edge of the plot.
<a name="xgraph program"></a>
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
		name {initCount}
		type {int}
		default {"0"}
		desc { Offset the traces }
	}
	protected {
		int traceCount;
		int nTracesSoFar;
	}

	setup {
		SDFXgraph::setup();
		nTracesSoFar = 0;
		traceCount = int(initCount);
	}

	go {
		if (int(traceLength) > 0 && traceCount >= int(traceLength)) {
			traceCount = 0;
			graph.newTrace();
			index = xInit;	// reinitialize x index
			nTracesSoFar++;
		}
		traceCount++;
		SDFXgraph::go();
	}
}
