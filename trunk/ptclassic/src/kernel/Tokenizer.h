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

Tokenizers can also parse strings in memory.  To do this, use
a constructor like

Tokenizer lexer("x+2.0*y/(z-23)", "*+-/()");

Here, successive "lexer >> tokenbuf" operations will return
x, +, 2.0, *, y, /, (, z, -, 23, and ).  Subsequent calls will
return strings of length zero (the first character of tokenbuf
will be set to 0).

Tokenizers can read process include files; the fromFile method
is used.
*******************************************************************/

#ifndef _Tokenizer_h
#define _Tokenizer_h 1
#ifdef __GNUG__
#pragma interface
#endif

const int WORDBUF_SIZE = 256;

class TokenContext;
class istream;

class Tokenizer {
private:
	const char *special;	// list of one-character tokens
	const char *whitespace;	// list of whitespace characters
	istream *strm;		// associated input stream
	TokenContext* stack;	// stack for include files
	char* curfile;		// current input file name
	short depth;		// depth of nesting
	short myStrm;		// true if strm needs deletion at end
	int line_num;		// current line number
	char comment_char;	// character for comments
	char quote_char;	// character for quoted strings
	char escape_char;	// behave like the " \ " in C
	char c;			// last char read
	char ungot;

	// this is the common part of all constructors
	void init ();

	// this method reads a character from the stream into c
	int get();

	// used to include files from other files and get back
	void push(istream* s,const char* f);
	void pop();

public:
	// the "default whitespace characters" string (it includes
	// space, newline and tab)
	static const char *defWhite;

	// Status info funcs
	const char* current_file() const { return curfile;}
	int current_line() const { return line_num;}

	// Open a file and read tokens from it
	int fromFile(const char* name);

	// return true if reading from a file
	int readingFromFile() const { return depth > 0 ? 1 : 0;}

	// constructors
	Tokenizer(istream& input,const char* spec,const char* w = defWhite);
	Tokenizer(const char* buffer,const char* spec,const char* w = defWhite);
	Tokenizer();

	// destructor
	~Tokenizer();

	// get next token
	Tokenizer& operator >> (char * s);

	// EOF check
	int eof() const;

	// Cast to void*, by analogy with streams, so we can say
	// Tokenizer tin;
	// while (tin) {...}
	operator void* () { return eof() ? 0 : this; }

	// Skip whitespace (closing include file if no more space)
	void skipwhite();

	// discard current line, or close file (error cleanup)
	void flush();

	// change whitespace characters, return old ones.
	const char* setWhite(const char* w) {
		const char* t = whitespace;
		whitespace = w;
		return t;
	}

	// change special characters, return old ones.
	const char* setSpecial(const char* s) {
		const char* t = special;
		special = s;
		return t;
	}

	// change comment character, return old one.
	char setCommentChar(char n) {
		char o = comment_char;
		comment_char = n;
		return o;
	}
};

#endif

		
