defstar {
	name { CxLMSPlot }
	domain { SDF }
	derivedFrom { CxLMS }
	desc {
Complex adaptive filter using LMS adaptation algorithm.  In addition,
the tap coefficients are plotted using the xgraph program, with a
separate plot for the magnitude and phase.  If "trace" is NO, only the
final tap values are plotted; if it is YES, a trace of each tap is
plotted as it adapts.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF dsp library }
	explanation {
This star is exactly like the CxLMS star, except that, in addition,
it makes a plot of the tap coefficients, one plot for the magnitude,
one for the phase.  It can produce two types of plots: a plot
of the final tap values or a plot that traces the
time evolution of each tap value: the time evolution is obtained
if \fItrace\fP is YES.
.lp
\fGraphTitleMag\fP is used for the title of the magnitude plot;
\fGraphTitlePhase\fP is used for the title of the phase plot;
\fGraphOptsMag\fP is handed to the xgraph program as option
values on the command line when the magnitudes are plotted, and
\fGraphOptsPhase\fP serves the same function for the phase.
.lp
If \fItrace\fP is YES, there may not be more than 64 taps in the filter.
	}
	seealso {CxLMS, Xgraph, XMgraph}
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
	start {
		SDFCxLMS::start();
		delete prevPhase;
		delete outPhase;
		int nPlots = 1;
		if (int(trace)) {
			nPlots = taps.size();
			prevPhase = new double[nPlots];
			outPhase = new double[nPlots];
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
		SDFCxLMS::go();
		if (int(trace)) {
			for (int i = 0; i < taps.size(); i++) {
				double mag = abs(taps[i]);
				double phase = (mag <= 0) ? 0 : arg(taps[i]);

				// handle phase wrapping for continuous plot

				double phaseChange = phase - prevPhase[i];
				if (phaseChange < -PI) phaseChange += 2*PI;
				if (phaseChange > PI) phaseChange -= 2*PI;
				outPhase[i] += phaseChange;
				prevPhase[i] = phase;
				magGraph.addPoint(i+1, index, mag);
				phaseGraph.addPoint(i+1, index, outPhase[i]);
			}
			index++;
		}
	}

	wrapup {
		// don't call SDFCxLMS::wrapup: no final file to write.
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


