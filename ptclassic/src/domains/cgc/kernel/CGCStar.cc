static const char file_id[] = "CGCStar.cc";
/******************************************************************
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

 Programmer: S. Ha, E. A. Lee, T. M. Parks

 This is the baseclass for stars that generate C language code

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCStar.h"
#include "CGCTarget.h"
#include "CGCGeodesic.h"

// The following is defined in CGCDomain.cc -- this forces that module
// to be included if any CGC stars are linked in.
extern const char CGCdomainName[];
extern const char* whichType(DataType);

const char* CGCStar :: domain () const { return CGCdomainName;}

// isa

ISA_FUNC(CGCStar, CGStar);

// return the actual buffer position to write to.
StringList CGCStar :: getActualRef(CGCPortHole* port, const char* offset) {
	StringList ref;
	ref << port->getGeoName();
	if (port->bufSize() > 1) {
		ref << "[(";
		if (port->staticBuf() == FALSE) {
			ref << targ()->offsetName(port);
		} else {
			ref << port->bufPos();
		}
		if (offset != 0) {
	    		ref << " - (" << offset << ")";
	    		if (port->linearBuf() == FALSE) {
	    			ref << " + " << port->bufSize() << ") % "
					<< port->bufSize();
	    		} else {
				ref << ")";
	    		}
	    		ref << ']';
		} else {
			ref << ")]";
		}
	}
    return ref;
}

// Reference to State or PortHole.
StringList CGCStar::expandRef(const char* name)
{
    StringList ref;
    CGCPortHole* port;
    State* st;
    StringList portName = expandPortName(name);

    if ((st = stateWithName(name)) != NULL)
    {
	registerState(name);
	ref << targ()->correctName(*st);
    }
    else if ((port = (CGCPortHole*)genPortWithName(portName)) != NULL)
    {
	// for embedded porthole
	if (port->bufType() == EMBEDDED) {
		ref << "(*" << port->getGeoName() << ")";
	} else {
		ref << getActualRef(port, 0);
	}
    }
    else
    {
	codeblockError(name, " is not defined as a state or port");
	ref.initialize();
    }
    return ref;
}

// Reference to State or PortHole with offset.
StringList CGCStar::expandRef(const char* name, const char* offset)
{
    StringList ref;
    State* state;
    CGCPortHole* port;
    StringList stateVal;
    StringList portName = expandPortName(name);
    int valOffset;
    int useState = FALSE;

    if ((state = stateWithName(offset)) != NULL)
    {
	// Note: currently only the value of a State can be used as an offset
	if (state->isA("IntState"))
	{
	    stateVal = expandVal(offset);
	    offset = stateVal;
	    valOffset = int(*(IntState*)state);
	    useState = TRUE;
	}
	else
	{
	    codeblockError(offset, " is not an IntState");
	    ref.initialize();
	    return ref;
	}
    }

    if ((state = stateWithName(name)) != NULL)
    {
	ref = expandRef(name);
	if (ref.length() == 0) return ref;	// error in getRef()
	if (state->size() > 1) ref << '[' << offset << ']';
    }
    else if (port = (CGCPortHole*)genPortWithName(portName))
    {
	// for embedded porthole.
	if ((port->bufSize() == 1) && (port->bufType() == EMBEDDED)) {
		ref << "(*";
		ref << port->getGeoName();
		ref << ")";
		return ref;
	}

	if (port->bufSize() > 1) {
		if ((port->staticBuf() == TRUE)&&(useState == TRUE)) {
			ref << port->getGeoName();
			int v = (port->bufPos() - valOffset +
				 	port->bufSize()) % port->bufSize();
			ref << '[' << v << ']';
		} else {
			ref << getActualRef(port, offset);
		}
	} else {
		ref << port->getGeoName();
	}
    }
    return ref;
}

void CGCStar :: initBufPointer() {
	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		StringList out = initializeOffset(p);
		out << initializeBuffer(p);
		if (out.length() > 0) addMainInit(out);
	}
}

void CGCStar::registerState(const char* name) {
	State* state;
	if(state = stateWithName(name)) {
		// If the state is not already on the list of referenced
		// states, add it.
		StateListIter nextState(referencedStates);
		const State* sp;
		while ((sp = nextState++) != 0)
			if(strcmp(name,sp->name()) == 0) return;
		referencedStates.put(*state);
	}
}

void CGCStar::initialize() {
	CGStar::initialize();
	referencedStates.initialize();
	spliceClust.initialize();
	if (!spliceClust.member(this)) spliceClust.append(this);
}

/////////////////////////////////////////////////////////////
// runIt: 
//	(1) prefix the code with a comment
//	(2) body of code
//	(3) update offset if necessary
//	(4) move output data for Spread/Collect pair
/////////////////////////////////////////////////////////////

int CGCStar :: run() {
	int status;
	if (spliceClust.size() > 1) {
		ListIter nextStar(spliceClust);
		CGCStar* s;
		while ((s = (CGCStar*) nextStar++) != 0) {
			status = s->runIt();
			if (!status) return status;
		}
	}
	else status = runIt();
	return status;
}

int CGCStar::runIt() {
	StringList code = "\t{  /* star ";
	code << fullName() << " (class " << className() << ") */\n";
	addCode(code);
	int status = CGStar::run();
	if (!status) return status;

	if (isItFork()) {
		addCode("\t}\n");
		return status;
	}

	// update the offset member
	updateOffsets();

	// data movement between shared (embedding) buffers if necessary
	// after we write to the first shared buffer (good for Spread/
	// Collect combination.)
	moveDataBetweenShared();

	return status;
}

