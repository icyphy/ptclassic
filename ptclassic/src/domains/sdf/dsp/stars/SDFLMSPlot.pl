defstar {
	name { LMSPlot }
	domain { SDF }
	derivedFrom { LMS }
	desc {
Adaptive filter using LMS adaptation algorithm.  In addition,
the tap coefficients are plotted using the xgraph program, with a
separate plot for the magnitude and phase.  If "trace" is NO, only the
final tap values are plotted; if it is YES, a trace of each tap is
plotted as it adapts.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
This star is exactly like the LMS star, except that, in addition,
it makes a plot of the tap coefficients.
.Id "adaptive filter with coefficient plotting"
.Id "filter, adaptive, with coefficient plotting"
.Id "filter, LMS, with coefficient plotting"
.Id "LMS adaptive filter with coefficient plotting"
It can produce two types
of plots: a plot of the final tap values or a plot that traces the
time evolution of each tap value: the time evolution is obtained
if \fItrace\fP is YES.
.lp
\fIgraphTitle\fP is used for the title of the plot; \fIgraphOptions\fP
is handed to the xgraph program as option values on the command line.
.lp
If \fItrace\fP is YES, there may not be more than 64 taps in the filter.
	}
	seealso {LMS, Xgraph, XMgraph}
	hinclude { "Display.h" }
	state {
		name { graphOptions }
		type { string }
		default { "-0 taps" }
		desc { options for graph }
	}
	state {
		name { graphTitle }
		type { string }
		default { "LMS filter tap values" }
		desc {title for graph }
	}
	state {
		name { trace }
		type { int }
		default { "NO" }
		desc {if YES, plot a trace of tap values as they adapt }
	}
	protected {
		XGraph graph;
		int index;
	}
	constructor {
		// remove saveTapsFile as a settable param
		saveTapsFile.clearAttributes(A_SETTABLE);
	}
	setup {
		SDFLMS::setup();
		int nPlots = 1;
		if (int(trace)) {
			nPlots = taps.size();
			index = 0;
			if (nPlots > 64) {
				Error::abortRun(*this,
				     ": maximum of 64 taps for trace plot");
				return;
			}
		}
		graph.initialize(this,nPlots, graphOptions, graphTitle, "");
	}
        go {
		SDFLMS::go();
		if (int(trace)) {
			for (int i = 0; i < taps.size(); i++)
				graph.addPoint(i+1, index, taps[i]);
			index++;
		}
	}
	wrapup {
		// don't call SDFLMS::wrapup: no final file to write.
		if (!int(trace)) {
			// plot final taps
			for (int i = 0; i < taps.size(); i++)
				graph.addPoint (taps[i]);
		}
		// end the plot
		graph.terminate();
	}
}

