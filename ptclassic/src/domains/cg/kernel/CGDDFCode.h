/******************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
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

 Programmer: Soonhoi Ha

 Baseclass for code generation routines for dynamic constructs

*******************************************************************/

#ifndef _CGDDFCode_h
#define  _CGDDFCode_h 1

#ifdef __GNUG__
#pragma interface
#endif

class CGStar;
class Geodesic;
class CGTarget;
class PortHole;
class Profile;
class MultiTarget;

class CGDDFCode {
public:

	CGDDFCode(MultiTarget* t) : owner(t) {}

///////// target dependent section ////////////////
	// Some preparation steps for code generation
	// are necessary for For and Recur construct.
	virtual void prepCode(MultiTarget*, Profile*, int, int);
	virtual void signalCopy(int);

	// for Case construct
	virtual void startCode_Case(CGStar*,Geodesic*,CGTarget*);
	virtual void middleCode_Case(int, CGTarget*);
	virtual void endCode_Case(CGTarget*);

	// for DoWhile construct
	virtual void startCode_DoWhile(Geodesic*, Geodesic*, CGTarget*);
	virtual void endCode_DoWhile(CGStar*, Geodesic*, CGTarget*);

	// for For construct
	virtual void startCode_For(CGStar*, Geodesic*, CGTarget*);
	virtual void middleCode_For(CGStar*, CGStar*, int, int, CGTarget*);
	virtual void endCode_For(CGTarget*);

	// for Recur construct
	virtual void startCode_Recur(Geodesic*, PortHole*, 
					const char*, CGTarget*);
	virtual void middleCode_Recur(Geodesic*,Geodesic*,
					const char*,CGTarget*);
	virtual void endCode_Recur(Geodesic*, const char*, CGTarget*);

protected:
	MultiTarget* owner;

};
#endif
