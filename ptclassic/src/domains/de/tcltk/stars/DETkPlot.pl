defstar {
	name {TkPlot}
	domain {DE}
	derivedFrom {TkXYPlot}
	desc {
Plot Y input(s) vs. time with dynamic updating.
Retracing is done to overlay successive time intervals,
as in an oscilloscope.
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
The \fIrepeat_border_points\fR parameter specifies whether the rightmost events
(with time stamps less than the upper end of \fIxRange\fR)
should be repeated off the left when a new trace is started.
These points occupy two positions in the buffer, and hence take two slots of persistence.
        }
        defstate {
	  name { repeat_border_points }
	  type { int }
	  default { YES }
	  desc { For better visual continuity, rightmost events are repeated on the left }
	}
        protected {
	  double *prevValue;
	  double *prevTime;
	  double *prevXpos;
	  int *prevValueSet;
	}
	constructor {
	  // Change default parameter values
	  label.setInitValue("Tk Plot");
	  xTitle.setInitValue("time");
	  xRange.setInitValue("0 100");
	  style.setInitValue("connect");
	  X.setAttributes(P_HIDDEN);
	  prevValue = NULL;
	  prevTime = NULL;
	  prevXpos = NULL;
	  prevValueSet = NULL;
	}
        begin {
	  delete [] prevValue;
	  delete [] prevTime;
	  delete [] prevXpos;
	  delete [] prevValueSet;
	  prevValue = new double[Y.numberPorts()];
	  prevTime = new double[Y.numberPorts()];
	  prevXpos = new double[Y.numberPorts()];
	  prevValueSet = new int[Y.numberPorts()];
	  for (int i = 0; i < Y.numberPorts(); i++) {
	    prevValueSet[i] = 0;
	    prevTime[i] = 0.0;
	    prevXpos[i] = 0.0;
	  }
          DETkXYPlot::begin();
	}
        destructor {
	  delete [] prevValue;
	  delete [] prevTime;
	  delete [] prevXpos;
	  delete [] prevValueSet;
	}
	go {
	  InDEMPHIter nexty(Y);
	  InDEPort *py;
	  // The trace number
	  int trace = 0;
	  while ((py = nexty++) != 0) {
	    if (py->dataNew) {
	      double xpos = prevXpos[trace] + arrivalTime - prevTime[trace];
	      if (xpos >= xMax) {
		if ((int)repeat_border_points) {
		  // Plot point off the right edge
		  xyplot.addPoint(xpos,(double)((*py)%0), trace+1);
		  if (prevValueSet[trace]) {
		    // Adjust time for previous point
		    while (prevXpos[trace] > xMin) {
		      prevXpos[trace] -= (xMax - xMin);
		    }
		    // Plot the point off the left edge
		    xyplot.breakPlot(trace+1);
		    xyplot.addPoint(prevXpos[trace],prevValue[trace], trace+1);
                  }
                } else {
		  xyplot.breakPlot(trace+1);
		}
	        while (xpos > xMax) {
		  xpos -= (xMax - xMin);
		}
	      }
	      xyplot.addPoint(xpos,(double)(py->get()), trace+1);
	      prevXpos[trace] = xpos;
	      prevTime[trace] = arrivalTime;
	      prevValue[trace] = (double)((*py)%0);
	      prevValueSet[trace] = 1;
	    }
	    trace++;
	  }
	}
}
