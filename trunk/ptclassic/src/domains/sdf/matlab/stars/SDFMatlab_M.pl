defstar {
	name { Matlab_M }
	domain { SDF }
	derivedFrom { Matlab }
	desc {
Evaluate a Matlab function if inputs are given or
evaluate a Matlab command if no inputs are given.
	}
	version { $Id$ }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
.Ir "Matlab interface"
This star converts the matrices/scalars on the input ports to Matlab format,
passes the Matlab matrices to the \fIMatlabFunction\fR, converts the
resulting Matlab matrices to Ptolemy matrices, and outputs the matrices.
The \fIMatlabFunction\fR state can either be a Matlab function name,
a call to a Matlab function, or one Matlab command.
The star will add the missing pieces to form a complete command.

For example, if \fIMatlabFunction\fR were "eig" and the star had one input
and two outputs, then the star would build the Matlab command
"[output#1, output#2] = eig(input#1);".
If \fIMatlabFunction\fR were "hilb(4)" and the star had no inputs and
one output, then the star would build the Matlab command
"[output#1] = hilb(4);".
If \fIMatlabFunction\fR were
"[output#1,output#2] = func1(func2(input#2),input#1)", then
the star would simply add a semicolon at the end.
Before the command is passed to Matlab, the pound '#' characters are replaced
with underscore '_' characters.
The pound characters are used to maintain compatibility with Ptolemy syntax.
	}
	inmulti {
		name { input }
		type { anytype }
	}
	outmulti {
		name { output }
		type { FLOAT_MATRIX_ENV }
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
		// set the number of input and output ports
		numInputs = input.numberPorts();
		numOutputs = output.numberPorts();

		// check data type of output ports
		if ( numOutputs > 0 ) {
		  DataType outType = output.type();
		  if ( outType != FLOAT_MATRIX_ENV &&
		       outType != COMPLEX_MATRIX_ENV ) {
		    Error::abortRun(*this,
				    "The output ports may only support float",
				    "matrix or complex matrix envelopes, not",
				    output.type());
		  }
		}

		// run the setup method of the base star (start Matlab, etc.)
		SDFMatlab::setup();

		// allocate Matlab input matrices and generate their names
		if ( numInputs > 0 ) {
		  freeMatlabMatrices(matlabInputMatrices, numInputs);
		  LOG_DEL; delete [] matlabInputMatrices;
		  LOG_NEW; matlabInputMatrices = new MatrixPtr[numInputs];
		  for ( int k = 0; k < numInputs; k++ ) {
		    matlabInputMatrices[k] = 0;
		  }
		  LOG_DEL; delete [] matlabInputNames;
		  LOG_NEW; matlabInputNames = new InfString[numInputs];
		  nameMatlabMatrices(matlabInputNames, numInputs, "input");
		}

		// allocate Matlab output matrices and generate their names
		if ( numOutputs > 0 ) {
		  freeMatlabMatrices(matlabOutputMatrices, numOutputs);
		  LOG_DEL; delete [] matlabOutputMatrices;
		  LOG_NEW; matlabOutputMatrices = new MatrixPtr[numOutputs];
		  for ( int k = 0; k < numOutputs; k++ ) {
		    matlabOutputMatrices[k] = 0;
		  }
		  LOG_DEL; delete [] matlabOutputNames;
		  LOG_NEW; matlabOutputNames = new InfString[numOutputs];
		  nameMatlabMatrices(matlabOutputNames, numOutputs, "output");
		}

		// create the command to be sent to the Matlab interpreter
		InfString commandString;
		buildMatlabCommand(commandString,
				   matlabInputNames, numInputs,
				   (const char *) MatlabFunction,
				   matlabOutputNames, numOutputs);
		LOG_DEL; delete [] matlabCommand;
		LOG_NEW; matlabCommand = new char[commandString.length() + 1];
		strcpy(matlabCommand, (char *) commandString);
	}

	go {
		// call the go method of the parent star
		SDFMatlab::go();

		// convert Ptolemy input matrices to Matlab matrices
		processInputMatrices();

		// evaluate the Matlab command (non-zero means error)
		int merror = evaluateMatlabCommand(matlabCommand);
		if ( merror ) {
		  freeMatlabMatrices(matlabInputMatrices, numInputs);
		  freeMatlabMatrices(matlabOutputMatrices, numOutputs);
		  Error::abortRun( *this,
				   "Matlab could not evaluate the command.");
		}
		else {
		  // convert Matlab matrices to Ptolemy matrices
		  int oerror = processOutputMatrices();
		  freeMatlabMatrices(matlabInputMatrices, numInputs);
		  freeMatlabMatrices(matlabOutputMatrices, numOutputs);
		  if ( oerror ) {			// non-zero means error
		    Error::abortRun( *this, "Could not convert the Matlab ",
				     "output matrices to Ptolemy matrices" );
		  }
		}
	}

	destructor {
		LOG_DEL; delete [] matlabInputMatrices;
		LOG_DEL; delete [] matlabOutputMatrices;

		LOG_DEL; delete [] matlabInputNames;
		LOG_DEL; delete [] matlabOutputNames;
		LOG_DEL; delete [] matlabCommand;
	}

	method {
	  name { freeMatlabMatrices }
	  access { protected }
	  type { void }
	  arglist { "(MatrixPtr *matlabMatrices, int numMatrices)" }
	  code {
		if ( matlabMatrices != 0 ) {
		  for ( int k = 0; k < numMatrices; k++ ) {
		    if ( matlabMatrices[k] != 0 ) {
		      mxFreeMatrix(matlabMatrices[k]);
		      matlabMatrices[k] = 0;
		    }
		  }
		}
	  }
	}

	method {
	  name { processInputMatrices }
	  access { protected }
	  type { int }
	  arglist { "()" }
	  code {
		int errorFlag = FALSE;

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
		    *realp = (double)((*iportp)%0);
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
		    // Matlab stores values in column-major order like Fortran
		    double *realp = mxGetPr(matlabMatrix);
		    double *imagp = mxGetPi(matlabMatrix);
		    for ( int icol = 0; icol < cols; icol++ ) {
		      for ( int irow = 0; irow < rows; irow++ ) {
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
		    // Matlab stores values in column-major order like Fortran
		    double *realp = mxGetPr(matlabMatrix);
		    for ( int icol = 0; icol < cols; icol++ ) {
		      for ( int irow = 0; irow < rows; irow++ ) {
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
		    // Matlab stores values in column-major order like Fortran
		    double *realp = mxGetPr(matlabMatrix);
		    for ( int icol = 0; icol < cols; icol++ ) {
		      for ( int irow = 0; irow < rows; irow++ ) {
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
		    // Matlab stores values in column-major order like Fortran
		    double *realp = mxGetPr(matlabMatrix);
		    for ( int icol = 0; icol < cols; icol++ ) {
		      for ( int irow = 0; irow < rows; irow++ ) {
			*realp++ = double(Amatrix[irow][icol]);
		      }
		    }
		  }
		  else {
		    errorFlag = TRUE;
		    InfString errstr;
		    errstr = "Unsupported data type ";
		    errstr << portType << " on input port " << i+1 << ".";
		    Error::warn(*this, (const char *) errstr);
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

		return( errorFlag );
	  }
	}

	method {
	  name { processOutputMatrices }
	  access { protected }
	  type { int }
	  arglist { "()" }
	  code {
		// copy each Matlab output matrix to a Ptolemy matrix

		// possible error messages and flags
		int incompatibleOutportPort = FALSE;
		int matlabFatalError = FALSE;
		int nullMatlabMatrix = FALSE;
		InfString merrstr, mverbstr, nerrstr, nverbstr;

		// iterate through the output ports
		MPHIter nextp(output);
		for ( int j = 0; j < numOutputs; j++ ) {
		  // current output porthole & advance nextp to next porthole
		  PortHole *oportp = nextp++;
		  DataType portType = oportp->resolvedType();

		  // create a new Matlab matrix for deallocation and save ref.
		  Matrix *matlabMatrix =
			getMatlabMatrix( (char *) matlabOutputNames[j] );

		  // save the pointer to the new Matlab matrix for deallocation
		  matlabOutputMatrices[j] = matlabMatrix;

		  // check to make that the Matlab matrix is defined
		  if ( matlabMatrix == 0 ) {
		    if ( ! nullMatlabMatrix ) {
		      nullMatlabMatrix = TRUE;
		      nerrstr = "For the Matlab command ";
		      nerrstr << matlabCommand << ", ";
		      nverbstr = " is not defined.";
		    }
		    else {
		      nerrstr << " and ";
		      nverbstr = " are not defined.";
		    }
		    nerrstr << matlabOutputNames[j];
		    continue;
		  }

		  // allocate a Ptolemy matrix
		  int rows = mxGetM(matlabMatrix);
		  int cols = mxGetN(matlabMatrix);

		  if ( mxIsFull(matlabMatrix) ) {

		    // for real matrices, imagp will be null
		    double *realp = mxGetPr(matlabMatrix);
		    double *imagp = mxGetPi(matlabMatrix);

		    // copy Matlab matrices (in column-major order) to Ptolemy
		    if ( portType == COMPLEX_MATRIX_ENV ) {
		      LOG_NEW; ComplexMatrix& Amatrix =
					*(new ComplexMatrix(rows, cols));
		      for ( int jcol = 0; jcol < cols; jcol++ ) {
			for ( int jrow = 0; jrow < rows; jrow++ ) {
			  double realValue = *realp++;
			  double imagValue = ( imagp ) ? *imagp++ : 0.0;
			  Amatrix[jrow][jcol] = Complex(realValue, imagValue);
			}
		      }
		      // write the matrix to output port j
		      // do not delete Amatrix: particle class handles that
		      (*oportp)%0 << Amatrix;
		    }
		    else if ( portType == FLOAT_MATRIX_ENV ) {
		      LOG_NEW; FloatMatrix& Amatrix =
					*(new FloatMatrix(rows, cols));
		      if ( mxIsComplex(matlabMatrix) ) {
			InfString myerrstr;
			myerrstr = "\nImaginary components ignored for the ";
			myerrstr << "Matlab matrix " << matlabOutputNames[j];
			myerrstr << " on output port " << j;
			Error::warn(*this, (const char *) myerrstr);
		      }
		      for ( int jcol = 0; jcol < cols; jcol++ ) {
			for ( int jrow = 0; jrow < rows; jrow++ ) {
			  Amatrix[jrow][jcol] = *realp++;
			}
		      }
		      // write the matrix to output port j
		      // do not delete Amatrix: particle class handles that
		      (*oportp)%0 << Amatrix;
		    }
		    // this situation should never be encountered since the
		    // setup method should have checked for output data type
		    else {
		      incompatibleOutportPort = TRUE;
		    }
		  }
		  else {
		    if ( ! matlabFatalError ) {
		      matlabFatalError = TRUE;
		      merrstr = "For the Matlab command ";
		      merrstr << matlabCommand << ", ";
		      mverbstr = " is not a full matrix.";
		    }
		    else {
		      merrstr << " and ";
		      mverbstr = " are not full matrices.";
		    }
		    merrstr << matlabOutputNames[j];
		  }
		}

		// print warning messages and return error status
		if ( incompatibleOutportPort ) {
		  Error::warn(*this,
			"Output port data type is neither a floating-point",
			"matrix nor a complex-valued matrix: the setup() ",
			"method should have flagged this error.");
		}
		if ( nullMatlabMatrix ) {
		  Error::warn( *this,
			       (const char *) nerrstr,
			       (const char *) nverbstr );
		}
		if ( matlabFatalError ) {
		  Error::warn( *this,
			       (const char *) merrstr,
			       (const char *) mverbstr );
		}

		return( incompatibleOutportPort ||
			nullMatlabMatrix ||
			matlabFatalError );
	  }
	}

}
