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
.Ir "Matlab interface"
The Matlab kernel is started during the setup phase if no other kernels
have been started, i.e., if only one instance of this base star exists.
The Unix command executed to start Matlab is the string
\fIOSStartCommand\fR.
The Matlab kernel is exitted during the wrapup phase.

The \fIScriptDirectory\fR is an optional directory specifying where
to find customized Matlab scripts.
If \fIScriptDirectory\fR is specified, then the star will tell Matlab
to change directories to this directory before the Matlab command
defined by this star is evaluated.
This allows custom scripts to be written without changing the value
of Matlab's path.
	}
	defstate {
		name { OSStartCommand }
		type { string }
		default { "matlab " }
		desc { Command given to the operating system to start Matlab. }
	}
	defstate {
		name { ScriptDirectory }
		type { string }
		default { "" }
		desc {
An optional directory specifying where to find any custom Matlab files.
You may use tilde characters and environment variables in the directory name.
}
	}

	hinclude { <sys/types.h>, <sys/stat.h>, "miscFuncs.h", "InfString.h" }

	header{
// Matlab interface library and Matlab data types (clash with COMPLEX)
extern "C" {
// matrix.h and engine.h are not actually system include files, but if
// we refer to them as "matrix.h" and "engine.h", then make depend
// will include them in the makefile.  If a user does not have Matlab,
// then they will have to regenerate a makefile because the makefile
// refers to Matlab files that they do not have.  A negative side
// effect of this is that if these files change, then this file will
// not be automatically recompiled (cxh)

#include <matrix.h>
#include <engine.h>

// Give Matlab's definition of COMPLEX and REAL different names
#undef  COMPLEX
#undef  REAL
#define MXCOMPLEX  1
#define MXREAL     0
}

#define MATLAB_BUFFER_LEN	1023
#define MATLAB_ERROR_STRING	">> \x07???"
#define MATLAB_ERROR_STRING_LEN	( sizeof(MATLAB_ERROR_STRING) - 1 )
	}

	protected {
		// Matlab (C) structures
		static Engine *matlabEnginePtr;
		static int matlabStarsCount;

		// place to put the first N output characters from Matlab
		char matlabOutputBuffer[MATLAB_BUFFER_LEN + 1];
	}

	code {
		// Initialization of static members
		Engine *SDFMatlab::matlabEnginePtr = 0;
		int SDFMatlab::matlabStarsCount = 0;
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
	  arglist { "(InfString matNames[], int numMatrices, const char *baseName)" }
	  code {
		// generate names for Matlab versions of input matrix names
		for ( int i = 0; i < numMatrices; i++ ) {
		    char numstr[32];
		    matNames[i] = baseName;
		    sprintf(numstr, "_%d", i+1);
		    matNames[i] << numstr;
		}
	  }
	}

	method {
	  name { buildMatlabCommand }
	  access { protected }
	  type { void }
	  arglist { "(InfString& commandString, InfString matlabInputNames[], int numInputs, const char *matlabFunction, InfString matlabOutputNames[], int numOutputs)" }
	  code {
		// replace pound signs in matlabFunction with underscores
		int length = strlen(matlabFunction);
		char *matlabFragment = new char[length + 1];
		strcpy(matlabFragment, matlabFunction);
		for (int i = 0; i < length; i++ ) {
		  if ( matlabFragment[i] == '#' ) {
		    matlabFragment[i] = '_';
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
		  if ( ( strchr(matlabFragment, '(') == 0 ) &&
		       ( numInputs > 0 ) ) {
		    commandString << "(" << matlabInputNames[0];
		    for ( int i = 1; i < numInputs; i++ ) {
		      commandString << ", " << matlabInputNames[i];
		    }
		    commandString << ")";
		  }
		}

		// suppress textual output by adding a semicolon at the end
		if ( strchr(matlabFragment, ';') == 0 ) {
		  commandString << ";";
		}

		delete [] matlabFragment;
	  }
	}

	method {
	  name { evaluateOneMatlabCommand }
	  access { protected }
	  type { int }
	  arglist { "(const char *matlabCommand)" }
	  code {
		// assert location of buffer to hold output of Matlab commands
		engOutputBuffer(matlabEnginePtr,
				matlabOutputBuffer,
				MATLAB_BUFFER_LEN);

		// engEvalString returns 0 on success and non-zero on failure
		matlabOutputBuffer[0] = 0;
		int merror = engEvalString(matlabEnginePtr, matlabCommand);
		matlabOutputBuffer[MATLAB_BUFFER_LEN] = 0;

		// kludge: engEvalString always returns 0 (success) even if
		// there is an error.  Therefore, we must determine if there
		// is an error.  Since we terminate Matlab commands with a
		// semicolon, an error occurs when the output of the Matlab
		// command is not null.
		if ( merror == 0 ) {
		  merror = ( matlabOutputBuffer[0] != 0 );
		}

		return( merror );
	  }
	}


	method {
	  name { evaluateMatlabCommand }
	  access { protected }
	  type { int }
	  arglist { "(const char *matlabCommand)" }
	  code {
		// change directories to one containing the Matlab command
		// expand the pathname and check its existence
		static InfString lastdirname;
		const char *dirname = (const char *) ScriptDirectory;
		if ( dirname[0] != 0 ) {
		  const char *expandeddirname = expandPathName(dirname);
		  char *fulldirname = new char[strlen(expandeddirname) + 1];
		  strcpy(fulldirname, expandeddirname);
		  struct stat stbuf;
		  if ( stat(fulldirname, &stbuf) == -1 ) {
		    if ( strcmp((char *) lastdirname, fulldirname) != 0 ) {
		      Error::warn( *this,
				   "Cannot access the directory ",
				   fulldirname );
		      lastdirname = fulldirname;
		    }
		  }
		  else {
		    char *changeDirCommand = new char[strlen(fulldirname) + 4];
		    strcpy(changeDirCommand, "cd ");
		    strcat(changeDirCommand, fulldirname);
		    evaluateOneMatlabCommand(changeDirCommand);
		    delete [] changeDirCommand;
		  }
		  delete [] fulldirname;
		}

		// evaluate the passed command and report error if one occurred
		int merror = evaluateOneMatlabCommand(matlabCommand);
		if ( merror ) {
		  InfString errstr;
		  errstr = "\nThe Matlab command `";
		  errstr << matlabCommand;
		  errstr << "'\ngave the following error message:\n";
		  Error::warn(*this, (const char *) errstr, matlabOutputBuffer);
		}

		return(merror);
	  }
	}

	method {
	  name { getMatlabMatrix }
	  access { protected }
	  type { "Matrix *" }
	  arglist { "(char *name)" }
	  code {
		return( engGetMatrix(matlabEnginePtr, name) );
	  }
	}

	method {
	  name { putMatlabMatrix }
	  access { protected }
	  type { int }
	  arglist { "(Matrix *matlabMatrix)" }
	  code {
		return( engPutMatrix(matlabEnginePtr, matlabMatrix) );
	  }
	}

	method {
	  name { startMatlab }
	  access { protected }
	  type { void }
	  arglist { "()" }
	  code {
		killMatlab();

		matlabEnginePtr = engOpen( ((const char *) OSStartCommand) );
		if ( matlabEnginePtr == 0 ) {
		  Error::abortRun( *this, "Could not start Matlab using ",
				   (const char *) OSStartCommand );
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
