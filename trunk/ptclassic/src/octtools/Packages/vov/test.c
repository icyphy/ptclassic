#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
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
#include "vov.h"
#include "errtrap.h"

int otherTest();

int
main(argc, argv)
    int argc;
    char** argv;
{
    errProgramName( "vov-test" );
    VOVbegin( argc, argv );
    VOVinput( VOV_UNIX_FILE, "fin1" );
    VOVinput( VOV_UNIX_FILE, "fin1" );
    VOVinput( VOV_UNIX_FILE, "fin1" );
    VOVinput( VOV_UNIX_FILE, "fin1" );
    VOVoutput( VOV_UNIX_FILE, "/usr/tmp/foo1" );
    VOVoutput( VOV_UNIX_FILE, "/usr/tmp/foo2" );
    VOVoutput( VOV_UNIX_FILE, "/usr/tmp/foo3" );
    VOVoutput( VOV_UNIX_FILE, "/usr/tmp/foo4" );
    VOVoutput( VOV_UNIX_FILE, "/usr/tmp/foo1" );
    VOVoutput( VOV_UNIX_FILE, "/usr/tmp/foo1" );
    printf( "Sleeping 2 s\n" );
    sleep( 2 );

    if ( argc == 1 ) {
	system( "vov-test 0" );
	printf( "system call succeeded" );
    }

    VOVdelete( VOV_UNIX_FILE, "/usr/tmp/foo1" );

    otherTest();

    VOVend( 0 );
    return 0;
}


int otherTest()
{
    FILE *fp;


    (void) VOVoutput(VOV_UNIX_FILE, "A1");  /* polarBear */
    fp = fopen ("A1", "w");
    (void) fprintf (fp, "yabba dabba do\n");
    (void) fclose (fp);

    (void) VOVinput(VOV_UNIX_FILE, "A1");   /* koalaBear */
    fp = fopen ("A1", "r");
    (void) fclose (fp);

    (void) VOVoutput(VOV_UNIX_FILE, "A2");  /* koalaBear */
    fp = fopen ("A2", "w");
    (void) fprintf (fp, "apple jack\n");
    (void) fclose (fp);

    (void) VOVdelete(VOV_UNIX_FILE, "A1");

    (void) VOVinput(VOV_UNIX_FILE, "A2");   /* polarBear */
    fp = fopen ("A2", "r");
    (void) fclose (fp);

    (void) VOVdelete(VOV_UNIX_FILE, "A2");

    return 0;
}


