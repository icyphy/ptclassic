#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1994 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#include "copyright.h"
#include "port.h"

/*
 * VEM Remote Procedure Call Support
 * 
 * David Harrison and Rick Spickelmier
 * University of California,  1986, 1987, 1988
 *
 * This file contains routines to support the Remote Procedure
 * Call system developed by Rick Spickelmier.  It contains
 * routines which are called by remote applications to 
 * set up command bindings,  close an application,  and open
 * new windows.
 */

#include <X11/Xlib.h>
#include "buffer.h"
#include "windows.h"
#include "vemUtil.h"
#include "message.h"
#include "list.h"
#include "commands.h"
#include "tap.h"
#include "rpcServer.h"
#include "serverNet.h"

#include "vemRPC.h"

#define MAXNAMELEN	255




vemStatus makeRemoteName(retName, host, path, num)
STR retName;			/* Returned name       */
STR host;			/* Host of application */
STR path;			/* Path to application */
int num;			/* Application number  */
/*
 * This routine constructs the unique name for a remote
 * application given its host, path, and application number.
 * The current format of the name is:
 *   "sname@host (n)"
 * where 'sname' is the stripped name, 'host' is the host
 * name,  and 'n' is the application number.
 */
{
    int index;
    char lhost[128], *ptr;

    for (index = strlen(path);  index >= 0;  index--) {
	if (path[index] == '/') {
	    break;
	}
    }
    index++;

    ptr = strchr(host, '.');
    if (ptr) {
	(void) strncpy(lhost, host, ptr - host);
        lhost[ptr - host] = '\0';
    } else {
	(void) strcpy(lhost, host);
    }
    (void) sprintf(retName, "%s@%s (%d)", &(path[index]), lhost, num);
    return VEM_OK;
}



vemStatus vemAltBindings(theWin, host, name, appNum, count, paneStrs, comStrs, keyStrs)
Window theWin;			/* Window to attach bindings */
STR host;			/* Host name of application  */
STR name;			/* Path to application       */
int appNum;			/* Application number        */
int count;			/* Number of strings         */
STR paneStrs[];		        /* pane names                */
STR comStrs[];		        /* command names             */
STR keyStrs[];		        /* key bindings              */
/*
 * This routine is used by the RPC package to set up bindings
 * for a remote application.  Initially,  all of the bindings
 * are menu bindings.  The resulting binding table is attached
 * to the alternate binding table for the window.
 */
{
    bdTable newTable;
    int index, selectNum = 0;
    char uidName[MAXNAMELEN], remoteName[MAXNAMELEN];
    char lastPane[MAXNAMELEN];

    /*
     * XXX RLS HACK
     *
     * the application number used to be prepended to each command name
     * allow multiple invocations of the same rpc application to be running
     * at the same time; unfortunately when I put in application number
     * recycling, numbers could overlap and bindings would no longer be
     * unique.....  so this is a quick hack until reference counting of
     * bindings is put into VEM
     */
    static int appCount = 0;

    appCount++;

    /* Define the command string names */
    for (index = 0;  index < count;  index++) {
	(void) sprintf(uidName, "%s-%s-%d", paneStrs[index], comStrs[index], appCount);
	if (bdRegAppFunc(uidName, comStrs[index],
			 appNum, index) != VEM_OK) {
	    vemMessage("Problems defining remote application commands.\n",
		       MSG_DISP);
	    return VEM_CORRUPT;
	}
    }
    /* Create new binding table */
    (void) makeRemoteName(remoteName, host, name, appNum);
    if (bdCreate(remoteName, &newTable) != VEM_OK) {
	vemMessage("Unable to create new remote binding table\n",
		   MSG_DISP);
	return VEM_CORRUPT;
    }
    /* Make bindings to the menu */
    lastPane[0] = '\0';
    for (index = 0;  index < count;  index++) {
	if (strcmp(paneStrs[index], lastPane) != 0) {
	    selectNum = 0;
	    (void) strcpy(lastPane, paneStrs[index]);
	}
	(void) sprintf(uidName, "%s-%s-%d", paneStrs[index], comStrs[index], appCount);
	if (bdBindMenu(newTable, uidName, paneStrs[index], selectNum)!=VEM_OK) {
	    vemMessage("Unable to bind remote command to menu\n",
		       MSG_DISP);
	    return VEM_CORRUPT;
	}
	if (bdBindAlias(newTable, uidName, comStrs[index]) != VEM_OK) {
	    (void) sprintf(errMsgArea, "Command '%s' is not unique\n",
		    comStrs[index]);
	    vemMessage(errMsgArea, 0);
	}

	if ((keyStrs[index] != NIL(char)) && (*keyStrs[index] != '\0')) {
	    if (bdBindKey(newTable, uidName, *keyStrs[index]) != VEM_OK) {
	        (void) sprintf(errMsgArea, "Key bindings '%s' is not unique\n",
		               keyStrs[index]);
	        vemMessage(errMsgArea, 0);
	    }
	}
	selectNum++;
    }
    /* Attach binding to window */
    if (wnSetInfo(theWin, (wnOpts *) 0, &newTable) != VEM_OK) {
	vemMessage("Unable to bind remote application to window\n", MSG_DISP);
	return VEM_CORRUPT;
    } else return VEM_OK;
}



