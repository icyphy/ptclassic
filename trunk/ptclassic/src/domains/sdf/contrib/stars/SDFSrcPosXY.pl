defstar {
	name {SrcPosXY}
	domain {SDF}
	version {@(#)SDFSrcPosXY.pl	1.0 9/1/96}
	desc {
This Star produces a vector containing a triangular indicator function for each
direction of arrival (read from the input) in polar coordinates suitable for
display with the TkXYPlot Star.
	}
	author { A. Richter, U. Trautwein }
	copyright {
Copyright (c) 1996 Technical University of Ilmenau.
All rights reserved.
	}
	location { SDF main library }
	explanation {
	}
	ccinclude { "ComplexSubset.h" }
	inmulti {
		name {input}
		type {complex}
		desc {Directions of Arrival}
	}
	output {
		name {outputx}
		type {float}
	}
	output {
		name {outputy}
		type {float}
	}
	defstate {
		name {AngRes}
		type {int}
		default {"360"}
		desc {Angular resolution.}
	}
	defstate {
		name {Decimation}
		type {int}
		default {"1"}
		desc {Decimation Factor.}
	}
	protected {
		int NumberSignals;
		double winc;
		double pix2_d;
		double *costab, *sintab;
		}
	constructor {
	    costab = sintab = 0;
	}
	destructor {
	    LOG_DEL; delete [] costab; 
	    LOG_DEL; delete [] sintab;
	}
	setup {
	    NumberSignals = input.numberPorts(); 
	    pix2_d = 2.0 * M_PI;

	    input.setSDFParams(Decimation,Decimation-1);

	    outputx.setSDFParams(AngRes, AngRes-1);
	    outputy.setSDFParams(AngRes, AngRes-1);

	    LOG_DEL; delete [] costab;
	    LOG_NEW; costab = new double[int(AngRes)];
	    LOG_DEL; delete [] sintab;
	    LOG_NEW; sintab = new double[int(AngRes)];

	    winc=pix2_d/(double)AngRes;
	    double phi;
	    for (int w = 0; w < AngRes ; w++)
	    {
		phi=winc*w;
		costab[w]=cos(phi); sintab[w]=sin(phi);
	    };
	}
	go {
	    MPHIter nextinput(input);
	    PortHole *pinput;
	    double  inp;

	    int w, outindex = 0;
	    while ((pinput = nextinput++) != 0) {
		inp = real((*pinput)%0);
		inp -=(int)(inp/pix2_d)*pix2_d;
		if (inp < 0.0) inp+=pix2_d;
		w = (int)(inp/winc+0.5);
		outputx%outindex << 0.0;
		outputy%(outindex++) << 0.0;
		outputx%outindex << costab[w];
		outputy%(outindex++) << sintab[w];
	    };
	    while (outindex != AngRes) {
		outputx%outindex << 0.0;
		outputy%(outindex++) << 0.0;
	    }
	}
}
