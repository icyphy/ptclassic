#ifndef _Cluster_h
#define _Cluster_h 1
#ifdef __GNUG__
#pragma interface
#endif


/*******************************************************************
 SCCS Version identification :
@(#)Cluster.h	1.31	11/13/97

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

 Programmers : Jose Luis Pino
 Date of Creation : 11/9/95
	
********************************************************************/

#include "DynamicGalaxy.h"
#include "GraphUtils.h"

class Scheduler;

/**********************************************************************
			  ClusterPort Class

   The big difference between GalPorts and ClusterPorts is that
   ClusterPorts maintain a notion of a farPort.  If we did not
   maintain the far port, we'd have to translate aliases everytime
   we wanted to see how a cluster is connected to another cluster.
   So, it is for efficiency - we also do not use geodesics.
 **********************************************************************/
class ClusterPort : public GalPort {
public:
    ClusterPort(PortHole&);
    ~ClusterPort();
    
    // Called after the galaxy is first converted to a clustering
    // hierarchy.  The inner-most ports are initialized first.
    virtual void initializeClusterPort();

    // Called after a merge/absorb operation
    virtual void update();
};

class Cluster : public DynamicGalaxy {
public:

    /******************************************************************
		      Constructors & Destructors
     ******************************************************************/
    inline Cluster():sched(0) {};
    inline ~Cluster() { setClusterScheduler(NULL); }

    /******************************************************************
			 Class Identification
     ******************************************************************/
    /*virtual*/ int isA(const char*) const;
    /*virtual*/ const char* className() const;
    /*virtual*/ int isItCluster () const; // {return TRUE;}

    /******************************************************************
			   Make new methods
     ******************************************************************/
    /*virtual*/ Block* makeNew() const { return new Cluster; }
    inline virtual ClusterPort* makeNewPort(PortHole& p) {
	return new ClusterPort(p);
    }
    // Calls makeNewPort and adds it to this cluster
    ClusterPort* addNewPort(PortHole&);
    
    /******************************************************************
			Group, Absorb, & Merge methods
			
Group: This creates a new cluster that contains the two argument clusters
       as its clusters.

Absorb: This absorbs the argument cluster into itself, increasing the
        the number of clusters inside itself by 1.

Merge: This merges the clusters inside the argument cluster into itself,
       increasing the number of clusters inside itself by the number
       of clusters inside of the argument cluster.
 
       These methods can optially delete the absorbed/merged block
       (and do so by default) from the original parent galaxy.

       The default mode (TRUE) operational cost of the methods is
       on order of:
			     O(N + Et*Ec)
		Where: N = nodes in the parent galaxy
	               Et = number of edges of 'this' cluster
		       Ec = number of edges of the cluster to
		            absorb/merge
		       
       There are two possibly more efficient ways of updating the
       original parent galaxy, which can reduce this cost to O(Et*Ec).
       
       The first method is if the block to absorb/merge was found
       using a GalTopBlockIter (or derived iterator class) on the
       parent galaxy, then the iterator's remove method can be used.

       Alternatively, if we are assured
       method could be used for a more effecient absorb/merge
       operation.  This will yield a unit time operation versus a
       time dependent on the number of blocks in the parent galaxy.
       Be careful, if FALSE is specified, we'll be left with a
       dangling pointer until ListIter::remove is called.
     ******************************************************************/

	// groups the argument cluster and this into a newly created empty
	// cluster and returns the new cluster
	virtual Cluster* group(Cluster& c, int removeFlag=TRUE);

    // absorbs the specified block into this cluster
    virtual int absorb(Cluster&, int /*removeFlag*/= TRUE);    

    // moves blocks and appropriate ports into this galaxy and
    // deletes the galaxy that was absorbed
    virtual int merge(Cluster&, int /*removeFlag*/= TRUE);

    inline void inheritNameParent(Block& b) {
	if (b.parent()) b.parent()->asGalaxy().addBlock(*this,b.name());
	else setName(b.name());
    }
    
    // Make sure a merge/absorb operation will not yield atomic actors
    // at the current clustering hierarchy level.  
    void makeNonAtomic();
    /******************************************************************
		      Cluster scheduler methods
     ******************************************************************/
    // Return the internal scheduler for the cluster if there is any
    /*virtual*/ Scheduler* scheduler() const;

    Scheduler* clusterScheduler() const;

    void setClusterScheduler(Scheduler*);

    /*virtual*/ int setTarget(Target*);
    
    /******************************************************************
		 Methods to convert a user-specified
	       galaxy hierarchy to a cluster hierarchy
     ******************************************************************/
    void initializeForClustering(Galaxy&);

    void initTopBlocksForClustering(Galaxy&,Galaxy&);

