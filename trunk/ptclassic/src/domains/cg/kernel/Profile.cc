static const char file_id[] = "Profile.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Profile.h"
#include "Error.h"

// constructor

Profile :: Profile(int n) {

	effP = 0;
	makespan = 0;
	maxPeriod = 0;
	assignedId = 0;
	
	// array creation
	create(n);
}

Profile :: ~Profile() {
	LOG_DEL; delete [] assignedId;
}

void Profile :: create(int n) {
	
	// array creation
	effP = n;
	startTime.create(n);
	finishTime.create(n);
	procId.create(n);
	for (int i = 0; i < effP; i++)	procId[i] = i;
}

void Profile :: createAssignArray(int n) {
	LOG_DEL; delete [] assignedId;
	LOG_NEW; assignedId = new IntArray[n];
	numAssignArray = n;
	for (int i = 0; i < n; i++) {
		assignedId[i].create(effP);
		assignedId[i].initialize();
	}
}

void Profile :: initialize() {
	startTime.initialize();
	finishTime.initialize();
	maxPeriod = 0;
	makespan = 0;
}

int Profile :: profileIx(int i, int t) {
	int j = i - 1;
	int limit = assignedId[j].size();
	for (int k = 0; k < limit; k++) {
		if (assignedId[j][k] == t) return k;
	}
	Error::abortRun("no profile index with a given processor id");
	return -1;
}

void Profile :: summary() {

	int latest = finishTime[0];
	int max = latest;

	// determine earlist, latest, and max quantity.
	for (int i = 1; i < effP; i++) {
		int e = startTime[i];
		int l = finishTime[i];
		if (l > latest)  latest  = l;
		if (l - e > max) max = l - e;
	}

	// set parameters
	makespan = latest;
	maxPeriod = max;
}

	//////////////////////
	// miscellanies
	/////////////////////

void Profile :: copyIt(Profile* dest, int offsetIndex,
				      int offsetTime) {

	for (int i = 0; i < effP; i++) {
		dest->setStartTime(i+offsetIndex, offsetTime+startTime[i]);
		dest->setFinishTime(i+offsetIndex, offsetTime+finishTime[i]);
	}
}
	
// currently insertion sort...

void Profile :: sortWithFinishTime(int start) {

	for (int i = 0; i < effP; i++)	procId[i] = i;

	// insertion sort
	for (i = 1 + start; i < effP; i++) {
		int j = i - 1;
		int x = finishTime[procId[i]];
		int temp = procId[i];
		while (j >= start) {
			if (x <= finishTime[procId[j]]) break;
			procId[j+1] = procId[j];
			j--;
		}
		j++;
		procId[j] = temp;
	}

	// copy values.
	LOG_NEW; int *value = new int[effP];
	for (i = 0; i < effP; i++) value[i] = finishTime[procId[i]];
	for (i = 0; i < effP; i++) finishTime[i] = value[i];
	for (i = 0; i < effP; i++) value[i] = startTime[procId[i]];
	for (i = 0; i < effP; i++) startTime[i] = value[i];
	LOG_DEL; delete value;
}

// currently insertion sort.
// It also updates procId[] arrays...

void Profile :: sortWithStartTime(int start) {

	for (int i = 0; i < effP; i++)	procId[i] = i;

	// insertion sort
	for (i = 1 + start; i < effP; i++) {
		int j = i - 1;
		int x = startTime[procId[i]];
		int temp = procId[i];
		while (j >= start) {
			if (x >= startTime[procId[j]]) break;
			procId[j+1] = procId[j];
			j--;
		}
		j++;
		procId[j] = temp;
	}

	// copy values.
	LOG_NEW; int *value = new int[effP];
	for (i = 0; i < effP; i++) value[i] = finishTime[procId[i]];
	for (i = 0; i < effP; i++) finishTime[i] = value[i];
	for (i = 0; i < effP; i++) value[i] = startTime[procId[i]];
	for (i = 0; i < effP; i++) startTime[i] = value[i];
	LOG_DEL; delete value;
}

// Calculate the idle time due to the unknown pattern of
// processor availability before executing this profile.
// Return the amount of the shift of the profile, which is
// equal to the idle time on the first available processor.
int Profile :: frontIdleLength(IntArray& avail) {

	// find the maximum difference between the pattern of processor
	// availability and the startTimes.
	int max = 0;
	int ix = 0;
	for (int i = 1; i < effP; i++) {
		int temp = avail[i] - startTime[i];
		if (temp > max) { max = temp; ix = i; }
	}
	return max;
}

// With a given pattern of processor availability, the profile creates
// extra free space, at the front, at the side and at the end.
// Sum up those idle times.
int Profile :: totalIdleTime(IntArray& avail, int numProcs) {

	// maximum shift
	int max = frontIdleLength(avail);

	// adjusted makespan
	int adj = makespan + max;

	// calculate the idle time.
	// First, total space is ...
	int idleSum = adj * numProcs;
	// Substract the execution time of the profile ...
	for (int i = 0; i < effP; i++) 
		idleSum = idleSum - (finishTime[i] - startTime[i]);
	// and the processor available times ...
	for (i = 0; i < numProcs; i++)
		idleSum = idleSum - avail[i];

	return idleSum;
}