Window vemOpenWindow(newFacet, geo, host, name, num)
octObject *newFacet;		/* Facet to open      */
STR geo;			/* geometry specification */
STR host;			/* Application host   */
STR name;			/* Application name   */
int num;			/* Application number */
/*
 * This function is called by a remote application that
 * wants to open a new graphics window which contains
 * 'theFacet'.  The application name and number are used
 * to make sure the new window inherits all the application
 * specific commands.  The window is opened in the standard
 * manner using XCreate.  The entire contents of the window
 * are shown.
 */
{
    Window retWindow;
    bdTable altTbl;
    char remoteName[MAXNAMELEN], promptStr[MAXNAMELEN];

    (void) makeRemoteName(remoteName, host, name, num);
    if (bdGetTable(remoteName, &altTbl) != VEM_OK) {
	(void) sprintf(errMsgArea, "WARNING:  No bindings for '%s'\n",
		remoteName);
	vemMessage(errMsgArea, 0);
	altTbl = (bdTable) 0;
    }

    /* Open new window using utility routine in vemUtil.c */
    
    (void) sprintf(promptStr, "New Window Requested by %s", remoteName);
    if (vuOpenWindow(&retWindow, promptStr, newFacet, geo) != VEM_OK) {
	return (Window) 0;
    } else {
	/* Make sure it has alternate bindings */
	wnSetInfo(retWindow, (wnOpts *) 0, &altTbl);
	return retWindow;
    }
}

Window vemOpenRPCWindow(newFacet, rpcSpec, geoSpec)
octObject *newFacet;		/* Facet to open      */
STR rpcSpec;			/* application specification */
STR geoSpec;			/* geometry specification */

/*
 * This function is called by a remote application that
 * wants to open a new graphics window which contains
 * 'theFacet', and start a new remote application.
 * The window is opened in the standard
 * manner using XCreate.  The entire contents of the window
 * are shown.
 */
{
    Window retWindow;
    char promptStr[MAXNAMELEN];

    /* Open new window using utility routine in vemUtil.c */
    (void) sprintf(promptStr, "New RPC Window Requested");
    if (vuRPCWindow(&retWindow, promptStr, newFacet, rpcSpec, geoSpec) != VEM_OK) {
	return (Window) 0;
    }
    return retWindow;
}



