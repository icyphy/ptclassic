#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
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
*/
#include "copyright.h"
#include "port.h"
#include "vov.h"
#include "utility.h"
#include "sys/types.h"
#include <sys/stat.h>
#include "errtrap.h"
#include "st.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#if defined(__sparc) && !defined(__svr4__) && defined(__GNUC__)
#include <sys/time.h>
#include <sys/resource.h>
extern int wait3(int *statusp, int options, struct rusage *rusage);
#endif

#include "vov_lib.h"

#define VOV_CHANNEL "$OCTTOOLS/bin/vov_channel"

static int verbose = 0;			/* Flag used for debugging purposes. */
static int channel_started_by_me = 0;	/* Set if channel forked by this process. */
static int sendDes;			/* File descriptor used to send messages 
					   to channel. */
static int receiveDes;			/* File descriptor used to receive
					   messages from channel. */
static int channelPid = 0;		/* PID of the channel. */
static int redirect = 1;		/* Redircet stdout and stderr. */  
static int enabled = 1;			/* Global flag, user controllable. */


int VOVdisable()
/*
 *  Reset the global enable flag for VOV.
 */
{
    static char* host_name = 0;
    if ( enabled ) {
	enabled = 0;
	host_name = getenv("VOV_HOST_NAME") ;
	if ( host_name != NIL(char) ) {
	    util_unsetenv( "VOV_HOST_NAME" );
	}
    } else {
	enabled = 1;
	if ( host_name ) util_setenv( "VOV_HOST_NAME", host_name);
    }
    return enabled;
}

void VOVredirect( r )
    int r;
/*
* Change the value of the redirect flag.
*/
{
    redirect = r;
}

static int startConditions()
    /*
     * Check that there are enough data to establish connection to
     * the VOV server and to fork the VOV channel.
     * RETURN: 1 all is OK
     *         0 otherwise.
     */
{
    struct stat buf;
    char* channelBin;

    /* Check two environment variables necessary to establish connection. */
    if ( getenv( "VOV_HOST_NAME" ) == NIL( char ) ) return 0;
    if ( getenv( "VOV_PROJECT_NAME" ) == NIL( char ) ) return 0;

    channelBin = util_file_expand( VOV_CHANNEL );

    /* Make sure that the channel is available and running. */
    if ( stat( channelBin, &buf ) == -1 ) {
	return 0;		/* Not available */
    }
    return 1;
}


static void declareChannelOpen()
    /*
     * Set the environment variable VOV_CHANNEL_OPEN.
     * If the current process forks another process which also tries to 
     * connect to VOV, that process will check this env-variable
     * and use the same channel.
     */
{
    char buf[64];
    sprintf( buf, "%d", receiveDes * 1000 + sendDes );
    util_setenv( "VOV_CHANNEL_OPEN", buf );
}

static int channelAlreadyOpen()
    /*
     * Check if the channel is already open. If so, use it.
     */
{
    char* fdstring = getenv( "VOV_CHANNEL_OPEN" );
    int codedfd;

    if ( fdstring == NIL(char) ) { /* Variable not set. */
	return 0;		    /* Channel is not already open.  */
    }
    codedfd = atoi( fdstring );	    /* Get encoded value of channel descriptors. */
    /* printf( "Using channel already open %d\n", codedfd ); */ /* Debug. */
    receiveDes = codedfd / 1000;
    sendDes = codedfd % 1000;
    return 1;
}


/* 
  NORMAL args for signal are:
  int sig; int code; struct sigcontext *scp;
   
  But this is not very portable. Best to avoid args.

 */

static SIGNAL_FN trapChildHandler( ) /* All args are ignored */
{
  int pid;
#ifdef USE_WAITPID
    int	status;
    if ( (pid = waitpid((pid_t) -1, &status, 0)) == -1)
      perror("waitpid");
#else

#ifdef WAIT_TAKES_INT_STAR
    int status;    
#else
    union wait status;
#endif /* WAIT_TAKES_INT_STAR */

    pid = wait3( &status, 0, 0 );

#endif /* USE_WAITPID */
    
    if ( pid == channelPid ) {
	errRaise( "VOV", 1, "Channel died" );
    } else {
	fprintf(stderr, "Strange child died" );
    }
}


