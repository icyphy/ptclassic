static const char file_id[] = "Sim56Target.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

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
	addCode("	movep	#$0000,x:m_bcr\n\n");
};

Block* Sim56Target::makeNew() const {
	LOG_NEW; return new Sim56Target(*this);
}

void Sim56Target :: wrapup () {
	MotorolaSimTarget :: wrapup();
	CG56Target::wrapup();
}

ISA_FUNC(Sim56Target,CG56Target);

// make an instance
static Sim56Target proto("sim-CG56","run code on the 56000 simulator");

static KnownTarget entry(proto, "sim-CG56");
