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
 * network routines for the RPC server
 */

/* for EWOULDBLOCK */
#include <errno.h>

#if defined(ultrix)
#include <time.h>
#else
/* kludge - due to moving target, handled in -I */
#define KERNEL
#include <time.h>
#undef KERNEL
#endif

#include <sys/types.h>
#include <sys/socket.h>

#ifdef SYSV
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <netinet/in.h>
#ifdef DNET
#include <netdnet/dn.h>
#include <pwd.h>
#endif /* DNET */

#include <X11/Xlib.h>
#include "xvals.h"
#include "list.h"
#include "bindings.h"
#include "buffer.h"
#include "message.h"
#include "commands.h"
#include "rpcServer.h"
#include "serverUtils.h"

/* maximum number of rpc applications */
int RPCMaxApplications = 0;


/* server is not byte swapped */
int RPCByteSwapped = 0;

/* largest file descriptor in use - used for select */
int RPCLargestFD = 0;

#ifdef DNET
extern char *getnodename();
#endif /* DNET */

/* how to spell "remote shell" */
#ifdef hpux
#define RemoteShell "remsh"
#else
#define RemoteShell "rsh"
#endif

/* Forward declarations */
static int RPCAddApplication
	ARGS((char *host, char *name));

/*
 * wait for RPC Events
 *
 * if a read event comes in, accept on it and return no event (timeout)
 *
 * returns:
 *	    server dies cleanly (RPC_EXIT)
 *	    something bad happened, abort (RPC_ABORT)
 *	    no event, timeout, accept (RPC_NO_EVENT)
 *	    X event (RPC_X_EVENT)
 *          file event (RPC_FILE_EVENT)
 *          rpc event (RPC_EVENT)
 *
 * retains state, since multiple events can be selected on a single select
 *
 */
int
RPCGetEvent(acceptMask, requestMask, fileMask, application, socket)
rpc_fd_set *acceptMask;
rpc_fd_set *requestMask;
rpc_fd_set *fileMask;
int *application;		/* application making the request */
int *socket;                    /* socket selected on, for file event */
{
    static rpc_fd_set *mask = NIL(rpc_fd_set);
    static int nfound = 0;	/* number of things selected */
    int s;			/* socket/file descriptor selected */
    int fd;			/* file descriptor for accept */
    int index;			/* application index */

    /* initialization */
    if (mask == NIL(rpc_fd_set)) {
	mask = RPCALLOC(rpc_fd_set);
    }

    /* see if there are any pending events */

    if (nfound == 0) {

	/* no pending events */
	
	/* set up the select mask */
	RPC_FD_OR_3(fileMask, requestMask, acceptMask, mask);
	
	/* no items to select on so exit */
	if (RPC_FD_ALL_ZERO(mask)) {
	    return(RPC_EXIT);
	}

	nfound = select(RPCLargestFD + 1,
			mask,
			RPCNIL(rpc_fd_set),
			RPCNIL(rpc_fd_set),
			RPCNIL(struct timeval));
	
	/* select error condition */
	if (nfound < 0) {
	    nfound = 0;
	    return(RPC_ABORT);
	}
	
	/* will never happen if RPC_TIMEOUT is not defined */
	if (nfound == 0) {
	    return(RPC_NO_EVENT);
	}
    }

    if (RPC_FD_ANY_COMMON_BITS(fileMask, mask)) {
	nfound--;
	s = RPC_FD_FIRST_SET_AND(fileMask, mask) - 1;
	RPC_FD_CLR(s, mask);
	*socket = s;
	return(RPC_FILE_EVENT);
    }

    if (RPC_FD_ANY_COMMON_BITS(acceptMask, mask)) {
	nfound--;				/* handle one request */
	s = RPC_FD_FIRST_SET_AND(acceptMask, mask) - 1;
	
	/* remove the current request from the pending mask */
	RPC_FD_CLR(s, mask);

	for (index = 0; index < RPCMaxApplications; index++) {

	    if (RPCApplication[index].active == 0) {
		continue;
	    }

	    /* accept on socket */
	    if (s == RPCApplication[index].WaitSocket) {
		if ((fd = accept(s, RPCNIL(struct sockaddr), RPCNIL(int))) < 0) {
		    vemMessage("Vem RPC Error: can't accept stream\n", 0);
		    return(RPC_ABORT);
		}
		if ((RPCApplication[index].SendStream = fdopen(fd, "w")) == NIL(FILE)) {
		    vemMessage("Vem RPC Error: can't fdopen stream\n", 0);
		    return(RPC_ABORT);
		}
		if ((RPCApplication[index].ReceiveStream = fdopen(fd, "r")) == NIL(FILE)) {
		    vemMessage("Vem RPC Error: can't fdopen stream\n", 0);
		    return(RPC_ABORT);
		}

		/* close the old socket */
		(void) close(s);
		RPCLargestFD = MAX(RPCLargestFD, fileno(RPCApplication[index].ReceiveStream));

		/*
	  	 * stop connection attempts and allow requests 
		 */
		RPC_FD_SET(fileno(RPCApplication[index].ReceiveStream), RPCSelectRequest);
		RPC_FD_CLR(RPCApplication[index].WaitSocket, RPCSelectAccept);
		RPCApplication[index].WaitSocket = -1;
		return(RPC_NO_EVENT);
	    }
	}
	vemMessage("Vem RPC Error: can't find application for stream\n", 0);
	return(RPC_ABORT);
    }

    if (RPC_FD_ANY_COMMON_BITS(requestMask, mask)) {
	nfound--;				/* handle one request */
	s = RPC_FD_FIRST_SET_AND(requestMask, mask) - 1;

	/* remove the current request from the pending mask */

	RPC_FD_CLR(s, mask);

	/* check for a request on the X file descriptor */
	if (s == Xfd) {
	    return(RPC_X_EVENT);
	}

	for (index = 0; index < RPCMaxApplications; index++) {
	    if (RPCApplication[index].active == 0 ||
		RPCApplication[index].ReceiveStream == (STREAM) -1) {
		/* not active or not connected */
		continue;
	    }
	    if (s == fileno(RPCApplication[index].ReceiveStream)) {
		*application = index;
		return(RPC_EVENT);
	    }
	}
	vemMessage("Vem RPC Error: can't find application for stream\n", 0);
	return(RPC_ABORT);
    }
    return(RPC_NO_EVENT);
}


