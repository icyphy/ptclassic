static const char file_id[] = "Tree.cc";

/* 
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
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Tree.h"
#include <ACG.h>


extern ACG*  gen;

ISA_FUNC (TreeData, Message);

//  The Preferred Routing Table (PRT)
//  Note there is implicit association between VPI's 7 and 8, 4 and 5

static int  prta [4][4] = { {0, 4, 6, 7},
                            {4, 0, 7, 6},
                            {6, 7, 0, 4},
                            {7, 6, 4, 0} };

static int  prtb [4][4] = { {0, 5, 6, 8},
                            {5, 0, 8, 6},
                            {6, 8, 0, 5},
                            {8, 6, 5, 0} };


void
TreeData :: doStage (int swFrom, int swTo, TreeNode *t)

{
       int vpi;

//     goal is to construct one level of tree corresponding to a connection
//     or path from one switch to another

//     the left child will always exist, assign a vpi as determined by PRT
//     above after allocating it and setting its switch number member, the
//     vpi represents a direct link between the two switches
       LOG_NEW; t->left = new TreeNode();
       t->left->sw = swTo;

//     to randomize routing, choose from one of two PRT's above
       double p = (*random)();

       if (int(p)) vpi = prta [swFrom-1] [swTo-1];
       else        vpi = prtb [swFrom-1] [swTo-1];

       t->vpiOne = vpi;

//     if the VPI was 4, 5, 7, or 8 this means that the swFrom is doubly connected
//     to swTo, need to build the right child node

       if (vpi == 4) {
         t->vpiTwo = 5;
         LOG_NEW; t->right = new TreeNode();
       }
      
       if (vpi == 5) {
         t->vpiTwo = 4;
         LOG_NEW; t->right = new TreeNode();
       }
      
       if (vpi == 7) {
         t->vpiTwo = 8;
         LOG_NEW; t->right = new TreeNode();
       }

       if (vpi == 8) {
         t->vpiTwo = 7;
         LOG_NEW; t->right = new TreeNode();
       }

//     if the "right" child exists, set its switch number
       if (t->right)  { t->right->sw = swTo; }

}
    


void 
TreeData :: doTwoStage (int swFrom, int swInterm, int swTo, TreeNode* t)

{

//     this method essentially builds two levels of the tree, given
//     a source switch, switch to pass over, and destination switch

       doStage (swFrom, swInterm, t);
       doStage (swInterm, swTo, t->left);
       if (t->right) { doStage (swInterm, swTo, t->right); }

}



void 
TreeData :: makeTrees (int swSrc, int swDest)

{
       double p;

//     prepare random number generators
       if (random) { LOG_DEL; delete random; }
       LOG_NEW; random = new Uniform (0.0, 2.0, gen);

//     construct the direct routing tree (first of five trees)

       LOG_NEW; TreeNode*  ta = new TreeNode;
       ta->sw = swSrc;
       doStage (swSrc, swDest, ta);
       trees[0] = ta;

//     set up the two intermediate trees
//     to do this, need the numbers of the other two switches besides
//     the source and dest switch 

       int  sw1 = 0, sw2 = 0, hold;

       for (int i = 1; i<=4; i++)
         if (i != swSrc && i != swDest)  { sw1 = i;  break; }
       for (i = 1; i<=4; i++)
         if (i != swSrc && i != swDest && i != sw1)  { sw2 = i;  break; }

//     for purposes of randomization do a coin flip for sw1 and sw2
       p = (*random)();

       if (int(p)) {
         hold = sw1;
         sw1 = sw2;
         sw2 = hold;
       }

       LOG_NEW; TreeNode*  tb = new TreeNode;
       tb->sw = swSrc;
       LOG_NEW; TreeNode*  tc = new TreeNode;
       tc->sw = swSrc;

       doTwoStage (swSrc, sw1, swDest, tb);
       doTwoStage (swSrc, sw2, swDest, tc);
       trees[1] = tb;
       trees[2] = tc;

//     Now do the worst case trees
     
//     randomize sw1 and sw2 again
       p = (*random)();

       if (int(p)) {
         hold = sw1;
         sw1 = sw2;
         sw2 = hold;
       }

       LOG_NEW; TreeNode* td = new TreeNode;
       td->sw = swSrc;
       LOG_NEW; TreeNode* te = new TreeNode;
       te->sw = swSrc;

       doStage (swSrc, sw1, td);
       doStage (swSrc, sw2, te);

       doTwoStage (sw1, sw2, swDest, td->left);
       if (td->right) { doTwoStage (sw1, sw2, swDest, td->right); }
       doTwoStage (sw2, sw1, swDest, te->left);
       if (te->right) { doTwoStage (sw2, sw1, swDest, te->right); }
       trees[3] = td;
       trees[4] = te;

}

