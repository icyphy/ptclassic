/*******************************************************************
SCCS Version identification :
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
