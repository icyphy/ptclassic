static const char file_id[] = "CGUtilities.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer: J. Pino, J. Buck

 Misc CG routines.
 
*******************************************************************/
#include "pt_fstream.h"
#include "CGUtilities.h"
#include "Error.h"
#include "miscFuncs.h"
#include <sys/param.h>
#include <ctype.h>
#include <string.h>

//when this is moved to CGUtilies, CG56Ta
char* makeLower(const char* name) {
	LOG_NEW; char* newp = new char[strlen(name)+1];
	char *o = newp;
	while (*name) {
		char c = *name++;
		if (isupper(c)) *o++ = tolower(c);
		else *o++ = c;
	}
	*o = 0;
	return newp;
}

int rshSystem(const char* hname, const char* cmd, const char* dir) {
	StringList rshCommand = cmd;
	if ( dir != NULL ) {
	    StringList newCmd;
	    newCmd << "cd " << dir << ";" << rshCommand;
	    rshCommand = newCmd;
	}
	if (hname==NULL || *hname=='\0' || strcmp(hname,"localhost")==0 ) {
	    ; // no work required
	} else {
	    // In order for background process to work, we have to make sure
	    // all rsh pipes are closed.  But we cant close them on this side
	    // we send the command through a pipe.  Thus we have the remove
	    // side close the files.  In the future, we may want to *not* do
	    // this, or redirect it to a file, for non-background jobs.
	    // If the command has newlines, we can't safely put '()' around it.
	    // Solution is to put backquotes before newlines, but that might
	    // not be safe.

	    StringList wrapCmd;
	    wrapCmd << "( " << rshCommand << " )";
	    wrapCmd << " < /dev/null > /dev/null 2>&1";
	    rshCommand = wrapCmd;

	    StringList preCmd, postCmd;
	    const char* cmdtext = rshCommand;
	    if ( strchr(cmdtext,'\'')!=NULL ) {
		// cmd has quotes.  put it in file and send through pipe.
	        char *cmdfilename = tempnam( NULL, "pt");
	        pt_ofstream cmdfile(cmdfilename);
	        cmdfile << rshCommand;
		cmdfile.close();
		preCmd << "/bin/cat " << cmdfilename << " | ";
		postCmd << " ; /bin/rm -f " << cmdfilename;
	    } else {
		// cmd is quoteless.  Just echo it directly into the pipe.
		preCmd << "echo '" << rshCommand << "' | ";
	    }
	    // -debug required to keep stdin open
	    rshCommand = "";
	    rshCommand << preCmd << "xon " << hname << " -debug sh" << postCmd;
	}
	cerr << "rshCommand: " << rshCommand << "\n";
	return system(rshCommand);
}