    inline virtual int flattenTest(Galaxy&) { return TRUE; }

    virtual Cluster* convertGalaxy(Galaxy&);
    
    virtual Cluster* convertStar(Star&);

private:
    Scheduler* sched;

};

/**********************************************************************
		       Cluster Iterator Classes

   The cluster iterator classes assumes that all top blocks are
   Clusters.  This property is TRUE assuming that
   Cluster::initializeForClustering has been called for interpreted
   Galaxy.

   These iterators also ignore pointers to bogus blocks which could
   have been left in the Galaxy using merge/absorb with the removeFlag
   set to FALSE.

   It would be nice to automatically cleanup the Galaxy that we are
   iterating over - but it would be dangerous.  This could cause a
   core dump if given a nested Cluster Iterator inside of another -
   where both are iterating over the same galaxy and one iterator
   removes a bogus entry which the other is currently pointing to.
 **********************************************************************/
class ClusterIter: private GalTopBlockIter {
public:
    inline ClusterIter(Galaxy& g):GalTopBlockIter(g),prnt(g) {};
    Cluster* next();
    Cluster* next(int flagValue, int index);
    inline Cluster* operator++(POSTFIX_OP) { return next();}
    GalTopBlockIter::reset;
    GalTopBlockIter::remove;
private:
    Block& prnt;
};

class SuccessorClusterIter: private SuccessorIter {
public:
    inline SuccessorClusterIter(Block&b):SuccessorIter(b),prnt(b.parent()) {};
    inline virtual ~SuccessorClusterIter() {};
    Cluster* next();
    Cluster* next(int flag_loc, int flag_val);
    inline Cluster* operator++(POSTFIX_OP) { return next();}
    SuccessorIter::reset;
private:
    Block* prnt;
};

class PredecessorClusterIter: private PredecessorIter {
public:
    PredecessorClusterIter(Block&b):
    PredecessorIter(b),prnt(b.parent()) {};
    
    inline virtual ~PredecessorClusterIter() {};
    Cluster* next();
    Cluster* next(int flag_loc, int flag_val);
    inline Cluster* operator++(POSTFIX_OP) { return next();}
    PredecessorIter::reset;
private:
    Block* prnt;
};

/**********************************************************************
		     ClusterPort Iterator Classes
 **********************************************************************/
class ClusterPortIter : private BlockPortIter {
public:
    inline ClusterPortIter(Block&b):BlockPortIter(b) {};
    inline ClusterPort* next() {
	return (ClusterPort*)BlockPortIter::next();
    }
    inline ClusterPort* operator++(POSTFIX_OP) {
	return ClusterPortIter::next();
    }
    BlockPortIter::reset;
    BlockPortIter::remove;
};

class ClusterOutputIter: private BlockOutputIter {
public:
    inline ClusterOutputIter(Block& b):BlockOutputIter(b) {};
    inline ClusterPort* next() {
	return (ClusterPort*)BlockOutputIter::next();
    }
    inline ClusterPort* next(int flag_loc, int flag_val) {
	return (ClusterPort*)BlockOutputIter::next(flag_loc,flag_val);
    }
    inline ClusterPort* operator++(POSTFIX_OP) {
	return ClusterOutputIter::next();
    }
    BlockOutputIter::reset;
    BlockOutputIter::remove;
};

class ClusterInputIter: private BlockInputIter {
public:
    inline ClusterInputIter(Block& b):BlockInputIter(b) {};
    inline ClusterPort* next() {
	return (ClusterPort*)BlockInputIter::next();
    }
    inline ClusterPort* next(int flag_loc, int flag_val) {
	return (ClusterPort*)BlockInputIter::next(flag_loc,flag_val);
    }
    inline ClusterPort* operator++(POSTFIX_OP) {
	return ClusterInputIter::next();
    }
    BlockInputIter::reset;
    BlockInputIter::remove;
};

/**********************************************************************
		   Miscellaneous Cluster Functions
 **********************************************************************/

// A cluster is defined as valid if is both nonempty and the cluster's
// parent is equal to that specified in this function's prnt argument.
int isValidCluster(Cluster&, Block* /*prnt*/);

// Remove all bogus clusters - optionally do this recursively.
// It is not necessary to do this recursively if you have only done
// merges and aborbs at the top level of the give Galaxy.
void cleanupAfterCluster(Galaxy&,int /*recursive*/ = FALSE);

// This method returns a StringList that describes the Cluster
// interconnection inside a give Galaxy in dotty format.  To find out
// more about this format refer to the URL:
// http://www.research.att.com/orgs/ssr/book/reuse.
StringList printClusterDot(Galaxy&);
#endif
