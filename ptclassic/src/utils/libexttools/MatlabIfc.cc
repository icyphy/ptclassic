static const char file_id[] = "MatlabIfc.cc";

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2

 Programmer:  Brian L. Evans
 Date of creation: 11/6/95
 Revisions:

 Base class for the Ptolemy interface to Matlab.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include "StringList.h"
#include "InfString.h"
#include "miscFuncs.h"
#include "Error.h"
#include "MatlabIfc.h"
#include "MatlabIfcFuns.h"

#define MATLAB_BUFFER_LEN        4096

// counts how many instances of this class have been created
static int matlabStarsCount = 0;

// keeps track of the lone Ptolemy-controlled Matlab process running
static Engine* matlabEnginePtr = 0;

// default buffer to store return strings from Matlab
static char defaultMatlabBuffer[MATLAB_BUFFER_LEN];

// constructor
MatlabIfc :: MatlabIfc() {
    matlabStarsCount++;
    scriptDirectory = 0;
    deleteFigures = FALSE;
    matlabFigureHandle = "PtolemyMatlabIfc";
    matlabFigureHandle << matlabStarsCount;
    matlabOutputBuffer = defaultMatlabBuffer;
    matlabOutputBufferLen = MATLAB_BUFFER_LEN;
    matlabOutputBuffer[0] = 0;
}

// destructor
MatlabIfc :: ~MatlabIfc() {
    // close any figures associated with this star
    if ( deleteFigures ) {
	CloseMatlabFigures();
    }

    matlabStarsCount--;
    if ( matlabStarsCount == 0 ) {
	KillMatlab();
    }

    delete [] scriptDirectory;
}

// set data members
int MatlabIfc :: SetDeleteFigures(int flag) {
    deleteFigures = flag;
    return deleteFigures;
}

const char* MatlabIfc :: SetScriptDirectory(const char* dir) {
    delete [] scriptDirectory;
    scriptDirectory = expandPathName(dir);
    return scriptDirectory;
}

const char* MatlabIfc :: SetFigureHandle(const char* handle) {
    matlabFigureHandle = handle;
    return matlabFigureHandle;
}

const char* MatlabIfc :: SetMatlabCommand(const char* command) {
    commandString = command;
    return (const char*) commandString;
}

char* MatlabIfc :: SetOutputBuffer(char *buffer, int bufferlen) {
    matlabOutputBuffer = buffer;
    matlabOutputBufferLen = bufferlen;
    return matlabOutputBuffer;
}

// get data members
int MatlabIfc :: GetDeleteFigures() {
    return deleteFigures;
}

const char* MatlabIfc :: GetScriptDirectory() {
    return scriptDirectory;
}

const char* MatlabIfc :: GetFigureHandle() {
    return (const char*) matlabFigureHandle;
}

const char* MatlabIfc :: GetMatlabCommand() {
    return (const char*) commandString;
}

char* MatlabIfc :: GetOutputBuffer() {
    return matlabOutputBuffer;
}

int MatlabIfc :: GetOutputBufferLength() {
    return matlabOutputBufferLen;
}

Engine* MatlabIfc :: GetCurrentEngine() {
    return matlabEnginePtr;
}

int MatlabIfc :: GetMatlabIfcInstanceCount() {
    return matlabStarsCount;
}

// setup methods

// generate names for Matlab versions of input matrix names
void MatlabIfc :: NameMatlabMatrices(char* matNames[], int numMatrices,
				     const char* baseName) {
    for (int i = 0; i < numMatrices; i++ ) {
	StringList newname = baseName;
	char numstr[32];
	sprintf(numstr, "_%d", i+1);
	newname << numstr;
	matNames[i] = savestring(newname);
    }
}

