#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
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
*/
/*  graphExtract  - Graph extraction package
 *
 *  Author:        Enrico Malavasi
 *  Last revised:  July 1991
 */
#include "port.h"
#include "copyright.h"
#include "utility.h"
#include "oct.h"
#include "oh.h"
#include "fang.h"
#include "harpoon.h"
#include "tap.h"
#include "errtrap.h"
#include "pepint.h"

#define  NODE_ALLOC    300;
#define  EDGE_ALLOC    300;

/*  Definition of node and edge structures */
typedef struct khNode {
  octId    id;             /*  Terminal id  */
  int      red;            /*  1 if it cannot be reduced  */
} khNode;
typedef struct khEdge {
  int      n1;             /*  First node   */
  double   res;            /*  Resistance   */
  int      n2;             /*  Second node  */
} khEdge;

typedef struct Kh {
  octId id;
  int   node_alloc;         /*  No. of allocated nodes  */
  int   edge_alloc;         /*  No. of allocated edges  */
  int   node_size;          /*  No. of nodes defined  */
  int   edge_size;          /*  No. of edges defined  */
  int   GenPointer;         /*  Next edge to be returned by generator */
  octStatus Sorted;        /*  Graph has been sorted */
  khNode  *node;           /*  Nodes  */
  khEdge  *edge;           /*  Edges  */
} Kh;                      /*  This is the graph     */

static int gphNo;          /*  Current graph id no. */
static int gph_size = 0;   /*  Total no. of graphs extracted  */
static Kh  *gph;           /*  Array of the graphs extracted  */

#define     GRAPH  gph[gphNo]

static int
pepGetGphNo(net)
octObject  *net;
/*
 *  Given a net return the graph extracted from it, if any.
 *  If no graph has been extracted so far, return (-1)
 */
{
  int  i;
  for(i=0 ; i<gph_size ; i++)
    if(gph[i].id == net->objectId)
      return i;
  return (-1);
}

static int
pepNodeNo(id)
octId  id;              /*  Connector id */
/*
 *  Given the id of a connector, return its position in the node array
 *  for the current graph. If necessary, allocate a new node for it.
 */
{
  int  i;
  
  for(i=0 ; i<GRAPH.node_size ; i++)
    if(GRAPH.node[i].id == id)
      return i;      /*  There it is: in the array ! */

  if(GRAPH.node_size >= GRAPH.node_alloc) {
    GRAPH.node_alloc += NODE_ALLOC;
    GRAPH.node = REALLOC(khNode, GRAPH.node, GRAPH.node_alloc);
  }
  GRAPH.node[GRAPH.node_size].id  =  id;
  GRAPH.node[GRAPH.node_size].red =  0;   /*  Reducible by default */
  return GRAPH.node_size++;
}

static void
pepSort()
/*  Now sort the items in the current graph, with decreasing resistance */
{
  int  i, j;                        /*  Loop counter  */

  /*  2.) Sort according to res  */
  for(i=0 ; i<GRAPH.edge_size ; i++)
    for(j=i+1 ; j<GRAPH.edge_size ; j++)
      if(GRAPH.edge[i].res < GRAPH.edge[j].res)
	SWAP(khEdge, GRAPH.edge[i], GRAPH.edge[j]);
  GRAPH.Sorted = OCT_OK;
}

