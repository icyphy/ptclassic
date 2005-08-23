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
static const char file_id[] = "Encoder.cc";

/*
Viterbi Encoder

Version identification:
@(#)Encoder.cc	1.3	3/7/96

Programmer:     Neal Becker
 */

#include "Encoder.h"

void Encoder::Reset() {
  for( int i = 0; i < 6; i++ )
    State[ i ] = 0;
}

void Encoder::Shift( int Input ) {
  State[ 5 ] = State[ 4 ];
  State[ 4 ] = State[ 3 ];
  State[ 3 ] = State[ 2 ];
  State[ 2 ] = State[ 1 ];
  State[ 1 ] = State[ 0 ];
  State[ 0 ] = Input;
}

int Encoder::G1( int Input ) {
  return Input ^ State[ 1 ] ^ State[ 2 ] ^ State[ 4 ] ^ State[ 5 ];
}

int Encoder::G2( int Input ) {
  return Input ^ State[ 0 ] ^ State[ 1 ] ^ State[ 2 ] ^ State[ 5 ];
}

void Encoder::operator() ( int Input, int Output[ 2 ] ) {
  Output[ 0 ] = G1( Input );
  Output[ 1 ] = G2( Input );
  Shift( Input );
}