int
RPCinet_createAndBindSocket(port)
int *port;
/*
 * create an internet socket and bind it
 */
{
    int s;			/* socket */
    struct sockaddr_in sin;	/* internet binding address */
    int retry;			/* number of times to retry the bind */
    int length;
    retry = 20;

    vemMessage("RPC Server: inet protocol\n", 0);
    
    sin.sin_port = 0;
    sin.sin_family = AF_INET;		/* family - internet */
    sin.sin_addr.s_addr = INADDR_ANY;	/* allow connect from any where */
    
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	vemMessage("Vem RPC Error: inet socket fails\n", 0);
	perror("socket inet");
	return(-1);
    }

    while (retry > 0) {
	if (bind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
	    vemMessage("Vem RPC Error: retrying bind\n", 0);
	    retry--;
	    continue;
	}
	break;
    }
    
    if (retry == 0) {
	vemMessage("Vem RPC Error: can't find a port\n", 0);
	return(-1);
    }

    length = sizeof(sin);
    
    /* XXX should be (struct sockaddr *) */
    if (getsockname(s, (char *) &sin, &length) < 0) {
	vemMessage("Vem RPC Error: getsockname failed\n", 0);
	return(-1);
    }
    
    *port = sin.sin_port;

    return(s);
}

#ifdef DNET	
int
RPCdnet_createAndBindSocket(port)
int *port;
/*
 * create a decnet socket and bind it
 */
{
    int s;			/* socket */
    struct sockaddr_dn dn;	/* decnet binding address */
    int retry;			/* number of times to retry the bind */
	
    retry = 20;

    vemMessage("RPC: decnet protocol\n", 0);
    
    if ((s = socket(AF_DECnet, SOCK_STREAM, 0)) < 0) {
	vemMessage("Vem RPC Error: decnet socket fails\n", 0);
	perror("dnet socket");
	return(-1);
    }
    
    (void) memset((char *) &dn, sizeof(dn), (char) 0);
    dn.sdn_family = AF_DECnet;

    /* XXX probably horribly wrong, but it works */
    (void) sprintf(dn.sdn_objname, "X%d", getpid());
    dn.sdn_objnamel = strlen(dn.sdn_objname);
    
    while (retry > 0) {
	if (bind(s, (struct sockaddr *) &dn, sizeof(dn)) < 0) {
	    vemMessage("RPC: retrying bind\n", 0);
	    retry--;
	    continue;
	}
	break;
    }
    
    if (retry == 0) {
	vemMessage("Vem RPC Error: can't find an object number\n", 0);
	return(-1);
    }
    
    *port = getpid();
    
    return(s);
}
#endif /* DNET */


