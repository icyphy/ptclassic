/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 7/30/90

 This class implements support for incremental linking.
**************************************************************************/

class Linker {
public:
	static void init(const char* myName);
	static int linkObj(const char* objName);
private:
	static const char* genHeader(const char*);
	static const char* ptolemyName;
	static int pid;
};
