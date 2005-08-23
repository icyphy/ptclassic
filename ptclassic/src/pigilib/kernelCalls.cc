static const char file_id[] = "kernelCalls.cc";

/* 
Version identification:
@(#)kernelCalls.cc	2.93	04/23/98

Copyright (c) 1990-1997 The Regents of the University of California.
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


Ptolemy kernel calls.
Some code borrowed from Interpreter.cc, see this file for more info.

Programmer: J. Buck, E. Goei, E. A. Lee
*/

// The order of include files is very tricky here.

#include "compat.h"
#include "InterpUniverse.h"
#include "StringList.h"
#include "Scheduler.h"
#include "SimControl.h"
#include "miscFuncs.h"
#include "Domain.h"
#include "Target.h"
#include "KnownTarget.h"
#include "KnownState.h"
#include "ConstIters.h"
#include "pt_fstream.h"
#include <ACG.h>
#include <signal.h>
#include <ctype.h>

// Include the global ptcl object (contains the universe)
#include "PTcl.h"
extern PTcl *ptcl;  

extern "C" {
#define Pointer screwed_Pointer         /* rpc.h and type.h define Pointer */
#include "err.h"
#include "icon.h"			/* define LookAtFile */
#include "xfunctions.h"
// Although kernelCalls.cc is a C++ file, kernelCalls.h is a C include file
#include "kernelCalls.h"		/* define functions prefixed by Kc */

// We must include ptk.h last because ptk.h includes tk.h which 
// eventually includes X11/X.h, which on Solaris2.4 
// there is a #define Complex 0, which causes no end of trouble.
#include "ptk.h"
#undef Pointer
#include "mkIcon.h"
}

extern ACG* gen;
extern char DEFAULT_DOMAIN[];

///////////////////////////////////////////////////////////////////////
// Define a stream for logging -- log output to pigiLog.pt

pt_ofstream LOG;

extern "C" boolean
KcInitLog(const char* file) {
	LOG.open(file);
	return LOG ? TRUE : FALSE;
}

extern "C" void KcLog(const char* str) { LOG << str; }

extern "C" void KcFlushLog() { LOG.flush(); }

///////////////////////////////////////////////////////////////////////
// a SafeTcl is just a const char* with a different way of printing.
class SafeTcl {
	friend ostream& operator<<(ostream&,const SafeTcl&);
private:
	const char* str;
public:
	SafeTcl(const char* s) : str(s) {}
};

///////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////
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

// Any future extensions to the name syntax will impact code in ganttIfc.c.

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
	const char* clName = b->className();  // FIXME: needed by ACS domain.
	clName = clName; // ACS needs b->className() as a secondary initialization on KnownBlock list.
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
			StringList buf = "No multiport or state named '";
			buf << names[i] << "' in class '"
			    << b->className() << "'";
			ErrAdd(buf);
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
	const Block* b = KnownBlock::find (c, ptcl->curDomain);
	if (!b) return b;
	return checkFields(b,(const char **)names,nf,isMPH) ? b : 0;
}

// This function writes the domain to the log file, if and only if
// it has changed.

static void logDomain() {
	static const char* oldDom = "";
	if (strcmp(ptcl->curDomain, oldDom) == 0) return;
	LOG << "domain " << ptcl->curDomain << "\n";
	oldDom = ptcl->curDomain;
}

///////////////////////////////////////////////////////////////////////
// Return the domain of an object: note, it may not be the current
// domain (e.g. SDF in DDF)
extern "C" const char *
KcDomainOf(const char* name) {
	const Block* b = findClass (name);
	if (!b) {
		ErrAdd("Unknown block");
		return NULL;
	}
	return b->domain();
}

