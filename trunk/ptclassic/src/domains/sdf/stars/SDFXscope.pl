defstar {
	name { Xscope }
	domain { SDF }
	desc { Generate a multi-trace plot with the xgraph program. }
	derivedFrom { Xgraph }
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
This star is an enhanced version of Xgraph.  It is identical
except that it can plot multiple traces, like an oscilloscope.
As for Xgraph,
the \fItitle\fR parameter specifies a title for the plot.
The \fIsaveFile\fR parameter optionally specifies a file for
storing the data in a syntax acceptable to xgraph.
A null string prevents any such storage.
The \fIoptions\fR string is passed directly to the xgraph program
as command-line options.  See the manual section describing xgraph
for a complete explanation of the options.
.pp
Multiple traces may be plotted by setting the \fItraceLength\fR
state to a nonzero value.  In this case, a new plot (starting at
x value zero) is started every \fItraceLength\fR samples.  The
first \fIignore\fR traces (not samples, as the state descriptor says)
are not plotted; this is useful for letting transients die away.
	}
	defstate {
		name {traceLength}
		type {int}
		default {"0"}
		desc { Number of samples per trace.  If 0, only one trace. }
	}
	protected {
		int traceCount;
		int nTracesSoFar;
	}

	start {
		SDFXgraph::start();
		traceCount = 0;
		nTracesSoFar = 0;
		// we will handle the ignoring here.
		graph.setIgnore(0);
	}

	go {
		if (int(traceLength) > 0 && traceCount >= int(traceLength)) {
			traceCount = 0;
			if (nTracesSoFar >= int(ignore))
				graph.newTrace();
			nTracesSoFar++;
		}
		traceCount++;
		if (nTracesSoFar >= int(ignore))
			SDFXgraph::go();
	}
}
