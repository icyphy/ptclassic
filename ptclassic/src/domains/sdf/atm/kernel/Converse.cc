static const char file_id[] = "Converse.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

Programmer:  Allen Y. Lao
Date of creation:  7/25/91

Implementation of the methods of the Conversation class
No doubt the methods appear mystifying in nature so here are
some comments


Suppose that the following virtual circuit has been defined:

  
       --------       --------       --------
   1  |        |  6  |        |  5  |        | 2
  --->|  SW 1  |---->|  SW 3  |---->|  SW 4  |--->
      |        |     |        |     |        |
       --------       --------       --------
  
  The numbers represent VPI's (1-8) of the corresponding links
  We can see that there is one indirection (i.e. one wasted
  switch being traversed) so this would mean that our array
  would be essentially with contents:

       SW    VPI    VPI
       --    ---    ---
        1     1      6 
        3     6      5 
        4     5      2

  Even though the array is of dim 4x3, here the fourth row's
  elements would all be set to zero.
  Initially, once the VC has been setup, the indirections variable
  should be set, in this case one.
  Then setPhones(x,x) is called which sets up the array to appear:

       SW    VPI    VPI
       --    ---    ---
        x     1      x 
        x     x      x
        4     x      2 

  This seems odd but allows for ease in successive calls of the
  update method as a tree is traversed.  If it turns out that
  while going along a tree, that the tree will fail, "indirections"
  is modified if necessary, and setPhones is called again to clear
  the array for another try as we move on to our next tree.

  The first update call, after passing SW 1 would have:

       SW    VPI    VPI
       --    ---    ---
        1     1      6
        x     6      x
        4     x      2

  After passing SW 3, the array assumes final form as given on way top
  Note, that readRow will be called to return the elements of a single
  row, this will be necessary for controller to send update packets
  to the SDF switch routing tables.

  Final note: when SCPE and DCPE are attached to same switch, then of
  course no routing is necessary.  Here "indirections" is set to -1
  by the controller, and the route array would only have one row
  (naturally).

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

extern int vciToVpi (int);
extern int vciToSW (int);

#include "Converse.h"

void
Conversation :: setPhones (int vcia, int vcib)

{
       phoneFirst = vcia;
       phoneSecond = vcib;
       talkingOrNot = FALSE;

       for (int i = 0; i<4; i++)
         for (int j = 0; j<3; j++)
           route [i] [j] = 0;

       route [0] [1] = vciToVpi(vcia);
       route [indirections+1] [0] = vciToSW(vcib);
       route [indirections+1] [2] = vciToVpi(vcib);
}



int
Conversation :: otherPhone (int vci)

{
      if (vci == phoneFirst)
        return phoneSecond;
      else if (vci == phoneSecond)
        return phoneFirst;
      else
        return 0;
}



void
Conversation :: update (int nodeNum, int sw, int vpi)

{
      route [nodeNum] [0] = sw;
      route [nodeNum] [2] = vpi;
      route [nodeNum+1] [1] = vpi;
}



void
Conversation :: readRow (int row, int& sw, int& vpiFrom, int& vpiTo)

{
      sw = route [row-1] [0];
      vpiFrom = route [row-1] [1];
      vpiTo = route [row-1] [2];
}

