defstar {
	name { Matlab }
	domain { SDF }
	desc {
Base star for Matlab interface stars in the SDF domain.
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
The Matlab kernel is started during the setup phase if no other kernels
have been started, i.e., if only one instance of this base star exists.
.Ir "Matlab interface"
	}
	defstate {
		name { options }
		type { string }
		default { "" }
		desc { Command line options for Matlab. }
	}
	defstate {
		name { where_defined }
		type { string }
		default { "" }
		desc {
.EQ
delim @@
.EN
An optional pathname specifying where to find any custom Matlab files.
The pathname may not contain the tilde (~) character (use $HOME instead).
Before the Matlab command defined by this star is evaluated,
Matlab will change directories to this one.
.EQ
delim $$
.EN
}
	}

	// matrix.h and engine.h are provided with Matlab
	header{
// Matlab interface library and Matlab data types (clash with COMPLEX)
extern "C" {
#include "matrix.h"
#include "engine.h"

// Give Matlab's definition of COMPLEX and REAL different names
#undef  COMPLEX
#undef  REAL
#define MXCOMPLEX  1
#define MXREAL     0
}
	}

	protected {
		// Matlab (C) structures
		static Engine *matlabEnginePtr;
		static int matlabStarsCount;
	}

	code {
		// Initialization of static members
		int SDFMatlab::matlabStarsCount = 0;
		Engine *SDFMatlab::matlabEnginePtr = 0;
	}

	constructor {
		matlabStarsCount++;
	}

	destructor {
		matlabStarsCount--;
		if ( matlabStarsCount == 0 ) {
		  killMatlab();
		}
	}

	setup {
		if ( ! matlabIsRunning() ) {
		  startMatlab();
		}
	}

	method {
	  name { nameMatlabMatrices }
	  access { protected }
	  type { void }
	  arglist { "(StringList matNames[], int numMatrices, char *baseName)" }
	  code {
		// generate names for Matlab versions of input matrix names
		for ( int i = 0; i < numMatrices; i++ ) {
		    char numstr[16];
		    matNames[i] = baseName;
		    sprintf(numstr, "%d", i+1);
		    matNames[i] << numstr;
		}
	  }
	}

	method {
	  name { buildMatlabCommand }
	  access { protected }
	  type { void }
	  arglist { "(StringList& commandString, StringList matlabInputNames[], int numInputs, char *matlabFunction, StringList matlabOutputNames[], int numOutputs)" }
	  code {
		// create the command to be sent to the Matlab interpreter
		commandString = "";
		if ( numOutputs > 0 ) {
		  commandString << "[" << matlabOutputNames[0];
		  for ( int i = 1; i < numOutputs; i++ ) {
		    commandString << ", " << matlabOutputNames[i];
		  }
		  commandString << "] = ";
		}
		commandString << ((char *) matlabFunction);
		if ( numInputs > 0 ) {
		  commandString << "(" << matlabInputNames[0];
		  for ( int i = 1; i < numInputs; i++ ) {
		    commandString << ", " << matlabInputNames[i];
		  }
		  commandString << ")";
		}
	  }
	}

	method {
	  name { evaluateMatlabCommand }
	  access { protected }
	  type { int }
	  arglist { "(char *matlabCommand)" }
	  code {
		// change directories to one containing the command
		char *pathname = (char *) where_defined;
		if ( pathname[0] != '\0' ) {
		  char *changeDirCommand = new char[strlen(pathname) + 4];
		  strcpy(changeDirCommand, "cd ");
		  strcat(changeDirCommand, pathname);
		  int cdstatus =
		    engEvalString(matlabEnginePtr, changeDirCommand);
		  char numstr[64];
		  sprintf(numstr,
		  	  "Matlab returned %d indicating that it could not ",
			  cdstatus);
		  if ( cdstatus != 0 ) {
		    Error::warn( *this,
				 numstr,
				 "change directories using the command ",
				 changeDirCommand );
		  }
		  delete [] changeDirCommand;
		}

		int mstatus = engEvalString( matlabEnginePtr, matlabCommand );
		if ( mstatus != 0 ) {
		  char numstr[64];
		  sprintf(numstr, "Matlab returned %d indicating ", mstatus);
		  Error::abortRun( *this, numstr,
				   "an error in the Matlab command ",
				   matlabCommand );
		}
	  }
	}

	method {
	  name { startMatlab }
	  access { protected }
	  type { void }
	  arglist { "()" }
	  code {
		killMatlab();

		StringList shellCommand = "matlab "; 
		shellCommand << ((char *) options);
		matlabEnginePtr = engOpen( ((char *) shellCommand) );
		if ( matlabEnginePtr == 0 ) {
		  Error::abortRun( *this, "Could not start Matlab using ",
				   (char *) shellCommand );
		}
	  }
	}

	method {
	  name { matlabIsRunning }
	  access { protected }
	  type { int }
	  arglist { "()" }
	  code {
		return( matlabEnginePtr != 0 );
	  }
	}

	method {
	  name { killMatlab }
	  access { protected }
	  type { void }
	  arglist { "()" }
	  code {
		if ( matlabIsRunning() ) {
		  if ( engClose( matlabEnginePtr ) ) {
		    Error::warn(*this, "Error when terminating connection ",
				"to the Matlab kernel.");
		  }
		}
		matlabEnginePtr = 0;
	  }
	}
}
