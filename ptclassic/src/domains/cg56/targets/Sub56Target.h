/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino

 Target for Motorola 56000 assembly code generation that generates
 two subroutines, ptolemyInit, ptolemyMain.
 
*******************************************************************/
#ifndef _Sub56Target_h
#define _Sub56Target_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CG56Target.h"
#include "StringState.h"

class Sub56Target : public CG56Target {
private:
	char* uname;
	void initStates();
protected:
	StringState dirName;
	StringState disCode;
public:
	Sub56Target(const char*,const char*,
		    unsigned,unsigned,unsigned,unsigned);
	Sub56Target(const Sub56Target&);
	~Sub56Target();
	void headerCode();
	int setup(Galaxy&);
	int run();
	void wrapup();
	Block* clone() const;
	int isA(const char*) const;
};

#endif
