static const char file_id[] = "ACSCorona.cc";
/**********************************************************************
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA OR SANDERS, A LOCKHEED
MARTIN COMPANY, BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS
SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
OR SANDERS, A LOCKHEED MARTIN COMPANY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA AND SANDERS, A LOCKHEED MARTIN COMPANY
SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
THE UNIVERSITY OF CALIFORNIA OR SANDERS, A LOCKHEED MARTIN COMPANY
HAVE NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.  COPYRIGHTENDKEY


 Programmers:  Eric Pauer (Sanders), Christopher Hylands
 Date of creation: 1/15/98
 Version: @(#)ACSCorona.cc	1.16 09/08/99

***********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ACSCorona.h"
#include "ACSKnownCategory.h"
#include "KnownBlock.h"
#include "ACSTarget.h"


// FIXME: can't call pigilib to dynamically load cores because ptcl, pitcl
// don't link with it.

/* extern "C" int
KcCompileAndLink (const char* name, const char* idomain, const char* srcDir,
  int permB, const char* linkArgs); */

// The following is defined in ACSDomain.cc -- this forces that module
// to be included if any ACS stars are linked in.
extern const char ACSdomainName[];

// my domain.
const char* ACSCorona :: domain () const { return ACSdomainName;}

// isA
ISA_FUNC(ACSCorona, ACSStar);

// initialize cores name and parent members
void ACSCorona::initCores()
{
  ListIter iterator(coreList);
  ACSCore* ptr;
  while ( (ptr = (ACSCore*)(iterator++)) != 0 ) {
	ptr->setNameParent(name(),parent());
  }
}

// select Core to be used
int ACSCorona::setCore(const char *coreCategory)
{
  ListIter iterator(coreList);
  ACSCore* ptr;
  // iterate through list until Core is found, if not return null
  while ( (ptr = (ACSCore*)(iterator++)) != 0 ) {
	if ( strcmp(coreCategory, ptr->getCategory()) == 0 ) {
		currentCore = ptr;
		return TRUE;
	}
  }
  return FALSE;
}

// register Core in the list
int ACSCorona::registerCore(ACSCore &core, const char* implementationName)
{
  // store pointer to core in the CoreList
  return 0;
}

// This method must be called in the default constructors of the derrived
// corona classes.
void ACSCorona::addCores() { 
	StringList categories = getCoreCategories();
	StringListIter next(categories);
	StringList coreName;
	const char* coronaName;
	const char* domainName;
	const char* category;
	ACSCore* core;
//	const char* srcdir = getSrcDirectory();

	initCoreFlag = 1;

	
	while ((category=next++) != 0) {
		coronaName = className();
		domainName = domain();
		coronaName += strlen(domainName);
		coreName = coronaName;
		coreName += category;
		if ( (core =(ACSCore *)KnownBlock::find(coreName,domainName)) != NULL ) {
			coreList.put(core->makeNew(*this));
		} /* else {
			if(!KcCompileAndLink(coreName, domainName, srcdir, 0, 0)) continue;
			if ((core = (ACSCore *) KnownBlock::find(coreName,domainName)) != NULL ) {
				coreList.put(core->makeNew(*this));
				continue;
			}
			Error::warn("ACSCorona::addCores(): core compiled ok but still not known.");	
		} */
	}
}

// virtual initialize() called by run(), sets current core and initializes it.	
void ACSCorona::initialize() {
	initCores();
	ACSStar::initialize();
	ACSTarget* t = (ACSTarget*)target();
	if(!setCore(t->getCoreCategory())) {
		Error::abortRun(*this,"initialize(): could not set core.");
		return;
	}
	if ( currentCore ) {
		currentCore->initialize();
	} else {
		Error::abortRun(*this,"initialize(): currentCore not set.");
	}
}

int ACSCorona::setTarget(Target* t) {
	ACSStar::setTarget(t);
	ACSTarget* tp = (ACSTarget*)target();
  	ListIter iterator(coreList);
  	ACSCore* ptr;
  	while ( (ptr = (ACSCore*)(iterator++)) != 0 ) {
		ptr->setTarget(t);
  	}
	if(!setCore(tp->getCoreCategory())) {
		Error::abortRun(*this,"setTarget(Target* t): could not set core.");
		return FALSE;
	}
	if ( currentCore ) {
		return(currentCore->setTarget(t));
	} else {
		Error::abortRun(*this,"setTarget(Target* t): currentCore not set.");
	}
	return FALSE;
}
	
	

// virtual go() called by run, calls current core go() method.
void ACSCorona::go() {
	if ( currentCore ) {
		currentCore->go();
	} else {
		Error::abortRun(*this, className(), ".run(): currentCore not set.");
	}
}

