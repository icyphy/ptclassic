#include "Code.h"
#include <stream.h>

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

**************************************************************************/

void BlockOfCode :: printCode () {
	CodeBlockIter line(*this);
	for(int i = size(); i>0; i--) {
		LineOfCode* p = line++;
		cout << p->getText() << "\n";
	}
}