// After firing that star, we may need to move the input data between
// shared buffers (for example, Spread/Collect) since these movements
// are not visible from the user.
// In case, embedded portholes are connected together.
void CGCStar :: moveDataBetweenShared() {
	StringList code;

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		// consider output only
		if (p->isItInput()) continue;

		CGCPortHole* farP = (CGCPortHole*) p->far();
		if (farP == 0) {
			if (!p->switched()) continue;
			farP = (CGCPortHole*) p->cgGeo().destPort();
		}

		if (farP->embedding() == 0) continue;

		BlockPortIter piter(*farP->parent());
		CGCPortHole* op;
		// march from output#1 ....
		while ((op = (CGCPortHole*) piter++) != 0) {
			CGCPortHole* ip = (CGCPortHole*) op->far();
			CGCPortHole* ep = (CGCPortHole*) ip->embedded();
			if (ep == 0) continue;

			// now we check whether we need to move data
			// we know that writing to the shared
			// buffer is always static and linear.
			int to = p->bufPos() - p->numXfer();
			if (to < 0) to += p->bufSize();
			int from = to - p->numXfer() + 1;

			int start = op->whereEmbedded();
			int stop = start + op->numXfer() - 1;

			if (stop < from) continue;
			if (start > to) {
				if (code.length() > 0) addCode(code);
				return;
			}

			int there;
			if (from <= start) there = ip->whereEmbedded();
			else  there = ip->whereEmbedded() + from - start;

			if (start < from) start = from;
			if (stop > to) stop = to;

			const char* farName = ep->getGeoName();

			code << "\t/* moveDataBetweenShared */\n";
			if (op->numXfer() > 1) {
				code << "\t{ int i,j;\n\t  j = " << there;
				code << ";\n\t  for(i = " << start;
				code << "; i <= " <<  stop << "; i++) {\n";
				code << "\t\t" << farName << "[j++] = ";
				code << p->getGeoName() << "[i];\n\t}\n";
			} else {
				code << "\t" << farName << '[' << there << ']';
				code << " = " << p->getGeoName();
				code << '[' << start << "];\n";
			}
		}
	}
	if (code.length() > 0) addCode(code);
}

void CGCStar :: updateOffsets() {

	CGCTarget* t = targ();
	StringList code2;

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		if ((p->bufSize() > 1) && (p->staticBuf() == FALSE)) {
			if (p->numberTokens == p->bufSize()) continue;
			StringList pname = t->offsetName(p);
			code2 << "\t" << pname << " += ";
			code2 << p->numberTokens << ";\n\tif (";
			code2 << pname << " >= ";
			code2 << p->bufSize() << ")\n\t\t";
			code2 << pname << " -= ";
			code2 << p->bufSize() << ";\n";
		}
	}
	code2 << "\t}\n";
	addCode(code2);
}

	/////////////////////////////////////////////
	// porthole declarations and initializations
	/////////////////////////////////////////////

// Define variables only for each output port, except outputs of forks.
StringList CGCStar :: declarePortHole(CGCPortHole* p) {
	StringList out;
	if (p->isItOutput()) {
		emptyFlag = FALSE;
		int dimen = p->bufSize();

		out << "    " << whichType(p->resolvedType());

		// if embedded properly, do not allocate the buffer.
		if (p->bufType() == EMBEDDED) {
			out << "*";	// add pointer
			dimen = 0;
		}

		out << " " << p->getGeoName();

		if(dimen > 1) {
			out << "[" << dimen << "]";
		}
		out << ";\n";
	}
	return out;
}

// static function to initialize porthole data structure.
StringList typelessPortInit(const char* typeName, const char* pName,
			int dim = 1, double realVal = 0, double imagVal = 0) {
	StringList out;
	if (strcmp(typeName, COMPLEX) == 0) {
		out << pName;
		if (dim > 0) out << "[i]";
		out << ".real = " << realVal << ";\n\t";
		out << pName;
		if (dim > 0) out << "[i]";
		out << ".imag = " << imagVal << ";\n";
	} else {
		out << pName;
		if (dim > 0) out << "[i]";
		out << " = " << realVal << ";\n";
	}
	return out;
}

