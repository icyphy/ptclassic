static const char file_id[] = "kernelCalls.cc";
/* 
Version identification:
$Id$
Ptolemy kernel calls.
Some code borrowed from Interpreter.cc, see this file for more info.

Programmer: J. Buck, E. Goei

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.
*/

#include "kernelCalls.h"
#include "InterpUniverse.h"
#include "StringList.h"
#include "Scheduler.h"
#include "SimControl.h"
#include "miscFuncs.h"
#include "Domain.h"
#include "Target.h"
#include "KnownTarget.h"
#include "ConstIters.h"
#include "UserOutput.h"
#include <ACG.h>
#include <signal.h>
#include <ctype.h>

extern ACG* gen;

static InterpUniverse *universe = NULL;  // Universe to execute
static InterpGalaxy *currentGalaxy = NULL;  // current galaxy to make things in
static InterpGalaxy *saveGalaxy = NULL;  // used to build galaxies
static Target *galTarget = NULL;	 // target for a galaxy

// Define a stream for logging -- log output to pigiLog.pt

UserOutput LOG;

extern "C" boolean
KcInitLog(const char* file) {
	if (!LOG.fileName(file)) {
		LOG.fileName("/dev/null");
		return FALSE;
	}
	return TRUE;
}

// a SafeTcl is just a const char* with a different way of printing.
class SafeTcl {
	friend UserOutput& operator<<(UserOutput&,const SafeTcl&);
private:
	const char* str;
public:
	SafeTcl(const char* s) : str(s) {}
};

// Write a string to the log file so it remains as "one thing"
UserOutput& operator<<(UserOutput& o,const SafeTcl& stcl) {
	const char* str = stcl.str;
	if (!str || *str == 0) { return o << "\"\"";}
	// check if we need {} or "".
	const char* s = str;
	char c;
	int special = 0;
	while ((c = *s++) != 0) {
		if (isalnum(c) || c == '.' || c == '_') continue;
		if (strchr("\"\\[$\n", c) != 0) {
			return o << "{" << str << "}";
		}
		special++;
	}
	if (special) return o << "\"" << str << "\"";
	return o << str;
}

extern "C" void KcLog(const char* str) { LOG << str; }

// Write a value (such as a parameter value) to the LOG file
// in a way that is safe for
// Parse a classname
// We allow classnames to be specified as, say
// Printer.input=2
// This means to make a Printer star and to create two portholes
// within the MultiPortHole named "input".  For now, there may
// be only one multiporthole name specified.
//
// The returned name and mphname are in static buffers and will be
// overwritten by the next call.
// First argument: pointer to the name to be parsed
// Second argument: pointer to a char* array that on return, will point
//	to the name of the multiPortHole (mph).
// Third argument: reference to an integer that on return will contain
//	the number of portHoles to instantiate.
// Return value: pointer to the stem, or class name.

#define MAX_NUM_MULTS 4

static const char* parseClass (const char* name, const char* mph[], int nP[]) {
	static char buf[128], buf2[MAX_NUM_MULTS][128], buf3[32];
	char *p;
	p = buf;

	for( int i = 0; i < MAX_NUM_MULTS; i++) {
	    mph[i] = buf2[i];
	    buf2[i][0] = 0;
	    nP[i] = 0;
	}
	while (*name && *name != '.') *p++ = *name++;
	*p = 0;
	if (*name == 0) return buf;

	// we have a . field.
	for(int mphCounter = 0; mphCounter < MAX_NUM_MULTS; mphCounter++) {
	    name++;
	    p = buf2[mphCounter];
	    while (*name && *name != '=')
		*p++ = *name++;
	    *p = 0;

	    // do we have a = field?
	    if (*name == '=') {
		name++;
		nP[mphCounter] = atoi (name);
	    }
	    // skip to the next '.' or end of string
	    while (*name && *name != '.') name++;
	    if(*name == 0) return buf;
	}
	if(*name == '.') {
            ErrAdd ("Too many multiPortHoles specified");
	    return 0;
	}
	return buf;
}

// Call parseClass from C, returning only the pointer to the class name.
extern "C" const char* callParseClass (char* name) {
	const char* junkChar[MAX_NUM_MULTS];
	int junkInt[MAX_NUM_MULTS];
	return parseClass (name, junkChar, junkInt);
}

// const version of Block::multiPortWithName
static const MultiPortHole* findMPH(const Block* b,const char* name) {
	CBlockMPHIter next(*b);
	const MultiPortHole* m;
	while ((m = next++) != 0) {
		if (strcmp (name, m->readName()) == 0)
			return m;
	}
	return NULL;
};

