static const char file_id[] = "Set.h";

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

/**********************************************************************

 A set of integers

 @Description The members of the set are stored as bits in a word for
 constant-time access.  Each member has an integer index in the range
 0,1,...,Set.size() - 1.

 **********************************************************************/
class Set {

  Set(int);
  ~Set;

  // Return one more than the largest allowed index
  int size() { return mysize; }

  // Return non-zero if the item is present, zero otherwise
  int operator [] (int index) {
    return words[word(index)] & (1 << bit(index));
  }

  // Add an item to the set
  Set & operator |= (int index) {
    words |= (1 << bit(index));
    return *this;
  }

  Set & operator |= (Set &);

  // Remove an item from the set
  Set & operator -= (int index) {
    words &= ~(1 << bit(index));
    return *this;
  }

  Set & operator -= (Set &);

  int cardinality();

protected:

  // Return the array for the index
  int word( int index ) { return index / sizeof(int); }

  // Return the bit for the index
  int bit( int index ) { return index % sizeof(int); }
    
  // Number of bits in the set
  int mysize;

  // Array of integers used to store the bitmask
  int * words;

}