static int pipeOpen()
    /*
     * RETURN: 0, if pipes are inactive, 1 if they are active
     */
{
    int pipes[4][2];
#ifndef PTHPPA
    extern char* environ;	/* Declared in unistd.h on hppa*/
#endif
    char* channelBin;
    int p;			/* index for pipes. */

    /* Make sure that the channel is available and running. */
    if ( ! startConditions() ) {
	return 0;
    }

    channelBin = util_file_expand( VOV_CHANNEL );

    /* Open the 4 pipes */
    for ( p = 0 ; p < 4 ; p++ ) {
	if ( pipe( pipes[p] ) == -1 ) {
	    errRaise( "VOV", 1, "Pipe failed" );
	}
    }

    sendDes = pipes[2][1];
    receiveDes = pipes[3][0];

    channelPid = vfork();
    if ( channelPid == -1 ) {
	errRaise( "VOV", 1, "Fork failed" );
    }

    if ( channelPid == 0 ) {		/* This is the child process. */
	char* argv[8];
	int i = 0;			/* Index in argv. */

	close( pipes[0][1] );
	close( pipes[1][1] );
	close( pipes[2][1] );
	close( pipes[3][0] );
	pipes[3][0] = pipes[3][1]; /* This simplifies following loop. */

	argv[i++] = util_file_expand( VOV_CHANNEL );
	for ( p = 0 ; p < 4 ; p++ ) {
	    char buf[32];
	    sprintf( buf, "%d", pipes[p][0] );
	    argv[i++] = util_strsav( buf );
	}
	argv[i++] = 0 ;		/* Terminate arg list. */

	if ( execve( argv[0], (char **CONST)argv,
		    (char **CONST)environ ) == -1 ) {
	    errRaise( "VOV", 1, "Failed execve");
	}
	_exit( 127 );		/* Not reached. */
    }

    close( pipes[0][0] );
    close( pipes[1][0] );
    close( pipes[2][0] );
    close( pipes[3][1] );

    /* Redirect stdout and stderr through the channel */
    if ( redirect ) {
	if ( dup2( pipes[0][1], fileno( stdout )) == -1 ) {
	    errRaise( "VOV", 1 , "Cannot redirect stdout" );
	}
	if ( dup2( pipes[1][1], fileno( stderr )) == -1 ) { 
	    errRaise( "VOV", 1 , "Cannot redirect stderr" );
	}
    }

    declareChannelOpen();
    return 1;
}

void pipeClose()
{
    close( receiveDes );
    close( sendDes );
}


int libActive( startFlag )
    int startFlag;
{
    static int active = -1;

    if (!enabled) return 0;		/* Check the global enabling flag first. */

    if ( active == -1 ) {
	if (  startFlag ) {
	    active = pipeOpen();
	} else {
	    active = 0;		/* VOVbegin was not called first. */
	}
    }
    return active;
}

void pipeSendString( s )
    char* s;
{
    char buf[1024];
    sprintf( buf, "%s\n", s );
    if ( verbose ) printf ( buf );
    if ( write( sendDes, buf, strlen(buf) ) == -1 ) {
	errRaise( "VOV", 1, "Write string failed" );
    }
}

void pipeSendInt( n )
    int n;
{
    char buf[256];
    sprintf( buf, "%d", n );
    if ( verbose ) printf ( buf );
    pipeSendString( buf );
}

#define BUF_SIZE 64
int pipeReceive()
{
    char buf[BUF_SIZE];					/* We expect short answers. */
    int count;						/* Number of characters read. */
    SIGNAL_FN (*oldhandler)();				/* Signal handler. */

    oldhandler = signal( SIGCHLD, trapChildHandler );	/* Trap possible death of channel. */
    do {
	count =  read( receiveDes, buf, BUF_SIZE ) ;
	if ( count == -1 ) {
	    errRaise( "VOV", 1 , "Read failed" );
	}
    } while ( count == 0 );
    buf[count] = '\0';
    signal( SIGCHLD, oldhandler );			/* Restore old signal handler. */
    return atoi( buf );					/*  */
}

int VOVbegin( argc, argv )
    int argc;
    char** argv;
{
    if ( channelAlreadyOpen() ) {
	return 0;
    }
    if ( libActive(1) )  {
	int i;
	channel_started_by_me = 1;
	pipeSendString( "BEGIN" );
	pipeSendInt( argc );
	for ( i = 0 ; i < argc; i++ ) {
	    pipeSendString( argv[i] );
	}
	if ( pipeReceive() == 0 ) {
	    errRaise( "VOV", 1, "Could not connect to server.\n\nUse 'unsetenv VOV_HOST_NAME' and try again" );
	}
    } 
    if ( libActive( 0 ) ) {
	char* exec = util_path_search(argv[0]);
	if ( exec != NIL(char) ) {
	    (void) VOVinput( VOV_EXECUTABLE, exec ); /*  */
	} else {
	    errRaise( "VOV", 1, "Could not find executable for %s", argv[0] );
	}
    }
    return 0;
}


static st_table* inputTable = 0;
static st_table* outputTable = 0;

static char* makekey( type, name )
    VovPlaceType type;
    char* name;
{
    static char buf[1024];
    (void)sprintf( buf, "%2d;%s", type, name );
    return buf;
}

