ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/19/90
 Revised 10/3/90 to work under the preprocessor.

 Draws a graph with the xgraph function.  It is assumed that "xgraph"
 is on your path, or this will not work!!!

 This version is for the SDF domain.
**************************************************************************/
}

defstar {
	name { Xgraph }
	domain { SDF }
	desc { "Generate a plot with the xgraph program." }
	input {
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
		FILE *strm;
		int delFile;
		const char* fileName;
		int index;
	}
	ccinclude { "miscFuncs.h" }
	constructor {
		strm = NULL;
		delFile = FALSE;
	}
	start {
		fileName = saveFile;
		index = 0;
		if (fileName == NULL || *fileName == 0) {
			fileName = tempFileName();
			delFile = TRUE;
		}
		else fileName = savestring (expandPathName(fileName));
		// should check if file already exists here
		if ((strm = fopen (fileName, "w")) == NULL) {
			StringList msg = readFullName();
			msg += "Can't open file ";
			msg += fileName;
			Error::abortRun (msg);
		}
	}
// go.  Does nothing if open failed.
	go {
		if (!strm) return;
		float data = input%0;
		fprintf (strm, "%d %g\n", index, data);
		index++;
	}
// wrapup.  Does nothing if open failed, or 2nd wrapup call.
	wrapup {
		if (!strm) return;
		fclose (strm);
		exec("");
		strm = NULL;
	}
	destructor {
		if (strm) fclose (strm);
		if (delFile) unlink (fileName);
		delete fileName;
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

			if (delFile) cmd += "( ";
			cmd += "xgraph ";

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
			cmd += fileName;
			if (delFile) {
				cmd += "; /bin/rm -f ";
				cmd += fileName;
				cmd += ")";
			}
			cmd += "&";
			system (cmd);
			// no longer need to clean up
			delFile = FALSE;
		}
	}
}
