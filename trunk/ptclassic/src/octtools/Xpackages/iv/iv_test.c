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
/* test the iv.a routines */

#include "copyright.h"
#include "port.h"
#include "iv.h"
#include "errtrap.h"

int
modifiedN()
{
    (void) printf("N has been modified\n");
    return(IV_OK);
}

static ivWhichItem itemPtr[] = {
    {"Simple", 0},
    {"Complex", 0},
    {"Net", 0},
    {"Pin", 1}
};

int
main()
{
    double 	a = 100.0, c = 12.67, d = 1999999e12; /* test variables */
    int		n = 13, b = 0;
    double 	aa = 120.1;
    XEvent 	theEvent;
    Display 	*display;
    char	*theString;
    int		exitFlag = 0;
    int		destroy = 0;
    int		destroyed = 0;
    int 	selected = 0;
    IVWindow 	*IVwin;
    IVWindow	*IVexitWindow;
    int enable = 0;
    int tog = 0;


    errCore( 1 );		/* Dump the core on error. */
    errProgramName( "Iv-test" );
    theString = (char *) calloc((unsigned) (20), sizeof(char));
    (void) strcpy(theString, "Test String");

    if ((display = XOpenDisplay( (char*)0 )) == (Display *) 0 ) {
	abort();
    }
  
    XSynchronize( display, True );
    IVwin = iv_InitWindow( display, "test","Test interactive variables", 12 );
    iv_AddVar(IVwin, "WhichOne", (int*)itemPtr, IV_WHICH,
	      (sizeof(itemPtr)/sizeof(ivWhichItem)) , IV_NULL_FUNCTION);
    iv_AddVar(IVwin, "Variable A", (int*)&a, IV_DOUBLE, 1, IV_NULL_FUNCTION);
    iv_AddVar(IVwin, "Variable C", (int*)&c, IV_DOUBLE, 2, IV_NULL_FUNCTION);
    iv_AddVar(IVwin, "Variable D", (int*)&d, IV_DOUBLE, 3, IV_NULL_FUNCTION);
    iv_AddVar(IVwin, "Integer N" , &n, IV_INT, IV_NO_OPT, modifiedN);
    iv_AddVar(IVwin, "Toggle var", &tog, IV_TOGGLE,
	      IV_NO_OPT, IV_NULL_FUNCTION);
    iv_AddVar(IVwin, "Boolean var", &b, IV_BOOLEAN,
	      IV_NO_OPT, IV_NULL_FUNCTION);
    iv_AddVar(IVwin, "string", (int*)&theString, IV_STRING,
	      IV_NO_OPT, IV_NULL_FUNCTION);
    iv_SetEnableFlag( IVwin, (int*)&c, 0);
    iv_SetEnableFlag( IVwin, (int*)&d, 0);

    iv_SetEnableFlag( IVwin, (int*)&b, 1);
    iv_SetEnableFlag( IVwin, (int*)&tog, 0);

    iv_MapWindow(IVwin, IV_NO_OPT, IV_NO_OPT, IV_MOUSE);

    IVexitWindow = iv_InitWindow( display, "test.dummy","Test another window", 6 );
    iv_AddVar(IVexitWindow, "Exit test", &exitFlag, IV_TOGGLE, 
	      IV_NO_OPT, IV_NULL_FUNCTION);
    iv_AddVar(IVexitWindow, "Enable C", &enable, IV_BOOLEAN,
	      IV_NO_OPT, IV_NULL_FUNCTION);
    iv_AddVar(IVexitWindow, "Destroy other win", &destroy, IV_TOGGLE,
	      IV_NO_OPT, IV_NULL_FUNCTION);
    iv_MapWindow( IVexitWindow, 200 , 300 , IV_NO_OPT );

    while (!exitFlag) {
	static int enC = 0;
	XNextEvent( display, &theEvent);
	if (iv_HandleEvent( &theEvent ) != IV_OK) {
	    (void) printf("Extraneous event\n");
	}
	if ( enC != enable ) {
	    enC = enable;
	    iv_SetEnableFlag( IVwin, (int*)&c, enable );
	}
	if ( tog == 1 ) {
	    tog = 0;

	    iv_SetEnableFlag( IVwin, (int*)&b, 1);
	    iv_SetEnableFlag( IVwin, (int*)&tog, 0);
	}
	if ( b == 1 ) {
	    b = 0;

	    iv_SetEnableFlag( IVwin, (int*)&b, 0);
	    iv_SetEnableFlag( IVwin, (int*)&tog, 1);
	}
    }

    iv_AddVar(IVwin, "Another variable AA", (int*)&aa, IV_DOUBLE, IV_NO_OPT, IV_NULL_FUNCTION);
    iv_SetEnableFlag( IVwin, (int*)&c, 1 );
    iv_MapWindow(IVwin, IV_NO_OPT, IV_NO_OPT, IV_NO_OPT);
    a = 9.6;
    d = 9.9;
    iv_UpdateVar(IVwin, (int*)IV_NO_OPT);
    iv_PromptVar(IVwin, &n, 1);
    iv_SetEraseFlag( IVwin, 1);
    c = 6.9;
    iv_UpdateVar(IVwin, (int*)&c);
    exitFlag = 0;
    while ( !exitFlag ) {
	if ( destroy ) {
	    if ( ! destroyed ) {
		printf( "Destroying other window" );
		iv_FreeWindow( IVwin );
		IVwin = 0;
		destroyed = 1;
	    }
	}
	iv_ProcessAllEvents( display );
    }
    if ( ! destroyed ) {
	selected = iv_WhichSelect( IVwin, itemPtr );
	iv_FreeWindow( IVwin );
    }
    (void) printf("Selected : %d\n", selected);
    (void) printf("Variable A: %f\n", a);
    (void) printf("Variable C: %f\n", c);
    (void) printf("Variable D: %f\n", d);
    (void) printf("Integer N: %d\n", n);
    (void) printf("Boolean var: %d\n", b);
    (void) printf("string:'%s'\n", theString);
    (void) printf("exit flag: %d\n", exitFlag); 
    return 0;
}

