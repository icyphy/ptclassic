/* edg  5/22/90
Version identification:
$Id$
Ptolemy kernel calls.
Some code borrowed from Interpreter.cc, see this file for more info.


 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.
*/

#include "Universe.h"
#include "InterpGalaxy.h"
#include "KnownBlock.h"
#include "Block.h"
#include "StringList.h"
#include "State.h"
#include "miscFuncs.h"

/* boolean data type for pigi */
typedef int boolean;
#ifndef TRUE
#define TRUE	(1)
#define FALSE	(0)
#endif

/* error handling function for pigi */
extern "C" void ErrAdd(char*);

// Terminal data structs pigi
#define TERM_ARR_MAX 14  /* max # I/O terms = max # positions */
struct Term {
    char *name;
    boolean multiple;
};

struct TermList {
    Term in[TERM_ARR_MAX];
    int in_n;
    Term out[TERM_ARR_MAX];
    int out_n;
};

// Parameter structs for pigi
struct ParamStruct {
    const char *name;
    char *value;
};
typedef struct ParamStruct ParamType;

struct ParamListStruct {
    int length; /* length of array */
    ParamType *array; /* points to first element */
};
typedef struct ParamListStruct ParamListType;

// An InterpUniverse is an Universe with an interpreted galaxy
// in it.  This is accomplished by making it Runnable and an InterpGalaxy.

class InterpUniverse : public InterpGalaxy, public Runnable {
public:
        InterpUniverse () : Runnable(KnownBlock::newSched(),
                                     KnownBlock::domain(),this)
        { setBlock("mainGalaxy",NULL);}
        void newSched() {
                delete scheduler;
                scheduler = KnownBlock::newSched();
                type = KnownBlock::domain();
        }
};

PlasmaList plasmaList;  // This global is needed for the kernel
static InterpUniverse *universe = NULL;  // Universe to execute
static InterpGalaxy *currentGalaxy = NULL;  // current galaxy to make things in
static InterpGalaxy *saveGalaxy = NULL;  // used to build galaxies

// Delete the universe and make another
extern "C" void
KcClearUniverse() {
    delete universe;
    universe = new InterpUniverse;
    currentGalaxy = universe;
}

// Create a new instance of star or galaxy and set params for it
extern "C" boolean
KcInstance(char *name, char *ako, ParamListType* pListPtr) {
    if (!currentGalaxy->addStar(name, ako))
	return FALSE;
    if (!pListPtr || pListPtr->length == 0) return TRUE;
    for (int i = 0; i < pListPtr->length; i++) {
	if(!currentGalaxy->setState(name, pListPtr->array[i].name,
				    pListPtr->array[i].value)) return FALSE;
    }
    return TRUE;
}

// connect
extern "C" boolean
KcConnect(char *inst1, char *t1, char *inst2, char *t2, int delay) {
    return currentGalaxy->connect(inst1, t1, inst2, t2, delay);
}

// create a galaxy formal terminal
extern "C" boolean
KcAlias(char *fterm, char *inst, char *aterm) {
    return currentGalaxy->alias(fterm, inst, aterm);
}

// start a galaxy definition
extern "C" boolean
KcDefgalaxy(char *galname) {
    saveGalaxy = currentGalaxy;
    currentGalaxy = new InterpGalaxy;
    currentGalaxy->setBlock(galname, saveGalaxy);
    return (TRUE);
}

extern "C" boolean
KcEndDefgalaxy() {
// add to the knownlist for the current domain
    currentGalaxy->addToKnownList(KnownBlock::domain());
    currentGalaxy = saveGalaxy;
    return (TRUE);
}

// Run the universe
extern "C" boolean
KcRun(int n) {
    if (!universe->initSched())
	return FALSE;
    universe->setStopTime(n);
    universe->run();
    universe->endSimulation();
    return (TRUE);
}

/* 5/17/90
Inputs: name = name of star or galaxy (sog)
Outputs: return = whether name is known
*/
extern "C" boolean
KcIsKnown(char *class) {
    return ((KnownBlock::find(class) == 0) ? FALSE : TRUE);
}

/* 5/17/90
Get information about the portholes of a sog.
Inputs: name = name of sog
Outputs: terms = list of info about each porthole

Changed to support multiPortHoles, 7/24/90
*/
extern "C" boolean
KcGetTerms(char* name, TermList* terms)
{
    Block *block;
    if ((block = KnownBlock::find(name)) == 0) {
	return (FALSE);
    }
    char *names[TERM_ARR_MAX];
    int isOut[TERM_ARR_MAX];
    int n = block->portNames(names, isOut, TERM_ARR_MAX);
    int nm = block->multiPortNames(names+n, isOut+n, TERM_ARR_MAX-n);
    terms->in_n = 0;
    terms->out_n = 0;
    for (int i=0; i < n + nm; i++) {
	if (isOut[i]) {
	    terms->out[terms->out_n].name = names[i];
	    terms->out[terms->out_n++].multiple = (i >= n);
	} else {
	    terms->in[terms->in_n].name = names[i];
	    terms->in[terms->in_n++].multiple = (i >= n);
	}
    }
    return (TRUE);
}

/*
Get default params of a block.

Inputs:
    name = name of block to get params of
    pListPtr = the address of a ParamList node
Outputs:
    return = TRUE if ok, else FALSE and pListPtr->length == 0
        returns FALSE if star does not exist (or invalid param string).
    pListPtr = returns a filled ParamList node that contains params
        if the star has any.  Callers can free() memory in ParamList
        array when no longer needed.
*/
extern "C" boolean
KcGetParams(char* name, ParamListType* pListPtr)
{
    Block *block = KnownBlock::find(name);
    if (block == 0) {
	return (FALSE);
    }
    int n = block->numberStates();
    pListPtr->length = n;
    pListPtr->array = new ParamStruct[n];
    for (int i = 0; i < n; i++) {
	    State& s = block->nextState();
	    pListPtr->array[i].name = s.readName();
	    pListPtr->array[i].value = s.getInitValue();
    }
    return (TRUE);
}

/*
Inputs: name = name of block to get info of
    info = adr of (char *) for return value
Outputs: info = points to info string, free string when done
*/
extern "C" boolean
KcInfo(char* name, char** info)
{
    Block* b = KnownBlock::find(name);
    if (!b) {
	ErrAdd("Unknown block");
	return (FALSE);
    }
    *info = savestring((char *)b->printVerbose());
    return (TRUE);
}

/*
Load object file dynamically into kernel.
*/
extern "C" boolean
KcLoad(char* file)
{
    ErrAdd("Load not implemented yet");
    return (FALSE);
}
