defstar {
	name { LMSPlot }
	domain { SDF }
	derivedFrom { LMS }
	desc {
This star is exactly like the LMS star, except that, in addition
to the functions of LMS, it makes a plot of the tap coefficients.
It can produce two types of plots: a plot of the final tap values
or a plot that traces the time evolution of each tap value.
The time evolution is obtained if the value of the parameter "trace" is "YES".
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
.Id "adaptive filter with coefficient plotting"
.Id "filter, adaptive, with coefficient plotting"
.Id "filter, LMS, with coefficient plotting"
.Id "LMS adaptive filter with coefficient plotting"
The value of the \fIgraphTitle\fP parameter
is used for the title of the plot; the value of \fIgraphOptions\fP
is handed to the xgraph program as option values on the command line.
.lp
If \fItrace\fP is YES, then there may not be more than 64 taps in the filter,
since this is the maximum number of plots that the xgraph program can handle.
	}
	seealso {LMS, XMgraph, LMSTkPlot, LMSPlotCx, LMSTkPlotCx}
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