vemStatus
vemRemoteInvokeCommand(curSpot, cmdList, binding)
vemPoint *curSpot;		/* Current spot        */
lsList cmdList;
bdBinding *binding;		/* Binding for command */
/*
 * This command attempts to call a VEM command (either tightly
 * bound or remote).  If the TYPE_OUT flag is set,  the command
 * outputs its name (with a colon in front).  If NO_OUTPUT is
 * set,  no console messages of any kind are produced.  The
 * argument list for the command is assumed to be in cur_con.commandArgs.
 */ 
{
    wnOpts opts;
    rpcStatus status;

    switch (binding->bindType) {
    case TIGHT_COMMAND:
	/* Tightly bound command */
	return (*(binding->bindValue.assocFunc))(curSpot, cmdList);
    case INVOKE_REMOTE:
	/* Start new remote application */
	wnGetInfo(curSpot->theWin, &opts, (octId *) 0,
		  (octId *) 0, (bdTable *) 0, (struct octBox *) 0);
	if (curSpot == (vemPoint *) 0) {
	    vemMessage("Cannot invoke application on console window\n",
		       MSG_NOLOG|MSG_DISP);
	} else if (opts.disp_options & VEM_REMOTERUN) {
	    vemMessage("Application running in this window\n",
		       MSG_NOLOG|MSG_DISP);
	} else {
	    char name[1204], *prune;
	    
	    (void) RPCExecApplication(binding->bindValue.theApp.hostName,
				      binding->bindValue.theApp.pathName,
				      curSpot, cmdList, (long) 0);
	    opts.disp_options |= VEM_REMOTERUN;

	    if ((prune = strrchr(binding->bindValue.theApp.pathName, '/')) == NIL(char)) {
		(void) sprintf(name, "%s@%s",
			       binding->bindValue.theApp.pathName,
			       binding->bindValue.theApp.hostName);
	    } else {
		(void) sprintf(name, "%s@%s", ++prune,
			       binding->bindValue.theApp.hostName);
	    }
	    wnSetInfo(curSpot->theWin, &opts, (bdTable *) 0);
	    wnSetRPCName(curSpot->theWin, name);
	}
	break;
	
    case REMOTE_COMMAND:
	/* Invoke remote command */
	status = RPCUserFunction(binding->bindValue.theFun.appNum,
			       (long) binding->bindValue.theFun.appIndex,
			       curSpot, cmdList);
	if (status == RPC_OK) {
	    return VEM_OK;
	} else {
	    return VEM_ABORT;
	}
	break;
    }
    return VEM_OK;
}


/*ARGSUSED*/
vemStatus
vemCommand(cmdName, spot, cmdList, userOpt)
STR cmdName;			/* Command name                  */
vemPoint *spot;			/* Where the command was invoked */
lsList cmdList;			/* Argument list                 */
long userOpt;			/* User option word              */
/*
 * This routine is called by an RPC application to run a named
 * VEM command.  The command name must be a legally defined
 * ALIAS in the binding table of the 'spot' window.  The
 * implementation uses 'vemInvokeCommand' defined in vem.c.
 */
{
    bdTable theTable, altTable;
    bdBinding theBinding;
    char command[150], compString[4096];
    int completions;
    vemStatus status;

    bufBinding(spot->theFct, &theTable);
    if (wnGetInfo(spot->theWin, (wnOpts *) 0, (octId *) 0,
		  (octId *) 0, &altTable, (struct octBox *) 0) != VEM_OK)
      {
	  /* Bad alternate table -- replace with zero */
	  altTable = (bdTable) 0;
      }
    (void) strcpy(command, cmdName);
    status = bdName(theTable, altTable, command, &completions, compString, &theBinding);
    if ((status != VEM_OK) && (status != VEM_NOTUNIQ)) {
	return VEM_CORRUPT;
    }
    return vemRemoteInvokeCommand(spot, cmdList, &theBinding);
}




