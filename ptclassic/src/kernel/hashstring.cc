static const char file_id[] = "hashstring.cc";
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

 Programmer:  J. T. Buck

This module keeps a hash table of strings.

**************************************************************************/
const int HASH_TABLE_SIZE = 601;
#include "logNew.h"

extern "C" int strcmp(const char*,const char*);
extern "C" char *strcpy(char*,const char*);

struct hent {
	char *txt;
	struct hent *link;
	// null constructor: we rely on static variables starting at zero,
	// since hashstring may get called before table is constructed.
	hent() {}
	// this one is used to initialize extra hash entries on chains.
	hent(char* c) : txt(c), link(0) {}
	~hent();
};

// destructor: recursive
hent::~hent() {
	LOG_DEL; delete [] txt;
	LOG_DEL; delete link;
}

static struct hent htable[HASH_TABLE_SIZE];
static const char* empty = "";

// faster compare: args are constrained to be nonempty.
// saves time because most compares fail on first char.
inline int streq(const char* p,const char* q) {
	return *p == *q && strcmp(p+1,q+1) == 0;
}

int hs_calls = 0;
int hs_entries = 0;

const char * hashstring (const char* text) {
	hs_calls++;
	if (!text || !*text) return empty;

	register struct hent *p;
	register const char *s = text;
	register int h;
	int len;

/* Compute hash function */
	for (h = 0; *s; s++) h += *s;
	p = htable + h % HASH_TABLE_SIZE;
/* Search down list */
	while (p->txt && !streq(text, p->txt))
		p = p->link;
	if (p->txt == 0) {
/* Allocate a new string */
		len = s - text;
		LOG_NEW; p->txt = new char[len+1];
		strcpy (p->txt, text);
		LOG_NEW; p->link = new hent(0);
		hs_entries++;
	}
	return p->txt;
}

