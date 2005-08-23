defcore {
	name { IDCT }
	domain { ACS }
	coreCategory { FixSim }
	corona { IDCT }
	desc { computes the inverse discrete cosine transform of a block of 8 samples }
	version { @(#)ACSIDCTFixSim.pl	1.2 09/08/99 }
	author { James Lundblad }
    	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }
        defstate {
                name { ArrivingPrecision }
                type {int}
                default {"YES"}
                desc {
Flag indicating whether or not to use the arriving particles as they are:
YES keeps the same precision, and NO casts them to the precision specified
by the parameter "InputPrecision".
		}
        }
        defstate {
                name { InputPrecision }
                type { precision }
                default { 4.14 }
                desc {
Precision of the input in bits.
The input particles are only cast to this precision if the parameter
"ArrivingPrecision" is set to NO.
		}
        }
        defstate {
                name { CoeficientPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the taps in bits. }
        }
        defstate {
                name { InternalPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the accumulation in bits. }
        }
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the output in bits. } 
        }
	ccinclude { <math.h> }
	protected {
		Fix oneOverSqrt2, W1, W2, W3, W5, W6, W7;
		Fix x0, x1, x2, x3, x4, x5, x6, x7, x8;
		Fix fixIn, out;
	}
	setup {
		ACSFixSimCore::setup();
		corona.input.setSDFParams(8,7);
		corona.output.setSDFParams(8,7);
		fixIn = Fix( ((const char *) InputPrecision) );
		if ( fixIn.invalid() )
		  Error::abortRun( *this, "Invalid InputPrecision" );

		oneOverSqrt2 = Fix( ((const char *) CoeficientPrecision) );
		if ( oneOverSqrt2.invalid() )
		  Error::abortRun( *this, "Invalid CoeficientPrecision" );

		W1 = Fix( ((const char *) CoeficientPrecision) ); 
		W2 = Fix( ((const char *) CoeficientPrecision) ); 
		W3 = Fix( ((const char *) CoeficientPrecision) ); 
		W5 = Fix( ((const char *) CoeficientPrecision) ); 
		W6 = Fix( ((const char *) CoeficientPrecision) ); 
		W7 = Fix( ((const char *) CoeficientPrecision) ); 

		x0 = Fix( ((const char *) InternalPrecision) );
		if ( x0.invalid() )
			Error::abortRun( *this, "Invalid InternalPrecision" );
		x1 = Fix( ((const char *) InternalPrecision) );
		x2 = Fix( ((const char *) InternalPrecision) );
		x3 = Fix( ((const char *) InternalPrecision) );
		x4 = Fix( ((const char *) InternalPrecision) );
		x5 = Fix( ((const char *) InternalPrecision) );
		x6 = Fix( ((const char *) InternalPrecision) );
		x7 = Fix( ((const char *) InternalPrecision) );
		x8 = Fix( ((const char *) InternalPrecision) );

		out = Fix( ((const char *) OutputPrecision) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );

		// Set the overflow handlers for assignments/computations
		fixIn.set_ovflow( ((const char *) OverflowHandler) );
		if ( fixIn.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );

		oneOverSqrt2.set_ovflow( ((const char *) OverflowHandler) );
		W1.set_ovflow( ((const char *) OverflowHandler) );
		W2.set_ovflow( ((const char *) OverflowHandler) );
		W3.set_ovflow( ((const char *) OverflowHandler) );
		W5.set_ovflow( ((const char *) OverflowHandler) );
		W6.set_ovflow( ((const char *) OverflowHandler) );
		W7.set_ovflow( ((const char *) OverflowHandler) );

		x0.set_ovflow( ((const char *) OverflowHandler) );
		x1.set_ovflow( ((const char *) OverflowHandler) );
		x2.set_ovflow( ((const char *) OverflowHandler) );
		x3.set_ovflow( ((const char *) OverflowHandler) );
		x4.set_ovflow( ((const char *) OverflowHandler) );
		x5.set_ovflow( ((const char *) OverflowHandler) );
		x6.set_ovflow( ((const char *) OverflowHandler) );
		x7.set_ovflow( ((const char *) OverflowHandler) );
		x8.set_ovflow( ((const char *) OverflowHandler) );

		out.set_ovflow( ((const char *) OverflowHandler) );

		oneOverSqrt2.set_rounding( int(RoundFix) );
		W1.set_rounding( int(RoundFix) );
		W2.set_rounding( int(RoundFix) );
		W3.set_rounding( int(RoundFix) );
		W5.set_rounding( int(RoundFix) );
		W6.set_rounding( int(RoundFix) );
		W7.set_rounding( int(RoundFix) );

		oneOverSqrt2 = 1.0/sqrt(2.0);
		W1 = M_SQRT2*cos(M_PI/16.0);
		W2 = M_SQRT2*cos(2.0*M_PI/16.0);
		W3 = M_SQRT2*cos(3.0*M_PI/16.0);
		W5 = M_SQRT2*cos(5.0*M_PI/16.0);
		W6 = M_SQRT2*cos(6.0*M_PI/16.0);
		W7 = M_SQRT2*cos(7.0*M_PI/16.0);

		

		fixIn.set_rounding( int(RoundFix) );

		x0.set_rounding( int(RoundFix) );
		x1.set_rounding( int(RoundFix) );
		x2.set_rounding( int(RoundFix) );
		x3.set_rounding( int(RoundFix) );
		x4.set_rounding( int(RoundFix) );
		x5.set_rounding( int(RoundFix) );
		x6.set_rounding( int(RoundFix) );
		x7.set_rounding( int(RoundFix) );
		x8.set_rounding( int(RoundFix) );

		out.set_rounding( int(RoundFix) );
	}
	go {
		Fix tmp1 = corona.input%4;
		Fix tmp2 = corona.input%6;
		Fix tmp3 = corona.input%2;
		Fix tmp4 = corona.input%1;
		Fix tmp5 = corona.input%7;
		Fix tmp6 = corona.input%5;
		Fix tmp7 = corona.input%3;
		Fix tmp8 = corona.input%0;
		if ( int(ArrivingPrecision) ) {
			x1 = tmp1;
			checkOverflow(x1);
			x2 = tmp2;
			checkOverflow(x2);
			x3 = tmp3;
			checkOverflow(x3);
			x4 = tmp4;
			checkOverflow(x4);
			x5 = tmp5;
			checkOverflow(x5);
			x6 = tmp6;
			checkOverflow(x6);
			x7 = tmp7;
			checkOverflow(x7);
			x0 = tmp8;
			checkOverflow(x0);
		} else {
			fixIn = tmp1;
			x1 = fixIn;
			checkOverflow(x1);
			fixIn = tmp2;
			x2 = fixIn;
			checkOverflow(x2);
			fixIn = tmp3;
			x3 = fixIn;
			checkOverflow(x3);
			fixIn = tmp4;
			x4 = fixIn;
			checkOverflow(x4);
			fixIn = tmp5;
			x5 = fixIn;
			checkOverflow(x5);
			fixIn = tmp6;
			x6 = fixIn;
			checkOverflow(x6);
			fixIn = tmp7;
			x7 = fixIn;
			checkOverflow(x7);
			fixIn = tmp8;
			x0 = fixIn;
			checkOverflow(x0);
		}


 		 /* first stage */
  		x8 = W7*(x4+x5);
		checkOverflow(x8);
  		x4 = x8 + (W1-W7)*x4;
		checkOverflow(x4);
  		x5 = x8 - (W1+W7)*x5;
		checkOverflow(x5);
  		x8 = W3*(x6+x7);
		checkOverflow(x8);
  		x6 = x8 - (W3-W5)*x6;
		checkOverflow(x6);
  		x7 = x8 - (W3+W5)*x7;
		checkOverflow(x7);



  		/* second stage */
  		x8 = x0 + x1;
		checkOverflow(x8);
  		x0 -= x1;
		checkOverflow(x0);
  		x1 = W6*(x3+x2);
		checkOverflow(x1);
  		x2 = x1 - (W2+W6)*x2;
		checkOverflow(x2);
  		x3 = x1 + (W2-W6)*x3;
		checkOverflow(x3);
  		x1 = x4 + x6;
		checkOverflow(x1);
  		x4 -= x6;
		checkOverflow(x4);
  		x6 = x5 + x7;
		checkOverflow(x6);
  		x5 -= x7;
		checkOverflow(x5);
  
  		/* third stage */
  		x7 = x8 + x3;
		checkOverflow(x7);
  		x8 -= x3;
		checkOverflow(x8);
  		x3 = x0 + x2;
		checkOverflow(x3);
  		x0 -= x2;
		checkOverflow(x0);
  		x2 = oneOverSqrt2*(x4+x5);
		checkOverflow(x2);
  		x4 = oneOverSqrt2*(x4-x5);
		checkOverflow(x4);

		out = (x7+x1);
		checkOverflow(out);
		corona.output%0 << out;
		out = (x3+x2);
		checkOverflow(out);
		corona.output%1 << out;
		out = (x0+x4);
		checkOverflow(out);
		corona.output%2 << out;
		out = (x8+x6);
		checkOverflow(out);
		corona.output%3 << out;
		out = (x8-x6);
		checkOverflow(out);
		corona.output%4 << out;
		out = (x0-x4);
		checkOverflow(out);
		corona.output%5 << out;
		out = (x3-x2);
		checkOverflow(out);
		corona.output%6 << out;
		out = (x7-x1);
		checkOverflow(out);
		corona.output%7 << out;
	}
}
