static const char file_id[] = "CGUtilities.cc";
/******************************************************************
Version identification:
@(#)CGDisplay.cc	1.3	7/14/92

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino, J. Buck

 Misc CG routines.
 
*******************************************************************/
#include "CGUtilities.h"
#include "Error.h"
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

int rshSystem(const char* hostname,const char* command) {
	char* host;
	char* pthost;
	StringList rshCommand ="";
	char ptolemyHost[MAXHOSTNAMELEN];	//defined in <sys/param.h>
	gethostname(ptolemyHost,MAXHOSTNAMELEN);
	host = makeLower(hostname);
	pthost = makeLower(ptolemyHost);
	if(strncmp(host,pthost,strlen(host)) && strcmp(host,"localhost")) {
		rshCommand = "rsh ";
		rshCommand += hostname;
		rshCommand += " ";
	}
	rshCommand += command;
	LOG_DEL; delete host;
	LOG_DEL; delete pthost;
	return system(rshCommand);
}
