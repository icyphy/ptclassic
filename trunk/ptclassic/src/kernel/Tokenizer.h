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
	istream *savestrm;
	char comment_char;
	char quote_char;
	char escape_char;
	char c;				// last char read
	get();

public:
	// Open a file and read tokens from it
	int fromFile(const char* name);

	// return true if reading from a file
	int readingFromFile() { return savestrm ? 1 : 0;}

	// constructors
	Tokenizer(istream& input,char *spec);
	Tokenizer();

	// get next token
	Tokenizer& operator >> (char * s);

	// EOF check
	int eof() {return strm->eof();}

	// discard current line, or close file (error cleanup)
	void flush();
};

// for some reason, g++ has trouble when these are included in the
// class definition: complains about stray \ characters.

inline	Tokenizer::Tokenizer(istream& input,char *spec="()") {
	special=spec;
	strm=&input;
	savestrm=NULL;
	whitespace= " \n\t";
	comment_char = '#';
	quote_char = '\"';
	escape_char = '\\';
}

inline Tokenizer::Tokenizer() {
	special="()";
	strm=cin;
	savestrm=NULL;
	whitespace=" \n\t";
	comment_char = '#';
	quote_char = '\"';
	escape_char = '\\';
}
#endif

		