// build up a Matlab command for a block with inputs and/or outputs
const char* MatlabIfc :: BuildMatlabCommand(
		char* matlabInputNames[], int numInputs,
		const char* matlabFunction,
		char* matlabOutputNames[], int numOutputs) {

    int length = strlen(matlabFunction);

    // replace pound signs in matlabFunction with underscores
    char* matlabFragment = new char[length + 1];
    strcpy(matlabFragment, matlabFunction);
    for ( int i = 0; i < length; i++ ) {
        if ( matlabFragment[i] == '#' ) {
	    matlabFragment[i] = '_';
	}
    }

    // see if the Matlab command is terminated by a semicolon
    int endingSemicolonPresent = FALSE;
    for ( int j = length - 1; j >= 0; j-- ) {
	if ( ! isspace(matlabFragment[j]) ) {
	    endingSemicolonPresent = ( matlabFragment[j] == ';' );
	    break;
	}
    }

    // create the command to be sent to the Matlab interpreter
    commandString = "";
    if ( strchr(matlabFragment, '=') != 0 ) {
	  commandString << matlabFragment;
    }
    else {
	if ( numOutputs > 0 ) {
	    commandString << "[" << matlabOutputNames[0];
	    for ( int i = 1; i < numOutputs; i++ ) {
	      commandString << ", " << matlabOutputNames[i];
	    }
	    commandString << "] = ";
	}
	commandString << matlabFragment;
	if ( ( strchr(matlabFragment, '(') == 0 ) && ( numInputs > 0 ) ) {
	    commandString << "(" << matlabInputNames[0];
	    for ( int i = 1; i < numInputs; i++ ) {
	      commandString << ", " << matlabInputNames[i];
	    }
	    commandString << ")";
	}
    }

    // add a semicolon at the end of the command if one is
    // not present in order to suppress textual output
    if ( ! endingSemicolonPresent ) {
	commandString << ";";
    }

    delete [] matlabFragment;
    return (const char*) commandString;
}

// manage the Matlab process (low-level methods)

// start a Matlab process
Engine* MatlabIfc :: MatlabEngineOpen(char* unixCommand) {
    return engOpen(unixCommand);
}

// send a command to the Matlab Engine for evaluation
int MatlabIfc :: MatlabEngineSend(char* command) {
    return engEvalString(matlabEnginePtr, command);
}

// tell Matlab where to put the output of computations
int MatlabIfc :: MatlabEngineOutputBuffer(char* buffer, int buferLength) {
    return engOutputBuffer(matlabEnginePtr, buffer, buferLength);
}

// get pointer to a copy of a Matlab matrix
Matrix* MatlabIfc :: MatlabEngineGetMatrix(char* name) {
    return engGetMatrix(matlabEnginePtr, name);
}

// put a matrix in the Matlab environment (Matlab will copy the matrix)
int MatlabIfc :: MatlabEnginePutMatrix(Matrix* matrixPtr) {
    return engPutMatrix(matlabEnginePtr, matrixPtr);
}

// kill the Matlab connection
int MatlabIfc :: MatlabEngineClose() {
    return engClose(matlabEnginePtr);
}

// higher-level interface to the Matlab process
int MatlabIfc :: EvaluateOneCommand(char* command) {
    if ( MatlabIsRunning() ) {
	// assert location of buffer to hold output of Matlab commands
	MatlabEngineOutputBuffer(matlabOutputBuffer, matlabOutputBufferLen - 1);

	// MatlabEngineSend returns 0 on success and non-zero on failure
	matlabOutputBuffer[0] = 0;
	int merror = MatlabEngineSend(command);
	matlabOutputBuffer[matlabOutputBufferLen - 1] = 0;

	// kludge: MatlabEngineSend always returns 0 (success) even if
	// there is an error.  Therefore, we must determine if there
	// is an error.  An error occurs if when the output of the
	// Matlab command contains MATLAB_ERR_STR
	if ( merror == 0 ) {
	    merror = ( strstr(matlabOutputBuffer, MATLAB_ERR_STR) != 0 );
	}
	return merror;
    }
    else {
	strcpy(matlabOutputBuffer, "Matlab is not running!");
	return 1;
    }
}

// highest-level interface to the Matlab process