// Find a class.  Handle Printer.input=2.  Fail if any mphname is bogus.
static const Block* findClass (const char* name) {
	const char* mph[MAX_NUM_MULTS];
	int nP[MAX_NUM_MULTS];
	const char* c = parseClass (name, mph, nP);
	if (!c) return 0;
	const Block* b = KnownBlock::find (c);
	if (!b) return b;
	for(int i=0; i < MAX_NUM_MULTS; i++)
	    if(nP[i] && !findMPH(b,mph[i])) {
	        char buf[80];
	        sprintf (buf, "No multiport named '%s' in class '%s'",
				mph[i], c);
	        ErrAdd (buf);
	        return 0;
	    }
	return b;
}

// This function writes the domain to the log file, if and only if
// it has changed.

static void logDomain() {
	static const char* oldDom = "";
	const char* dom = KnownBlock::domain();
	if (strcmp(dom, oldDom) == 0) return;
	LOG << "domain " << dom << "\n";
}

// Return the domain of an object: note, it may not be the current
// domain (e.g. SDF in DDF)
extern "C" const char *
KcDomainOf(char* name) {
	const Block* b = findClass (name);
	if (!b) {
		ErrAdd("Unknown block");
		return NULL;
	}
	return b->domain();
}

// Delete the universe and make another
extern "C" void
KcClearUniverse(const char* name) {
	LOG_DEL; delete universe;
	LOG_NEW; universe = new InterpUniverse(name);
	currentGalaxy = universe;
	LOG << "reset\n";
	LOG << "# Creating universe '" << name << "'\n";
	logDomain();
}

// Create a new instance of star or galaxy and set params for it
extern "C" boolean
KcInstance(char *name, char *ako, ParamListType* pListPtr) {
	int nP[MAX_NUM_MULTS];
	const char* mph[MAX_NUM_MULTS];
	const char* cname = parseClass (ako, mph, nP);
	if (!cname || !currentGalaxy->addStar(name, cname))
		return FALSE;
 	LOG << "\tstar " << name << " " << cname << "\n";
	for(int j = 0; j < MAX_NUM_MULTS; j++ ) {
	    if (nP[j] && !currentGalaxy->numPorts (name, mph[j], nP[j]))
		return FALSE;
	    if (nP[j])
		LOG << "\tnumports " << name << " " << mph[j] << " "
			<< nP[j] << "\n";
	}
	if (!pListPtr || pListPtr->length == 0) return TRUE;
	for (int i = 0; i < pListPtr->length; i++) {
		LOG << "\tsetstate " << name << " " <<
			pListPtr->array[i].name << " " <<
			SafeTcl(pListPtr->array[i].value) << "\n";
		if(!currentGalaxy->setState(name, pListPtr->array[i].name,
			pListPtr->array[i].value)) return FALSE;
	}
	return TRUE;
}

// create a new state for the galaxy
extern "C" boolean
KcMakeState(char *name, char *type, char *initVal) {
// first check to see if state type is known
	if (!KnownState::find(type)) {
		StringList msg = ": parameter '";
		msg += name;
		msg += "' has unknown type: '";
		msg += type;
		msg += "'\nThe following parameter types are known:\n";
		msg += KnownState::nameList();
		Error::abortRun(*currentGalaxy, msg);
		return FALSE;
	}
	LOG << "\tnewstate " << name << " " << type << " " << 
		SafeTcl(initVal) << "\n";
	return currentGalaxy->addState(name, type, initVal);
}

// connect or busconnect
extern "C" boolean
KcConnect(char *inst1, char *t1, char *inst2, char *t2, char* delay, char* width) {
	if (width == 0) width = "";
	if (delay == 0) delay = "";
	if (*width) {
		LOG << "\tbusconnect ";
	}
	else {
		LOG << "\tconnect ";
	}
	LOG << inst1 << " " << t1 << " " << inst2 << " " << t2;
	if (*width)
		LOG << " " << SafeTcl(width);
 	if (*delay)
 		LOG << " " << SafeTcl(delay);
 	LOG << "\n";
	if (*width)
		return currentGalaxy->busConnect(inst1, t1, inst2, t2,
						 width, delay);
	else
		return currentGalaxy->connect(inst1, t1, inst2, t2, delay);
}

