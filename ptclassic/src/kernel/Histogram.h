#ifndef _Histogram_h
#define _Histogram_h 1
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

 Programmer: J. T. Buck 
 Date of creation: 6/12/91
 Revisions:

 Histogram is a class that can be used to make histograms.
 XHistogram uses Histogram and the XGraph class to display histograms.

**************************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "logNew.h"

inline int round(double x) { return x > 0 ? int(x+0.5) : -int(0.5-x); }

// a simple integer vector class
class intVec {
private:
	int* data;
	int xsize;
public:
	intVec() : xsize(0), data(0) {}
	~intVec() { INC_LOG_DEL; delete [] data; }
	int& elem(int i) { return data[i];}
	int& operator[](int i) { return elem(i);}
	int capacity() const { return xsize;}
	void resize(int newSize);
};

// default max # of bins.
const int HISTO_MAX_BINS = 1000;

class Histogram {
public:
	Histogram(double width = 1.0, int maxBins = HISTO_MAX_BINS)
		: centerRef(0), nSamples(0), minBin(0),
	maxBin(0), sum(0), sumsq(0), binWidth(width), maxNoBins(maxBins) {}

	// translate value to bin number
	int valueToBin(double x) const;

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
	int maxNoBins;		// maximum # of bins allowed.
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
			const char* saveFile,
			int maxBins = HISTO_MAX_BINS);

	void addPoint(float y);

	void terminate();

	// access to Histogram methods
	int numCounts() const { return hist->numCounts();}
	double mean() const { return hist->mean();}
	double variance() const { return hist->variance();}

private:
	XGraph *display;
	Histogram *hist;
	char *optstring;
};

#endif
