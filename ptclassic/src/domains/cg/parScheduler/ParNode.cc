static const char file_id[] = "ParNode.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision:

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ParNode.h"

                        ///////////////////////
                        ///  *Constructor*  ///
                        ///////////////////////

ParNode::ParNode(SDFStar* s, int invoc_no) : EGNode(s, invoc_no)
{
	StaticLevel = 0;	// Initialize to something invalid
	type = 0;		
	procId = 0;
	exTime = s->myExecTime();
	waitNum = 0;
}

// Alternate constructor for idle nodes and communication nodes
// If type = 1, it is an idle node.
// If type = -1, it is a send node, type = -2 indicates a receive node

ParNode::ParNode(int t) : EGNode(0,0) {
	
	type = t;
	StaticLevel = 0;	// Initialize to something invalid
	procId = 0;
	exTime = 0;
	waitNum = 0;
}

                           ///////////////
                           ///  print  ///
                           ///////////////
// Prints the star name, level, execution time.

StringList ParNode::print() {

	StringList out;
	out += printShort();
	out += " (level = ";
	out += StaticLevel;
	out += ", execution time = ";
	out += myMaster()->myExecTime();
	out += ")\n";
	return out;
}

