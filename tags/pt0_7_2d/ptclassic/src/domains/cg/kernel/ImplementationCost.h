#ifndef _ImplementationCost_h
#define _ImplementationCost_h 1
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
Version: @(#)ImplementationCost.h	1.4	07/02/96

This file defines an ImplementationCost class to track software and
hardware implementation costs.

******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

class ImplementationCost {
    public:
	// constructor
	ImplementationCost(int numProcessors, int numDataBanks);

	// destructor
	~ImplementationCost();

	// initialize data members
	void initialize();

	// return values of data members
	inline int executionTime() { return exectime; }
	inline int progMemoryCost(int i) {
		return progMemoryVector[i];
	}
	inline int dataMemoryCost(int i) {
		return dataMemoryVector[i];
	}

	// set values of data members
	inline void setExecutionTime(int etime) {
		exectime = etime;
	}
	inline void setProgMemoryCost(int i, int cost) {
		progMemoryVector[i] = cost;
	}
	inline void setDataMemoryCost(int i, int cost) {
		dataMemoryVector[i] = cost;
	}

	// number of clock cycles
	inline int time() { return exectime; }

	// number program and data memory locations
	inline int size() { return totalMemory(); }

	// total weighted cost of the implementation
	double totalCost(double Wtime, double* Wprog, double* Wdata);

    protected:
	int exectime;
	int progNodes;
	int* progMemoryVector;
	int dataBanks;
	int* dataMemoryVector;

	// number program and data memory locations
	int totalMemory();
};

#endif