/*
 * create a passive socket
 *  returns a socket if okay, else RPC_ERROR (-1)
 */
rpcStatus
RPCSetUpSocket(protocol, port)
char *protocol;
int *port;
{
    int s;			/* socket */

    if (strcmp(protocol, "inet") == 0) {

	if ((s = RPCinet_createAndBindSocket(port)) < 0) {
	    vemMessage("Vem RPC Error: socket create and bind failed\n", 0);
	    return(RPC_ERROR);
	}
		
#ifdef DNET
    } else if (strcmp(protocol, "decnet") == 0) {

	if ((s = RPCdnet_createAndBindSocket(port)) < 0) {
	    vemMessage("Vem RPC Error: socket create and bind failed\n", 0);
	    return(RPC_ERROR);
	}
#endif /* DNET */
	
    } else {
	(void) sprintf(RPCErrorBuffer, "Vem RPC Error: unsupported protocol: %s\n", protocol);
	vemMessage(RPCErrorBuffer, 0);
	return(RPC_ERROR);
    }
    
    /* make the socket passive */
    if (listen(s, 10) < 0) {
	vemMessage("Vem RPC Error: listen fails\n", 0);
	perror("listen");
	return(RPC_ERROR);
    }

    return(s);
}


/*
 * find a free entry in the rpc application array
 */
static int
RPCFindSlot()
{
    int i;

#ifdef SYSV
    struct rlimit	rlp;
#endif
    if (RPCMaxApplications == 0) {
#ifdef SYSV
	getrlimit(RLIMIT_NOFILE, &rlp);
	RPCMaxApplications = rlp.rlim_cur - 1 - 4; /* -4 for stdin, out, err, X */
#else
	RPCMaxApplications = getdtablesize() - 4; /* -4 for stdin, out, err, X */
#endif
	RPCApplication = RPCARRAYALLOC(struct RPCApplication, RPCMaxApplications);
	for (i = 0; i < RPCMaxApplications; i++) {
	    RPCApplication[i].active = 0;
	}
    }

    for (i = 0; i < RPCMaxApplications; i++) {
	if (RPCApplication[i].active == 0) {
	    return(i);
	}
    }
    return(-1);
}

	    
/*
 * create an application and set up the necessary sockets and select masks
 */
static int
RPCAddApplication(host, name)
char *host;
char *name;
{
    char newhost[256];
    int size;
    char *p;
    struct RPCApplication *app;
    int indx;
    
    
    indx = RPCFindSlot();
    if (indx == -1) {
	vemMessage("Vem RPC Error: too many applications\n", MSG_DISP);
	return(RPC_ERROR);
    }

    app = &(RPCApplication[indx]);
    app->active = 1;

    /* host is of the form 'machine/user:proto' */

    app->host = RPCstrsave(host);

    /* see if a protocol is specified */
    if ((p = strchr(app->host, ':')) == NIL(char)) {
	app->protocol = RPCstrsave("inet");
    } else {
	*p = '\0';
	app->protocol = ++p;
    }

    /* see if a user name is specified */
    if ((p = strchr(app->host, '/')) == NIL(char)) {
	app->user = NIL(char);
    } else {
	*p = '\0';
	app->user = ++p;
    }

    /* if "localhost", get the real host name */
    
    if (strcmp(app->host, "localhost") == 0) {

	if (strcmp(app->protocol, "inet") == 0) {
	    size = (int) sizeof(newhost);
	    if (gethostname(newhost, size) < 0) {
		vemMessage("Vem RPC Error: can't get my hostname\n", 0);
		return(RPC_ERROR);
	    }
	    RPCFREE(app->host);
	    app->host = RPCstrsave(newhost);
	    
#ifdef DNET
	} else if ((strcmp(app->protocol, "decnet") == 0)
		   || (strcmp(app->protocol, "decnet-vms") == 0)) {
	    (void) strcpy(newhost, getnodename());
	    RPCFREE(app->host);
	    app->host = RPCstrsave(newhost);
	    
#endif /* DNET */
	} else {
	    vemMessage("Vem RPC Error: unsupported protocol\n", MSG_DISP);
	    return(RPC_ERROR);
	}
	
    }
    
    app->name = RPCstrsave(name);
    app->state = RPC_BUSY;
/* fprintf(stderr, "going busy (add application request)\n"); */

    if (strcmp(app->protocol, "decnet-vms") != 0) {

	/* create a socket for the connection */
	if ((app->WaitSocket = RPCSetUpSocket(app->protocol, &(app->Port))) == RPC_ERROR) {
	    vemMessage("Vem RPC Error: socket failed\n", 0);
	    return(RPC_ERROR);
	}

	/* allow connections to be made to this socket */
	RPC_FD_SET(app->WaitSocket, RPCSelectAccept);
	/* set the value of the largest FD - for select */
	RPCLargestFD = MAX(RPCLargestFD, app->WaitSocket);
    }

    /* no value for the stream right now - set on the accept */
    app->SendStream = (STREAM) -1;
    app->ReceiveStream = (STREAM) -1;

    return(indx);
}

