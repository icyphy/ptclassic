defstar {
	name {BeamXYNorm}
	domain {SDF}
	version { $Id$ }
	desc {
Outputs the magnitude of the antenna array beam pattern with respect to the
antenna positions and the actual steering vector. The output is in polar
coordinates suitable for display with the TkXYPlot Star.
	}
	author { A. Richter and U. Trautwein }
	copyright {
Copyright (c) 1996-1997 Technical University of Ilmenau.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF contribution library }
	explanation {
	}
	ccinclude { "ComplexSubset.h" }
	input {
	    name {steering}
	    type {complex}
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
		name {Positions}
		type {complexarray}
		default {"(0.0,0.0)"}
		desc {Antenna positions.}
	}
	defstate {
		name {AngRes}
		type {int}
		default {"360"}
		desc {Angular resolution.}
	}
	defstate {
		name {wavelength}
		type {float}
		default {"0.30"}
		desc {Wavelength.}
	}
	defstate {
	        name {Decimation}
		type {int}
		default {"1"}
		desc {Input Decimation Factor.}
	}
	protected {
		int NumberElements;
		// store one set of steering values for output
		Complex *dirvectors;
		Complex *ScaledSteering;
		double *costab;
		double *sintab;
		}
	constructor {
	    dirvectors = 0; costab = sintab = 0; ScaledSteering = 0;
	}
	destructor {
	    LOG_DEL; delete [] dirvectors;
	    LOG_DEL; delete [] costab; 
	    LOG_DEL; delete [] sintab;
	    LOG_DEL; delete [] ScaledSteering;
	}
	setup {
	    NumberElements = Positions.size();

	    steering.setSDFParams(Decimation, NumberElements-1);

	    outputx.setSDFParams(AngRes, AngRes-1);
	    outputy.setSDFParams(AngRes, AngRes-1);

	    LOG_DEL; delete [] dirvectors;
	    LOG_NEW; dirvectors = new Complex[NumberElements*int(AngRes)];
	    LOG_DEL; delete [] costab;
	    LOG_NEW; costab = new double[int(AngRes)];
	    LOG_DEL; delete [] sintab;
	    LOG_NEW; sintab = new double[int(AngRes)];
	    LOG_DEL; delete [] ScaledSteering;
	    LOG_NEW; ScaledSteering = new Complex[NumberElements];

	    double wi=2.0*M_PI/double(AngRes);
	    double phi;
	    Complex TranPos;
	    Complex w2pij; w2pij=Complex(0.0,-2.0*M_PI/double(wavelength));
	    Complex *p;
	    double winc=2.0*M_PI/(double)AngRes;

	    p=dirvectors;
	    for (int w = 0; w < AngRes ; w++)
	    {
		phi=wi*(double) w;
		TranPos=27e3*exp(Complex(0.0,phi));
		
		for (int element = 0; element < NumberElements; element++)
		{
		    
		    *(p++)=exp(w2pij*Complex(abs(TranPos-Positions[element]),0));
		    costab[w]=cos(winc*w); sintab[w]=sin(winc*w);
		 }
	     }
	}
	go {
	    double Gabs, Wscale;
	    Complex G;
	    Complex *p;
	    int i;
	    
	    Wscale = 0.0;
	    for( i = 0; i < NumberElements; i++)
	       Wscale+= abs(Complex(steering%(i)));
	    for( i = 0; i < NumberElements; i++)
	       ScaledSteering[i]=Complex(steering%(NumberElements-i-1))/Complex(Wscale,0.0);

	    p=dirvectors;
	    for (int w = 0; w < AngRes ; w++)
	    {
		G=0;
		for (int element = 0; element < NumberElements; element++)
		    G+=*(p++) * conj(ScaledSteering[element]);

 		Gabs=abs(G);
		outputx%w << Gabs*costab[w];
		outputy%w << Gabs*sintab[w];
	    }
	}
}

