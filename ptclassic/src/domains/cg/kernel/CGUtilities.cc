/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer: Jose Luis Pino, J. Buck

 Misc CG routines.
 
*******************************************************************/
static const char file_id[] = "CGUtilities.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>		// declare pclose
#include <unistd.h>		// declare access
#include "pt_fstream.h"
#include "type.h"
#include "CGUtilities.h"
#include "StringList.h"
#include "miscFuncs.h"
#include "Error.h"
#include <sys/param.h>
#include <ctype.h>		// declare isspace, isdigit, isalnum, isupper
#include <string.h>		// declare strlen and strcasecmp
#include "compat.h"
#include <sys/stat.h>		// declare stat structure and chmod function

extern "C" {
#include "displayFile.h"	// declare displayFile()
}

// Maximum number of characters in the name of a host machine
#define MAX_HOSTNAME_LEN 128

// Make a writeable local or remote directory.  Returns 0 on success
// and -1 on failure.  If a writeable directory already exists, it
// returns 0.
int makeWriteableDirectory(const char* hname, const char* directory) {
    // For the mkdir Unix command, the -p flag is necessary to handle a
    // nested directory where one of the parent directories does not exist
    // The -p option works on the machines that Ptolemy runs on.
    StringList command = "mkdir -p ";
    int retval = 0;
    command << directory;

    if (onHostMachine(hname)) {
	// Create a new directory if a writeable directory of the same
	// name does not exist.  We do not use the access command here
	// because the access command could only detect if a file exists
	// and is writeable.  It does not check to see if it is a directory.
	struct stat stbuf;
        if (stat(directory, &stbuf) == -1) {
            // Directory does not exist.  Attempt to create it.
	    retval = rshSystem(hname, command);
	}
	else {
            // Something by that name exists, see whether it's a directory
            if ((stbuf.st_mode & S_IFMT) != S_IFDIR) {
                retval = -1;
            }
	}
    }
    else {
	retval = rshSystem(hname, command);
    }
    return retval;
}

// Copy one or more files to a directory on the same local network
static int copyNetworkedFiles(const char* hname, const char* source,
			      const char* directory, const char* fileName,
			      const char* prolog, const char* epilog) {
    StringList command;
    if (prolog) command << prolog << "; ";
    if (onHostMachine(hname)) {
	command << "cp " << source << " " << directory;
    }
    else {
	// From the man page for rcp: the -p option causes rcp to attempt
	// to preserve (duplicate) in its copies the modification times and
	// modes of the source files, ignoring the umask.
	command << "rcp -p " << source << " " << hname << ":" << directory;
    }
    if (fileName) command << "/" << fileName;
    if (epilog) command << "; " << epilog;
    return (rshSystem("localhost", (const char*)command) == 0);
}

// Converts a string to lower case
char* makeLower(const char* name) {
    LOG_NEW; char* newp = new char[strlen(name) + 1];
    char *o = newp;
    while (*name) {
	char c = *name++;
	if (isupper(c)) *o++ = tolower(c);
	else *o++ = c;
    }
    *o = 0;
    return newp;
}

