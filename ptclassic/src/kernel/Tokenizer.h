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

#ifndef _Tokenizer_h
#define _Tokenizer_h 1

#include <stream.h>
#include <string.h>

const int WORDBUF_SIZE = 256;

class Tokenizer {
private:
	char *special;
	char *whitespace;
	istream *strm;
	char comment_char;
	char quote_char;
	char escape_char;
	char c;				// last char read

public:
	Tokenizer(istream& input,char *spec);
	Tokenizer();
	Tokenizer& operator >> (char * s);
	int eof() {return strm->eof();}
	void flush();
};

// for some reason, g++ has trouble when these are included in the
// class definition: complains about stray \ characters.

inline	Tokenizer::Tokenizer(istream& input,char *spec="()") {
	special=spec;
	strm=&input;
	whitespace= " \n\t";
	comment_char = '#';
	quote_char = '\"';
	escape_char = '\\';
}

inline Tokenizer::Tokenizer() {
	special="()";
	strm=cin;
	whitespace=" \n\t";
	comment_char = '#';
	quote_char = '\"';
	escape_char = '\\';
}
#endif

		
