#ifndef  _Tree_h
#define  _Tree_h 1
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer:  Allen Y. Lao

//  This module defines the TreeData and TreeNode classes which
//  encapsulate information sent by the MQRouter Star to the
//  MQController about all possible routes for the SCPE to send
//  packets to the DCPE for the ATM simulation.
//
//  First thing to realize is that no routing needs to be done if
//  the SCPE and DCPE reside on the same switch; in this case, the
//  controller would never call the Router star anyway to form trees.
//
//  The best way to understand the tree idea is with an example.
//  Refer to diagram in jcl's report, suppose that CPE #6
//  would like to talk to CPE #11.  The MQRouter star's responsibility
//  is to send back a total of five trees, each one corresponding to
//  a unique path in terms of traversed SWITCHES, repeat SWITCHES.
//  One tree represents the most direct route, two trees show middle-
//  case routing, and the final two are of worst-case routing.  In the
//  above case,
//
//  Tree #1:  SW #2 to SW #4 (direct)
//  Tree #2:  SW #2 to SW #3 to SW #4 (intermediate)
//  Tree #3:  SW #2 to SW #1 to SW #4 (intermediate)
//  Tree #4:  SW #2 to SW #1 to SW #3 to SW #4 (worst)
//  Tree #5:  SW #2 to SW #3 to SW #1 to SW #4 (worst)
//
//  Now, suppose we're looking at Tree #3.  Diagram:
//
//  
//                       ------
//                      |      |
//                      | SW 2 |
//                      |      |
//                      | 4  5 |
//                       ------
//                     /        \
//                    /          \
//             ------              ------
//            |      |            |      |
//            | SW 1 |            | SW 1 |
//            |      |            |      |
//            | 7  8 |            | 7  8 |
//             ------              ------
//           /        \          /        \
//          /          \        /          \
//    ------        ------   ------         ------
//   |      |      |      | |      |       |      |
//   | SW 4 |      | SW 4 | | SW 4 |       | SW 4 |
//   |      |      |      | |      |       |      |
//    ------        ------   ------         ------    
//
//
//
//
//  Looking at the class defintion of a TreeNode, it has a member for
//  a switch no., two vpi's, and pointers "left" and "right" to other
//  tree nodes.  The method makeTrees() would return a tree as above
//  for the sequence stated for Tree #3.  The switch number for each node is
//  marked with "SW," and the two associated VPI's are on the bottom of
//  each node box.  The diagonal lines might represent the pointers "left"
//  and "right" as each node points to other nodes.  The bottom row's nodes
//  are an exception; its left and right pointers are null.  Also these nodes
//  are not assigned any VPI's.  
//
//  A few observations can be made.  First, the tree, by virtue of the
//  configuration of the network being simulated, is "balanced."  That is,
//  the depth of the tree is the same no matter what path we take from the
//  top root node to any bottom leaf node.  Second, it exhibits symmetry.
//  If we "slice" the top node vertically in half, the nodes on the left and
//  halves of the tree are exactly identical.  This goes on and on; with each
//  of these half-trees, i.e. consisting of a SW 1 box and its two children
//  nodes, we can slice the root node in half again, and symmetry once again
//  holds.  This lends way to a straightforward means of traversing the
//  trees in finding a suitable path for the call request.
//
//  There will be cases when a node will have only one child.  This would
//  occur when we are trying to bridge two switches with only one connecting
//  path, (in the diagram, the switches would be diagonally opposite each
//  other.) The convention here is to assign the only child to the parent's
//  left pointer but to leave the right pointer null.
//    
//  Thus, the basic means the controller takes to find a virtual circuit
//  given the five trees as listed above is to look at each tree individually,
//  starting with the direct tree of course, and moving on to the next "worst"
//  tree if the present tree yields no success.  First, it would make an
//  inquiry about SW 2, VPI 4.  If this fails, it would try the right-side
//  VPI, that is SW 2, VPI 5, and if this also fails, the tree as a whole
//  fails.  Our basic idea is that if we are testing a "leftside" VPI and
//  get from the MQCCLT a NAK, we try the "rightside" VPI.  If it turns out
//  that any rightside VPI fails, as a rule, the tree fails.  We are assured
//  that this is the correct approach because of the tree's overall symmetry.
//  Also, if a leftside VPI inquiry has failed, and the current node has no
//  "right" member, that is, the current node has only one child, there is
//  likewise failure of the tree.  Otherwise, if the controller makes a successful
//  inquiry, it moves to the corresponding child node depending on which VPI
//  it just made an inquiry about.  The process resumes again.  At any time,
//  if the controller moves on to a node, and the node is seen to have no
//  children, that indicates a successful path has been found.
//
//  A few other notes about the controller's approach to this:  every time
//  it makes a successful inquiry, it records the SW and VPI numbers that
//  it just asked about into an object of class Conversation with the update()
//  method (refer to files Conversation.h, and .cc)  Of course, if the tree
//  winds up failing, the current "partially successful" path data is scratched.
//  Also note that this process requires no recursion due to the nature of the
//  trees.
//
//  As for the Router star, its principal method is "makeTrees" which tells
//  it to set up its member "TreeNode** trees."  This is an array of pointers
//  to TreeNodes, each pointer pointing to the root node of a distinct tree.
//  Thus, "trees" contains all the information of routing within itself and
//  forms the body of useful information in a TreeData object (derived from
//  PacketData.)
//
//  The Router star has need for a Preferred Routing Table (PRT) but no other
//  information needs to be made available for it to construct the trees as
//  described above.  TreeData has a method called doStage, which when passed
//  two switch numbers, determines the VPI(s) bridging the two switches and
//  sets up one level of the tree corresponding to this connection.  This
//  already explains how the direct tree is done.  For the other trees, consider
//  the case of trying to get from SW 1 to SW 3 with a non-direct route.
//  doStage would be called twice for each of the intermediate trees.
//
//   Intermediate #1: doStage (SW 1, SW 2) then doStage (SW 2, SW 3)
//   Intermediate #2: doStage (SW 1, SW 4) then doStage (SW 4, SW 3)
//
//  In a somewhat similar way, the worst-case trees can be constructed with 
//  multiple calls of doStage().  Note that doStage() is where the PRT is used.

