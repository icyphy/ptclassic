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

Copyright (c) 1996-%Q% The Regents of the University of California.
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

class ostream;

/****

This is a simple integer matrix class designed to just hold things.

@Author Praveen K. Murthy
****/
class SimpleIntMatrix {
public:
    // m is the actual array, kept public for simplicity
    int **m;

    // number of rows and number of columns
    int nrows; int ncols;

    // Constructor
    SimpleIntMatrix (int nr=1, int nc=1);

    // Destructor
    ~SimpleIntMatrix();

    // Resize the matrix
    void resize(int nr, int nc);

    // equality operator
    SimpleIntMatrix& operator=(const SimpleIntMatrix& a);

    // Copy constructor
    SimpleIntMatrix(const SimpleIntMatrix& a);
};

ostream& operator << (ostream& o, const SimpleIntMatrix& a);

/////////////////////////
// class AcyLoopScheduler //
/////////////////////////
/****

AcyLoopScheduler is a loop scheduler to optimize for buffer sizes.
<a name="ClassAcyLoopScheduler">
@Description Implements algorithms and heuristics described in Chapters 6,7 of 
<a href="http://ptolemy.eecs.berkeley.edu/~murthy/book.html">
	<em>"Software Synthesis from Dataflow Graphs",</em></a> by
<p>
Shuvra S. Bhattacharyya, 
<a href="http://ptolemy.eecs.berkeley.edu/~murthy/">
Praveen K. Murthy</a>
, and Edward A. Lee,
Kluwer Academic Publishers, Norwood, MA, 1996
<p>
This scheduler generates single appearance looped schedules for acyclic
graphs.
</a>

@Author Praveen K. Murthy
****/
class AcyLoopScheduler : public SDFScheduler {
public:

	// constructor
	AcyLoopScheduler(const char* log=0);

	// destructor
	~AcyLoopScheduler();

	// the main routine
	int computeSchedule(Galaxy& g);

	// return TRUE if graph is well-ordered
	int isWellOrdered(Galaxy* g, SequentialList& topsort);

	// compute optimal loop hierarchy
	int DPPO();


	// A routine called by schedule command in ptcl or display-schedule
	// in pigi
	/* virtual */ StringList displaySchedule();

	// run method for code-generation targets
	/* virtual */ void compileRun();

	// Compute the BMLB bound for the galaxy
	int computeBMLB();

	// Set up and execute everything related to RPMC+DPPO
	int callRPMC(AcyCluster* clusterGraph);

	// Set up and execute everything related to APGAN+DPPO
	int callAPGAN(Galaxy& gal);

	// Check that the topSort array represents a valid topological sort
	int checkTopsort();

	// Set the buffer sizes after schedule has been computed
	// return TRUE if no errors
	int fixBufferSizes(int i, int j);

	// A hack to do a pre-pass on all the spliced-in stars
	int clusterSplicedStars(AcyCluster*);
	
	// For debugging; print out various matrices that the scheduler
	// uses

	// this one prints out the reachability, incidence, and delay matrices
	void printTopMatrices();

	// this one prints out the matrices used by DPPO
	void printDPPOMatrices();

	// this one prints out the RPMC and APGAN topSort arrays
	void printTopSorts();

	// this one prints out the node number corresponding to each star
	void printStarNumbers();

protected:
	// create the nodelist; TRUE if no errors
	int createNodelist(Galaxy& gal);

	// generate top-sort using RPMC
	int RPMC(AcyCluster* gr);

	// generate top-sort using APGAN
	int APGAN(Galaxy* gr);

	// This one actually does the work for displaySchedule()
	StringList dispNestedSchedules(int depth, int i, int j, int g);

	// This one does the work for compileRun
	void genCode(Target& t, int depth, int i, int j, int g);

	// This one is called from AcyScheduler::runOnce and does the
	// actual work
	int runOnce(int i, int j, int g);

	// Create the incMatrix and delMatrix
	void createIncidenceMatrix(Galaxy& gal);

	// Create reachMatrix, returns TRUE if successfull
	int createReachabilityMatrix(Galaxy& gal);

	// This is used by <code>createReachabilityMatrix</code>
	int visitSuccessors(Block* s, int flagLoc);

	// Called from SDFScheduler::run
	/* virtual */ void runOnce();

	// Build up the top. sort after APGAN clustering process.
	int buildTopsort(AcyCluster* gr, int ti);

	// create edgelist; called by APGAN
	void createEdgelist(Galaxy* gr);

	// propagate flag values at specified location to the cluster
	// wrappers after galaxy has been initialized to a clustering
	// hierarchy.
	void copyFlagsToClusters(Galaxy* gr, int flagLocation);

	// To keep top level a cluster also, we just have one cluster
	// inside the galaxy after initializeForClustering.
	int addTopLevelCluster(Galaxy* gal);

private:
	// flag to indicate whether schedule was successfully computed.
	int schedSuccess;

	// log file where all output from here is written to if specified
	const char* logFile;

	// Incidence matrix
	SimpleIntMatrix incMatrix;

	// delay matrix to hold delays on arcs
	SimpleIntMatrix delMatrix;

	//Reachability matrix
	SimpleIntMatrix reachMatrix;

	// Cost matrix for schedule; generated by DPPO()
	SimpleIntMatrix costMatrix;

	// This matrix holds the schedule info; generated by DPPO()
	SimpleIntMatrix splitMatrix;

	// This matrix holds the gcds of sub-schedules; generated by DPPO()
	SimpleIntMatrix gcdMatrix;

	// Graph size that we are dealing with
	int graphSize;

	// topSort points to either RPMCTopSort or APGANTopSort
	int* topSort;

	// This is allocated as an array to hold the indices of nodes in
	// topological order; used by RPMC
	int* RPMCTopSort;

	// This is allocated as an array to hold the indices of nodes in
	// topological order; used by APGAN
	int* APGANTopSort;

	// Inverse topSort array (i.e, topSort(topInvSort[i]) = i)
	int* topInvSort;

	// an array containing pointers of all stars in the universe
	DataFlowStar** nodelist;

	// a list of all the edges in the graph; this list is used and
	// modeified by APGAN
	SequentialList edgelist;
};

#endif
