static const char file_id[] = "dist_debug.cc";

#include "distributions.h"
#include "Error.h"
#include "type.h"
#include "streamCompat.h"
#include <std.h>

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
							COPYRIGHTENDKEY

 Programmer:  Soonhoi Ha

 Methods for distributions.  If a user wants to add a new type, he/she
 must adjust "NUM" and "defType".

**************************************************************************/

int DistGeneral :: makeTable() {

	int fi;

	// read the table
	if ((fi = open(name, 0)) < 0) {
		Error::abortRun("cannot open the table file (", name, ").\n");
		return FALSE;
	}

	ifstream in(fi);
	in >> size;
#if defined(__GNUG__) && __GNUG__ == 1
	LOG_DEL; delete [size+1] table;
#else
	LOG_DEL; delete [] table;
#endif
	LOG_NEW; table = new DistTable[size+1];

	int num = 0;
	while(in.good()) {
		int index;
		double pi;
		in >> index >> pi;
		table[num].index = index;
		table[num].value = pi;
		num++;
	}

	return TRUE;
}		

