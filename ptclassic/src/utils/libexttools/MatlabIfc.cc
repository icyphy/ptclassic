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
#include "MatlabIfc.h"
#include "MatlabIfcFuns.h"

#define MATLAB_BUFFER_LEN        8192

// counts how many instances of this class have been created
static int matlabStarsCount = 0;

// keeps track of the lone Ptolemy-controlled Matlab process running
static Engine* matlabEnginePtr = 0;

// default buffer to store return strings from Matlab
static char matlabDefaultBuffer[MATLAB_BUFFER_LEN];

// constructor
MatlabIfc :: MatlabIfc() {
    matlabStarsCount++;
    scriptDirectory = 0;
    deleteFigures = FALSE;
    figureHandle = "PtolemyMatlabIfc";
    figureHandle << matlabStarsCount;
    MakeFigureHandleCommand();
    outputBuffer = matlabDefaultBuffer;
    outputBufferLen = MATLAB_BUFFER_LEN;
    outputBuffer[0] = 0;
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
    figureHandle = handle;
    MakeFigureHandleCommand();
    return figureHandle;
}

const char* MatlabIfc :: SetMatlabCommand(const char* command) {
    commandString = command;
    return (const char*) commandString;
}

char* MatlabIfc :: SetOutputBuffer(char *buffer, int bufferlen) {
    outputBuffer = buffer;
    outputBufferLen = bufferlen;
    return outputBuffer;
}

const char* MatlabIfc :: SetErrorString(const char* command) {
    errorString = command;
    return errorString;
}

const char* MatlabIfc :: SetWarningString(const char* command) {
    warningString = command;
    return warningString;
}

// get data members
int MatlabIfc :: GetDeleteFigures() {
    return deleteFigures;
}

const char* MatlabIfc :: GetScriptDirectory() {
    return scriptDirectory;
}

const char* MatlabIfc :: GetFigureHandle() {
    return (const char*) figureHandle;
}

const char* MatlabIfc :: GetMatlabCommand() {
    return (const char*) commandString;
}

char* MatlabIfc :: GetOutputBuffer() {
    return outputBuffer;
}

int MatlabIfc :: GetOutputBufferLength() {
    return outputBufferLen;
}

char* MatlabIfc :: GetErrorString() {
    return errorString;
}

char* MatlabIfc :: GetWarningString() {
    return warningString;
}

// get static data

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
// FIXME: This is a hack to workaround problems in the Matlab 4 engine
// interface that affects System V operating systems like Solaris and HP.
// Problem is that engClose routine issues ioctl(2, TIOCGWINSZ, 0xEFFFC028)
// which attempts to get the window size of terminal that does not exist.
// This showed up when running tycho and opening a Matlab console.
// If one runs tycho as "tycho -ptiny >/dev/null" then the problem does
// not exist.  -- ble, cxh
Engine* MatlabIfc :: MatlabEngineOpen(char* unixCommand) {
    // A better solution is to use ioctl to sever the association
    // from the parent stderr and the matlab engine stderr
    freopen("/dev/null", "w", stderr);
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
    int merror;
    if ( MatlabIsRunning() ) {
	// assert location of buffer to hold output of Matlab commands
	MatlabEngineOutputBuffer(outputBuffer, outputBufferLen - 1);

	// MatlabEngineSend returns 0 on success and non-zero on failure
	outputBuffer[0] = 0;
	merror = MatlabEngineSend(command);
	outputBuffer[outputBufferLen - 1] = 0;

	// kludge: MatlabEngineSend always returns 0 (success) even if
	// there is an error.  Therefore, we must determine if there
	// is an error.  An error occurs if when the output of the
	// Matlab command contains MATLAB_ERR_STR
	if ( merror == 0 ) {
	    merror = ( strstr(outputBuffer, MATLAB_ERR_STR) != 0 );
	}
    }
    else {
	strcpy(outputBuffer, "Matlab is not running!");
	merror = 1;
    }

    if ( merror ) {
	errorString = "\nThe Matlab command `";
	errorString << command;
	errorString << "'\ngave the following error message:\n";
	errorString << outputBuffer;
    }

    return merror;
}

// highest-level interface to the Matlab process

