/*
 * C interface for creating a picture display utility using Tk
 *
 * Author: Luis Gutierrez (based on ptkPlot )
 *
 * @(#)ptkPicture.c	1.3 06/16/98
 */
/*
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
*/
#include "ptkPicture_defs.h"
#include <string.h>

/* Global scratch buffer used for constructing Tcl commands */
#define SCRATCH_STRING_SIZE 512
static char strTmp[SCRATCH_STRING_SIZE];
static char errorMsg[SCRATCH_STRING_SIZE];  

/* macro to create an error message */
#define ptkPicture_makeErrorMsg( msg ) \
strncpy(strTmp,(const char*)interp->result,(size_t)SCRATCH_STRING_SIZE); \
strncpy(errorMsg, msg , (size_t)SCRATCH_STRING_SIZE); \
strncat(errorMsg,(const char*)strTmp,((size_t)SCRATCH_STRING_SIZE \
 - strlen(errorMsg)));

char* ptkPictureErrorMsg(){
  return errorMsg;
}

int ptkFreePicture(interp, picture)
     Tcl_Interp* interp;
     Picture* picture;
{
  if (picture->name) {
    printf("deleted commands!\n");
    sprintf(strTmp,"scalex%s",picture->name);
    Tcl_DeleteCommand(interp, strTmp);
    sprintf(strTmp,"scaley%s",picture->name);
    Tcl_DeleteCommand(interp, strTmp);
  }
  return 1;
}

    
int ptkResizeCanvas(interp, picture, height, width)
     Tcl_Interp* interp;
     Picture* picture;
     int height;
     int width;
{
  sprintf(strTmp,"catch { %s.c configure -height %d -width %d }",
	  picture->name, height, width);
  if (Tcl_Eval(interp, strTmp) != TCL_OK) {
    ptkPicture_makeErrorMsg ("Error resizing canvas\n\
                              Tcl_Eval failed(ptkPicture.c):\n ")
    return 0;
  } else return 1;
}

int ptkResizePicture(interp, picture, height, width, buffer)
     Tcl_Interp* interp;
     Picture* picture;
     int height;
     int width;
     u_char* buffer;
{
  /* sets the size of the picture.  It DOES NOT resize the canvas
     (see ptkResizeCanvas)
  */
  picture->photoblock.height = height;
  picture->photoblock.width = width;
  picture->photoblock.pitch = width;
  picture->scaled_height = height;
  picture->scaled_width = width;
  picture->upsmplx = picture->upsmply = 1;
  picture->downsmply = picture->downsmply = 1;
  if (buffer) picture->photoblock.pixelPtr = buffer;
  return (ptkResizeCanvas(interp, picture, height, width));
}

int ptkUpdatePicture(Picture* picture)
{
  Tk_PhotoPutZoomedBlock(picture->photohandle, &(picture->photoblock), 0, 0,
			 picture->scaled_width, picture->scaled_height,
			 picture->upsmplx, picture->upsmply,
			 picture->downsmplx, picture->downsmply);
  /*
  Tk_PhotoPutBlock(picture->photohandle, &(picture->photoblock), 0, 0, 
		   picture->photoblock.width, picture->photoblock.height);
    FIX: the Tk_Photo... function returns void so for now 
    just return 1 every time 
   
  */
  return 1;
}

