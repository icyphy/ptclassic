#ifndef _dataType_h
#define _dataType_h 1
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck
 Date of creation: 8/13/90
 Revisions:

define the pseudo-class DataType and the "standard" particle types.

**************************************************************************/


// Types of data stored in queues between stars
typedef const char* DataType;

// build in standard types, user may add others
// ANYTYPE is provided for the benefit of Stars like Fork
// and Printer that operate independently of Particle type

extern const DataType INT, FLOAT, STRING, COMPLEX, ANYTYPE;

#endif
