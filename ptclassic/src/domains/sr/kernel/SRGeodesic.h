/* -*- c++ -*- */
 
#ifndef _SRGeodesic_h
#define _SRGeodesic_h
 
/*  Version $Id$
 
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
    Created:    25 October 1996

*/

#ifdef __GNUG__
#pragma interface
#endif

#include "Geodesic.h"

/**********************************************************************

  The geodesic for the SR domain

  @Description This implements the single-driver, multiple-receiver
  communication channels of the SR domain.

  <P> originatingPort defines the driver, but receivers defines the list
  of receivers.  The Geodesic::destinationPort is ignored.

**********************************************************************/
class SRGeodesic : public Geodesic {
private:

  // A list of InSRPort *s driven by this geodesic 
  // @Description This supplants the destinationPort field of the
  // Geodesic class
  SequentialList receivers;

public:

  SRGeodesic();
  ~SRGeodesic();

  PortHole * setSourcePort( GenericPort &, int, const char * = 0 );
  PortHole * setOldSourcePort( GenericPort &, int, const char * = 0 );
  PortHole * setDestPort( GenericPort & );
  PortHole * setOldDestPort( GenericPort & );

  PortHole * getReceiver();

  int isItPersistent() const;
  int disconnect( PortHole & );  
};

#endif
