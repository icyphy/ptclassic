#ifndef _MotorolaAttributes_h
#define _MotorolaAttributes_h 1
/******************************************************************
Version identification:
@(#)MotorolaAttributes.h	1.5	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer: J. Pino

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "Attribute.h"

const bitWord AB_XMEM = 0x10000; // allocate in X memory bank
const bitWord AB_YMEM = 0x20000; // allocate in Y memory bank
const bitWord PB_XMEM = 0x10000; // allocate in X memory bank
const bitWord PB_YMEM = 0x20000; // allocate in Y memory bank
extern const Attribute A_XMEM;
extern const Attribute A_YMEM;
extern const Attribute P_XMEM;
extern const Attribute P_YMEM;

#endif
