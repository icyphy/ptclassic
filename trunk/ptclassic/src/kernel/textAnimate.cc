// Animation functions for text interfaces
// $Id$
// Copyright (c) 1992 The Regents of the University of California.
//                      All Rights Reserved.
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

		