vemStatus vemCloseApplication(host, name, appNum)
STR host;			/* Application host name     */
STR name;			/* Path to application       */
int appNum;			/* Unique application number */
/*
 * This routine signals the termination of a RPC application.
 * The clean-up work involved is to search all windows for
 * the remote binding table of the application and zero it
 * out.  The binding table is then deleted.
 */
{
    bdTable doomedTable;
    char remoteName[MAXNAMELEN];
    lsGen gen;
    char *ptr;
    struct rpcDemon *demon;

    /* clean up the rpc demons for this application */
    gen = lsStart(RPCDemonList);
    while (lsNext(gen, &ptr, 0) == LS_OK) {
	demon = (struct rpcDemon *) ptr;
	if (demon->app == appNum) {
	    lsDelBefore(gen, 0);
        }
    }

    if (wnSetRPCName(RPCApplication[appNum].theWin, (char *) 0) != VEM_OK) {
	fprintf(stderr, "could not find window to set\n");
    }

    (void) makeRemoteName(remoteName, host, name, appNum);
    if (bdGetTable(remoteName, &doomedTable) != VEM_OK) {
	wnOpts opts;

	if (wnGetInfo(RPCApplication[appNum].theWin, &opts,
		      (octId *) 0, (octId *) 0, (bdTable *) 0,
		      (struct octBox *) 0) == VEM_OK) {
	      opts.disp_options &= ~(VEM_REMOTERUN);
	      wnSetInfo(RPCApplication[appNum].theWin, &opts, (bdTable *) 0);
        }
	return VEM_OK;
    }
    wnZeroAltTbl(doomedTable);
    bdDestroy(doomedTable);
    return VEM_OK;
}


/*
* Given a window (spot), find the number of the application running in it.
* -1 is returned if the number could not be found.
*/
int getApplicationNumber( spot )
    vemPoint *spot;			/* Where command was issued */
{
    bdTable altTable;
    bdBinding binding;
    static int sel[] = {0, 0};
    char name[100];

    if (spot == (vemPoint *) 0) {
	vemMessage("No remote process specified\n", MSG_NOLOG|MSG_DISP);
	return -1;
    }
    wnGetInfo(spot->theWin, (wnOpts *) 0, (octId *) 0,
	      (octId *) 0, &altTable, (struct octBox *) 0);
    if (altTable == (bdTable) 0) {
	vemMessage("No remote process attached to window\n",
		   MSG_NOLOG|MSG_DISP);
	return -1;
    }
    if (bdMenu(altTable, 2, sel, (STR *) name, &binding) != VEM_OK) {
	vemMessage("Can't determine application number\n",
		   MSG_NOLOG|MSG_DISP);
	return -1;
    }
    /* Good news!  We have an application number! */
    return binding.bindValue.theFun.appNum;
}



/*ARGSUSED*/
vemStatus killAppCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command kills the remote application running in the
 * window indicated by 'spot'.  If the there is no application
 * running in the window,  it will complain.  I am trusting
 * that there is at least one remote menu selection.
 */
{
    int n = getApplicationNumber( spot );

    if ( n >= 0 ) {
	/* Good news!  We have an application number! */
	(void) RPCKillApplication( n );
	(void) sprintf(errMsgArea, "Killed application %d\n", n );
	vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    }
    return VEM_OK;
}


/*
 * allow any RPC commands to be run, even ones not in the bindings
 *
 * Rick Spickelmier
 *
 */

