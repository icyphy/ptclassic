/* edg  5/22/90
Version identification:
$Id$
Ptolemy kernel calls.
Some code borrowed from Interpreter.cc, see this file for more info.


 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.
*/

#include "kernelCalls.h"
#include "KnownBlock.h"
#include "StringList.h"
#include "miscFuncs.h"

static InterpUniverse *universe = NULL;  // Universe to execute
static InterpGalaxy *currentGalaxy = NULL;  // current galaxy to make things in
static InterpGalaxy *saveGalaxy = NULL;  // used to build galaxies

// Define a stream for logging -- log output to pigiLog.pt
#include <ostream.h>

ostream LOG("pigiLog.pt", "w");

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
// Second argument: pointer to a char* that on return, will point
//	to the name of the multiPortHole (mph).
// Third argument: reference to an integer that on return will contain
//	the number of multiPortHoles referenced.
// Return value: pointer to the stem, or class name.

static const char* parseClass (const char* name, const char** mph, int& nP) {
	static char buf[128], buf2[128];
	char *p;
	p = buf;
	*mph = buf2;
	nP = 0;

	buf2[0] = 0;
	while (*name && *name != '.') *p++ = *name++;
	*p = 0;
	if (*name == 0) return buf;
// we have a . field.
	name++;
	p = buf2;
	while (*name && *name != '=')
		*p++ = *name++;
	*p = 0;
// do we have a = field?
	if (*name == '=') {
		name++;
		nP = atoi (name);
	}
	return buf;
}

// Call parseClass from C, returning only the pointer to the class name.
extern "C" char* callParseClass (char* name) {
	const char* junkChar;
	int junkInt;
	return (char*) parseClass (name, &junkChar, junkInt);
}

// Find a class.  Handle Printer.input=2.  Fail if the mphname is bogus.
inline static Block* findClass (const char* name) {
	const char* mph;
	int nP;
	Block* b = KnownBlock::find (parseClass (name, &mph, nP));
	if (!b || nP == 0) return b;
	return b->multiPortWithName (mph) ? b : 0;
}

// Delete the universe and make another
extern "C" void
KcClearUniverse() {
	delete universe;
	universe = new InterpUniverse;
	currentGalaxy = universe;
	LOG << "(reset)\n";
}

// Create a new instance of star or galaxy and set params for it
extern "C" boolean
KcInstance(char *name, char *ako, ParamListType* pListPtr) {
	int nP;
	const char* mph;
	const char* cname = parseClass (ako, &mph, nP);
	if (!currentGalaxy->addStar(name, cname))
		return FALSE;
 	LOG << "\t(star " << name << " " << cname << ")\n";
	if (nP && !currentGalaxy->numPorts (name, mph, nP))
		return FALSE;
	if (nP)
		LOG << "\t(numports " << name << " " << mph << " "
			<< nP << ")\n";
	if (!pListPtr || pListPtr->length == 0) return TRUE;
	for (int i = 0; i < pListPtr->length; i++) {
		LOG << "\t(setstate " << name << " " <<
			pListPtr->array[i].name << " \"" <<
			pListPtr->array[i].value << "\")\n";
		if(!currentGalaxy->setState(name, pListPtr->array[i].name,
					    pListPtr->array[i].value)) return FALSE;
	}
	return TRUE;
}

// 7/25/90
// create a new state for the galaxy
extern "C" boolean
KcMakeState(char *name, char *type, char *initVal) {
	LOG << "\t(state " << name << " " << type << " \"" <<
		initVal << "\")\n";
	return currentGalaxy->addState(name, type, initVal);
}

// connect
extern "C" boolean
KcConnect(char *inst1, char *t1, char *inst2, char *t2, int delay) {
 	LOG << "\t(connect (" << inst1 << " \"" << t1 << "\") (" <<
 		inst2 << " \"" << t2 << "\")";
 	if (delay)
 		LOG << " " << delay;
 	LOG << ")\n";
	return currentGalaxy->connect(inst1, t1, inst2, t2, delay);
}

// create a galaxy formal terminal
extern "C" boolean
KcAlias(char *fterm, char *inst, char *aterm) {
	LOG << "\t(alias " << fterm << " " << inst << " " << aterm << ")\n";
	return currentGalaxy->alias(fterm, inst, aterm);
}

/* Given the name of a domain, set
   the KnownBlocks currentDomain to correspond to the this domain.
   Returns false if this fails.
        EAL, 9/23/90
*/
extern "C" boolean
KcSetKBDomain(char* domain) {
	LOG << "\t(domain " << domain << ")\n";
	return KnownBlock::setDomain(domain) ? TRUE : FALSE;
}

/* 9/22/90, by eal
Return the name of the current domain in KnownBlock
*/
extern "C" char*
curDomainName() {
        return (char*) KnownBlock::domain();
}

// start a galaxy definition
extern "C" boolean
KcDefgalaxy(char *galname, char *domain) {
	LOG << "(defgalaxy " << galname << "\n\t(domain " << domain << ")\n";
	saveGalaxy = currentGalaxy;
	currentGalaxy = new InterpGalaxy;
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
	LOG << ")\n";
	currentGalaxy->addToKnownList(outerDomain);

	currentGalaxy = saveGalaxy;
	return TRUE;
}

// Run the universe
extern "C" boolean
KcRun(int n) {
 	LOG << "(run " << n << ")\n(wrapup)\n";
	if (!universe->initSched())
		return FALSE;
	universe->setStopTime(n);
	universe->run();
	universe->endSimulation();
	return TRUE;
}