// create a galaxy formal terminal
extern "C" boolean
KcAlias(char *fterm, char *inst, char *aterm) {
	LOG << "\talias " << fterm << " " << inst << " " << aterm << "\n";
	return currentGalaxy->alias(fterm, inst, aterm);
}

/* Given the name of a domain, set
   the KnownBlocks currentDomain to correspond to the this domain.
   Returns false if this fails.
        EAL, 9/23/90
*/

extern char DEFAULT_DOMAIN[];

extern "C" boolean
KcSetKBDomain(const char* domain) {
	if (!domain) domain = DEFAULT_DOMAIN;
	return KnownBlock::setDomain(domain) ? TRUE : FALSE;
}

/* 9/22/90, by eal
Return the name of the current domain in KnownBlock
*/
extern "C" const char*
curDomainName() {
        return KnownBlock::domain();
}

// start a galaxy definition
extern "C" boolean
KcDefgalaxy(const char *galname, const char *domain, const char* innerTarget) {
	logDomain();
	LOG << "defgalaxy " << galname << " {\n\tdomain " << domain << "\n";
	if (innerTarget && strcmp(innerTarget, "<parent>") != 0) {
		LOG << "target " << innerTarget << "\n";
		galTarget = KnownTarget::clone(innerTarget);
	}
	else galTarget = 0;
	saveGalaxy = currentGalaxy;
	LOG_NEW; currentGalaxy = new InterpGalaxy(galname);
	currentGalaxy->setBlock(galname, saveGalaxy);
	// Set the domain of the galaxy
	return currentGalaxy->setDomain(domain);
}

extern "C" boolean
KcEndDefgalaxy(const char* outerDomain) {
	//
	// add to the knownlist for the outer domain, and create a
	// wormhole if that is different from current domain.
	// note that this call also restores the KnownBlock::currentDomain
	// to equal the outerDomain.
	LOG << "}\n";
	currentGalaxy->addToKnownList(outerDomain,galTarget);

	currentGalaxy = saveGalaxy;
	return TRUE;
}

static char dummyDesc[] =
"Galaxy created by VEM\nTo set the descriptor for this galaxy, do look-inside, then edit-comment";

// set the descriptor of the current galaxy.
extern "C" void
KcSetDesc(const char* desc) {
	if (desc && *desc) {
		currentGalaxy->setDescriptor(savestring(desc));
	}
	else currentGalaxy->setDescriptor(dummyDesc);
}

// Run the universe
extern "C" boolean
KcRun(int n) {
 	LOG << "run " << n << "\nwrapup\n";
	LOG.flush();
	if (!universe->initSched())
		return FALSE;
	universe->setStopTime(n);
	universe->run();
	universe->endSimulation();
	return TRUE;
}

// Set the seed for the random number generation.
extern "C" void
KcEditSeed(int n) {
 	LOG << "seed " << n << "\n";
	LOG_DEL; if (gen) delete gen;
	LOG_NEW; gen = new ACG(n);
}

/* 5/17/90
Inputs: name = name of star or galaxy (sog)
Outputs: return = whether name is known
*/
extern "C" boolean
KcIsKnown(char *className) {
	return findClass(className) ? TRUE : FALSE;
}

/* 3/28/91
Return TRUE if name is a compiled-in star, false if static, derived from
InterpGalaxy (meaning it is legal to replace it), or unknown.
*/
extern "C" boolean
KcIsCompiledInStar(char *className) {
	const Block* b = findClass(className);
	if (b == 0 || b->isA("InterpGalaxy")) return FALSE;
	return !KnownBlock::isDynamic(b->readName());
}

/* 12/22/91 - by Edward A. Lee
   Return TRUE if the string is in the list of strings.
*/
static boolean
isStringInList(const char* string, const char* list[],
		int listLen, int &position) {
	for(int i=0; i < listLen; i++) {
		if(strcmp(string, list[i]) == 0) {
			position = i;
		 	return TRUE;
		}
	}
	return FALSE;
}

