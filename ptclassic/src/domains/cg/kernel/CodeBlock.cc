static const char file_id[] = "Code.cc";
#include "Code.h"
#include <stream.h>

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

**************************************************************************/

void CodeBlock:: printCode () {
	cout << text << "\n";
}

void CodeBlockList :: printCode () {
	CodeBlockIter line(*this);
	for(int i = size(); i>0; i--) {
		CodeBlock* p = line++;
		p->printCode();
	}
}
