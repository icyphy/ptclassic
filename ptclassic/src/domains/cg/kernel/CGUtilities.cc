static const char file_id[] = "CGUtilities.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino, J. Buck

 Misc CG routines.
 
*******************************************************************/
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

int rshSystem(const char* hname,const char* cmd,
	      const char* dir) {
	char* hostname = savestring(hname);
	char* command = savestring(cmd);
	char* directory = NULL;
	if (dir != NULL) directory = savestring(dir);
	StringList rshCommand = "";
	if (strcmp(hostname,"localhost") != 0 ) 
	{
		rshCommand = "xon ";
		rshCommand += hostname;
		rshCommand += " -debug 'export DISPLAY; ";
	}
	if (directory != NULL) {
		rshCommand += "cd ";
		rshCommand += directory;
		rshCommand += "; ";
	}
	rshCommand += command;
	if (strcmp(hostname,"localhost") != 0 ) rshCommand += "'";
	LOG_DEL; delete hostname;
	LOG_DEL; delete command;
	LOG_DEL; delete directory;
	return system(rshCommand);
}