*************************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "Message.h"
#include <Uniform.h>
#include <stream.h>

#define numTrees 5


class TreeNode 

{
public:
       TreeNode() :  vpiOne(0), vpiTwo(0), left(0), right(0) {}

       ~TreeNode() 
       { LOG_DEL;  delete left;  LOG_DEL;  delete right; }

       int           sw;                //  associated switch no. of node
       int           vpiOne, vpiTwo;    //  "left" and "right" VPI's resp.

       TreeNode*     left;              //  "left" and "right" point to children nodes
       TreeNode*     right;             //  recall if only one child, assign it to "left"
                                        //  and leave "right" null
};



class TreeData : public Message

{
private:
       TreeNode **       trees;    // contains all routing information

protected:
       Uniform*         random; 

public:
       TreeData ()   { LOG_NEW; trees = new TreeNode* [numTrees]; random = NULL; }

       ~TreeData()     {  delete [] trees;
                          if (random) { LOG_DEL; delete random; }  }

       void  doStage (int swFrom, int swTo, TreeNode *t);
       void  doTwoStage (int sw1, int sw2, int swTo, TreeNode *t);

       //  the workhorse method
       void  makeTrees (int swSrc, int swDest);

       //  generic packetData methods
       const char*  dataType()   const { return "TreeData"; }
       int isA (const char*) const;
       Message*  clone()      const { INC_LOG_NEW; return new TreeData; }
       TreeNode**   tree()       { return trees; }

};

       
#endif
