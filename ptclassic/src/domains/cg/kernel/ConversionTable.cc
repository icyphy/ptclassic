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

#include <iostream.h>
#include <string.h>
#include "dataType.h"
#include "ConversionTable.h"

// Constructor
ConversionTable::ConversionTable(int r):rows(r),rowToInitialize(0) {
  table = new ConversionTableRow[rows];
}
    
// Destructor
ConversionTable::~ConversionTable() {
  int i;
  for (i = 0; i < rows ; i++)
    delete [] table[i].star;
  delete [] table;
}
    
// Add a table row.
int ConversionTable::tblRow(DataType src, DataType dst, const char* star) {
  int status;
  if (rowToInitialize >= rows) {
    cout << "Attempting to add too many rows to table\n";
    status = 0;
  } else {
    table[rowToInitialize].src = src;
    table[rowToInitialize].dst = dst;
    table[rowToInitialize++].star = strcpy (new char[strlen(star)+1], star);
    status =1;
  }
  return status;
}