int MatlabIfc :: StartMatlab() {
    KillMatlab();

    // start the Matlab engine which starts Matlab
    matlabEnginePtr = MatlabEngineOpen(MATLAB_UNIX_COMMAND);
    if ( ! MatlabIsRunning() ) {
	errorString = "Could not start Matlab using ";
	errorString << MATLAB_UNIX_COMMAND;
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

    // tag any created figures 
    AttachMatlabFigureIds();

    // evaluate the passed command and report error if one occurred
    int merror = EvaluateOneCommand(command);

    return merror;
}

// ChangeMatlabDirectory:  expand the pathname given by the data member
// scriptDirectory, and if it is a valid directory, change to dirs to it
int MatlabIfc :: ChangeMatlabDirectory() {

    // Keep track of the last directory name in 'lastdirname' only for
    // the purposes of suppressing multiple error messages.
    // Initialize 'lastdirname' to an empty string; otherwise, it
    // will return a null pointer when cast to char* or const char*
    static InfString lastdirname = "";

    int retval = FALSE;
    if ( scriptDirectory && *scriptDirectory ) {
	struct stat stbuf;
	if ( stat(scriptDirectory, &stbuf) == -1 ) {
	    if ( strcmp(lastdirname, scriptDirectory) != 0 ) {
		errorString = "Cannot access the directory ";
		errorString << scriptDirectory;
		lastdirname = scriptDirectory;
	    }
	    retval = FALSE;
	}
	else {
	    InfString changeDirCommand = "cd ";
	    changeDirCommand << scriptDirectory;
	    EvaluateOneCommand(changeDirCommand);
	    retval = TRUE;
	}
    }
    return retval;
}

// establish that all newly created Matlab figures will be
// associated with the arg handle implemented by the Matlab
// function defined by MATLAB_SET_FIGURES in PTOLEMY_MATLAB_DIRECTORY
int MatlabIfc :: AttachMatlabFigureIds() {
    return EvaluateOneCommand(figureHandleCommand);
}

// close all figures associated with arg handle implemented by Matlab
// script defined by MATLAB_CLOSE_FIGURES in PTOLEMY_MATLAB_DIRECTORY
int MatlabIfc :: CloseMatlabFigures() {
    InfString command = MATLAB_CLOSE_FIGURES;
    command << "('" << figureHandle << "');";
    return EvaluateOneCommand(command);
}

int MatlabIfc :: KillMatlab() {
    int retval = TRUE;
    if ( MatlabIsRunning() ) {
	if ( MatlabEngineClose() ) {
	    errorString =
		"Error when terminating connection to the Matlab kernel.";
	    retval = FALSE;
	}
    }
    if ( matlabEnginePtr ) free(matlabEnginePtr);
    matlabEnginePtr = 0;
    return retval;
}

Matrix* MatlabIfc :: PtolemyToMatlab(Particle& particle, DataType portType,
				     int* errflag) {
    // can't use a switch because enumerated data types
    // are assigned to strings and not to integers
    Real* realp = 0;
    Real* imagp = 0;
    Matrix* matlabMatrix = 0;
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
	Real* realp = mxGetPr(matlabMatrix);
	Real* imagp = mxGetPi(matlabMatrix);
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
	Real* realp = mxGetPr(matlabMatrix);
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
	Real* realp = mxGetPr(matlabMatrix);
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
	Real* realp = mxGetPr(matlabMatrix);
	for ( int icol = 0; icol < cols; icol++ ) {
	    for ( int irow = 0; irow < rows; irow++ ) {
		*realp++ = double(Amatrix[irow][icol]);
	    }
	}
    }
    else {
	*errflag = TRUE;
	matlabMatrix = mxCreateFull(1, 1, MXREAL);
	Real* realp = mxGetPr(matlabMatrix);
	*realp = 0.0;
    }

    return matlabMatrix;
}

