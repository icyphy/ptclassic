#ifndef _AnyAsmStar_h
#define _AnyAsmStar_h 1
/******************************************************************
Version identification:
$Id$

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
