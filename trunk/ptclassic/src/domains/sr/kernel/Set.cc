static const char file_id[] = "Set.cc";

/* Version $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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

    Author:     S. A. Edwards
    Created:    23 October 1996

 */

// Allocate an empty set of the given size
//
// @Description If this is a Set(i), then the indices of its contents
// are 0,1,...,i-1

Set::Set(int s)
{
  mysize = s;
  words = new int[word(s-1)];
  for ( i = word(s) ; --i >= 0 ; ) {
    words[i] = 0;
  }
}

// Destroy the set

Set::~Set()
{
  delete [] words;
}

// Return the number of items in the set

int Set::cardinality()
{
  count = 0;
  for ( int i = mysize ; --i >= 0 ; ) {
    if ((*this)[i]) {
      count++;
    }
  }

  return count;

}

// Add a set to a set

Set & operator |= (Set & s)
{
  for ( int i = word(mysize-1)+1 ; --i >= 0 ; ) {
    words[i] |= s.words[i];
  }
  return *this;
}

// Remove a set from a set
Set & operator -= (Set & s) {
  for ( int i = word(mysize-1)+1 ; --i >= 0 ; ) {
    words[i] &= ~(s.words[i]);
  }
  return *this;
}