// Run command on hostname in specified directory.  If directory == 0
// then the command will be executed in the home directory.  Returns the
// status flag of the system call: 0 means success and -1 means error.
int rshSystem(const char* hname, const char* cmd, const char* dir) {
    StringList rshCommand = cmd;
    if ( dir != NULL ) {
	StringList newCmd;
	newCmd << "cd ";
	if ( onHostMachine(hname) ) {
	  char *expandedName = expandPathName(dir);
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
	if ( strchr(cmdtext, '\'') ) {
	    // cmd has quotes.  put it in file and send through pipe.
	    char *cmdfilename = tempnam( NULL, "pt");
	    if ( cmdfilename == 0 ) {
	      Error::abortRun("rshSystem: the tempnam function",
			      "cannot allocate memory");
	      return -1;
	    }
	    pt_ofstream cmdfile(cmdfilename);
	    cmdfile << rshCommand;
	    cmdfile.close();
	    preCmd << "/bin/cat " << cmdfilename << " | ";
	    postCmd << " ; /bin/rm -f " << cmdfilename;
	    free(cmdfilename);
	}
	else {
	    // cmd is quoteless.  Just echo it directly into the pipe.
	    preCmd << "echo '" << rshCommand << "' | ";
	}
	// -debug required to keep stdin open
	rshCommand = "";
	// ptxon is a script based on the X11R5 xon script
	rshCommand << preCmd << "ptxon " << hname << " -debug sh" << postCmd;
    }
    cout << "rshCommand: " << rshCommand << "\n";
    cout.flush();
    return system(rshCommand);
}

// Write a string to a file in a specified directory on a given host.  If
// the directory does not exit, it will be created.  The code can be
// optionally displayed.  If host is not the localhost, then this method
// will use rcp to copy it.  If mode is not null, will execute a
// chmod on the file with the given mode.  Returns TRUE if successful.
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
	char* expandedName = expandPathName(dir);
	directory << expandedName;
	delete [] expandedName;
    }

    fileName << directory << "/" << file;

    // create the directory if necessary
    if (makeWriteableDirectory(hname, directory) == -1) return FALSE;

    cout << "rcpWriteFile: writing file " << file << "\n";
    cout.flush();

    // write file to local machine 
    const char* outputFileName = tmpFile ? tmpFile : fileName.chars();
    pt_ofstream o(outputFileName);
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
    if (mode != -1) chmod(outputFileName, mode);

    //  rcp the file to another machine if necessary
    int status = TRUE;
    if (tmpFile) {
        StringList rcp = "rcp -p ";
        rcp << tmpFile << " " << hname << ":" << fileName;
        if (system(rcp)) status = FALSE;
	// if we display the file, we must delete the tmpfile after
	// it is displayed, otherwise we delete it here
        if (!displayFlag) unlink(tmpFile);
   }

    //  display the file on local machine
    if (displayFlag) {
	if (displayFile(outputFileName,
			(void (*)(const char *)) NULL,
			(void (*)(const char *)) NULL) != TRUE )
	  status = FALSE;
	if (tmpFile) {
	  StringList displayCommand;
	  displayCommand << "; rm " << tmpFile;
	  system(displayCommand);
	}
    }

    LOG_DEL; delete [] tmpFile;
    return status;
}

