#include "Star.h"
#include <stream.h>

OutputToUser :: OutputToUser()
	{ outputStream = &cout; }	// Start with default

void OutputToUser :: fileName(char *fileName)
	{
	if ( strcmp(fileName,"cout") == 0 )
		outputStream = &cout;
	else
		outputStream = new ostream(fileName,io_writeonly,a_create);
	}

void OutputToUser :: outputString(char *s)
        { ((ostream*)outputStream) -> operator<< (s) ; }

void OutputToUser :: operator << (int i)
	{ outputString(form("%d",i)); }

void OutputToUser :: operator << (char *s)
	{ outputString(form("%s",s)); }

void OutputToUser :: operator << (float f)
	{ outputString(form("%g",f)); }

void Star :: profile() {
	cout << "Star: " << readFullName() << "\n";
	cout << "Descriptor: " << readDescriptor() << "\n";

	cout << "Ports in the star:\n";
	for(int i = numberPorts(); i>0; i--)
		nextPort().profile();
}
