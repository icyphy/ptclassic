defstar {
  name {Cfft}
  domain {SDF}
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
  desc {
Compute the complex fft.
InputSize is number of input samples.
fftSize is the size of fft.  zero padding is used if 
  fftSize > InputSize
Inverse if Inverse == 1
Use Hamming window if UseWindow
	     }
  author { N. Becker }
  location { SDF user contribution library }
	input {
		name {input}
		type {complex}
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
    Complex *Temp;
    Complex *In;
  }
  constructor {
    Temp = 0;
    In = 0;
  }
  destructor {
    delete Temp;
    delete In;
  }
  setup {
    delete Temp;
    Temp = new Complex[ (int)fftSize ];
    delete In;
    In = new Complex[ (int)fftSize ];
    input.setSDFParams( (int)InputSize, (int)InputSize-1 );
    output.setSDFParams( (int)fftSize, (int)fftSize-1 );
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
    fft F( (int)Inverse, (int)fftSize );

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
          
    Complex* Result = F.Compute( In, Temp );

    double Norm = (int)Inverse ? WinNorm : WinNorm / (double)(int)fftSize;
    for( i = 0; i < (int)fftSize; i++ )
      Result[ i ] *= Norm;

    for ( i = 0; i < (int)fftSize; i++ )
      output%i << Result[ (int)fftSize - i - 1 ];
  }
}
