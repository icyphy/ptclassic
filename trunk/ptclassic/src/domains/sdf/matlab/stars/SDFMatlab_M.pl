defstar {
	name { Matlab_M }
	domain { SDF }
	derivedFrom { Matlab }
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
to Matlab format and passed to the Matlab function \fIMatlabFunction\fR
in the order that the input ports are connected to the star.
If there are no inputs, then the \fIMatlabFunction\fR is evaluated without
any arguments; e.g., a \fIMatlabFunction\fR of "hilb(4)" would return
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
                name { MatlabFunction }
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
	hinclude { "Matrix.h", "InfString.h" }

	header { typedef Matrix *MatrixPtr; }

	protected {
		// Matlab (C) structures
		MatrixPtr *matlabInputMatrices;
		MatrixPtr *matlabOutputMatrices;

		// Ptolemy (C++) structures for Matlab calls
		InfString *matlabInputNames;	    // array of variable names
		InfString *matlabOutputNames;	    // array of variable names
		char *matlabCommand;

		// Variables for number of inputs and outputs to this star
		int numInputs;
		int numOutputs;
	}

	constructor {
		matlabInputMatrices = 0;
		matlabOutputMatrices = 0;

		matlabInputNames = 0;
		matlabOutputNames = 0;
		matlabCommand = 0;
	}

	setup {
		SDFMatlab::setup();

		// establish of number inputs & allocate Matlab matrices &
		// generate names for Matlab versions of input matrix names
		numInputs = input.numberPorts();
		if ( numInputs > 0 ) {
		  LOG_DEL; delete [] matlabInputMatrices;
		  LOG_NEW; matlabInputMatrices = new MatrixPtr[numInputs];
		  LOG_DEL; delete [] matlabInputNames;
		  LOG_NEW; matlabInputNames = new InfString[numInputs];
		  nameMatlabMatrices( matlabInputNames,
		  		      numInputs,
				      (char *) MatlabInputVarName );
		}

		// establish of number outputs & allocate Matlab matrices &
		// generate names for Matlab versions of output matrix names
		numOutputs = output.numberPorts();
		if ( numOutputs > 0 ) {
		  LOG_DEL; delete [] matlabOutputMatrices;
		  LOG_NEW; matlabOutputMatrices = new MatrixPtr[numOutputs];
		  LOG_DEL; delete [] matlabOutputNames;
		  LOG_NEW; matlabOutputNames = new InfString[numOutputs];
		  nameMatlabMatrices( matlabOutputNames,
		  		      numOutputs,
				      (char *) MatlabOutputVarName );
		}

		// create the command to be sent to the Matlab interpreter
		InfString commandString;
		buildMatlabCommand(commandString, matlabInputNames, numInputs,
				   (char *) MatlabFunction, matlabOutputNames,
				   numOutputs);
		LOG_NEW; matlabCommand = new char[commandString.length() + 1];
		strcpy(matlabCommand, (char *) commandString);
	}

	go {
		// allocate memory for Matlab matrices
		MPHIter nexti(input);
		for ( int i = 0; i < numInputs; i++ ) {
		  // read a reference to the matrix on input port i
		  PortHole *iportp = nexti++;
		  Envelope Apkt;
		  ((*iportp)%0).getMessage(Apkt);
		  const ComplexMatrix& Amatrix =
			*(const ComplexMatrix *)Apkt.myData();

		  // allocate a Matlab matrix and name it
		  int rows = Amatrix.numRows();
		  int cols = Amatrix.numCols();
		  Matrix *matlabMatrix = mxCreateFull(rows, cols, MXCOMPLEX);
		  mxSetName( matlabMatrix, (char *) matlabInputNames[i]);

		  // copy values in the Ptolemy matrix to the Matlab matrix
		  double *realp = mxGetPr( matlabMatrix );
		  double *imagp = mxGetPi( matlabMatrix );
		  for ( int irow = 0; irow < rows; irow++ ) {
		    for ( int icol = 0; icol < cols; icol++ ) {
		      Complex temp = Amatrix[irow][icol];
		      *realp++ = real(temp);
		      *imagp++ = imag(temp);
		    }
		  }

		  // let Matlab know about the new Matlab matrix we've defined
		  putMatlabMatrix(matlabMatrix);

		  // save the pointer to the new Matlab matrix for deallocation
		  matlabInputMatrices[i] = matlabMatrix;
		}

		// evaluate the Matlab command (non-zero means error)
		// second argument indicates whether or not to abort on error
		evaluateMatlabCommand(matlabCommand, TRUE);

		// copy each Matlab output matrix to a Ptolemy matrix
		MPHIter nextp(output);
		InfString errstr;
		char *verbstr;
		int fatalErrorFlag = FALSE;
		for ( int j = 0; j < numOutputs; j++ ) {
		  // create a new Matlab matrix for deallocation and save ref.
		  Matrix *matlabMatrix =
			getMatlabMatrix( (char *) matlabOutputNames[j] );

		  // allocate a Ptolemy matrix
		  int rows = mxGetM( matlabMatrix );
		  int cols = mxGetN( matlabMatrix );
		  ComplexMatrix& Amatrix = *(new ComplexMatrix(rows, cols));

		  if ( mxIsFull(matlabMatrix) ) {
		    // for real matrices, imagp will be null
		    double *realp = mxGetPr( matlabMatrix );
		    double *imagp = mxGetPi( matlabMatrix );
		    LOG_NEW;
		    for ( int jrow = 0; jrow < rows; jrow++ ) {
		      for ( int jcol = 0; jcol < cols; jcol++ ) {
		        double realValue = ( realp ) ? ( *realp++ ) : 0.0;
		        double imagValue = ( imagp ) ? ( *imagp++ ) : 0.0;
		        Amatrix[jrow][jcol] = Complex(realValue, imagValue);
		      }
		    }
		  }
		  else {
		    if ( ! fatalErrorFlag ) {
		      errstr = "For the Matlab command ";
		      errstr << matlabCommand << ", ";
		      verbstr = " is not a full matrix.";
		    }
		    else {
		      errstr << " and ";
		      verbstr = " are not full matrices.";
		    }
		    errstr << matlabOutputNames[j];
		    fatalErrorFlag = TRUE;
		  }

		  // save the pointer to the new Matlab matrix for deallocation
		  matlabOutputMatrices[j] = matlabMatrix;

		  // write the matrix to output port j
		  // do not delete Amatrix--- particle class will handle that
		  PortHole *oportp = nextp++;
		  (*oportp)%0 << Amatrix;
		}

		// free Matlab memory-- assume Matlab memory alloc is efficient
		for ( int k = 0; k < numInputs; k++ ) {
		  mxFreeMatrix(matlabInputMatrices[k]);
		}
		for ( int m = 0; m < numOutputs; m++ ) {
		  mxFreeMatrix(matlabOutputMatrices[m]);
		}

		// close Matlab connection and exit Ptolemy if fatal error
		if ( fatalErrorFlag ) {
		  killMatlab();
		  Error::abortRun(*this, (char *) errstr, verbstr);
		}
	}

	destructor {
		LOG_DEL; delete [] matlabInputMatrices;
		LOG_DEL; delete [] matlabOutputMatrices;

		LOG_DEL; delete [] matlabInputNames;
		LOG_DEL; delete [] matlabOutputNames;
		LOG_DEL; delete [] matlabCommand;
	}
}
