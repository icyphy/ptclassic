/*******************************************************************
SCCS version identification
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer: J. T. Buck
 Modified by Alan Kamas to use a Tcl interpreter in the main loop

 Main program for pigiRpc.

********************************************************************/
#include <string.h>

extern ptkConsoleWindow();

extern "C" {
	int KcInitLog(const char*);
	void KcCatchSignals();
	void CompileInit();
	void KcLoadInit(const char*);

	int ptkRPCInit( int argc, char **argv);
	int ptkMainLoop ();

};

extern const char* pigiFilename;

main (int argc, char ** argv)
{
	pigiFilename = argv[0];
	KcLoadInit(argv[0]);
	KcInitLog("pigiLog.pt");
	CompileInit();
	KcCatchSignals();

	/* Strip off end of argv, not front */
	int doConsole = 0;
	if (argc>=2 && strcmp(argv[argc-1], "-console")==0) {
		doConsole = 1;
		--argc;
	}

	ptkRPCInit (argc, argv);
	if ( doConsole )  ptkConsoleWindow();
	ptkMainLoop();
	return 0;
}
