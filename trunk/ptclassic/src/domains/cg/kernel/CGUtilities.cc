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

 Programmer: Jose L. Pino, J. Buck

 Misc CG routines.
 
*******************************************************************/
#include "pt_fstream.h"
#include "CGUtilities.h"
#include "StringList.h"
#include "miscFuncs.h"
#include "Error.h"
#include <sys/param.h>
#include <ctype.h>
#include <string.h>

static const char defaultDisplay[] = "xedit -name ptolemy_code %s";

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
	newCmd << "cd " << (onHostMachine(hname)?expandPathName(dir):dir)
	       <<";"<< rshCommand;
	rshCommand = newCmd;
    }
    if (onHostMachine(hname)) {
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
    cout << "rshCommand: " << rshCommand << "\n";
    cout.flush();
    return system(rshCommand);
}

int rcpWriteFile(const char* hname, const char* dir, const char* file,
                 const char* text, int displayFlag, int mode) {
    StringList mkdir, directory, fileName;
    char* tmpFile = 0;

// if it on a local machine, do a expandPathName; if not, rely on
// the remote shell to expand the variables correctly

    if ( !onHostMachine(hname)) {
	LOG_NEW; tmpFile = tempFileName();
	directory << dir;
    }
    else {
	directory << expandPathName(dir);
    }

// create the directory if necessary (-p)
    fileName << directory << "/" << file;
    mkdir << "mkdir -p " << directory;
    if (rshSystem(hname,mkdir)) return FALSE;

    cout << "rcpWriteFile: writing file " << file << "\n";
    cout.flush();

//  write file to local machine 
    pt_ofstream o(tmpFile ? tmpFile : fileName);
    if (o) {
	if (text != NULL) o << text;  // if text is null create empty file
	o.flush();
    }
    else {
	if (tmpFile) {LOG_DEL; delete [] tmpFile;}
	return FALSE;
    }

//  chmod to appropriate mode.  Since we use the -p flag on rcp
//  the mode settings will be copied as well if we are writing to
//  a external host
    if (mode != -1) {
	chmod((const char*)(tmpFile?tmpFile:fileName),mode);
    }

//  display the file on local machine
    if (displayFlag) {
	const char* disp = getenv ("PT_DISPLAY");
	char cmdbuf[256];
	sprintf(cmdbuf,(disp?disp:defaultDisplay),
	       (tmpFile?tmpFile:fileName) );
	StringList displayCommand;
	if (tmpFile) displayCommand << "(";
	displayCommand << cmdbuf;
	if (tmpFile) displayCommand << "; rm " << tmpFile << ")";
	displayCommand << "&";
	system(displayCommand);
    }

//  rcp the file to another machine if necessary
    int status = TRUE;
    if (tmpFile) {
	StringList rcp;
	rcp <<"rcp -p "<<tmpFile<<" "<<hname<<":"<<fileName;
	if(!system(rcp)) status = FALSE;
	unlink(tmpFile);
 	LOG_DEL; delete [] tmpFile;
   }

    return status;
}

int onHostMachine(const char* hname) {
	if (hname==NULL||*hname=='\0'||(strcmp(hname,"localhost")==0))
		return TRUE;
	FILE* fp = popen("/bin/hostname", "r");
	if (fp == NULL) {
		Error::warn("popen error");
	} else {
		char line[40];
		if (fgets(line, 40, fp) != NULL) {
			char* myHost = makeLower(line);
			char* temp = makeLower((const char*) hname);
			if (strncmp(myHost, temp, strlen(temp)) == 0) {
				return TRUE;
		   	}
			LOG_DEL; delete temp;
			LOG_DEL; delete myHost;
		}
	}
	pclose(fp);
	return FALSE;
}