// Returns 1 for Failure and 0 for Success
// copy a Matlab output matrix to a Ptolemy matrix
int MatlabIfc :: MatlabToPtolemy(
		Particle& particle, DataType portType,
		Matrix* matlabMatrix, int* warnflag, int* errflag) {
    // error and warning flags
    *errflag = FALSE;
    int incompatibleOutportPort = FALSE;
    *warnflag = FALSE;

    // allocate a Ptolemy matrix
    int rows = mxGetM(matlabMatrix);
    int cols = mxGetN(matlabMatrix);

    if ( mxIsFull(matlabMatrix) ) {

	// for real matrices, imagp will be null
	Real* realp = mxGetPr(matlabMatrix);
	Real* imagp = mxGetPi(matlabMatrix);

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
		*warnflag = TRUE;
		warningString =
		    "\nImaginary components ignored for the Matlab matrix ";
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
    if ( matlabMatrices ) {
	for ( int k = 0; k < numMatrices; k++ ) {
	    if ( matlabMatrices[k] ) {
		mxFreeMatrix(matlabMatrices[k]);
		matlabMatrices[k] = 0;
	    }
	}
    }
}

void MatlabIfc :: NameMatlabMatrix(Matrix* matrixPtr, const char *name) {
    mxSetName(matrixPtr, name);
}

// SetMatlabVariable
int MatlabIfc :: SetMatlabVariable(const char* name,
				   int numrows, int numcols,
				   char** realPartStrings,
				   char** imagPartStrings) {
    int realOrComplex = (imagPartStrings) ? MXCOMPLEX : MXREAL;
    Matrix* newMatlabMatrixPtr = mxCreateFull(numrows, numcols, realOrComplex);

    // check for memory allocation error
    if ( newMatlabMatrixPtr == 0 ) return FALSE;

    // copy real part over to new Matlab matrix
    // Matlab stores values in column-major order like Fortran
    Real* realp = mxGetPr(newMatlabMatrixPtr);
    for ( int jcol = 0; jcol < numcols; jcol++ ) {
	// index = jrows * numcols + jcol
	int index = jcol;
	for ( int jrow = 0; jrow < numrows; jrow++ ) {
	    realp[index] = atof(*realPartStrings++);
	    index += numcols;
	}
    }

    // copy imag part over to new Matlab matrix (if complex)
    // Matlab stores values in column-major order like Fortran
    if ( realOrComplex == MXCOMPLEX ) {
	Real* imagp = mxGetPi(newMatlabMatrixPtr);
	for ( int jcol = 0; jcol < numcols; jcol++ ) {
	    // index = jrows * numcols + jcol
	    int index = jcol;
	    for ( int jrow = 0; jrow < numrows; jrow++ ) {
		imagp[index] = atof(*imagPartStrings++);
		index += numcols;
	    }
	}
    }

    mxSetName(newMatlabMatrixPtr, name);
    MatlabEnginePutMatrix(newMatlabMatrixPtr);
    mxFreeMatrix(newMatlabMatrixPtr);

    return TRUE;
}

// GetMatlabVariable
int MatlabIfc :: GetMatlabVariable(char* name,
				   int* numrows, int* numcols,
				   char*** realPartStrings,
				   char*** imagPartStrings) {

    int retval = TRUE;
    Matrix* matlabMatrix = MatlabEngineGetMatrix(name);

    if ( matlabMatrix ) {
	*numrows = mxGetM(matlabMatrix);
	*numcols = mxGetN(matlabMatrix);
	Real* realp = mxGetPr(matlabMatrix);
	Real* imagp = mxGetPi(matlabMatrix);
	int numelements = (*numrows) * (*numcols);
	// Under cfront compiler, we cannot use (char *) instead of char* here
	*realPartStrings = new char* [numelements];
	if ( imagp ) *imagPartStrings = new char* [numelements];
	else *imagPartStrings = 0;
	for ( int jcol = 0; jcol < *numcols; jcol++ ) {
	    // index = jrows * numcols + jcol
	    int index = jcol;
	    for ( int jrow = 0; jrow < *numrows; jrow++ ) {
		StringList realstring = *realp++;
		(*realPartStrings)[index] = savestring(realstring);
		if ( imagp ) {
		    StringList imagstring = *imagp++;
		    (*imagPartStrings)[index] = savestring(imagstring);
		}
		index += *numcols;
	    }
	}
	mxFreeMatrix(matlabMatrix);
    }
    else {
	retval = FALSE;
	*numrows = 0;
	*numcols = 0;
	*realPartStrings = 0;
	*imagPartStrings = 0;
    }

    return retval;
}

int MatlabIfc :: UnsetMatlabVariable(char* name) {
    InfString command = "clear ";
    command << name;
    return EvaluateOneCommand(command);
}

int MatlabIfc :: UnsetMatlabVariable(char* name[], int numMatrices) {
    int errorFlag = FALSE;
    for (int i = 0; i < numMatrices; i++) {
      int merror = UnsetMatlabVariable(name[i]);
      if ( merror ) errorFlag = TRUE;
    }
    return errorFlag;
}

void MatlabIfc :: FreeStringArray(char** strarray, int numstrings) {
    if ( strarray ) {
	for ( int k = 0; k < numstrings; k++ ) {
	    delete [] strarray[k];
	    strarray[k] = 0;
	}
    }
}

const char* MatlabIfc :: MakeFigureHandleCommand() {
    figureHandleCommand = MATLAB_SET_FIGURES;
    figureHandleCommand << "('" << figureHandle << "');";
    return figureHandleCommand;
}
