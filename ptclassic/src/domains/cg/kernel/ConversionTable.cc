static const char file_id[] = "ConversionTable.cc";
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
#pragma implementation
#endif


#include <string.h>
#include "ConversionTable.h"

// Constructor
ConversionTable::ConversionTable() {
  rows = 0;
  tablesize = 16;		// Arbitrary initial estimate of table size
  table = new ConversionTableRow[tablesize];
}
    
// Destructor
ConversionTable::~ConversionTable() {
  for (int i = 0; i < rows ; i++)
    delete [] table[i].star;
  delete [] table;
}
    
// Add a table row.
void ConversionTable::tblRow(DataType src, DataType dst, const char* star) {
  if (rows >= tablesize) {
    // need to enlarge table
    tablesize *= 2;
    ConversionTableRow* newtable = new ConversionTableRow[tablesize];
    for (int i = 0; i < rows; i++)
      newtable[i] = table[i];
    delete [] table;
    table = newtable;
  }
  table[rows].src = src;
  table[rows].dst = dst;
  table[rows].star = strcpy (new char[strlen(star)+1], star);
  rows++;

}
