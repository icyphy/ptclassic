/******************************************************************
Version identification:
@(#)spin_init2.c	1.00    07/26/98

Copyright (c) 1990-1998 The Regents of the University of California.
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

 Programmer:  Shang-Pin Chang
 Date of creation: 11/2/97 

 Definition of spin_init2 (initialize tl_* variables; decouple those in spin.h).

*******************************************************************/

#include "tl.h"

typedef struct Transition {
	Symbol *name;
	Node *cond;
	int redundant;
	struct Transition *nxt;
} Transition;

typedef struct State {
	Symbol	*name;
	Transition *trans;
	Graph	*colors;
	unsigned char redundant;
	unsigned char accepting;
	unsigned char reachable;
	struct State *nxt;
} State;

typedef struct Cache {
	Node *before;
	Node *after;
	int same;
	struct Cache *nxt;
} Cache;

extern Mapping	*Mapped;
extern Graph	*Nodes_Set;
extern Graph	*Nodes_Stack;
extern State *never;
extern Cache	*stored;
extern Node	*can;

void spin_init2(void)
{
Mapped = (Mapping *) 0;
Nodes_Set = (Graph *) 0;
Nodes_Stack = (Graph *) 0;
never = (State *) 0;
stored = (Cache *) 0;
can = ZN;
}

