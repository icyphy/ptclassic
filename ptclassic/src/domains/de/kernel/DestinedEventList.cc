static const char file_id[] = "DestinedEventList.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1997 The Regents of the University of California.
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

 Programmer: John Davis
 Date: 5/19/97
        The DestinedEventList class is used to store CqLevelLinks
        of the Calendar Queue. Each DEStar has a DestinedEventList
        associated with it. When a star dies, its DestinedEventList 
        removes pending events destined for the star.

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DestinedEventList.h"


// DestinedEventList::DestinedEventList() {}

Link * DestinedEventList::appendGet( CqLevelLink * obj ) 
{
	return (Link *)LinkedList::appendGet( obj );
}

void DestinedEventList::remove( Link * obj )
{
	LinkedList::directRemove( obj );
	return;
}





