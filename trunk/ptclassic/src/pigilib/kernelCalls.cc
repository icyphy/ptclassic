/* edg  5/22/90
Version identification:
$Id$
Ptolemy kernel calls.
Some code borrowed from Interpreter.cc, see this file for more info.


 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.
*/


#include "SDFUniverse.h"
#include "InterpGalaxy.h"
#include "KnownBlock.h"
#include "Block.h"
#include "StringList.h"

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
struct Term_s {
    char *name;
    boolean multiple;
};
typedef struct Term_s Term;
struct TermList_s {
    Term in[TERM_ARR_MAX];
    int in_n;
    Term out[TERM_ARR_MAX];
    int out_n;
};
typedef struct TermList_s TermList;

// Parameter structs for pigi
struct ParamStruct {
    char *name;
    char *value;
};
typedef struct ParamStruct ParamType;
struct ParamListStruct {
    int length; /* length of array */
    ParamType *array; /* points to first element */
};
typedef struct ParamListStruct ParamListType;


// An InterpUniverse is an SDFUniverse with an interpreted galaxy in it.
class InterpUniverse : SDFUniverse {
public:
    InterpUniverse (InterpGalaxy *g) {
	setBlock("Universe", NULL);
	addBlock(g->setBlock("mainGalaxy", this));
    }
};

PlasmaList plasmaList;  // This global is needed for the kernel
static InterpUniverse *universe = NULL;  // Universe to execute
static InterpGalaxy *currentGalaxy = NULL;  // current galaxy to make things in
static InterpGalaxy *saveGalaxy = NULL;  // used to build galaxies

// Delete the universe and make another
extern "C" void
KcClearUniverse() {
    if (universe != NULL && currentGalaxy != NULL) {
	delete universe;
	delete currentGalaxy;
    }
    currentGalaxy = new InterpGalaxy;
    universe = new InterpUniverse(currentGalaxy);
}

// Create a new instance of star or galaxy
extern "C" boolean
KcInstance(char *name, char *ako) {
    currentGalaxy->addStar(name, ako);
    return (TRUE);
}

// connect
extern "C" boolean
KcConnect(char *inst1, char *t1, char *inst2, char *t2, int delay) {
    currentGalaxy->connect(inst1, t1, inst2, t2, delay);
    return (TRUE);
}

// create a galaxy formal terminal
extern "C" boolean
KcAlias(char *fterm, char *inst, char *aterm) {
    currentGalaxy->alias(fterm, inst, aterm);
    return (TRUE);
}

extern "C" boolean
KcDefgalaxy(char *galname) {
    saveGalaxy = currentGalaxy;
    currentGalaxy = new InterpGalaxy;
    currentGalaxy->setBlock(galname, saveGalaxy);
    return (TRUE);
}

extern "C" boolean
KcEndDefgalaxy() {
    currentGalaxy->addToKnownList();
    currentGalaxy = saveGalaxy;
    return (TRUE);
}

// Run the universe
extern "C" boolean
KcRun(int n) {
    universe->initialize();
    for (int i = 0; i < n; i++) {
	universe->go();
    }
    universe->wrapup();
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
Caveats: This doesn't support multiportholes yet.
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
    terms->in_n = 0;
    terms->out_n = 0;
    for (int i=0; i < n; i++) {
	if (isOut[i]) {
	    terms->out[terms->out_n].name = names[i];
	    terms->out[terms->out_n++].multiple = FALSE;
	} else {
	    terms->in[terms->in_n].name = names[i];
	    terms->in[terms->in_n++].multiple = FALSE;
	}
    }
    return (TRUE);
}

/* 5/17/90
Get params of a block.
Not implemented yet, must wait for param/state methods in kernel.
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
    /* for now, say that star has no params */
    pListPtr->length = 0;
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
    Block* b = KnownBlock::clone(name);
    if (!b) {
	ErrAdd("Unknown block");
	return (FALSE);
    }
    b->setBlock(name, NULL);
    StringList sl = b->printVerbose;  // sl holds string until it can be copied
    char* s = sl;
    *info = new char[strlen(s) + 1];
    strcpy(*info, s);
    delete b;
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
