static const char file_id[] = "Distribution.cc";

/**************************************************************************
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

 Programmer:  Soonhoi Ha
 Date of creation: 7/2/91

 Definition of some statistical distributions.
 If a user wants to add a new type, he/she must adjust "NUM" and "defType".

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Distribution.h"
#include "dist_template.h"
#include "distributions.h"
#include "StringList.h"
#include "StringState.h"
#include "MultiTarget.h"

// For setType() method...
// Change here to add a new distribution.
// list of the distributions ------------------------------------------

#define NUM 3           // number of defined distribution types.

static DistBase *dists[NUM];

class BuildDists {
public:
	BuildDists() {
		LOG_NEW; dists[0] = new DistGeometric;
		LOG_NEW; dists[1] = new DistUniform;
		LOG_NEW; dists[2] = new DistGeneral;
	}
	~BuildDists() {
		for (int i = 0; i < NUM; i++) {
			LOG_DEL; delete dists[i];
		}
	}
};

// this is here to force constructor/destructor calls.
static BuildDists dummyDistBuilder;

struct definedTypes {
        char* name;
        distType type;
	int index;
};

static definedTypes defType[] = {
        {"geometric", GEOMETRIC, 0},
        {"uniform", UNIFORM, 1},
        {"general", GENERAL, 2}
};

// ---------------------------------------------------------------------

// virtual functions.

StringList DistBase :: printParams() {
	StringList out;
	out += "undefined";
	return out; } 

int DistBase :: setParams(Galaxy*, MultiTarget*) 
	{ return TRUE; }	// read the parameters.

DistBase* DistBase :: copy() { return NULL; }	// copy the right distribution.

void DistBase :: paramInfo() {}		// information on parameters.

double DistBase :: assumedValue() { return 0; } 

const char* DistBase :: myType() const { return "unspecified"; }

DistBase :: ~DistBase() {
	LOG_DEL; if (params) delete params; 
}

// Select the right model of distribution.
// The type should be defined in the target by the "paramType" state.

int DistBase :: setType(Galaxy* g, MultiTarget* tg) {

        distType temp = NONE;

        // fetch the type of the distribution from the target state.
        StringState* st = (StringState*) tg->galParam(g,"paramType");
        if (!st) {
                Error :: error(*tg, "has distribution type undefined;\n",
                        "Use the default type - geometric");
                temp = GEOMETRIC;
        } else {
                const char* nm = st->initValue();
                for (int i = 0; i < NUM; i++)
                        if (!strncmp(defType[i].name, nm, 3)) {
                                temp = defType[i].type;
                                break;
                        }
                if (temp == NONE) return FALSE;
        }

        // get the right Distribution class.
        if (temp == type) return TRUE;
        else    type = temp;
        if (params) { LOG_DEL;   delete params; }
        params = dists[defType[type].index]->copy();

        return TRUE;
}

