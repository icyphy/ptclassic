/* 
Copyright (c) 1993 The Regents of the University of California.
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
*/

/*
    ptkRegisterCommands.cc  aok
    Version: @(#)ptkRegisterCmds.cc	1.2 6/29/93
*/


/* 
    This function registers C functions used by Tcl with Tcl.  
    A new entry should be made into ptkRegisterCommands whenever
    a new Tcl callable function is added to the GUI.
*/

#include "ptk.h"
#include "ptcl.h"

extern "C" {
#include "ptkRegisterCmds.h"
}

/* global */ 
PTcl *ptcl;

void ptkRegisterCmds( Tcl_Interp *ip, Tk_Window appWin)
{

   // Construct the global PTcl object.  This should be done once only.
   ptcl = new PTcl(ip);

}