///////////////////////////////////////////////////////////////////////
// Create a new instance of star or galaxy and set params for it
extern "C" boolean
KcInstance(char *name, char *ako, ParamListType* pListPtr) {
	char* names[MAX_NUM_FIELDS];
	char* values[MAX_NUM_FIELDS];
	int isMPH[MAX_NUM_FIELDS];
	int nf;
	const char* cname = parseClass (ako, nf, names, values);
	if (!cname) return FALSE;
	const Block* b = KnownBlock::find(cname, ptcl->curDomain);
	if (!b || !checkFields(b,(const char **)names,nf,isMPH)) return FALSE;
	if (!cname || !ptcl->currentGalaxy->addStar(name, cname))
		return FALSE;
 	LOG << "\tstar " << SafeTcl(name) << " " << SafeTcl(cname) << "\n";
	for(int j = 0; j < nf; j++ ) {
		if (isMPH[j]) {
			int nP = atoi(values[j]);
			LOG << "\tnumports " << SafeTcl(name) << " " 
			    << names[j] << " " << nP << "\n";
			if (!ptcl->currentGalaxy->numPorts (name, names[j], nP))
				return FALSE;
		}
		else {
			LOG << "\tsetstate " << SafeTcl(name) << " "
			    << SafeTcl(names[j]) << " "
			    << SafeTcl(values[j]) << "\n";
			if (!ptcl->currentGalaxy->setState(name,names[j],
						     hashstring(values[j])))
				return FALSE;
		}
	}
	if (!pListPtr || pListPtr->length == 0) return TRUE;
	for (int i = 0; i < pListPtr->length; i++) {
		LOG << "\tsetstate " << SafeTcl(name) << " " <<
			SafeTcl(pListPtr->array[i].name) << " " <<
			SafeTcl(pListPtr->array[i].value) << "\n";
		if(!ptcl->currentGalaxy->setState(name, pListPtr->array[i].name,
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
		Error::abortRun(*ptcl->currentGalaxy, msg);
		return FALSE;
	}
	LOG << "\tnewstate " << SafeTcl(name) << " " << type << " " << 
		SafeTcl(initVal) << "\n";
	return ptcl->currentGalaxy->addState(name, type, initVal);
}

// connect or busconnect
extern "C" boolean
KcConnect(char *inst1, char *t1, char *inst2, char *t2, char* initDelayValues, char* width) {
	if (width == 0) width = "";
	if (initDelayValues == 0) initDelayValues = "";
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
 	if (*initDelayValues)
 		LOG << " " << SafeTcl(initDelayValues);
 	LOG << "\n";
	if (*width)
		return ptcl->currentGalaxy->busConnect(inst1, t1, inst2, t2,
						 width, initDelayValues);
	else
		return ptcl->currentGalaxy->connect(inst1, t1, inst2, t2, initDelayValues);
}

///////////////////////////////////////////////////////////////////////
// create a galaxy formal terminal
extern "C" boolean
KcAlias(char *fterm, char *inst, char *aterm) {
	LOG << "\talias " << SafeTcl(fterm) << " " << SafeTcl(inst)
	    << " " << SafeTcl(aterm) << "\n";
	return ptcl->currentGalaxy->alias(fterm, inst, aterm);
}

///////////////////////////////////////////////////////////////////////
// Given the name of a domain, set ptcl->curDomain, and the domain of the
// current galaxy if it exists, to correspond to this domain.
// Returns false if this fails (invalid domain).

extern "C" boolean
KcSetKBDomain(const char* newdomain) {
	const char* domain =
		newdomain ? hashstring(newdomain) : hashstring(DEFAULT_DOMAIN);

	if (!KnownBlock::validDomain(domain)) {
		Error::abortRun("Invalid domain: ", domain);
		return FALSE;
	}

	// FIXME: this isn't quite right, but can go away when we
	// complete the job of eliminating current domain.  We avoid
	// changing the galaxy domain if it is non-empty.

	// equality can be used here because of hashstring call.
	if (ptcl->currentGalaxy && ptcl->currentGalaxy->numberBlocks() == 0 &&
	    ptcl->currentGalaxy->domain() != domain &&
	    !ptcl->currentGalaxy->setDomain(domain)) {
		return FALSE;
	}

	ptcl->curDomain = domain;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// Return the name of the current domain in KnownBlock
extern "C" const char*
curDomainName() {
	return ptcl->curDomain;
}

///////////////////////////////////////////////////////////////////////
// galaxy definition
extern "C" boolean
KcDefgalaxy(const char *galname, const char *domain, const char* innerTarget) {
	logDomain();
	LOG << "defgalaxy " << SafeTcl(galname) << " {\n";
	LOG_NEW; ptcl->currentGalaxy = new InterpGalaxy(galname,ptcl->curDomain);
	ptcl->currentGalaxy->setBlock(galname, 0);
	// Set the domain of the galaxy
	LOG << "\tdomain " << domain << "\n";
	if (!KcSetKBDomain(domain)) return FALSE;
	if (innerTarget && strcmp(innerTarget, "<parent>") != 0) {
		LOG << "\ttarget " << innerTarget << "\n";
		ptcl->currentTarget = KnownTarget::clone(innerTarget);
	}
	else ptcl->currentTarget = 0;
	return TRUE;
}

extern "C" boolean
KcEndDefgalaxy(const char* definitionSource, const char* outerDomain) {
	//
	// add to the knownlist for the outer domain, and create a
	// wormhole if that is different from current domain.
	// note that this call also restores the current domain
	// to equal the outerDomain.
	LOG << "}\n";
	ptcl->currentGalaxy->addToKnownList(hashstring(definitionSource),
					    outerDomain,ptcl->currentTarget);
	ptcl->currentGalaxy = ptcl->universe;
	return KcSetKBDomain(outerDomain);
}

static char dummyDesc[] =
"Galaxy created by VEM\nTo set the descriptor for this galaxy, do look-inside, then edit-comment";

// set the descriptor of the current galaxy.
extern "C" void
KcSetDesc(const char* desc) {
	if (desc && *desc) {
		ptcl->currentGalaxy->setDescriptor(hashstring(desc));
	}
	else ptcl->currentGalaxy->setDescriptor(dummyDesc);
}

///////////////////////////////////////////////////////////////////////
// Display the schedule.  Must run the universe first.
extern "C" boolean
KcDisplaySchedule() {
 	// LOG << "run " << n << "\nwrapup\n";
	// LOG.flush();
	if ( ptcl->currentTarget == NULL ) {
	    Error::error("No current target");
	    return FALSE;
	}
	StringList name = "~/";
	name << ptcl->universe->name() << ".sched";
	pt_ofstream str(name);
	if (str) {
		str << ptcl->currentTarget->displaySchedule();
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
KcIsKnown(const char *className) {
	return findClass(className) ? TRUE : FALSE;
}

/* 1/30/97
Inputs: basename and domain of an interpreted galaxy.
Outputs: if the Ptolemy kernel has a matching compiled galaxy,
return its definition source string.  Otherwise return NULL.
*/
extern "C" const char*
KcGalaxyDefSource (const char* baseName, const char* domain) {
	// Galaxies are sought on the global knownlist.
	const char* defSource = NULL;
	if (KnownBlock::isDefined(baseName, domain, defSource))
		return defSource;
	return NULL;
}

/* 1/30/97
Inputs: basename of an interpreted universe.
Outputs: if the Ptolemy kernel has a matching compiled universe,
return its definition source string.  Otherwise return NULL.
*/
extern "C" const char*
KcUniverseDefSource (const char* baseName) {
	// Universes are sought in the universe list of the active PTcl object.
	InterpUniverse* u = ptcl->univWithName(baseName);
	if (u != NULL)
		return u->definitionSource();
	return NULL;
}

/* 1/30/97
Set the definition source string for the current universe.
*/
extern "C" void
KcSetUniverseDefSource (const char* pathName) {
	ptcl->universe->setDefinitionSource(pathName);
}

/* 3/28/91
Return TRUE if className is a compiled-in star of the current domain,
FALSE if unknown, derived from InterpGalaxy (meaning it is legal
legal to replace it), or dynamic.
*/
extern "C" boolean
KcIsCompiledInStar(const char *className) {
	const Block* b = findClass(className);
	if (b == 0 || b->isA("InterpGalaxy")) return FALSE;

	// If b is nonzero, it means b is a star that is known, or works,
	// in the current domain.  b could be a star of a subdomain.
	// In that case, we consider b NOT a compiled-in star
	// of the current domain.  This way, we can allow dynamically
	// linking in a star with the same name as a star in a subdomain.
	if (strcmp(b->domain(), ptcl->curDomain)) return FALSE;

	return !KnownBlock::isDynamic(b->name(), ptcl->curDomain);
}

/* 12/22/91 - by Edward A. Lee
   Return TRUE if the string is in the list of strings.
*/
static boolean
isStringInList(const char* string, const char* list[],
		int listLen, int &position) {
	for(int i = 0; i < listLen; i++) {
		if (strcmp(string, list[i]) == 0) {
			position = i;
		 	return TRUE;
		}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////
// Get information about the portholes of a sog.
// Inputs: name = name of sog
// Outputs: newNames, newTypes, newIsOut, numOrdPortsPtr, newNameCountPtr
// See MkGetTerms in mkIcon.c.
extern "C" boolean
KcCheckTerms(const char* name, const char** newNames, const char** newTypes,
	     int* newIsOut, int* numOrdPortsPtr, int* newNameCountPtr)
{
	const Block* block = 0;
	char* mphname[MAX_NUM_FIELDS];
	char* npspec[MAX_NUM_FIELDS];
	int isMPH[MAX_NUM_FIELDS];
	const char* cname;
	int nf;

	cname = parseClass(name, nf, mphname, npspec);

	if (!cname || (block = findClass(name)) == 0 ||
	    !checkFields(block, (const char**)mphname, nf, isMPH)) {
		StringList buf = "Invalid galaxy name '";
		buf << name << "' (interpreted as '" << cname << "')";
		ErrAdd(buf);
		return FALSE;
	}

	const char* names[MAX_NUM_TERMS];
	const char* types[MAX_NUM_TERMS];
	int isOut[MAX_NUM_TERMS];

	int n = block->portNames(names, types, isOut, MAX_NUM_TERMS);
	int nm = block->multiPortNames(names+n, types+n,
				       isOut+n, MAX_NUM_TERMS-n);
	int numOrdPorts = n;
        int j;

	// Copy all the names of the ordinary portHoles
	int newNameCount = 0;
	for (j = 0; j < numOrdPorts; j++) {
		newNames[newNameCount] = names[j];
		newTypes[newNameCount] = types[j];
		newIsOut[newNameCount++] = isOut[j];
	}

	// For each multiPortHole, create newNames
	for (j = 0; j < nf; j++) {
	    const char* mphName;
	    const char* mphType;
	    int dir;
	    if (isMPH[j]) {
		// Check that the block has a mph with name mphname[j]
		int position = 0;
		if (isStringInList(mphname[j], &(names[n]), nm, position)) {
			mphName = names[n+position];
			mphType = types[n+position];
			dir = isOut[n+position];
		} else {
			StringList buf = "No multiport named '";
			buf << mphname[j] << "' in class '" << cname << "'";
			ErrAdd(buf);
			return FALSE;
		}
		// Create the new names
		int np = atoi(npspec[j]);
		for (int i = 1; i <= np; i++) {
			StringList buf = mphName;
			buf << "#" << i;
			newNames[newNameCount] = savestring(buf);
			newIsOut[newNameCount] = dir;
			newTypes[newNameCount++] = mphType;
		}
	    }
	}

	// Since all the multiPortHoles treated above have been converted
	// to ordinary PortHoles,
	numOrdPorts = newNameCount;

	// Now look for any multiPortHoles that were not converted
	for (int mphNum = 0; mphNum < nm; mphNum++) {
	    int mpos = 0;
	    if (isStringInList(names[n+mphNum], (const char**)mphname,
			        nf, mpos) && npspec[mpos])
		continue;
	    newNames[newNameCount] = names[n+mphNum];
	    newTypes[newNameCount] = types[n+mphNum];
	    newIsOut[newNameCount++] = isOut[n+mphNum];
	}

	// Return values
	*numOrdPortsPtr = numOrdPorts;
	*newNameCountPtr = newNameCount;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// Ask if a porthole within a named block is a multiporthole.
extern "C" boolean
KcIsMulti(char* blockname, char* portname)
{
	Block* block = ptcl->currentGalaxy->blockWithName(blockname);
	if (block == 0) return FALSE;
	return block->multiPortWithName(portname) ? TRUE : FALSE;
}

///////////////////////////////////////////////////////////////////////
// Get default params of a block, or for a target.
// KcGetParams, or KcGetTargetParams
// 
// Inputs:
//     name = name of block, or target, to get params of
//     pListPtr = the address of a ParamList node
//     names = list of state names to exclude.
//                   names on this lists are not returned.
//     n_names: how many names.
// Outputs:
//     return = TRUE if ok (no error), else FALSE.
//     returns FALSE if star does not exist or new fails to allocate memory.
//     pListPtr = returns a filled ParamList node that contains params
//     if the star has any.  Callers can free() memory in ParamList
//     array when no longer needed.
static boolean
realGetParams(const Block* block, ParamListType* pListPtr,
	      char** names, int n_names)
{
	if (block == 0) {
	    return FALSE;
	}

	// initialization
	pListPtr->array = 0;
	pListPtr->length = 0;
	pListPtr->dynamic_memory = 0;
	pListPtr->flat_plist_flag = TRUE;
	const char* clName = block->className(); // FIXME: needed by ACS domain.
	clName = clName; // ACS needs b->className() as a secondary initialization on KnownBlock list.

	int n = block->numberStates();
	int j = 0; // # of settable states, initialized to 0
        int i;
	if (n) { 
	    /* Since we don't know beforehand how many of the n states
	       are nonsettable, we temporarily request n ParamStructs
	       at tempArray. 
	    */
	    LOG_NEW; ParamType* tempArray = new ParamStruct[n];
	    if (!tempArray) {  // Out of memory error
		return FALSE;
	    }
	    CBlockStateIter nexts(*block);
	    for (i = 0; i < n; i++) {
		    int pos;
		    const State* s = nexts++;
		    // Only return settable states that are not in
		    // the names list.
		    if ((s->attributes() & AB_SETTABLE) &&
			!isStringInList(s->name(), (const char **)names,
					n_names, pos)) {
			// This cast is okay because tempArray is a "flat"
			// parameter list; i.e., name/type/value are not
			// considered dynamic strings
		        tempArray[j].name = (char *) s->name();
			tempArray[j].type = (char *) s->type();
		        tempArray[j++].value = (char *) s->initValue();
		    }
	    }
	    /* Now we know there are exactly j settable states.
	       If there are any settable states (j > 0), we requests
	       j ParamStructs at pListPtr->array and copy tempArray
	       to pListPtr->array.
	    */
	    if (j) {
	        // Don't use new to create pListp->array, use calloc so that we
	        // can easily free this from within C.
	        // See FreeFlatPList(pListPtr) in paramStructs.c should be
	        // called to free pListp->array.
	        pListPtr->array = (ParamType *) calloc(j, sizeof(ParamType));
		if (!pListPtr->array) { // out of memory error
		    return FALSE;
		}
		for (i = 0; i < j; i++)
		    pListPtr->array[i] = tempArray[i];
	    }
	    LOG_DEL; delete [] tempArray;
	}
	pListPtr->length = j;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// Get a parameter list for a star or galaxy */
extern "C" boolean
KcGetParams(char* name, ParamListType* pListPtr) {
	char *stateNames[MAX_NUM_FIELDS];
	int nf;
	const char* cname = parseClass(name, nf, stateNames);
	return realGetParams(KnownBlock::find(cname, ptcl->curDomain),
			     pListPtr, stateNames, nf);
}

///////////////////////////////////////////////////////////////////////
// like the above, except that it is for a target
extern "C" boolean
KcGetTargetParams(char* name, ParamListType* pListPtr) {
	return realGetParams(KnownTarget::find(name), pListPtr, 0, 0);
}

///////////////////////////////////////////////////////////////////////
// modify parameters of a target
extern "C" boolean
KcModTargetParams(ParamListType* pListPtr) {
	if (!ptcl->currentTarget) return TRUE;
	for (int i = 0; i < pListPtr->length; i++) {
		const char* n = pListPtr->array[i].name;
		const char* v = pListPtr->array[i].value;
		LOG << "\ttargetparam " << SafeTcl(n) << " " << SafeTcl(v) << "\n";
		State* s = ptcl->currentTarget->stateWithName(n);
		if (s == 0) {
			Error::abortRun (*ptcl->currentTarget, "no target-state named ", n);
			return FALSE;
		}
		s->setInitValue(hashstring(v));
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// Inputs: name = name of block to get info of
//     info = adr of (char *) for return value
// Outputs: info = points to info string, free string when done
extern "C" boolean
KcInfo(const char* name, char** info)
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

static void displayStates(const Block *b,char** names,int n_names);

extern "C" int
KcPrintTerms(const char* name) {
	TermList terms;
        int i;
	if (!MkGetTerms(name, &terms)) return FALSE;
	if (terms.in_n) accum_string ("Inputs:\n");
	for (i = 0; i < terms.in_n; i++) {
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
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// Input: the name of a star in the current domain
// Pops up a window displaying the profile and returns TRUE if all goes well,
// otherwise returns FALSE.
extern "C" int
KcProfile (const char* name) {
	char* fieldnames[MAX_NUM_FIELDS];
	int n_fields = 0;
	const Block* b = findClass (name);
	int isTargetFlag = 0;
	if (b) {
		parseClass(name, n_fields, fieldnames);
	}
	else {
		isTargetFlag = 1;
		b = KnownTarget::find (name);
	}
	if (!b) {
		ErrAdd ("Unknown block: ");
		ErrAdd (name);
		return FALSE;
	}
	clr_accum_string ();
	// if dynamically linked, say so
	if (isTargetFlag && KnownTarget::isDynamic(b->name()) ||
	    !isTargetFlag && KnownBlock::isDynamic(b->name(), ptcl->curDomain))
		accum_string ("Dynamically linked ");
	if (isTargetFlag) {
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
		else
			accum_string ("Wormhole: ");
		LOG_DEL; delete [] msg;
		accum_string (name);
		accum_string ("\n");
	}
	accum_string (b->isItAtomic() ? "Star: " : "Galaxy: ");
	accum_string (name);
	accum_string (" (");
	accum_string (b->domain());
	accum_string (")\n");
	const char* desc = b->descriptor();
	accum_string (desc);

	// some descriptors don't end in '\n'
	if (desc[strlen(desc)-1] != '\n') accum_string ("\n");

	// get termlist
	if (!isTargetFlag) {
		if (!KcPrintTerms(name)) {
			ErrAdd(name);
			ErrAdd(" does not have valid input/output terminals");
			return FALSE;
		}
	}

	// now do states
	displayStates(b, fieldnames, n_fields);
	pr_accum_string ();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// display settable states.  Omit those in the "names" list.
static void displayStates(const Block *b,char** names,int n_names) {
	const char* clName = b->className(); // FIXME: needed by ACS domain.
	clName = clName; // ACS needs b->className() as a secondary initialization on KnownBlock list.
	if (b->numberStates()) accum_string ("Settable states:\n");
	CBlockStateIter nexts(*b);
	const State *s;
	int pos;
	while ((s = nexts++) != 0) {
		if ((s->attributes() & AB_SETTABLE) == 0 ||
		    isStringInList(s->name(),(const char **)names,n_names,pos))
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
	
///////////////////////////////////////////////////////////////////////
// Make multiple ports in a star multiporthole
extern "C" boolean
KcNumPorts (char* starname, char* portname, int numP) {
	LOG << "\tnumports " << starname << " " << portname << " "
		<< numP << "\n";
	return ptcl->currentGalaxy->numPorts(starname, portname, numP);
}

///////////////////////////////////////////////////////////////////////
// Return the number of domains that the system knows about
extern "C" int
numberOfDomains() {
        return Domain::number();
}

///////////////////////////////////////////////////////////////////////
// Return the name of the nth domain in the list of known domains
extern "C" const char*
nthDomainName(int n) {
        return Domain::nthName(n);
}

///////////////////////////////////////////////////////////////////////
// add a node with the given name
extern "C" int
KcNode (const char* name) {
	LOG << "\tnode " << name << "\n";
	return ptcl->currentGalaxy->addNode(name);
}

///////////////////////////////////////////////////////////////////////
// connect a porthole to a node
extern "C" int
KcNodeConnect (const char* inst, const char* term, const char* node) {
 	LOG << "\tnodeconnect " << inst << " " << term << " " <<
		node << "\n";
	return ptcl->currentGalaxy->nodeConnect(inst, term, node);
}

///////////////////////////////////////////////////////////////////////
// Return a list of targets supported by the given domain.
extern "C" int
KcDomainTargets(const char* domain, const char** names, int nMax) {
	return KnownTarget::getList(domain, names, nMax);
}

///////////////////////////////////////////////////////////////////////
// Set the target for the universe
extern "C" int
KcSetTarget(const char* targetName) {
	LOG << "target " << targetName << "\n";
	int temp = ptcl->universe->newTarget (hashstring(targetName));
	ptcl->currentTarget = ptcl->universe->myTarget();
	return temp;
}

///////////////////////////////////////////////////////////////////////
// Return the default target name for the given domain.
extern "C" const char*
KcDefTarget(const char* domain) {
	return KnownTarget::defaultName(domain);
}

///////////////////////////////////////////////////////////////////////
extern "C" void
TildeExpand(const char *input, char* buffer) {
    char *pathname = expandPathName(input);
    strcpy(buffer, pathname);
    delete [] pathname;
    return;
}

///////////////////////////////////////////////////////////////////////
// catch signals -- Vem passes the SIGUSR1 signal by default
extern "C" void
KcCatchSignals() {
	SimControl::catchInt(SIGUSR1);
}

///////////////////////////////////////////////////////////////////////
// functions for enabling and disabling the run-time Tk event loop
extern int runEventsOnTimer();		// from ptklib/ptkRunEvents.cc

static int EventLoopState = FALSE;

extern "C" int KcEventLoopActive() {
	return (EventLoopState);
}

extern "C" void KcSetEventLoop(int on) {
        if (on) {
            SimControl::setPollAction(runEventsOnTimer);
	    EventLoopState = TRUE;
        } else {
            SimControl::setPollAction(NULL);
	    EventLoopState = FALSE;
        }
}


///////////////////////////////////////////////////////////////////////
// interface to hashstring function.
extern "C" const char* HashString(const char* arg) {
    return hashstring(arg);
}
