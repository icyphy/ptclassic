static const char file_id[] = "ParProcessors.cc";

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

#include "ParProcessors.h"
#include "StringList.h"

ParProcessors :: ParProcessors(int pNum, BaseMultiTarget* t) : mtarget(t) {
         numProcs = pNum;
         pIndex.create(pNum);
         pId.create(pNum); 
}

void ParProcessors :: initialize()
{
	// reset the pattern of processor availability.
	// initialize parallel schedules.
	for (int i = 0; i < numProcs; i++) {
		getProc(i)->initialize();
		pIndex[i] = i;
		pId[i] = i;
	}
}

UniProcessor* ParProcessors :: getProc(int num) { return 0; }

/*****************************************************************
		DISPLAY 
 ****************************************************************/

StringList ParProcessors :: display (NamedObj* galaxy) {

	int makespan = getMakespan();
	int sum = 0;	// Sum of idle time.

	StringList out;

	// title and number of processors.
	out += "** Parallel schedule of \"";
	out += galaxy->readFullName();
	out += "\" on ";
	out += numProcs;
	out += " processors.";
	out += "\n\n";

	// Per each processor.
	for (int i = 0; i < numProcs; i++) {
		out += "[PROCESSOR ";
		out += i;
		out += "]\n";
		out += getProc(i)->display(makespan);
		sum += getProc(i)->getSumIdle();
		out += "\n\n";
	}

	double util = 100*(1 - sum / (double(numProcs)*double(makespan)));

	// Print out the schedule statistics.
	out += "************* STATISTICS ******************";
	out += "\nMakespan is ................. ";
	out += makespan;
	out += "\nTotal Idle Time is .......... ";
	out += sum;
	out += "\nProcessor Utilization is .... ";
	out += util;
	out += " %\n\n";

	return out;
}

void ParProcessors :: sortWithFinishTime() {

	// insertion sort
        for (int i = 1; i < numProcs; i++) {
                int j = i - 1;
		UniProcessor* up = getProc(i);
                int x = up->getAvailTime();
                int temp = pId[i];
                while (j >= 0) {
			if (x > getProc(j)->getAvailTime()) break;
                        pId[j+1] = pId[j];
                        j--;
                }
                j++;
                pId[j] = temp;
        }
}

// calculate the makespan...
int ParProcessors :: getMakespan() {
	int max = 0;
	for (int i = 0; i < numProcs; i++) {
		int temp = getProc(i)->getAvailTime();
		if (temp > max) max = temp;
	}
	return max;
}

// sort the processor ids with available times.
void ParProcessors :: sortWithAvailTime(int guard) {

	// initialize indices
	for (int i = 0; i < numProcs; i++) {
		pIndex[i] = i;
	}

	// insertion sort
	// All idle processors are appended after the processors
	// that are available at "guard" time and before the processors
	// busy.

        for (i = 1; i < numProcs; i++) {
                int j = i - 1;
		UniProcessor* up = getProc(i);
                int x = up->getAvailTime();
                int temp = pIndex[i];
		if (x == 0) {	// If the processor is idle.
                	while (j >= 0) {
			   	if (guard < getProc(j)->getAvailTime()) 
					break;
				pIndex[j+1] = pIndex[j];
                        	j--;
			}
                } else if (x > guard) {
			while (j >= 0) {
				if(x < getProc(j)->getAvailTime()) break;
                        	pIndex[j+1] = pIndex[j];
                        	j--;
                	}
		} else {
			int temp = getProc(j)->getAvailTime();
			while (j >= 0 && (x < temp || temp == 0)) {
                        	pIndex[j+1] = pIndex[j];
                        	j--;
				if (j >=0)
					temp = getProc(j)->getAvailTime();
                	}
		}
                j++;
                pIndex[j] = temp;
        }
}
