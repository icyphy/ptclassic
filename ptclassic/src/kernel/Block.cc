#include "Block.h"
#include "StringList.h"

char* Block :: readFullName () {
	StringList out;
	if(blockIamIn != NULL) {
	   out = blockIamIn->readFullName();
	   out += ".";
	   out += name;
	   return out;
	} else {
	   return name;
	}
}


Block :: operator char* () {
	StringList out;
	out = "Block: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";

	out += "Ports in the block:\n";
	for(int i = numberPorts(); i>0; i--)
		out += nextPort().operator char* ();

	return out;
}
