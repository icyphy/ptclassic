defstar {
	name {TkPlot}
	domain {SDF}
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
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF tcltk library }
	explanation {
The \fIpersistence\fR parameter specifies the number
of points that are buffered, and hence the number that will
be shown on a plot.  In line-drawing mode, however, where style = "connect",
some points are plotted twice if the \fIrepeat_border_points\fR
parameter is true.
The \fIrepeat_border_points\fR parameter specifies whether the rightmost point
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
	protected {
	  int sampleCount;
	}
	setup {
	  sampleCount = 0;
	}
	go {
	  MPHIter nexty(Y);
	  PortHole *py;
	  int set = 1;
	  while ((py = nexty++) != 0) {
	    xyplot.addPoint(sampleCount,(double)((*py)%0), set++);
	  }
	  sampleCount += 1;
	  if (sampleCount > int(xMax)) {
	    sampleCount = int(xMin);
	    // Plot the points all over again for continuity
	    nexty.reset();
	    set = 1;
	    while ((py = nexty++) != 0) {
	      xyplot.breakPlot(set);
	      if ((int)repeat_border_points) {
		xyplot.addPoint(sampleCount,(double)((*py)%0), set);
	      }
	      set++;
	    }
	    if ((int)repeat_border_points) sampleCount += 1;
	  }
	}
}
