static const char file_id[] = "Set.cc";

/* Version $Id$

@Copyright (c) 1996-%Q% The Regents of the University of California.
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

    Author:     S. A. Edwards
    Created:    23 October 1996

 */

#include "Set.h"
#include "StringList.h"
#include <stream.h>

// int Set::numsets = 0;

// Allocate an empty set of the given size with given initial value
//
// @Description If this is a Set(i), then the indices of its contents
// are 0,1,...,i-1.  The size must be equal to or greater than one.

Set::Set(int s /* number of elements in the set */,
	 int f /* zero = empty set, non-zero = full set */ )
{
  mysize = s;

  words = new int[numwords()];
  for ( int i = numwords() ; --i >= 0 ; ) {
    words[i] = f ? ~0 : 0;
  }
  // numsets++;
}

// Make a copy of a set

void Set::setequal(Set & s) {
  delete [] words;
  mysize = s.mysize;
  words = new int[numwords()];
  for ( int i = numwords() ; --i >= 0 ; ) {
    words[i] = s.words[i];
  }
}

// Destroy the set

Set::~Set()
{
  delete [] words;
  // numsets--;
}

// Return the number of items in the set

int Set::cardinality() const
{
  int count = 0;
  for ( int i = mysize ; --i >= 0 ; ) {
    if ((*this)[i]) {
      count++;
    }
  }

  return count;

}

// Return a member of a set, or -1 if it's empty

int Set::onemember() const
{
  for ( int i = mysize ; --i >= 0 ; ) {
    if ((*this)[i]) {
      return i;
    }
  }
  return -1;
}

// Add a set to a set

Set & Set::operator |= (Set & s)
{
  for ( int i = numwords() ; --i >= 0 ; ) {
    words[i] |= s.words[i];
  }
  return *this;
}

// Remove a set from a set
Set & Set::operator -= (Set & s) {
  for ( int i = numwords() ; --i >= 0 ; ) {
    words[i] &= ~(s.words[i]);
  }
  return *this;
}

// AND a set with a set
Set & Set::operator &= (Set & s) {
  for ( int i = numwords() ; --i >= 0 ; ) {
    words[i] &= s.words[i];
  }
  return *this;  
}

// Return a printed representation of a set
StringList Set::print() const
{
  StringList out;

  int addSpace = 0;

  int i;
  SetIter next(*this);
  while ( (i = next++) >= 0 ) {
    if ( addSpace ) { out << " "; }
    out << i;
    addSpace = 1; 
  }

  return out;
  
}

