#ifndef _Nebula_h
#define _Nebula_h 1
#ifdef __GNUG__
#pragma interface
#endif


/*******************************************************************
 SCCS Version identification :
 $Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmers : Jose Luis Pino & T. M. Parks
 Date of Creation : 6/10/90
	
********************************************************************/

#include "Block.h"
#include "GalIter.h"
#include "DynamicGalaxy.h"
#include "Scheduler.h"
class Star;
class NebulaPort;

class Nebula {
friend class NebulaIter;
public:

    Nebula(Star& self);

    virtual ~Nebula();
    // set the master and build nebula
    virtual void setMasterBlock(Block* master,PortHole** newPorts = NULL);
    
    // Set the scheduler of the Nebula
    void setInnerSched(Scheduler* s);

    void setStopTime(double limit) {
	sched->setStopTime(limit);
    }

    double getStopTime() {
	return sched->getStopTime();
    }
    
    Galaxy* galaxy() { return &gal;}

    // Generate the schedules of the nested Nebulas recursively.
    int generateSchedule();

    int isNebulaAtomic() {
	return master? master->isItAtomic() : NULL;
    }

    void addNebula(Nebula*);

    void addGalaxy(Galaxy*,PortHole**);

    void initMaster();

    Scheduler* outerSched() {
	return star().scheduler()?star().scheduler():master->scheduler();
    }
    
    Scheduler* innerSched() { return sched;} 

//	void merge(Nebula*);
//	void absorb(Nebula*);

    int run();

    virtual PortHole* clonePort(const PortHole*) = 0;

    // unfortuanetly, we need this to return the nebula port side
    // from a porthole pointer.  The derived class just needs to
    // return the PortHole pointer casted to its repective NebulaPort
    // hole class.  (ie DFNebulaPort for DFNebula)
    virtual NebulaPort* nebulaPort(PortHole*) const = 0;

    virtual Nebula* asNebula(Block*) const = 0;
    
    virtual Nebula* newNebula(Block* s = NULL) const = 0;

    virtual int flattenGalaxy(Galaxy*) {return FALSE;}
    
    Star& star() const { return selfStar; }
    StringList displaySchedule();
    
protected:
    // The Star part of the Nebula.
    Star& selfStar;

    Block* master;

    DynamicGalaxy gal;
    Scheduler* sched;

};

// An iterator for NebulaList.
class NebulaIter : private GalStarIter {
public:
    NebulaIter(Nebula& n):master(n),GalStarIter(n.gal) {};
    Nebula* next() { return master.asNebula(GalStarIter::next());}
    Nebula* operator++(POSTFIX_OP) { return next();}
    GalStarIter::reset;
private:
    Nebula& master;
};

class NebulaPort {
public:
    NebulaPort(PortHole& self, const PortHole& p, Nebula* parnetN);
    const PortHole& real() const { return pPort; }
    PortHole& asPort() const { return selfPort;}
    int isItInput() const {
	return pPort.isItInput();
    }
    int isItOutput() const {
	return pPort.isItOutput();
    }

private:
    PortHole& selfPort;
    const PortHole& pPort;
};

class NebulaIterContext;

class AllNebulaIter {
public:
    AllNebulaIter(Nebula&);
    ~AllNebulaIter();
    Nebula* next();
    Nebula* operator++(POSTFIX_OP) { return next();}
    void reset();
protected:
    NebulaIter *thisLevelIter;
    NebulaIterContext *stack;
    void push(Nebula&);
    void pop();
};

class FatNebulaIter : private AllNebulaIter {
public:
    FatNebulaIter(Nebula&);
    Nebula* next();
    Nebula* operator++(POSTFIX_OP) { return next();}
    void reset() { AllNebulaIter::reset();}

    // need a public destructor because of private derivation
    ~FatNebulaIter(){}
};

#endif


