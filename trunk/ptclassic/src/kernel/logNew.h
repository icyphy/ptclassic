/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 1/7/92

Macros for use with the logging memory allocator.  Use LOG_NEW
and LOG_DEL before calls to new and delete, respectively, in .cc
files.  Use INC_LOG_NEW and INC_LOG_DEL in .h files.

If the symbol MEMORYLOG is not defined, these macros are dummies.

It is possible to do a "mixed" version of Ptolemy: some files compiled
with this symbol on, others not.  If you do this, and logNew.cc was
built with logging on, then all allocation from files built with logging
off will be reported as being file "Unknown", line 0.

If MEMORYLOG is defined, all files must begin with a line of the form

static const char file_id[] = "this-file-name.cc";

before any include files are included.

**************************************************************************/
#ifndef _logNew_h
#define _logNew_h 1

#ifdef MEMORYLOG
// if we get here, memory logging is enabled.
#define LOG_NEW logNew(file_id,__LINE__)
#define LOG_DEL logDel(file_id,__LINE__)
#define INC_LOG_NEW incLogNew(file_id,__FILE__,__LINE__)
#define INC_LOG_DEL incLogDel(file_id,__FILE__,__LINE__)

void logNew(const char*,int);
void logDel(const char*,int);
void incLogNew(const char*,const char*,int);
void incLogDel(const char*,const char*,int);
#else // !MEMORYLOG
// if we get here, there is no memory logging and these macros do nothing.
#define LOG_NEW
#define LOG_DEL
#define INC_LOG_NEW
#define INC_LOG_DEL
#endif

#endif
