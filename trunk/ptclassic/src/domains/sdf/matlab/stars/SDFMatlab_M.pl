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
This star converts the matrices/scalars on the input ports to Matlab format,
passes the Matlab matrices to the \fIMatlabFunction\fR, converts the
resulting Matlab matrices to Ptolemy format, and outputs the matrices/scalars.
If there are no inputs, then the \fIMatlabFunction\fR is evaluated as is
without any arguments being passed to it; e.g., a value of
"hilb(4)" for \fIMatlabFunction\fR would return a 4 x 4 Hilbert matrix.
The names of the input arguments and output variables for the Matlab function
are derived from \fImatlabInputName\fR and \fImatlabOutputName\fR,
respectively.
The values of \fImatlabInputName\fR and \fImatlabOutputName\fR may be the same.
The user should specify these names so as to avoid name conflicts with
built-in Matlab commands and custom Matlab scripts.

As an example of how the Matlab command is built, suppose that this
star has three inputs and two outputs.
If \fImatlabInputName\fR is "Pout", \fImatlabOutputName\fR is "Pin",
and \fIfIMatlabFunction\fR is "doit", then this star would evaluate
the Matlab command "[Pout1, Pout2] = doit(Pin1, Pin2, Pin3);".
.Ir "Matlab interface"
	}
	inmulti {
		name { input }
		type { anytype }
	}
	outmulti {
		name { output }
		type { anytype }
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
	hinclude { "dataType.h", "Matrix.h", "InfString.h" }

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
		LOG_DEL; delete [] matlabCommand;
		LOG_NEW; matlabCommand = new char[commandString.length() + 1];
		strcpy(matlabCommand, (char *) commandString);
	}

	go {
		// convert Ptolemy input matrices to Matlab matrices
		processInputMatrices();

		// evaluate the Matlab command (non-zero means error)
		// second argument indicates whether or not to abort on error
		evaluateMatlabCommand(matlabCommand, TRUE);

		// convert the Matlab matrices to Ptolemy matrices
		processOutputMatrices();
	}

	destructor {
		LOG_DEL; delete [] matlabInputMatrices;
		LOG_DEL; delete [] matlabOutputMatrices;

		LOG_DEL; delete [] matlabInputNames;
		LOG_DEL; delete [] matlabOutputNames;
		LOG_DEL; delete [] matlabCommand;
	}

	method {
	  name { processInputMatrices }
	  access { protected }
	  type { void }
	  arglist { "()" }
	  code {
		// allocate memory for Matlab matrices
		MPHIter nexti(input);
		for ( int i = 0; i < numInputs; i++ ) {
		  // read a reference to the matrix on input port i
		  PortHole *iportp = nexti++;
		  DataType portType = iportp->resolvedType();
		  Matrix *matlabMatrix = 0;

		  // can't use a switch because enumerated data types
		  // are assigned to strings and not to integers
		  if ( portType == INT ||
		       portType == FLOAT ||
		       portType == FIX ) {
		    matlabMatrix = mxCreateFull(1, 1, MXREAL);
		    double *realp = mxGetPr(matlabMatrix);
		    *realp = double((*iportp)%0);
		  }
		  else if ( portType == COMPLEX ) {
		    matlabMatrix = mxCreateFull(1, 1, MXCOMPLEX);
		    double *realp = mxGetPr(matlabMatrix);
		    double *imagp = mxGetPi(matlabMatrix);
		    Complex temp = (*iportp)%0;
		    *realp = real(temp);
		    *imagp = imag(temp);
		  }
		  else if ( portType == COMPLEX_MATRIX_ENV ) {
		    Envelope Apkt;
		    ((*iportp)%0).getMessage(Apkt);
		    const ComplexMatrix& Amatrix =
			*(const ComplexMatrix *)Apkt.myData();

		    // allocate a Matlab matrix and name it
		    int rows = Amatrix.numRows();
		    int cols = Amatrix.numCols();
		    matlabMatrix = mxCreateFull(rows, cols, MXCOMPLEX);

		    // copy values in the Ptolemy matrix to the Matlab matrix
		    double *realp = mxGetPr(matlabMatrix);
		    double *imagp = mxGetPi(matlabMatrix);
		    for ( int irow = 0; irow < rows; irow++ ) {
		      for ( int icol = 0; icol < cols; icol++ ) {
			Complex temp = Amatrix[irow][icol];
			*realp++ = real(temp);
			*imagp++ = imag(temp);
		      }
		    }
		  }
		  else if ( portType == FIX_MATRIX_ENV ) {
		    Envelope Apkt;
		    ((*iportp)%0).getMessage(Apkt);
		    const FixMatrix& Amatrix =
			*(const FixMatrix *)Apkt.myData();

		    // allocate a Matlab matrix and name it
		    int rows = Amatrix.numRows();
		    int cols = Amatrix.numCols();
		    matlabMatrix = mxCreateFull(rows, cols, MXREAL);

		    // copy values in the Ptolemy matrix to the Matlab matrix
		    double *realp = mxGetPr(matlabMatrix);
		    for ( int irow = 0; irow < rows; irow++ ) {
		      for ( int icol = 0; icol < cols; icol++ ) {
			*realp++ = double(Amatrix[irow][icol]);
		      }
		    }
		  }
		  else if ( portType == FLOAT_MATRIX_ENV ) {
		    Envelope Apkt;
		    ((*iportp)%0).getMessage(Apkt);
		    const FloatMatrix& Amatrix =
			*(const FloatMatrix *)Apkt.myData();

		    // allocate a Matlab matrix and name it
		    int rows = Amatrix.numRows();
		    int cols = Amatrix.numCols();
		    matlabMatrix = mxCreateFull(rows, cols, MXREAL);

		    // copy values in the Ptolemy matrix to the Matlab matrix
		    double *realp = mxGetPr(matlabMatrix);
		    for ( int irow = 0; irow < rows; irow++ ) {
		      for ( int icol = 0; icol < cols; icol++ ) {
			*realp++ = Amatrix[irow][icol];
		      }
		    }
		  }
		  else if ( portType == INT_MATRIX_ENV ) {
		    Envelope Apkt;
		    ((*iportp)%0).getMessage(Apkt);
		    const IntMatrix& Amatrix =
			*(const IntMatrix *)Apkt.myData();

		    // allocate a Matlab matrix and name it
		    int rows = Amatrix.numRows();
		    int cols = Amatrix.numCols();
		    matlabMatrix = mxCreateFull(rows, cols, MXREAL);

		    // copy values in the Ptolemy matrix to the Matlab matrix
		    double *realp = mxGetPr(matlabMatrix);
		    for ( int irow = 0; irow < rows; irow++ ) {
		      for ( int icol = 0; icol < cols; icol++ ) {
			*realp++ = double(Amatrix[irow][icol]);
		      }
		    }
		  }
		  else {
		    char errstr[64];
		    sprintf(errstr, "Unsupported data type %d on port %d",
		     int(portType), i+1);
		    Error::warn(*this, errstr);
		    matlabMatrix = mxCreateFull(1, 1, MXREAL);
		    double *realp = mxGetPr(matlabMatrix);
		    *realp = 0.0;
		  }

		  // Give the current matrix a name
		  mxSetName(matlabMatrix, (char *) matlabInputNames[i]);

		  // let Matlab know about the new Matlab matrix we've defined
		  putMatlabMatrix(matlabMatrix);

		  // save the pointer to the new Matlab matrix for deallocation
		  matlabInputMatrices[i] = matlabMatrix;
		}
	  }
	}

	method {
	  name { processOutputMatrices }
	  access { protected }
	  type { void }
	  arglist { "()" }
	  code {
		// copy each Matlab output matrix to a Ptolemy matrix
		MPHIter nextp(output);
		InfString errstr;
		char *verbstr = "";
		int fatalErrorFlag = FALSE;
		for ( int j = 0; j < numOutputs; j++ ) {
		  PortHole *oportp = nextp++;  // current output porthole
		  int matlabMatrixWrongForm = TRUE;

		  // create a new Matlab matrix for deallocation and save ref.
		  Matrix *matlabMatrix =
			getMatlabMatrix( (char *) matlabOutputNames[j] );

		  // allocate a Ptolemy matrix
		  int rows = mxGetM(matlabMatrix);
		  int cols = mxGetN(matlabMatrix);

		  if ( mxIsFull(matlabMatrix) ) {

		    // for real matrices, imagp will be null
		    double *realp = mxGetPr( matlabMatrix );
		    double *imagp = mxGetPi( matlabMatrix );

		    if ( mxIsComplex(matlabMatrix) ) {
		      matlabMatrixWrongForm = FALSE;
		      if ( rows == 1 && cols == 1 ) {
			(*oportp)%0 << Complex(*realp, *imagp);
		      }
		      else {
			LOG_NEW; ComplexMatrix& Amatrix =
					*(new ComplexMatrix(rows, cols));
			for ( int jrow = 0; jrow < rows; jrow++ ) {
			  for ( int jcol = 0; jcol < cols; jcol++ ) {
			    Amatrix[jrow][jcol] = Complex(*realp++, *imagp++);
			  }
			}
			// write the matrix to output port j
			// do not delete Amatrix: particle class handles that
			(*oportp)%0 << Amatrix;
		      }
		    }
		    else if ( mxIsDouble(matlabMatrix) ) {
		      matlabMatrixWrongForm = FALSE;
		      if ( rows == 1 && cols == 1 ) {
			(*oportp)%0 << *realp;
		      }
		      else {
			LOG_NEW; FloatMatrix& Amatrix =
					*(new FloatMatrix(rows, cols));
			for ( int jrow = 0; jrow < rows; jrow++ ) {
			  for ( int jcol = 0; jcol < cols; jcol++ ) {
			    Amatrix[jrow][jcol] = *realp++;
			  }
			}
			// write the matrix to output port j
			// do not delete Amatrix: particle class handles that
			(*oportp)%0 << Amatrix;
		      }
		    }
		  }

		  if ( matlabMatrixWrongForm ) {
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
	}

}
