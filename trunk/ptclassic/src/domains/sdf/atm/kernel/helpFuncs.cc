static const char file_id[] = "helpFuncs.cc";
/**************************************************************************
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

 Programmer:  Allen Y, Lao
 Date of creation:  7/19/91

 Simple methods for packet-type checking: voice and tree type data


*************************************************************************/

#include "VoiceData.h"
#include "Tree.h"
#include "Block.h"


int voiceCheck (Envelope& e, NamedObj& n)  {

       if (!e.typeCheck("VoiceData"))  {
         const char* msg = e.typeError("VoiceData");
         Error :: abortRun (n, msg, "error in incoming packet.");
         return FALSE;
       }

       return TRUE;

}



int treeCheck (Envelope& e, NamedObj& n)  {

       if (!e.typeCheck("TreeData"))  {
         const char* msg = e.typeError("VoiceData");
         Error :: abortRun (n, msg, "error in incoming packet.");
         return FALSE;
       }

       return TRUE;

}



//  for ATM simulation, this converts a CPE vci (range 1-12)
//  to appropiate vpi value (1, 2, or 3) or Switch number
//  from 1 to 4


int vciToVpi (int vci)  {

       if (vci%3 == 1 || vci%3 == 2)
         return vci%3;
       else
         return 3;

}



int vciToSW (int vci)  {

       return  1 + (vci-1)/3;

}


