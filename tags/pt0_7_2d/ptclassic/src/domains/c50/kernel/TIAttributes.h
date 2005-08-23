#ifndef _TIAttributes_h
#define _TIAttributes_h 1
/******************************************************************
Version identification:
@(#)TIAttributes.h	1.2	8/15/96

@Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer: A. Baensch

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "Attribute.h"

const bitWord AB_BMEM = 0x10000; // allocate in B1 memory bank
const bitWord AB_UMEM = 0x20000; // allocate in UD memory bank
const bitWord PB_BMEM = 0x10000; // allocate in B1 memory bank
const bitWord PB_UMEM = 0x20000; // allocate in UD memory bank
extern const Attribute A_BMEM;
extern const Attribute A_UMEM;
extern const Attribute P_BMEM;
extern const Attribute P_UMEM;

#endif


