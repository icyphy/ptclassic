/*******************************************************************
SCCS Version identification :
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck
 Date of creation: 4/14/91

This header file defines the interface for error reporting.  It
is designed to support multiple implementations: one for output
on terminals, one that pops up error windows, etc.

*******************************************************************/
#ifndef _Error_h
#define _Error_h 1
class NamedObj;

class Error {
public:
// this pair tells the schedulers to halt, adds a prefix "ERROR: ".
	static void abortRun (const char*, const char* = 0, const char* = 0);
	static void abortRun (const NamedObj&, const char*, const char* = 0,
			      const char* = 0);
// this pair does not tell the schedulers to halt, prefix "ERROR: ".
	static void error (const char*, const char* = 0, const char* = 0);
	static void error (const NamedObj&, const char*, const char* = 0,
			   const char* = 0);
// this pair does not tell the schedulers to halt, prefix "WARNING: ".
// there may be other differences as well.
	static void warn (const char*, const char* = 0, const char* = 0);
	static void warn (const NamedObj&, const char*, const char* = 0,
			  const char* = 0);
// this pair has no prefix at all.
	static void message (const char*, const char* = 0, const char* = 0);

	static void message (const NamedObj&, const char*, const char* = 0,
			     const char* = 0);

	static int canMark();
	static void mark (const NamedObj&);
};
#endif
