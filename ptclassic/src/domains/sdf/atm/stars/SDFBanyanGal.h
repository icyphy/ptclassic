/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
							COPYRIGHTENDKEY

 Programmer:  John C. Loh
 Date of creation: 6/10/90

 Banyan galaxy composed of 2x2 BanyanStg elements, each using
 intermediate queueing

*************************************************************************/
#ifndef _SDFBanyanGal_h
#define _SDFBanyanGal_h 1
#ifdef __GNUG__
#pragma interface
#endif



#include "SDFBanyanStg.h" 
#include "Galaxy.h"

class SDFBanyanGal : public Galaxy 

{

public: 
        MultiInSDFPort left0, left1;
	MultiOutSDFPort right0, right1; 

	SDFBanyanStg* stars;

	SDFBanyanGal ();
	~SDFBanyanGal ();

        const char* className()  const;
        Block* makeNew()  const;
	const char* domain()  const;

	void setup();
        void wrapup();

};

#endif
