#ifndef _AcyLoopScheduler_h
#define _AcyLoopScheduler_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "SDFScheduler.h"
#include "SDFCluster.h"
#include "StringList.h"
#include "AcyCluster.h"

/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer:  Praveen Murthy
 Date of creation: Feb/10/96

Acyclic loop scheduler to optimize for buffer sizes.
Implements algorithms and heuristics described in

	"Software synthesis from dataflow graphs", by

Shuvra S. Bhattacharyya, Praveen K. Murthy, and Edward A. Lee,
Kluwer Academic Publishers, 1996

*******************************************************************/


class Matrix {
public:
	int **m;
	int nrows, ncols;
	Matrix (int nr=1, int nc=1) : nrows(nr), ncols(nc) {
	    m = new int *[nr];
	    for (int i=0; i<nr; i++) {
		m[i] = new int[nc];
		for (int j=0; j<nc; j++) m[i][j] = 0;
	    }
	}
	~Matrix() {
	    for(int i=0;i<nrows;i++) delete [] m[i];
	    delete m;
	}
	Matrix& operator=(const Matrix& a) {
	    for(int i=0; i<nrows; i++) delete m[i];
	    delete m;
	    m = new int *[nrows=a.nrows];
	    ncols=a.ncols;
	    for (i=0; i<nrows; i++) {
		m[i] = new int[ncols];
		for (int j=0; j<ncols; j++) m[i][j] = a.m[i][j];
	    }
	    return *this;
	}

	Matrix(const Matrix& a) {
	    m = new int *[nrows=a.nrows];
	    ncols=a.ncols;
	    for(int i=0;i<nrows;i++) {
		m[i] = new int[ncols];
		for(int j=0;j<ncols;j++) m[i][j]=a.m[i][j];
	    }
	}
    };
/////////////////////////
// class AcyLoopScheduler //
/////////////////////////
/****

AcyLoopScheduler is a loop scheduler to optimize for buffer sizes
<a name="ClassAcyLoopScheduler">
@Description Implements algorithms and heuristics described in Chapters 6,7 of 
<a href="ptolemy.eecs.berkeley.edu/~murthy/book.html">
	<em>"Software Synthesis from Dataflow Graphs",</em></a> by

Shuvra S. Bhattacharyya, Praveen K. Murthy, and Edward A. Lee,
Kluwer Academic Publishers, Norwood, MA, 1996
</a>
****/
class AcyLoopScheduler : public SDFScheduler {
public:

	AcyLoopScheduler();
	~AcyLoopScheduler(); // destructor
	// the main routine
	int computeSchedule(Galaxy& g);
	// return TRUE if graph is well-ordered
	void isWellOrdered(Galaxy* g, SequentialList& topsort);
	int DPPO();		// compute optimal loop hierarchy
	int RPMC(AcyCluster* gr);	// generate top-sort using RPMC
	int APGAN(AcyCluster* gr);	// generate top-sort using APGAN
//	SDFCluster* cluster(int i, int j);	// cluster using DPPO results

	virtual StringList displaySchedule();
	virtual void compileRun();
	void fixBufferSizes(int i, int j);
	int clusterSplicedStars(AcyCluster*);
	
protected:
	void genCode(Target& t, int depth, int i, int j, int g);
	StringList dispNestedSchedules(int depth, int i, int j, int g);
	void runOnce(int i, int j, int g);
	void createIncidenceMatrix(Galaxy& gal);
	void createReachabilityMatrix(Galaxy& gal);
	virtual void runOnce();
	int buildAPGANTopsort(AcyCluster* gr, int ti);
	void createEdgelist(AcyCluster* gr);
	void copyFlagsToClusters(AcyCluster* gr, int flagLocation);
private:
	Matrix* incMatrix;	// Incidence matrix
	Matrix* delMatrix;  // delay matrix to hold delays on arcs
	Matrix* reachMatrix; //Reachability matrix
	Matrix* costMatrix;
	Matrix* splitMatrix;
	Matrix* gcdMatrix;
	int graphSize;  // Graph size that we are dealing with
	int* topSort;
	int* RPMCTopSort;
	int* APGANTopSort;
	int* topInvSort;
	DataFlowStar** nodelist;
	SequentialList edgelist;
	AcyCluster* graph;
};

#endif
