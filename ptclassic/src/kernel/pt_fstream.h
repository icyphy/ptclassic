#ifndef _pt_fstream_h
#define _pt_fstream_h 1

#ifdef __GNUG__
#pragma interface
#endif
// Copyright (c) 1992 The Regents of the University of California.
//                       All Rights Reserved.
// Programmer:  J. Buck
// $Id$

// This defines derived types of ifstream and ofstream that do the following:

// 1. expand the file name using expandFileName
// 2. report file-open errors using Ptolemy's error-reporting functions.
// The system error value (from "errno") is reported as part of the error
// message and Error::abortRun is called on an open failure.
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
