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

**************************************************************************/
#ifndef _logNew_h
#define _logNew_h 1

#define REGISTER_FILE static const char file_id[] = __FILE__
#define LOG_NEW logNew(file_id,__LINE__)
#define LOG_DEL logDel(file_id,__LINE__)
#define INC_LOG_NEW incLogNew(file_id,__FILE__,__LINE__)
#define INC_LOG_DEL incLogDel(file_id,__FILE__,__LINE__)

void logNew(const char*,int);
void logDel(const char*,int);
void incLogNew(const char*,const char*,int);
void incLogDel(const char*,const char*,int);

#endif
