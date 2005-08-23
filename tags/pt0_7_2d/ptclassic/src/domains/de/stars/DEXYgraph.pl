defstar {
	name { XYgraph }
	derivedFrom { Xgraph }
	domain { DE }
	version { @(#)DEXYgraph.pl	1.2 06/24/97 }
	author { J. Voigt }
	copyright { copyright (c) 1997 Dresden University of Technology
                    Mobile Communications Systems
	}
	location { The WiNeS-Library. }
        desc { Generates an X-Y plot with the xgraph program.
        The X data is on "xInput" and the Y data is on "input".
	}
	explanation { This star is mainly the same like J. Buck's XYgraph star 
        for the SDF-domain.
	}
	input {
            name { xInput }
            type { anytype }
	}
	go {
            graph.addPoint(float(xInput.get()), float(input.get()));
        }
    }
