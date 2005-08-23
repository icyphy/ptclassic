#ifndef _Profile_h
#define _Profile_h
#ifdef __GNUG__
#pragma interface
#endif

/*****************************************************************
Version identification:
@(#)Profile.h	1.7	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#include "IntArray.h"

// This class stores the profile of a parallel schedule.

			   ///////////////////
			   // class Profile //
			   ///////////////////

class Profile {

public:
	// Constructor
	Profile() :  assignedId(0), makespan(0), maxPeriod(0) {}
	Profile(int n);
	~Profile();

	// Dynamic creation
	void create(int n);

	// initialize
	void initialize();

	// sort with finish time, and change the procId accordingly.
	// The argument indicates the starting index of sorting.
	// For example, if 1 is given, the first index is excluded in the
	// sorting.
	void sortWithFinishTime(int start = 0);

	// sort with start time, and update the procId accordingly.
	void sortWithStartTime(int start = 0);

	// Set start and finish time on processor k.
	void setStartTime(int k, int t) { startTime[k] = t; }
	void setFinishTime(int k, int t) { finishTime[k] = t; }

	// Get start and finish time on processor k.
	int getStartTime(int k) { return startTime[k]; }
	int getFinishTime(int k) { return finishTime[k]; }

	// procId for the inner schedule
	void setProcId(int k, int t) { procId[k] = t; }
	int getProcId(int k) { return procId[k]; }

	// create "assignedId" array
	void createAssignArray(int);
	IntArray* assignArray(int i) { return &assignedId[i-1]; }
	void assign(int i, int k, int t) { assignedId[i-1][k] = t; }
	int assignedTo(int i, int k) { return assignedId[i-1][k]; }
	int profileIx(int i, int t);
	int numInstance() { return numAssignArray; }

	// The number of effectively-used processors.
	void setEffP(int i) { effP = i; }
	int  getEffP() { return effP; }

	// Set the total cost of the profile.
	void setTotalCost(double i)	{ totalCost = i; }
	double getTotalCost()	{ return totalCost; }

	// Get makespan or maxPeriod 
	int getMakespan() { return makespan ;}
	int getPeriod() { return maxPeriod ;}

	// Set the minimum Displacement
	void setMinDisplacement(int i) { minDisplacement = i; }
	int getMinDisplacement() { return minDisplacement; }

	// Get the ratio makespan over minDisplacement
	int ToverK() 
		{ return minDisplacement? makespan/minDisplacement : 0; }

	// Set makespan and maxPeriod.
	void summary();

	// Copy the profile
	void copyIt(Profile* dest, int offsetIndex = 0, int offsetTime = 0);

	// Calculate the front idle length due to the unknown pattern of
	// processor availability before executing this profile.
	// Return the amount of the shift of the profile, which is
	// equal to the idle time on the first available processor.
	int frontIdleLength(IntArray& avail);

	// With a given pattern of processor availability, the profile
	// remains this processor-time space, at the front, at the side
	// and at the end.
	// Return the sum of those idle times.
	int totalIdleTime(IntArray& avail, int numProcs);

private:
	int effP;		// number of actually-used processors.

	// Start and finish time on each processor
	IntArray	startTime;
	IntArray	finishTime;

	// processor id of the local schedule.
	// Mapping between virtual processors represented by Profile and
	// actual processors inside.
	// procId[i] = j means that the (i+1)th virtual processor is
	// assigned to the (j+1)th actual processor.
	IntArray	procId;

	// processor id for outer processors.
	// assignedId[k][i] = j means that (i+1)th profile  of the (k+1)th
	// invocation is assigned to the (j+1)th outer processor.
	// The number of arrays is "numInstance" of the star containing this
	// profile.
	IntArray* 	assignedId;
	int		numAssignArray;

	// makespan: schedule length assuming the blocked scheduling.
	//         = latest finishTime.
	int makespan;

	// minimum displacement of parallel schedules.
	// If we want to overlap many instances of this profile,
	// this value gives the minimum displacement.
	int minDisplacement;

	// maxPeriod: maximum schedule length on a processor.
	//	    = max over i (finishTime[i] - startTime[i])
	int maxPeriod;
	
	// Total cost of the profile
	double totalCost;

	// processor id of synchronization point.
	// Synchronization point is hardwired to 0 for program simplicity.
	// no more need of "int syncPoint;"
}; 
	
#endif