/* 5/17/90
Get information about the portholes of a sog.
Inputs: name = name of sog
Outputs: terms = list of info about each porthole

Changed to support multiPortHoles, 7/24/90
Changed to add info on data types, 10/5/90
Changed to support several multiPortHole clusters, 12/22/91
*/
extern "C" boolean
KcGetTerms(char* name, TermList* terms)
{
	const Block *block;
	const char* mphname[MAX_NUM_MULTS];
	int npspec[MAX_NUM_MULTS];
	const char* cname;

	cname = parseClass (name, mphname, npspec);

	if (!cname || (block = findClass(name)) == 0) {
		char buf[80];
		sprintf (buf, "Invalid Galaxy Name '%s' (interpreted as '%s')",
			name, cname);
		ErrAdd (buf);
		return FALSE;
	}
	const char *names[MAX_NUM_TERMS];
	const char *newNames[MAX_NUM_TERMS];
	const char *types[MAX_NUM_TERMS];
	const char *newTypes[MAX_NUM_TERMS];
	int isOut[MAX_NUM_TERMS];
	int newIsOut[MAX_NUM_TERMS];

	int n = block->portNames(names, types, isOut, MAX_NUM_TERMS);
	int nm = block->multiPortNames(names+n, types+n,
				       isOut+n, MAX_NUM_TERMS-n);
	int numOrdPorts = n;
	// Copy all the names of the ordinary portHoles
	int newNameCount = 0;
	for(int j=0; j<numOrdPorts; j++) {
		newNames[newNameCount] = names[j];
		newTypes[newNameCount] = types[j];
		newIsOut[newNameCount++] = isOut[j];
	}
	// For each multiPortHole, create newNames
	for(j=0; j < MAX_NUM_MULTS; j++) {
	    const char *mphName, *mphType;
	    int dir;
	    if (npspec[j]) {
		// Check that the block has a mph with name mphname[j]
		int position;
		if(isStringInList(mphname[j], &(names[n]), nm, position)) {
			mphName = names[n+position];
			mphType = types[n+position];
			dir = isOut[n+position];
		} else {
			char buf[80];
			sprintf (buf, "No multiport named '%s' in class '%s'",
				mphname[j], cname);
			ErrAdd (buf);
			return FALSE;
		}
		// Create the new names
		for (int i = 1; i <= npspec[j]; i++) {
			char buf[128];
			sprintf (buf, "%s#%d", mphName, i);
			newNames[newNameCount] = savestring (buf);
			newIsOut[newNameCount] = dir;
			newTypes[newNameCount++] = mphType;
		}
	    }
	}
	// Since all the multiPortHoles treated above have been converted
	// to ordinary PortHoles,
	numOrdPorts = newNameCount;

	// Now look for any multiPortHoles that were not converted
	for(int mphNum = 0; mphNum < nm; mphNum++) {
	    int mpos;
	    if(isStringInList(names[n+mphNum], mphname, MAX_NUM_MULTS, mpos) &&
			npspec[mpos])
		continue;
	    newNames[newNameCount] = names[n+mphNum];
	    newTypes[newNameCount] = types[n+mphNum];
	    newIsOut[newNameCount++] = isOut[n+mphNum];
	}

	terms->in_n = 0;
	terms->out_n = 0;
	for (int i=0; i < newNameCount; i++) {
		if (newIsOut[i]) {
			terms->out[terms->out_n].name = newNames[i];
			terms->out[terms->out_n].type = newTypes[i];
			terms->out[terms->out_n++].multiple =
				(i >= numOrdPorts);
		}
		else {
			terms->in[terms->in_n].name = newNames[i];
			terms->in[terms->in_n].type = newTypes[i];
			terms->in[terms->in_n++].multiple =
				(i >= numOrdPorts);
		}
	}
	return TRUE;
}

/* 7/31/90
Ask if a porthole within a named block is a multiporthole.
*/
extern "C" boolean
KcIsMulti(char* blockname, char* portname)
{
	Block *block = currentGalaxy->blockWithName(blockname);
	if (block == 0) return FALSE;
	return block->multiPortWithName(portname) ? TRUE : FALSE;
}

/*
Get default params of a block, or for a target.
KcGetParams, or KcGetTargetParams

Inputs:
    name = name of block, or target, to get params of
    pListPtr = the address of a ParamList node
Outputs:
    return = TRUE if ok (no error), else FALSE.
        returns FALSE if star does not exist or new fails to allocate memory.
    pListPtr = returns a filled ParamList node that contains params
        if the star has any.  Callers can free() memory in ParamList
        array when no longer needed.
*/

