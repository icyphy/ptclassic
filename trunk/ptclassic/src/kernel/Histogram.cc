static const char file_id[] = "Histogram.cc";
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
#include "Histogram.h"
#include "Display.h"
#include <builtin.h>
#include "miscFuncs.h"

void intVec::resize(int newSize) {
	if (newSize == xsize) return;
	int* oldData = data;
	LOG_NEW; data = new int[newSize];
	int ncopy = min(newSize, xsize);
	for (int i = 0; i < ncopy; i++)
		data[i] = oldData[i];
	for (i = ncopy; i < newSize; i++)
		data[i] = 0;
	LOG_DEL; delete oldData;
	xsize = newSize;
}

const int growStep = 16;

// increment a bin in the intVec, growing the vector as needed.
inline static void incBin(intVec& v, int bin) {
	if (v.capacity() <= bin) v.resize(bin+growStep);
	v.elem(bin) += 1;
}

// add one to a bucket in the histogram.
void Histogram::addCount(int bin) {
	if (bin >= 0) incBin(zpos,bin);
	else incBin(neg,-bin-1);
	nSamples++;
	if (bin < minBin) minBin = bin;
	if (bin > maxBin) maxBin = bin;
}

void Histogram::add(double x) {
	int absbin = round (x / binWidth);
	if (nSamples == 0) {
		centerRef = absbin;
		incBin(zpos,0);
		nSamples = 1;
	}
	else addCount(absbin - centerRef);
	sum += x;
	sumsq += x*x;
}

int Histogram::getData(int binno, int& count, double& binCtr) {
	binno += minBin;
	binCtr = (binno + centerRef) * binWidth;
	count = 0;
	if (binno < minBin || binno > maxBin) return 0;
	count = binno >= 0 ? zpos[binno] : neg[-1-binno];
	return 1;
}

XHistogram :: XHistogram () {
	LOG_NEW; display = new XGraph;
	hist = 0;
	optstring = 0;
}

void XHistogram :: initialize(Block* parent, double binW, const char* options,
			      const char* title, const char* saveFile) {
	char buf[100];
	sprintf (buf, "-bar -nl -brw %g %s", binW/2, options);
	optstring = savestring(buf);
	display->initialize(parent, 1, optstring, title, saveFile);
	LOG_DEL; delete hist;
	LOG_NEW; hist = new Histogram(binW);
}

void XHistogram :: addPoint(float y) {
	hist->add(y);
}

void XHistogram :: terminate() {
	int count;
	double binCtr;
	int binno = 0;
	while (hist->getData (binno, count, binCtr)) {
		display->addPoint (binCtr, float(count));
		binno++;
	}
	display->terminate();
	LOG_DEL; delete hist;
	hist = 0;
	LOG_DEL; delete optstring;
	optstring = 0;
}

XHistogram :: ~XHistogram() {
	LOG_DEL; delete display;
	LOG_DEL; delete hist;
	LOG_DEL; delete optstring;
}
