defstar {
	name {TkPlot}
	domain {CGC}
	derivedFrom {TkXYPlot}
	desc {
Plot Y input(s) vs. time with dynamic updating.
Two styles are currently supported: "dot" causes
individual points to be plotted, whereas "connect" causes
connected lines to be plotted. Drawing a box in the plot
will reset the plot area to that outlined by the box.
There are also buttons for zooming in and out, and for
resizing the box to just fit the data in view.
	}
	version { @(#)CGCTkPlot.pl	1.8    06 Oct 1996 }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Tcl/Tk library }
	htmldoc {
The <i>persistence</i> parameter specifies the number
of points that are buffered, and hence the number that will
be shown on a plot.  In line-drawing mode, however, where style = "connect",
some points are plotted twice if the <i>repeat_border_points</i>
parameter is true.
The <i>repeat_border_points</i> parameter specifies whether the rightmost point
(those on the right boundary of the plot)
should be repeated on the left boundary when a new trace is started.
These points occupy two positions in the buffer, and hence take two slots of persistence.
	}
        defstate {
	  name { repeat_border_points }
	  type { int }
	  default { YES }
	  desc { For better visual continuity, rightmost events are repeated on the left }
	}
	constructor {
	  // Change default parameter values
	  label.setInitValue("Tk Plot");
	  xTitle.setInitValue("n");
	  xRange.setInitValue("0 100");
	  style.setInitValue("connect");
	  X.setAttributes(P_HIDDEN);
	}
	initCode {
	  addDeclaration("double $starSymbol(sampleCount);");
	  CGCTkXYPlot::initCode();
	  addCode("$starSymbol(sampleCount) = $starSymbol(xMin);");
	}
	go {
	  int i;
	  for (i = 0; i < Y.numberPorts(); i++) {
	    addCode(updateDisplay(i+1));
	  }
	  addCode("$starSymbol(sampleCount) += 1.0;");
	  addCode("if ($starSymbol(sampleCount) > $starSymbol(xMax)) {");
	  addCode("$starSymbol(sampleCount) = $starSymbol(xMin);");
	  // Plot points all over again for continuity
	  for (i = 0; i < Y.numberPorts(); i++) {
	    addCode(breakLine(i+1));
	    if ((int)repeat_border_points) {
	      addCode(updateDisplay(i+1));
	    }
	  }
          if ((int)repeat_border_points) {
	      addCode("$starSymbol(sampleCount) += 1.0; }");
	  }
	  else {
	      addCode("}");
	  }
	}
        codeblock (updateDisplay,"int setnum") {
	  if (ptkPlotPoint(interp, &$starSymbol(plotwin), @setnum,
			   $starSymbol(sampleCount), $ref(Y#@setnum)) == 0) {
	    errorReport("in TkPlot, error plotting data point");
	  }
	}
	codeblock (breakLine, "int setnum") {
	  ptkPlotBreak(interp, &$starSymbol(plotwin), @setnum);
	}
}

