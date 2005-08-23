defstar {
	name {BeamAbs}
	domain {SDF}
	version {@(#)SDFBeamAbs.pl	1.8 3/2/95}
	desc {
Outputs the magnitude of the antenna array beam pattern with respect to the 
antenna positions and the actual steering vector.
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
	input {
	    name {steering}
	    type {complex}
	}	
	output {
		name {output}
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
	protected {
		int NumberElements;
		// store one set of steering values for output
		Complex *dirvectors;
		}
	constructor {
	    dirvectors = 0;
	}
	destructor {
	    LOG_DEL; delete [] dirvectors;
	}
	setup {
	    NumberElements = Positions.size();

	    steering.setSDFParams(NumberElements, NumberElements-1);
	    output.setSDFParams(AngRes, AngRes-1);

	    LOG_DEL; delete [] dirvectors;
	    LOG_NEW; dirvectors = new Complex[NumberElements*int(AngRes)];

	    double wi=2.0*M_PI/double(AngRes);
	    double phi;
	    Complex TranPos;
	    Complex w2pij; w2pij=Complex(0.0,-2.0*M_PI/double(wavelength));
	    Complex *p;

	    p=dirvectors;
	    for (int w = 0; w < int(AngRes) ; w++)
	    {
		phi=wi*(double) w;		
		TranPos=27e3*exp(Complex(0.0,phi));
		
		for (int element = 0; element < NumberElements; element++)
		{		    
		    *(p++)=exp(w2pij*Complex(abs(TranPos-Positions[element]),0));
		 }
	     }
	}
	go {
	    Complex G;
	    Complex *p;

	    p=dirvectors;
	    for (int w = AngRes-1; w >=0 ; w--)
	    {
		G=0;
		for (int element = 0; element < NumberElements; element++)
		    G+=*(p++) * conj(Complex(steering%(NumberElements-element-1)));
		output%w << abs(G);		    
	    }
	}
}
