/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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
#include "Tokenizer.h"
#include <std.h>
#include <stdio.h>
#include <stream.h>
#include "miscFuncs.h"

// The default whitespace string
const char Tokenizer::defWhite[] = " \t\n\r";

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
}

// This one reads from a file
Tokenizer::Tokenizer(istream& input,const char *spec, const char* w) {
	special = spec;
	strm = &input;
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
	strm = new istream(strlen(p), p);
	init ();
}

// This one reads from stdin (cin)
Tokenizer::Tokenizer() {
	special = "()";
	strm = &cin;
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
Tokenizer::push(istream* s,const char* f) {
// save existing context on stack
	stack = new TokenContext(curfile,strm,line_num,stack);
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
	delete strm;
	delete curfile;
	strm = t->savestrm;
	curfile = t->filename;
	line_num = t->line_num;
	stack = t->link;
	delete t;
	depth--;
}

// Open a new file.
// We use stdio to open the file to avoid error messages.  Return 1
// for success, 0 for failure.
int
Tokenizer::fromFile(const char *filename) {
	int fd = open(expandPathName(filename), 0);
	if (fd < 0) return 0;
	push (new istream(fd),filename);
	return 1;
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

// EOF check
int
Tokenizer::eof () const {
	return (depth>0) ? 0 : strm->eof();
}

// get the next token.
// We allow tok >> w1 >> w2.
Tokenizer&
Tokenizer::operator >> (char *s) {
// note: we always read into c, so it will save the last character read
// skip whitespace, if any
top:
	while (get()) {
		if (!strchr (whitespace, c)) break;
	}
// Return null token on eof
	if (strm->eof()) {
		*s = 0;
		return *this;
	}
// if c is the comment character, dump the line and start over
	if (c == comment_char) {
		do { get(); } while (c != '\n');
		goto top;
// if c is the quote character, grab text until closequote
	}
	else if (c == quote_char) {
		while (get()) {
			if (c == escape_char) {
				get();
				*s++ = slash_interp(c);
			}
			else if (c == quote_char || strm->eof())
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
