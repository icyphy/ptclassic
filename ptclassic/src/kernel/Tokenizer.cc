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

static slash_interp(char c) {
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

// Open a new file.
// We use stdio to open the file to avoid error messages.  Return 1
// for success, 0 for failure.
Tokenizer::fromFile(const char *filename) {
	if (savestrm) return 0;
	FILE *stdstrm = fopen (filename, "r");
	if (stdstrm == NULL) return 0;
	savestrm = strm;
	strm = new istream(stdstrm);
	return 1;
}

// Get the next character from the tokenizer.
Tokenizer::get() {
// Get next character into c.
// On EOF, pop up if we were within a file.
	if (strm->eof() && savestrm) {
		strm = savestrm;
		savestrm = 0;
	}
// We return \n on an EOF so the previous token will be terminated.
	if (strm->eof()) {
		c = '\n';
		return 0;
	}
	else {
		strm->get(c);
		return 1;
	}
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
			strm->unget(c);
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
	if (savestrm) {
		delete strm;	// close file
		strm = savestrm;
		savestrm = 0;
	}
	while (c != '\n') get();
}