static void addInput( type, name )
    VovPlaceType type;
    char* name;
{
    char* key;
    if ( !inputTable ) {
	inputTable = st_init_table( strcmp, st_strhash );
    }
    
    key = util_strsav( makekey( type, name ) );
    if ( st_insert( inputTable, key, 0 ) ) {
	errRaise( "VOV", 1, "Already in table" );
    }
}

static void addOutput( type, name )
    VovPlaceType type;
    char* name;
{
    char* key;
    if ( !outputTable ) {
	outputTable = st_init_table( strcmp, st_strhash );
    }
    
    key = util_strsav( makekey( type, name ) );
    if ( st_insert( outputTable, key, 0 ) ) {
	errRaise( "VOV", 1, "Already in table" );
    }
}

static int isInput( type, name )
    VovPlaceType type;
    char* name;
{
    if ( inputTable ) {
	char* key = makekey( type, name );
	char* value;
	return st_lookup( inputTable, key, &value );
    }
    return 0;
}

static int isOutput( type, name )
    VovPlaceType type;
    char* name;
{
    if ( outputTable ) {
	char* key = makekey( type, name );
	char* value;
	return st_lookup( outputTable, key, &value );
    }
    return 0;
}




int VOVaffinity( affinity )
    char* affinity;
{
    if ( ! libActive(0) ) return 0;
    pipeSendString( "AFFINITY" );
    pipeSendString( affinity );
    return pipeReceive();
}

int VOVinput( type, name )
    VovPlaceType type;
    char* name;
{
    if ( ! libActive(0) ) return 0;
    if ( isInput( type, name ) ) return 0; /* Do not do it again. */

    if ( isOutput( type, name ) ) {
	/* Place already declared as output. Input declarations are skipped. */
	return 0;
    }
    addInput( type, name );
    pipeSendString( "INPUT" );
    pipeSendInt( type );
    pipeSendString( name );
    return pipeReceive();
}

int VOVoutput( type, name )
    VovPlaceType type;
    char* name;
{
    if ( ! libActive(0) ) return 0;
    if ( isOutput( type, name ) ) return 0; /* Do not do it again. */

    addOutput( type, name );
    pipeSendString( "OUTPUT" );
    pipeSendInt( type );
    pipeSendString( name );
    return pipeReceive();
}



int VOVdelete( type, name )
    VovPlaceType type;
    char* name;
{
    if ( ! libActive(0) ) return 0;

    if ( ! isOutput( type, name ) ) {
	/* Only outputs can be deleted (temporary places) */
	return 0;		/* Do nothing. */
    }
    pipeSendString( "DELETE" );
    pipeSendInt( type );
    pipeSendString( name );
    return pipeReceive();
}




void  _VOVend( status )
    int status;
    /*
     * Ends the current transition.
     * Do not wait for any reply.
     */
{
    if ( libActive(0) )  {
	pipeSendString( "END" );
	pipeSendInt( status );
	pipeClose();
    }
}

void  VOVend( status )
    int status;
    /*
     * Ends the current transition.
     */
{
#ifdef WAIT_TAKES_INT_STAR
    int s;
#else
    union wait s;
#endif
    int w;

    if ( libActive( 0 ) ) {
	if ( channel_started_by_me ) {
	    fflush( stdout );
	    fflush( stderr );
	    _VOVend( status );
	    do {
#ifdef USE_WAITPID
	      if ((w = waitpid((pid_t) -1, &s, 0)) == -1)
		perror("waitpid");
#else
		w = wait3( &s, 0, 0 );
#endif /* USE_WAITPID */
	    } while ( w!= channelPid );
	    exit( status );		/*  */
	} else {
	    _exit( status );		/* This leaves all descriptors open. */
	}
    }
    exit( status );
}


/* Utility function, good for replacement of most fopen() calls. */
FILE* VOVfopen( file, mode )
    char* file;
    char* mode;
{
    FILE* fp;

    if ( strcmp( mode,  "w" ) == 0 ) {
	VOVoutput( VOV_UNIX_FILE, file );
    }

    fp = fopen( file, mode );

    if ( strcmp( mode,  "r" ) == 0 && fp != NIL(FILE) ) {
	VOVinput( VOV_UNIX_FILE, file );
    }
    if ( strcmp( mode,  "a" ) == 0 ) {
	if ( fp != NIL(FILE) ) {
	    VOVinput( VOV_UNIX_FILE, file );
	}
	VOVoutput( VOV_UNIX_FILE, file );
    }
    return fp;
}


/* */
/*
  Suspend and restart retracing with these two routines.
*/
void VOVsuspendTracing()
{
}

void VOVrestoreTracing()
{
}



