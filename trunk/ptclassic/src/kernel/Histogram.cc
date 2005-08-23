static const char file_id[] = "Histogram.cc";
/**************************************************************************
Version identification:
@(#)Histogram.cc	2.16	12/08/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer: J. T. Buck 
 Date of creation: 6/12/91
 Revisions:

 Histogram is a class that can be used to make histograms.
 XHistogram uses Histogram and the XGraph class to display histograms.

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Histogram.h"
#include "Display.h"
#include <minmax.h>
#include "miscFuncs.h"
#include "StringList.h"

// resize causes the intVec to grow or shrink.  The data are reallocated
// and copied.

void intVec::resize(int newSize) {
        int i;
	if (newSize == xsize) return;
	int* oldData = data;
	LOG_NEW; data = new int[newSize];
	int ncopy = min(newSize, xsize);
	for (i = 0; i < ncopy; i++)
		data[i] = oldData[i];
	for (i = ncopy; i < newSize; i++)
		data[i] = 0;
	LOG_DEL; delete [] oldData;
	xsize = newSize;
}

const int growStep = 16;

// increment a bin in the intVec, growing the vector as needed.
// we grow a bit extra because we expect larger samples later on.

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

// convert value to equivalent histogram bin.
// limit the number of bins generated so there are never more than
// maxNoBins bins.
int Histogram::valueToBin(double x) const {
	if (nSamples == 0) return 0;
	else {
		int v = round(x / binWidth) - centerRef;
		int loLimit = maxBin - maxNoBins + 1;
		if (v < loLimit) return loLimit;
		int hiLimit = maxNoBins - minBin - 1;
		if (v >= hiLimit) return hiLimit;
		return v;
	}
}

void Histogram::add(double x) {
	if (nSamples == 0) centerRef = round(x / binWidth);
	addCount(valueToBin(x));
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
	showPercentages = 0;
}

void XHistogram :: initialize(Block* parent, double binW, const char* options,
			      const char* title, const char* saveFile,
			      int maxBins) {
	char numstr[64];
	sprintf(numstr, "%g", binW/2);
	StringList buf = "-bar -nl";
	buf << " -brw " << numstr << " " << options;
	LOG_DEL; delete [] optstring;
	optstring = savestring(buf);
	display->initialize(parent, 1, optstring, title, saveFile);
	LOG_DEL; delete hist;
	LOG_NEW; hist = new Histogram(binW,maxBins);
	showPercentages = 0;
}

void XHistogram :: terminate() {
	if (numCounts() > 0) {
		double scaleFactor;
		if (showPercentages)
			scaleFactor = 100.0 / numCounts();
		else
			scaleFactor = 1.0;
		int count;
		double binCtr;
		int binno = 0;
		while (hist->getData (binno, count, binCtr)) {
			display->addPoint (binCtr, count * scaleFactor);
			binno++;
		}
		display->terminate();
	}
	LOG_DEL; delete hist;
	hist = 0;
	LOG_DEL; delete [] optstring;
	optstring = 0;
}

XHistogram :: ~XHistogram() {
	LOG_DEL; delete display;
	LOG_DEL; delete hist;
	LOG_DEL; delete [] optstring;
}
