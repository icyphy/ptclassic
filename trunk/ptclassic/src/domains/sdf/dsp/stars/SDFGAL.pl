defstar
{
    name { GAL }
    domain { SDF } 
    desc { Gradient Adaptive Lattice filter. }
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
	name { input }
	type { float }
    }

    output
    {
	name { residual }
	type { float}
    }

    state
    {
	name { order }
	type { int }
	default { 1 }
	desc { Lattice filter order. }
    }

    state
    {
	name { timeConstant }
	type { float }
	default { 1.0 }
	desc { Adaptation time constant. }
    }

    protected
    {
	double* f;	// forward prediction errors
	double* b;	// backward predition errors
	double* k;	// reflection coefficients
	double* sigma;	// error power estimates
	double alpha;	// adaptation gain
	double beta;	// error filter parameter
	int previousOrder;
    }

    constructor
    {
	f = b = k = sigma = NULL;
	previousOrder = -1;
    }

    destructor
    {
	LOG_DEL; delete f;
	LOG_DEL; delete b;
	LOG_DEL; delete k;
	LOG_DEL; delete sigma;
    }

    setup
    {
	// Reallocate arrays only if size had changed.
	if (previousOrder != order)
	{
	    // Delete left-over arrays.
	    LOG_DEL; delete f;
	    LOG_DEL; delete b;
	    LOG_DEL; delete k;
	    LOG_DEL; delete sigma;

	    // Allocate new arrays.
	    LOG_NEW; f = new double[order+1];
	    LOG_NEW; b = new double[order+1];
	    LOG_NEW; k = new double[order+1];
	    LOG_NEW; sigma = new double[order+1];

	    previousOrder = order;
	}

	// Zero out old values.
	for(int m = 0; m <= order; m++)
	{
	    f[m] = b[m] = k[m] = sigma[m] = 0.0;
	}

	beta = (timeConstant - 1.0) / (timeConstant + 1.0);
	alpha = 1.0 - beta;
    }

    go
    {
        // update forward errors
        f[0] = input%0;
        for(int m = 1; m <= order; m++)
        {
            f[m] = f[m-1] - k[m]*b[m-1];
        }

	// update backward errors and reflection coefficients
	for(m = order; m > 0; m--)
	{
	    b[m] = b[m-1] - k[m]*f[m-1];
	    sigma[m] *= beta;
	    sigma[m] += (1.0 - beta) * (f[m-1]*f[m-1] + b[m-1]*b[m-1]);
	    if (sigma[m] != 0.0)
	    {
		k[m] += alpha * (f[m]*b[m-1] + b[m]*f[m-1]) / sigma[m];
		if (k[m] > 1.0) k[m] = 1.0;
		if (k[m] < -1.0) k[m] = -1.0;
	    }
	}

    	b[0] = input%0;
    	residual%0 << f[order];
    }
}