int ptkScaleY(clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char **argv;
{ 
  int factor;
  Picture* picture;
  picture = (Picture*) clientData;
  factor = atoi(argv[1]);
  if (factor > 0) {
    picture->scaled_height = factor*(picture->photoblock.height);
    picture->upsmply = factor;
    picture->downsmply = 1;
  } else if (factor < 0) {
    factor = -1*factor;
    picture->scaled_height = (int)((picture->photoblock.height)/factor);
    picture->upsmply = 1;
    picture->downsmply = factor;
  } else {
    picture->scaled_height = picture->photoblock.height;
    picture->upsmply = picture->downsmply = 1;
  }
  if (ptkResizeCanvas(interp, picture, 
		      picture->scaled_height,
		      picture->scaled_width) != 1) return TCL_ERROR;
  ptkUpdatePicture(picture);
  return TCL_OK;
}

int ptkScaleX(clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp *interp;
     int argc;
     char **argv;
{ 
  int factor;
  Picture* picture;
  picture = (Picture*) clientData;
  factor = atoi(argv[1]);
  if (factor > 0) {
    picture->scaled_width = factor*(picture->photoblock.width);
    picture->upsmplx = factor;
    picture->downsmplx = 1;
  } else if (factor < 0) {
    factor = -1*factor;
    picture->scaled_width = (int)((picture->photoblock.width)/factor);
    picture->upsmplx = 1;
    picture->downsmplx = factor;
  } else {
    picture->scaled_width = picture->photoblock.width;
    picture->upsmplx = picture->downsmplx = 1;
  }
  if (ptkResizeCanvas(interp, picture, 
		      picture->scaled_height,
		      picture->scaled_width) != 1) return TCL_ERROR;
  ptkUpdatePicture(picture);
  return TCL_OK;
}

int ptkCreatePicture(interp, win, name, geometry, title, height,
		     width,palette, numinputs, buffer, picture)
	Tcl_Interp* interp;
	Tk_Window* win;
	char* name; /* unique name for widget */
	char* geometry; /* window manager option */
	char* title; /* title for widget */
	int	height;
	int	width;
	int	palette;
	int     numinputs;
	u_char* buffer;
	Picture* picture;
{
  /* now create window; assumes ptkPicture.tcl has been sourced */
  sprintf(strTmp,
	  "catch { ptkCreatePicture %s %d %d %d \"%s\" \"%s\" [curuniverse] }",
	  name, width, height, palette, title, geometry);
  if (Tcl_GlobalEval(interp, strTmp) != TCL_OK) {
    ptkPicture_makeErrorMsg ("Error creating picture\n\
                              Tcl_GloablEval failed(ptkPicture.c):\n")
    return 0;
  } 

  /* fill in picture structure */
  sprintf(strTmp,"%s.pic",name);
#if TCL_MAJOR_VERSION < 8
  picture->photohandle = Tk_FindPhoto(strTmp);
#else
  picture->photohandle = Tk_FindPhoto(interp, strTmp);
#endif
  if (picture->photohandle == NULL) return TCL_ERROR;
  picture->photoblock.width = width;
  picture->photoblock.height = height;
  picture->photoblock.pitch = width;
  picture->photoblock.pixelSize = 3;
  picture->photoblock.offset[0] = 0;
  picture->photoblock.offset[1] = 1;
  picture->photoblock.offset[2] = 2;
  picture->photoblock.pixelPtr = buffer;
  picture->name = name;
  picture->title = title;
  picture->palette = palette;
  picture->scaled_height = height;
  picture->scaled_width = width;
  picture->downsmplx = picture->upsmplx = 1;
  picture->downsmply = picture->upsmply = 1;
  if (numinputs == 1){
    /* if it has only 1 input it's assumed the image is grayscale 
       default is color
    */
    picture->photoblock.pixelSize = 1;
    picture->photoblock.offset[0] = 0;
    picture->photoblock.offset[1] = 0;
    picture->photoblock.offset[2] = 0;
  }

  /* create commands that are supposed to be invoked from within the
     tcl script.  These include commands that are bound to events
     like resizing window etc. Currently no such commands are defined.
     To add command do this:

  sprintf(strTmp,"<command name here>%s",name);
  Tcl_CreateCommand(interp,strTmp,<C procedure name here>,
		    (ClientData)&picture,(Tcl_CmdDeleteProc *)NULL);

     And then define the C procedure as:

  int C_Proc(clientData, interp, argc, argv)
  ClientData clientData;
  Tcl_Interp *interp;
  int argc;
  char **argv;
  { };
  */

  /* create command to scale in x direction */
  sprintf(strTmp,"scalex%s",name);
  Tcl_CreateCommand(interp, strTmp, ptkScaleX,
		    (ClientData)picture, 
		    (Tcl_CmdDeleteProc *)NULL);
  /* create command to scale in y direction */
  sprintf(strTmp,"scaley%s",name);
  Tcl_CreateCommand(interp, strTmp, ptkScaleY,
		    (ClientData)picture, 
		    (Tcl_CmdDeleteProc *)NULL);

  return 1;
}  


