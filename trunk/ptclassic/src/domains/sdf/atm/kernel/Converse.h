#ifndef  _Converse_h
#define  _Converse_h 1
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

This defines a class which contains all pertinent information
to describe the status of a call in our ATM simulation.
There will be one instance of this for every phone in the simulation.
Note if phone A talks to phone B, their conversation objects will
appear identical.
 
*************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"


class Conversation

{
private:

//     VCI values for identities of phones now talking: phoneOne
//     indicates which phone initiated the call, phoneTwo the
//     receiving phone
       int   phoneFirst, phoneSecond;

//     array records the virtual circuit for this call
       int   route [4] [3];


public:

//     dummy constructor and destructor
       Conversation()   { talkingOrNot = FALSE; }
       ~Conversation()  { }

//     if value is -1, means the callers are attached to same switch
//     and no routing ever had to be performed
//     otherwise assumes a value of 0, 1, or 2 for this simulation
//     it indicates how many "wasted," intermediate switches had to be
//     traversed for a path to be set up between the two callers
       int   indirections;

//     indicates whether conversation is now in progress or not
       int   talkingOrNot;

//     DO NOT call this before "indirections" has been set
//     prepare array, set it up for calls of the update() method
//     indicate identities of the two telephone parties
       void  setPhones (int vcia, int vcib);

//     what's the VCI of the phone talking to the argument phone VCI?
       int   otherPhone (int vci);

//     called while controller traverses routing trees, for first node
//     nodeNum is zero, this method is passed a switch and outgoing VPI
//     to place into table
       void  update (int nodeNum, int sw, int vpi);

//     given a row number, this method gives its elements in that row
//     of the table, first row is one
       void  readRow (int row, int& sw, int& vpiFrom, int& vpiTo);

};

#endif
