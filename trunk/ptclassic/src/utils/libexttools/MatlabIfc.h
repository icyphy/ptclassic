#ifndef _MatlabIfc_h
#define _MatlabIfc_h 1

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
#pragma interface
#endif

#include "StringList.h"

#define MATLAB_BUFFER_LEN        1023
#define MATLAB_ERR_STR           "\x07???"

#define MXCOMPLEX 1
#define MXREAL 0

// Matlab limits filenames to 20 characters (base name + ".m")
// Therefore, Matlab functions must have 18 or fewer characters
#define PTOLEMY_MATLAB_DIRECTORY "$PTOLEMY/lib/matlab"
#define MATLAB_SET_FIGURES       "PtSetStarFigures"
#define MATLAB_CLOSE_FIGURES     "PtCloseStarFigures"
#define MATLAB_UNIX_COMMAND      "matlab"

class MatlabIfc {
public:
	// constructor
	MatlabIfc();

	// destructor
	~MatlabIfc();

	// set data members
	int SetDeleteFigures(int flag);
	const char* SetScriptDirectory(const char* dir);
	const char* SetFigureHandle(const char* handle);
	const char* SetMatlabCommand(const char* command);

	// get data members
	int GetDeleteFigures();
	const char *GetScriptDirectory();
	const char *GetFigureHandle();
	const char *GetMatlabCommand();

	// setup functions
	void NameMatlabMatrices(char *matNames[], int numMatrices,
				const char *baseName);
	const char *BuildMatlabCommand(const char *matlabFunction,
				char *matlabInputNames[], int numInputs,
				char *matlabOutputNames[], int numOutputs);

	// manage Matlab process (low-level methods)
	// derived class must override these methods
	char* MatlabEngineOpen(char* unixCommand);
	int MatlabEngineSend(char* command);
	int MatlabEngineOutputBuffer();
	char* MatlabEngineGetMatrix(char* name);
	char* MatlabEnginePutMatrix(char* matrix);
	int MatlabEngineClose();

        // higher-level interfaces to the Matlab process
	int EvaluateOneCommand(char* command);

	// highest-level interface to the Matlab process
	int StartMatlab();
	int MatlabIsRunning();
	int EvaluateUserCommand();
	int EvaluateUserCommand(char* command);
	int ChangeMatlabDirectory();
	int AttachMatlabFigureIds();
	int CloseMatlabFigures();
	int KillMatlab();

protected:
	// counts how many instances of this class have been created
	static int matlabStarsCount;

	// keeps track of the lone Ptolemy-controlled Matlab process running
	static char* matlabEnginePtr;

	// indicate whether or not to delete created figures upon destruction
	int deleteFigures;

	// script directory containing user scripts
	char *scriptDirectory;

	// place to put the first N output characters from Matlab
	char matlabOutputBuffer[MATLAB_BUFFER_LEN + 1];

	// name attached to Matlab figures
	StringList matlabFigureHandle;

	// Matlab command to execute
	StringList commandString;
}

#endif
