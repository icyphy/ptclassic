static const char file_id[] = "Tokenizer.cc";
/******************************************************************
Version identification:
@(#)Tokenizer.cc	1.30	3/28/96

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer:  J. T. Buck
 Date of creation: 3/18/90

The Tokenizer class

A Tokenizer has an associated istream, and two character strings:
whitespace, listing characters to be ignored, and specialops,
listing characters that turn into single-character tokens.  Other
characters turn into multi-character tokens.  If tok is a tokenizer,
tok >> s, where s is a char *, copies the next token to tok.

The tokenizer can also switch to reading files on request.
There is an until-end-of-line comment character (default #)
and a quote character for strings.
*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Tokenizer.h"
#include <std.h>
#include <stdio.h>
#include "miscFuncs.h"
#include <fstream.h>
#include <strstream.h>

// older versions of the GNU libg++ library have a bug; this
// turns on the bug fix code

#ifdef __GNUG__
#if !defined(__GNUC_MINOR__) || __GNUC_MINOR__ < 7
#define NEED_DONT_CLOSE
#endif
#endif

// The default whitespace string
const char *Tokenizer::defWhite = " \t\n\r";

// Constructors
// for some reason, g++ has trouble when these are included in the
// class definition: complains about stray \ characters.

// Common part of constructors
void
Tokenizer::init() {
	stack = NULL;
	depth = 0;
	ungot = 0;
	curfile = 0;
	line_num = 1;
	comment_char = '#';
	quote_char = '\"';
	escape_char = '\\';

	pb_buffer.ptr = NULL;
	pb_buffer.index = pb_buffer.size = 0;
}

// This one reads from a file
Tokenizer::Tokenizer(istream& input,const char *spec, const char* w) {
	special = spec;
	strm = &input;
	myStrm = 0;
	whitespace = w;
	init ();
}

// This one reads from a text buffer
Tokenizer::Tokenizer(const char* buffer,const char* spec,const char* w) {
	special = spec;
	whitespace = w;
// istream constructor calls the second argument "char" even though it
// is not changed.  A cast to get around this.
	char* p = (char*) buffer;
	LOG_NEW; strm = new istrstream(p, strlen(p));

#ifdef NEED_DONT_CLOSE
// work around memory leak with libg++-2.2
// FIXME: remove when library is fixed.
	strm->unsetf(ios::dont_close);
#endif

	myStrm = 1;
	init ();
}

// This one reads from stdin (cin)
Tokenizer::Tokenizer() {
	special = "()";
	strm = &cin;
	myStrm = 0;
	whitespace = defWhite;
	init ();
}

// This class saves contexts: the stream, and the name of the
// file being read from

struct TokenContext {
	char* filename;
	istream* savestrm;
	int line_num;
	TokenContext* link;
	TokenContext(char* f,istream* s,int ln, TokenContext* l)
		: filename(f), savestrm(s), line_num(ln), link(l) {}
};

// Destructor
Tokenizer::~Tokenizer() {
	while (depth) pop();
	if (myStrm) {
		LOG_DEL; delete strm;
	}
	if (pb_buffer.ptr)
		free(pb_buffer.ptr);
}

// Function to interpret escaped characters in strings
static int slash_interp(char c) {
	switch (c) {
	case 'n':	return '\n';
	case 't':	return '\t';
	case '0':	return 0;
	case 'e':	return 033;
	case 'b':	return 07;
	case 'r':	return '\r';
	default:	return c;
	}
}

void
Tokenizer::push(istream* s, const char* f) {
// save existing context on stack
	LOG_NEW; stack = new TokenContext(curfile, strm, line_num, stack);
// save filename in dynamic memory, and set curfile to it.
	curfile = savestring(f);
	strm = s;
	line_num = 1;
	depth++;
}

// close include file and return to old state
void
Tokenizer::pop() {
	if (depth == 0) return;
	TokenContext* t = stack;
	LOG_DEL; delete strm;
	LOG_DEL; delete [] curfile;
	strm = t->savestrm;
	curfile = t->filename;
	line_num = t->line_num;
	stack = t->link;
	LOG_DEL; delete t;
	depth--;
}

// Open a new file.  Return TRUE for success, FALSE for failure.

int
Tokenizer::fromFile(const char *filename) {
    char* expandedFileName = expandPathName(filename);
    LOG_NEW; ifstream* s = new ifstream(expandedFileName);
    LOG_DEL; delete [] expandedFileName;
    if (s == 0) {
	return FALSE;
    }
    if (*s == 0) {
	LOG_DEL; delete s;
	return FALSE;
    }
    // Push stream pointer and copy of original filename on stack
    push(s, filename);
    return TRUE;
}

// Get the next character from the tokenizer.  Return 0 on eof, else 1.
int
Tokenizer::get() {
// Get next character into c.
	if (ungot) {
		c = ungot;
		ungot = 0;
		return 1;
	}
// check pushback buffer
	if (pb_buffer.index > 0) {
		c = pb_buffer.ptr[--pb_buffer.index];
		return 1;
	}

// Attempt to read a character.
	while (1) {
		strm->get(c);
		if (*strm) {
			if (c == '\n') line_num++;
			return 1;
		}
		if (depth == 0) break;
		// Just the end of an include file: pop up a level
		pop();
	}
// It's really EOF.
// We return \n on an EOF so the previous token will be terminated.
	c = '\n';
	return 0;
}

// skipwhite -- skip past any whitespace in the stream.  If we hit
// eof and are in an include file, this will have the effect of popping
// out.  This is to allow the interpreter to print prompts at the right
// time.
void
Tokenizer::skipwhite () {
	if (depth == 0) return;
	while (depth) {
		get();
// skip line if comment character
		if (c == comment_char) {
			do { get(); } while (c != '\n');
		}
		else if (strchr (whitespace, c) == NULL) break;
	}
// put back last character
	ungot = c;
	return;
}

// clearwhite -- Since the function "skipwhite" does not adequately
// skip over blanks, etc. in the top level file, this new function
// will.  Note that I could not change "skipwhite" as it is used by
// the interpreter.cc code.  - Alan Kamas, 6/17/94
void
Tokenizer::clearwhite () {
top:	
        while (get()) {
                if (!strchr(whitespace, c)) break;
        }
// Quit if eof
	if (strm->eof() && (pb_buffer.index == 0)) {
                return;
        }
// if c is the comment character, dump the line and start over
        if (c == comment_char) {
                do { get(); } while (c != '\n');
                goto top;
        }
// put back last character
	ungot = c;
	return;
}

// EOF check
int
Tokenizer::eof () const {
	return (depth>0) ? 0 : (strm->eof() && (pb_buffer.index == 0));
}

// get the next token.
// We allow tok >> w1 >> w2.
Tokenizer&
Tokenizer::operator >> (char *s) {
// note: we always read into c, so it will save the last character read
// skip whitespace, if any
	clearwhite();
	// Return null token on eof
	if (strm->eof() && (pb_buffer.index == 0)) {
		*s = 0;
		return *this;
	}
// check the first character after the whitespace
	get();
// special case: first character is special.
	if (c == quote_char) {
		while (get()) {
			if (c == escape_char) {
				get();
				*s++ = slash_interp(c);
			}
			else if (c == quote_char || (strm->eof() && (pb_buffer.index == 0)))
				break;
			else *s++ = c;
		}
		*s = 0;
		return *this;
	}
	*s++ = c;
	if (strchr(special, c)) {
		*s = 0;
		return *this;
	}
// normal case: accumulate text as long as characters are normal.
	while (get()) {
		if (strchr(whitespace, c))
			break;
		if (strchr(special, c) || c == comment_char || c == quote_char) {
			ungot=c;
			break;
		}
		*s++ = c;
	}
	*s = 0;
	return *this;
}

// push back a token;
// The token is saved at the end of the pushback buffer after a
// whitespace character, so that it is later reparsed if the
// special character set changes.

void
Tokenizer::pushBack(const char* s)
{
	int toklen = strlen(s)+1;
	// save ungot character, if any
	if (ungot) toklen++;

	if (pb_buffer.index + toklen >= pb_buffer.size) {
		pb_buffer.size += (toklen > 16) ? toklen : 16;
		pb_buffer.ptr = (char*)
			(pb_buffer.ptr ? realloc(pb_buffer.ptr, pb_buffer.size)
				       : malloc(pb_buffer.size));
	}
	char* pb_ptr = &pb_buffer.ptr[pb_buffer.index];

	if (ungot) {
		*pb_ptr++ = ungot,  ungot = 0;
		pb_buffer.index++, toklen--;
	}
	if (*whitespace) *pb_ptr++ = *whitespace;

	// push characters in reversed order
	s += strlen(s);
	for (int i=0; i < toklen-1; i++)
		*pb_ptr++ = *--s;

	pb_buffer.index += toklen;
}

// this function discards the remainder of a line, if we're in the middle
// of processing one.  (reads until c is \n).
// note that we do do ungets, so c isn't always the last char read,
// but we never unget \n so this isn't a problem.
// If we're in a subfile, we close it.
void
Tokenizer::flush() {
	while (depth) pop ();
	while (c != '\n') get();
}

char
Tokenizer::peekAtNextChar() {
	char next = 0;
	if (get()) ungot = next = c;
	return next;
}