/* 5/17/90
Inputs: name = name of star or galaxy (sog)
Outputs: return = whether name is known
*/
extern "C" boolean
KcIsKnown(char *className) {
	return findClass(className) ? TRUE : FALSE;
}

/* 5/17/90
Get information about the portholes of a sog.
Inputs: name = name of sog
Outputs: terms = list of info about each porthole

Changed to support multiPortHoles, 7/24/90
Changed to add info on data types, 10/5/90
*/
extern "C" boolean
KcGetTerms(char* name, TermList* terms)
{
	Block *block;
	const char* mphname;
	const char* cname;
	int npspec;

	cname = parseClass (name, &mphname, npspec);

	if ((block = findClass(name)) == 0) {
		return FALSE;
	}
	char *names[TERM_ARR_MAX];
	const char *types[TERM_ARR_MAX];
	int isOut[TERM_ARR_MAX];

	int n = block->portNames(names, isOut, TERM_ARR_MAX);
// fill in datatypes
	for (int i = 0; i < n; i++) {
		PortHole* p = block->portWithName (names[i]);
		if (p) types[i] = p->myType();
	}
	int nm = block->multiPortNames(names+n, isOut+n, TERM_ARR_MAX-n);
	for (i = 0; i < nm; i++) {
		MultiPortHole* p = block->multiPortWithName (names[n+i]);
		if (p) types[n+i] = p->myType();
	}
	if (npspec) {
		if (nm == 0 || nm == 1 && strcmp (names[n], mphname) != 0) {
			char buf[80];
			sprintf (buf, "No multiport named '%s' in class '%s'",
				 mphname, cname);
			ErrAdd (buf);
			return FALSE;
		}
		else if (nm > 1) {
			ErrAdd ("Case not yet implemented: more than one MPH");
			return FALSE;
		}
		else {
			char* mphname = names[n];
			int dir = isOut[n];
			const char* mphtype = types[n];
			for (int i = 1; i <= npspec; i++) {
				char buf[128];
				sprintf (buf, "%s#%d", mphname, i);
				names[n+i-1] = savestring (buf);
				isOut[n+i-1] = dir;
				types[n+i-1] = mphtype;
			}
			n += npspec;
			nm = 0;
		}
	}
	terms->in_n = 0;
	terms->out_n = 0;
	for (i=0; i < n + nm; i++) {
		if (isOut[i]) {
			terms->out[terms->out_n].name = names[i];
			terms->out[terms->out_n].type = types[i];
			terms->out[terms->out_n++].multiple = (i >= n);
		}
		else {
			terms->in[terms->in_n].name = names[i];
			terms->in[terms->in_n].type = types[i];
			terms->in[terms->in_n++].multiple = (i >= n);
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
	Block *block = findClass(name);
	if (block == 0) {
		return FALSE;
	}
	int n = block->numberStates();
	pListPtr->length = n;
	pListPtr->array = new ParamStruct[n];
	for (int i = 0; i < n; i++) {
		State& s = block->nextState();
		pListPtr->array[i].name = s.readName();
		pListPtr->array[i].value = s.getInitValue();
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
	Block* b = findClass(name);
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
extern "C" int
KcProfile (char* name) {
	Block* b = findClass (name);
	if (!b) {
		ErrAdd ("Unknown block: ");
		ErrAdd (name);
		return FALSE;
	}
	clr_accum_string ();
	if (b->isItAtomic ()) {
		accum_string ("Star: ");
		accum_string (name);
		accum_string (" (");
		accum_string (b->asStar().domain());
		accum_string (")\n");
	}
	else {
		accum_string ("Compiled-in galaxy: ");
		accum_string (name);
		accum_string (" (");
		accum_string (b->asGalaxy().myDomain);
		accum_string ("\n");
	}
	const char* desc = b->readDescriptor();
	accum_string (desc);
// some descriptors don't end in '\n'
	if (desc[strlen(desc)-1] != '\n')
		accum_string ("\n");
// get termlist
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
	}
// now do states
	if (b->numberStates()) accum_string ("States:\n");
	for (i = b->numberStates(); i>0; i--) {
		State& s = b->nextState();
		accum_string ("   ");
		accum_string (s.readName());
		accum_string (" (");
		accum_string (s.type());
		accum_string ("): ");
		const char* d = s.readDescriptor();
		if (d && strcmp (d, s.readName()) != 0) {
			accum_string (d);
			accum_string (": ");
		}
		accum_string ("default = ");
		const char* v = s.getInitValue();
		if (!v) v = "";
		if (strlen (v) > 32) accum_string ("\n        ");
		accum_string (v);
		accum_string ("\n");
	}
	pr_accum_string ();
	return TRUE;
}

	
/* 7/25/90
   Make multiple ports in a star multiporthole
 */
extern "C" boolean
KcNumPorts (char* starname, char* portname, int numP) {
	LOG << "\t(numports " << starname << " " << portname << " "
		<< numP << ")\n";
	return currentGalaxy->numPorts(starname, portname, numP);
}

/*
Load object file dynamically into kernel.
*/
extern "C" boolean
KcLoad(char* file)
{
	ErrAdd("Load not implemented yet");
	return FALSE;
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
extern "C" char*
nthDomainName(int n) {
        return (char*) Domain::nthDomainName(n);
}
