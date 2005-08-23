static const char file_id[] = "ptkRegisterCommands.cc";

/* 
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
*/

/*
    ptkRegisterCommands.cc  aok
    Version: @(#)ptkRegisterCmds.cc	1.14	04/13/98
*/


/* 
    This function registers C functions used by Tcl with Tcl.  
    A new entry should be made into ptkRegisterCommands whenever
    a new Tcl callable function is added to the GUI.
*/


#include "PTcl.h"
extern PTcl *ptcl;  
#include "POct.h"
extern POct *poct;  
#include "PVem.h"
extern PVem *pvem;  

// Even though ptkRegisterCmds.cc is a C++ file,
// ptkRegisterCmds.h is a C include file.
extern "C" {
#include "ptkRegisterCmds.h"
#include "ptkTclIfc.h"

// We must include ptk.h last because ptk.h includes tk.h which 
// eventually includes X11/X.h, which on Solaris2.4 
// there is a #define Complex 0, which causes no end of trouble.
#include "ptk.h"
}


/* global */ 
PTcl *ptcl;
POct *poct;
PVem *pvem;

void ptkRegisterCmds( Tcl_Interp *ip)
{

   // Construct the global PTcl object.  This should be done once only.
   ptcl = new PTcl(ip);

   // Construct the global POct object.  This should be done once only.
   poct = new POct(ip);

   // Construct the global POct object.  This should be done once only.
   pvem = new PVem(ip);

   // Register various commands used by pigi
   registerTclFns(ip);
}