// Copy a file to a directory.  This will either perform a link system
// call or do a rcp to copy a file over the network. Returns TRUE if
// successful.  The old file will not be deleted if there is an error
// in creating or accessing the directory.
int rcpCopyFile(const char* hname, const char* dir, const char* filePath,
		int deleteOld, const char* newFileName) {
    char* expandedName = expandPathName(filePath);
    StringList expandedFilePath = expandedName;
    delete [] expandedName;
    if (access(expandedFilePath, R_OK) == -1) {
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

    char* expandedDirName = expandPathName(dir);
    StringList directory = expandedDirName;
    delete [] expandedDirName;

    // Create the directory
    if (makeWriteableDirectory(hname, directory) == -1) return FALSE;

    // Conditionally delete the old file name
    StringList rmOldFile;
    if (deleteOld) {
	rmOldFile << "/bin/rm -f " << directory << "/" << fileName;
    }

    return copyNetworkedFiles(hname, expandedFilePath, directory, fileName,
			      0, rmOldFile);
}
 
// Copy multiple files given by filePathList to a directory.  The list of
// files is separated by white space.  Returns TRUE if successful.
// The old files will not be deleted if there is an error in creating
// or accessing the directory.
int rcpCopyMultipleFiles(const char* hname, const char* dir,
	const char* filePathList, int deleteOld /*=TRUE*/) {

    // Expand the environment variables in the remote directory name
    char* expandedDirName = expandPathName(dir);
    StringList directory = expandedDirName;
    delete [] expandedDirName;

    // Create the directory
    if (makeWriteableDirectory(hname, directory) == -1) return FALSE;

    // Walk through the list of files
    char* fileList = expandPathName(filePathList);
    char* lastString;
    char* curString;
    int notAtEndOfString;
    int validFiles = TRUE;
    lastString = fileList;
    curString = lastString;
    do {
        while (*curString && !isspace(*curString)) curString++;
        notAtEndOfString = *curString;
        if (lastString != curString) {
	    char oldChar = *curString; 
            *curString = 0;
	    // Check existence of the current file in lastString
	    if (access(lastString, R_OK) == -1) {
		Error::abortRun("rcpCopyMultipleFiles: ", lastString,
				" does not exist");
	        *curString = oldChar;
		validFiles = FALSE;
		break;
	    }
	    *curString = oldChar;
            if (notAtEndOfString) curString++;
            lastString = curString;
        }
    } while (notAtEndOfString);

    if (validFiles) {
	StringList rmOldFiles;
	if (deleteOld) {
	    rmOldFiles << "/bin/rm -f " << fileList;
	}
        validFiles = copyNetworkedFiles(hname, fileList, directory, 0,
					0, rmOldFiles);
    }

    delete [] fileList;
    return validFiles;
}

// Returns TRUE if hname is the machine Ptolemy is running on.  Open a pipe
// to the Unix command "hostname" and read the result.  It returns TRUE if
// hname is 0, "\0", or "localhost" as well.
int onHostMachine(const char* hname) {
	if (hname == 0 || *hname == '\0' || strcmp(hname, "localhost") == 0) {
	    return TRUE;
	}

	FILE* fp = popen("hostname", "r");
	if (fp == NULL) {
	    Error::warn("Cannot open a pipe to 'hostname'");
	    return FALSE;
	}

	int retval = FALSE;
	char line[MAX_HOSTNAME_LEN];
	if (fgets(line, MAX_HOSTNAME_LEN, fp) != NULL) {
	    line[MAX_HOSTNAME_LEN - 1] = 0;
	    retval = ( strcasecmp(line, hname) == 0 );
	}
	pclose(fp);
	return retval;
}

// Sanitize a string so that it is usable as a C/C++ identifier.  If
// the string begins with a digit, the character 'x' is prepended.
// Then, all the characters in the string that are not alphanumeric
// are changed to '_'.  The returned (const char*) pointer points to
// the resulting string in an internal buffer maintained by this
// function, and is only valid until the next invocation of this
// function.
const char* ptSanitize(const char* string) {
    // This pointer points to the dynamically allocated buffer that
    // holds the result string.  The pointer is static so that we can
    // remember the buffer allocated in the previous invocation of
    // this function.
    static char* sanitizedString = 0;

    // Allocate a new buffer for this invocation.
    delete [] sanitizedString;
    LOG_NEW; sanitizedString = new char[strlen(string) + 2];
    char* cPtr = sanitizedString;

    // Check for leading digit.
    if (isdigit(*string)) *(cPtr++) = 'x';

    // Replace non-alphanumeric characters.
    while (*string) {
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

// Find a local file name: either copy the remote file to a local temporary
// file in /tmp or return the expanded path name of the local file.  If the
// deleteFlag is set to TRUE, then delete the filename after finishing with
// it.  This deletion is automated by the cleanupLocalFileName function.
// An error is indicated if the result is an empty string list
StringList findLocalFileName(const char* hname, const char* dir,
			     const char* filename, int& deleteFlag) {
    StringList targetpath = dir;
    targetpath << "/" << filename;

    StringList pathname;
    if ( !onHostMachine(hname) ) {
	deleteFlag = TRUE;
	pathname = "/tmp/";
	pathname << filename;
	StringList copycmd = "rcp -p ";
	copycmd << hname << ":" << targetpath << " " << pathname;
	if ( !system(copycmd) ) {
	    pathname.initialize();
	    StringList errmsg = "Could not copy ";
	    errmsg << filename << " in the directory " << dir
		   << " on the remote machine " << hname;
	    Error::warn(errmsg);
	}
    }
    else {
	deleteFlag = FALSE;
	char* expandedPathName = expandPathName(targetpath);
	pathname = expandedPathName;
	delete [] expandedPathName;
    }
    return pathname;
}

// Delete the filename if deleteFlag is TRUE.
int cleanupLocalFileName(const char* pathname, int deleteFlag) {
    int retval = TRUE;
    if ( deleteFlag ) {
	StringList removeCommand = "/bin/rm -f ";
	removeCommand << pathname;
	if (! system(removeCommand)) {
	    StringList errmsg = "Could not remove ";
	    errmsg << pathname;
	    Error::warn(errmsg);
	}
    }
    return retval;
}
