/*******************************************************************
SCCS version identification:
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

 Programmer: Kennard White

    tclMain.c

    Main loop for tty-based tcl program.
    Based on tcl 6.3 "tclTest.c".

********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <stdio.h>
#include <errno.h>
#include <sys/param.h>
#include <tcl.h>

#include "topStd.h"
#include "topTcl.h"


/**
    Searches a directory path list {dir_path} for {file}.
    (In {dir_path}, dirs are seperated by colons: ``:'').  It will return
    a pointer to a statically allocated string which is a valid
    directory that contains {file}.  If {file} is NULL, the first
    directory in {dir_path} that exists will be returned.
**/
char*
topTclFindDirViaPath( Tcl_Interp *ip, char *dir_path, char *file) {
    char	path[MAXPATHLEN];
    static char	buf[MAXPATHLEN];
    char	*p, *pend, *dirend = (char *) NULL;
    Tcl_DString	tmpbuf;

    strcpy( path, dir_path);
    for ( p=path; *p != '\0'; p = pend) {
	if ( (pend = strchr( p, ':')) == NULL ) {
	    pend = p + strlen(p);
	} else {
	    *pend++ = '\0';
	}
	if ( (p = Tcl_TildeSubst( ip, p, &tmpbuf)) == NULL )
	    continue;
	strcpy( buf, p);
	Tcl_DStringFree(&tmpbuf);
	if ( file != NULL ) {
	    dirend = buf + strlen(buf);
	    dirend[0] = '/';
	    strcpy(dirend+1,file);
	}
	if ( access(buf,R_OK) == 0 ) {
	    if ( file != NULL )
		dirend[0] = '\0';
	    return buf;
	}
    }
    return NULL;
}

char*
topTclGetAppLibDir( Tcl_Interp *ip, char *appname, char *defpath,
  char *initfile) {
    int		i;
    char	appUpper[MAXPATHLEN];
    char	appLower[MAXPATHLEN];
    char	buf1[MAXPATHLEN];
    char	initfile_buf[MAXPATHLEN];
    char	*p, *dir;

    for ( i=0; appname[i] != '\0'; i++) {
	appUpper[i] = TOP_ToUpper(appname[i]);
	appLower[i] = TOP_ToLower(appname[i]);
    }
    appUpper[i] = appLower[i] = '\0';

    if ( initfile == NULL ) {
	initfile = initfile_buf;
        sprintf( initfile, "%s.tcl", appLower);
    }

    sprintf( buf1, "%s_library", appname);
    if ( (dir=Tcl_GetVar( ip, buf1, TCL_GLOBAL_ONLY)) != NULL )
	return dir;
    sprintf( buf1, "%s_library", appUpper);
    if ( (dir=Tcl_GetVar( ip, buf1, TCL_GLOBAL_ONLY)) != NULL )
	return dir;
    sprintf( buf1, "%s_library", appLower);
    if ( (dir=Tcl_GetVar( ip, buf1, TCL_GLOBAL_ONLY)) != NULL )
	return dir;

    sprintf( buf1, "%s_LIBRARY", appname);
    if ( (dir=getenv(buf1)) != NULL )
	return dir;
    sprintf( buf1, "%s_LIBRARY", appUpper);
    if ( (dir=getenv(buf1)) != NULL )
	return dir;
    sprintf( buf1, "%s_LIBRARY", appLower);
    if ( (dir=getenv(buf1)) != NULL )
	return dir;

    sprintf( buf1, "%s_LIBPATH", appname);
    if ( (p=getenv(buf1)) != NULL 
      && (dir=topTclFindDirViaPath( ip, p, initfile)) != NULL )
	return dir;
    sprintf( buf1, "%s_LIBPATH", appUpper);
    if ( (p=getenv(buf1)) != NULL 
      && (dir=topTclFindDirViaPath( ip, p, initfile)) != NULL )
	return dir;
    sprintf( buf1, "%s_LIBPATH", appLower);
    if ( (p=getenv(buf1)) != NULL 
      && (dir=topTclFindDirViaPath( ip, p, initfile)) != NULL )
	return dir;

    if ( defpath != NULL
      && (dir=topTclFindDirViaPath( ip, defpath, initfile)) != NULL )
	return dir;

    if ( access( initfile, R_OK) == 0 )
	return ".";

    if ( (dir=Tcl_GetVar( ip, "tk_library", TCL_GLOBAL_ONLY)) != NULL ) {
	sprintf( buf1, "%s/%s", dir, initfile);
	if ( access( buf1, R_OK)==0 )
	    return dir;
    }
    if ( (dir=Tcl_GetVar( ip, "tcl_library", TCL_GLOBAL_ONLY)) != NULL ) {
	sprintf( buf1, "%s/%s", dir, initfile);
	if ( access( buf1, R_OK)==0 )
	    return dir;
    }
    return NULL;
}

