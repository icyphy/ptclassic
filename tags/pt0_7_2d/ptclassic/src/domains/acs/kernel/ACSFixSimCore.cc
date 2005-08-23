static const char file_id[] = "ACSFixSimCore.cc";
/**********************************************************************
Copyright (c) 1998-1998 The Regents of the University of California.
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


 Programmers:  J. A. Lundblad
 Date of creation: 3/11/98
 Version: @(#)ACSFixSimCore.cc	1.7 09/21/99

***********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ACSFixSimCore.h"
#include "ACSKnownCategory.h"
#include <stdio.h>              // sprintf()

// isA
ISA_FUNC(ACSFixSimCore, ACSCore);

// Global category string
extern const char ACSFixSimCategory[] = "FixSim";

// add string to KnownCategory list
static ACSFixSimCore proto;
static ACSKnownCategory	entry(proto);

// there is no corona member in the base class but we need to pass the
// corona to the constructor to addStates to it.
ACSFixSimCore::ACSFixSimCore(ACSCorona& corona) : ACSCore(ACSFixSimCategory) {

corona.addState(OverflowHandler.setState("OverflowHandler",this,"saturate","Overflow characteristic for the output.\nIf the result of the sum cannot be fit into the precision of the output,\nthen overflow occurs and the overflow is taken care of by the method\nspecified by this parameter.\nThe keywords for overflow handling methods are:\n\"saturate\" (the default), \"zero_saturate\", \"wrapped\", and \"warning\".\nThe \"warning\" option will generate a warning message whenever overflow occurs."));

corona.addState(ReportOverflow.setState("ReportOverflow",this,"NO","If non-zero, e.g. YES or TRUE, then after a simulation has finished,\nthe star will report the number of overflow errors if any occurred\nduring the simulation."));

corona.addState(RoundFix.setState("RoundFix",this,"YES","If YES or TRUE, then all fixed-point computations, assignments, and\ndata type conversions will be rounded.\nOtherwise, truncation will be used."));

} 

// copied from SDFFix
void ACSFixSimCore::wrapup() {
if ( int(ReportOverflow) && ( overflows > 0 ) ) {
		  StringList msg;
		  char percentageStr[24];      // must be at least 6 chars
		  double percentage = 0.0;
		  if ( totalChecks > 0 )
		    percentage = (100.0*overflows)/totalChecks;
		  // truncate percentage to one decimal place
		  sprintf(percentageStr, "%.1f", percentage);
		  msg << " experienced overflow in " << overflows 
		      << " out of " << totalChecks
		      << " fixed-point calculations checked ("
		      << percentageStr << "%)";
		  Error::warn( *this, msg );
		}
}

// copied from SDFFix
void ACSFixSimCore::setup() {
overflows = 0;
		totalChecks = 0;
}

// copied from SDFFix
int ACSFixSimCore::checkOverflow (Fix& fix)
{
int overflag = fix.ovf_occurred();
			totalChecks++;
			if ( overflag ) {
			  overflows++;
			  fix.clear_errors();
			}
			return overflag;
}



