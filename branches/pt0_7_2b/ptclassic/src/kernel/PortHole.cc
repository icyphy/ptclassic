#include "type.h"
#include "Connect.h"
#include "Block.h"
#include <stream.h>


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

void PortHole :: profile() {
	char* parentBlock;

	// Get the name of the parent block, if there is one
	if( blockIamIn != NULL)
		parentBlock = blockIamIn->readFullName();
	else
		parentBlock = "null (no block)";

	if(isItInput())
	   cout << "	Input ";
	else if(isItOutput())
	   cout << "	Output ";

	cout << "PortHole: " << parentBlock << "."
	     << readName() << "\n";

	if(alias != NULL) {
	   PortHole& eventualAlias = realPort();
	   cout << "	  Aliased to: "
		<< eventualAlias.blockIamIn->readFullName()
		<< "."
	        << eventualAlias.readName()
		<< "\n";
	} else {

	   if (farSidePort != NULL)
	      cout << "	  Connected to port: "
		   << farSidePort->blockIamIn->readFullName()
		   << "."
		   << farSidePort->readName()
		   << "\n";
	   else
	      cout << "	  Not connected.\n";
	}
}