char*
topTclSetAppLib( Tcl_Interp *ip, char *appname, char *defpath) {
    char	applibvar[100];
    char	*dir;
    char	*dpath = defpath ? defpath : ".:lib";

    if ( (dir = topTclGetAppLibDir( ip, appname, dpath, NULL)) == NULL )
	return NULL;
    sprintf( applibvar, "%s_library", appname);
    Tcl_SetVar( ip, applibvar, dir, TCL_GLOBAL_ONLY);
    return Tcl_SetVar( ip, "app_library", dir, TCL_GLOBAL_ONLY);
}


char*
topTclGetLib( Tcl_Interp *ip) {
    char	*dir;

    if ( (dir=topTclGetAppLibDir( ip, "tcl", NULL, "init.tcl")) != NULL )
	return dir;
    if ( Tcl_Eval( ip, "info library") != TCL_OK )
	return NULL;
    return ip->result;
}

char*
topTclSetLib( Tcl_Interp *ip) {
    char	*dir;

    if ( (dir = topTclGetLib( ip)) == NULL )
	return NULL;
    dir = Tcl_SetVar( ip, "tcl_library", dir, TCL_GLOBAL_ONLY);
    Tcl_ResetResult(ip);
    return dir;
}


static char *initCmd1 =
    "if [file exists $tcl_library/init.tcl] {source $tcl_library/init.tcl}";

static char *initCmd2 =
    "if [file exists $tk_library/tk.tcl] {source $tk_library/tk.tcl}";

int
topTclSourceInitFile( Tcl_Interp *ip, char *appname, char *defpath) {
    char	buf[MAXPATHLEN], *applib;

    if ( topTclSetLib( ip) == NULL ) {
	Tcl_SetResult(ip,"Can't find tcl library directory", TCL_STATIC);
	return TCL_ERROR;
    }

    if ( appname != NULL ) {
	if ( (applib = topTclSetAppLib( ip, appname, defpath)) == NULL ) {
	    Tcl_SetResult(ip,"Can't find application library directory",
	      TCL_STATIC);
	    return TCL_ERROR;
	}
        sprintf( buf, "%s/%s.tcl", applib, appname);
	if ( Tcl_EvalFile( ip, buf) != TCL_OK ) {
	    Tcl_AddErrorInfo( ip, "\nApplication initialization file failed");
	    return TCL_ERROR;
	}
    } else {
	strcpy( buf, initCmd1);	/* writable strings */
        if ( Tcl_Eval( ip, buf) != TCL_OK ) {
	    Tcl_AddErrorInfo( ip, "\nStandard TCL initialization file failed");
	    return TCL_ERROR;
	}
	if ( Tcl_GetVar( ip, "tk_library", TCL_GLOBAL_ONLY) != NULL ) {
	    strcpy( buf, initCmd2);	/* writable strings */
	    if ( Tcl_Eval( ip, buf) != TCL_OK ) {
	        Tcl_AddErrorInfo( ip, "\nStandard TK initialization file failed");
	        return TCL_ERROR;
	    }
	}
    }
    return TCL_OK;
}

void
topTclInitVars(Tcl_Interp *ip, char *fileName, int argc, char **argv) {
    char	*args;
    char	buffer[50];

    args = Tcl_Merge(argc-1, argv+1);
    Tcl_SetVar(ip, "argv", args, TCL_GLOBAL_ONLY);
    ckfree(args);
    sprintf(buffer, "%d", argc-1);
    Tcl_SetVar(ip, "argc", buffer, TCL_GLOBAL_ONLY);
    Tcl_SetVar(ip, "argv0", fileName ? fileName : argv[0], TCL_GLOBAL_ONLY);

    Tcl_SetVar(ip, "tcl_interactive",
      (fileName==NULL) && isatty(0) ? "1" : "0", TCL_GLOBAL_ONLY);
}

