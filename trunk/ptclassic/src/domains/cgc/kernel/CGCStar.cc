static const char file_id[] = "CGCStar.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee, T. M. Parks

 This is the baseclass for stars that generate C language code

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCStar.h"
#include "CGCTarget.h"

// The following is defined in CGCDomain.cc -- this forces that module
// to be included if any CGC stars are linked in.
extern const char CGCdomainName[];
extern const char* whichType(DataType);

const char* CGCStar :: domain () const { return CGCdomainName;}

// isa

ISA_FUNC(CGCStar, CGStar);

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
	ref << target()->correctName(*st);
    }
    else if ((port = (CGCPortHole*)genPortWithName(portName)) != NULL)
    {
	// for embedded porthole
	if (port->bufType() == EMBEDDED) {
		ref << "(*";
		ref << port->getGeoName();
		ref << ")";
	} else {
		ref << port->getGeoName();
		if (port->bufSize() > 1) {
			ref << '[';
			if (port->staticBuf() == FALSE) {
	    			ref += target()->offsetName(port);
			} else {
				ref += port->bufPos();
			}
			ref << ']';
		}
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
		
	ref << port->getGeoName();
	if (port->bufSize() > 1) {
		if ((port->staticBuf() == TRUE) && (useState == TRUE)) {
			int v = (port->bufPos() + valOffset) % port->bufSize();
			ref << '[' << v << ']';
			return ref;
		}
		ref << "[(";
		if (port->staticBuf() == FALSE) {
	    		ref += target()->offsetName(port);
		} else {
			ref += port->bufPos();
		}
	    	ref << " - (" << offset << ")";
	    	if (port->linearBuf() == FALSE) {
	    		ref << " + " << port->bufSize() << ") % " 
				<< port->bufSize();
	    	} else {
			ref << ")";
	    	}
	    	ref << ']';
	}
    }
    return ref;
}

void CGCStar :: initBufPointer() {
	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		StringList out;
		out += initializeOffset(p);
		out += initializeBuffer(p);
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
}

/////////////////////////////////////////////////////////////
// run: (1) move input data from shared buffer if necessary
//	(2) prefix the code with a comment
//	(3) body of code
//	(4) update offset if necessary
/////////////////////////////////////////////////////////////

int CGCStar::run() {
	// data movement from shared (embedding) buffer to the private buffer 
	// if necessary.
	moveDataFromShared();

	StringList code = "\t{  /* star ";
	code += fullName();
	code += " (class ";
	code += className();
	code += ") */\n";
	gencode(code);
	int status = CGStar::run();
	
	if (isItFork()) {
		gencode("\t}\n");
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

// Before firing that star, we may need to move the input data from the
// shared buffer to the private buffer in case of embedding: when the
// input needs past samples or delays.
void CGCStar :: moveDataFromShared() {
	StringList code;
	int flag = 0;

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		// consider input only
		if (p->isItOutput()) continue;

		// type indication
		flag = 0;
		CGCPortHole* farP = (CGCPortHole*) p->far();
		if (farP->embedded() && (farP->bufType() == OWNER)) 
			flag = 1;
		else if (farP->embedding() && (farP->bufType() == COPIED))
			flag = -1;
		if (flag == 0) continue;
	
		CGCPortHole* ep = farP;
		if (flag > 0)
			ep = (CGCPortHole*) farP->embedded();

		if (p->numXfer() > 1) {
			code << "\t{ int i,j,k;\n\t for(i = 0; i < ";
			code << p->numXfer() << "; i++) {\n";
			code << "\t\t j = (";
			code << target()->offsetName(p);
			code << " - " << p->numXfer();
			code << " + 1 + i) % " << p->bufSize() << ";\n";
			code << "\t\t k = (j - ";
			code << target()->appendedName(*p, "copy_ix");
			code << ") % " << p->bufSize() << ";\n";
			code << "\t\t if (" << farP->numXfer(); 
			code << " <= k) continue;\n";
			code << "\t\t" << p->getGeoName();
			code << "[j] = ";
			if (flag > 0) {
				code << ep->getGeoName();
			} else {
				code << target()->appendedName(*ep, "copy");
			}
			code << "[k];\n\t}\n";
		} else {
			code << "\t{ int j,k;\n";
			code << "\t j = (";
			code << target()->offsetName(p);
			code << " - " << p->numXfer();
			code << " + 1 + i) % " << p->bufSize() << ";\n";
			code << "\t k = (j - ";
			code << target()->appendedName(*p, "copy_ix");
			code << ") % " << p->bufSize() << ";\n";
			code << "\t if (" << farP->numXfer(); 
			code << " > k) {\n";
			code << "\t\t" << p->getGeoName();
			code << "[j] = ";
			if (flag > 0) {
				code << ep->getGeoName();
			} else {
				code << target()->appendedName(*ep, "copy");
			}
			code << "[k];\n\t}\n\t}\n";
		}
	}
	if (code.length() > 0) gencode(code);
}
				
// After firing that star, we may need to move the input data between
// shared buffers (for example, Spread/Collect) since these movements
// are not visible from the user.
void CGCStar :: moveDataBetweenShared() {
	StringList code;

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		// consider output only
		if (p->isItInput()) continue;

		CGCPortHole* farP = (CGCPortHole*) p->far();
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
				if (code.length() > 0) gencode(code);
				return;
			}
			
			int there = ip->whereEmbedded() + from - start;
			if (start < from) start = from;
			if (stop > to) stop = to;

			code << "\t{ int i,j;\n\t j = " << there;
			code << ";\n\t for(i = " << start;
			code << "; i <= " <<  stop << "; i++) {\n";
			code << "\t\t" << p->getGeoName() << "[i] = "; 
			if (ep->bufType() != COPIED) {
				code << ep->getGeoName();
			} else {
				code << target()->appendedName(*ep, "copy");
			}
			code << "[j++];\n\t}\n";
		}
	}
	if (code.length() > 0) gencode(code);
}