octStatus
pepGraphExtract(net)
octObject   *net;
/*
 *  This function extracts the graph from the given net. No reduction
 *  is performed at this stage. Actual terminals from instances and
 *  terminals attached to formal terms are marked as non-reducible, that
 *  is they must remain visible during serial/parallel reduction.
 *  Vias, contacts and corners are NOT considered !!!.
 *  Returns OCT_OK or OCT_ERROR if the graph is empty (no segments).
 */
{
  octGenerator  Gener,
                TGener;           /*    terminal generator  */
  octObject     term1, term2;     /*    terminal  */
  octObject     path;             /*    path segment  */
  double        pepPathRes();
  int           i;                /*    Loop counter  */

/*   Initialize the graph, if it does not exist already  */
  if((gphNo=pepGetGphNo(net)) < 0) {
    gphNo = gph_size++;
    if(gph_size == 1)
      gph = ALLOC(Kh, gph_size);
    else
      gph = REALLOC(Kh, gph, gph_size);
    
    GRAPH.node_alloc = NODE_ALLOC;
    GRAPH.node       = ALLOC( khNode, GRAPH.node_alloc);

    GRAPH.edge_alloc = EDGE_ALLOC;
    GRAPH.edge       = ALLOC( khEdge, GRAPH.edge_alloc);

    GRAPH.id         = net->objectId;
  }
  GRAPH.node_size = 0;
  GRAPH.edge_size = 0;
  GRAPH.GenPointer = (-1);

  /*  Extract all the segments and their terminals  */
  OH_ASSERT(octInitGenContents(net, OCT_PATH_MASK, &Gener));
  while(octGenerate(&Gener, &path) == OCT_OK) {
    OH_ASSERT(octInitGenContents(&path, OCT_TERM_MASK, &TGener));
    if(octGenerate(&TGener, &term1) == OCT_OK)
      if(octGenerate(&TGener, &term2) == OCT_OK) {

	if(GRAPH.edge_size >= GRAPH.edge_alloc) {
	  GRAPH.edge_alloc += EDGE_ALLOC;
	  GRAPH.edge=REALLOC(khEdge,GRAPH.edge,GRAPH.edge_alloc);
	}
	GRAPH.edge[GRAPH.edge_size].n1   =  pepNodeNo(term1.objectId);
	GRAPH.edge[GRAPH.edge_size].res  =
	  pepPathRes(&path, &term1, &term2);
	GRAPH.edge[GRAPH.edge_size].n2   =  pepNodeNo(term2.objectId);

	GRAPH.edge_size++;  
	continue;
      }
    errRaise(PEP_PKG_NAME, 1, "Wrong connectors found in path");
  }

  if(GRAPH.edge_size <= 0)
    return OCT_ERROR;

  /*  Set the non-reducible nodes.  */
  for(i=0 ; i<GRAPH.node_size ; i++) {
    ohGetById(&term1, GRAPH.node[i].id);
    /*  Instance terminals:  */
    if(octIsAttached(net, &term1) == OCT_OK)
      GRAPH.node[i].red = 1;
    /*  Formal terminals:  */
    if(octGenFirstContainer(&term1, OCT_TERM_MASK, &term2) == OCT_OK)
      GRAPH.node[i].red = 1;
  }

  /*  Sort. For each item set (n1 < n2)  */
  for(i=0 ; i<GRAPH.edge_size ; i++)
    if(GRAPH.edge[i].n1 > GRAPH.edge[i].n2)
      SWAP(int, GRAPH.edge[i].n1, GRAPH.edge[i].n2);

  GRAPH.Sorted = OCT_ERROR;
  return OCT_OK;
}

static int
pepSerialReduce()
/* 
 *  Look for the series of two elements. If found, they are substituted
 *  by a new segment whose res is the sum of their resistances.
 *  If a series is found and reduced, return 1, otherwise return 0.
 */
{
  int  i, j,                     /*  Loop counters  */
       cnt;                      /*  Counter - segments on a node  */
  int  e[2];                     /*  Segment indexes  */
  int  n1,  n2;                  /*  New serial extremes  */ 

  for(i=0 ; i<GRAPH.node_size ; i++)
    if(GRAPH.node[i].red == 0) {

      for(j=0,cnt=0 ; j<GRAPH.edge_size ; j++) {
	if((GRAPH.edge[j].n1 == i) ||
	   (GRAPH.edge[j].n2 == i)) {
	  if(cnt >= 2) {
	    cnt++;
	    break;
	  }
	  e[cnt++] = j;
	}
      }
  
      if(cnt == 2) {
	n1 = (GRAPH.edge[e[0]].n1 == i) ?
	  GRAPH.edge[e[0]].n2 :  GRAPH.edge[e[0]].n1;
	n2 = (GRAPH.edge[e[1]].n1 == i) ?
	  GRAPH.edge[e[1]].n2 :  GRAPH.edge[e[1]].n1;
	if(n1 == n2)    continue;   /*  Parallel */
	if(n1 > n2)
	  SWAP(int, n1, n2);
	GRAPH.edge[e[0]].n1  = n1;
	GRAPH.edge[e[0]].n2  = n2;
	GRAPH.edge[e[0]].res = GRAPH.edge[e[0]].res + GRAPH.edge[e[1]].res;

	GRAPH.edge_size--;
	for(j=e[1] ; j<GRAPH.edge_size ; j++)
	  GRAPH.edge[j] = GRAPH.edge[j+1];

	GRAPH.Sorted = OCT_ERROR;
	return  1;
      }
    }

  return 0;
}

