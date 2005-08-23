/* -*- c++ -*- */
 
/*  Version $Id$
 
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
    Created:    25 October 1996

*/

#include "SRGeodesic.h"
#include "PortHole.h"
#include "Block.h"
#include <stream.h>
#include <assert.h>


SRGeodesic::SRGeodesic()
{

  cout << "Constructed an SRGeodesic\n";

}

PortHole * SRGeodesic::setSourcePort( GenericPort & p, int, const char * = 0 )
{
  cout << "SRGeodesic::setSourcePort called on " << p.parent()->name()
    << " " << p.name() << "\n";

  return (PortHole *) &p;  
}

PortHole * SRGeodesic::setDestPort( GenericPort & p )
{
  cout << "SRGeodesic::setDestPort called on " << p.parent()->name()
    << " " << p.name() << "\n";

  return (PortHole *) &p;

}