int MatlabIfc :: StartMatlab() {
    KillMatlab();

    // start the Matlab engine which starts Matlab
    matlabEnginePtr = MatlabEngineOpen(MATLAB_UNIX_COMMAND);
    if ( ! MatlabIsRunning() ) {
	Error::abortRun( "Could not start Matlab using ",
			 MATLAB_UNIX_COMMAND );
        return FALSE;
    }

    // add the PTOLEMY_MATLAB_DIRECTORY to the Matlab path
    char* fulldirname = expandPathName(PTOLEMY_MATLAB_DIRECTORY);
    InfString setPathCommand = "path(path, '";
    setPathCommand << fulldirname;
    setPathCommand << "');";
    EvaluateOneCommand(setPathCommand);
    delete [] fulldirname;
    return TRUE;
}

// check to see if a Matlab process is running
int MatlabIfc :: MatlabIsRunning() {
    return ( matlabEnginePtr != 0 );
}

int MatlabIfc :: EvaluateUserCommand() {
    return EvaluateUserCommand(commandString);
}

int MatlabIfc :: EvaluateUserCommand(char* command) {

     // change directories to one containing the Matlab command
     ChangeMatlabDirectory();

     // evaluate the passed command and report error if one occurred
     int merror = EvaluateOneCommand(command);
     if ( merror ) {
	  StringList errstr = "\nThe Matlab command `";
	  errstr << command << "'\ngave the following error message:\n";
	  Error::warn(errstr, matlabOutputBuffer);
     }

     return merror;
}

int MatlabIfc :: ChangeMatlabDirectory() {

    // expand the pathname and check its existence
    static InfString lastdirname;
    int retval = 0;
    if ( scriptDirectory && *scriptDirectory ) {
	struct stat stbuf;
	if ( stat(scriptDirectory, &stbuf) == -1 ) {
	    if ( strcmp((char*) lastdirname, scriptDirectory) != 0 ) {
		Error::warn( "Cannot access the directory ",
			     scriptDirectory );
		lastdirname = scriptDirectory;
	    }
            retval = 0;
	}
	else {
	    char* changeDirCommand = new char[strlen(scriptDirectory) + 4];
	    strcpy(changeDirCommand, "cd ");
	    strcat(changeDirCommand, scriptDirectory);
	    EvaluateOneCommand(changeDirCommand);
	    delete [] changeDirCommand;
	    retval = 1;
	}
     }
     return retval;
}

// establish that all newly created Matlab figures will be
// associated with the arg handle implemented by the Matlab
// function defined by MATLAB_SET_FIGURES in PTOLEMY_MATLAB_DIRECTORY
int MatlabIfc :: AttachMatlabFigureIds() {
     InfString command = MATLAB_SET_FIGURES;
     command << "('" << matlabFigureHandle << "');";
     return EvaluateOneCommand(command);
}

// close all figures associated with arg handle implemented by Matlab
// script defined by MATLAB_CLOSE_FIGURES in PTOLEMY_MATLAB_DIRECTORY
int MatlabIfc :: CloseMatlabFigures() {
    InfString command = MATLAB_CLOSE_FIGURES;
    command << "('" << matlabFigureHandle << "');";
    return EvaluateOneCommand(command);
}

int MatlabIfc :: KillMatlab() {
    int retval = TRUE;
    if ( MatlabIsRunning() ) {
	if ( MatlabEngineClose() ) {
	    Error::warn("Error when terminating connection ",
			"to the Matlab kernel.");
	    retval = FALSE;
	}
    }
    matlabEnginePtr = 0;
    return retval;
}

