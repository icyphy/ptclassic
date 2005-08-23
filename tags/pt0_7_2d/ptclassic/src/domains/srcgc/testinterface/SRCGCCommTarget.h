/******************************************************************
Version identification:
@(#)SRCGCCommTarget.h 1.19  7/30/96

Copyright (c) 1990-1996 The Regents of the University of California.
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

Programmer: 

Target for Ariel S-56X DSP board.

*******************************************************************/
#ifndef _SRCGCCommTarget_h
#define _SRCGCCommTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "SRCGCTarget.h"
#include "StringState.h"
#include "IntState.h"

class SRCGCCommTarget : public SRCGCTarget {
  public:
   // Constructor
   SRCGCCommTarget(const char* name, const char* desc,
                   const char* assocDomain = SRCGCdomainName);

   // Copy Constructor
   SRCGCCommTarget(const SRCGCCommTarget&);

   /*virtual*/ void writeCode();
   /*virtual*/ int runCode();

   // Return a copy of itself
   /*virtual*/ Block* makeNew() const;

   // Type checking
   /*virtual*/ int isA(const char*) const;
    
   // Routines to construct CG wormholes, using the
   // $PTOLEMY/src/domains/cgc/targets/main/CGWormTarget
   /*virtual*/ CommPair fromCGC(PortHole&);
   /*virtual*/ CommPair toCGC(PortHole&);

   //   /*virtual*/ AsynchCommPair createPeekPoke(CGTarget& peekTarget,
   //                                             CGTarget& pokeTarget);
    
  protected:
   /*virtual*/ void headerCode();
   /*virtual*/ void trailerCode();
   /*virtual*/ void setup();

  private:
   int pairNumber;
   inline void configureCommPair(CommPair&);
   void initStates();
   CodeStream aioCmds;
   CodeStream shellCmds;
   StringState monitorProg;
};

#endif
