static const char file_id[] = "Profile.cc";

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

#include "Profile.h"

// constructor

Profile :: Profile(int n) {

	effP = 0;
	makespan = 0;
	maxPeriod = 0;
	syncPoint = 0;
	
	// array creation
	create(n);
}

void Profile :: create(int n) {
	
	// array creation
	effP = n;
	startTime.create(n);
	finishTime.create(n);
	procId.create(n);
}

void Profile :: initialize() {
	startTime.initialize();
	finishTime.initialize();
	maxPeriod = 0;
	makespan = 0;
	syncPoint = 0;
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

void Profile :: copyIt(Profile* dest, int offsetIndex = 0,
				      int offsetTime = 0) {

	for (int i = 0; i < effP; i++) {
		dest->setStartTime(i+offsetIndex, offsetTime+startTime[i]);
		dest->setFinishTime(i+offsetIndex, offsetTime+finishTime[i]);
	}
}
	
// currently insertion sort...

void Profile :: sortWithFinishTime() {

	int order[effP];
	for (int i = 0; i < effP; i++)	order[i] = i;

	// insertion sort
	for (i = 1; i < effP; i++) {
		int j = i - 1;
		int x = finishTime[order[i]];
		int temp = order[i];
		while (j >= 0) {
			if (x > finishTime[order[j]]) break;
			order[j+1] = order[j];
			j--;
		}
		j++;
		order[j] = temp;
	}

	// copy values.
	int value[effP];
	for (i = 0; i < effP; i++) value[i] = finishTime[order[i]];
	for (i = 0; i < effP; i++) finishTime[i] = value[i];
	for (i = 0; i < effP; i++) value[i] = startTime[order[i]];
	for (i = 0; i < effP; i++) startTime[i] = value[i];
}

// currently insertion sort.
// It also updates procId[] arrays...

void Profile :: sortWithStartTime() {

	for (int i = 0; i < effP; i++)	procId[i] = i;

	// insertion sort
	for (i = 1; i < effP; i++) {
		int j = i - 1;
		int x = startTime[procId[i]];
		int temp = procId[i];
		while (j >= 0) {
			if (x < startTime[procId[j]]) break;
			procId[j+1] = procId[j];
			j--;
		}
		j++;
		procId[j] = temp;
	}

	// copy values.
	int value[effP];
	for (i = 0; i < effP; i++) value[i] = finishTime[procId[i]];
	for (i = 0; i < effP; i++) finishTime[i] = value[i];
	for (i = 0; i < effP; i++) value[i] = startTime[procId[i]];
	for (i = 0; i < effP; i++) startTime[i] = value[i];
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
