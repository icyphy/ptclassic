/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 Baseclass for all single-processor code generation targets.

*******************************************************************/

#ifndef _CGTarget_h
#define  _CGTarget_h 1
#include "Target.h"

class CGTarget : public Target {
protected:
	StringList myCode;
public:
	CGTarget(const char* name, const char* starclass, const char* desc);
	void initialize();
	void start();
	void wrapup();
	Block* clone() const;
	void addCode(const char*);
	virtual void headerCode();
	~CGTarget();
};
#endif
