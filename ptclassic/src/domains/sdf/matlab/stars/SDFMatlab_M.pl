defstar {
	name { Matlab_M }
	domain { SDF }
	desc {
Evaluate Matlab functions if inputs are given or evaluate
Matlab commands and scripts if no inputs are given.
	}
	version { $Id$ }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
This star passes one or more complex matrix inputs to a Matlab function
and produces a complex matrix output that is the result.
The \fIoptions\fR string is passed directly to the Matlab kernel.
The Matlab kernel is started during the setup phase and exitted during
the wrapup phase.
At each firing of the star, the matrices on the input ports are converted
to Matlab format and passed to the Matlab function \fImatlabFunction\fR
in the order that the input ports are connected to the star.
If there are no inputs, then the \fImatlabFunction\fR is evaluated without
any arguments; e.g., a \fImatlabFunction\fR of "hilb(4)" would return
a 4 x 4 Hilbert matrix.
The names of the input and output variables in Matlab are derived from
\fImatlabInputName\fR and \fImatlabOutputName\fR, respectively.
If the \fImatlabInputName\fR is the same as the \fImatlabOutputName\fR,
then some or all of the matrices are reused by Matlab.
.Ir "Matlab interface"
	}
	inmulti {
		name { input }
		type { COMPLEX_MATRIX_ENV }
	}
	outmulti {
		name { output }
		type { COMPLEX_MATRIX_ENV }
	}
	defstate {
		name { options }
		type { string }
		default { "" }
		desc { Command line options for Matlab. }
	}
	defstate {
		name { matlabFunction }
		type { string }
		default { "" }
		desc {
The Matlab command to execute.
The values of the input ports will be passed as arguments to this function.
}
	}
	defstate {
		name { MatlabInputVarName }
		type { string }
		default { "Pmm" }
		desc {
Base name for Matlab variables to hold the value of the input matrices.
The variables will be of the form input name + port number, e.g. "Pmm1".
}
	}
	defstate {
		name { MatlabOutputVarName }
		type { string }
		default { "Pmm" }
		desc {
Base name for Matlab variables to hold the value of the output matrices.
The variables will be of the form output name + port number, e.g. "Pmm1".
}
	}

	// Matrix.h is from the Ptolemy kernel
	hinclude { "Matrix.h" }

	// matrix.h and engine.h are provided with Matlab
	header{
// Matlab interface library and Matlab data types
extern "C" {
#include "matrix.h"
#include "engine.h"
}
	}

	protected {
		// Matlab (C) structures
		Engine *matlabEnginePtr;
		Matrix **matlabInputMatrices;
		Matrix **matlabOutputMatrices;

		// Ptolemy (C++) structures for Matlab calls
		StringList *matlabInputNames;	    // array of variable names
		StringList *matlabOutputNames;	    // array of variable names
		char *matlabCommand;

		// Variables for number of inputs and outputs to this star
		int numInputs;
		int numOutputs;
	}

	constructor {
		matlabEnginePtr = 0;
		matlabInputMatrices = 0;
		matlabOutputMatrices = 0;

		matlabInputNames = 0;
		matlabOutputNames = 0;
		matlabCommand = 0;
	}

	setup {
		char numstr[16];
		int i;

		// make sure that we can start up Matlab
		StringList shellCommand = "matlab "; 
		shellCommand << ((const char *) options);
		if ( matlabEnginePtr != 0 ) {
		  engClose( matlabEnginePtr );
		}
		matlabEnginePtr = engOpen( ((char *) shellCommand) );
		if ( matlabEnginePtr == 0 ) {
		  Error::abortRun( *this, "Could not start Matlab using ",
				   (const char *) shellCommand );
		}

		// establish of number inputs & allocate Matlab matrices &
		// generate names for Matlab versions of input matrix names
		numInputs = input.numberPorts();
		if ( numInputs > 0 ) {
		  if ( matlabInputMatrices != 0 ) {
		    free(matlabInputMatrices);
		  }
		  matlabInputMatrices =
		    (Matrix **) malloc( numInputs * sizeof(Matrix *) );
		  char *inputBaseName = ((char *) MatlabInputVarName);
		  if ( matlabInputNames != 0 ) {
		    delete [] matlabInputNames;
		  }

		  matlabInputNames = new StringList[numInputs];
		  for ( i = 0; i < numInputs; i++ ) {
		    sprintf(numstr, "%d", i+1);
		    matlabInputNames[i] << inputBaseName << numstr;
		  }
		}

		// establish of number outputs & allocate Matlab matrices &
		// generate names for Matlab versions of output matrix names
		numOutputs = output.numberPorts();
		if ( numOutputs > 0 ) {
		  if ( matlabOutputMatrices != 0 ) {
		    free(matlabOutputMatrices);
		  }
		  matlabOutputMatrices =
		    (Matrix **) malloc( numOutputs * sizeof(Matrix *) );
		  char *outputBaseName = ((char *) MatlabOutputVarName);
		  if ( matlabOutputNames != 0 ) {
		    delete [] matlabOutputNames;
		  }

		  matlabOutputNames = new StringList[numOutputs];
		  for ( i = 0; i < numOutputs; i++ ) {
		    sprintf(numstr, "%d", i+1);
		    matlabOutputNames[i] << outputBaseName << numstr;
		  }
		}

		// create the command to be sent to the Matlab interpreter
		if ( matlabCommand != 0 ) {
		  delete [] matlabCommand;
		}
		StringList commandString;
		if ( numOutputs > 0 ) {
		  commandString << "[" << matlabOutputNames[0];
		  for ( i = 1; i < numOutputs; i++ ) {
		    commandString << ", " << matlabOutputNames[i];
		  }
		  commandString << "] = ";
		}
		commandString << ((char *) matlabFunction);
		if ( numInputs > 0 ) {
		  commandString << "(" << matlabInputNames[0];
		  for ( i = 1; i < numInputs; i++ ) {
		    commandString << ", " << matlabInputNames[i];
		  }
		  commandString << ")";
		}
		matlabCommand = new char[commandString.length()];
		strcpy(matlabCommand, (char *) commandString);
	}

	go {
		// allocate memory for Matlab matrices
		MPHIter nexti(input);
		for ( int i = 0; i < numInputs; i++ ) {
		  Envelope Apkt;
		  ((*nexti++)%0).getMessage(Apkt);
		  const ComplexMatrix& Amatrix =
			*(const ComplexMatrix *)Apkt.myData();

		  // allocate Matlab matrices
		  int rows = Amatrix.numRows();
		  int cols = Amatrix.numCols();
		  matlabInputMatrices[i] = mxCreateFull(rows, cols, COMPLEX);
		  mxSetName( matlabInputMatrices[i],
			     (char *) matlabInputNames[i]);

		  // copy values in Ptolemy matrix to Matlab matrix
		  double *realVector = mxGetPr(matlabInputMatrices[i]);
		  double *imagVector = mxGetPi(matlabInputMatrices[i]);
		  double *rp = realVector;
		  double *ip = imagVector;
		  Complex temp;
		  for ( int irow = 0; irow < rows; irow++ ) {
		    for ( int icol = 0; icol < cols; icol++ ) {
		      temp = Amatrix[icol][icol];
		      *rp++ = real(temp);
		      *ip++ = imag(temp);
		    }
		  }
		  mxSetPr(matlabInputMatrices[i], realVector);
		  mxSetPi(matlabInputMatrices[i], imagVector);
		}

		// evaluate the Matlab command (non-zero means error)
		int mstatus = engEvalString( matlabEnginePtr, matlabCommand );
		if ( mstatus == 0 ) {
		  char numstr[64];
		  sprintf(numstr, "Matlab returned %d indicating ", mstatus);
		  Error::abortRun( *this, numstr,
				   "an error in the Matlab command ",
				   matlabCommand );
		}

		// copy each Matlab output matrix to a Ptolemy matrix
		MPHIter nextp(output);
		for ( int j = 0; j < numOutputs; j++ ) {
		  matlabOutputMatrices[j] =
			engGetMatrix( matlabEnginePtr,
				      (char *) matlabOutputNames[j] );

		  double *rp = mxGetPr(matlabOutputMatrices[j]);
		  double *ip = mxGetPi(matlabOutputMatrices[j]);
		  int rows = mxGetM(matlabOutputMatrices[j]);
		  int cols = mxGetN(matlabOutputMatrices[j]);
		  ComplexMatrix& Amatrix = *(new ComplexMatrix(rows, cols));
		  for ( int jrow = 0; jrow < rows; jrow++ ) {
		    for ( int jcol = 0; jcol < cols; jcol++ ) {
		      Amatrix[jrow][jcol] = Complex(*rp++, *ip++);
		    }
		  }
		  ((*nextp++)%0) << Amatrix;
		}

		// free Matlab memory-- assume Matlab is good with memory alloc
		for ( int k = 0; k < numInputs; k++ ) {
		  mxFreeMatrix(matlabInputMatrices[k]);
		}
		for ( int m = 0; m < numOutputs; m++ ) {
		  mxFreeMatrix(matlabOutputMatrices[m]);
		}
	}

	destructor {
		delete [] matlabInputNames;
		delete [] matlabOutputNames;
		delete [] matlabCommand;
	}

	wrapup {
		if ( ! engClose(matlabEnginePtr) ) {
		  Error::warn(*this, "Error when terminating connection ",
			      "to the Matlab kernel.");
		}
		if ( matlabInputMatrices != 0 ) free( matlabInputMatrices );
		if ( matlabOutputMatrices != 0 ) free( matlabOutputMatrices );
	}
}
