defstar
{
    name { GGAL }
    derivedFrom { GAL }
    domain { SDF } 
    desc { Ganged Gradient Adaptive Lattice filters. }
    version { $Id$ }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { SDF dsp library }

    input
    {
	name { synthIn }
	type { float }
    }

    output
    {
	name { synthOut }
	type { float}
    }

    protected
    {
	double* b;
    }

    constructor
    {
	b = NULL;
    }

    destructor
    {
	LOG_DEL; delete [] b;
    }

    setup
    {	
	// Reallocate array only if size has changed.
	if (previousOrder != order)
	{
	    LOG_DEL; delete [] b;
	    LOG_NEW; b = new double[order+1];
	}

	// Zero out old values.
	for(int m = 0; m <= order; m++)
	{
	    b[m] = 0.0;
	}

	SDFGAL::setup();
    }

    go
    {
	double f;
	f = synthIn%0;

	for(int m = order-1; m >= 0; m--)
	{
	    f += k[m+1]*b[m];
	    b[m+1] = b[m] - k[m+1]*f;
	}
	b[0] = f;

	synthOut%0 << f;

	// update reflection coefficients
	SDFGAL::go();
    }
}