/* close the connection */
void
RPCServerEnd(application)
int application;
{
    STREAM stream;
    char buffer[1024];
    extern long time();
    struct RPCApplication *app;


    app = &(RPCApplication[application]);

    (void) sprintf(buffer, "Elapsed time is %ld seconds\nvem> ",
	    (long)(time((long *) 0) - app->startingTime));
    vemMessage(buffer, MSG_NOLOG|MSG_DISP);

    stream = app->ReceiveStream;

    RPC_FD_CLR(fileno(stream), RPCSelectRequest);

    (void) fclose(stream);
    (void) fclose(app->SendStream);

    app->ReceiveStream = (STREAM) -1;
    app->SendStream = (STREAM) -1;
    app->WaitSocket = 0;
    app->active = 0;
}

/*
 * remote exec the application
 */
rpcStatus
RPCExecApplication(host, path, spot, argList, userOptionWord)
char *host;		/* host to run the application */
char *path;		/* path to the application */
vemPoint *spot;
lsList argList;
long userOptionWord;
{
    char command[1024];		/* command to be run */
    char thisHost[256];		/* local host name */
    int size;
    int application;
    RPCSpot *Spot;
    lsList cmdList;
    struct RPCApplication *app;
    char number[4];
    long *pointer;
    char remoteUser[128];
    
    application = RPCAddApplication(host, path);

    app = &(RPCApplication[application]);
    
    /* get the local host name */
    
    if (strcmp(app->protocol, "inet") == 0) {
	size = (int) sizeof(thisHost);
	if (gethostname(thisHost, size) < 0) {
	    vemMessage("Vem RPC Error: can't get my internet hostname\n", 0);
	    return(RPC_ERROR);
	}
#ifdef DNET
    } else if ((strcmp(app->protocol, "decnet") == 0)
	       || (strcmp(app->protocol, "decnet-vms") == 0)) {
	(void) strcpy(thisHost, getnodename());
	if (thisHost == 0) {
	    vemMessage("Vem RPC Error: can't get my decnet node name\n", 0);
	    return(RPC_ERROR);
	}
#endif /* DNET */
    } else {
	vemMessage("Vem RPC Error: unsupported protocol\n", 0);
	return(RPC_ERROR);
    }

    app->localhost = RPCstrsave(thisHost);
    
    /* translate the VEM stuff to RPC stuff */

    app->menuSize = 0;

    /*
     * translate the spot
     */
    Spot = RPCALLOC(RPCSpot);
    app->spot = Spot;
    Spot->theWin = spot->theWin;
    app->theWin = spot->theWin;
    Spot->thePoint = spot->thePoint;

    Spot->facet = spot->theFct;

    /*
     * translate the argument list
     */

    /* create a cloned bag list */
    app->clonedBags = lsCreate();
    (void) RPCTranslateArgsToRPC(argList, &cmdList, Spot->facet, application);

    app->argList = cmdList;
    app->userOptionWord = userOptionWord;

    app->display = RPCstrsave(DisplayString(xv_disp()));
    
    if (strcmp(app->protocol, "decnet-vms") != 0) {
	/*
	 * set up the numbers so that application can determine if it is byte
	 * swapped relative to the server
	 */
	
	number[0] = 0x01;
	number[1] = 0x02;
	number[2] = 0x03;
	number[3] = 0x04;
	pointer = (long *) number;

	if (app->user == NIL(char)) {
	    remoteUser[0]='\0';
	} else {
	    (void) sprintf(remoteUser, "-l %s", app->user);
	}

	/* if the remote and local hosts are different,
		or if a remote user has been specified */
	if ((strcmp(app->host, app->localhost) != 0)
		|| (strlen(remoteUser) > 0))
	    /* then build up a backgrounded r-shell with stdin disconnected */
            (void) sprintf(command,
			   "%s %s %s -n %s %s %s %d %s %d %d %d %d %ld &",
			   RemoteShell,
			   app->host,
			   remoteUser,
			   app->name,
			   DisplayString(xv_disp()),
			   app->localhost,
			   app->Port,
			   app->protocol,
			   number[0], number[1], number[2], number[3],
			   *pointer);
	    /* else build up a backgrounded shell with stdin disconnected */
	    else (void) sprintf(command, "%s %s %s %d %s %d %d %d %d %ld &",
				app->name,
				DisplayString(xv_disp()),
				app->localhost,
				app->Port,
				app->protocol,
				number[0], number[1], number[2], number[3],
				*pointer);
	printf("%s\n", command);
	vemMessage(command, MSG_DISP);

	if (system(command) < 0) {
	    (void) sprintf(RPCErrorBuffer, "Vem RPC Error: Cannot exec %s on host %s\n",
		    app->name,
		    app->host);
	    vemMessage(RPCErrorBuffer, MSG_DISP);
	    return(RPC_ERROR);
	}
	
#ifdef DNET
    } else {

	int s;
	char buffer[1024];
	struct passwd *pw;

	/* get the user name */
	if (app->user == NIL(char)) {
	    pw = getpwuid(getuid());
	    app->user = RPCstrsave(pw->pw_name);
	}
	
	(void) sprintf(buffer, "%s/%s", app->host, app->user);
	
	if ((s = dnet_conn(buffer, path, SOCK_STREAM, 0, 0, 0, 0)) < 0) {
	    vemMessage("Vem RPC Error: dnet connect failed\n", 0);
	    nerror("dnet connection");
	    return(RPC_ERROR);
	}

	if ((app->ReceiveStream = fdopen(s, "r")) == NIL(FILE)) {
	    vemMessage("Vem RPC Error: server: can't assign socket to read stream\n", 0);
	    return(RPC_ERROR);
	}
	
	if ((app->SendStream = fdopen(s, "w")) == NIL(FILE)) {
	    vemMessage("Vem RPC Error: server: can't assign socket to write stream\n", 0);
	    return(RPC_ERROR);
	}

	/* set up the selection mask */
	RPC_FD_SET(fileno(app->ReceiveStream), RPCSelectRequest);
	RPCLargestFD = MAX(RPCLargestFD, fileno(app->ReceiveStream));
	app->WaitSocket = -1;
#endif /* DNET */
    }	

    app->startingTime = time((long *) 0);
    
    if (app->user == NIL(char)) {
	(void) sprintf(RPCErrorBuffer, "%s running on host %s\n", app->name, app->host);
    } else {
	(void) sprintf(RPCErrorBuffer, "%s running on host %s as user %s\n", app->name, app->host, app->user);
    }
    vemMessage(RPCErrorBuffer, MSG_DISP);

    return(RPC_OK);
}


