#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "StringState.h"
#include "miscFuncs.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  I. Kuroda and J. Buck
 Date of creation: 5/26/90
 Revisions:
	Support value substitution in StringStates, with the syntax
	state foo = {foo}

	State names within braces are replaced by their values.

 Functions for class StringState

**************************************************************************/


/*************************************************************************

	class StringState methods

**************************************************************************/


void StringState  :: initialize() {
	char buf[2048];
	// cast is because cfront doesn't want to delete pointers to const
	delete (char*)val;
	const char* p = initValue;
	char *q = buf;
	while (*p) {
		if (*p == '{') {
			p++;
			// a statename: {name}
			// blanks inside are allowed: { name }, say.
			char statename[128], *s = statename;
			while (*p && *p != '}') {
				if (*p == ' ') p++;
				else *s++ = *p++;
			}
			if (*p == 0) {
				parseError ("missing '}' character");
				return;
			}
			*s = 0;
			p++;	// skip the '}' character
			const State* st = lookup(statename, parent()->parent());
			if (!st) {
				parseError ("undefined symbol", statename);
				return;
			}
			StringList sl = st->currentValue();
			const char* sltext = sl;
			if (sltext) {	// check if non-null string
				int l = strlen (sltext);
				strcpy (q, sltext);
				q += l;
			}
		}
		else if (*p == '\\') {
			*q++ = *p++;
			if (*p) *q++ = *p++;
		}
		else *q++ = *p++;
	}
	*q = 0;
	val = savestring (buf);
}

StringList StringState :: currentValue() const {
	StringList res;
	if (val) res = val;
	return res;
}

State* StringState :: clone() const {
	return new StringState;
}

// make known state entry
static StringState proto;
static KnownState entry(proto,"STRING");

ISA_FUNC(StringState,State);
