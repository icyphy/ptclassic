#ifndef _MotorolaAttributes_h
#define _MotorolaAttributes_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

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
