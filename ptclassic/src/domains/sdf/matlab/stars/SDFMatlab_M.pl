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
	location { SDF Matlab library }
	htmldoc {
<a name="Matlab interface"></a>
This star converts the matrices/scalars on the input ports to Matlab format,
passes the Matlab matrices to the <i>MatlabFunction</i>, converts the
resulting Matlab matrices to Ptolemy matrices, and outputs the matrices.
The <i>MatlabFunction</i> state can either be a Matlab function name,
a call to a Matlab function, or one Matlab command.
The star will add the missing pieces to form a complete command.
<p>
For example, if <i>MatlabFunction</i> were "eig" and the star had one input
and two outputs, then the star would build the Matlab command
"[output#1, output#2] = eig(input#1);".
If <i>MatlabFunction</i> were "hilb(4)" and the star had no inputs and
one output, then the star would build the Matlab command
"[output#1] = hilb(4);".
If <i>MatlabFunction</i> were
"[output#1,output#2] = func1(func2(input#2),input#1)", then
the star would simply add a semicolon at the end.
Before the command is passed to Matlab, the pound '#' characters are replaced
with underscore '_' characters.
The pound characters are used to maintain compatibility with Ptolemy syntax.
<p>
When Ptolemy invokes a new instance of Matlab, it will launch
a remote Matlab process on the machine give by the value of the
<code>MATLAB_SERVER_HOSTNAME</code> environment variable if it
is set; otherwise, Ptolemy will launch a Matlab process on the
local workstation.
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
		name { MatlabSetUp }
		type { string }
		default { "" }
		desc {
The Matlab command to execute during the begin method of the Matlab star.
During the begin procedure, there is no data passing into or out of the star.
		}
	}
	defstate {
		name { MatlabFunction }
		type { string }
		default { "" }
		desc {
The Matlab command to execute each time the star fires.
The values of the input and output ports may be accessed using the
Ptolemy notation input#i and output#i, where i is the port number,
as explained above.
		}
	}
	defstate {
		name { MatlabWrapUp }
		type { string }
		default { "" }
		desc {
The Matlab command to execute during the wrapup procedure of the Matlab star.
During the wrapup procedure, there is no data passing into or out of the star.
		}
	}

	// Ptolemy kernel includes
	hinclude { "dataType.h", "Matrix.h", "StringList.h" }

	// Matlab interface includes
	hinclude { "MatlabPtIfc.h", "MatlabIfcFuns.h" }

	header { typedef Matrix* MatrixPtr; }

	protected {
		// Matlab (C) structures
		MatrixPtr* matlabInputMatrices;
		MatrixPtr* matlabOutputMatrices;

		// Ptolemy (C++) structures for Matlab calls
		char** matlabInputNames;    // array of Matlab variable names
		char** matlabOutputNames;   // array of Matlab variable names

		// Variables for number of inputs and outputs to this star
		int numInputs;
		int numOutputs;
	}

	constructor {
		matlabInputMatrices = 0;
		matlabOutputMatrices = 0;

		matlabInputNames = 0;
		matlabOutputNames = 0;

		numInputs = 0;
		numOutputs = 0;
	}

	setup {
		// run the setup method of the base star
		SDFMatlab::setup();

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
	}

	begin {
		// run the begin method of the base star (start Matlab, etc.)
		SDFMatlab::begin();

		// free any existing memory
		matlabInterface->FreeMatlabMatrices(matlabInputMatrices,
						    numInputs);
		delete [] matlabInputMatrices;
		matlabInputMatrices = 0;
		matlabInterface->FreeMatlabMatrices(matlabOutputMatrices,
						    numOutputs);
		delete [] matlabOutputMatrices;
		matlabOutputMatrices = 0;

		matlabInterface->FreeStringArray(matlabInputNames, numInputs);
		delete [] matlabInputNames;
		matlabInputNames = 0;
		matlabInterface->FreeStringArray(matlabOutputNames, numOutputs);
		delete [] matlabOutputNames;
		matlabOutputNames = 0;

		// allocate Matlab input matrices and generate their names
		if ( numInputs > 0 ) {
		  matlabInputMatrices = new MatrixPtr[numInputs];
		  for ( int k = 0; k < numInputs; k++ ) {
		    matlabInputMatrices[k] = 0;
		  }
		  matlabInputNames = new char*[numInputs];
		  matlabInterface->NameMatlabMatrices(matlabInputNames,
		  				      numInputs, "input");
		}

		// allocate Matlab output matrices and generate their names
		if ( numOutputs > 0 ) {
		  matlabOutputMatrices = new MatrixPtr[numOutputs];
		  for ( int k = 0; k < numOutputs; k++ ) {
		    matlabOutputMatrices[k] = 0;
		  }
		  matlabOutputNames = new char*[numOutputs];
		  matlabInterface->NameMatlabMatrices(matlabOutputNames,
		  				      numOutputs, "output");
		}

		// create the command to be sent to the Matlab interpreter
		matlabInterface->BuildMatlabCommand(
			matlabInputNames, numInputs,
			(const char *) MatlabFunction,
			matlabOutputNames, numOutputs);

		// evaluate the MatlabSetUp if provided
		const char* setupCommand = (const char*) MatlabSetUp;
		if ( setupCommand && *setupCommand ) {
		  InfString matlabCommand = setupCommand;
		  int err = matlabInterface->EvaluateUserCommand(matlabCommand);
		  if ( err ) {
		    Error::abortRun( *this, matlabInterface->GetErrorString() );
		  }
		}
	}

	go {
		// convert Ptolemy input matrices to Matlab matrices
		processInputMatrices();

		// evaluate the Matlab command (non-zero means error)
		StringList errmsg;
		int merror = matlabInterface->EvaluateUserCommand();
		if ( merror ) {
		  errmsg = matlabInterface->GetErrorString();
		  Error::abortRun( *this, matlabInterface->GetErrorString() );
		}
		else {
		  // convert Matlab matrices to Ptolemy matrices
		  merror = processOutputMatrices();
		  errmsg = "Could not convert the Matlab output matrices to Ptolemy matrices";
		}

		// Free dynamic memory
		matlabInterface->FreeMatlabMatrices(matlabInputMatrices,
						    numInputs);
		matlabInterface->UnsetMatlabVariable(matlabInputNames,
						     numInputs);
		matlabInterface->FreeMatlabMatrices(matlabOutputMatrices,
						    numOutputs);
		matlabInterface->UnsetMatlabVariable(matlabOutputNames,
						     numOutputs);

		// Report any errors that occurred
		if ( merror ) {
		  Error::abortRun( *this, errmsg );
		}
	}

	wrapup {
		const char* wrapupCommand = (const char*) MatlabWrapUp;
		if ( wrapupCommand && *wrapupCommand ) {
		  InfString matlabCommand = wrapupCommand;
		  int err = matlabInterface->EvaluateUserCommand(matlabCommand);
		  if ( err ) {
		    Error::abortRun( *this, matlabInterface->GetErrorString() );
		  }
		}
	}

	destructor {
		matlabInterface->FreeMatlabMatrices(matlabInputMatrices,
						   numInputs);
		delete [] matlabInputMatrices;
		matlabInterface->FreeMatlabMatrices(matlabOutputMatrices,
						   numOutputs);
		delete [] matlabOutputMatrices;

		matlabInterface->FreeStringArray(matlabInputNames, numInputs);
		delete [] matlabInputNames;
		matlabInterface->FreeStringArray(matlabOutputNames, numOutputs);
		delete [] matlabOutputNames;
	}

	// Returns 1 for Failure and 0 for Success
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
		  int errflag = FALSE;
		  PortHole *iportp = nexti++;
		  DataType portType = iportp->resolvedType();
		  Matrix *matlabMatrix =
		  	matlabInterface->PtolemyToMatlab(
				(*iportp)%0, portType, &errflag);

		  // check for error in the call to PtolemyToMatlab
		  if ( errflag ) {
		    errorFlag = TRUE;
		    StringList errstr;
		    errstr = "Unsupported data type ";
		    errstr << portType << " on input port " << i+1 << ".";
		    Error::warn(*this, errstr);
		  }

		  // Give the current matrix a name
		  matlabInterface->NameMatlabMatrix(matlabMatrix,
		  				   matlabInputNames[i]);

		  // let Matlab know about the new Matlab matrix we've defined
		  // FIXME: Memory Leak
		  matlabInterface->MatlabEnginePutMatrix(matlabMatrix);

		  // save the pointer to the new Matlab matrix for deallocation
		  matlabInputMatrices[i] = matlabMatrix;
		}

		return( errorFlag );
	  }
	}

	// Returns 1 for Failure and 0 for Success
	method {
	  name { processOutputMatrices }
	  access { protected }
	  type { int }
	  arglist { "()" }
	  code {
		// possible error messages and flags
		int incompatibleOutportPort = FALSE;
		int matlabFatalError = FALSE;
		int nullMatlabMatrix = FALSE;
		StringList merrstr, mverbstr, nerrstr, nverbstr;

		// iterate through the output ports
		MPHIter nextp(output);
		for ( int j = 0; j < numOutputs; j++ ) {
		  // current output porthole & advance nextp to next porthole
		  PortHole *oportp = nextp++;
		  DataType portType = oportp->resolvedType();

		  // create a new Matlab matrix for deallocation and save ref.
		  // FIXME: Memory leak
		  Matrix *matlabMatrix =
		  matlabInterface->MatlabEngineGetMatrix(matlabOutputNames[j]);

		  // save the pointer to the new Matlab matrix for deallocation
		  matlabOutputMatrices[j] = matlabMatrix;

		  // check to make that the Matlab matrix is defined
		  if ( matlabMatrix == 0 ) {
		    if ( ! nullMatlabMatrix ) {
		      nullMatlabMatrix = TRUE;
		      nerrstr = "For the Matlab command ";
		      nerrstr << matlabInterface->GetMatlabCommand() << ", ";
		      nverbstr = " is not defined.";
		    }
		    else {
		      nerrstr << " and ";
		      nverbstr = " are not defined.";
		    }
		    nerrstr << matlabOutputNames[j];
		    continue;
		  }

		  StringList matrixId = matlabOutputNames[j];
		  matrixId << " on output port " << j;
		  int errflag = FALSE;
		  int warnflag = FALSE;
		  int badport = matlabInterface->MatlabToPtolemy(
		  			(*oportp)%0, portType, matlabMatrix,
					&warnflag, &errflag);
		  incompatibleOutportPort = incompatibleOutportPort || badport;

		  if ( warnflag ) {
		    Error::warn(*this, matlabInterface->GetWarningString() );
		  }

		  if ( errflag ) {
		    if ( ! matlabFatalError ) {
		      matlabFatalError = TRUE;
		      merrstr = "For the Matlab command ";
		      merrstr << matlabInterface->GetMatlabCommand() << ", ";
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
		  Error::warn(*this, nerrstr, nverbstr);
		}
		if ( matlabFatalError ) {
		  Error::warn(*this, merrstr, mverbstr );
		}

		return( incompatibleOutportPort ||
			nullMatlabMatrix ||
			matlabFatalError );
	  }
	}
}
