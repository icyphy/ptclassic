defstar {
	name { LMSPlotCx }
	domain { SDF }
	derivedFrom { LMSCx }
	desc {
Complex adaptive filter using LMS adaptation algorithm.  In addition,
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
This star is exactly like the LMSCx star, except that, in addition,
it makes a plot of the tap coefficients, one plot for the magnitude,
one for the phase.  It can produce two types of plots: a plot
of the final tap values or a plot that traces the
time evolution of each tap value: the time evolution is obtained
if \fItrace\fP is YES.
.Id "adaptive filter, complex"
.Id "filter, adaptive, complex"
.Id "LMS adaptive filter, complex"
.Id "filter, LMS adaptive, complex"
.lp
\fIGraphTitleMag\fP is used for the title of the magnitude plot;
\fIGraphTitlePhase\fP is used for the title of the phase plot;
\fIGraphOptsMag\fP is handed to the xgraph program as option
values on the command line when the magnitudes are plotted, and
\fIGraphOptsPhase\fP serves the same function for the phase.
.lp
If \fItrace\fP is YES, there may not be more than 64 taps in the filter.
	}
	seealso {LMSCx, Xgraph, XMgraph}
	hinclude { "Display.h" }
	state {
		name { graphOptsMag }
		type { string }
		default { "" }
		desc { options for magnitude graph }
	}
	state {
		name { graphOptsPhase }
		type { string }
		default { "" }
		desc { options for phase graph }
	}
	state {
		name { graphTitleMag }
		type { string }
		default { "LMS filter tap magnitudes" }
		desc {title for magnitude graph }
	}
	state {
		name { graphTitlePhase }
		type { string }
		default { "LMS filter tap phases" }
		desc {title for phase graph }
	}
	state {
		name { trace }
		type { int }
		default { "NO" }
		desc {if YES, plot a trace of tap values as they adapt }
	}
	protected {
		XGraph magGraph;
		XGraph phaseGraph;
		int index;
		double *prevPhase;
		double *outPhase;
	}
	constructor {
		// remove saveTapsFile as a settable param
		saveTapsFile.clearAttributes(A_SETTABLE);
		prevPhase = outPhase = 0;
	}
	destructor {
		LOG_DEL; delete prevPhase;
		LOG_DEL; delete outPhase;
	}		
	setup {
		SDFLMSCx::setup();
		LOG_DEL; delete prevPhase;
		LOG_DEL; delete outPhase;
		int nPlots = 1;
		if (int(trace)) {
			nPlots = taps.size();
			LOG_NEW; prevPhase = new double[nPlots];
			LOG_NEW; outPhase = new double[nPlots];
			for (int i = 0; i < nPlots; i++)
				prevPhase[i] = outPhase[i] = 0.0;
			index = 0;
			// enforce a restriction imposed by xgraph program
			if (nPlots > 64) {
				Error::abortRun(*this,
				     ": maximum of 64 taps for trace plot");
				return;
			}
		}
		magGraph.initialize(this, nPlots, graphOptsMag,
			graphTitleMag, "");
		phaseGraph.initialize(this, nPlots, graphOptsPhase,
			graphTitlePhase, "");
	}

        go {
		SDFLMSCx::go();
		if (int(trace)) {
			for (int i = 0; i < taps.size(); i++) {
				double mag = abs(taps[i]);
				double phase = (mag <= 0) ? 0 : arg(taps[i]);

				// handle phase wrapping for continuous plot

				double phaseChange = phase - prevPhase[i];
				if (phaseChange < -M_PI) phaseChange += 2*M_PI;
				if (phaseChange > M_PI) phaseChange -= 2*M_PI;
				outPhase[i] += phaseChange;
				prevPhase[i] = phase;
				magGraph.addPoint(i+1, index, mag);
				phaseGraph.addPoint(i+1, index, outPhase[i]);
			}
			index++;
		}
	}

	wrapup {
		// don't call SDFLMSCx::wrapup: no final file to write.
		if (!int(trace)) {
			// plot final taps
			for (int i = 0; i < taps.size(); i++) {
				double mag = abs(taps[i]);
				double phase = (mag <= 0) ? 0 : arg(taps[i]);
				magGraph.addPoint (mag);
				phaseGraph.addPoint (phase);
			}
		}
		// end the plot
		magGraph.terminate();
		phaseGraph.terminate();
	}
}