Matrix* MatlabIfc :: PtolemyToMatlab(Particle& particle, DataType portType,
				     int *errflag) {
    // can't use a switch because enumerated data types
    // are assigned to strings and not to integers
    double *realp = 0;
    double *imagp = 0;
    Matrix *matlabMatrix = 0;
    if ( portType == INT || portType == FLOAT || portType == FIX ) {
	matlabMatrix = mxCreateFull(1, 1, MXREAL);
	realp = mxGetPr(matlabMatrix);
	*realp = double(particle);
    }
    else if ( portType == COMPLEX ) {
	matlabMatrix = mxCreateFull(1, 1, MXCOMPLEX);
	realp = mxGetPr(matlabMatrix);
	imagp = mxGetPi(matlabMatrix);
	Complex temp = particle;
	*realp = real(temp);
	*imagp = imag(temp);
    }
    else if ( portType == COMPLEX_MATRIX_ENV ) {
	Envelope Apkt;
	particle.getMessage(Apkt);
	const ComplexMatrix& Amatrix = *(const ComplexMatrix *)Apkt.myData();

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
	particle.getMessage(Apkt);
	const FixMatrix& Amatrix = *(const FixMatrix *)Apkt.myData();

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
	particle.getMessage(Apkt);
	const FloatMatrix& Amatrix = *(const FloatMatrix *)Apkt.myData();

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
	particle.getMessage(Apkt);
	const IntMatrix& Amatrix = *(const IntMatrix *)Apkt.myData();

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
	*errflag = TRUE;
	matlabMatrix = mxCreateFull(1, 1, MXREAL);
	double *realp = mxGetPr(matlabMatrix);
	*realp = 0.0;
    }

    return matlabMatrix;
}

// Returns 1 for Failure and 0 for Success
// copy a Matlab output matrix to a Ptolemy matrix
int MatlabIfc :: MatlabToPtolemy(
		Particle &particle, DataType portType,
		Matrix* matlabMatrix, const char* matrixId, int *errflag) {
    // error flags
    *errflag = FALSE;
    int incompatibleOutportPort = FALSE;

    // allocate a Ptolemy matrix
    int rows = mxGetM(matlabMatrix);
    int cols = mxGetN(matlabMatrix);

    if ( mxIsFull(matlabMatrix) ) {

	// for real matrices, imagp will be null
	double *realp = mxGetPr(matlabMatrix);
	double *imagp = mxGetPi(matlabMatrix);

	// copy Matlab matrices (in column-major order) to Ptolemy
	if ( portType == COMPLEX_MATRIX_ENV ) {
	    ComplexMatrix& Amatrix = *(new ComplexMatrix(rows, cols));
	    for ( int jcol = 0; jcol < cols; jcol++ ) {
		for ( int jrow = 0; jrow < rows; jrow++ ) {
		    double realValue = *realp++;
		    double imagValue = ( imagp ) ? *imagp++ : 0.0;
		    Amatrix[jrow][jcol] = Complex(realValue, imagValue);
		}
	    }
	    // write the matrix to output port j
	    // do not delete Amatrix: particle class handles that
	    particle << Amatrix;
	}
	else if ( portType == FLOAT_MATRIX_ENV ) {
	    FloatMatrix& Amatrix = *(new FloatMatrix(rows, cols));
	    if ( mxIsComplex(matlabMatrix) ) {
		StringList myerrstr;
		myerrstr = "\nImaginary components ignored for the ";
		myerrstr << "Matlab matrix " << matrixId;
		Error::warn(myerrstr);
	    }
	    for ( int jcol = 0; jcol < cols; jcol++ ) {
		for ( int jrow = 0; jrow < rows; jrow++ ) {
		    Amatrix[jrow][jcol] = *realp++;
		}
	    }
	    // write the matrix to output port j
	    // do not delete Amatrix: particle class handles that
	    particle << Amatrix;
	}
	// this situation should never be encountered since the
	// setup method should have checked for output data type
	else {
	    incompatibleOutportPort = TRUE;
	}
    }
    else {
	*errflag = TRUE;
    }

    return incompatibleOutportPort;
}

void MatlabIfc :: FreeMatlabMatrices(Matrix *matlabMatrices[], int numMatrices) {
    if ( matlabMatrices != 0 ) {
	for ( int k = 0; k < numMatrices; k++ ) {
	    if ( matlabMatrices[k] != 0 ) {
		mxFreeMatrix(matlabMatrices[k]);
		matlabMatrices[k] = 0;
	    }
	}
    }
}

void MatlabIfc :: NameMatlabMatrix(Matrix* matrixPtr, const char *name) {
    mxSetName(matrixPtr, name);
}
