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

 Base class for an external interface to Matlab.
 Only one Matlab process is run and is shared by all of Ptolemy.

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include <string.h>
#include "StringList.h"
#include "InfString.h"
#include "MatlabIfcFuns.h"

#define MATLAB_ERR_STR           "\x07???"

// Matlab limits filenames to 20 characters (base name + ".m")
// Therefore, Matlab functions must have 18 or fewer characters
#define PTOLEMY_MATLAB_DIRECTORY "$PTOLEMY/lib/matlab"
#define MATLAB_SET_FIGURES       "PtSetStarFigures"
#define MATLAB_CLOSE_FIGURES     "PtCloseStarFigures"

class MatlabIfc {
public:
	// constructor
	MatlabIfc();

	// destructor
	~MatlabIfc();

	// Methods to Access Data Members

	// set protected data members
	int SetDeleteFigures(int flag);
	const char* SetScriptDirectory(const char* dir);
	const char* SetFigureHandle(const char* handle);
	const char* SetMatlabCommand(const char* command);
	char* SetOutputBuffer(char *buffer, int bufferlen);
	const char* SetErrorString(const char* string);
	const char* SetWarningString(const char* string);

	// get protected data members
	int GetDeleteFigures();
	const char* GetScriptDirectory();
	const char* GetFigureHandle();
	const char* GetMatlabCommand();
	char* GetOutputBuffer();
	int GetOutputBufferLength();
	char* GetErrorString();
	char* GetWarningString();

	// get static members
	Engine* GetCurrentEngine();
	int GetMatlabIfcInstanceCount();

	// Methods to interface to the Matlab process via the Matlab engine

	// A. low-level interfaces
	Engine* MatlabEngineOpen(char* unixCommand);
	int MatlabEngineSend(char* command);
	int MatlabEngineOutputBuffer(char* buffer, int buferLength);
	Matrix* MatlabEngineGetMatrix(char* name);
	int MatlabEnginePutMatrix(Matrix* matrixPtr);
	int MatlabEngineClose();

        // B. higher-level interfaces to the Matlab process
	int EvaluateOneCommand(char* command);

	// C. highest-level interface to the Matlab process
	int StartMatlab(char* command = 0);
	int MatlabIsRunning();
	int EvaluateUserCommand();
	int EvaluateUserCommand(char* command);
	int ChangeMatlabDirectory();
	int AttachMatlabFigureIds();
	int CloseMatlabFigures();
	int KillMatlab();

	// Methods not using any data members but serve at C++ wrappers

	// A. not specific to Ptolemy
	//    1. generate a list of names for Matlab matrices
	//       use FreeStringArray to deallocate matNames
	void NameMatlabMatrices(char *matNames[],
				int numMatrices,
				const char* baseName);

	//    2. build up a complete Matlab command
	const char* BuildMatlabCommand(
				char* matlabInputNames[], int numInputs,
				const char* matlabFunction,
				char* matlabOutputNames[], int numOutputs);

	//    3. free an array of Matlab matrices
	void FreeMatlabMatrices(Matrix *matlabMatrices[], int numMatrices);

	//    4. name a Matlab matrix in memory
	void NameMatlabMatrix(Matrix* matrixPtr, const char *name);

	// Methods for interface to/from another scripting language (e.g. Tcl)
	// Use FreeStringArray to deallocate realPartStrings/imagPartStrings
	int SetMatlabVariable(const char* name,
			      int numrows, int numcols,
			      char** realPartStrings,
			      char** imagPartStrings);
	int GetMatlabVariable(char* name,
			    int* numrows, int* numcols,
			    char*** realPartStrings,
			    char*** imagPartStrings);
	int UnsetMatlabVariable(char* name);
	int UnsetMatlabVariable(char* name[], int numMatrices);
	void FreeStringArray(char** strarray, int numstrings);

protected:
	// indicate whether or not to delete created figures upon destruction
	int deleteFigures;

	// script directory containing user scripts
	char* scriptDirectory;

	// place to put the first N output characters from Matlab
	char* outputBuffer;
	int outputBufferLen;

	// name attached to Matlab figures
	StringList figureHandle;

	// Matlab command to execute to tag Matlab figures
	InfString figureHandleCommand;

	// set figureHandleCommand to Matlab command to attach tags to figures
	const char* MakeFigureHandleCommand();

	// Matlab command to execute
	InfString commandString;

	// Matlab error string
	InfString errorString;

	// Matlab warning string
	InfString warningString;
};

#endif
