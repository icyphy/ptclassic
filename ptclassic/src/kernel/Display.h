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

#include "type.h"
#include "miscFuncs.h"

#define MAX_NO_GRAPHS 64

	//////////////////////////////////////
	// class XGraph
	//////////////////////////////////////

class XGraph
{
public:
	// destructor
	~XGraph();
	// constructor
	XGraph();

	void initialize(int noGraphs,
			const char* options,
			const char* title,
			const char* saveFile);

	// Add a point to the data set to be plotted.
	// When there is only one data set, and the x axis simply increments
	// by one with each new data sample, then there is no need to supply
	// the x axis data.
	void addPoint(float y);

	// Add a point to the data set to be plotted.
	// Again, this can only be used when there is only one data set.
	void addPoint(float x, float y);

	// add a point to one of several data sets, numbered 1 through N,
	// where N <= MAX_NO_GRAPHS
	void addPoint(int dataSet, float x, float y);

        void terminate();

private:
	const char* opt;
	const char* sf;
	const char* ttl;
	int ng, index;
	FILE *strm[MAX_NO_GRAPHS];
	const char* tmpFileNames[MAX_NO_GRAPHS];
};

#endif
