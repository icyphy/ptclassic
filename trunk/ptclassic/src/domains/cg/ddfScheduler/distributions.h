/**************************************************************************
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

 Programmer:  Soonhoi Ha
 Date of creation: 7/2/91

 Definition of some statistical distributions.

**************************************************************************/
#ifndef _distributions_h
#define _distributions_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Distribution.h"
#include "dist_template.h"

class Target;

class DistGeometric : public DistBase {
public:
	int setParams(Galaxy*, MultiTarget*);
	StringList printParams();
	DistBase* copy();
	void paramInfo(); 
	double assumedValue();
	const char* myType() const;
	
	// read the parameters.
	int getMin() { return min; }
	double getP() { return p; }

	// constructor
	DistGeometric() { min = 0; p = 0.5; }
private:
	int min;
	double p;
};

class DistUniform : public DistBase {
public:
	int setParams(Galaxy*, MultiTarget*);
	StringList printParams();
	DistBase* copy();
	void paramInfo();
	double assumedValue();
	const char* myType() const;
	
	// read the parameters.
	int getMin() { return min; }
	int getMax() { return max; }

	// constructor
	DistUniform() { min = 1; max = 10; }
private:
	int min;
	int max;
};


class DistTable {
public:
	DistTable() { index = 0; value = 0; }
	int index;
	double value;
};

class DistGeneral : public DistBase {
public:
	int setParams(Galaxy*, MultiTarget*);   // set file names and 
					// the data into the table.
	StringList printParams();
	DistBase* copy();
	void paramInfo();
	double assumedValue();
	const char* myType() const;
	
	// read the parameters.
	DistTable& getTable(int index) { return table[index]; }
	int tableSize()	{ return size; }

	// constructor
	DistGeneral() { table = (DistTable*) 0; }
	~DistGeneral();

private:
	DistTable* table;
	int size;		// size of the table.
	const char* name;		// file name.
	int makeTable();	// make table from the files.
};
	
#endif
