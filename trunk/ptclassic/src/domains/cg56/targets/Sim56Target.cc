static const char file_id[] = "Sim56Target.cc";
/******************************************************************
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

 Programmer: J. Buck, J. Pino

 Target for Motorola 56000 assembly code generation that runs its
 output on the simulator.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Sim56Target.h"
#include "KnownTarget.h"
#include "MotorolaTarget.h"

Sim56Target :: Sim56Target(const char* nam, const char* desc) :
	MotorolaTarget(nam,desc,"CG56Star"),CG56Target(nam,desc),MotorolaSimTarget(nam,desc,"CG56Star")
{
	initStates();
}

Sim56Target::Sim56Target(const Sim56Target& arg) :
	MotorolaTarget(arg),CG56Target(arg),MotorolaSimTarget(arg)
{
	initStates();
	copyStates(arg);
}

void Sim56Target :: initStates() {
	MotorolaSimTarget::initStates("56000","48","ff0");
}

int Sim56Target::compileCode() {
	return MotorolaSimTarget::compileCode();
}

int Sim56Target::loadCode() {
	return MotorolaSimTarget::loadCode();
}

int Sim56Target::runCode() {
	return MotorolaSimTarget::runCode();
}

void Sim56Target :: headerCode () {
	CG56Target :: headerCode();
	MotorolaSimTarget :: headerCode();
	myCode << "	movep	#$0000,x:m_bcr\n\n";
};

Block* Sim56Target::makeNew() const {
	LOG_NEW; return new Sim56Target(*this);
}

void Sim56Target :: trailerCode () {
	MotorolaSimTarget :: trailerCode();
	CG56Target::trailerCode();
}

ISA_FUNC(Sim56Target,CG56Target);

// make an instance
static Sim56Target proto("sim-CG56","run code on the 56000 simulator");

static KnownTarget entry(proto, "sim-CG56");
