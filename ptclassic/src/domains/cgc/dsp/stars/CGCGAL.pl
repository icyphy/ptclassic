defstar
{
    name { GAL }
    domain { CGC } 
    desc { Gradient Adaptive Lattice filter. }
    version { $Id$ }
    author { T. M. Parks }
    copyright { 1992 The Regents of the University of California }
    location { CGC local library }

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

    state
    {
	name { alpha }
	type { float }
	default { 0.0 }
	desc { Adaptation parameter. }
	attributes { A_NONSETTABLE }
    }

    state
    {
	name { k }
	type { floatArray }
	default { "0.0" }
	desc { Reflection coefficients. }
	attributes { A_NONCONSTANT | A_NONSETTABLE }
    }

    state
    {
	name { f }
	type { floatArray }
	default { "0.0 0.0" }
	desc { Forward prediction error. }
	attributes { A_NONCONSTANT | A_NONSETTABLE }
    }

    state
    {
	name { b }
	type { floatArray }
	default { "0.0 0.0" }
	desc { Backward prediction error. }
	attributes { A_NONCONSTANT | A_NONSETTABLE }
    }

    state
    {
	name { e }
	type { floatArray }
	default { "0.0 0.0" }
	desc { Error power estimate. }
	attributes { A_NONCONSTANT | A_NONSETTABLE }
    }

    codeblock (main)
    {
	{
	    int m;

	    /* Update forward errors. */
	    $ref(f,0) = $ref(input);
	    for(m = 1; m <= $val(order); m++)
	    {
	       $ref(f,m) = $ref(f,m-1) - $ref(k,m) * $ref(b,m-1);
	    }

	    /* Update backward errors, reflection coefficients. */
	    for(m = $val(order); m > 0; m--)
	    {
		$ref(b,m) = $ref(b,m-1) - $ref(k,m)*$ref(f,m-1);
		$ref(e,m) *= 1.0 - $val(alpha);
		$ref(e,m) += $val(alpha) * ($ref(f,m-1)*$ref(f,m-1) + $ref(b,m-1)*$ref(b,m-1));
		if ($ref(e,m) != 0.0)
		{
		    $ref(k,m) += $val(alpha) * ($ref(f,m)*$ref(b,m-1) + $ref(b,m)*$ref(f,m-1)) / $ref(e,m);
		    if ($ref(k,m) > 1.0) $ref(k,m) = 1.0;
		    if ($ref(k,m) < -1.0) $ref(k,m) = -1.0;
		}
	    }

	    $ref(b,0) = $ref(input);
	    $ref(residual) =  $ref(f,order);
	}
    }

    setup
    {
	k.resize(order+1);
	f.resize(order+1);
	b.resize(order+1);
	e.resize(order+1);
	alpha = 1.0 - ((timeConstant - 1.0) / (timeConstant + 1.0));
    }

    go
    {
	addCode(main);
    }
}
