/******************************************************************
  Version $Id$

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

    Programmer:		Mike J. Chen
    Date of creation:   13 August 1993

*******************************************************************/
#ifndef _MDSDFStar_h
#define _MDSDFStar_h

#ifdef __GNUG__
#pragma interface
#endif

#include "SDFStar.h"
#include "MDSDFPortHole.h"

class MDSDFStar : public SDFStar {
 public:
    // Constructor
    MDSDFStar();

    // inherited from SDF
    // int isSDF() const;                /* returns TRUE */
    // int isSDFinContext() const;       /* returns TRUE */

    // Class identification.
    /* virtual */ int isA(const char* className) const;

    // Domain identification.
    /* virtual */ const char* domain() const;

    // Domain-specific initialization, done after scheduling, before running.
    /* virtual */ void initialize();

    // Domain-specific function to simulate a star firing for scheduling
    int simRunStar(int rowIndex, int colIndex, int deferFiring);

    // Domain-specific function to test if a star is deferrable
    /* virtual */ int deferrable();

    // Domain-specific function to test if a star is runnable
    /* virtual */ int notRunnable();

    // Domain-specific function to fire a star
//    /* virtual */ int run();
//  DataFlowStar::run;

    // inherited from DataFlowStar
    // Fraction repetitions;
    // unsigned noTimes;  // number of times star has fired

    // During scheduling, the scheduler must keep track of how many times
    // it has scheduled a star in each dimension.  This is a convenient
    // place to do that.
    unsigned rowIndex;
    unsigned colIndex;

    Fraction rowRepetitions;  // the number of row and column firings for
    Fraction colRepetitions;  // this star per iteration

//    Fraction initRowRepetitions;  // the number of row and column firings for
//    Fraction initColRepetitions;  // for this star on its initial iteration

    int startRowIndex;  // the current row and column firing index for this
    int startColIndex;  // star
};


#endif