// virtual wrapup() called by run, calls current core wrapup() method.
void ACSCorona::wrapup() {
	if ( currentCore ) {
		currentCore->wrapup();
	} else {
		Error::abortRun(*this, className(), ".wrapup(): currentCore not set.");
	}
}

// virtual initCode() called by run, calls current core initCode() method.
void ACSCorona::initCode() {
	if ( currentCore ) {
		currentCore->initCode();
	} else {
		Error::abortRun(*this, className(), ".initCode(): currentCore not set.");
	}
}

// virtual myExecTime() called by run, calls current core myExecTime() method.
int ACSCorona::myExecTime() {
	if ( currentCore ) {
		return(currentCore->myExecTime());
	} else {
Error::abortRun(*this, className(), ".myExecTime(): currentCore not set.");
	}
	return 0;
}
	

	
// add a splice star to the spliceClust list.  If atEnd
// is true, append it to the end, otherwise prepend it.
void ACSCorona :: addSpliceStar(ACSStar* s, int atEnd) {
	if ( currentCore ) {
		currentCore->addSpliceStar(s,atEnd);
	} else {
Error::abortRun(*this, className(), ".addSpliceStar(ACSStar* s, int atEnd): currentCore not set.");
	}
}

// Generate declaration, initialization and function codes for
// command-line arguments
StringList ACSCorona :: cmdargStates(Attribute a)
{
	if ( currentCore ) {
		return currentCore->cmdargStates(a);
	} else {
Error::abortRun(*this, className(), ".cmdargStates(Attribute a=ANY): currentCore not set.");
	}
	return StringList("");
}

StringList ACSCorona :: cmdargStatesInits(Attribute a)
{
	if ( currentCore ) {
		return currentCore->cmdargStatesInits(a);
	} else {
Error::abortRun(*this, className(), ".cmdargStatesInits(Attribute a): currentCore not set.");
	}
	return StringList("");
}

StringList ACSCorona :: setargStates(Attribute a)
{
	if ( currentCore ) {
		return currentCore->setargStates(a);
	} else {
Error::abortRun(*this, className(), ".setargStates(Attribute a): currentCore not set.");
	}
	return StringList("");
}

StringList ACSCorona :: setargStatesHelps(Attribute a)
{
	if ( currentCore ) {
		return currentCore->setargStatesHelps(a);
	} else {
Error::abortRun(*this, className(), ".setargStatesHelps(Attribute a=ANY): currentCore not set.");
	}
	return StringList("");
}

// Generate declarations and initialization code for PortHoles
StringList ACSCorona :: declarePortHoles(Attribute a)
{
	if ( currentCore ) {
		return currentCore->declarePortHoles(a);
	} else {
Error::abortRun(*this, className(), ".declarePortHoles(Attribute a=ANY): currentCore not set.");
	}
	return StringList("");
}

StringList ACSCorona :: initCodePortHoles(Attribute a)
{
	if ( currentCore ) {
		return currentCore->initCodePortHoles(a);
	} else {
Error::abortRun(*this, className(), ".initCodePortHoles(Attribute a=ANY): currentCore not set.");
	}
	return StringList("");
}

// Generate declarations and initialization code for States
StringList ACSCorona :: declareStates(Attribute a)
{
	if ( currentCore ) {
		return currentCore->declareStates(a);
	} else {
Error::abortRun(*this, className(), ".declareStates(Attribute a=ANY): currentCore not set.");
	}
	return StringList("");
}

StringList ACSCorona :: initCodeStates(Attribute a)
{
	if ( currentCore ) {
		return currentCore->initCodeStates(a);
	} else {
Error::abortRun(*this, className(), ".initCodeStates(Attribute a=ANY): currentCore not set.");
	}
	return StringList("");
}

StringList ACSCorona :: BufferIndex(const ACSPortHole * port, const char * name, const char * offset) {
	if ( currentCore ) {
		return currentCore->BufferIndex(port, name, offset);
	} else {
Error::abortRun(*this, className(), ".BufferIndex(const ACSPortHole * port, const char * name, const char * offset): currentCore not set.");
	}
	return StringList("");
}


int ACSCorona::isItFork() { 
	if ( currentCore ) {
		return currentCore->isItFork();
	} else {
Error::abortRun(*this, className(), ".isItFork(): currentCore not set.");
	}
	return 0;
}

int ACSCorona :: runCG() {
	if ( currentCore ) {
		return currentCore->run();
	} else {
		Error::abortRun(*this, className(), ".runCG(): currentCore not set.");
	}
	return 0;
}
