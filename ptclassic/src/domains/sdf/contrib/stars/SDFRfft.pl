defstar {
  name {Rfft}
  domain {SDF}
  desc {
Compute the fft of a real input, using the '2-channel trick'.
InputSize is number of input samples.
fftSize is the size of fft.  zero padding is used if 
  fftSize > InputSize
Inverse if Inverse == 1
Use Hamming window if UseWindow
The output is half the size of the input.
	     }
  author { N. Becker }
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
  location { SDF user contribution library }
	input {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {complex}
	}
	defstate {
		name {InputSize}
		type {int}
		default {256}
		desc {Number of input samples to read.}
	}
	defstate {
		name {fftSize}
		type {int}
		default {256}
		desc {Size of the transform.}
	}
	defstate {
		name {Inverse}
		type {int}
		default {0}
		desc { Equals 1 for inverse. }
	}
	defstate {
		name {UseWindow}
		type {int}
		default {0}
		desc { Equals 1 to use Hamming window }
	}
	protected {
	  Complex *Temp1;
	  Complex *Temp2;
	  double *In;
	}
  constructor {
    Temp1 = 0;
    Temp2 = 0;
    In = 0;
  }
  destructor {
    delete Temp1;
    delete Temp2;
    delete In;
  }
  setup {
    delete Temp1;
    Temp1 = new Complex[ (int)fftSize/2 ];
    delete Temp2;
    Temp2 = new Complex[ (int)fftSize/2 ];
    delete In;
    In = new double[ (int)fftSize ];
    input.setSDFParams( (int)InputSize, (int)InputSize-1 );
    output.setSDFParams( (int)fftSize / 2, ((int)fftSize/2)-1 );
  }
  hinclude {"fft.h"}

  inline method {
    name { WinVal }
    type { double }
    arglist {"(double t)"}
    code {
      return .54 + .46 * cos( Pi2 * t );
    }

  }

  go {
    fft F( (int)Inverse, (int)fftSize / 2 );

    for (int i = 0; i < (int)InputSize; i++ )
      In[ (int)InputSize - i - 1 ] = input%i;
    for (i = (int)InputSize; i < (int)fftSize; i++ )
      In[ i ] = 0;

    double WinNorm;
    if( (int)UseWindow ) {
      double Sum = 0;
      for( i = 0; i < (int)fftSize; i++ ) {
	double TimeArg = ( (double)i / ( (double)(int)fftSize - 1.) ) - 0.5;
	double WV = WinVal( TimeArg );
	In[ i ] *= WV;
	Sum += WV;
      }
      WinNorm = (int)fftSize / Sum;	// This is the 1/(DC gain) compared to rect. window
    }
    else WinNorm = 1;
          
    for( i = 0; i < (int)fftSize / 2; i++ )
      Temp1[ i ] = Complex( In[ 2 * i ], In[ ( 2 * i ) + 1 ] );

    Complex* Result = F.Compute( Temp1, Temp2 );

    double Norm = (int)Inverse ? WinNorm : WinNorm / (double)(int)fftSize;
    for( i = 0; i < (int)fftSize; i++ )
      Result[ i ] *= Norm;

    for( i = 0; i < (int)fftSize/2; i++ ) {
      Complex X1 = .5 * ( Result[ i ] + conj( Result[ i == 0 ? 0 : (int)fftSize/2 - i ] ) );
      Complex X2 = Complex( 0, -.5 ) * ( Result[ i ] - conj( Result[ i == 0 ? 0 : (int)fftSize/2 - i ] ) );
      output%((int)fftSize/2 - i - 1) << Norm * ( X1 + F.getW( -i, (int)fftSize ) * X2 );
    }
  }
}
