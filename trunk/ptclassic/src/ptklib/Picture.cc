static const char file_id[] = "Picture.cc";
/* 
SCCS Version identification :
@(#)Picture.cc	1.1   06/25/97

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
						COPYRIGHTENDKEY

 Programmer: Luis Gutierrez
 (based on code for XYPlot written by Edward A. Lee and D. Niehaus)
 Created:

Defines and maintains a TK Picture.

*/
#ifdef __GNUG__
#pragma implementation
#endif

#include "InfString.h"
#include "Picture.h"
extern "C" {
#include "ptkPicture_defs.h"
}
#include "ieee.h"
#include "ptkTclCommands.h"

picObj::picObj(){
  const char * Picture_window_base = ".tkPictureBase";
  okFlag = 1;
  starID = Picture_window_base;
  starID += unique++;
// If it has not already happened, source the Tcl file corresponding
// to this class.  We test ptkInterp because some instances of this
// class may be created before the Tcl interpreter has started.
     if (ptkInterp) {
       InfString sourceCmd = "source $env(PTOLEMY)/src/ptklib/ptkPicture.tcl";
       if (Tcl_GlobalEval(ptkInterp, (char *)sourceCmd) != TCL_OK) {
	 InfString errorMsg = "PicObj constructor: \
                               failed to source ptkPicture.tcl\n\"";
	 errorMsg += ptkInterp->result;
	 errorMsg += "\"\n";
	 Error::abortRun((char *) errorMsg);
	 okFlag = 0;
       }
     }
     picture.name = NULL;
     picture.title = NULL;
     
};

int picObj::setup (Block* /*star*/,
		  char* title,
		  int height,
		  int width,
		  int palette,
		  u_char* buffer, 
		  int numinputs)
{
  if (!ptkInterp) return 0;
  if (!okFlag) return 0;


  // First check to see whether a global Tcl variable named
  // "ptkControlPanel" is defined.  If so, then use it as the
  // stem of the window name.  This means that the window will
  // be a child window of the control panel, and hence will be
  // destroyed when the control panel window is destroyed.
  char initControlPanelCmd[ sizeof(PTK_CONTROL_PANEL_INIT) ];
  strcpy(initControlPanelCmd, PTK_CONTROL_PANEL_INIT);
  if (Tcl_GlobalEval(ptkInterp, initControlPanelCmd) != TCL_OK) {
    winName = starID;
  } else {
    winName = ptkInterp->result;
    winName += starID;
  }

  if(ptkCreatePicture(ptkInterp, &ptkW, (char *)winName, "", title,
		      height, width, palette, 
		      numinputs, buffer, &picture) != 1){
    Error::abortRun(ptkPictureErrorMsg());
    okFlag = 0;
    return 0;
  }
  return 1;
}

picObj::~picObj(){
	// avoid core dump if interpreter did not set up right
  	if (!ptkInterp) return;
	if (!okFlag) return;
	ptkFreePicture(ptkInterp, &picture);
}

int picObj::updatePicture(void){
  if (!okFlag) return 0;
  return (ptkUpdatePicture(&picture));
};
  
int picObj::resizePicture(int height, int width, u_char* buffer){
  if (!okFlag) return 0;
  if (ptkResizePicture(ptkInterp, &picture, 
		      height, width, buffer) != 1){
    Error::abortRun(ptkPictureErrorMsg());
    okFlag = 0;
    return 0;
  }
  return 1;
}


// Initialize the static counter for unique names,
// and the hash table for keeping track of the data sets.
// The initialization occurs at load time.
int picObj::unique = 0;
  
	





