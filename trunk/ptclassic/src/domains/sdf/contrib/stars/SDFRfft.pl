defstar {
	name {Rfft}
	domain {SDF}
	desc {
Compute the FFT of a real input, using the '2-channel trick'.
\fIInputSize\fR is number of input samples and
\fIfftSize\fR is the size of FFT.
Zero padding is used if \fIfftSize\fR > \fIInputSize\fR.
The inverse FFT is taken if \fIInverse\fR is non-zero.
Apply a Hamming window to the data if \fIUseWindow\fR is non-zero.
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
    delete [] Temp1;
    delete [] Temp2;
    delete [] In;
  }
  setup {
    int fftlength = fftSize;
    delete [] Temp1;
    Temp1 = new Complex[ fftlength/2 ];
    delete [] Temp2;
    Temp2 = new Complex[ fftlength/2 ];
    delete [] In;
    In = new double[ fftlength ];
    input.setSDFParams( int(InputSize), int(InputSize)-1 );
    output.setSDFParams( fftlength / 2, (fftlength/2)-1 );
  }
  hinclude {"nb_fft.h"}

  inline method {
    name { WinVal }
    type { double }
    arglist {"(double t)"}
    code {
      return .54 + .46 * cos( Pi2 * t );
    }

  }

  go {
    int invflag = Inverse;
    int fftlength = fftSize;
    nb_fft F( invflag, fftlength / 2 );

    for (int i = 0; i < int(InputSize); i++ )
      In[ int(InputSize) - i - 1 ] = input%i;
    for (i = int(InputSize); i < fftlength; i++ )
      In[ i ] = 0;

    double WinNorm = 1;
    if( int(UseWindow) ) {
      double Sum = 0;
      for( i = 0; i < fftlength; i++ ) {
	double TimeArg = ( double(i) / ( double(fftlength) - 1.) ) - 0.5;
	double WV = WinVal( TimeArg );
	In[ i ] *= WV;
	Sum += WV;
      }
      // This is the 1/(DC gain) compared to rect. window
      WinNorm = fftlength / Sum;
    }

    for( i = 0; i < fftlength / 2; i++ )
      Temp1[ i ] = Complex( In[ 2 * i ], In[ ( 2 * i ) + 1 ] );

    Complex* Result = F.Compute( Temp1, Temp2 );

    double Norm = int(Inverse) ? WinNorm : WinNorm / (double(fftlength));
    for( i = 0; i < fftlength; i++ )
      Result[ i ] *= Norm;

    for( i = 0; i < fftlength/2; i++ ) {
      Complex X1 = .5 * ( Result[ i ] +
		   conj( Result[ i == 0 ? 0 : fftlength/2 - i ] ) );
      Complex X2 = Complex( 0, -.5 ) * ( Result[ i ] -
		   conj( Result[ i == 0 ? 0 : fftlength/2 - i ] ) );
      output%(fftlength/2 - i - 1) <<
		Norm * ( X1 + F.getW( -i, fftlength ) * X2 );
    }
  }
}