void CGCStar :: updateOffsets() {

	CGCTarget* t = target();
	StringList code2;

	BlockPortIter next(*this);
	CGCPortHole* p;
	while ((p = (CGCPortHole*) next++) != 0) {
		if ((p->bufSize() > 1) && (p->staticBuf() == FALSE)) {
			if (p->numberTokens == p->bufSize()) continue;
			code2 += "\t";
			code2 += t->offsetName(p);
			code2 += " += ";
			code2 += p->numberTokens;
			code2 += ";\n\tif (";
			code2 += t->offsetName(p);
			code2 += " >= ";
			code2 += p->bufSize();
			code2 += ")\n\t\t";
			code2 += t->offsetName(p);
			code2 += " -= ";
			code2 += p->bufSize();
			code2 += ";\n";
		}
	}
	code2 += "\t}\n";
	gencode(code2);
}

	/////////////////////////////////////////////
	// Variable declarations and initializations
	/////////////////////////////////////////////

// Define variables only for each output port, except outputs of forks
// And set the buffer type.
StringList CGCStar :: declarePortHole(CGCPortHole* p) {
	StringList out;
	int copied = FALSE;
	if (p->isItOutput()) {
		emptyFlag = FALSE;
		int dimen = p->bufSize();

		// If it is embedding inputs;
		if (p->embedding()) {
			out += "    ";
			out += whichType(p->resolvedType());
			out += " ";
			out += target()->appendedName(*p, "copy");
			out << '[' << p->numXfer() << "];\n";
			if (dimen > p->numXfer()) { 
				copied = TRUE;
			} else {
				// If the input on this arc does not have
				// extra buffer requirements,
				p->setBufType(OWNER);
				return out;
			}
		}

		out += "    ";
		out += whichType(p->resolvedType());

		CGCPortHole* farP = (CGCPortHole*) p->far();

		// if embedded properly, do not allocate the buffer.
		if ((p->embedded() && (dimen == p->numXfer())) ||
			(farP->embedded() && (dimen == farP->numXfer()))) {
			out += "*";	// add pointer
			dimen = 0;
		} 

		out += " ";
		out += target()->correctName(*p);

		if(dimen > 1) {
			out += "[";
			out += dimen;
			out += "]";
		}
		out += ";\n";

		// buffer type determination.
		if (copied) {
			p->setBufType(COPIED);
		} else {
			if (dimen) p->setBufType(OWNER);
			else p->setBufType(EMBEDDED);
		}
	}
	return out;
}

