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

*******************************************************************/
#include "Tokenizer.h"

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

Tokenizer&
Tokenizer::operator >> (char *s) {
// note: we always read into c, so it will save the last character read
// skip whitespace, if any
top:
	if (strm->eof()) {
		*s = 0;
		return *this;
	}
	do {
		strm->get(c);
	} while (strchr (whitespace, c));
// if c is the comment character, dump the line and start over
	if (c == comment_char) {
		do { strm->get(c); } while (c != '\n');
		goto top;
// if c is the quote character, grab text until closequote
	}
	else if (c == quote_char) {
		while (1) {
			strm->get(c);
			if (c == escape_char) {
				strm->get(c);
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
	while (1) {
		strm->get(c);
		if (strchr(whitespace, c) || strm->eof())
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
void
Tokenizer::flush() {
	while (c != '\n') strm->get(c);
}
