ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/19/90
 Revised (from Xgraph) by E. A. Lee to accept multiple inputs, 9/18/90.
 Revised 10/3/90 to work under the preprocessor.

 Draws a graph with the xgraph function.  It is assumed that "xgraph"
 is on your path, or this will not work!!!

 This version is for the SDF domain.
**************************************************************************/

#define MAXNOINPUTS 10
}

defstar {
	name { XMgraph }
	domain { SDF }
	desc { "Generate a multi-signal plot with the xgraph program." }
	inmulti {
		name { input }
		type { float }
	}
	defstate {
		name {title}
		type {string}
		default {"X graph"}
		desc {"graph title"}
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc {"file to save xgraph input"}
	}
	defstate {
		name {options}
		type {string}
		default {""}
		desc {"command line options for xgraph"}
	}
	protected {
		FILE *strm[MAXNOINPUTS];
		int delFile;
		const char* fileName[MAXNOINPUTS];
		int index;
	}
	ccinclude { "miscFuncs.h" }
// constructor initializes streams and filenames
	constructor {
		for (int i = 0; i < MAXNOINPUTS; i++) {
			strm[i] = NULL;
			fileName[i] = NULL;
		}
		delFile = FALSE;
	}
// start function: open temp files for each input stream
	start {
		StringList msg;
		int noIns;
		if((noIns = input.numberPorts()) > MAXNOINPUTS) {
			Error::abortRun (*this, ": too many inputs");
			return;
		}
		for (int i = noIns; i>0; i--) {
			fileName[i] = tempFileName();
			// open and make sure the file is writable
			if ((strm[i] = fopen (fileName[i], "w")) == NULL) {
				msg = readFullName();
				msg += ": Can't open temporary file for writing: ";
				msg += fileName[i];
				Error::abortRun (msg);
			}
		}
		index = 0;
		input.reset();
	}
// go.  Does nothing if open failed.
// Otherwise write to temporary files.
	go {
		for (int i = input.numberPorts(); i>0; i--) {
			if (strm[i])
				fprintf (strm[i], "%d %g\n",
					 index, double(input()%0));
		}
		index++;
	}
// wrapup.  Does nothing if open failed, or 2nd wrapup call.
// closes files and execs the program.
	wrapup {
		for (int i = input.numberPorts(); i>0; i--) {
			if (strm[i]) {
				fclose (strm[i]);
				strm[i] = NULL;
			}
		}
		exec("");
	}
// destructor: close and delete file if open
	destructor {
		for (int i = input.numberPorts(); i>0; i--) {
			if (strm[i]) fclose (strm[i]);
			if (delFile) unlink (fileName[i]);
			delete fileName[i];
		}
	}
// execute the program
// extraOpts is mainly for derived stars.
	method {
            name { exec }
            access { protected }
            arglist { "(const char* extraOpts)" }
            type { void }
            code {
                StringList cmd;
    
                cmd += "( xgraph ";
    
                const char* t = title;
                if (t && *t) {
                    cmd += "-t '";
                    cmd += t;
                    cmd += "' ";
                }
                const char* o = options;
                if (o && *o) {
                    cmd += o;
                    cmd += " ";
                }
                if (extraOpts && *extraOpts) {
                    cmd += extraOpts;
                    cmd += " ";
                }
                for (int i = input.numberPorts(); i>0; i--) {
                    cmd += fileName[i];
                    cmd += " ";
                }
    
                char* sf = saveFile;
                if (sf != NULL && *sf != 0) {
                    const char* saveFileName = savestring (expandPathName(sf));
    
                    // Easiest way to check to see whether the file can be
                    // written is to call creat and then close the file.
                    // This has the side benefit of zeroing out the file if it exists.
                    int tempFileDesc;
                    if ((tempFileDesc = creat(saveFileName,0644)) == -1) {
                        // File is not writable
			StringList msg;
                        msg = readFullName();
                        msg += ": No write permission on file: ";
                        msg += saveFileName;
                        msg += ". Data not saved.";
                        errorHandler.error (msg);
                    } else {
                        // File is OK.  Close it, then write to it.
                        close(tempFileDesc);
                        for (i = input.numberPorts(); i>0; i--) {
                            cmd += "; /bin/cat ";
                            cmd += fileName[i];
                            cmd += " >> ";
                            cmd += saveFileName;
                            cmd += "; /bin/echo \"\" >> ";
                            cmd += saveFileName;
                        }
                    }
                }
    
                // remove temporary files
                for (i = input.numberPorts(); i>0; i--) {
                    cmd += "; /bin/rm -f ";
                    cmd += fileName[i];
                }
                cmd += ")";
                cmd += "&";
                system (cmd);
                // no longer need to clean up
                delFile = FALSE;
	}
    }
}
