static const char file_id[] = "CG56Target.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

 Base target for Motorola 56000 assembly code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CG56Target.h"
#include "CG56Star.h"

#ifdef __GNUG__
#pragma implementation
#endif

const Attribute ANY(0,0);

CG56Memory :: CG56Memory(unsigned x_addr, unsigned x_len, unsigned y_addr,
			 unsigned y_len) :
	DualMemory("x",A_XMEM,ANY,x_addr,x_len,"y",A_YMEM,ANY,y_addr,y_len)
{}

CG56Target :: CG56Target(const char* nam, const char* desc,
			 unsigned x_addr, unsigned x_len,
			 unsigned y_addr, unsigned y_len) :
	xa(x_addr), xl(x_len), ya(y_addr), yl(y_len),
	AsmTarget(nam,desc,"CG56Star",
		  *new CG56Memory(x_addr,x_len,y_addr,y_len))
{
}

void CG56Target :: headerCode () {
        StringList code = "; generated code for target ";
        code += readFullName();
        code += "\n";
	code += mem.printMemMap(";","");
        addCode(code);
}

CG56Target :: ~CG56Target () {
	LOG_DEL; delete &mem;
}

Block* CG56Target :: clone () const {
	LOG_NEW; return new CG56Target(readName(),readDescriptor(),xa,xl,ya,yl);
}

StringList CG56Target::beginIteration(int repetitions, int) {
	StringList out;
	if (repetitions == -1)		// iterate infinitely
		out = "label\n";
	else {				// iterate finitely
		out = "\tdo\t";
		out += repetitions;
		out += ",label\n";
	}
	return out;
}

StringList CG56Target::endIteration(int repetitions, int) {
	StringList out;
	if (repetitions == -1)		// iterate infinitely
		out = "\tjmp\tlabel\n";
	else				// iterate finitely
		out = "label\n";
	return out;
}
