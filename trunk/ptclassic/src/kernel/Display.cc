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
}

// destructor removes any files that might be open
XGraph :: ~XGraph () {
	for (int i = 0; i<ng; i++) {
		if (strm[i]) fclose (strm[i]);
		delete tmpFileNames[i];
	}
}

void XGraph :: initialize(Block* parent,
			  int noGraphs,
			  const char* options,
			  const char* title,
			  const char* saveFile)
{
	StringList msg;

	blockIamIn = parent;
	opt = options;
	ttl = title;
	sf = saveFile;
	ng = noGraphs;

	index = 0;

	if(ng > MAX_NO_GRAPHS) {
		Error::abortRun (*blockIamIn, "Too many xgraph inputs");
		return;
	}

	for (int i = 0; i<ng; i++) {
	    tmpFileNames[i] = tempFileName();
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
inline void XGraph :: fcheck(float y) {
#ifndef vax
	if (isinf(y) || isnan(y)) Error::abortRun(*blockIamIn,
					"Numeric overflow or divide by 0 detected");
#endif
};

void XGraph :: addPoint(float y) {
	fcheck (y);
	if ( strm[0] )
	    fprintf(strm[0], "%d %g\n", index, y);
	index++;
}

void XGraph :: addPoint(float x, float y) {
	fcheck(x);
	fcheck(y);
	if ( strm[0] )
	    fprintf(strm[0], "%g %g\n", x, y);
}


void XGraph :: addPoint (int dataSet, float x, float y) {
	fcheck(x);
	fcheck(y);
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

        // remove temporary files
        for (i = 0; i<ng; i++) {
            cmd += "; /bin/rm -f ";
            cmd += tmpFileNames[i];
        }
        cmd += ")";
        cmd += "&";
        system (cmd);
}
