static const char file_id[] = "CGDDFScheduler.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 

CGDDFScheduler: macro scheduler to support CGDDF constructs

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CGDDFScheduler.h"
#include "CGDDFCluster.h"

//
// create clusterGal
//
CGMacroClusterGal* CGDDFScheduler::createClusterGal(Galaxy* g, ostream* l) {
	LOG_NEW; return new CGDDFClusterGal(g,l);
}

