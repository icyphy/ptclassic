static const char file_id[] = "StringState.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include <std.h>
#include "StringState.h"
#include "miscFuncs.h"

/**************************************************************************
Version identification:
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
							COPYRIGHTENDKEY

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

StringState :: StringState() : val(0) {}

const char* StringState :: className() const {return "StringState";}

const char* StringState :: type() const { return "STRING";}

void StringState  :: initialize() {
	char buf[2048];
	LOG_DEL; delete []val;
	const char* p = initValue();
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
				parseError ("undefined symbol: ", statename);
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
			p++;	// skip the backslash
			if (*p) *q++ = *p++;
		}
		else *q++ = *p++;
	}
	*q = 0;
	val = savestring (buf);
}

StringState& StringState :: operator=(const char* newStr) {
	LOG_DEL; delete []val;
	val = savestring(newStr);
	return *this;
}

StringState& StringState :: operator=(StringList& newStrList) {
	// Strictly speaking this is not required, but I dont
	// really trust two much automatic type conversion.
	// Note that we might modify newStrList in the call below
	const char *newStr = newStrList;
	return (*this = newStr);
}

StringState& StringState :: operator=(const State& newState) {
	// Strictly speaking this is not required, but I dont
	// really trust two much automatic type conversion.
	StringList newStrList = newState.currentValue();
	return (*this = newStrList);
}

StringList StringState :: currentValue() const {
	StringList res;
	if (val) res = val;
	return res;
}

State* StringState :: clone() const {
	LOG_NEW; return new StringState;
}

StringState :: ~StringState() {
	LOG_DEL; delete []val;
}

// make known state entry
static StringState proto;
static KnownState entry(proto,"STRING");

ISA_FUNC(StringState,State);
