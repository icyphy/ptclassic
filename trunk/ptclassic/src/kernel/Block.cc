#include "Block.h"
#include <stream.h>

// TO BE DONE:  
// Look for a better way to do this.  Right now, memory is
// re-used by each call to readFullName().

static char fullName[200];

char* Block :: readFullName () {
	if(blockIamIn != NULL) {
	   blockIamIn->readFullName();
	   strcat(fullName,".");
	   strcat(fullName,name);
	   return(fullName);
	} else {
	   strcpy(fullName, name);
	   return(fullName);
	}
}


void Block :: profile() {
	cout << "Block: " << readFullName() << "\n";
	cout << "Descriptor: " << readDescriptor() << "\n";

	cout << "Ports in the block:\n";
	for(int i = numberPorts(); i>0; i--)
		nextPort().profile();
}
