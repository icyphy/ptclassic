defstar {
	name { Matlab }
	domain { SDF }
	desc {
Base star for Matlab interface stars in the SDF domain.
	}
	version { $Id$ }
	author { Brian L. Evans }
	acknowledge { Steve L. Eddins, The MathWorks Inc., Natick, MA }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
.Ir "Matlab interface"
.Ir "Eddins, S."
All instances of Matlab stars share one Matlab process.
A Matlab kernel is started during the setup phase if a kernel has not
yet been started, i.e., if only one instance of this base star exists.
The Unix command executed to start Matlab is simply "matlab".
If "matlab" is not on your path as defined by your .cshrc file,
then this star should fail gracefully when fired.
When Ptolemy exits, Ptolemy will kill the Matlab kernel.

The \fIScriptDirectory\fR state specifies where to find customized Matlab
scripts and establishes a default directory for writing output files.
If \fIScriptDirectory\fR is not empty, then the star will tell Matlab
to change directories to this directory before the Matlab command
defined by this star is evaluated.
This allows custom scripts to be written without changing the value
of Matlab's path.
If you are writing values to sub-directory using Matlab commands, then
it is best to specify a path name for the \fIScriptDirectory\fR
to ensure that the Matlab command will write to the same sub-directory
each time the Matlab command defined by this star is evaluated.

The \fIDeleteOldFigures\fR setting allows the user to control whether
or not the Matlab figures generated by this star (if any) will be deleted
when the star's destructor method is called.

In the current implementation, the Matlab command can be a concatenation
of several Matlab statements.
A trailing semicolon is added when missing to all Matlab commands in order
to suppress output of the last statement in the command.
All textual output produced by the Matlab command is captured in a buffer
local to the star.
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
	defstate {
		name { DeleteOldFigures }
		type { int }
		default { YES }
		desc {
If this state is YES or TRUE, then old figures generated by Matlab
will be deleted whenever the destructor for this star is run, e.g.
when the run panel in the graphical interface is closed.
}
	}


	hinclude { <sys/types.h>, <sys/stat.h>, <ctype.h>, "miscFuncs.h", "InfString.h" }

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

#define MATLAB_BUFFER_LEN	 1023
#define MATLAB_ERR_STR		 "\x07???"

// Matlab limits filenames to 20 characters (base name + ".m")
// Therefore, Matlab functions must have 18 or fewer characters
#define PTOLEMY_MATLAB_DIRECTORY "$PTOLEMY/src/domains/sdf/matlab/stars"
#define MATLAB_SET_FIGURES       "PtSetStarFigures"
#define MATLAB_CLOSE_FIGURES     "PtCloseStarFigures"
#define MATLAB_UNIX_COMMAND      "matlab"
	}

	protected {
		// Matlab (C) structures
		static Engine *matlabEnginePtr;
		static int matlabStarsCount;

		// place to put the first N output characters from Matlab
		char matlabOutputBuffer[MATLAB_BUFFER_LEN + 1];

		// name attached to Matlab figures
		StringList matlabFigureHandle;
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
                // close any figures associated with this star
		if ( int(DeleteOldFigures) ) {
		  closeMatlabFigures(matlabFigureHandle);
		}

		matlabStarsCount--;
		if ( matlabStarsCount == 0 ) {
		  killMatlab();
		}
	}

	setup {
		if ( ! matlabIsRunning() ) {
		  startMatlab();
		}

		matlabFigureHandle = fullName();
	}

	go {
                // tag any created figures with the handle for this star
		setMatlabFigureIds(matlabFigureHandle);
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
		int length = strlen(matlabFunction);

		// replace pound signs in matlabFunction with underscores
		char *matlabFragment = new char[length + 1];
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
		  if ( ( strchr(matlabFragment, '(') == 0 ) &&
		       ( numInputs > 0 ) ) {
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
	  }
	}

	method {
	  name { evaluateOneMatlabCommand }
	  access { protected }
	  type { int }
	  arglist { "(char *matlabCommand)" }
	  code {
	      if (matlabIsRunning() ) {
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
		// is an error.  An error occurs if when the output of the
		// Matlab command contains MATLAB_ERR_STR
		if ( merror == 0 ) {
		  merror = ( strstr(matlabOutputBuffer, MATLAB_ERR_STR) != 0 );
		}

		return( merror );
              } else {
 	        strcpy(matlabOutputBuffer, "Matlab is not running!");
 	        return 1;
              }
	  }
	}

	method {
	  name { changeMatlabDirectory }
	  access { protected }
	  type { int }
	  arglist { "(const char *dirname)" }
	  code {
		// expand the pathname and check its existence
		static InfString lastdirname;
                int retval = 0;
		if ( dirname[0] != 0 ) {
		  const char *fulldirname = expandPathName(dirname);
		  struct stat stbuf;
		  if ( stat(fulldirname, &stbuf) == -1 ) {
		    if ( strcmp((char *) lastdirname, fulldirname) != 0 ) {
		      Error::warn( *this,
				   "Cannot access the directory ",
				   fulldirname );
		      lastdirname = fulldirname;
		    }
                    retval = 0;
		  }
		  else {
		    char *changeDirCommand = new char[strlen(fulldirname) + 4];
		    strcpy(changeDirCommand, "cd ");
		    strcat(changeDirCommand, fulldirname);
		    evaluateOneMatlabCommand(changeDirCommand);
		    delete [] changeDirCommand;
                    retval = 1;
		  }
		  delete [] fulldirname;
		}
                return retval;
	  }
	}

	method {
	  name { evaluateMatlabCommand }
	  access { protected }
	  type { int }
	  arglist { "(char *matlabCommand)" }
	  code {
		// change directories to one containing the Matlab command
		changeMatlabDirectory( (const char *) ScriptDirectory );

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

	// establish that all newly created Matlab figures will be
	// associated with the arg handle implemented by the Matlab
	// function defined by MATLAB_SET_FIGURES in PTOLEMY_MATLAB_DIRECTORY
	method {
	  name { setMatlabFigureIds }
	  access { protected }
	  type { int }
	  arglist { "(const char *handle)" }
	  code {
		InfString command = MATLAB_SET_FIGURES;
		command << "('";
		command << handle;
		command << "');";
		return( evaluateOneMatlabCommand(command) );
	  }
	}

        // close all figures associated with arg handle implemented by Matlab
	// script defined by MATLAB_CLOSE_FIGURES in PTOLEMY_MATLAB_DIRECTORY
	method {
	  name { closeMatlabFigures }
	  access { protected }
	  type { int }
	  arglist { "(const char *handle)" }
	  code {
		InfString command = MATLAB_CLOSE_FIGURES;
		command << "('";
		command << handle;
		command << "');";
		return( evaluateOneMatlabCommand(command) );
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

		// start the Matlab engine which starts Matlab
		matlabEnginePtr = engOpen(MATLAB_UNIX_COMMAND);
		if ( matlabEnginePtr == 0 ) {
		  Error::abortRun( *this, "Could not start Matlab using ",
				   MATLAB_UNIX_COMMAND );
                  return;
		}

		// add the PTOLEMY_MATLAB_DIRECTORY to the Matlab path
		const char *fulldirname =
				expandPathName(PTOLEMY_MATLAB_DIRECTORY);
		InfString command = "path(path, '";
		command << fulldirname;
		command << "');";
		evaluateOneMatlabCommand(command);
		delete [] fulldirname;
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
