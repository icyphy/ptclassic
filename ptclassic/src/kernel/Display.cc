static const char file_id[] = "Display.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 10/20/90

These are methods used by classes defined in Display.h that
are too large to efficiently inline.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Display.h"
#include "miscFuncs.h"
#include "Error.h"
#include "StringList.h"
#include "Block.h"
#include "UserOutput.h"
#include <std.h>
#include <math.h>

// constructor initializes streams and filenames
XGraph :: XGraph () {
	for (int i = 0; i < MAX_NO_GRAPHS; i++) {
	    strm[i] = NULL;
	    tmpFileNames[i] = NULL;
	}
	ng = 0;
}

// close all files, and remove any files that might be open
void XGraph :: zapFiles () {
	for (int i = 0; i<ng; i++) {
		if (strm[i]) {
			fclose (strm[i]);
			strm[i] = 0;
		}
		char *name = tmpFileNames[i];
		if (name) {
			unlink(name);
			LOG_DEL; delete name;
		}
	}
	ng = 0;
}

void XGraph :: initialize(Block* parent,
			  int noGraphs,
			  const char* options,
			  const char* title,
			  const char* saveFile,
			  int ignore)
{
	StringList msg;

	blockIamIn = parent;
	opt = options;
	ttl = title;
	nIgnore = ignore;
	sf = saveFile;
	ng = noGraphs;

	index = 0;

	if(ng > MAX_NO_GRAPHS) {
		Error::abortRun (*blockIamIn, "Too many xgraph inputs");
		return;
	}

	for (int i = 0; i<ng; i++) {
	    tmpFileNames[i] = tempFileName();
	    count[i] = 0;
	    // open and make sure the file is writable
	    if ((strm[i] = fopen (tmpFileNames[i], "w")) == NULL) {
		msg += "Can't open temporary file for writing:";
		msg += tmpFileNames[i];
		Error::abortRun (*blockIamIn, msg);
	    }
	}
}

// this check traps IEEE infinities and NaN values
// the ifdef condition should really be: implement this function if the
// processor uses IEEE arithmetic.
#ifdef mips
#include <nan.h>
#endif
#ifdef sun
#define IsNANorINF(X) (isnan(X) || isinf(X))
#endif
#ifndef IsNANorINF
#define IsNANorINF(X) 0
#endif

// function to print "1st, 2nd, 23rd", etc.  Return value is in a static
// buffer and is wiped out by subsequent calls.
static char* ordinal(int n) {
	int ldig = n%10;
	const char* format;
	switch (ldig) {
	case 1:
		format = "%dst";
		break;
	case 2:
		format = "%dnd";
		break;
	case 3:
		format = "%drd";
		break;
	default:
		format = "%dth";
	}
	static char buf[10];
	sprintf (buf, format, n);
	return buf;
}

void XGraph :: fcheck(double y, int set) {
	if (IsNANorINF(y)) {
		char buf[128];
		sprintf (buf, "in the %s value on input stream %d",
			 ordinal(count[set-1]), set);
		Error::abortRun(*blockIamIn, "Overflow or divide by zero\n",
				buf);
		zapFiles();
	}
}

void XGraph :: addPoint(float y) {
	count[0]++;
	fcheck (y);
	if (count[0] >= nIgnore && strm[0])
		fprintf(strm[0], "%d %g\n", index, y);
	index++;
}

void XGraph :: addPoint (int dataSet, float x, float y) {
	count[dataSet-1]++;
	fcheck(x,dataSet);
	fcheck(y,dataSet);
	if (count[dataSet-1] >= nIgnore	&& strm[dataSet-1])
		fprintf(strm[dataSet-1], "%g %g\n", x, y);
}

// start a new trace on the graph.
void XGraph :: newTrace(int dataSet) {
	if (strm[dataSet-1])
	    fprintf(strm[dataSet-1], "move ");
	index = 0;
}

void XGraph :: terminate () {
	for (int i = 0; i<ng; i++)
	   if (strm[i]) {
		fclose (strm[i]);
		strm[i] = NULL;
	   }
        StringList cmd;
    
        cmd += "( xgraph ";

	// put title on command line
        if (ttl && *ttl) {
		if (strchr(ttl,'\'')) {
			cmd += "-t \""; cmd += ttl; cmd += "\" ";
		}
		else {
			cmd += "-t '"; cmd += ttl; cmd += "' ";
		}
        }

	// put options on the command line
        if (opt && *opt) {
            cmd += opt;
            cmd += " ";
        }

	// put filenames on the command line
        for (i = 0; i<ng; i++) {
            cmd += tmpFileNames[i];
            cmd += " ";
        }

        if (sf != NULL && *sf != 0) {
            const char* saveFileName = savestring (expandPathName(sf));

            // Easiest way to check to see whether the file can be
            // written is to call creat and then close the file.
            // This has the side benefit of zeroing out the file if it exists.
            int tempFileDesc;
            if ((tempFileDesc = creat(saveFileName,0644)) == -1) {
                // File is not writable
		    Error::warn (*blockIamIn, "No write permission on file: ",
				 saveFileName, ". Data not saved.");
            } else {
                // File is OK.  Close it, then write to it.
                close(tempFileDesc);
                for (i = 0; i<ng; i++) {
                    cmd += "; /bin/cat ";
                    cmd += tmpFileNames[i];
                    cmd += " >> ";
                    cmd += saveFileName;
                    cmd += "; /bin/echo \"\" >> ";
                    cmd += saveFileName;
                }
                    }
        }

        // issue commands to remove temporary files
        for (i = 0; i<ng; i++) {
		char* name = tmpFileNames[i];
		cmd += "; /bin/rm -f ";
		cmd += name;
		// remove the filenames so we won't zap them later.
		LOG_DEL; delete name;
		tmpFileNames[i] = 0;
        }
	ng = 0;
        cmd += ")";
        cmd += "&";
        system (cmd);
}
