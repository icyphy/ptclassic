/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Joseph T. Buck
 Date of creation: 6/6/90

 This module, when linked into the interpreter, makes it an SDF interpreter.
**************************************************************************/
#include "Domain.h"
#include "SDFScheduler.h"
#include "SDFUniverse.h"
#include "Output.h"

extern Error errorHandler;

Scheduler*
Domain::newSched() { return new SDFScheduler;}

const char*
Domain::universeType() { return SDFstring;}

const char*
Domain::domainName() { return "SDF";}

int
Domain::set(const char* newDom) {
	if (strcmp (newDom, domainName()) != 0) {
		errorHandler.error("Cannot change domain (yet)!");
		return FALSE;
	}
	else return TRUE;
}
