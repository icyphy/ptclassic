static const char file_id[] = "kernelCalls.cc";
/* 
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


Ptolemy kernel calls.
Some code borrowed from Interpreter.cc, see this file for more info.

Programmer: J. Buck, E. Goei
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
#include "pt_fstream.h"
#include <ACG.h>
#include <signal.h>
#include <ctype.h>

extern ACG* gen;

static InterpUniverse *universe = NULL;  // Universe to execute
static InterpGalaxy *currentGalaxy = NULL;  // current galaxy to make things in
static InterpGalaxy *saveGalaxy = NULL;  // used to build galaxies
static Target *galTarget = NULL;	 // target for a galaxy

// Define a stream for logging -- log output to pigiLog.pt

pt_ofstream LOG;

extern "C" boolean
KcInitLog(const char* file) {
	LOG.open(file);
	return LOG ? TRUE : FALSE;
}

// a SafeTcl is just a const char* with a different way of printing.
class SafeTcl {
	friend ostream& operator<<(ostream&,const SafeTcl&);
private:
	const char* str;
public:
	SafeTcl(const char* s) : str(s) {}
};

// Write a string to a stream so it remains as "one thing"
ostream& operator<<(ostream& o,const SafeTcl& stcl) {
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
// or
// SomeStar.state=value
// they may be cascaded, for up to MAX_NUM_FIELDS fields:
// Star.a=b.c=d.e=f
// Values may be quoted like "foo" or 'bar'
// Star.a="1.0".b='a b c'
//
// The first field represents either the name of a multiporthole
// or the name of a state.  The second field represents either
// the number of ports within the multiport or the value of the
// state.
// parseClass does not distinguish the two cases; it only separates
// out the field names and values.

// First argument: pointer to the name to be parsed
// Second argument: reference, for returning the number of fields.
// Third argument: pointer to an array of char* pointers that
// get set with the names of the fields: port or state names.
// Fourth argument: pointer to an array of char* pointers that
// get set with the values for the fields.
// The third and fourth can be omitted, to avoid returning the fields.
// Return value: pointer to the stem, or class name.
// 0 is returned if there is a syntax error.

#define MAX_NUM_FIELDS 20

const char quotes[] = "\'\"";

static const char* parseClass (const char* name, int& nf, char** fieldNames=0,
			       char** fieldValues=0)
{
	static char buf[1024];
	const char* cp = strchr(name,'.');
	nf = 0;
	if (cp == 0 || strchr(cp,'=') == 0) return name;
	int l = cp - name;
	strcpy(buf,name);
	buf[l] = 0;

	// parse a series of name=value.name=value extensions.
	// all pointers point into buf, we add null characters to
	// split fields.

	char * p = buf + l + 1;
	while (nf < MAX_NUM_FIELDS) {
		if (fieldNames) fieldNames[nf] = p;
		p = strchr(p,'=');
		if (p == 0) return 0;
		*p++ = 0;
		char c = *p;
		// see if this is a quote character, if so, look for
		// a matching close quote.
		if (strchr(quotes,c)) {
			if (fieldValues) fieldValues[nf++] = p+1;
			else nf++;
			p = strchr(p+1,c);
			if (p == 0) return 0;
			*p++ = 0;
			if (*p == 0) return buf;
			else if (*p != '.') return 0;
			p++;
		}
		else {
			// normal case, dot or end of string ends value.
			if (fieldValues) fieldValues[nf++] = p;
			else nf++;
			p = strchr(p,'.');
			if (p == 0) return buf;
			*p++ = 0;
		}
	}
	return 0;
}

// for debugging.
extern "C" void testParse(const char* name) {
	char* names[MAX_NUM_FIELDS];
	char* values[MAX_NUM_FIELDS];
	const char* cname;
	int nf;
	if ((cname = parseClass(name,nf,names,values)) != 0) {
		cerr << "base: " << cname << "\n";
		for (int i = 0; i < nf; i++) {
			cerr << i << "). {" << names[i] << "} {"
			     << values[i] << "}\n";
		}
	}
	else {
		cerr << "parseClass error\n";
	}
	cerr.flush();
}

// Call parseClass from C, returning only the pointer to the class name.
extern "C" const char* callParseClass (char* name) {
	int nf;
	return parseClass (name, nf);
}

// const version of Block::multiPortWithName
static const MultiPortHole* findMPH(const Block* b,const char* name) {
	CBlockMPHIter next(*b);
	const MultiPortHole* m;
	while ((m = next++) != 0) {
		if (strcmp (name, m->name()) == 0)
			return m;
	}
	return NULL;
};

// const version of Block::stateWithName
static const State* findState(const Block* b,const char* name) {
	CBlockStateIter next(*b);
	const State* m;
	while ((m = next++) != 0) {
		if (strcmp (name, m->name()) == 0)
			return m;
	}
	return NULL;
}

// Determine which field names are states and which are multiports
// in a given block.  Return true for success, false if one is missing.

static boolean checkFields(const Block* b,
			   const char** names, int nf, int* isMPH) {
	for(int i=0; i < nf; i++) {
		if (findMPH(b,names[i])) isMPH[i] = 1;
		else if (findState(b,names[i])) isMPH[i] = 0;
		else {
			char buf[80];
			sprintf (buf,
			  "No multiport or state named '%s' in class '%s'",
				 names[i], b->className());
			ErrAdd (buf);
			return FALSE;
		}
	}
	return TRUE;
}

// Find a class.  Handle Printer.input=2.  Fail if any name is bogus.
static const Block* findClass (const char* name) {
	char* names[MAX_NUM_FIELDS];
	int isMPH[MAX_NUM_FIELDS];
	int nf;
	const char* c = parseClass (name, nf, names);
	if (!c) return 0;
	const Block* b = KnownBlock::find (c);
	if (!b) return b;
	return checkFields(b,names,nf,isMPH) ? b : 0;
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
	char* names[MAX_NUM_FIELDS];
	char* values[MAX_NUM_FIELDS];
	int isMPH[MAX_NUM_FIELDS];
	int nf;
	const char* cname = parseClass (ako, nf, names, values);
	if (!cname) return FALSE;
	const Block* b = KnownBlock::find(cname);
	if (!b || !checkFields(b,names,nf,isMPH)) return FALSE;
	if (!cname || !currentGalaxy->addStar(name, cname))
		return FALSE;
 	LOG << "\tstar " << SafeTcl(name) << " " << SafeTcl(cname) << "\n";
	for(int j = 0; j < nf; j++ ) {
		if (isMPH[j]) {
			int nP = atoi(values[j]);
			LOG << "\tnumports " << SafeTcl(name) << " " 
			    << names[j] << " " << nP << "\n";
			if (!currentGalaxy->numPorts (name, names[j], nP))
				return FALSE;
		}
		else {
			LOG << "\tsetstate " << SafeTcl(name) << " "
			    << SafeTcl(names[j]) << " "
			    << SafeTcl(values[j]) << "\n";
			if (!currentGalaxy->setState(name,names[j],
						     hashstring(values[j])))
				return FALSE;
		}
	}
	if (!pListPtr || pListPtr->length == 0) return TRUE;
	for (int i = 0; i < pListPtr->length; i++) {
		LOG << "\tsetstate " << SafeTcl(name) << " " <<
			SafeTcl(pListPtr->array[i].name) << " " <<
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
	LOG << "\tnewstate " << SafeTcl(name) << " " << type << " " << 
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
	LOG << SafeTcl(inst1) << " " << SafeTcl(t1) << " "
	    << SafeTcl(inst2) << " " << SafeTcl(t2);
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
	LOG << "\talias " << SafeTcl(fterm) << " " << SafeTcl(inst)
	    << " " << SafeTcl(aterm) << "\n";
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
	LOG << "defgalaxy " << SafeTcl(galname) << " {\n\tdomain "
	    << domain << "\n";
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
		currentGalaxy->setDescriptor(hashstring(desc));
	}
	else currentGalaxy->setDescriptor(dummyDesc);
}

// Run the universe
extern "C" boolean
KcRun(int n) {
 	LOG << "run " << n << "\nwrapup\n";
	LOG.flush();
	SimControl::clearHalt();
	universe->initTarget();
	if (SimControl::haltRequested())
		return FALSE;
	universe->setStopTime(n);
	universe->run();
	universe->wrapup();
	return TRUE;
}

extern "C" boolean LookAtFile(const char*);

// Display the schedule.  Must run the universe first.
extern "C" boolean
KcDisplaySchedule() {
 	// LOG << "run " << n << "\nwrapup\n";
	// LOG.flush();
	if ( galTarget == NULL ) {
	    cerr << "No current target";
	    return FALSE;
	}
	StringList name;
	name << "~/schedule." << universe->name();
	pt_ofstream str(name);
	if (str) {
		str << galTarget->displaySchedule();
		str.close();
		return LookAtFile(name);
	}
	return FALSE;
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
	return !KnownBlock::isDynamic(b->name());
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
	char* mphname[MAX_NUM_FIELDS];
	char* npspec[MAX_NUM_FIELDS];
	int isMPH[MAX_NUM_FIELDS];
	const char* cname;
	int nf;

	cname = parseClass (name, nf, mphname, npspec);
	
	if (!cname || (block = findClass(name)) == 0 ||
	    !checkFields(block,mphname,nf,isMPH)) {
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
	for(j=0; j < nf; j++) {
	    const char *mphName, *mphType;
	    int dir;
	    if (isMPH[j]) {
		// Check that the block has a mph with name mphname[j]
		int position = 0;
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
		int np = atoi(npspec[j]);
		for (int i = 1; i <= np; i++) {
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
	    if(isStringInList(names[n+mphNum], mphname, nf, mpos) &&
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
    names = list of state names to exclude.
                  names on this lists are not returned.
    n_names: how many names.
Outputs:
    return = TRUE if ok (no error), else FALSE.
        returns FALSE if star does not exist or new fails to allocate memory.
    pListPtr = returns a filled ParamList node that contains params
        if the star has any.  Callers can free() memory in ParamList
        array when no longer needed.
*/

