#ifndef _Histogram_h
#define _Histogram_h 1
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck 
 Date of creation: 6/12/91
 Revisions:

 Histogram is a class that can be used to make histograms.
 XHistogram uses Histogram and the XGraph class to display histograms.

**************************************************************************/

#include <builtin.h>
#include "int.Vec.h"

inline int round(double x) { return x > 0 ? int(x+0.5) : -int(0.5-x); }

class Histogram {
public:
	Histogram(double width = 1.0) : centerRef(0), nSamples(0), minBin(0),
	maxBin(0), sum(0), sumsq(0), binWidth(width) {}

	// add a new sample to the histogram.
	void add(double x);

	// get counts, centers by bin number, where 0 indicates the
	// smallest bin.  return TRUE if this is a valid bin.  Thus
	// the histogram data can be received by stepping from 0 to
	// first failure

	int getData(int binno, int& count, double& binCtr);

	int numCounts() const { return nSamples;}

	double mean() const {
		return nSamples == 0 ? 0 : sum / nSamples;
	}

	double variance() const {
		return nSamples < 2 ? 0 : sumsq / (nSamples - 1);
	}

private:
	void addCount(int bin);	// add a count to the appropriate bin #

	intVec zpos;		// vector for first bin and those greater
	intVec neg;		// vector for lesser bins
	int centerRef;		// normalized position of first bin
	int nSamples;		// number of counts
	int minBin;		// minimum bin #
	int maxBin;		// maximum bin #
	double sum;		// sum of all samples
	double sumsq;		// sum of all samples, squared
	double binWidth;	// width of bins
};

// Class for plotting histograms with the code from Display.h
class XGraph;
class Block;

class XHistogram {
public:
	XHistogram();
	~XHistogram();

	void initialize(Block* parent,
			double binWidth,
			const char* options,
			const char* title,
			const char* saveFile);

	void addPoint(float y);

	void terminate();
private:
	XGraph *display;
	Histogram *hist;
	const char *optstring;
};

#endif
