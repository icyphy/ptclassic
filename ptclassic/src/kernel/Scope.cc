static const char file_id[] = "StateScope.cc";
#ifdef __GNUG__
#pragma implementation
#endif

/**************************************************************************
Version identification:
$Id$	%Q%

Copyright (c) 1990-%Q% The Regents of the University of California.
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

#include "StateScope.h"
#include "Wormhole.h"

void StateScope::remove(Block& b) {
    Galaxy::removeBlock(b);
    b.setParentScope((StateScope*)NULL);
}

StateScope::~StateScope() {
    GalTopBlockIter nextBlock(*this);
    Block *block;
    while ((block = nextBlock++)) {
	block->setParentScope((StateScope*)NULL);
    }
    BlockListIter nextChildScope(childScopes);
    while ((block = nextChildScope++))
	delete block;
    StateListIter nextState(states);
    State* state;
    while ((state=nextState++))
	delete state;
}
    
StateScope::StateScope(Galaxy& galaxyToScope, StateScope* parentScope) {
    GalTopBlockIter nextBlock(galaxyToScope);
    Block *block;
    while ((block = nextBlock++)) {
	block->setParentScope(this);
	Galaxy::addBlock(*block);
	StateScope* childScope = NULL;
	if (!block->isItAtomic())
	    childScope=new StateScope(block->asGalaxy(),this);
	if (block->isItWormhole()) 
	    childScope =
		new StateScope(((Wormhole*)block)->insideGalaxy(),this);
	if (childScope) childScopes.put(*childScope);
    }
    setParent(parentScope);
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
