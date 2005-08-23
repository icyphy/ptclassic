static const char file_id[] = "Scope.cc";
#ifdef __GNUG__
#pragma implementation
#endif

/**************************************************************************
Version identification:
@(#)Scope.cc	1.5	1996

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
 Programmer:  Jose Luis Pino
 Date of creation: 10/17/95
		       
**************************************************************************/

#include "Scope.h"
#include "Wormhole.h"

StringList Scope::fullName() const {
    StringList fullNm;
    if (parentScope())
	fullNm << parentScope()->fullName() << '.';
    fullNm << name();
    return fullNm;
}
			     
State* Scope::lookup(const char* name) {
    State* state = stateWithName(name);
    if (!state && parentScope())
	return parentScope()->lookup(name);
    return state;
}
			     
void Scope::remove(Block& b) {
    Galaxy::removeBlock(b);
    b.setScope((Scope*)NULL);
    optionalDestructor();
}

void Scope::optionalDestructor() {
    if (numberBlocks() == 0 && childScopes.size() == 0) {
	if (parentScope()) parentScope()->removeChild(*this);
	delete this;
    }
}

void Scope::removeChild(Scope& child) {
    child.setParentScope((Scope*)NULL);
    childScopes.remove(&child);
    optionalDestructor();
}
    
Scope::~Scope() {
    if (numberBlocks() != 0 || childScopes.size() != 0) {
	Error::abortRun("~Scope: Attempt to delete a non-empty scope",
		   	this->name());
	GalTopBlockIter nextBlock(*this);
	Block *block;
	while ((block = nextBlock++)) {
	    block->setScope((Scope*)NULL);
	}
	BlockListIter nextChildScope(childScopes);
	while ((block = nextChildScope++))
	    delete block;
    }
    if (parentScope()) parentScope()->removeChild(*this);
    StateListIter nextState(states);
    State* state;
    while ((state=nextState++))
	delete state;
}

Scope* Scope::createScope(Galaxy& galaxyToScope) {
    GalTopBlockIter nextBlock(galaxyToScope);
    Block *block;
    block = nextBlock++;
    if (!block) return NULL;

    /* If the galaxy has clusters, a Scope hierarchy already
       exists. Shouldn't this return the top most scope of the
       blocks inside the clusters?  FIXME */
    if (block->isItCluster()) return NULL;

    if (block->scope()) {
	if (block->scope()->parentScope() &&
	    block->scope()->parentScope() != galaxyToScope.scope())
        {
	    StringList msg;
	    msg << "createScope: scope "
		<< block->scope()->parentScope()->fullName() 
		<< " inconsistant.";
	    Error::warn(msg);
	    block->scope()->setParentScope(galaxyToScope.scope());
	}
	return block->scope();
    }
    return new Scope(galaxyToScope);
}

Scope::Scope(Galaxy& galaxyToScope):prntScope(0){
    GalTopBlockIter nextBlock(galaxyToScope);
    Block *block;
    while ((block = nextBlock++)) {
	if (block->scope()) {
	    StringList message;
	    message << "Scope Constructor: " << block->name()
			<< " is already in the "
			<< block->scope()->fullName()
			<< " scope.  A memory leak will result.";
	    Error::warn(message);
	}
	block->setScope(this);
	Galaxy::addBlock(*block);
	Scope* child = NULL;
	if (!block->isItAtomic())
	    child = createScope(block->asGalaxy());
	if (block->isItWormhole()) {
	    Galaxy& wormGalaxy = ((Star*)block)->asWormhole()->insideGalaxy();
	    wormGalaxy.setScope(this);
	    child = createScope(wormGalaxy);
	}
	if (child) {
	    childScopes.put(*child);
	    child->setParentScope(this);
	}
    }
    setName(galaxyToScope.name());
    StateListIter nextState(galaxyToScope.states);
    State *state;
    while ((state = nextState++)) {
	State* clone = state->clone();
	clone->setState(state->name(),this,state->initValue());
	StringList cVal;
	cVal << state->currentValue();
	clone->setCurrentValue(hashstring(cVal));
	addState(*clone);
    }
}
