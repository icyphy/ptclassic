#ifndef _StringParticle_h
#define _StringParticle_h 1

#ifdef __GNUG__
#pragma interface
#endif

/**************************************************************************
Version identification:
@(#)StringParticle.h	1.3	2/29/96

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

 Programmer:  John Reekie
 Date of creation: 1/27/96

This is a Particle class that contains StringMessages (accessed via
Envelopes). The class exists in order to make String messages
"first-class" datatypes.

**************************************************************************/
#include "Message.h"

extern const DataType STRING;

class StringParticle : public MessageParticle {
public:
	DataType type() const;

	// particle copy
	Particle& operator = (const Particle& p);

	// clone, useNew, die analogous to other particles.

	Particle* clone() const;

	Particle* useNew() const;
	void die();
};


#endif