vemStatus rpcAnyApplication(spot, cmdList)
vemPoint *spot;
lsList cmdList;
/*
 * run any RPC command
 */
{
    vemOneArg *lastArg;
    Pointer pointer;
    char host[1024], path[1024];
    wnOpts opts;
    char name[1204], *prune;
    
    if (spot == (vemPoint *) 0) {
	vemMessage("No RPC aplications in a non-graphics window\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    if (lsLength(cmdList) < 1) {
	vemMessage("[args] \"host path\" : rpc-any\n", MSG_NOLOG|MSG_DISP);
	vemMessage("[args] \"path\" : rpc-any (host defaults to localhost)\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    /* verify that no remote commands are running in the window */
    wnGetInfo(spot->theWin, &opts, (octId *) 0, (octId *) 0,
	      (bdTable *) 0, (struct octBox *) 0);
    if (opts.disp_options & VEM_REMOTERUN) {
	vemMessage("Application running in this window\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    /* get the last argument */
    lsDelEnd(cmdList, &pointer);
    lastArg = (vemOneArg *) pointer;

    if (lastArg->argType != VEM_TEXT_ARG) {
	vemMessage("[args] \"host path\" : rpc-any\n", MSG_NOLOG|MSG_DISP);
	vemMessage("[args] \"path\" : rpc-any (host defaults to localhost)\n",
		   MSG_NOLOG|MSG_DISP);
	lsNewEnd( cmdList, pointer, 0 ); /* Put arg back on stack. */
	return VEM_ARGRESP;
    }

    if (sscanf(lastArg->argData.vemTextArg, "%s %s", host, path) != 2) {
	(void) sscanf(lastArg->argData.vemTextArg, "%s", path);
	(void) strcpy(host, "localhost");
    }

    (void) RPCExecApplication(host, path, spot, cmdList, (long) 0);

    opts.disp_options |= VEM_REMOTERUN;
    wnSetInfo(spot->theWin, &opts, (bdTable *) 0);

    if ((prune = strrchr(path, '/')) == NULL) {
	(void) sprintf(name, "%s@%s", path, host);
    } else {
	(void) sprintf(name, "%s@%s", ++prune, host);
    } 
    wnSetRPCName(spot->theWin, name);

    bufMarkActive( spot->theFct ); /* All buffers with RPC are active. */
    return VEM_OK;
}

/*
 * Send a signal to an RPC application
 *
 * Andrea Casotto (Aug 91)
 *
 */
static void rpcSignalUsage()
{
    vemMessage("usage: \"signal number\" : rpc-signal\n", MSG_NOLOG|MSG_DISP);
}

vemStatus rpcSignalCmd(spot, cmdList)
    vemPoint *spot;
    lsList cmdList;
{
    vemOneArg *lastArg;
    int signal_number = SIGUSR1; /* By default, send signal USR1. */
    char command[1024];
    char localhost[128];
    char buf[256];		/* For error messages */
    wnOpts opts;
    struct RPCApplication *app;
    char remoteUser[128];
    int appNum;			/* Application number (internal index for vem) */
    Pointer pointer;
    /* HP-UX does not support siglist */
#if !defined(hpux) && !defined(SYSV) && !defined(linux)
    extern char *sys_siglist[];
#endif

    if (spot == (vemPoint *) 0) {
	vemMessage("No RPC aplications in a non-graphics window\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    if (lsLength(cmdList)>1) {
	rpcSignalUsage();
	return VEM_ARGRESP;
    } 

    /* verify that there is ad RPC running in the window */
    wnGetInfo(spot->theWin, &opts, (octId *) 0, (octId *) 0,
	      (bdTable *) 0, (struct octBox *) 0);
    if ((opts.disp_options & VEM_REMOTERUN) == 0 ) {
	vemMessage("No application is running in this window.\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }


    appNum = getApplicationNumber( spot );
    if ( appNum == -1 ) {
	return VEM_ARGRESP;	/* Error */
    }


    if (lsLength(cmdList)==1) {
	/* get the last argument */
	lsDelEnd(cmdList, &pointer);
	lastArg = (vemOneArg *) pointer;
	
	if (lastArg->argType != VEM_TEXT_ARG) {
	    rpcSignalUsage();
	    lsNewEnd( cmdList, pointer, 0 ); /* Put arg back in cmdlist */
	    return VEM_ARGRESP;
	}
	signal_number = atoi( lastArg->argData.vemTextArg );
    }
    /* NSIG (number of signals) is defined in signal.h */
    if ( signal_number < SIGHUP || signal_number > NSIG ) { 
	sprintf( buf, "Illegal signal number %d (should be in [%d,%d])\n", 
		signal_number, SIGHUP, SIGUSR2 );
	vemMessage( buf, MSG_NOLOG|MSG_DISP );
	return VEM_ARGRESP;
    }
    

    app = &(RPCApplication[appNum]);

    /* Use kill if at all possible, else use rsh .. kill */
    if ( gethostname( localhost, 128 ) == -1 ) {
	perror( "getting hostname" );
	vemMessage( "cannot get hostname",MSG_NOLOG|MSG_DISP);;
	return VEM_CORRUPT;
    }
    if ( app->user == NIL(char) && !strcmp(app->host, localhost )) {
#if defined(hpux) || defined(SYSV)
	sprintf( buf, "Signal %d to RPC app. (direct)...\n", signal_number);
#else
	sprintf( buf, "Signal %d(%s) to RPC app. (direct)...\n", signal_number,
		sys_siglist[signal_number]);
#endif
	vemMessage(buf,MSG_NOLOG|MSG_DISP);
	if ( kill( app->pid, signal_number ) == -1 ) {
	    perror( "cannot send signal" );
	}
    } else {

	if (app->user == NIL(char)) {
	    remoteUser[0]='\0';
	} else {
	    (void) sprintf(remoteUser, "-l %s", app->user);
	}

    
	sprintf( command, "rsh %s %s kill -%d %ld &",
		app->host, remoteUser,
		signal_number, (long)app->pid );
    
#if defined(hpux) || defined(SYSV)
	sprintf(buf, "Signal %d to RPC app. (via rsh)...\n", signal_number);
#else
	sprintf(buf, "Signal %d(%s) to RPC app. (via rsh)...\n", signal_number,
		sys_siglist[signal_number]);
#endif
	vemMessage(buf,MSG_NOLOG|MSG_DISP);


	if (util_csystem(command) < 0) {
	    vemMessage("RPC Warning: cannot send signal\n", MSG_NOLOG|MSG_DISP);
	}
    }
    return VEM_OK;
}



vemStatus rpcReset(spot, cmdList)
vemPoint *spot;
lsList cmdList;
/*
 * tell a window that an rpc application is not running in it
 * (for Permission Denied and File Not Found errors before initialization)
 */
{
    wnOpts opts;

    if (spot == (vemPoint *) 0) {
	vemMessage("No RPC aplications in a non-graphics window\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    if (lsLength(cmdList) != 0) {
	vemMessage(": rpc-reset\n", MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    if (wnGetInfo(spot->theWin, &opts, (octId *) 0,
		  (octId *) 0, (bdTable *) 0,
		  (struct octBox *) 0) == VEM_OK)
      {
	  opts.disp_options &= ~(VEM_REMOTERUN);
	  wnSetInfo(spot->theWin, &opts, (bdTable *) 0);
	  wnSetRPCName(spot->theWin, (char *) 0);
      }

    return VEM_OK;
}



/*ARGSUSED*/
void vemFlushTechInfo(fct)
octObject *fct;
/*
 * Flushes the technology information associated with the facet
 * `fct'.  No actual refresh of the display is done.  It tries
 * to change the technology in place.
 */
{
    /* Wipes out all locally cached information */
    tkWipe();
    /* Wipes out all of TAPs cached information */
    tapFlushCache();
    /* Causes all buffers to be reconsidered */
    bufTechFlush();
    /* Redraws all windows */
    wnRefresh();
}



static char *_vem_dsp_name = (char *) 0;
static int _vem_chrom = TAP_BW;

void vemInitDisplayType(name, chromatism)
char *name;
int chromatism;
/*
 * This routine initializes the current display name and chromatism.
 */
{
    _vem_dsp_name = name;
    _vem_chrom = chromatism;
    tapSetDisplayType(name, chromatism);
    return;
}


void
vemSetDisplayType(name, chromatism)
char *name;			/* Device name         */
int chromatism;			/* TAP_COLOR or TAP_BW */
/*
 * Sets the current display type to the specified name with the
 * specified chromatism.  This routine uses the technology module
 * to wipe out all cached technology information and the window
 * module to cause a redraw of all VEM windows.  The name and
 * chromatism are saved and returned by a call to vemGetDisplayType().
 */
{
    tapSetDisplayType(name, chromatism);
    _vem_dsp_name = name;
    _vem_chrom = chromatism;
    tkWipe();
    wnRefresh();
    return;
}


void
vemGetDisplayType(name, chromatism)
char **name;			/* RETURN */
int *chromatism;		/* RETURN */
/*
 * Returns the current display name and chromatism.  These were
 * set by the last call to vemSetDisplayType.
 */
{
    *name = _vem_dsp_name;
    *chromatism = _vem_chrom;
    return;
}


char *
rpcVerString()
/*
 * return the rpc version
 */
{
    static char dummy[1024];

    (void) sprintf(dummy, "%ld", RPC_VERSION);

    return(dummy);
}
