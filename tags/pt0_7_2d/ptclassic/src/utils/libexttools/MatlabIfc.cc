static const char file_id[] = "MatlabIfc.cc";

/**************************************************************************
Version identification:
@(#)MatlabIfc.cc	1.36	04/20/98

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Base class for an external interface to Matlab.

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
#include "Linker.h"

#define MATLAB_BUFFER_LEN 8192

// Default command to start the Matlab engine.  See the {Matlab External
// Interface Guide For Unix Workstations}, 1993, page 2-14, under the
// description for the engOpen command.  We use 'matlab' so as to allow
// the user to define the matlab script to do special functions.
#define MATLAB_START_COMMAND      "matlab"

// counts how many instances of this class have been created
static int matlabStarsCount = 0;

// keeps track of the lone controlled Matlab process that is running
static Engine* matlabEnginePtr = 0;

// default buffer to store return strings from Matlab
static char matlabDefaultBuffer[MATLAB_BUFFER_LEN];

// pointers to matlab functions
#include "MatlabIfc.cc.Auto1"

// constructor
MatlabIfc :: MatlabIfc() {
    matlabStarsCount++;
    scriptDirectory = 0;
    deleteFigures = FALSE;
    figureHandle = "MatlabIfc";
    figureHandle << matlabStarsCount;
    MakeFigureHandleCommand();
    outputBuffer = matlabDefaultBuffer;
    outputBufferLen = MATLAB_BUFFER_LEN;
    outputBuffer[0] = 0;

#if MATLABSHARED
    if (! Linker::enabled())
	Error::abortRun( "Internal error - Linker isn't enabled");
    else { 
	StringList linkArg = "-L";
	linkArg << getenv("PTOLEMY") << "/lib." << getenv("PTARCH")
		<< " -lptmatlab";
	if (Linker::multiLink(linkArg, 1) == FALSE) {
#include "MatlabIfc.cc.Auto2"
    	}

    }
#elif MATLABSTATIC
    // do nothing
#else
#include "MatlabIfc.cc.Auto2"
#endif
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

// error
void MatlabIfc :: MatlabEngineError() {
#ifdef MATLABSHARED
    Error::abortRun("The Matlab shared libraries could not be found.");
#else
    Error::abortRun("The external interface to Matlab has not been compiled "
		    "into Ptolemy.");
#endif
}
    
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
mxArray* MatlabIfc :: MatlabEngineGetMatrix(char* name) {
    return engGetArray(matlabEnginePtr, name);
}

// put a matrix in the Matlab environment (Matlab will copy the matrix)
int MatlabIfc :: MatlabEnginePutMatrix(mxArray* matrixPtr) {
    return engPutArray(matlabEnginePtr, matrixPtr);
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

// Highest-level interface to the Matlab process.  We will start
// Matlab by using (1) userCommand if it is non-null, or (2) a
// remote shell to the machine given by PTMATLAB_REMOTE_HOST
// if the environment variable PTMATLAB_REMOTE_HOST is set, or
// (3) "matlab".  For cases (2) and (3), we run matlab inside
// a shell process so that we can redirect standard error as
// a temporary workaround to the following bug in Matlab 4.
//
// FIXME: The Matlab 4 engine interface has an undesirable side effects
// on System V operating systems like Solaris and HP.  The problem is
// that engClose routine issues ioctl(2, TIOCGWINSZ, 0xEFFFC028) which
// attempts to get the window size of terminal that does not exist.
// This showed up when running tycho and opening a Matlab console.
// If one runs tycho as "tycho -ptiny >/dev/null" then the problem does
// not exist.  See about for a temporary fix-- ble, cxh
int MatlabIfc :: StartMatlab(char* userCommand) {
    InfString command;
    const char* matlabServer = 0;

    KillMatlab();

    // Use the command specified by the user if given:  this allows
    // this interface to be used on platforms other than Unix
    if (userCommand) {
	command = userCommand;
    }
    else {
	// Construct the basic shell command for the Matlab process
	// FIXME: This assumes that we are using Unix and X windows
        const char* display = getenv("DISPLAY");
	command = "/bin/sh -c \'";
	if (display) {
	   command <<  "DISPLAY=" << display << "; export DISPLAY; ";
	}
	command << MATLAB_START_COMMAND << " 2>/dev/null'";

	// See if the user wants to run the Matlab process on a remote machine
        matlabServer = getenv("PTMATLAB_REMOTE_HOST");
	if (matlabServer) {
	    // See the {Matlab External Interface Guide For Unix Workstations},
	    // 1993, page 2-14, under the explanation of the engOpen command.
	    // We modified the example to use sh instead of csh to improve
	    // portability. - ble, xw
	    StringList remoteCommand = "rsh";
	    const char* username = getenv("USER");
	    if (username == 0) {
	        username = getenv("LOGNAME");
	    }
	    if (username) {
		remoteCommand << " -l " << username;
	    }
	    remoteCommand <<  " " << matlabServer;
	    remoteCommand << " \"" << command << "\"";
	    command = remoteCommand;
	}
    }

    // start the Matlab engine which starts Matlab
    matlabEnginePtr = MatlabEngineOpen(command);
    if ( ! MatlabIsRunning() ) {
	if ( matlabServer ) {
	    errorString = "Could not start Matlab remotely on ";
	    errorString << matlabServer << " using '";
	    errorString << command << "'.";
	    errorString << " Please check rsh/rlogin permission"
			<< " to the remote machine.";
	}
	else {
	    errorString = "Could not start Matlab using '";
	    errorString << command << "'.";
	}
	return FALSE;
    }

    // add the MATLAB_SCRIPT_DIR to the Matlab path
    char* fulldirname = expandPathName(MATLAB_SCRIPT_DIR);
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
// function defined by MATLAB_SET_FIGURES in MATLAB_SCRIPT_DIR
int MatlabIfc :: AttachMatlabFigureIds() {
    return EvaluateOneCommand(figureHandleCommand);
}

// close all figures associated with arg handle implemented by Matlab
// script defined by MATLAB_CLOSE_FIGURES in MATLAB_SCRIPT_DIR
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

void MatlabIfc :: FreeMatlabMatrices(mxArray *matlabMatrices[], int numMatrices) {
    if ( matlabMatrices ) {
	for ( int k = 0; k < numMatrices; k++ ) {
	    if ( matlabMatrices[k] ) {
		mxDestroyArray(matlabMatrices[k]);
		matlabMatrices[k] = 0;
	    }
	}
    }
}

void MatlabIfc :: NameMatlabMatrix(mxArray* matrixPtr, const char *name) {
    mxSetName(matrixPtr, name);
}

// SetMatlabVariable
int MatlabIfc :: SetMatlabVariable(const char* name,
				   int numrows, int numcols,
				   char** realPartStrings,
				   char** imagPartStrings) {
    mxComplexity realOrComplex = (imagPartStrings) ? mxCOMPLEX : mxREAL;
    mxArray* newMatlabMatrixPtr = mxCreateDoubleMatrix(numrows, numcols, realOrComplex);

    // check for memory allocation error
    if ( newMatlabMatrixPtr == 0 ) return FALSE;

    // copy real part over to new Matlab matrix
    // Matlab stores values in column-major order like Fortran
    double* realp = mxGetPr(newMatlabMatrixPtr);
    for ( int jcol = 0; jcol < numcols; jcol++ ) {
	// to convert row-major to column-major ordering use
	// index = jcol * numrows + jrow
	int index = jcol * numrows;
	for ( int jrow = 0; jrow < numrows; jrow++ ) {
	    realp[index] = atof(*realPartStrings++);
	    index++;
	}
    }

    // copy imag part over to new Matlab matrix (if complex)
    // Matlab stores values in column-major order like Fortran
    if ( realOrComplex == mxCOMPLEX ) {
	double* imagp = mxGetPi(newMatlabMatrixPtr);
	for ( int jcol = 0; jcol < numcols; jcol++ ) {
	    // to convert row-major to column-major ordering use
	    // index = jcol * numrows + jrow
	    int index = jcol * numrows;
	    for ( int jrow = 0; jrow < numrows; jrow++ ) {
		imagp[index] = atof(*imagPartStrings++);
		index++;
	    }
	}
    }

    mxSetName(newMatlabMatrixPtr, name);
    MatlabEnginePutMatrix(newMatlabMatrixPtr);
    mxDestroyArray(newMatlabMatrixPtr);

    return TRUE;
}

// GetMatlabVariable
int MatlabIfc :: GetMatlabVariable(char* name,
				   int* numrows, int* numcols,
				   char*** realPartStrings,
				   char*** imagPartStrings) {

    int retval = TRUE;
    mxArray* matlabMatrix = MatlabEngineGetMatrix(name);

    if ( matlabMatrix ) {
	*numrows = mxGetM(matlabMatrix);
	*numcols = mxGetN(matlabMatrix);
	double* realp = mxGetPr(matlabMatrix);
	double* imagp = mxGetPi(matlabMatrix);
	int numelements = (*numrows) * (*numcols);
	// Under cfront compiler, we cannot use (char *) instead of char* here
	*realPartStrings = new char* [numelements];
	if ( imagp ) *imagPartStrings = new char* [numelements];
	else *imagPartStrings = 0;
	for ( int jcol = 0; jcol < *numcols; jcol++ ) {
	    // to convert column-major order to row-major order use
	    // index = jcol * numrows + jrow
	    int index = jcol * (*numrows);
	    for ( int jrow = 0; jrow < *numrows; jrow++ ) {
		StringList realstring = *realp++;
		(*realPartStrings)[index] = savestring(realstring);
		if ( imagp ) {
		    StringList imagstring = *imagp++;
		    (*imagPartStrings)[index] = savestring(imagstring);
		}
		index++;
	    }
	}
	mxDestroyArray(matlabMatrix);
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