static boolean
realGetParams(const Block* block, ParamListType* pListPtr)
{
	if (block == 0) {
		return FALSE;
	}
	pListPtr->array = NULL; // initialization
	int n = block->numberStates();
	int j = 0; // # of settable states, initialized to 0
	if (n) { 
	    /* Since we don't know beforehand how many of the n states
	       are nonsettable, we temperarily requests n ParamStructs
	       at tempArray. 
	    */
	    LOG_NEW; ParamType* tempArray = new ParamStruct[n];
	    if (!tempArray) {  // Out of memory error
		return FALSE;
	    }
	    CBlockStateIter nexts(*block);
	    for (int i = 0; i < n; i++) {
		    const State* s = nexts++;
		    // Only return settable states
		    if (s->attributes() & AB_SETTABLE) {
		        tempArray[j].name = s->readName();
			tempArray[j].type = s->type();
		        tempArray[j++].value = s->getInitValue();
		    }
	    }
	    /* Now we know there are exactly j settable states.
	       If there are any settable states (j > 0), we requests
	       j ParamStructs at pListPtr->array and copy tempArray
	       to pListPtr->array.
	    */
	    if (j) {
		LOG_NEW; pListPtr->array = new ParamStruct[j];
		if (!pListPtr->array) { // out of memory error
		    return FALSE;
		}
		for (i = 0; i < j; i++)
		    pListPtr->array[i] = tempArray[i];
	    }
	    LOG_DEL; delete tempArray;
	}
	pListPtr->length = j;
	return TRUE;
}

/* Get a parameter list for a star or galaxy */
extern "C" boolean
KcGetParams(char* name, ParamListType* pListPtr) {
	return realGetParams(findClass(name), pListPtr);
}

/* like the above, except that it is for a target */
extern "C" boolean
KcGetTargetParams(char* name, ParamListType* pListPtr) {
	return realGetParams(KnownTarget::find(name), pListPtr);
}

/* modify parameters of a target */
extern "C" boolean
KcModTargetParams(ParamListType* pListPtr) {
	if (!galTarget) return TRUE;
	for (int i = 0; i < pListPtr->length; i++) {
		const char* n = pListPtr->array[i].name;
		const char* v = pListPtr->array[i].value;
		LOG << "\ttargetparam " << n << " " << SafeTcl(v) << "\n";
		State* s = galTarget->stateWithName(n);
		if (s == 0) {
			Error::abortRun (*galTarget, "no target-state named ", n);
			return FALSE;
		}
		s->setValue(savestring(v));
	}
	return TRUE;
}

/*
Inputs: name = name of block to get info of
    info = adr of (char *) for return value
Outputs: info = points to info string, free string when done
*/
extern "C" boolean
KcInfo(char* name, char** info)
{
	const Block* b = findClass(name);
	if (!b) {
		ErrAdd("Unknown block");
		return FALSE;
	}
	StringList msg = b->printVerbose();
	*info = savestring((const char *)msg);
	return TRUE;
}

/* 10/5/90 
 Input: the name of a star in the current domain
 Pops up a window displaying the profile and returns true if all goes well,
 otherwise returns false.
*/
static void displayStates(const Block *b);

extern "C" int
KcProfile (char* name) {
	const Block* b = findClass (name);
	int tFlag = 0;
	if (!b) {
		tFlag = 1;
		b = KnownTarget::find (name);
	}
	if (!b) {
		ErrAdd ("Unknown block: ");
		ErrAdd (name);
		return FALSE;
	}
	clr_accum_string ();
	// if dynamically linked, say so
	if (tFlag && KnownTarget::isDynamic(b->readName())
	    || !tFlag && KnownBlock::isDynamic (b->readName()))
		accum_string ("Dynamically linked ");
	if (tFlag) {
		accum_string ("Target: ");
		accum_string (name);
		accum_string ("\n");
	}
	else if (b->isItWormhole()) {
		// get the first line put out by printVerbose.
		StringList pv = b->printVerbose();
		char* msg = pv.newCopy();
		char* p = strchr(msg, ':');
		if (p) {
			p[1] = 0;
			accum_string (msg);
			accum_string (" ");
		}
		else accum_string ("Wormhole: ");
		LOG_DEL; delete msg;
		accum_string (name);
		accum_string ("\n");
	}
	else if (b->isItAtomic ()) {
		accum_string ("Star: ");
		accum_string (name);
		accum_string (" (");
		accum_string (b->asStar().domain());
		accum_string (")\n");
	}
	else {
		accum_string ("Galaxy: ");
		accum_string (name);
		accum_string (" (");
		accum_string (b->asGalaxy().myDomain);
		accum_string (")\n");
	}
	const char* desc = b->readDescriptor();
	accum_string (desc);
// some descriptors don't end in '\n'
	if (desc[strlen(desc)-1] != '\n')
		accum_string ("\n");
// get termlist
	if (!tFlag) {
	TermList terms;
	KcGetTerms (name, &terms);
	if (terms.in_n) accum_string ("Inputs:\n");
	for (int i = 0; i < terms.in_n; i++) {
		accum_string ("   ");
		accum_string (terms.in[i].name);
		if (terms.in[i].multiple) accum_string (" (multiple)");
		accum_string (": ");
		accum_string (terms.in[i].type);
		accum_string ("\n");
	}
	if (terms.out_n) accum_string ("Outputs:\n");
	for (i = 0; i < terms.out_n; i++) {
		accum_string ("   ");
		accum_string (terms.out[i].name);
		if (terms.out[i].multiple) accum_string (" (multiple)");
		accum_string (": ");
		accum_string (terms.out[i].type);
		accum_string ("\n");
	} // end forloop
	} // end if (!tFlag)
// now do states
	displayStates(b);
	pr_accum_string ();
	return TRUE;
}

