defstar
{
    name { GGAL }
    derivedFrom { GAL }
    domain { CGC } 
    desc { Ganged Gradient Adaptive Lattice filters. }
    version { $Id$ }
    author { T. M. Parks }
    copyright { 1992 The Regents of the University of California }
    location { CGC local library }

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

    state
    {
	name { B }
	type { floatArray }
	default { "0.0 0.0" }
	desc { Backward prediction error. }
	attributes { A_NONCONSTANT | A_NONSETTABLE }
    }

    codeblock (main)
    {
	{
	    double F;
	    int m;

	    F = $ref(synthIn);
	    for(m = $val(order)-1; m >= 0; m--)
	    {
		F += $ref(k,m+1) * $ref(B,m);
		$ref(B,m+1) = $ref(B,m) - $ref(k,m+1) * F;
	    }
	    $ref(B,0) = F;
	    $ref(synthOut) = F;
	}
    }

    setup
    {
	B.resize(order+1);
	CGCGAL::setup();
    }

    go
    {
	addCode(main);
	CGCGAL::go();
    }
}
