#ifndef _ConversionTable_h
#define  _ConversionTable_h 1

/******************************************************************
Version identification:
$Id$

Copyright (c) %Q% The Regents of the University of California.
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

 Programmers: Jose Luis Pino, Christopher Hylands

 Manage Type Conversion for Code Generation.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "dataType.h"

struct ConversionTableRow {
    DataType src, dst;
    const char *star;		// name of star class to do this conversion
};

class ConversionTable {
public:
    ConversionTable();
    ~ConversionTable();

    // Add a row to the table
    void tblRow(DataType src, DataType dst, const char* star);

    // Accessors
    int numEntries() const { return rows; }
    const ConversionTableRow* entry (int i) const { return table + i; }
    
private:
    ConversionTableRow* table;
    int rows;			// number of slots in use
    int tablesize;		// allocated size of table
};
#endif
