#ifndef _AnyAsmStar_h
#define _AnyAsmStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Buck

This file defines a class of star that can be used under any
assembly language domain in addition to the type that generates
code for that domain.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif
#include "AsmStar.h"

#define AnyAsmPortHole AsmPortHole
#define InAnyAsmPort InAsmPort
#define OutAnyAsmPort OutAsmPort
#define MultiInAnyAsmPort MultiInAsmPort
#define MultiOutAnyAsmPort MultiOutAsmPort

class AnyAsmStar : public AsmStar {
public:
	const char* domain() const;
	int isA(const char*) const;
};

#endif