// Initialize the porthole
StringList CGCStar :: initializeBuffer(CGCPortHole* p) {
	StringList out;
	if (p->isItInput()) return out;

	out << "    /* initializeBuffer for " << p->fullName() << " */\n";
	CGCPortHole* farP = (CGCPortHole*) p->far();

	// if embedded properly, do not allocate the buffer.
	if (p->bufType() == EMBEDDED) {
		out << "    " << p->getGeoName() << " = ";

		CGCPortHole* ep;	// embedding buffer.
		int loc = 0;
		// there are some hacks to handle buffer size = 1 scalar.
		if (p->embedded()) {
			ep = ((CGCPortHole*) p->embedded())->realFarPort();
			loc = p->whereEmbedded();
			if (ep->maxBufReq() == 1) out << "&";
			out << ep->getGeoName();
		} else if (farP->embedded()) {
			ep = (CGCPortHole*) farP->embedded();
			loc = farP->whereEmbedded();
			if (ep->maxBufReq() == 1) out << "&";
			out << ep->getGeoName();
		}
		if (ep->maxBufReq() > 1)
			out << " + " << loc;
		out << ";\n";
		return out;

	} 

	// for owner buffer
	if (p->bufSize() > 1) {
		// initialize output buffer
		StringList init =
			typelessPortInit(p->resolvedType(),p->getGeoName());

		out << "    { int i;\n    for (i = 0; i < ";
		out << p->bufSize() << "; i++) {\n\t" << init << "    }}\n";
	}

	return out;
}

	////////////////////////////////////
	// declare and initialize offset
	////////////////////////////////////

// declare offset and copy_offset for moving embedded data.
StringList CGCStar :: declareOffset(const CGCPortHole* p) {
	StringList out;
	if ((p->bufSize() > 1) && (p->staticBuf() == FALSE)) {
		emptyFlag = FALSE;
		out << "    " << "int ";
		out << targ()->offsetName(p) << ";\n";
	}
	return out;
}

StringList CGCStar :: initializeOffset(const CGCPortHole* p) {
	StringList out;
	if ((p->bufSize() > 1) && (p->staticBuf() == FALSE)) {
		out << "    /* initializeOffset for " << p->fullName()
		    << " */\n";
		out << "    " << targ()->offsetName(p);
		out << " = " << p->bufPos() << ";\n";
	}
	return out;
}

	////////////////////////////////////
	// declare and initialize states
	////////////////////////////////////

#define SMALL_STRING 20

// static function to initialize state data structure.
StringList typelessStateInit(const char* typeName, const char* val) {
	StringList out;
	if (strcmp(typeName,COMPLEX) && strcmp(typeName,"COMPLEXARRAY"))
		out << val;
	else {
		out << '{';
		char wval[SMALL_STRING];
		char* wc = wval;
		char c = *val++;
		while (c != '(') c = *val++;
		c = *val++;
		while (c != ',') {
			*wc++ = c; c = *val++;
		}
		*wc = 0;
		out << wval << ", ";		// write real part
		wc = wval; c = *val++;
		while (c != ')') {
			*wc++ = c; c = *val++;
		}
		*wc = 0;
		out << wval << "}";		// write imaginary part.
	}
	return out;
}

// declare and initialize state
// Note: no automatic aggregate initialization is possibile inside
// function.

StringList CGCStar :: declareState(const State* s) {
	int flag = targ()->makingFunc();

	StringList sname = targ()->correctName(*s);
	StringList out;
	out << "    " << whichType(s->type()) <<  " " << sname;

	if (s->size() > 1) {
		StringList initS;

		out << "[" << s->size();

		if (flag) out << "];\n";
		else out <<  "] = {\n";

		StringList temp = s->currentValue();
		const char* sval = temp;
		int leng = 0;
		int maxIndex = s->size() - 1;
		for (int i = 0; i <= maxIndex; i++) {
			char wval[SMALL_STRING];
			char* wc = wval;
			char c = *sval++;
			while ((c != 0) && (c != '\n')) {
				*wc++ = c;
				c = *sval++;
			}
			*wc = 0;
			StringList sinit = typelessStateInit(s->type(),wval);

			if (flag) {
				initS << "\t" << sname;
				initS << "[" << i << "] = " << sinit << ";\n";
			} else {
				leng += strlen(wval) + 3;
				if (leng > 80) {
					out << "\n";
					leng = strlen(wval) + 3;
				}
				out << sinit;
				if (i != maxIndex)  out << ",  ";
			}
		}
		if (flag) {
			addMainInit(initS);
		} else {
			out << " };\n";
		}
	} else {
		StringList temp = s->currentValue();
		StringList it = typelessStateInit(s->type(),temp);
		out << " = " << it << ";\n";
	}
	emptyFlag = FALSE;
	return out;
}

	////////////////////////////////////
	// code stream management
	////////////////////////////////////

// Add lines to be put at the beginning of the code file
int CGCStar :: addInclude(const char* decl) {
	StringList temp = "#include ";
	temp << decl << "\n";
	return addCode(temp, "include", decl);
}