static boolean
realGetParams(const Block* block, ParamListType* pListPtr,char** names,
	      int n_names)
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
		    int pos;
		    const State* s = nexts++;
		    // Only return settable states that are not in
		    // the names list.
		    if ((s->attributes() & AB_SETTABLE) &&
			!isStringInList(s->name(),names,n_names,pos)) {
		        tempArray[j].name = s->name();
			tempArray[j].type = s->type();
		        tempArray[j++].value = s->initValue();
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
	char *stateNames[MAX_NUM_FIELDS];
	int nf;
	const char* cname = parseClass(name, nf, stateNames);
	return realGetParams(KnownBlock::find(cname),
			     pListPtr, stateNames, nf);
}

/* like the above, except that it is for a target */
extern "C" boolean
KcGetTargetParams(char* name, ParamListType* pListPtr) {
	return realGetParams(KnownTarget::find(name), pListPtr, 0, 0);
}

/* modify parameters of a target */
extern "C" boolean
KcModTargetParams(ParamListType* pListPtr) {
	if (!galTarget) return TRUE;
	for (int i = 0; i < pListPtr->length; i++) {
		const char* n = pListPtr->array[i].name;
		const char* v = pListPtr->array[i].value;
		LOG << "\ttargetparam " << SafeTcl(n) << " " << SafeTcl(v) << "\n";
		State* s = galTarget->stateWithName(n);
		if (s == 0) {
			Error::abortRun (*galTarget, "no target-state named ", n);
			return FALSE;
		}
		s->setInitValue(hashstring(v));
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
	StringList msg = b->print(0);
	*info = savestring(msg);
	return TRUE;
}

/* 10/5/90 
 Input: the name of a star in the current domain
 Pops up a window displaying the profile and returns true if all goes well,
 otherwise returns false.
*/
static void displayStates(const Block *b,char** names,int n_names);

extern "C" int
KcProfile (char* name) {
	char* fieldnames[MAX_NUM_FIELDS];
	int n_fields = 0;
	const Block* b = findClass (name);
	int tFlag = 0;
	if (b) {
		parseClass(name,n_fields,fieldnames);
	}
	else {
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
	if (tFlag && KnownTarget::isDynamic(b->name())
	    || !tFlag && KnownBlock::isDynamic (b->name()))
		accum_string ("Dynamically linked ");
	if (tFlag) {
		accum_string ("Target: ");
		accum_string (name);
		accum_string ("\n");
	}
	else if (b->isItWormhole()) {
		// get the first line put out by printVerbose.
		StringList pv = b->print(0);
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
	accum_string(b->isItAtomic() ? "Star: " : "Galaxy: ");
	accum_string (name);
	accum_string (" (");
	accum_string (b->domain());
	accum_string (")\n");
	const char* desc = b->descriptor();
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
	displayStates(b,fieldnames,n_fields);
	pr_accum_string ();
	return TRUE;
}

// display settable states.  Omit those in the "names" list.
static void displayStates(const Block *b,char** names,int n_names) {
	if (b->numberStates()) accum_string ("Settable states:\n");
	CBlockStateIter nexts(*b);
	const State *s;
	int pos;
	while ((s = nexts++) != 0) {
		if ((s->attributes() & AB_SETTABLE) == 0 ||
		    isStringInList(s->name(),names,n_names,pos))
			continue;
		accum_string ("   ");
		accum_string (s->name());
		accum_string (" (");
		accum_string (s->type());
		accum_string ("): ");
		const char* d = s->descriptor();
		if (d && strcmp (d, s->name()) != 0) {
			accum_string (d);
			accum_string (": ");
		}
		accum_string ("default = ");
		const char* v = s->initValue();
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
        return Domain::number();
}

/* 9/22/90, by eal
Return the name of the nth domain in the list of known domains
*/
extern "C" const char*
nthDomainName(int n) {
        return Domain::nthName(n);
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
	int temp = universe->newTarget (hashstring(targetName));
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

extern "C" const char*
KcExpandPathName(const char* name) {
    return expandPathName(name);
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
