#ifndef _pt_fstream_h
#define _pt_fstream_h 1

#ifdef __GNUG__
#pragma interface
#endif
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
// Programmer:  J. Buck
// $Id$

// This defines derived types of ifstream and ofstream that do the following:

// 1. expand the file name using expandFileName
// 2. report file-open errors using Ptolemy's error-reporting functions.
//    The system error value (from "errno") is reported as part of the error
//    message and Error::abortRun is called on an open failure.
// 3. The special file names "<cin>", "<cout>" and "<cerr>" 
//    (and <stdin>, <stdout> ,<stderr>) are recognized.
//
// Otherwise they are the same as their baseclasses.

#include <fstream.h>

class pt_ifstream : public ifstream {
public:
	pt_ifstream() : ifstream() {}
	pt_ifstream(int fd) : ifstream(fd) {}
	pt_ifstream(const char *name, int mode=ios::in, int prot=0664);
	void open(const char *name, int mode=ios::in, int prot=0664);
};

class pt_ofstream : public ofstream {
public:
	pt_ofstream() : ofstream() {}
	pt_ofstream(int fd) : ofstream(fd) {}
	pt_ofstream(const char *name, int mode=ios::out, int prot=0664);
	void open(const char *name, int mode=ios::out, int prot=0664);
};

#endif
