/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: Jose Luis Pino, J. Buck

 Misc CG routines.
 
*******************************************************************/
static const char file_id[] = "CGUtilities.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>		// Pick up decl for pclose().
#include <unistd.h>		// Pick up F_OK for access() under sun4 cfront.
#include "pt_fstream.h"
#include "CGUtilities.h"
#include "StringList.h"
#include "miscFuncs.h"
#include "Error.h"
#include <sys/param.h>
#include <ctype.h>
#include <string.h>
#include "compat.h"

#if defined(hpux) || defined(SYSV) || defined(SVR4) || defined(__svr4__) || defined(PTIRIX5_CFRONT) 
#include <sys/stat.h>		// Pick up chmod declaration.
#endif /* hpux SYSV SVR4 */

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
	newCmd << "cd ";
	if ( onHostMachine(hname) ) {
	  const char *expandedName = expandPathName(dir);
	  newCmd << expandedName;
	  delete [] expandedName;
	}
	else {
	  newCmd << dir;
	}
	newCmd << ";" << rshCommand;
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
	if ( strchr(cmdtext,'\'') ) {
	    // cmd has quotes.  put it in file and send through pipe.
	    char *cmdfilename = tempnam( NULL, "pt");
	    if ( cmdfilename == 0 ) {
	      Error::abortRun("rshSystem: the tempnam function",
			      "cannot allocate memory");
	      return FALSE;
	    }
	    pt_ofstream cmdfile(cmdfilename);
	    cmdfile << rshCommand;
	    cmdfile.close();
	    preCmd << "/bin/cat " << cmdfilename << " | ";
	    postCmd << " ; /bin/rm -f " << cmdfilename;
	    free(cmdfilename);
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
	const char *expandedName = expandPathName(dir);
	directory << expandedName;
	delete [] expandedName;
    }

    fileName << directory << "/" << file;

// create the directory if necessary (-p)
    if (onHostMachine(hname)) {
	if (access(directory, W_OK) == -1) {
	    // -p flag is necessary because it might be a nested directory
	    // where one of the parent directories does not exist
	    mkdir << "mkdir -p " << directory;
	    if (system(mkdir)) return FALSE;
	}
    }
    else {
	mkdir << "mkdir -p " << directory;
	if (rshSystem(hname,mkdir)) return FALSE;
    }
	    
    cout << "rcpWriteFile: writing file " << file << "\n";
    cout.flush();

//  write file to local machine 
    // cfront1.0 barfs because there is not StringList ? operand, so
    // don't use it.  However, this is a poor solution.
    if (tmpFile) {
      pt_ofstream o(tmpFile);
      if (o) {
	if (text != NULL) o << text;  // if text is null create empty file
	o.flush();
      }
      else {
	if (tmpFile) {LOG_DEL; delete [] tmpFile;}
		return FALSE;
      }

    } else {
      pt_ofstream o(fileName.chars());
      if (o) {
	if (text != NULL) o << text;  // if text is null create empty file
	o.flush();
      }
      else {
	if (tmpFile) {LOG_DEL; delete [] tmpFile;}
	return FALSE;
    }

    }

//  chmod to appropriate mode.  Since we use the -p flag on rcp
//  the mode settings will be copied as well if we are writing to
//  a external host
    if (mode != -1) {
        if(tmpFile)
	  chmod((const char*)tmpFile,mode);
	else
	  chmod((const char*)fileName,mode);
    }

//  rcp the file to another machine if necessary
    int status = TRUE;
    if (tmpFile) {
        StringList rcp;
        rcp <<"rcp -p "<<tmpFile<<" "<<hname<<":"<<fileName;
        if(system(rcp)) status = FALSE;
	// if we display the file, we must delete the tmpfile after
	// it is displayed, otherwise we delete it here
        if (!displayFlag) unlink(tmpFile);
   }

//  display the file on local machine
    if (displayFlag) {
	const char* disp = getenv ("PT_DISPLAY");
	char cmdbuf[256];
	if(tmpFile)
	  sprintf(cmdbuf,(disp?disp:defaultDisplay), tmpFile);
	else 
	  sprintf(cmdbuf,(disp?disp:defaultDisplay), fileName.chars());
	StringList displayCommand;
	if (tmpFile) displayCommand << "(";
	displayCommand << cmdbuf;
	if (tmpFile) displayCommand << "; rm " << tmpFile << ")";
	displayCommand << "&";
	system(displayCommand);
    }

    LOG_DEL; delete [] tmpFile;
    return status;
}

int rcpCopyFile(const char* hname, const char* dir, const char* filePath,
		int deleteOld, const char* newFileName) {
    const char *expandedName = expandPathName(filePath);
    StringList expandedFilePath = expandedName;
    delete [] expandedName;
    if (access(expandedFilePath,R_OK) == -1) {
	Error::abortRun("rcpCopyFile: ", filePath, " does not exist");
	return FALSE;
    }

    StringList fileName;
    if (newFileName) {
	fileName << newFileName;
    }
    else {
	const char* fileNameStart = strrchr(filePath,'/');
	if (fileNameStart)
	    fileName << ++fileNameStart;
	else
	    fileName << expandedFilePath;
    }

    const char *expandedDirName = expandPathName(dir);
    StringList directory = expandedDirName;
    delete [] expandedDirName;
    StringList command;
    StringList rmOldFile;
    if (deleteOld) {
	rmOldFile << "/bin/rm -f " << directory << "/" << fileName << "; ";
    }
    if (onHostMachine(hname)) {
	if (access(directory, W_OK) == -1)
	    command << "mkdir " << directory << "; ";
	command << rmOldFile << "cp " << expandedFilePath << " " 
		<< directory << "/" << fileName;
    }
    else {
	command << "mkdir -p " << directory << "; " << rmOldFile;
	rshSystem(hname,command);
	command.initialize();
	command << "rcp -p " << expandedFilePath << " " << hname << ":" 
		<< directory << "/" << fileName;
    }
    return !rshSystem("localhost",(const char*)command);
}
    
int onHostMachine(const char* hname) {
	if (hname==NULL||*hname=='\0'||(strcmp(hname,"localhost")==0))
		return TRUE;
	FILE* fp = popen("hostname", "r");
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
			LOG_DEL; delete [] temp;
			LOG_DEL; delete [] myHost;
		}
	}
	pclose(fp);
	return FALSE;
}


// This function sanitizes a string so that it is usable as a
// C/C++ identifier.  If the string begins with a digit, the
// character 'x' is prepended.  Then, all the characters in
// the string that are not alphanumeric are changed to '_'.
// The returned (const char*) pointer points to the resulting
// string in an internal buffer maintained by this function,
// and is only valid until the next invocation of this function.
const char* ptSanitize(const char* string)
{
    // This pointer points to the dynamically allocated buffer that
    // holds the result string.  The pointer is static so that we can
    // remember the buffer allocated in the previous invocation of
    // this function.
    static char *sanitizedString = 0;

    // Delete the buffer allocated in the previous invocation.
    if (sanitizedString) { LOG_DEL; delete sanitizedString; }

    // Allocate a new buffer for this invocation.
    LOG_NEW; sanitizedString = new char [strlen(string) + 2];

    char *cPtr = sanitizedString;

    // Check for leading digit.
    if (isdigit(*string)) *(cPtr++) = 'x';

    // Replace non-alphanumeric characters.
    while (*string != 0) {
	if (isalnum(*string))
	    *(cPtr++) = *string;
        else
	    *(cPtr++) = '_';
	string++;
    }
    *cPtr = 0;

    // Note that the result string is kept until the next invocation.
    return sanitizedString;
}
