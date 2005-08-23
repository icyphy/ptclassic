static const char file_id[] = "MatlabHook.cc";
 
/**************************************************************************
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
 
 Programmer:  Guy Maor
 Date of creation: 7/24/97
 Version: @(#)MatlabHook.cc	1.1	07/25/97
 
 Hook code for the Ptolemy interface to Matlab.
 
**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MatlabHook.h"
#include "MatlabIfcFuns.h"
#include "MatlabIfc.h"

MatlabHook MatlabHook::Me;

MatlabHook::MatlabHook ()
{
#include "MatlabHook.cc.Auto"
}
