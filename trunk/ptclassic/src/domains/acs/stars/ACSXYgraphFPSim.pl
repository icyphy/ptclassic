defcore {
    name { XYgraph }
    domain { ACS }
    coreCategory { FPSim }
    corona { XYgraph } 
    desc { Generates an X-Y plot with the pxgraph program.
The X data is on "xInput" and the Y data is on "input". }
    version { @(#)ACSXYgraphFPSim.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	setup {
		corona.graph.initialize(this, 1,
			corona.options, corona.title, corona.saveFile, corona.ignore);
	}

	go {
		corona.graph.addPoint(float(corona.xInput%0), float(corona.input%0));
	}

	wrapup {
		corona.graph.terminate();
	}



}

