static const char file_id[] = "Display.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck and E. A. Lee
 Date of creation: 

These are methods used by classes defined in Display.h that
are too large to efficiently inline.

The XGraph class wants to talk to a modified version of the xgraph
program that accepts input in binary form (indicated by the -binary
flag).  This is much faster than the ASCII form used by standard xgraph.

The following commands may appear in binxgraph input

d x y

Here d is the character 'd', x is a binary x value (a float), and y is
a binary y value (a float).  This command draws from the current position
to x,y.

md x y

Like the above, only the pen moves without drawing to the point x,y.
The m and d characters are literal.

e

End trace, start a new trace (this isn't generated by the current Display.cc
but might be in the future).

The old ASCII format is used if saveFile is given.
**************************************************************************/
const int MAX_NO_GRAPHS = 64;

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
#include "streamCompat.h"

// constructor initializes streams and filenames
XGraph :: XGraph () : strm(0), tmpFileNames(0), count(0), blockIamIn(0), ng(0)
{}

// close all files, and remove any files that might be open
void XGraph :: zapFiles () {
	for (int i = 0; i<ng; i++) {
		if (strm[i]) {
			fclose (strm[i]);
		}
		char *name = tmpFileNames[i];
		if (name) {
			unlink(name);
			LOG_DEL; delete name;
		}
	}
	LOG_DEL; delete [] strm;
	LOG_DEL; delete [] tmpFileNames;
	LOG_DEL; delete [] count;
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

	// just in case initialize is called twice.
	zapFiles();

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

	// allocate memory for data structures
	LOG_NEW; strm = new FILE*[ng];
	LOG_NEW; tmpFileNames = new char*[ng];
	LOG_NEW; count = new int[ng];

	// write data in ASCII if saveFile is specified.
	ascii = (saveFile != 0 && *saveFile != 0);

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
	addPoint (0, float(index), y);
	index++;
}

void XGraph :: addPoint (int dataSet, float x, float y) {
	int didx = dataSet-1;
	count[didx]++;
	fcheck(x,dataSet);
	fcheck(y,dataSet);
	if (count[didx] >= nIgnore && strm[didx]) {
	    if (ascii) {
		fprintf(strm[didx], "%g %g\n", x, y);
	    }
	    else {
		float v[2];
		v[0] = x;
		v[1] = y;
		putc('d',strm[didx]);
		fwrite((char*)v, sizeof v[0], 2, strm[didx]);
	    }
	}
}

// start a new trace on the graph.
void XGraph :: newTrace(int dataSet) {
	index = 0;
	FILE* fd = strm[dataSet-1];
	if (!fd) return;
	if (ascii)
		fprintf(fd, "move ");
	else
		putc('m',fd);
}

void XGraph :: terminate () {
	for (int i = 0; i<ng; i++)
	   if (strm[i]) {
		fclose (strm[i]);
		strm[i] = NULL;
	   }
        StringList cmd;
    
        cmd += "( xgraph ";
	if (!ascii)
		cmd += "-binary ";

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
