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

#include "Display.h"
#include "miscFuncs.h"
#include "Output.h"
#include "StringList.h"

// constructor initializes streams and filenames
XGraph :: XGraph () {
	for (int i = 0; i < MAX_NO_GRAPHS; i++) {
	    strm[i] = NULL;
	    tmpFileNames[i] = NULL;
	}
}

// destructor removes any files that might be open
XGraph :: ~XGraph () {
	for (int i = 0; i<ng; i++) {
		if (strm[i]) fclose (strm[i]);
		delete tmpFileNames[i];
	}
}

void XGraph :: initialize(int noGraphs,
			  const char* options,
			  const char* title,
			  const char* saveFile)
{
	StringList msg;

	opt = options;
	ttl = title;
	sf = saveFile;
	ng = noGraphs;

	index = 0;

	if(ng > MAX_NO_GRAPHS) {
		Error::abortRun ("XGraph class: Too many xgraph inputs");
		return;
	}

	for (int i = 0; i<ng; i++) {
	    tmpFileNames[i] = tempFileName();
	    // open and make sure the file is writable
	    if ((strm[i] = fopen (tmpFileNames[i], "w")) == NULL) {
		msg += "XGraph class: Can't open temporary file for writing:";
		msg += tmpFileNames[i];
		Error::abortRun (msg);
	    }
	}
}


void XGraph :: addPoint(float y) {
	if ( strm[0] )
	    fprintf(strm[0], "%d %g\n", index, y);
	index++;
}

void XGraph :: addPoint(float x, float y) {
	if ( strm[0] )
	    fprintf(strm[0], "%g %g\n", x, y);
}


void XGraph :: addPoint (int dataSet, float x, float y) {
	// Do nothing if the file is not open
	if ( strm[dataSet-1] )
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
            cmd += "-t '";
            cmd += ttl;
            cmd += "' ";
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
		StringList msg;
                msg += "XGraph object: No write permission on file: ";
                msg += saveFileName;
                msg += ". Data not saved.";
                errorHandler.error (msg);
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

        // remove temporary files
        for (i = 0; i<ng; i++) {
            cmd += "; /bin/rm -f ";
            cmd += tmpFileNames[i];
        }
        cmd += ")";
        cmd += "&";
        system (cmd);
}