static void displayStates(const Block *b) {
	if (b->numberStates()) accum_string ("States:\n");
	CBlockStateIter nexts(*b);
	const State *s;
	while ((s = nexts++) != 0) {
		accum_string ("   ");
		accum_string (s->readName());
		accum_string (" (");
		accum_string (s->type());
		accum_string ("): ");
		const char* d = s->readDescriptor();
		if (d && strcmp (d, s->readName()) != 0) {
			accum_string (d);
			accum_string (": ");
		}
		accum_string ("default = ");
		const char* v = s->getInitValue();
		if (!v) v = "";
		if (strlen (v) > 32) accum_string ("\n        ");
		accum_string (v);
		accum_string ("\n");
	}
}
	
/* 7/25/90
   Make multiple ports in a star multiporthole
 */
extern "C" boolean
KcNumPorts (char* starname, char* portname, int numP) {
	LOG << "\tnumports " << starname << " " << portname << " "
		<< numP << "\n";
	return currentGalaxy->numPorts(starname, portname, numP);
}

/* 9/22/90, by eal
Return the number of domains that the system knows about
*/
extern "C" int
numberOfDomains() {
        return Domain::numberOfDomains();
}

/* 9/22/90, by eal
Return the name of the nth domain in the list of known domains
*/
extern "C" const char*
nthDomainName(int n) {
        return Domain::nthDomainName(n);
}

// add a node with the given name
extern "C" int
KcNode (const char* name) {
	LOG << "\tnode " << name << "\n";
	return currentGalaxy->addNode(name);
}

// connect a porthole to a node
extern "C" int
KcNodeConnect (const char* inst, const char* term, const char* node) {
 	LOG << "\tnodeconnect " << inst << " " << term << " " <<
		node << "\n";
	return currentGalaxy->nodeConnect(inst, term, node);
}

/*
Return a list of targets supported by the current domain.
*/
extern "C" int
KcDomainTargets(const char** names, int nMax) {
	return KnownTarget::getList(KnownBlock::domain(),names,nMax);
}

/*
Set the target for the universe
*/
extern "C" int
KcSetTarget(const char* targetName) {
	LOG << "target " << targetName << "\n";
	int temp = universe->newTarget (savestring(targetName));
	galTarget = universe->myTarget();
	return temp;
}

/*
Return the default target name for the current domain.
*/
extern "C" const char*
KcDefTarget() {
	return KnownTarget::defaultName();
}

/*
Register functions to handle animation.
*/

static SimAction *animatePre = 0, *animatePost = 0;

static void onHighlight(Star* s, const char*) {
	Error::mark(*s);
}

static void offHighlight(Star*, const char*) {
	FindClear();
}

extern "C" void
KcSetAnimationState(int s) {
	if (s) {
	    if (!animatePre) {
		animatePre = SimControl::registerAction(onHighlight,1);
		animatePost = SimControl::registerAction(offHighlight,0);
	    }
        }
	else {
	    if (animatePre) {
		SimControl::cancel(animatePre);
		SimControl::cancel(animatePost);
	    }
	}
}

/* 
Get the state of animation.
*/
extern "C" int
KcGetAnimationState() {
	return (animatePre != 0);
}

// catch signals -- Vem passes the SIGUSR1 signal by default

extern "C" void
KcCatchSignals() {
	SimControl::catchInt(SIGUSR1);
}

// dummy class to clean up at end

class KernelCallsOwner {
public:
	KernelCallsOwner() {}
	~KernelCallsOwner() {
		if (currentGalaxy != universe) {
			LOG_DEL; delete currentGalaxy;
		}
		LOG_DEL; delete universe;
	}
};

// constructor for this guy is called before main; destructor at end.

static KernelCallsOwner kco;