// declare offset and copy_offset for moving embedded data. 
StringList CGCStar :: declareOffset(const CGCPortHole* p) {
	StringList out;
	if ((p->bufSize() > 1) && (p->staticBuf() == FALSE)) {
		emptyFlag = FALSE;
		out += "    ";
		out += "int ";
		out += target()->appendedName(*p,"ix");
		out += ";\n";
	}
	// copy_offset definition.
	if (p->isItInput()) {
		CGCPortHole* farP = (CGCPortHole*) p->far();
		if ((farP->embedded() && farP->bufType() == OWNER) ||
		    (farP->embedding() && farP->bufType() == COPIED)) {
			emptyFlag = FALSE;
			out += "    ";
			out += "int ";
			out += target()->appendedName(*p, "copy_ix");
			out += " = 0;\n";
		}
	}
	return out;
}

// declare state
StringList CGCStar :: declareState(const State* s) {
	StringList out;
	out += "    ";
	out += whichType(s->type());
	out += " ";
	out += target()->correctName(*s);
	if (s->size() > 1) {
		out += "[";
		out += s->size();
		out += "]";
	}
	out += ";\n";
	emptyFlag = FALSE;
	return out;
}

// Initialize the porthole
StringList CGCStar :: initializeBuffer(CGCPortHole* p) {
	StringList out;
	if (p->isItInput()) return out;

	CGCPortHole* farP = (CGCPortHole*) p->far();
	// if embedded properly, do not allocate the buffer.
	if (p->bufType() == EMBEDDED) {
		out += "    "; 
		out += p->getGeoName();
		out += " = ";

		CGCPortHole* ep;	// embedding buffer.
		if (p->embedded())
			ep = ((CGCPortHole*) p->embedded())->realFarPort();
		else if (farP->embedded())
			ep = (CGCPortHole*) farP->embedded();

		out += ep->getGeoName();
		out += " + ";
		out += p->whereEmbedded();
		out += ";\n";
		return out;
	} else if (p->bufType() == COPIED) {
		out += "    { int i;\n";
		out += "    for (i = 0; i < ";
		out += p->numXfer();
		out += "; i++)\n\t";
		out += target()->appendedName(*p, "copy");
		out += "[i] = 0;\n    }\n";
	} 

	// for copied and owner buffer
	if (p->bufSize() > 1) {
		// initialize output buffer
		out += "    { int i;\n";
		out += "    for (i = 0; i < ";
		out += p->bufSize();
		out += "; i++)\n\t";
		out += p->getGeoName();
		out += "[i] = 0;\n    }\n";
	}
	return out;
}

StringList CGCStar :: initializeOffset(const CGCPortHole* p) {
	StringList out;
	if ((p->bufSize() > 1) && (p->staticBuf() == FALSE)) {
		out = target()->offsetName(p);
		out += " = ";
		out += p->bufPos();
		out += ";\n";
	}
	return out;
}

#define SMALL_STRING 20

// Initialize the state
StringList CGCStar :: initializeState(const State* s) {
	CGCTarget* t = target();
	StringList out;
	if (s->size() > 1) {
		StringList temp = s->currentValue();
		const char* sval = temp;
		for (int i = 0; i < s->size(); i++) {
			out += "    ";
			out += t->correctName(*s);
			out += "[";
			out += i;
			out += "] = ";
			char wval[SMALL_STRING];
			char* wc = wval;
			char c = *sval++;
			while ((c != 0) && (c != '\n')) {
				*wc++ = c;
				c = *sval++;
			}
			*wc = 0;
			out += wval;
			out += ";\n";
			}
	} else {
		out += "    ";
		out += t->correctName(*s);
		out += " = ";
		out += s->currentValue();
		out += ";\n";
	}
	return out;
}


	// Add lines to be put at the beginning of the code file
void CGCStar :: addInclude(const char* decl) {
	target()->addInclude(decl);
}

	// Add declarations, to be put at the beginning of the main section
void CGCStar :: addDeclaration(const char* decl) {
	target()->addDeclaration(decl);
}

	// Add global declarations, to be put ahead of the main section
void CGCStar :: addGlobal(const char* decl) {
	target()->addGlobal(decl);
}

	// Add procedures, to be put ahead of the main section
void CGCStar :: addProcedure(const char* decl) {
	target()->addProcedure(decl);
}

	// Add main initializations, to be put at the beginning of the main 
	// section
void CGCStar :: addMainInit(const char* decl) {
	target()->addMainInit(decl);
}

