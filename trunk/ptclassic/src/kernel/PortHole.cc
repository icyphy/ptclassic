#include "type.h"
#include "Connect.h"
#include "Block.h"
#include "StringList.h"


PortHole& PortHole :: setPort(char* s,
			      Block* parent,
			      dataType t = FLOAT) {
	name = s;
	alias = NULL;
	myGeodesic = NULL;
	type = t;
	blockIamIn = parent;
	farSidePort = NULL;
	return *this;
}

PortHole& SDFPortHole :: setPort (char* s,
			     Block* parent,
			     dataType t = FLOAT,
			     unsigned numTokens = 1) {
	PortHole::setPort(s,parent,t);
	numberTokens = numTokens;
	return *this;
}

PortHole :: operator char* () {
	StringList out;
	char* parentBlock;

	// Get the name of the parent block, if there is one
	if( blockIamIn != NULL)
		parentBlock = blockIamIn->readFullName();
	else
		parentBlock = "null (no block)";

	if(isItInput())
	   out = "	Input ";
	else if(isItOutput())
	   out = "	Output ";

	out += "PortHole: ";
	out += parentBlock;
	out += ".";
	out += readName();
	out += "\n";

	if(alias != NULL) {
	   PortHole& eventualAlias = realPort();
	   out += "	  Aliased to: ";
	   out += eventualAlias.blockIamIn->readFullName();
	   out += ".";
	   out += eventualAlias.readName();
	   out += "\n";
	} else {

	   if (farSidePort != NULL) {
	      out += "	  Connected to port: ";
	      out += farSidePort->blockIamIn->readFullName();
	      out += ".";
	      out += farSidePort->readName();
	      out += "\n";
	      }
	   else
	      out += "	  Not connected.\n";
	}
	return out;
}


MultiPortHole& MultiPortHole :: setPort(char* s,
			      Block* parent,
			      dataType t = FLOAT) {
	name = s;
	type = t;
	alias = NULL;
	blockIamIn = parent;
	return *this;
}

MultiPortHole :: operator char* () {
	StringList out;
	char* parentBlock;

	// Get the name of the parent block, if there is one
	if( blockIamIn != NULL)
		parentBlock = blockIamIn->readFullName();
	else
		parentBlock = "null (no block)";

	if(isItInput())
	   out = "	Input ";
	else if(isItOutput())
	   out = "	Output ";

	out += "MultiPortHole: ";
	out += parentBlock;
	out += ".";
	out += readName();
	out += "\n";

	// TO BE DONE:
	// Probably want to peruse ports in the MultiPort here,
	// using the char* cast of each to get its information.

	return out;
}

PortHole& MultiPortHole :: newPort() {
	PortHole* newport = new PortHole;
	ports.put(*newport);
	blockIamIn->addPort(newport->setPort(readName(), blockIamIn, type));
	return *newport;
}


MultiPortHole& MultiSDFPort :: setPort (char* s,
			     Block* parent,
			     dataType t = FLOAT,
			     unsigned numTokens = 1) {
	MultiPortHole::setPort(s,parent,t);
	numberTokens = numTokens;
	return *this;
}

PortHole& MultiInPortHole :: newPort () {
	InPortHole* newport = new InPortHole;
	ports.put(*newport);
	blockIamIn->
	    addPort(newport->
			setPort(readName(), blockIamIn, type, numberTokens));
	return *newport;
}


PortHole& MultiOutPortHole :: newPort () {
	OutPortHole* newport = new OutPortHole;
	ports.put(*newport);
	blockIamIn->
	    addPort(newport->
			setPort(readName(), blockIamIn, type, numberTokens));
	return *newport;
}