static int
pepParallelReduce()
/* 
 *  Look for the parallel of two elements. If found, they are substituted
 *  by a new segment whose res is the parallel of their resistances.
 *  If a parallel is found and reduced, return 1, otherwise return 0.
 */
{
  int  i, j;

  for(i=0,j=1 ; j<GRAPH.edge_size ; i++,j++)
    if(GRAPH.edge[i].n1 == GRAPH.edge[j].n1)
      if(GRAPH.edge[i].n2 == GRAPH.edge[j].n2) {
	GRAPH.edge[i].res = (GRAPH.edge[i].res*GRAPH.edge[j].res) /
	                 (GRAPH.edge[i].res+GRAPH.edge[j].res);
	GRAPH.edge_size--;
	for( ; j<GRAPH.edge_size ; j++)
	  GRAPH.edge[j] = GRAPH.edge[j+1];

	GRAPH.Sorted = OCT_ERROR;
	return  1;
      }

  return 0;
}

octStatus
pepReduce(net)
octObject  *net;
/*
 *  Perform full reduction by parallel/serial iteration.
 *  Return OCT_OK or OCT_ERROR if the graph has not been extracted or
 *  it is null.
 */
{
  int  action;

  if((gphNo=pepGetGphNo(net)) < 0)
    return OCT_ERROR;

  for(action=1 ; action==1 ; ) {
    action = 0;
    while(pepSerialReduce())
      action = 1;
    while(pepParallelReduce())
      action = 1;
  }

  return OCT_OK;
}

octStatus
pepInitGenerator(net)
octObject *net;
/*
 *  Initialize the resistance sequence generator.
 *  Return OCT_OK or OCT_ERROR if the graph has not been
 *  correctly extracted previously.
 */
{
  if((gphNo=pepGetGphNo(net)) < 0)
    return OCT_ERROR;

  if(GRAPH.edge_size <= 0) {
    GRAPH.GenPointer = (-1);
    return OCT_ERROR;
  }
  GRAPH.GenPointer = 0;
  return OCT_OK;
}

double
pepGenerateRes(net, term1, term2)
octObject  *net,
           *term1, *term2;
/*
 *  Each call returns the value of one of the path resistances in the
 *  starting from the highest one and in decreasing order.
 *  After the smallest one, a negative value is returned.
 */
{
  if((gphNo=pepGetGphNo(net)) < 0)
    return OCT_ERROR;

  if((GRAPH.GenPointer < 0) ||
     (GRAPH.GenPointer >= GRAPH.edge_size))
    return ((double) -1);
  
  if(GRAPH.Sorted != OCT_OK) {
    GRAPH.GenPointer = 0;
    pepSort();
  }
  
  ohGetById(term1,GRAPH.node[GRAPH.edge[GRAPH.GenPointer].n1].id);
  ohGetById(term2,GRAPH.node[GRAPH.edge[GRAPH.GenPointer].n2].id);
  
  return GRAPH.edge[GRAPH.GenPointer++].res;
}


octStatus
pepFreeGraph(net)
octObject *net;
/*
 *  Free the stuff allocated by the graph extractor.
 *  Return OCT_OK or OCT_ERROR if no graph had been allocated for
 *  this net
 */
{
  int  id;

  if((id=pepGetGphNo(net)) < 0)
    return OCT_ERROR;       /*  Not allocated  */

  FREE(gph[id].node);
  FREE(gph[id].edge);
  gph_size--;
  if(id != gph_size)
    gph[id] = gph[gph_size];
  return  OCT_OK;
}
