// Animation functions for text interfaces
// $Id$
/*
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
*/
//
// Programmer: J. Buck

static const char file_id[] = "textAnimate.cc";

#include "SimControl.h"
#include "Star.h"
#include "textAnimate.h"
#include <stream.h>

static SimAction *pre = 0, *post = 0;

static void textHighlight(Star* s, const char*) {
	cout << "RUNNING: " << s->fullName() << " ... ";
}

static void textClearHighlight(Star*,const char*) {
	cout << "DONE\n";
}

int textAnimationState() { return pre != 0;}

void textAnimationOn() {
	if (!pre) {
		pre = SimControl::registerAction(textHighlight,1);
		post = SimControl::registerAction(textClearHighlight,0);
	}
}

void textAnimationOff() {
	if (pre) {
		SimControl::cancel(pre);
		SimControl::cancel(post);
	}
	pre = post = 0;
}

		
