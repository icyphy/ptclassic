/******************************************************************
Version identification:
$Id$

Copyright (c) 1997-%Q% The Regents of the University of California.
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

 Programmer:  Shang-Pin Chang
 Date of creation: 11/2/97 

 Definition of DESiPStar (interface between SPIN and Ptolemy DE domain).

*******************************************************************/

#ifndef _DESiPStar_h_
#define _DESiPStar_h_

#include "DERepeatStar.h"
#include "spin.h"
#ifdef PC
#include "y_tab.h"
#else
#include "y.tab.h"
#endif


	////////////////////////////////////
	// class DESiPStar
	////////////////////////////////////

class DESiPStar : public DERepeatStar {

protected:
	int sipID;
        int SPINReady;
        double NxtFireDur;
        RunList *run_Proc;
        double RunSpin(RunList *, double);
        void outputPML(int, const char *, const char *,
                const char *, const char *, const char *);
        void setNextFiring(void);
        void GlobalUpdate(void);
        void registerPXPStar(void);
        void unregisterPXPStar(void);
	virtual const char* getScope(void) { return NULL; }
public:
	// SPIN call C-function via func()
	virtual int func(int,int) { return 0; }
	int arglist[256];

	// constructor
	DESiPStar();

	// destructor
	~DESiPStar();

        // class identification
        /* virtual */ int isA(const char*) const;

};

typedef struct PXPStarList {
        DESiPStar* entry;
        struct PXPStarList* next;
} PXPStarList;

typedef struct varList {
        char *entry;
        struct varList *next;
} varList;

#endif
