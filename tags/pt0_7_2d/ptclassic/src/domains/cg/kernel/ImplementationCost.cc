/******************************************************************
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

Author:  Brian L. Evans
Created: July 2, 1996
Version: @(#)ImplementationCost.cc	1.3	07/08/96

This file defines an ImplementationCost class to track software
and hardware implementation costs.

******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ImplementationCost.h"

static const char file_id[] = "ImplementationCost.cc";

// constructor
ImplementationCost::ImplementationCost(int numProcessors, int numDataBanks) :
		exectime(0), progNodes(numProcessors), progMemoryVector(0),
		dataBanks(numDataBanks), dataMemoryVector(0) {
	if (progNodes) progMemoryVector = new int[progNodes];
	if (dataBanks) dataMemoryVector = new int[dataBanks];
	initialize();
}

// destructor
ImplementationCost::~ImplementationCost() {
	delete [] progMemoryVector;
	delete [] dataMemoryVector;
}

// initialize data members
void ImplementationCost::initialize() {
	int i;
	for (i = 0; i < progNodes; i++) progMemoryVector[i] = 0;
	for (i = 0; i < dataBanks; i++) dataMemoryVector[i] = 0;
}

int ImplementationCost::totalMemory() {
	int i, totalmem = 0;
	for (i = 0; i < progNodes; i++) totalmem += progMemoryVector[i];
	for (i = 0; i < dataBanks; i++) totalmem += dataMemoryVector[i];
	return totalmem;
}

double ImplementationCost::totalCost(
		double Wtime, double* Wprog, double* Wdata) {
	int i;
	double totalcost = Wtime * exectime; 
	for (i = 0; i < progNodes; i++)
	    totalcost += Wprog[i] * progMemoryVector[i];
	for (i = 0; i < dataBanks; i++)
	    totalcost += Wdata[i] * dataMemoryVector[i];
	return totalcost;
}
