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