void
topTclPrompt0(Tcl_Interp *ip) {
    char	*promptCmd;
    int		code;

    promptCmd = Tcl_GetVar(ip, "tcl_prompt0", TCL_GLOBAL_ONLY);
    if ( promptCmd == NULL ) {
	defaultPrompt:
        fputs("Welcome to the TCL interpreter.  Type ``exit'' to leave.\n",
	  stdout);
    } else {
	code = Tcl_Eval(ip, promptCmd);
	if (code != TCL_OK) {
	    Tcl_AddErrorInfo(ip, "\n    (script that generates prompt0)");
	    fprintf(stderr, "%s\n", ip->result);
	    goto defaultPrompt;
	}
    }
}

void
topTclPrompt(Tcl_Interp *ip, TOPLogical partialB, char *defPrompt) {
    char	*promptCmd;
    int		code;

    promptCmd = Tcl_GetVar(ip,
	partialB ? "tcl_prompt2" : "tcl_prompt1", TCL_GLOBAL_ONLY);
    if (promptCmd == NULL) {
	defaultPrompt:
	fprintf( stdout, "%s%c ", defPrompt ? defPrompt : "",
	  partialB ? '?' : '>'); 
    } else {
	code = Tcl_Eval(ip, promptCmd);
	if (code != TCL_OK) {
	    Tcl_AddErrorInfo(ip, "\n    (script that generates prompt)");
	    fprintf(stderr, "%s\n", ip->result);
	    goto defaultPrompt;
	}
    }
    fflush(stdout);
}

int
topTclMainLoop( Tcl_Interp *ip_in, char *prompt, char *appname, int initB) {
#define BUFFER_SIZE 1000
    char	line[BUFFER_SIZE+1], *cmd, *defPrompt;
    Tcl_DString	cmdbuf;
    Tcl_Interp	*ip;
    int		result, gotPartial, ttyB;

    ip = ip_in ? ip_in : Tcl_CreateInterp();

    defPrompt = prompt ? prompt : (appname ? appname : "tcl");

    if ( initB ) {
	if ( (result=topTclSourceInitFile( ip, appname, NULL)) != TCL_OK ) {
	    fprintf( stderr, "%s: %s\n", appname ? appname : "???", 
	      topTclGetRetMsg( ip, result));
	    return TCL_ERROR;
	}
    }

    if ( (ttyB = isatty(0)) )
	topTclPrompt0( ip);

    Tcl_DStringInit(&cmdbuf);
    gotPartial = 0;
    while (1) {
	clearerr(stdin);
	if ( ttyB )
	    topTclPrompt(ip, gotPartial, defPrompt);
	if (fgets(line, sizeof(line)-10, stdin) == NULL) {
	    if (ferror(stdin)) {
		if (errno == EINTR) {
		     if (Tcl_AsyncReady()) {
			 (void) Tcl_AsyncInvoke(NULL, 0);
		     }
		     clearerr(stdin);
		} else {
		    break;
		}
	    } else {
	        if (!gotPartial)
		    break;	/* assume EOF */
	    }
	    line[0] = 0;
	}
	cmd = Tcl_DStringAppend(&cmdbuf, line, -1);
	if ( line[0]!='\0' && !Tcl_CommandComplete(cmd) ) {
	    gotPartial = 1;
	    continue;
	}

	gotPartial = 0;
	result = Tcl_RecordAndEval(ip, cmd, 0);
	Tcl_DStringFree(&cmdbuf);
	if ( ttyB ) {
	    printf( "%s\n", topTclGetRetMsg( ip, result));
	} else if ( result != TCL_OK ) {
	    fprintf( stderr, "%s: %s\n", appname ? appname : "???",
	      topTclGetRetMsg( ip, result));
	}
    }
    if ( ip_in == NULL )	Tcl_DeleteInterp(ip);
    return TCL_OK;
}
