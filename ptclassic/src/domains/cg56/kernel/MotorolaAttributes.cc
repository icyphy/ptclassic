static const char file_id[] = "MotorolaAttributes.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. Pino

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "MotorolaAttributes.h"
#include "AsmStar.h"

extern const Attribute A_XMEM = {AB_MEMORY|AB_XMEM,AB_YMEM};
extern const Attribute A_YMEM = {AB_MEMORY|AB_YMEM,AB_XMEM};
extern const Attribute P_XMEM = {PB_XMEM,PB_YMEM};
extern const Attribute P_YMEM = {PB_YMEM,PB_XMEM};
