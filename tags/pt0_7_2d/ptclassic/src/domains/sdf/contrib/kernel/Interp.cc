/*
Copyright (c) 1990-1996 The Regents of the University of California.
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
*/
static const char file_id[] = "Interp.cc";

/*
Version identification:
@(#)Interp.cc	1.2     3/7/96

Programmer:     Neal Becker
 */

#include "Interp.h"

int Interp::Threshold( double x ) {
  for( int i = 0; i < Size; i++ ) {
    if( x < X[ i ] )
      return i;
  }
// should't happen
  return Size - 1;
}

double Interp::Lin( double x, int below, int above ) {
  double delta = ( x - X[ below ] )/( X[ above ] - X[ below ] );
  return Y[ above ] * delta + Y[ below ] * ( 1 - delta );
}

double Interp::operator [] ( double x ) {
  int below, above;
  if( x <= X[ 0 ] ) {
    below = 0;
    above = 1;
  }
  else if ( x >= X[ Size - 1 ] ) {
    below = Size - 2;
    above = Size - 1;
  }
  else {
    above = Threshold( x );
    below = above - 1;
  }
  return Lin( x, below, above );
}
