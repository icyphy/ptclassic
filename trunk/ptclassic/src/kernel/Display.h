/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 10/20/90

This header file contains basic container class data structures
used for X window displays, such as classes that invoke xgraph.

**************************************************************************/
#ifndef _Display_h
#define _Display_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "type.h"
#include "miscFuncs.h"
#include <stdio.h>

class Block;

	//////////////////////////////////////
	// class XGraph
	//////////////////////////////////////

class XGraph
{
public:
	// destructor
	~XGraph() { zapFiles();}

	// constructor
	XGraph();

	void initialize(Block* parent,
			int noGraphs,
			const char* options,
			const char* title,
			const char* saveFile = 0,
			int ignore = 0);

	// Modify the "ignore count"
	void setIgnore(int n) { nIgnore = n;}

	// Add a point to the data set to be plotted.
	// When there is only one data set, and the x axis simply increments
	// by one with each new data sample, then there is no need to supply
	// the x axis data.
	void addPoint(float y);

	// Add a point to the data set to be plotted.
	// Again, this can only be used when there is only one data set.
	void addPoint(float x, float y) {
		addPoint(1,x,y);
	}

	// add a point to one of several data sets, numbered 1 through N,
	// where N <= MAX_NO_GRAPHS
	void addPoint(int dataSet, float x, float y);

	// start a new trace for the nth dataset.
	void newTrace(int dataSet = 1);

	// complete and draw the graph
        void terminate();
protected:
	// close and remove all temporary files.
	void zapFiles();
private:
	void fcheck(double v,int set = 1);

	Block* blockIamIn;	// owner Block, for error messages
	const char* opt;	// options for xgraph program
	const char* sf;		// file for saving data
	const char* ttl;	// title for graph
	int index;		// index for x-axis = 0,1,2,...
	short ng;		// number of graph traces
	short ascii;		// flag for saving data in ASCII
	FILE **strm;		// file descriptors for graph traces
	char** tmpFileNames;	// file names for graph traces
	int* count;		// counts of # values on each stream
	int nIgnore;		// # of initial points to ignore
};

#endif