/*
 * opposite of the above - kill the application
 *  does not work if the application is setuid somebody else
 */

rpcStatus
RPCKillApplication(application)
int application;
{
    char command[1024];
    struct RPCApplication *app;
    char remoteUser[128];

    app = &(RPCApplication[application]);

    if (app->user == NIL(char)) {
	remoteUser[0]='\0';
    } else {
	(void) sprintf(remoteUser, "-l %s", app->user);
    }

    (void) sprintf(command, "rsh %s %s kill -%d %ld",
		     app->host,
		     remoteUser,
		     RPC_KILL_SIGNAL,
		     (long)app->pid);

    if (system(command) < 0) {
	vemMessage("RPC Warning: can't execute the kill\n", MSG_DISP);
	return(RPC_OK);
    }
    
/*
 * not needed since RPC will detect the application dying via
 * a selection failure and then it will call this
 *
 *  vemCloseApplication(RPCApplication[application].host,
 *			RPCApplication[application].name,
 *			application);
 *
 */
    return(RPC_OK);
}


/*
 * process a remote file request
 */
extern int errno;


void
RPCProcessFileWriteRequest(socket, fd)
int socket;
int fd;
{
    char buffer[BUFSIZ];
    int count;
    
    /* read until EOF, EWOULDBLOCK or an error */
    for (;;) {
	count = read(socket, buffer, sizeof(buffer));
	if ((count == 0) && (errno == EWOULDBLOCK)) return;
	if (count <= 0) {
	    (void) close(fd);
	    (void) close(socket);
	    RPC_FD_CLR(socket, RPCFileRequest);
	    return;
	}
	/* write the data to the file, XXX should check the count! */
	(void) write(fd, buffer, count);
    }
}

