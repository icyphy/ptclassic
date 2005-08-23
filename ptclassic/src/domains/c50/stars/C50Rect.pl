defstar {
	name { Rect }
	domain { C50 }
	desc {
Generate a rectangular pulse of height "height" (default 1.0).
and width "width" (default 8).  If "period" is greater than zero,
then the pulse is repeated with the given period.
	}
	version { @(#)C50Rect.pl	1.4	7/29/96 }
	author { Luis Gutierrez, A. Baensch }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	output {
		name {output}
		type {fix}
	}
	state {
		name {height}
		type {fix}
		desc { height of rectangular pulse. }
		default { ONE }
	}
	state {
		name {width}
		type {int}
		desc { width of rectangular pulse. }
		default { 8 }
		attributes { A_SETTABLE | A_UMEM | A_CONSEC }
	}
	state {
		name {periodCounter}
		type {int}
		desc { internal }
		default { 0 }
		attributes {A_UMEM|A_NONCONSTANT|A_NONSETTABLE|A_CONSEC}
	}

	state {
		name {period}
		type {int}
		desc { period of pulse }
		default { 0 }
		attributes { A_SETTABLE | A_UMEM }
	}

	protected {
	// integer representation of height parameter
	int	heightAsInt;
	}

	codeblock(std,""){
	lar	ar0,#$addr(periodCounter)
	mar	*,ar0
	lacc	*,0
	add	#1,0
	sacb
	lacc	*+,16
	sub	*-,16
	lar	ar1,#$addr(output)
	xc	1,lt
	exar
	sacl	*
	lacc	*-,16
	sub	*,16,ar1
	nop
@( int(heightAsInt) > 255  ? "\txc\t2,lt\n":"\txc\t1,lt\n")\
	lacc	#@(int(heightAsInt))
	sacl	*
	}	


	setup {
		int w = width;
		int p = period;
		periodCounter.setInitValue(-1);
		period = (p - 1);

		if (w < 1) 
		    Error::abortRun(*this, "Invalid width value.");
		if (p < 1)    
		    Error::abortRun(*this, "Invalid period value.");
		if (p <= w)
		    Error::abortRun(*this,
				    "Period must be greater than width");
		
		double temp = height.asDouble();
		if ( temp > 0) {
		    heightAsInt = int(32768*temp + 0.5);
		} else {
		    heightAsInt = int(32768*(2+temp) + 0.5);
		};
	}			      

	go {
		addCode(std());
	}

	execTime {
		return 16;
	}
}





