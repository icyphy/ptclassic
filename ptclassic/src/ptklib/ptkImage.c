/*
Copyright (c) 1990-1995 The Regents of the University of California.
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

Version: $Id$

Author:  Mei Xiao (xmei@eecs.berkeley.edu)
         Department of Electrical Engineering & Computer Sciences
	 University of California at Berkeley

Implements images of type "ptimage" for Tk.  ptimage images
stored in 8 bits per pixel along with a well managed colormap for
PsedoColor visual class and in 24 bits per pixel for TrueColor
or DirectColor visual class.

This new image type "ptimage" borrows ideas from the prototype of
Tk's existing type "image", but has a very different core
implementation.   Tk is freely distributable software.  For
information about the usage and redistribution of Tk, see the
the file "license.terms" under the Tk distribution in $PTOLEMY/tcltk.
 */

#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "err.h"
#include "local.h"

/* We must include ptkImage.h last because it pulls in ptk.h.  See ptk.h. */
#include "ptkImage.h"

/* From Tk's generic/tkInt.h */
/*
#ifndef TkPutImage
extern void             TkPutImage _ANSI_ARGS_((unsigned long *colors,
                            int ncolors, Display* display, Drawable d,
                            GC gc, XImage* image, int src_x, int src_y,
                            int dest_x, int dest_y, unsigned int width,
                            unsigned int height));
#endif */

/* From Tk's generic/tkVisual.c */
/* Comment the following out until we upgrade to tk4.1 or higher
   extern void Tk_PreserveColormap _ANSI_ARGS_((Display *display,
					     Colormap colormap)); */

/* UCHAR is a conditional cast from integer to unsigned char in Tcl/Tk */
#ifndef UCHAR
#define UCHAR(c)	(c)
#endif

/* panic is a function in Tcl/Tk we redefine to use pigilib error reporting */
#define panic(msg)	ErrAdd(msg)

/*
 * Declaration for internal Xlib function used here:
 */

extern _XInitImageFuncPtrs _ANSI_ARGS_((XImage *image));

/*
 * A signed 8-bit integral type.  If chars are unsigned and the compiler
 * isn't an ANSI one, then we have to use short instead (which wastes
 * space) to get signed behavior.
 */

#if defined(__STDC__) || defined(_AIX)
    typedef signed char schar;
#else
#   ifndef __CHAR_UNSIGNED__
	typedef char schar;
#   else
	typedef short schar;
#   endif
#endif

/*
 * An unsigned 32-bit integral type, used for pixel values.
 * We use int rather than long here to accommodate those systems
 * where longs are 64 bits.
 */

typedef unsigned int pixel; 
  
/*
 * The type record for ptimage images:
 */

static int	    ImgPtimageCreate _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name, int argc, char **argv,
			    Tk_ImageType *typePtr, Tk_ImageMaster master,
			    ClientData *clientDataPtr));
static ClientData   ImgPtimageGet _ANSI_ARGS_((Tk_Window tkwin,
			    ClientData clientData));
static void	    ImgPtimageDisplay _ANSI_ARGS_((ClientData clientData,
			    Display *display, Drawable drawable,
			    int imageX, int imageY, int width, int height,
			    int drawableX, int drawableY));
static void	    ImgPtimageFree _ANSI_ARGS_((ClientData clientData,
			    Display *display));
static void	    ImgPtimageDelete _ANSI_ARGS_((ClientData clientData));

Tk_ImageType tkPtimageImageType = {
    "ptimage",		        /* name */
    ImgPtimageCreate,		/* createProc */
    ImgPtimageGet,		/* getProc */
    ImgPtimageDisplay,	        /* displayProc */
    ImgPtimageFree,		/* freeProc */
    ImgPtimageDelete,		/* deleteProc */
    (Tk_ImageType *) NULL	/* nextPtr */
};

/*
 * Default configuration
 */

#define DEF_PHOTO_GAMMA		"1"

/*
 * Information used for parsing configuration specifications:
 */
static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_STRING, "-data", (char *) NULL, (char *) NULL,
       (char *) NULL, Tk_Offset(PtimageMaster, dataString), TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-file", (char *) NULL, (char *) NULL,
       (char *) NULL, Tk_Offset(PtimageMaster, fileString), TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-format", (char *) NULL, (char *) NULL,
       (char *) NULL, Tk_Offset(PtimageMaster, format), TK_CONFIG_NULL_OK},
    {TK_CONFIG_DOUBLE, "-gamma", (char *) NULL, (char *) NULL,
       DEF_PHOTO_GAMMA, Tk_Offset(PtimageMaster, gamma), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
       (char *) NULL, 0, 0}
};

/*
 * Hash table used to provide access to ptimage images from C code.
 */

static Tcl_HashTable ptimageHash;
static int ptimageHashInitialized;	/* set when Tcl_InitHashTable done */

/*
 * Hash table used to hash from (display, colormap, gamma)
 * to ColorTable address.
 */

static Tcl_HashTable ptimageColorTabHash;
static int ptimageColorTabHashInitialized;
#define N_COLOR_HASH	(sizeof(ColorTableId) / sizeof(int))

/*
 * Pointer to the first in the list of known ptimage image formats.
 */

static Tk_PtimageImageFormat *formatList = NULL;

/*
 * Forward declarations
 */

static int	ImgPtimageCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
static void	ImgPtimageCmdDeletedProc _ANSI_ARGS_((
			    ClientData clientData));
static int	ImgPtimageConfigureMaster _ANSI_ARGS_((
			    Tcl_Interp *interp, PtimageMaster *masterPtr,
			    int argc, char **argv, int flags));
static void	ImgPtimageConfigureInstance _ANSI_ARGS_((
			    PtimageInstance *instancePtr));
static void	GetColorTable _ANSI_ARGS_((PtimageInstance *instancePtr));
static void	FreeColorTable _ANSI_ARGS_((ColorTable *colorPtr));
static void	AllocateColors _ANSI_ARGS_((ColorTable *colorPtr));
static void     UpdatePixmap _ANSI_ARGS_((PtimageInstance *instancePtr));
static void	DisposeColorTable _ANSI_ARGS_((ClientData clientData));
static void	DisposeInstance _ANSI_ARGS_((ClientData clientData));
static int	MatchFileFormat _ANSI_ARGS_((Tcl_Interp *interp,
			    FILE *f, char *fileName, char *formatString,
			    Tk_PtimageImageFormat **imageFormatPtr,
			    int *widthPtr, int *heightPtr));
static int	MatchStringFormat _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, char *formatString,
			    Tk_PtimageImageFormat **imageFormatPtr,
			    int *widthPtr, int *heightPtr));

static void     ZeroByte _ANSI_ARGS_((char *string, size_t length));
static void     safeBcopy _ANSI_ARGS_((char *src, char *dst, size_t len));

/*
 *--------------------------------------------------------------
 *
 * Additional definitions used to manage images of type "ptimage".
 *
 *--------------------------------------------------------------
 *
 * The following type is used to identify a particular ptimage image
 * to be manipulated:
 */

/* typedef void *Tk_PtimageHandle; */

/*
 * Procedure prototypes and structures used in reading and
 * writing ptimage images:
 */

/*
typedef struct Tk_PtimageImageFormat Tk_PtimageImageFormat;
typedef int (Tk_PtimageFileMatchProc) _ANSI_ARGS_((FILE *f, char *fileName,
        char *formatString, int *widthPtr, int *heightPtr));
typedef int (Tk_PtimageStringMatchProc) _ANSI_ARGS_((char *string,
        char *formatString, int *widthPtr, int *heightPtr));
typedef int (Tk_PtimageFileReadProc) _ANSI_ARGS_((Tcl_Interp *interp,
        FILE *f, char *fileName, char *formatString,
        Tk_PtimageHandle imageHandle));
typedef int (Tk_PtimageStringReadProc) _ANSI_ARGS_((Tcl_Interp *interp,
        char *string, char *formatString, Tk_PtimageHandle imageHandle));
typedef int (Tk_PtimageFileWriteProc) _ANSI_ARGS_((Tcl_Interp *interp,
        char *fileName, char *formatString, Tk_PtimageHandle imageHandle));
typedef int (Tk_PtimageStringWriteProc) _ANSI_ARGS_((Tcl_Interp *interp,
        Tcl_DString *dataPtr, char *formatString,
	Tk_PtimageHandle imageHandle)); */

/*
 * The following structure represents a particular file format for
 * storing images (e.g., PPM, GIF, JPEG, etc.).  It provides information
 * to allow image files of that format to be recognized and read into
 * a ptimage image or a ptimage image to be written into an image file
 * of that format.
 */
/*
struct Tk_PtimageImageFormat {
    char *name;                 * Name of image file format *
    Tk_PtimageFileMatchProc *fileMatchProc;
                                * Procedure to call to determine whether
                                 * an image file matches this format. *
    Tk_PtimageStringMatchProc *stringMatchProc;
                                * Procedure to call to determine whether
                                 * the data in a string matches this format. *
    Tk_PtimageFileReadProc *fileReadProc;
                                * Procedure to call to read data from
                                 * an image file into a ptimage image. *
    Tk_PtimageStringReadProc *stringReadProc;
                                * Procedure to call to read data from
                                 * a string into a ptimage image. *
    Tk_PtimageFileWriteProc *fileWriteProc;
                                * Procedure to call to write data from
                                 * a ptimage image to a file. *
    Tk_PtimageStringWriteProc *stringWriteProc;
                                * Procedure to call to obtain a string
                                 * representation of the data in a ptimage
                                 * image.*
    struct Tk_PtimageImageFormat *nextPtr;
                                * Next in list of all ptimage image formats
                                 * currently known.  Filled in by Tk, not
                                 * by image format handler. *
}; */

/*
 *----------------------------------------------------------------------
 *
 * Tk_CreatePtimageImageFormat --
 *
 *	This procedure is invoked by an image file handler to register
 *	a new ptimage image format and the procedures that handle the
 *	new format.  The procedure is typically invoked during
 *	Tcl_AppInit.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The new image file format is entered into a table used in the
 *	ptimage image "read" and "write" subcommands.
 *
 *----------------------------------------------------------------------
 */

void
Tk_CreatePtimageImageFormat(formatPtr)
    Tk_PtimageImageFormat *formatPtr;
				/* Structure describing the format. All of the
				 * fields except "nextPtr" must be filled in 
				 * by caller.  Must not have been passed to
				 * Tk_CreatePtimageImageFormat previously.  */
{
    Tk_PtimageImageFormat *copyPtr;

    copyPtr = (Tk_PtimageImageFormat *) ckalloc(sizeof(Tk_PtimageImageFormat));
    *copyPtr = *formatPtr;
    copyPtr->name = (char *) ckalloc((unsigned) (strlen(formatPtr->name) + 1));
    strcpy(copyPtr->name, formatPtr->name);
    copyPtr->nextPtr = formatList;
    formatList = copyPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * ImgPtimageCreate --
 *
 *	This procedure is called by the Tk image code to create
 *	a new ptimage image.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	The data structure for a new ptimage image is allocated and
 *	initialized.
 *
 *----------------------------------------------------------------------
 */

static int
ImgPtimageCreate(interp, name, argc, argv, typePtr, master, clientDataPtr)
    Tcl_Interp *interp;		/* Interpreter for application containing
				 * image. */
    char *name;			/* Name to use for image. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings for options (doesn't
				 * include image name or type). */
    Tk_ImageType *typePtr;	/* Pointer to our type record (not used). */
    Tk_ImageMaster master;	/* Token for image, to be used by us in
				 * later callbacks. */
    ClientData *clientDataPtr;	/* Store manager's token for image here;
				 * it will be returned in later callbacks. */
{
    PtimageMaster *masterPtr;
    Tcl_HashEntry *entry;
    int isNew;

    /*
     * Allocate and initialize the ptimage image master record.
     */

    masterPtr = (PtimageMaster *) ckalloc(sizeof(PtimageMaster));
    memset((void *) masterPtr, 0, sizeof(PtimageMaster));
    masterPtr->tkMaster = master;
    masterPtr->interp = interp;
    masterPtr->imageCmd = Tcl_CreateCommand(interp, name, ImgPtimageCmd,
	    (ClientData) masterPtr, ImgPtimageCmdDeletedProc);
    masterPtr->pix8  = NULL;
    masterPtr->pix24 = NULL;
    masterPtr->newPix8  = NULL;
    masterPtr->newPix24 = NULL;
    masterPtr->instancePtr = NULL;

    /*
     * Process configuration options given in the image create command.
     */

    if (ImgPtimageConfigureMaster(interp,masterPtr,argc,argv,0) != TCL_OK) {
	ImgPtimageDelete((ClientData) masterPtr);
	return TCL_ERROR;
    }

    /*
     * Enter this ptimage image in the hash table.
     */

    if (!ptimageHashInitialized) {
	Tcl_InitHashTable(&ptimageHash, TCL_STRING_KEYS);
	ptimageHashInitialized = 1;
    }
    entry = Tcl_CreateHashEntry(&ptimageHash, name, &isNew);
    Tcl_SetHashValue(entry, masterPtr);

    *clientDataPtr = (ClientData) masterPtr;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ImgPtimageCmd --
 *
 *	This procedure is invoked to process the Tcl command that
 *	corresponds to a ptimage image.  See the user documentation
 *	for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

static int
ImgPtimageCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about ptimage master. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    PtimageMaster *masterPtr = (PtimageMaster *) clientData;
    int c, result;
    int x, y, imageWidth, imageHeight;
    unsigned char *pixelPtr;
    char string[16];
    Tk_PtimageImageFormat *imageFormat;
    int matched;
    FILE *f;
    size_t length;
    Tcl_DString buffer;
    char *realFileName;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);

    /*
    if ((c == 'b') && (strncmp(argv[1], "blank", length) == 0)) {
	//
	// ptimage blank command - just call Tk_PtimageBlank.
	//

	if (argc == 2) {
	    Tk_PtimageBlank(masterPtr);
	} else {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " blank\"", (char *) NULL);
	    return TCL_ERROR;
	}
    } */
    if ((c == 'c') && (length >= 2)
	    && (strncmp(argv[1], "cget", length) == 0)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " cget option\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	result = Tk_ConfigureValue(interp, Tk_MainWindow(interp), configSpecs,
		(char *) masterPtr, argv[2], 0);
    } else if ((c == 'c') && (length >= 3)
	    && (strncmp(argv[1], "configure", length) == 0)) {
	/*
	 * ptimage configure command - handle this in the standard way.
	 */

	if (argc == 2) {
	    return Tk_ConfigureInfo(interp, Tk_MainWindow(interp),
		    configSpecs, (char *) masterPtr, (char *) NULL, 0);
	}
	if (argc == 3) {
	    return Tk_ConfigureInfo(interp, Tk_MainWindow(interp),
		    configSpecs, (char *) masterPtr, argv[2], 0);
	}
	return ImgPtimageConfigureMaster(interp, masterPtr, argc-2, argv+2,
		TK_CONFIG_ARGV_ONLY);
    } else if ((c == 'g') && (strncmp(argv[1], "get", length) == 0)) {
	/*
	 * ptimage get command - first parse and check parameters.
	 */

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " get x y\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK)
		|| (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
	    return TCL_ERROR;
	}
	if ((x < 0) || (x >= masterPtr->width)
		|| (y < 0) || (y >= masterPtr->height)) {
	    Tcl_AppendResult(interp, argv[0], " get: ",
		    "coordinates out of range", (char *) NULL);
	    return TCL_ERROR;
	}

	/*
	 * Extract the value of the desired pixel and format it as a string.
	 */

	pixelPtr = masterPtr->pix24 + (y * masterPtr->width + x) * 3;
	sprintf(string, "%d %d %d", pixelPtr[0], pixelPtr[1],
		pixelPtr[2]);
	Tcl_AppendResult(interp, string, (char *) NULL);
    } else if ((c == 'r') && (strncmp(argv[1], "read", length) == 0)) {
        /*
	 * ptimage read command - 
	 */

        realFileName = Tcl_TildeSubst(interp, argv[2], &buffer);
	if (realFileName == NULL) {
	   return TCL_ERROR;
        }
	f = fopen(realFileName, "rb");
	Tcl_DStringFree(&buffer);
	if (f == NULL) {
	   Tcl_AppendResult(interp, "Could not read image file \"",
		  "\": ", Tcl_PosixError(interp), (char *) NULL);
           return TCL_ERROR;
        }
	if (MatchFileFormat(interp, f, argv[2], (char *) NULL,
		&imageFormat, &imageWidth, &imageHeight) != TCL_OK) {
           fclose(f);
	   return TCL_ERROR;
        }

	/*
	 * Call the handler's file read procedure to read the data 
	 * into the image.
	 */

        result = (*imageFormat->fileReadProc)(interp, f, argv[2],
		    (char *) NULL, (Tk_PtimageHandle) masterPtr);
        if (f != NULL) {
	   fclose(f);
        }
	return result;
    } else if ((c == 'w') && (strncmp(argv[1], "write", length) == 0)) {
	/* 
	 * ptimage write command -
	 */

        /* 
	 * Search for an appropriate image file format handler,
	 * and give an error if none is found.
	 */

        matched = 0;
	for (imageFormat = formatList; imageFormat != NULL;
	     imageFormat = imageFormat->nextPtr) {
            matched = 1;
	    if (imageFormat->fileWriteProc != NULL) {
	       break;
            }
        }
	if (imageFormat == NULL) {
	   Tcl_AppendResult(interp, "no available image file format ",
		  "has file writing capability", (char *) NULL);
           return TCL_ERROR;
        }

	/* 
	 * Call the handler's file write procedure to write out 
	 * the image
	 */

        return (*imageFormat->fileWriteProc)(interp, argv[2],
		  (char *) NULL, (Tk_PtimageHandle) masterPtr);
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
	 "\": must be cget, configure, or get", (char *) NULL);
	return TCL_ERROR;
    }

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ImgPtimageConfigureMaster --
 *
 *	This procedure is called when a ptimage image is created or
 *	reconfigured.  It processes configuration options and resets
 *	any instances of the image.
 *
 * Results:
 *	A standard Tcl return value.  If TCL_ERROR is returned then
 *	an error message is left in masterPtr->interp->result.
 *
 * Side effects:
 *	Existing instances of the image will be redisplayed to match
 *	the new configuration options.
 *
 *----------------------------------------------------------------------
 */

static int
ImgPtimageConfigureMaster(interp, masterPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Interpreter to use for reporting errors. */
    PtimageMaster *masterPtr;	/* Pointer to data structure describing
				 * overall ptimage image to (re)configure. */
    int argc;			/* Number of entries in argv. */
    char **argv;		/* Pairs of configuration options for image. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget,
				 * such as TK_CONFIG_ARGV_ONLY. */
{
    PtimageInstance *instancePtr;
    char *oldFileString, *oldDataString, *realFileName;
    double oldGamma;
    int result;
    FILE *f;
    Tk_PtimageImageFormat *imageFormat;
    int imageWidth, imageHeight;
    Tcl_DString buffer;

    /*
     * Save the current values for fileString and dataString, so we
     * can tell if the user specifies them anew.
     */

    oldFileString = masterPtr->fileString;
    oldDataString = (oldFileString == NULL)? masterPtr->dataString: NULL;
    oldGamma = masterPtr->gamma;

    /*
     * Process the configuration options specified.
     */

    if (Tk_ConfigureWidget(interp, Tk_MainWindow(interp), configSpecs,
	    argc, argv, (char *) masterPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * Regard the empty string for -file, -data or -format as the null
     * value.
     */

    if ((masterPtr->fileString != NULL) && (masterPtr->fileString[0] == 0)) {
	ckfree(masterPtr->fileString);
	masterPtr->fileString = NULL;
    }
    if ((masterPtr->dataString != NULL) && (masterPtr->dataString[0] == 0)) {
	ckfree(masterPtr->dataString);
	masterPtr->dataString = NULL;
    }
    if ((masterPtr->format != NULL) && (masterPtr->format[0] == 0)) {
	ckfree(masterPtr->format);
	masterPtr->format = NULL;
    }

    /*
     * Set the image to the user-requested size, if any,
     * and make sure storage is correctly allocated for this image.
     */

    /*
     * Read in the image from the file or string if the user has
     * specified the -file or -data option.
     */

    if ((masterPtr->fileString != NULL)
	    && (masterPtr->fileString != oldFileString)) {

	realFileName = Tcl_TildeSubst(interp, masterPtr->fileString, &buffer);
	if (realFileName == NULL) {
	    return TCL_ERROR;
	}
	f = fopen(realFileName, "rb");
	Tcl_DStringFree(&buffer);
	if (f == NULL) {
	    Tcl_AppendResult(interp, "couldn't read image file \"",
		    masterPtr->fileString, "\": ", Tcl_PosixError(interp),
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (MatchFileFormat(interp, f, masterPtr->fileString,
		masterPtr->format, &imageFormat, &imageWidth,
                &imageHeight) != TCL_OK) {
	    fclose(f);
	    return TCL_ERROR;
	}
        Tk_PtimageSetSize(masterPtr, imageWidth, imageHeight);
	result = (*imageFormat->fileReadProc)(interp, f, masterPtr->fileString,
		masterPtr->format, (Tk_PtimageHandle) masterPtr);
	fclose(f);
	if (result != TCL_OK) {
	    return TCL_ERROR;
	}

	masterPtr->flags |= IMAGE_CHANGED;
    }

    if ((masterPtr->fileString == NULL) && (masterPtr->dataString != NULL)
	    && (masterPtr->dataString != oldDataString)) {

	if (MatchStringFormat(interp, masterPtr->dataString, 
		masterPtr->format, &imageFormat, &imageWidth,
		&imageHeight) != TCL_OK) {
	    return TCL_ERROR;
	}
	Tk_PtimageSetSize(masterPtr, imageWidth, imageHeight);
	if ((*imageFormat->stringReadProc)(interp, masterPtr->dataString,
		masterPtr->format, (Tk_PtimageHandle) masterPtr) != TCL_OK) {
	    return TCL_ERROR;
	}

	masterPtr->flags |= IMAGE_CHANGED;
    }

    /*
     * Enforce a reasonable value for gamma.
     */

    if (masterPtr->gamma <= 0) {
	masterPtr->gamma = 1.0;
    }

    if (masterPtr->gamma != oldGamma) {
	masterPtr->flags |= IMAGE_CHANGED;
    }

    /*
     * Cycle through all of the instances of this image, regenerating
     * the information for each instance.  Then force the image to be
     * redisplayed everywhere that it is used.
     */

    for (instancePtr = masterPtr->instancePtr; instancePtr != NULL;
	    instancePtr = instancePtr->nextPtr) {
	ImgPtimageConfigureInstance(instancePtr);
    }

    /*
     * Inform the generic image code that the image
     * has (potentially) changed.
     */

    Tk_ImageChanged(masterPtr->tkMaster, 0, 0, masterPtr->width,
	    masterPtr->height, masterPtr->width, masterPtr->height);
    masterPtr->flags &= ~IMAGE_CHANGED;

    return TCL_OK;
}
 
/*
 *----------------------------------------------------------------------
 *
 * ImgPtimageConfigureInstance --
 *
 *	This procedure is called to create displaying information for
 *	a ptimage image instance based on the configuration information
 *	in the master.  It is invoked both when new instances are
 *	created and when the master is reconfigured.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Generates errors via Tk_BackgroundError if there are problems
 *	in setting up the instance.
 *
 *----------------------------------------------------------------------
 */

static void
ImgPtimageConfigureInstance(instancePtr)
    PtimageInstance *instancePtr;	/* Instance to reconfigure. */
{
    PtimageMaster *masterPtr = instancePtr->masterPtr;
    XImage *imagePtr;
    int bitsPerPixel;
    ColorTable *colorTablePtr;
    int width, height, imW, nullCount;
    byte *imdata;

    /* Change the dimension of the instance if needed */
    instancePtr->width = masterPtr->width;
    instancePtr->height = masterPtr->height;

    /* Use the gamma value specified for the master */

    instancePtr->gamma = masterPtr->gamma;

    /*
     * If we don't currently have a color table, or if the one we
     * have no longer applies (e.g. because our gamma has changed,
     * or the pixel values of the image get changed),get a new one.
     */

    colorTablePtr = instancePtr->colorTablePtr;
    if ((colorTablePtr == NULL)
	    || (instancePtr->colormap != colorTablePtr->id.colormap)
	    || (instancePtr->masterPtr->flags & IMAGE_CHANGED)) {
	/*
	 * Free up our old color table, and get a new one.
	 */

	if (colorTablePtr != NULL) {
	    colorTablePtr->liveRefCount -= 1;
	    FreeColorTable(colorTablePtr);
	}
	/* my GetColorTable() really consists of my own AllocColors(), which
	 * does things after ImgPtimageConfigureMaster() has made
	 * Tk_PtSortColormap() and Tk_PTColorCompress8(); in addition,
	 * FreeColors gets recorded where photo's pixelMap is
	 */

	GetColorTable(instancePtr);

	/*
	 * Create a new XImage structure for sending data to
	 * the X server, if necessary.
	 */

	if (instancePtr->colorTablePtr->flags & BLACK_AND_WHITE) {
	    bitsPerPixel = 1;
	} else {
	    bitsPerPixel = instancePtr->visualInfo.depth;
	} 

        width = instancePtr->width;
        height = instancePtr->height;

	if ((instancePtr->imagePtr == NULL)
		|| (instancePtr->masterPtr->flags & IMAGE_CHANGED)
		|| (instancePtr->imagePtr->bits_per_pixel != bitsPerPixel)) {
	    if (instancePtr->imagePtr != NULL) {
		XFree((char *) instancePtr->imagePtr);
	    }

	    nullCount = (4 - (width % 4)) & 0x03;
				             /* # of padding bytes per line */
	    imW = width + nullCount;
	    imdata = (byte *) ckalloc((unsigned) (imW * height));
	    if (!imdata) panic("Could not ckalloc image data.");
	    imagePtr = XCreateImage(instancePtr->display,
	             instancePtr->visualInfo.visual, (unsigned) bitsPerPixel,
		     (bitsPerPixel > 1? ZPixmap: XYBitmap), 0, (char *) imdata,
		     width, height, 32, imW);

	    instancePtr->imagePtr = imagePtr;

	    /*
	     * Determine the endianness of this machine.
	     * We create images using the local host's endianness, rather
	     * than the endianness of the server; otherwise we would have
	     * to byte-swap any 16 or 32 bit values that we store in the
	     * image in those situations where the server's endianness
	     * is different from ours.
	     */

	    if (imagePtr != NULL) {
		union {
		    int i;
		    char c[sizeof(int)];
		} kludge;

		imagePtr->bitmap_unit = sizeof(pixel) * NBBY;
		kludge.i = 0;
		kludge.c[0] = 1;
		imagePtr->byte_order = (kludge.i == 1) ? LSBFirst : MSBFirst;
		_XInitImageFuncPtrs(imagePtr);
	    }
	} 
    }

    /*
     * If the user has specified a width and/or height for the master
     * which is different from our current width/height, set the size
     * to the values specified by the user.  If we have no pixmap, we
     * do this also, since it has the side effect of allocating a
     * pixmap for us.
     */

    /* Update the pixmap for this instance with the block of pixels  */

    UpdatePixmap(instancePtr);

}

/*
 *----------------------------------------------------------------------
 * UpdatePixmap --
 *----------------------------------------------------------------------
 */

static void 
UpdatePixmap(instancePtr)
    PtimageInstance *instancePtr;   /* instance whose pixmap gets updated */
{
    int  i, j, width, height, imW, nullCount;
    byte *imdata, *imptr, *picptr;
    XImage *ximage;
    ColorTable *colorPtr;
    Pixmap newPixmap;

    width = instancePtr->width;
    height = instancePtr->height;

    nullCount = (4 - (width % 4)) & 0x03; /* # of padding bytes per line */
    imW = width + nullCount; 

    ximage = instancePtr->imagePtr;
    imdata = ximage->data;

    colorPtr = instancePtr->colorTablePtr;
    picptr = instancePtr->masterPtr->newPix8;
    for (i = 0, imptr = imdata; i < height; i++) {
        for (j = 0; j < width; j++) 
            *imptr++ = (byte) colorPtr->xPixVals[*picptr++];
        for (j = 0; j < nullCount; j++) *imptr++ = 0;
    }

    if (instancePtr->pixmap != None) 
	Tk_FreePixmap(instancePtr->display, instancePtr->pixmap);
    newPixmap = Tk_GetPixmap(instancePtr->display, 
	                     RootWindow(instancePtr->display,
			                instancePtr->visualInfo.screen),
	                     width, height, instancePtr->visualInfo.depth);
    instancePtr->pixmap = newPixmap;

    /* Update the pixmap for this instance */
    /* The following XPutImage() call should really be TkPutImage(),
     * in order to port the stuff onto mac and pc. We won't change
     * it to TkPutImage() until Tk gets upgraded to 4.1 or higher.  
     */

    /*
    TkPutImage(colorPtr->freeColors, colorPtr->nFreeColors,
               instancePtr->display, instancePtr->pixmap,
	       instancePtr->gc, ximage, 0, 0, 0, 0,
	       (unsigned) width, (unsigned) height); */

    XPutImage(instancePtr->display, instancePtr->pixmap,
	       instancePtr->gc, ximage, 0, 0, 0, 0,
	       (unsigned) width, (unsigned) height);
} 

/*
 *----------------------------------------------------------------------
 *
 * ImgPtimageGet --
 *
 *	This procedure is called for each use of a ptimage image in a
 *	widget.
 *
 * Results:
 *	The return value is a token for the instance, which is passed
 *	back to us in calls to ImgPtimageDisplay and ImgPtimageFree.
 *
 * Side effects:
 *	A data structure is set up for the instance (or, an existing
 *	instance is re-used for the new one).
 *
 *----------------------------------------------------------------------
 */

static ClientData
ImgPtimageGet(tkwin, masterData)
    Tk_Window tkwin;		/* Window in which the instance will be
				 * used. */
    ClientData masterData;	/* Pointer to our master structure for the
				 * image. */
{
    PtimageMaster *masterPtr = (PtimageMaster *) masterData;
    PtimageInstance *instancePtr;
    Colormap colormap;
    XVisualInfo visualInfo, *visInfoPtr;
    int numVisuals;
    XColor *white, *black;
    XGCValues gcValues;

    /*
     * See if there is already an instance for windows using
     * the same colormap.  If so then just re-use it.
     */

    colormap = Tk_Colormap(tkwin);
    for (instancePtr = masterPtr->instancePtr; instancePtr != NULL;
	    instancePtr = instancePtr->nextPtr) {
	if ((colormap == instancePtr->colormap)
		&& (Tk_Display(tkwin) == instancePtr->display)) {

	    /*
	     * Re-use this instance.
	     */

	    if (instancePtr->refCount == 0) {
		/*
		 * We are resurrecting this instance.
		 */

		Tk_CancelIdleCall(DisposeInstance, (ClientData) instancePtr);
		if (instancePtr->colorTablePtr != NULL) {
		    FreeColorTable(instancePtr->colorTablePtr);
		}
		/* My GetColorTable function call really consists of 
		 * my own AllocColors(), which does things after 
		 * ImgPtimageConfigureMaster() has made Tk_PtSortColormap() and
		 * Tk_PtColorCompress8(); also, an array of FreeColors gets
		 * recorded where photo's pixelMap is. My ColorTable looks
		 * much conciser than that of photo's.
		 */

		GetColorTable(instancePtr);
                
	    }
	    instancePtr->refCount++;
	    return (ClientData) instancePtr;
	}
    }

    /*
     * The image isn't already in use in a window with the same colormap.
     * Make a new instance of the image.
     */

    instancePtr = (PtimageInstance *) ckalloc(sizeof(PtimageInstance));
    if (!instancePtr) panic("Could not alloc for PtimageInstance.");
    instancePtr->masterPtr = masterPtr;
    instancePtr->display = Tk_Display(tkwin);
    instancePtr->colormap = Tk_Colormap(tkwin);
    /* Comment this out until we upgrade to tk4.1 or higher 
    Tk_PreserveColormap(instancePtr->display, instancePtr->colormap); */
    instancePtr->refCount = 1;
    instancePtr->width = masterPtr->width;
    instancePtr->height = masterPtr->height;
    instancePtr->colorTablePtr = NULL;
    instancePtr->pixmap = None;
    instancePtr->imagePtr = 0;
    instancePtr->nextPtr = masterPtr->instancePtr;
    masterPtr->instancePtr = instancePtr;

    /* Obtain information about the visual */

    visualInfo.screen = Tk_ScreenNumber(tkwin);
    visualInfo.visualid = XVisualIDFromVisual(Tk_Visual(tkwin));
    visInfoPtr = XGetVisualInfo(Tk_Display(tkwin),
	    VisualScreenMask | VisualIDMask, &visualInfo, &numVisuals);

    if (visInfoPtr != NULL) {
	instancePtr->visualInfo = *visInfoPtr;
	switch (visInfoPtr->class) {
	    case DirectColor:
	    case TrueColor:
	    case PseudoColor:
	    case StaticColor:
	    case GrayScale:
	    case StaticGray:
		break;
	}
	XFree((char *) visInfoPtr);

    } else {
	panic("ImgPtimageGet couldn't find visual for window");
    }

    /*
    sprintf(buf, ((mono) ? "%d": "%d/%d/%d"), nRed, nGreen, nBlue);
    instancePtr->defaultPalette = Tk_GetUid(buf);
    */

    /*
     * Make a GC with background = black and foreground = white.
     */

    white = Tk_GetColor(masterPtr->interp, tkwin, "white");
    black = Tk_GetColor(masterPtr->interp, tkwin, "black");
    gcValues.foreground = (white != NULL)? white->pixel:
	    WhitePixelOfScreen(Tk_Screen(tkwin));
    gcValues.background = (black != NULL)? black->pixel:
	    BlackPixelOfScreen(Tk_Screen(tkwin));
    gcValues.graphics_exposures = False;
    instancePtr->gc = Tk_GetGC(tkwin,
	    GCForeground|GCBackground|GCGraphicsExposures, &gcValues);

    /*
     * Set configuration options and finish the initialization of the instance.
     */

    ImgPtimageConfigureInstance(instancePtr);

    /*
     * If this is the first instance, must set the size of the image.
     */

    if (instancePtr->nextPtr == NULL) {
	Tk_ImageChanged(masterPtr->tkMaster, 0, 0, 0, 0,
		masterPtr->width, masterPtr->height);
    }

    return (ClientData) instancePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * ImgPtimageDisplay --
 *
 *	This procedure is invoked to draw a ptimage image.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A portion of the image gets rendered in a pixmap or window.
 *
 *----------------------------------------------------------------------
 */

static void
ImgPtimageDisplay(clientData, display, drawable, imageX, imageY, width,
	height, drawableX, drawableY)
    ClientData clientData;	/* Pointer to PtimageInstance structure for
				 * for instance to be displayed. */
    Display *display;		/* Display on which to draw image. */
    Drawable drawable;		/* Pixmap or window in which to draw image. */
    int imageX, imageY;		/* Upper-left corner of region within image
				 * to draw. */
    int width, height;		/* Dimensions of region within image to draw. */
    int drawableX, drawableY;	/* Coordinates within drawable that
				 * correspond to imageX and imageY. */
{
    PtimageInstance *instancePtr = (PtimageInstance *) clientData;

    /*
     * If there's no pixmap, it means that an error occurred
     * while creating the image instance so it can't be displayed.
     */

    if (instancePtr->pixmap == None) {
     
	return;
    }

    /*
     * masterPtr->region describes which parts of the image contain
     * valid data.  We set this region as the clip mask for the gc,
     * setting its origin appropriately, and use it when drawing the
     * image.
     */

    XSetClipOrigin(display, instancePtr->gc, drawableX - imageX,
	    drawableY - imageY);
    XSetClipMask(display, instancePtr->gc, None);
    XCopyArea(display, instancePtr->pixmap, drawable, instancePtr->gc,
	    imageX, imageY, (unsigned) width, (unsigned) height,
	    drawableX, drawableY);
    XSetClipOrigin(display, instancePtr->gc, 0, 0);
}

/*
 *----------------------------------------------------------------------
 *
 * ImgPtimgeFree --
 *
 *	This procedure is called when a widget ceases to use a
 *	particular instance of an image.  We don't actually get
 *	rid of the instance until later because we may be about
 *	to get this instance again.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Internal data structures get cleaned up, later.
 *
 *----------------------------------------------------------------------
 */

static void
ImgPtimageFree(clientData, display)
    ClientData clientData;	/* Pointer to PtimageInstance structure for
				 * for instance to be displayed. */
    Display *display;		/* Display containing window that used image. */
{
    PtimageInstance *instancePtr = (PtimageInstance *) clientData;
    ColorTable *colorPtr;

    instancePtr->refCount -= 1;
    if (instancePtr->refCount > 0) {
	return;
    }

    /*
     * There are no more uses of the image within this widget.
     * Decrement the count of live uses of its color table, so
     * that its colors can be reclaimed if necessary, and
     * set up an idle call to free the instance structure.
     */

    colorPtr = instancePtr->colorTablePtr;
    if (colorPtr != NULL) {
	colorPtr->liveRefCount -= 1;
    }
    
    Tk_DoWhenIdle(DisposeInstance, (ClientData) instancePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ImgPtimageDelete --
 *
 *	This procedure is called by the image code to delete the
 *	master structure for an image.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Resources associated with the image get freed.
 *
 *----------------------------------------------------------------------
 */

static void
ImgPtimageDelete(masterData)
    ClientData masterData;	/* Pointer to PtimageMaster structure for
				 * image.  Must not have any more instances. */
{
    PtimageMaster *masterPtr = (PtimageMaster *) masterData;
    PtimageInstance *instancePtr;

    while ((instancePtr = masterPtr->instancePtr) != NULL) {
	if (instancePtr->refCount > 0) {
	    panic("tried to delete ptimage image when instances still exist");
	}
	Tk_CancelIdleCall(DisposeInstance, (ClientData) instancePtr);
	DisposeInstance((ClientData) instancePtr);
    }
    masterPtr->tkMaster = NULL;
    if (masterPtr->imageCmd != NULL) {
	Tcl_DeleteCommand(masterPtr->interp,
		Tcl_GetCommandName(masterPtr->interp, masterPtr->imageCmd));
    }
    if (masterPtr->pix8 != NULL) {
       ckfree((unsigned char *) masterPtr->pix8);
    }
    if (masterPtr->pix24 != NULL) {
	ckfree((unsigned char *) masterPtr->pix24);
    }
    if (masterPtr->newPix8 != NULL) {
        ckfree((unsigned char *) masterPtr->newPix8);
    }
    if (masterPtr->newPix24 != NULL) {
        ckfree((unsigned char *) masterPtr->newPix24);
    }
    Tk_FreeOptions(configSpecs, (char *) masterPtr, (Display *) NULL, 0);
    ckfree((char *) masterPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ImgPtimageCmdDeletedProc --
 *
 *	This procedure is invoked when the image command for an image
 *	is deleted.  It deletes the image.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The image is deleted.
 *
 *----------------------------------------------------------------------
 */

static void
ImgPtimageCmdDeletedProc(clientData)
    ClientData clientData;	/* Pointer to PtimageMaster structure for
				 * image. */
{
    PtimageMaster *masterPtr = (PtimageMaster *) clientData;

    masterPtr->imageCmd = NULL;
    if (masterPtr->tkMaster != NULL) {
	Tk_DeleteImage(masterPtr->interp, Tk_NameOfImage(masterPtr->tkMaster));
    }
}

/*
 *----------------------------------------------------------------------
 *
 * GetColorTable --
 *
 *	This procedure is called to allocate a table of colormap
 *	information for an instance of a ptimage image.  Only one such
 *	table is allocated for all ptimage instances using the same
 *	display, colormap (which really is the system default colormap),
 *      gamma values, and pixel values (for a specific ptimage image, if
 *	its configuration does "-file fname" later, then the pixel values
 *      might change), so that the application need only request a set
 *      of colors from the X server once for all such widgets that use
 *	this ptimage image. 
 * 	This procedure maintains a hash table to find previously-allocated
 *	ColorTables.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A new ColorTable may be allocated and placed in the hash
 *	table, and have colors allocated for it.
 *
 *----------------------------------------------------------------------
 */

static void
GetColorTable(instancePtr)
    PtimageInstance *instancePtr;	/* Instance needing a color table. */
{
    ColorTable *colorPtr;
    Tcl_HashEntry *entry;
    ColorTableId id; 
    int isNew;

    /*
     * Look for an existing ColorTable in the hash table.
     */

    memset((VOID *) &id, 0, sizeof(id));
    id.display = instancePtr->display;
    id.colormap = instancePtr->colormap;
    id.masterPtr = instancePtr->masterPtr;
    if (!ptimageColorTabHashInitialized) {
	Tcl_InitHashTable(&ptimageColorTabHash, N_COLOR_HASH);
	ptimageColorTabHashInitialized = 1;
    }
    entry = Tcl_CreateHashEntry(&ptimageColorTabHash, (char *) &id, &isNew);

    if (!isNew) {
	/*
	 * Re-use the existing entry.
	 */

	colorPtr = (ColorTable *) Tcl_GetHashValue(entry);

    } else {
	/*
	 * No color table currently available; need to make one.
	 */

	colorPtr = (ColorTable *) ckalloc(sizeof(ColorTable));
	if (!colorPtr) panic("Could not ckalloc colorTable in GetColorTable.");  
	/*
	 * The following line of code should not normally be needed due
	 * to the assignment in the following line.  However, it compensates
	 * for bugs in some compilers (HP, for example) where
	 * sizeof(ColorTable) is say, 24, but the assignment only copies 20
         * bytes, leaving 4 bytes uninitialized;  these cause problems when
         * using the id for lookups in imgPhotoColorHash, and can result in
	 * core dumps.
	 */

	memset((VOID *) &colorPtr->id, 0, sizeof(ColorTableId));
	colorPtr->id = id;
	/* Comment this out until we upgrade to tk4.1 or higher
	Tk_PreserveColormap(colorPtr->id.display, colorPtr->id.colormap); */
	colorPtr->flags = 0;
	colorPtr->refCount = 0;
        colorPtr->liveRefCount = 0;
	colorPtr->visualInfo = instancePtr->visualInfo;
	colorPtr->freeColors = NULL;
	colorPtr->nFreeColors = 0;
	colorPtr->xPixVals = NULL;
	Tcl_SetHashValue(entry, colorPtr);
    }

    colorPtr->refCount++;
    colorPtr->liveRefCount++;
    instancePtr->colorTablePtr = colorPtr;
    if (colorPtr->flags & DISPOSE_PENDING) {
	Tk_CancelIdleCall(DisposeColorTable, (ClientData) colorPtr);
	colorPtr->flags &= ~DISPOSE_PENDING;
    }

    /*
     * Allocate colors for this color table if necessary.
     */

    if ((colorPtr->nFreeColors == 0)
	    && ((colorPtr->flags & BLACK_AND_WHITE) == 0)) {
	AllocateColors(colorPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * FreeColorTable --
 *
 *	This procedure is called when an instance ceases using a
 *	color table.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If no other instances are using this color table, a when-idle
 *	handler is registered to free up the color table and the colors
 *	allocated for it.
 *
 *----------------------------------------------------------------------
 */

static void
FreeColorTable(colorPtr)
    ColorTable *colorPtr;	/* Pointer to the color table which is
				 * no longer required by an instance. */
{
    colorPtr->refCount--;
    if (colorPtr->refCount > 0) {
	return;
    }
    if ((colorPtr->flags & DISPOSE_PENDING) == 0) {
	Tk_DoWhenIdle(DisposeColorTable, (ClientData) colorPtr);
	colorPtr->flags |= DISPOSE_PENDING;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * AllocateColors --
 *
 *	This procedure allocates the colors required by a color table,
 *	and sets up the fields in the color table data structure which
 *	are used in dithering.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Colors are allocated from the X server.  Fields in the
 *	color table data structure are updated.
 *
 *----------------------------------------------------------------------
 */

static void
AllocateColors(colorPtr)
    ColorTable *colorPtr;	/* Pointer to the color table requiring
				 * colors to be allocated. */
{
    Display *display;
    Colormap colormap;
    PtimageMaster *masterPtr;
    double igam;

    /* 16-bit intensity value for i/n of full intensity. */
#   define CFRAC(i, n)	((i) * 65535 / (n))
    /* As for CFRAC, but apply exponent of g. */
#   define CGFRAC(i, n, g)	((int)(65535 * pow((double)(i) / (n), (g))))

    display   = colorPtr->id.display;
    colormap  = colorPtr->id.colormap;
    masterPtr = colorPtr->id.masterPtr;
    igam = 1.0 / masterPtr->gamma;

    /* Calculate the RGB coordinates of the colors we want to
     * allocate and store them in *colors.  */

    if ((colorPtr->visualInfo.class == DirectColor)
	    || (colorPtr->visualInfo.class == TrueColor)) {
        /* Direct/True Color: allocate shades of red, green, blue
	 * independently.  */
    }
    else {

      /* PseudoColor, StaticColor, GrayScale or StaticGray visual:
       * we have to allocate each color in the color cube separately.  */

      /* My stuff goes here. It is essentially my allocROColors() for the
       * stand-alone display test program */  

      unsigned long *xPixVals;
      unsigned long *freeColors;
      int nFreeColors = 0;
      int *FC2PColor;
      byte *rMap, *gMap, *bMap;
      XColor   defs[256], xcolorTab[256];
      int      failed[256];
      int      i, j, c, maxColors = 256;  

      int unique = 0;/* # of different colors that get allocated successfully*/

      int dispCells = 256; /* # of cells in default colormap of screen  */

      unsigned int length = 256; 

      xPixVals = (unsigned long *) ckalloc(length * sizeof(unsigned long));
      if (!xPixVals) panic("Could not alloc xPixVals in AllocateColors.");

      freeColors = (unsigned long *) ckalloc(length * sizeof(unsigned long));
      if (!freeColors) panic("Could not alloc freeColors in AllocateColors.");

      FC2PColor = (int *) ckalloc(length * sizeof(int));
      if (!FC2PColor) panic("Could not alloc FC2PColor in AllocateColors.");

      colorPtr->xPixVals = xPixVals;
      colorPtr->freeColors = freeColors;
      colorPtr->FC2PColor = FC2PColor;
      rMap = colorPtr->id.masterPtr->rMap;
      gMap = colorPtr->id.masterPtr->gMap;
      bMap = colorPtr->id.masterPtr->bMap;

      /* FIRST PASS COLOR ALLOCATION:
       * for each color in the 'desired colormap', try to get it via
       * XAllocColor().  If for any reason it fails, mark that pixel
       * 'unallocated' and worry about it later.  Repeat.                    */

      /* attempt to allocate first 'maxColors' (256 for PseudoColor) entries
       * in the post-compress desired colormap note: On displays with less
       * than 8 bits per RGB gun, it's quite possible that different colors
       * in the original picture will be mapped to the same color on the screen.
       * X does this for you silently.  However, this is not-desirable for
       * this application, because when I say 'allocate me 32 colors' I want
       * it to allocate 32 different colors, not 32 instances of the same 4
       * shades...                                                           */

      for (i = 0; i < 256; i++)
          failed[i] = 1;

      for (i = 0; i < masterPtr->numColors && unique < maxColors; i++) {
          c = masterPtr->allocOrder[i];
          defs[c].red   = rMap[c]<<8;
          defs[c].green = gMap[c]<<8;
          defs[c].blue  = bMap[c]<<8;

          defs[c].flags = DoRed | DoGreen | DoBlue;

          if (XAllocColor(display, colormap, &defs[c])) {
             unsigned long pixel, *fcptr;

             pixel = xPixVals[c] = defs[c].pixel;
                       /* xPixVals[], maps the pic's original pixel values to
                        * to X pixel values                                 */
             failed[c] = 0;

             /* See if the newly allocated color is new and different */
             for (j = 0, fcptr = freeColors; j < nFreeColors && *fcptr != pixel;
                     j++, fcptr++);
             if (j == nFreeColors) unique++;

             FC2PColor[nFreeColors] = c;
             freeColors[nFreeColors++] = pixel;
          }
          else {
               /* The allocation failed. */
               failed[c] = 1;    /* Just make sure */
          }
      }  /* FIRST PASS */

      if (nFreeColors == masterPtr->numColors) {
         /* Got all desired colors in the first pass. Great, return */
	 goto nextstep;
      }

      /* SECOND PASS COLOR ALLOCATION:
       * Allocating 'exact' colors failed.  Now try to allocate 'closest'
       * colors.

       * Read entire X colormap (or first 256 entries) in from display.
       * for each unallocated pixel, find the closest color that actually
       * is in the X colormap.  Try to allocate that color (read only).
       * If that fails, the THIRD PASS will deal with it.               */

      /* Read entire colormap (or first 256 entries) into 'xcolorTab' */

      if (dispCells > 0) {
                   /* only do SECOND PASS if there IS a colormap to read */

         for (i = 0; i < dispCells; i++)
             xcolorTab[i].pixel = (unsigned long) i;
         XQueryColors(display, colormap, xcolorTab, dispCells) ;

         for (i = 0; i < masterPtr->numColors && unique < maxColors; i++) {
             c = masterPtr->allocOrder[i];

             if (failed[c]) {  /* An unallocated pixel */
                int dist, mindist, close;
                int rdist, gdist, bdist;
                int ri, gi, bi;           /* corresponding to "i" loop */

                mindist = 200000;   close = -1;
                ri = rMap[c];  gi = gMap[c];  bi = bMap[c];

                for (j = 0; j < dispCells; j++) {

                    rdist = ri - (xcolorTab[j].red  >>8);
                    gdist = gi - (xcolorTab[j].green>>8);
                    bdist = bi - (xcolorTab[j].blue >>8);

                    dist = rdist * rdist + gdist * gdist + bdist * bdist;

                    if (dist < mindist) {
                       mindist = dist;
                       close   = j;
                    }
                }

                if (close < 0) panic("This Can't Happen!");
                if (XAllocColor(display, colormap, &xcolorTab[close])) {
                   safeBcopy((char *) &xcolorTab[close], (char *) &defs[c],
                                 sizeof(XColor));
                   failed[c]= 0;
                   xPixVals[c]  = xcolorTab[close].pixel;
                   FC2PColor[nFreeColors] = c;
                   freeColors[nFreeColors++] = xPixVals[c];
                   unique++;
                }
             }
         }
      } /* SECOND PASS */

      /* THIRD PASS COLOR ALLOCATION:
       * We've alloc'ed all the colors we can.  Now, we have to map any
       * remaining unalloced pixels into the colors found in the entire
       * X colormap */

      for (i = 0; i < masterPtr->numColors; i++) {
          c = masterPtr->allocOrder[i];

          if (failed[c]) {  /* An unallocated pixel */
             int dist, mindist, close;
             int rdist, gdist, bdist;
             int ri, gi, bi;               /* Corresponding to the "i" - loop */

             mindist = 200000;   close = -1;
             ri = rMap[c];  gi = gMap[c];  bi = bMap[c];

             /* search the the entire X colormap */
             for (j = 0; j < dispCells; j++) {

                 rdist = ri -(xcolorTab[j].red  >>8);
                 gdist = gi -(xcolorTab[j].green>>8);
                 bdist = bi -(xcolorTab[j].blue >>8);

                 dist = rdist * rdist + gdist * gdist + bdist * bdist;

                 if (dist < mindist) {
                    mindist = dist;
                    close = j;
                 }
             }

             if (close < 0) panic("Pass3: Failed to alloc ANY colors!");
             safeBcopy((char *) &xcolorTab[close], (char *) &defs[c],
                          sizeof(XColor));
             failed[c]= 0;
             xPixVals[c]  = defs[c].pixel;
          }
      } /* THIRD PASS */

      nextstep:

      colorPtr->nFreeColors = nFreeColors;
      
    } /* end of this LONG 'else' */

    /* For simplicity, let me assume it is a color display */
    colorPtr->flags |= COLOR_WINDOW;

    /* The following is a hairy hack.  We only want to index into
     * the pixelMap on colormap displays.  However, if the display
     * is on Windows, then we actually want to store the index not
     * the value since we will be passing the color table into the
     * TkPutImage call.                                           */

    if ((colorPtr->visualInfo.class != DirectColor)
                && (colorPtr->visualInfo.class != TrueColor)) {
#ifndef _Windows
            colorPtr->flags |= MAP_COLORS;
#endif /* _Windows */
    }

}

/*
 *----------------------------------------------------------------------
 *
 * DisposeColorTable --
 *
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The colors in the argument color table are freed, as is the
 *	color table structure itself.  The color table is removed
 *	from the hash table which is used to locate color tables.
 *
 *----------------------------------------------------------------------
 */

static void
DisposeColorTable(clientData)
    ClientData clientData;	/* Pointer to the ColorTable whose
				 * colors are to be released. */
{
    ColorTable *colorPtr;
    Tcl_HashEntry *entry;

    colorPtr = (ColorTable *) clientData;
    if (colorPtr->freeColors != NULL) {
	if (colorPtr->nFreeColors > 0) {
	    XFreeColors(colorPtr->id.display, colorPtr->id.colormap,
		    colorPtr->freeColors, colorPtr->nFreeColors, 0);
	    Tk_FreeColormap(colorPtr->id.display, colorPtr->id.colormap);
	}
	ckfree((char *) colorPtr->freeColors);
    }

    entry = Tcl_FindHashEntry(&ptimageColorTabHash, (char *) &colorPtr->id);
    if (entry == NULL) {
	panic("DisposeColorTable couldn't find hash entry");
    }
    Tcl_DeleteHashEntry(entry);

    ckfree((char *) colorPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * DisposeInstance --
 *
 *	This procedure is called to finally free up an instance
 *	of a ptimage image which is no longer required.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The instance data structure and the resources it references
 *	are freed.
 *
 *----------------------------------------------------------------------
 */

static void
DisposeInstance(clientData)
    ClientData clientData;	/* Pointer to the instance whose resources
				 * are to be released. */
{
    PtimageInstance *instancePtr = (PtimageInstance *) clientData;
    PtimageInstance *prevPtr;

    if (instancePtr->pixmap != None) {
	Tk_FreePixmap(instancePtr->display, instancePtr->pixmap);
    }
    if (instancePtr->gc != None) {
	Tk_FreeGC(instancePtr->display, instancePtr->gc);
    }
    if (instancePtr->imagePtr != NULL) {
	XFree((char *) instancePtr->imagePtr);
    }
    if (instancePtr->colorTablePtr != NULL) {
	FreeColorTable(instancePtr->colorTablePtr);
    }

    if (instancePtr->masterPtr->instancePtr == instancePtr) {
	instancePtr->masterPtr->instancePtr = instancePtr->nextPtr;
    } else {
	for (prevPtr = instancePtr->masterPtr->instancePtr;
		prevPtr->nextPtr != instancePtr; prevPtr = prevPtr->nextPtr) {
	    /* Empty loop body */
	}
	prevPtr->nextPtr = instancePtr->nextPtr;
    }
    Tk_FreeColormap(instancePtr->display, instancePtr->colormap);
    ckfree((char *) instancePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * MatchFileFormat --
 *
 *	This procedure is called to find a ptimge image-file-format
 *	-handler which can parse the image data in the given file.
 *      If a user-specified format string is provided, only handlers
 *      whose names match a prefix of the format string are tried.
 * Results:
 *	A standard TCL return value.  If the return value is TCL_OK, a
 *	pointer to the image format record is returned in
 *	*imageFormatPtr, and the width and height of the image are
 *	returned in *widthPtr and *heightPtr.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
MatchFileFormat(interp, f, fileName, formatString, imageFormatPtr,
        widthPtr, heightPtr)
    Tcl_Interp *interp;		/* Interpreter to use for reporting errors. */
    FILE *f;			/* The image file, open for reading. */
    char *fileName;		/* The name of the image file. */
    char *formatString;		/* User-specified format string, or NULL. */
    Tk_PtimageImageFormat **imageFormatPtr;
				/* A pointer to the ptimage image format
				 * record is returned here. */
    int *widthPtr, *heightPtr;	/* The dimensions of the image are
				 * returned here. */
{
    int matched;
    Tk_PtimageImageFormat *formatPtr;

    /*
     * Scan through the table of file format handlers to find
     * one which can handle the image.
     */

    matched = 0;
    for (formatPtr = formatList; formatPtr != NULL;
	 formatPtr = formatPtr->nextPtr) {
        if (formatString != NULL) {
            if (strncasecmp(formatString, formatPtr->name,
                    strlen(formatPtr->name)) != 0) {
                continue;
	        }
                matched = 1;
                if (formatPtr->fileMatchProc == NULL) {
                    Tcl_AppendResult(interp, "-file option isn't supported for ",
                            formatString, " images", (char *) NULL);
                    return TCL_ERROR;
		}
	}
	if (formatPtr->fileMatchProc != NULL) {
	    fseek(f, 0L, SEEK_SET);
	    if ((*formatPtr->fileMatchProc)(f, fileName, formatString,
		    widthPtr, heightPtr)) {
		if (*widthPtr < 1) {
		    *widthPtr = 1;
		}
		if (*heightPtr < 1) {
		    *heightPtr = 1;
		}
		break;
	    }
	}
    }

    if (formatPtr == NULL) {
        if ((formatString != NULL) && !matched) {
            Tcl_AppendResult(interp, "image file format \"", formatString,
                                 "\" is not supported", (char *) NULL);
	} else {
            Tcl_AppendResult(interp,
		    "couldn't recognize data in image file \"",
		    fileName, "\"", (char *) NULL);
	}
	return TCL_ERROR;
    }

    *imageFormatPtr = formatPtr;
    fseek(f, 0L, SEEK_SET);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * MatchStringFormat --
 *
 *	This procedure is called to find a ptimage image-file-format
 *	-handler which can parse the image data in the given string.
 *	If a user-specified format string is provided, only handlers
 *	whose names match a prefix of the format string are tried.
 *
 * Results:
 *	A standard TCL return value.  If the return value is TCL_OK, a
 *	pointer to the image format record is returned in
 *	*imageFormatPtr, and the width and height of the image are
 *	returned in *widthPtr and *heightPtr.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
MatchStringFormat(interp, string, formatString, imageFormatPtr,
        widthPtr, heightPtr)
    Tcl_Interp *interp;		/* Interpreter to use for reporting errors. */
    char *string;		/* String containing the image data. */
    char *formatString;         /* User-specified format string, or NULL. */
    Tk_PtimageImageFormat **imageFormatPtr;
				/* A pointer to the ptimage image format
				 * record is returned here. */
    int *widthPtr, *heightPtr;	/* The dimensions of the image are
				 * returned here. */
{
    int matched;
    Tk_PtimageImageFormat *formatPtr;

    /*
     * Scan through the table of file format handlers to find
     * one which can handle the image.
     */

    matched = 0;
    for (formatPtr = formatList; formatPtr != NULL;
	    formatPtr = formatPtr->nextPtr) {
         if (formatString != NULL) {
             if (strncasecmp(formatString, formatPtr->name,
                      strlen(formatPtr->name)) != 0) {
                 continue;
	     }
             matched = 1;
             if (formatPtr->stringMatchProc == NULL) {
                 Tcl_AppendResult(interp, "-data option isn't supported for ",
                         formatString, " images", (char *) NULL);
                 return TCL_ERROR;
	     }
	 }
	 if ((formatPtr->stringMatchProc != NULL)
		&& (*formatPtr->stringMatchProc)(string, formatString,
		widthPtr, heightPtr)) {
	    break;
	}
    }

    if (formatPtr == NULL) {
	if ((formatString != NULL) && !matched) {
	    Tcl_AppendResult(interp, "image format \"", formatString,
		    "\" is not supported", (char *) NULL);
	} else {
	    Tcl_AppendResult(interp, "couldn't recognize image data",
		    (char *) NULL);
	}
	return TCL_ERROR;
    }

    *imageFormatPtr = formatPtr;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_FindPtimage --
 *
 *	This procedure is called to get an opaque handle (actually a
 *	PtimageMaster *) for a given image. The `name' parameter is
 *      the name of the image.
 *
 * Results:
 *	The handle for the ptimage image, or NULL if there is no
 *	ptimage image with the name given.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Tk_PtimageHandle
Tk_FindPtimage(imageName)
    char *imageName;		/* Name of the desired ptimage image. */
{
    Tcl_HashEntry *entry;

    if (!ptimageHashInitialized) {
	return NULL;
    }
    entry = Tcl_FindHashEntry(&ptimageHash, imageName);
    if (entry == NULL) {
	return NULL;
    }
    return (Tk_PtimageHandle) Tcl_GetHashValue(entry);
}


/*
 *----------------------------------------------------------------------
 *
 * Tk_PtimageSetSize --
 *
 *      This procedure is called to set size of a ptimage image.
 *      This call is equivalent to using the -width and -height
 *      configuration options.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The size of the image may change; if so the generic
 *      image code is informed.
 *
 *----------------------------------------------------------------------
 */

void
Tk_PtimageSetSize(handle, width, height)
    Tk_PtimageHandle handle;    /* Handle for the image whose size is to
                                 * be set. */
    int width, height;          /* New dimensions for the image. */
{
    PtimageMaster *masterPtr;

    masterPtr = (PtimageMaster *) handle;

    masterPtr->width = width;
    masterPtr->height = height;
    Tk_ImageChanged(masterPtr->tkMaster, 0, 0, 0, 0,
            masterPtr->width, masterPtr->height);
}

/*
 *--------------------------------------------------------------
 * Tk_PtimageConfigureInstance --
 *-------------------------------------------------------------
 */

void
Tk_PtimageConfigureInstance(instancePtr)
    PtimageInstance *instancePtr;  /* Pointer to the ptimage instance that
				    * is to be configured.             */
{
    ImgPtimageConfigureInstance(instancePtr);
}
   

/*
 *--------------------------------------------------------------
 * A ptimage image file handler for PPM (Portable PixMap) files.
 *--------------------------------------------------------------
 */

/*
 * The format record for the PPM file format:
 */

static int		FileMatchPPM _ANSI_ARGS_((FILE *f, char *fileName,
			    char *formatString, int *widthPtr,
			    int *heightPtr));
static int		FileReadPPM _ANSI_ARGS_((Tcl_Interp *interp,
			    FILE *f, char *fileName, char *formatString,
			    Tk_PtimageHandle imageHandle));
static int		FileWritePPM _ANSI_ARGS_((Tcl_Interp *interp,
			    char *fileName, char *formatString,
			    Tk_PtimageHandle imageHandle));

Tk_PtimageImageFormat tkPtImgFmtPPM = {
    "PPM",			/* name */
    FileMatchPPM,		/* fileMatchProc */
    NULL,			/* stringMatchProc */
    FileReadPPM,		/* fileReadProc */
    NULL,			/* stringReadProc */
    FileWritePPM,		/* fileWriteProc */
    NULL,			/* stringWriteProc */
};

/*
 * Prototypes for local procedures defined in this file:
 */

static int		ReadPPMFileHeader _ANSI_ARGS_((FILE *f, int *widthPtr,
			    int *heightPtr, int *maxIntensityPtr));

/*
 *----------------------------------------------------------------------
 *
 * FileMatchPPM --
 *
 *	This procedure is invoked by the ptimage image type to see if
 *	a file contains image data in PPM format.
 *
 * Results:
 *	The return value is >0 if the first characters in file "f" look
 *	like PPM data, and 0 otherwise.
 *
 * Side effects:
 *	The access position in f may change.
 *
 *----------------------------------------------------------------------
 */

static int
FileMatchPPM(f, fileName, formatString, widthPtr, heightPtr)
    FILE *f;			/* The image file, open for reading. */
    char *fileName;		/* The name of the image file. */
    char *formatString;		/* User-specified format string, or NULL. */
    int *widthPtr, *heightPtr;	/* The dimensions of the image are
				 * returned here if the file is a valid
				 * raw PPM file. */
{
    int dummy;

    return ReadPPMFileHeader(f, widthPtr, heightPtr, &dummy);
}

/*
 *----------------------------------------------------------------------
 *
 * FileReadPPM --
 *
 *	This procedure is called by the ptimage image type to read
 *	PPM format data from a file and write it into a given
 *	ptimage image.
 *
 * Results:
 *	A standard TCL completion code.  If TCL_ERROR is returned
 *	then an error message is left in interp->result.
 *
 * Side effects:
 *	The access position in file f is changed, and new data is
 *	added to the image given by imageHandle.
 *
 *----------------------------------------------------------------------
 */

static int
FileReadPPM(interp, f, fileName, formatString, imageHandle)
    Tcl_Interp *interp;		/* Interpreter to use for reporting errors. */
    FILE *f;			  /* The image file, open for reading.      */
    char *fileName;		  /* The name of the image file.            */
    char *formatString;		  /* User-specified format string, or NULL. */
    Tk_PtimageHandle imageHandle; /* The ptimage image to write into.       */
{
    int width, height, maxIntensity;
    int i, j, type, length, numColors, maxColors = 256;
    byte *rMap, *gMap, *bMap, *allocOrder;
    byte *newPix8, trans[256], scale[256];
    byte *pix8 = 0, *pix24 = 0;
    byte *pixels, *npixels;
    PtimageMaster *masterPtr;
    
    type = ReadPPMFileHeader(f, &width, &height, &maxIntensity);
    if (type == 0) {
	Tcl_AppendResult(interp, "couldn't read raw PPM header from file \"",
		fileName, "\"", NULL);
	return TCL_ERROR;
    }
    if ((width <= 0) || (height <= 0)) {
	Tcl_AppendResult(interp, "PPM image file \"", fileName,
		"\" has dimension(s) <= 0", (char *) NULL);
	return TCL_ERROR;
    }
    if ((maxIntensity <= 0) || (maxIntensity >= 256)) {
	char buffer[30];

	sprintf(buffer, "%d", maxIntensity);
	Tcl_AppendResult(interp, "PPM image file \"", fileName,
		"\" has bad maximum intensity value ", buffer,
		(char *) NULL);
	return TCL_ERROR;
    }

    /* Now, put data into masterPtr->pix8, or masterPtr->pix24,
     * and run colormap manipulation to get masterPtr->newPix8 for
     * PseudoColor Visual model. Let me not worry about DirectColor
     * or TrueColor for now */

    masterPtr = (PtimageMaster *) imageHandle;
    if (!masterPtr->rMap) ckfree((char *) masterPtr->rMap);
    if (!masterPtr->gMap) ckfree((char *) masterPtr->gMap);
    if (!masterPtr->bMap) ckfree((char *) masterPtr->bMap);

    length = 256;
    rMap = (byte *) ckalloc((size_t) length);
    if(!rMap) panic("Could not alloc rMap for masterPtr."); 
    gMap = (byte *) ckalloc((size_t) length);
    if(!gMap) panic("Could not alloc gMap for masterPtr."); 
    bMap = (byte *) ckalloc((size_t) length);
    if(!bMap) panic("Could not alloc bMap for masterPtr."); 
    allocOrder = (byte *) ckalloc((size_t) length);
    if(!allocOrder) panic("Could not alloc allocOrder for masterPtr."); 

    masterPtr->rMap = rMap;
    masterPtr->gMap = gMap;
    masterPtr->bMap = bMap;
    masterPtr->allocOrder = allocOrder;

    if (type == PGM_FORMAT) {

       /* Fill in a greyscale colormap where maxIntensity maps to 255 */

       for (i = 0; i <= maxIntensity; i++)
            rMap[i] = gMap[i] = bMap[i] = (i * 255) / maxIntensity;

       /* Read Raw data, assuming maxIntensity <= 255 for simplicity */

       if (!masterPtr->pix8) ckfree((char *) masterPtr->pix8);

       pix8 = (byte *) ckalloc((unsigned) (width * height));
       if (!pix8) panic("Could not ckalloc 'pix8' for PGM file"); 
       fread(pix8, (size_t) 1, (size_t) width * height, f); 

       masterPtr->pix8 = pix8;
    }
    else {  /* Must be PPM_FORMAT */

       /* Read raw data, assuming maxIntensity <= 255 for simplicity */

       if (!masterPtr->pix24) ckfree((char *) masterPtr->pix24);

       pix24 = (byte *) ckalloc((unsigned) (width * height * 3));
       if (!pix24) panic("Could not ckalloc 'pix24' for PPM file");

       fread(pix24, (size_t)1, (size_t) width * height * 3, f);
         
       masterPtr->pix24 = pix24;

    } /* PPM */

    fclose(f);
    
    if (type == PGM_FORMAT) {

       if (!masterPtr->newPix8) ckfree((char *) masterPtr->newPix8);

       newPix8 = (byte *) ckalloc((unsigned) (width * height));
       if (!newPix8) panic("Could not ckalloc 'newPix8' for PGM file"); 

       for (i = 0, pixels = pix8, npixels = newPix8; i < height; i++)
            for (j = 0; j < width; j++, pixels++, npixels++)
                 *npixels = *pixels; 
 
       masterPtr->newPix8 = newPix8;
       masterPtr->formatFlag = PGM_FORMAT;
    }
    else {  /* Must be PPM_FORMAT */

       /* Have to scale all RGB values up (Tk_PtConv24to8 expects RGB values
        * to range from 0 to 255 */
       if (maxIntensity < 255) {
           for (i = 0; i <= maxIntensity; i++)
                scale[i] = (i * 255) /maxIntensity;
           for (i = 0, pixels = pix24; i < height; i++)
                for (j = 0; j < width * 3; j++, pixels++)
                     *pixels = scale[*pixels];
       }
 
       if (!masterPtr->newPix8) ckfree((char *) masterPtr->newPix8);
       masterPtr->newPix8 = Tk_PtConv24to8(pix24, width, height, maxColors,
                                           rMap, gMap, bMap);
       masterPtr->formatFlag = PPM_FORMAT; 
    }

    /* Sorting the desired colormap */
    Tk_PtSortColormap(masterPtr->newPix8, width, height, &numColors,
                    rMap, gMap, bMap, allocOrder, trans);
    Tk_PtColorCompress8(masterPtr->newPix8, trans, width, height);

    masterPtr->numColors = numColors;

    Tk_ImageChanged(masterPtr->tkMaster, 0, 0, width, height, width, height);

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * FileWritePPM --
 *
 *	This procedure is invoked to write image data to a file in PPM
 *	format.
 *
 * Results:
 *	A standard TCL completion code.  If TCL_ERROR is returned
 *	then an error message is left in interp->result.
 *
 * Side effects:
 *	Data is written to the file given by "fileName".
 *
 *----------------------------------------------------------------------
 */

static int
FileWritePPM(interp, fileName, formatString, imageHandle)
    Tcl_Interp *interp;
    char *fileName;
    char *formatString;
    Tk_PtimageHandle imageHandle; /* The ptimage image to read from  */
{
    PtimageMaster *masterPtr;
    int width, height;
    byte *pixelPtr;
    Tcl_DString buffer;
    char *realFileName;
    FILE *fp;

    realFileName = Tcl_TildeSubst(interp, fileName, &buffer);
    if (realFileName == NULL) {
       return TCL_ERROR;
    }
    fp = fopen(realFileName, "wb");
    Tcl_DStringFree(&buffer);
    if (fp == NULL) {
	Tcl_AppendResult(interp, fileName, ": ", Tcl_PosixError(interp),
		(char *)NULL);
	return TCL_ERROR;
    }

    masterPtr = (PtimageMaster *) imageHandle;
    width = masterPtr->width;
    height = masterPtr->height;

    if (masterPtr->formatFlag == PGM_FORMAT) {
       fprintf(fp, "P5\n%d %d\n255\n", width, height);
       pixelPtr = masterPtr->pix8;
       if (fwrite(pixelPtr, (size_t) 1, (unsigned) width * height,
                        fp) != width * height) {
          goto writeerror;
       }
    }
    else if (masterPtr->formatFlag == PPM_FORMAT) {
	    fprintf(fp, "P6\n%d %d\n255\n", width, height);
	    pixelPtr = masterPtr->pix24;
	    if (fwrite(pixelPtr, (size_t) 1, (unsigned) width * height * 3,
			     fp) != width * height * 3) {
               goto writeerror;
            }
    } 

    if (fclose(fp) == 0) {
	return TCL_OK;
    }
    fp = NULL;

 writeerror:
    Tcl_AppendResult(interp, "error writing \"", fileName, "\": ",
	    Tcl_PosixError(interp), (char *) NULL);
    if (fp != NULL) {
	fclose(fp);
    }
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * ReadPPMFileHeader --
 *
 *	This procedure reads the PPM header from the beginning of a
 *	PPM file and returns information from the header.
 *
 * Results:
 *	The return value is PGM if file "f" appears to start with
 *	a valid PGM header, PPM if "f" appears to start with a valid
 *      PPM header, and 0 otherwise.  If the header is valid,
 *	then *widthPtr and *heightPtr are modified to hold the
 *	dimensions of the image and *maxIntensityPtr is modified to
 *	hold the value of a "fully on" intensity value.
 *
 * Side effects:
 *	The access position in f advances.
 *
 *----------------------------------------------------------------------
 */

static int
ReadPPMFileHeader(f, widthPtr, heightPtr, maxIntensityPtr)
    FILE *f;			/* Image file to read the header from */
    int *widthPtr, *heightPtr;	/* The dimensions of the image are
				 * returned here. */
    int *maxIntensityPtr;	/* The maximum intensity value for
				 * the image is stored here. */
{
#define BUFFER_SIZE 1000
    char buffer[BUFFER_SIZE];
    int i, numFields, firstInLine, c;
    int type = 0;

    /*
     * Read 4 space-separated fields from the file, ignoring
     * comments (any line that starts with "#").
     */

    c = getc(f);
    firstInLine = 1;
    i = 0;
    for (numFields = 0; numFields < 4; numFields++) {
	/*
	 * Skip comments and white space.
	 */

	while (1) {
	    while (isspace(UCHAR(c))) {
		firstInLine = (c == '\n');
		c = getc(f);
	    }
	    if (c != '#') {
		break;
	    }
	    do {
		c = getc(f);
	    } while ((c != EOF) && (c != '\n'));
	    firstInLine = 1;
	}

	/*
	 * Read a field (everything up to the next white space).
	 */

	while ((c != EOF) && !isspace(UCHAR(c))) {
	    if (i < (BUFFER_SIZE-2)) {
		buffer[i]  = c;
		i++;
	    }
	    c = getc(f);
	}
	if (i < (BUFFER_SIZE-1)) {
	    buffer[i] = ' ';
	    i++;
	}
	firstInLine = 0;
    }
    buffer[i] = 0;

    /*
     * Parse the fields, which are: id, width, height, maxIntensity.
     */

    if (strncmp(buffer, "P6 ", 3) == 0) {
	type = PPM_FORMAT;
    } else if (strncmp(buffer, "P5 ", 3) == 0) {
	type = PGM_FORMAT;
    } else {
	return 0;
    }
    if (sscanf(buffer+3, "%d %d %d", widthPtr, heightPtr, maxIntensityPtr)
	    != 3) {
	return 0;
    }
    return type;
}

/*
 *----------------------------------------------------------------------
 * ZeroByte --
 *----------------------------------------------------------------------
 */

static void
ZeroByte(string, length)
char *string;
size_t length;
{ 
  for ( ; length > 0; length--) *string++ = 0;
}

/*
 *----------------------------------------------------------------------
 * safeBcopy --
 *----------------------------------------------------------------------
 */

static void
safeBcopy(src, dst, len)
char *src, *dst;
size_t len;
{
  /* Modern OS's (Solaris, etc.) frown upon the use of bcopy(), and
   * only want you to use memcpy().  However, memcpy() is broken,
   * in the sense that it doesn't properly handle overlapped regions
   * of memory.  This routine does, and also has its arguments in
   * the same order as bcopy() did, without using bcopy().
   */

  /* Determine if the regions overlap --
   *
   * 3 cases:  src=dst, src<dst, src>dst
   *
   * if src = dst, they overlap completely, but nothing needs to be moved;
   * if src < dst and src + len > dst then they overlap;
   * if src > dst and src < dst + len then they overlap.
   */

  if (src == dst || len <= 0)  /* Nothing to do, return */
     return;

  if ((src < dst) && (src + len > dst)) {
     /* Do a backward copy */
     src = src + len - 1;
     dst = dst + len - 1;
     for ( ; len > 0; len--, src--, dst--)
         *dst = *src;
  }

  else {
       /* They either overlap (src>dst) or they don't overlap */
       /* So do a forward copy */
       for ( ; len>0; len--, src++, dst++)
           *dst = *src;
  }
}

/*
 *--------------------------------------------------------------------------
 * structure and routine used in SortColormap() 
 *--------------------------------------------------------------------------
 */

typedef struct _CmapEnt {
  byte r,g,b, n;     /* Actual value of color + alignment                    */
  int oldindex;      /* Its index in the compressed but pre-ordered colormap */
  int freq;          /* Number of pixels of this color                       */
  int mindist;       /* Minimum distance to a selected color                 */
} CmapEnt;

/*
 *--------------------------------------------------------------------------
 * Tk_PtSortColormap --
 *--------------------------------------------------------------------------
 */

void
Tk_PtSortColormap(pic8, width, height, numColors, rmap, gmap, bmap,
			  order, trans)
byte *pic8, *rmap, *gmap, *bmap, *order, *trans;
int width, height, *numColors;
{
  /* Operates on 8-bit images. Sorts the colormap into 'best' order
   * 'order' is the 'best' order to allocate the colors.  'trans' is a
   * transformation to be done to pic8(in PIC8 mode) to compress the
   * colormap
   */

  static CmapEnt clrEntry[256], orderedEnt[256];
  static CmapEnt *clrPicker, *lastColor, *next;
  int  hist[256], ncolors, entry, maxuse, maxdist, dist, i, j;
  byte *p;

  /* Initialize order */
  for (i = 0; i < 256; i++) order[i] = i;

  /* Initialize histogram and compute it */
  for (i=0; i < 256; i++)
      hist[i] = 0;
  for (i = width * height, p = pic8; i; i--, p++)
      hist[*p]++;

  /* Put the colors that are actually used into clrEntry[] array, the 
   * pre-compressed colortable, in the  order they occur: calculate
   * ncolors, which is the # of actually-used colors, and close up gaps
   * in the table.
   */

  for (i = ncolors = 0; i < 256; i++) {
      if (hist[i]) {
         rmap[ncolors] = rmap[i];
         gmap[ncolors] = gmap[i];
         bmap[ncolors] = bmap[i];
         trans[i] = (byte) ncolors;  /* That is, to contruct a link between
				      * the index of an actually-used color
				      * in the original table and the index
				      * of its in the compressed table.
				      */
         clrPicker    = &clrEntry[ncolors];
         clrPicker->r = rmap[i];  
         clrPicker->g = gmap[i]; 
         clrPicker->b = bmap[i];
         clrPicker->freq = hist[i];
         clrPicker->oldindex = ncolors;
         clrPicker->mindist = 200000;   /* 255^2 * 3 = 195075 */
         ncolors++;
      }
  }

  /* Find the most-used color, put that in orderedEnt[0] */
  entry = -1;  maxuse = -1;
  for (i = 0; i < ncolors; i++) {
      if (clrEntry[i].freq > maxuse) {
         maxuse = clrEntry[i].freq;
	 entry = i;
      }
  }
  safeBcopy((char *)&clrEntry[entry], (char *)&orderedEnt[0], sizeof(CmapEnt));

  clrEntry[entry].freq = 0;   /* Since picked, 'freq' gets marked 0 */

  /* Sort the rest of colormap.  Half of the entries are allocated on the
   * basis of distance from already allocated colors, and half on the
   * basis of usage.
   *
   * Modification: Pick the first 10 colors based on maximum distance; 
   *               pick the remaining colors half by distance and half
   *               by usage. 
   *
   * To obtain O(n^2) performance, we keep each unselected color
   * (in clrEntry[], with freq > 0) marked with the minimum distance to
   * any of the selected colors (in orderedEnt[]). Each time we select
   * a color, we can update the minimum distances in O(n) time.
   */

  for (i = 1; i < ncolors; i++) {

      int lastClrR, lastClrG, lastClrB;

      /* Get RGB of color last selected  and choose selection method */

      lastColor = &orderedEnt[i-1];   /* point to just-selected color */
      lastClrR = lastColor->r;
      lastClrG = lastColor->g;
      lastClrB = lastColor->b;

      /* Now find the i'th most different color */

      if ((i & 1) || (i < 10)) {

         /* Select the unmarked color that has the greatest mindist from 
	  * the just-picked color in OrderedEnt[]*/

         entry = -1;  maxdist = -1;
         for (j = 0, next = clrEntry; j < ncolors; j++, next++) {
             if (next->freq) {    /* this color has not been marked already */
                /* update mindist */
                dist = (next->r - lastClrR)*(next->r - lastClrR) +
                       (next->g - lastClrG)*(next->g - lastClrG) +
                       (next->b - lastClrB)*(next->b - lastClrB);
                if (next->mindist > dist)
	           next->mindist = dist;
                if (next->mindist > maxdist) {
	           maxdist = next->mindist;
	           entry = j;
	        }
             }
         }

      }
      else {

           /* select the unused color that has the greatest usage */

           entry = -1;  maxuse = -1;
           for (j = 0, next = clrEntry; j < ncolors; j++, next++) {
               if (next->freq) {  /* this color has not been marked already */
                  /* update mindist */
                  dist = (next->r - lastClrR)*(next->r - lastClrR) +
                         (next->g - lastClrG)*(next->g - lastClrG) +
                         (next->b - lastClrB)*(next->b - lastClrB);
                  if (next->mindist > dist)
	          next->mindist = dist;
                  if (next->freq > maxuse) {
	             maxuse = next->freq;
		     entry = j;
                  } 
               }
           }
      }

      /* clrEntry[entry] is the next color to put in the map.  Do so. */

      safeBcopy((char *) &clrEntry[entry], (char *) &orderedEnt[i],
                     sizeof(CmapEnt));
      clrEntry[entry].freq = 0;

  }

  for (i = 0; i < ncolors; i++)
      order[i] = (byte) orderedEnt[i].oldindex;

  *numColors = ncolors;
}

/*
 *--------------------------------------------------------------------------
 * Tk_PtColorCompress8 --
 *--------------------------------------------------------------------------
 */

void
Tk_PtColorCompress8(pic8, trans, width, height)
byte *pic8, *trans;
int width, height;
{
  /* Modify pic8 to reflect new (compressed) colormap */

  int i;
  byte *p, j;

  for (i = width * height, p = pic8; i; i--, p++) {
      j = trans[*p];
      *p = j;
  }

}

static int NoNeedConv _ANSI_ARGS_((byte *pic24, int width, int height,
               byte *pic8, byte *rmap, byte *gmap, byte *bmap, int maxcols));
static int heckbert _ANSI_ARGS_((byte *pic24, int width, int height,
               byte *pic8, byte *rmap, byte *gmap, byte *bmap, int descols));

/*
 *--------------------------------------------------------------------------
 * Tk_PtConv24to8--
 *--------------------------------------------------------------------------
 */

byte *
Tk_PtConv24to8(pic24, width, height, maxcols, rmap, gmap, bmap)
byte *pic24, *rmap, *gmap, *bmap;
int width, height, maxcols;
{
  byte *pic8;

  if (!pic24) 
     return NULL;
 
  pic8 = (byte *) ckalloc((unsigned) (width * height));
  if (!pic8) {
     panic("Tk_PtConv24to8() failed to allocate 'pic8'."); 
     return pic8;
  }

  if (NoNeedConv(pic24, width, height, pic8, rmap, gmap, bmap, maxcols))
     return pic8;

  /* Do Herkbert's Median Cut algorithm : 24-bit to 8-bit */
  if (heckbert(pic24, width, height, pic8, rmap, gmap, bmap, maxcols) != 0) {
     free(pic8);
     pic8 = NULL;
  }

  return pic8;
}

/*
 *------------------------------------------------------------------------
 * NoNeedConv --
 *------------------------------------------------------------------------
 */

static int
NoNeedConv(pic24, width, height, pic8, rmap, gmap, bmap, maxcols)
byte *pic24, *pic8, *rmap, *gmap, *bmap;
int width, height, maxcols;
{

  /* Scans picture until it finds more than 'maxcols' different colors.
     If it finds more than 'maxcols' colors, returns 0; if it does not, 
     it then does the 24to8 conversion by simply sticking the colors it
     has found into a colormap, and changing instances of a color in pic24
     into colormap indices in pic8 */

  unsigned long xpixels[256], pixel;
  int           i, nc, low, high, mid;
  byte         *picptr, *pixelptr;

  /* Put the first color in the table by hand */
  nc = 0;  mid = 0;

  for (i = width * height, picptr = pic24; i; i--) {
      pixel  = (((unsigned long) *picptr++) << 16);
      pixel += (((unsigned long) *picptr++) << 8);
      pixel += *picptr++;

      /* Binary search the 'xpixels' array to see if it's in there */
      low = 0; high = nc - 1;

      while (low <= high) {
            mid = (low + high) / 2;
            if (pixel < xpixels[mid])
               high = mid - 1;
            else if (pixel > xpixels[mid])
                    low  = mid + 1;
            else break;
      }

      if (high < low) { /* Didn't find color in list, add it. */
         if (nc >= maxcols)
            return 0;
         safeBcopy((char *) &xpixels[low], (char *) &xpixels[low +1],
                           (nc - low) * sizeof(unsigned long));
         xpixels[low] = pixel;
         nc++;
      }
  }

  /* Run through the data a second time, this time mapping pixel values in
     pic24 into colormap offsets into 'xpixels' */

  for (i = width*height, picptr = pic24, pixelptr = pic8; i; i--, pixelptr++) {
      pixel  = (((unsigned long) *picptr++) << 16);
      pixel += (((unsigned long) *picptr++) << 8);
      pixel +=  *picptr++;

      /* Binary search the 'xpixels' array.  It *is* in there */
      low = 0;  high = nc - 1;
      while (low <= high) {
            mid = (low + high) / 2;
            if (pixel < xpixels[mid])
               high = mid - 1;
            else if (pixel > xpixels[mid])
                    low  = mid + 1;
            else break;
      }

      if (high < low) {
         panic("NoNeedConv:  Impossible situation!");
         exit(1);
      }

      *pixelptr = mid;
  }

  /* Now load up the 'desired colormap' */
  for (i = 0; i < nc; i++) {
      rmap[i] =  xpixels[i]>>16;
      gmap[i] = (xpixels[i]>>8) & 0xff;
      bmap[i] =  xpixels[i]     & 0xff;
  }

  return 1;
}

/*
 *------------------------------------------------------------------------
 * The following is based on jquant2.c from version 5         
 * of the IJG JPEG software, which is                       
 * Copyright (C) 1991-1994, Thomas G. Lane.            
 *------------------------------------------------------------------------
 */

#define MAXNUMCOLORS  256	/* maximum size of colormap */

#define C0_SCALE 2		/* scale R distances by this much */
#define C1_SCALE 3		/* scale G distances by this much */
#define C2_SCALE 1		/* and B by this much */

#define HIST_C0_BITS  5		/* bits of precision in R histogram */
#define HIST_C1_BITS  6		/* bits of precision in G histogram */
#define HIST_C2_BITS  5		/* bits of precision in B histogram */

/* Number of elements along histogram axes. */
#define HIST_C0_ELEMS  (1<<HIST_C0_BITS)
#define HIST_C1_ELEMS  (1<<HIST_C1_BITS)
#define HIST_C2_ELEMS  (1<<HIST_C2_BITS)

/* These are the amounts to shift an input value to get a histogram index. */
#define C0_SHIFT  (8-HIST_C0_BITS)
#define C1_SHIFT  (8-HIST_C1_BITS)
#define C2_SHIFT  (8-HIST_C2_BITS)


typedef unsigned char JSAMPLE;
typedef JSAMPLE * JSAMPROW;

typedef unsigned short histcell;  /* histogram cell; prefer an unsigned type */

typedef histcell * histptr;	/* for pointers to histogram cells */

typedef histcell hist1d[HIST_C2_ELEMS]; /* typedefs for the histogram array */
typedef hist1d hist2d[HIST_C1_ELEMS];
typedef hist2d hist3d[HIST_C0_ELEMS];

typedef short FSERROR;		/* 16 bits should be enough */
typedef int LOCFSERROR;		/* use 'int' for calculation temps */

typedef FSERROR *FSERRPTR;	/* pointer to error array */

typedef struct {
  /* The bounds of the box (inclusive); expressed as histogram indexes */
  int c0min, c0max;
  int c1min, c1max;
  int c2min, c2max;
  /* The volume (actually 2-norm) of the box */
  long volume;
  /* The number of nonzero histogram cells within this box */
  long colorcount;
} box;

typedef box *boxptr;

/* Local state for the IJG quantizer */

static hist2d *histogram;	/* pointer to the 3D histogram array */
static FSERRPTR fserrors;	/* accumulated-errors array */
static int *error_limiter;	/* table for clamping the applied error */
static int on_odd_row;	/* flag to remember which row we are on */
static JSAMPROW colormap[3];	/* selected colormap */
static int num_colors;	/* number of selected colors */


static void   fill_histogram (byte *, int);
static boxptr find_biggest_color_pop (boxptr, int);
static boxptr find_biggest_volume (boxptr, int);
static void   update_box (boxptr);
static int    median_cut (boxptr, int, int);
static void   compute_color (boxptr, int);
static void   select_colors (int);
static int    find_nearby_colors (int, int, int, JSAMPLE[]);
static void   find_best_colors (int,int,int,int, JSAMPLE[], JSAMPLE[]);
static void   fill_inverse_cmap (int, int, int);
static void   map_pixels (byte *, int, int, byte *);
static void   init_error_limit (void);

#define TRUE   1
#define FALSE  0

/* RANGE forces a to be in the range b..c (inclusive) */
#define RANGE(a,b,c) { if (a < b) a = b; if (a > c) a = c; }

/* Master control for heckbert() quantizer. */
static int
heckbert(pic24, width, height, pic8, rmap, gmap, bmap, descols)
byte *pic24, *pic8, *rmap, *gmap, *bmap;
int width, height, descols;
{
  size_t fs_arraysize = (width + 2) * (3 * sizeof(FSERROR));

  /* Allocate all the temporary storage needed */
  if (error_limiter == NULL)
     init_error_limit();
  histogram = (hist2d *) ckalloc(sizeof(hist3d));
  fserrors = (FSERRPTR) ckalloc(fs_arraysize);

  if (!error_limiter || !histogram || !fserrors) {
     /* we never free error_limiter once acquired */
     if (histogram) free(histogram);
     if (fserrors) free(fserrors);
     panic("heckbert() failed to allocate workspace.");
     return 1;
  }

  colormap[0] = (JSAMPROW) rmap;
  colormap[1] = (JSAMPROW) gmap;
  colormap[2] = (JSAMPROW) bmap;

  /* Compute the color histogram */
  fill_histogram(pic24, width * height);

  /* Select the colormap */
  select_colors(descols);

  /* Zero the histogram: now to be used as inverse color map */
  ZeroByte((char *) histogram, sizeof(hist3d));  

  /* Initialize the propagated errors to zero. */
  ZeroByte((char *) fserrors, fs_arraysize); 
  on_odd_row = FALSE;

  /* Map the image. */
  map_pixels(pic24, width, height, pic8);

  /* Release working memory. */
  /* we never free error_limiter once acquired */
  free(histogram);
  free(fserrors);

  return 0;
}


static void
fill_histogram (pic24, numpixels)
register byte *pic24;
register int numpixels;
{
  register histptr histp;
  register hist2d *hist = histogram;

  ZeroByte((char *) hist, sizeof(hist3d));

  while (numpixels-- > 0) {
        /* get pixel value and index into the histogram */
        histp = & hist[pic24[0] >> C0_SHIFT]
  		      [pic24[1] >> C1_SHIFT]
		      [pic24[2] >> C2_SHIFT];
        /* increment, check for overflow and undo increment if so. */
        if (++(*histp) <= 0)
           (*histp)--;
        pic24 += 3;
  }
}


static boxptr
find_biggest_color_pop (boxlist, numboxes)
boxptr boxlist;
int numboxes;
{
  register boxptr boxp;
  register int i;
  register long maxc = 0;
  boxptr which = NULL;
  
  for (i = 0, boxp = boxlist; i < numboxes; i++, boxp++) {
      if (boxp->colorcount > maxc && boxp->volume > 0) {
         which = boxp;
         maxc = boxp->colorcount;
      }
  }
  return which;
}


static boxptr
find_biggest_volume (boxlist, numboxes)
boxptr boxlist;
int numboxes;
{
  register boxptr boxp;
  register int i;
  register long maxv = 0;
  boxptr which = NULL;
  
  for (i = 0, boxp = boxlist; i < numboxes; i++, boxp++) {
      if (boxp->volume > maxv) {
         which = boxp;
         maxv = boxp->volume;
      }
  }
  return which;
}


static void
update_box (boxp)
boxptr boxp;
{
  hist2d * hist = histogram;
  histptr histp;
  int c0, c1, c2;
  int c0min, c0max, c1min, c1max, c2min, c2max;
  long dist0, dist1, dist2;
  long ccount;
  
  c0min = boxp->c0min;  c0max = boxp->c0max;
  c1min = boxp->c1min;  c1max = boxp->c1max;
  c2min = boxp->c2min;  c2max = boxp->c2max;
  
  if (c0max > c0min)
     for (c0 = c0min; c0 <= c0max; c0++)
         for (c1 = c1min; c1 <= c1max; c1++) {
	     histp = & hist[c0][c1][c2min];
	     for (c2 = c2min; c2 <= c2max; c2++)
	         if (*histp++ != 0) {
	            boxp->c0min = c0min = c0;
	            goto have_c0min;
	         }
         }

  have_c0min:
    if (c0max > c0min)
       for (c0 = c0max; c0 >= c0min; c0--)
           for (c1 = c1min; c1 <= c1max; c1++) {
	       histp = & hist[c0][c1][c2min];
	       for (c2 = c2min; c2 <= c2max; c2++)
	           if (*histp++ != 0) {
	              boxp->c0max = c0max = c0;
	              goto have_c0max;
	           }
               }

  have_c0max:
    if (c1max > c1min)
       for (c1 = c1min; c1 <= c1max; c1++)
           for (c0 = c0min; c0 <= c0max; c0++) {
	       histp = & hist[c0][c1][c2min];
	       for (c2 = c2min; c2 <= c2max; c2++)
	           if (*histp++ != 0) {
	              boxp->c1min = c1min = c1;
	              goto have_c1min;
	           }
               }

  have_c1min:
    if (c1max > c1min)
       for (c1 = c1max; c1 >= c1min; c1--)
           for (c0 = c0min; c0 <= c0max; c0++) {
	       histp = & hist[c0][c1][c2min];
	       for (c2 = c2min; c2 <= c2max; c2++)
	           if (*histp++ != 0) {
	              boxp->c1max = c1max = c1;
	              goto have_c1max;
	           }
               }

  have_c1max:
    if (c2max > c2min)
       for (c2 = c2min; c2 <= c2max; c2++)
           for (c0 = c0min; c0 <= c0max; c0++) {
	       histp = & hist[c0][c1min][c2];
	       for (c1 = c1min; c1 <= c1max; c1++, histp += HIST_C2_ELEMS)
	           if (*histp != 0) {
	              boxp->c2min = c2min = c2;
	              goto have_c2min;
	           }
               }

  have_c2min:
    if (c2max > c2min)
       for (c2 = c2max; c2 >= c2min; c2--)
           for (c0 = c0min; c0 <= c0max; c0++) {
	       histp = & hist[c0][c1min][c2];
	       for (c1 = c1min; c1 <= c1max; c1++, histp += HIST_C2_ELEMS)
	           if (*histp != 0) {
	              boxp->c2max = c2max = c2;
	              goto have_c2max;
	           }
               }

  have_c2max:

  dist0 = ((c0max - c0min) << C0_SHIFT) * C0_SCALE;
  dist1 = ((c1max - c1min) << C1_SHIFT) * C1_SCALE;
  dist2 = ((c2max - c2min) << C2_SHIFT) * C2_SCALE;
  boxp->volume = dist0 * dist0 + dist1 * dist1 + dist2 * dist2;
  
  ccount = 0;
  for (c0 = c0min; c0 <= c0max; c0++)
      for (c1 = c1min; c1 <= c1max; c1++) {
          histp = & hist[c0][c1][c2min];
          for (c2 = c2min; c2 <= c2max; c2++, histp++)
	      if (*histp != 0) {
	         ccount++;
	      }
          }
  boxp->colorcount = ccount;
}


static int
median_cut (boxlist, numboxes, desired_colors)
boxptr boxlist;
int numboxes, desired_colors;
{
  int n, lb;
  int c0, c1, c2, cmax;
  register boxptr b1, b2;

  while (numboxes < desired_colors) {
    /* Select box to split.
     * Current algorithm: by population for first half, then by volume.
     */
    if (numboxes * 2 <= desired_colors) 
       b1 = find_biggest_color_pop(boxlist, numboxes);
    else 
       b1 = find_biggest_volume(boxlist, numboxes);
    
    if (b1 == NULL)		/* no splittable boxes left! */
       break;
    b2 = &boxlist[numboxes];	/* where new box will go */
    /* Copy the color bounds to the new box. */
    b2->c0max = b1->c0max; b2->c1max = b1->c1max; b2->c2max = b1->c2max;
    b2->c0min = b1->c0min; b2->c1min = b1->c1min; b2->c2min = b1->c2min;
    /* Choose which axis to split the box on.
     */
    c0 = ((b1->c0max - b1->c0min) << C0_SHIFT) * C0_SCALE;
    c1 = ((b1->c1max - b1->c1min) << C1_SHIFT) * C1_SCALE;
    c2 = ((b1->c2max - b1->c2min) << C2_SHIFT) * C2_SCALE;
    cmax = c1; n = 1;
    if (c0 > cmax) {
       cmax = c0;
       n = 0;
    }
    if (c2 > cmax)
       n = 2; 
    switch (n) {
    case 0:
      lb = (b1->c0max + b1->c0min) / 2;
      b1->c0max = lb;
      b2->c0min = lb + 1;
      break;
    case 1:
      lb = (b1->c1max + b1->c1min) / 2;
      b1->c1max = lb;
      b2->c1min = lb + 1;
      break;
    case 2:
      lb = (b1->c2max + b1->c2min) / 2;
      b1->c2max = lb;
      b2->c2min = lb + 1;
      break;
    }
    /* Update stats for boxes */
    update_box(b1);
    update_box(b2);
    numboxes++;
  }
  return numboxes;
}


static void
compute_color (boxp, icolor)
boxptr boxp;
int icolor;
{
  /* Current algorithm: mean weighted by pixels (not colors) */
  /* Note it is important to get the rounding correct! */
  hist2d * hist = histogram;
  histptr histp;
  int c0, c1, c2;
  int c0min, c0max, c1min, c1max, c2min, c2max;
  long count;
  long total = 0;
  long c0total = 0;
  long c1total = 0;
  long c2total = 0;
  
  c0min = boxp->c0min;  c0max = boxp->c0max;
  c1min = boxp->c1min;  c1max = boxp->c1max;
  c2min = boxp->c2min;  c2max = boxp->c2max;
  
  for (c0 = c0min; c0 <= c0max; c0++)
      for (c1 = c1min; c1 <= c1max; c1++) {
          histp = & hist[c0][c1][c2min];
          for (c2 = c2min; c2 <= c2max; c2++) {
	      if ((count = *histp++) != 0) {
	         total += count;
	         c0total += ((c0 << C0_SHIFT) + ((1<<C0_SHIFT)>>1)) * count;
	         c1total += ((c1 << C1_SHIFT) + ((1<<C1_SHIFT)>>1)) * count;
	         c2total += ((c2 << C2_SHIFT) + ((1<<C2_SHIFT)>>1)) * count;
  	      }
          }
      }
  
  colormap[0][icolor] = (JSAMPLE) ((c0total + (total>>1)) / total);
  colormap[1][icolor] = (JSAMPLE) ((c1total + (total>>1)) / total);
  colormap[2][icolor] = (JSAMPLE) ((c2total + (total>>1)) / total);
}


/* Master routine for color selection */
static void
select_colors (descolors)
int descolors;
{
  box boxlist[MAXNUMCOLORS];
  int numboxes;
  int i;

  /* Initialize one box containing whole space */
  numboxes = 1;
  boxlist[0].c0min = 0;
  boxlist[0].c0max = 255 >> C0_SHIFT;
  boxlist[0].c1min = 0;
  boxlist[0].c1max = 255 >> C1_SHIFT;
  boxlist[0].c2min = 0;
  boxlist[0].c2max = 255 >> C2_SHIFT;
  /* Shrink it to actually-used volume and set its statistics */
  update_box(& boxlist[0]);
  /* Perform median-cut to produce final box list */
  numboxes = median_cut(boxlist, numboxes, descolors);
  /* Compute the representative color for each box, fill colormap */
  for (i = 0; i < numboxes; i++)
      compute_color(& boxlist[i], i);
  num_colors = numboxes;
}


/* log2(histogram cells in update box) for each axis; this can be adjusted */
#define BOX_C0_LOG  (HIST_C0_BITS-3)
#define BOX_C1_LOG  (HIST_C1_BITS-3)
#define BOX_C2_LOG  (HIST_C2_BITS-3)

#define BOX_C0_ELEMS  (1<<BOX_C0_LOG) /* # of hist cells in update box */
#define BOX_C1_ELEMS  (1<<BOX_C1_LOG)
#define BOX_C2_ELEMS  (1<<BOX_C2_LOG)

#define BOX_C0_SHIFT  (C0_SHIFT + BOX_C0_LOG)
#define BOX_C1_SHIFT  (C1_SHIFT + BOX_C1_LOG)
#define BOX_C2_SHIFT  (C2_SHIFT + BOX_C2_LOG)


static int
find_nearby_colors (minc0, minc1, minc2, colorlist)
int minc0, minc1, minc2;
JSAMPLE colorlist[];
{
  int numcolors = num_colors;
  int maxc0, maxc1, maxc2;
  int centerc0, centerc1, centerc2;
  int i, x, ncolors;
  long minmaxdist, min_dist, max_dist, tdist;
  long mindist[MAXNUMCOLORS];	/* min distance to colormap entry i */

  maxc0 = minc0 + ((1 << BOX_C0_SHIFT) - (1 << C0_SHIFT));
  centerc0 = (minc0 + maxc0) >> 1;
  maxc1 = minc1 + ((1 << BOX_C1_SHIFT) - (1 << C1_SHIFT));
  centerc1 = (minc1 + maxc1) >> 1;
  maxc2 = minc2 + ((1 << BOX_C2_SHIFT) - (1 << C2_SHIFT));
  centerc2 = (minc2 + maxc2) >> 1;

  minmaxdist = 0x7FFFFFFFL;

  for (i = 0; i < numcolors; i++) {
      /* We compute the squared-c0-distance term, then add in the other two. */
      x = colormap[0][i];
      if (x < minc0) {
         tdist = (x - minc0) * C0_SCALE;
         min_dist = tdist * tdist;
         tdist = (x - maxc0) * C0_SCALE;
         max_dist = tdist * tdist;
      }
      else if (x > maxc0) {
              tdist = (x - maxc0) * C0_SCALE;
              min_dist = tdist * tdist;
              tdist = (x - minc0) * C0_SCALE;
              max_dist = tdist * tdist;
      }
      else {
           /* within cell range so no contribution to min_dist */
           min_dist = 0;
           if (x <= centerc0) {
	      tdist = (x - maxc0) * C0_SCALE;
	      max_dist = tdist * tdist;
           }
	   else {
	        tdist = (x - minc0) * C0_SCALE;
	        max_dist = tdist * tdist;
           }
      }

      x = colormap[1][i];
      if (x < minc1) {
         tdist = (x - minc1) * C1_SCALE;
         min_dist += tdist * tdist;
         tdist = (x - maxc1) * C1_SCALE;
         max_dist += tdist * tdist;
      }
      else if (x > maxc1) {
              tdist = (x - maxc1) * C1_SCALE;
              min_dist += tdist * tdist;
              tdist = (x - minc1) * C1_SCALE;
              max_dist += tdist * tdist;
      }
      else {
           /* within cell range so no contribution to min_dist */
           if (x <= centerc1) {
	      tdist = (x - maxc1) * C1_SCALE;
	      max_dist += tdist * tdist;
           }
	   else {
	        tdist = (x - minc1) * C1_SCALE;
	        max_dist += tdist * tdist;
           }
      }

      x = colormap[2][i];
      if (x < minc2) {
         tdist = (x - minc2) * C2_SCALE;
         min_dist += tdist * tdist;
         tdist = (x - maxc2) * C2_SCALE;
         max_dist += tdist * tdist;
      }
      else if (x > maxc2) {
              tdist = (x - maxc2) * C2_SCALE;
              min_dist += tdist * tdist;
              tdist = (x - minc2) * C2_SCALE;
              max_dist += tdist * tdist;
      }
      else {
           /* within cell range so no contribution to min_dist */
           if (x <= centerc2) {
    	      tdist = (x - maxc2) * C2_SCALE;
	      max_dist += tdist * tdist;
           }
	   else {
	        tdist = (x - minc2) * C2_SCALE;
	        max_dist += tdist * tdist;
           }
      }

      mindist[i] = min_dist;	/* save away the results */
      if (max_dist < minmaxdist)
         minmaxdist = max_dist;
  }

  ncolors = 0;
  for (i = 0; i < numcolors; i++) {
      if (mindist[i] <= minmaxdist)
         colorlist[ncolors++] = (JSAMPLE) i;
  }
  return ncolors;
}


static void
find_best_colors (minc0, minc1, minc2, numcolors, colorlist, bestcolor)
int minc0, minc1, minc2, numcolors;
JSAMPLE colorlist[], bestcolor[];
{
  int ic0, ic1, ic2;
  int i, icolor;
  register long *bptr;	/* pointer into bestdist[] array */
  JSAMPLE * cptr;		/* pointer into bestcolor[] array */
  long dist0, dist1;		/* initial distance values */
  register long dist2;		/* current distance in inner loop */
  long xx0, xx1;		/* distance increments */
  register long xx2;
  long inc0, inc1, inc2;	/* initial values for increments */
  /* This array holds the distance to the nearest-so-far color for each cell */
  long bestdist[BOX_C0_ELEMS * BOX_C1_ELEMS * BOX_C2_ELEMS];

  /* Initialize best-distance for each cell of the update box */
  bptr = bestdist;
  for (i = BOX_C0_ELEMS*BOX_C1_ELEMS*BOX_C2_ELEMS-1; i >= 0; i--)
    *bptr++ = 0x7FFFFFFFL;
  
  /* Nominal steps between cell centers ("x" in Thomas article) */
#define STEP_C0  ((1 << C0_SHIFT) * C0_SCALE)
#define STEP_C1  ((1 << C1_SHIFT) * C1_SCALE)
#define STEP_C2  ((1 << C2_SHIFT) * C2_SCALE)
  
  for (i = 0; i < numcolors; i++) {
      icolor = colorlist[i];
      /* Compute (square of) distance from minc0/c1/c2 to this color */
      inc0 = (minc0 - (int) colormap[0][icolor]) * C0_SCALE;
      dist0 = inc0 * inc0;
      inc1 = (minc1 - (int) colormap[1][icolor]) * C1_SCALE;
      dist0 += inc1 * inc1;
      inc2 = (minc2 - (int) colormap[2][icolor]) * C2_SCALE;
      dist0 += inc2 * inc2;
      /* Form the initial difference increments */
      inc0 = inc0 * (2 * STEP_C0) + STEP_C0 * STEP_C0;
      inc1 = inc1 * (2 * STEP_C1) + STEP_C1 * STEP_C1;
      inc2 = inc2 * (2 * STEP_C2) + STEP_C2 * STEP_C2;
      /* Now loop over all cells in box, updating distance per Thomas method */
      bptr = bestdist;
      cptr = bestcolor;
      xx0 = inc0;
      for (ic0 = BOX_C0_ELEMS-1; ic0 >= 0; ic0--) {
          dist1 = dist0;
          xx1 = inc1;
          for (ic1 = BOX_C1_ELEMS-1; ic1 >= 0; ic1--) {
	      dist2 = dist1;
	      xx2 = inc2;
	      for (ic2 = BOX_C2_ELEMS-1; ic2 >= 0; ic2--) {
	          if (dist2 < *bptr) {
	             *bptr = dist2;
	             *cptr = (JSAMPLE) icolor;
	          }
	          dist2 += xx2;
	          xx2 += 2 * STEP_C2 * STEP_C2;
	          bptr++;
	          cptr++;
	      }
	      dist1 += xx1;
	      xx1 += 2 * STEP_C1 * STEP_C1;
          }
          dist0 += xx0;
          xx0 += 2 * STEP_C0 * STEP_C0;
      }
  }
}


static void
fill_inverse_cmap (c0, c1, c2)
int c0, c1, c2;
{
  hist2d * hist = histogram;
  int minc0, minc1, minc2;	/* lower left corner of update box */
  int ic0, ic1, ic2;
  register JSAMPLE *cptr;	/* pointer into bestcolor[] array */
  register histptr cachep;	/* pointer into main cache array */
  /* This array lists the candidate colormap indexes. */
  JSAMPLE colorlist[MAXNUMCOLORS];
  int numcolors;		/* number of candidate colors */
  /* This array holds the actually closest colormap index for each cell. */
  JSAMPLE bestcolor[BOX_C0_ELEMS * BOX_C1_ELEMS * BOX_C2_ELEMS];

  /* Convert cell coordinates to update box ID */
  c0 >>= BOX_C0_LOG;
  c1 >>= BOX_C1_LOG;
  c2 >>= BOX_C2_LOG;

  minc0 = (c0 << BOX_C0_SHIFT) + ((1 << C0_SHIFT) >> 1);
  minc1 = (c1 << BOX_C1_SHIFT) + ((1 << C1_SHIFT) >> 1);
  minc2 = (c2 << BOX_C2_SHIFT) + ((1 << C2_SHIFT) >> 1);
  
  numcolors = find_nearby_colors(minc0, minc1, minc2, colorlist);

  /* Determine the actually nearest colors. */
  find_best_colors(minc0, minc1, minc2, numcolors, colorlist, bestcolor);

  /* Save the best color numbers (plus 1) in the main cache array */
  c0 <<= BOX_C0_LOG;		/* convert ID back to base cell indexes */
  c1 <<= BOX_C1_LOG;
  c2 <<= BOX_C2_LOG;
  cptr = bestcolor;
  for (ic0 = 0; ic0 < BOX_C0_ELEMS; ic0++) {
      for (ic1 = 0; ic1 < BOX_C1_ELEMS; ic1++) {
          cachep = & hist[c0+ic0][c1+ic1][c2];
          for (ic2 = 0; ic2 < BOX_C2_ELEMS; ic2++) {
	      *cachep++ = (histcell) (*cptr++ + 1);
          }
      
      }
  }
}


static void
map_pixels (pic24, width, height, pic8)
byte *pic24, *pic8;
int width, height;
{
  register LOCFSERROR cur0, cur1, cur2;	/* current error or pixel value */
  LOCFSERROR belowerr0, belowerr1, belowerr2; /* error for pixel below cur */
  LOCFSERROR bpreverr0, bpreverr1, bpreverr2; /* error for below/prev col */
  register FSERRPTR errorptr;	/* => fserrors[] at column before current */
  JSAMPROW inptr;		/* => current input pixel */
  JSAMPROW outptr;		/* => current output pixel */
  histptr cachep;
  int dir;			/* +1 or -1 depending on direction */
  int dir3;			/* 3*dir, for advancing inptr & errorptr */
  int row, col;
  int *error_limit = error_limiter;
  JSAMPROW colormap0 = colormap[0];
  JSAMPROW colormap1 = colormap[1];
  JSAMPROW colormap2 = colormap[2];
  hist2d * hist = histogram;

  for (row = 0; row < height; row++) {
      inptr = & pic24[row * width * 3];
      outptr = & pic8[row * width];
      if (on_odd_row) {
         /* work right to left in this row */
         inptr  += (width - 1) * 3;	/* so point to rightmost pixel */
         outptr += width - 1;
         dir  = -1;
         dir3 = -3;
         errorptr = fserrors + (width + 1) * 3;/* => entry after last column */
         on_odd_row = FALSE;	/* flip for next time */
      }
      else {
           /* work left to right in this row */
           dir  = 1;
           dir3 = 3;
           errorptr = fserrors;	/* => entry before first real column */
           on_odd_row = TRUE;	/* flip for next time */
      }
      /* Preset error values: no error propagated to first pixel from left */
      cur0 = cur1 = cur2 = 0;
      /* and no error propagated to row below yet */
      belowerr0 = belowerr1 = belowerr2 = 0;
      bpreverr0 = bpreverr1 = bpreverr2 = 0;

      for (col = width; col > 0; col--) {
          cur0 = (cur0 + errorptr[dir3 + 0] + 8) >> 4;
          cur1 = (cur1 + errorptr[dir3 + 1] + 8) >> 4;
          cur2 = (cur2 + errorptr[dir3 + 2] + 8) >> 4;
          cur0 = error_limit[cur0];
          cur1 = error_limit[cur1];
          cur2 = error_limit[cur2];
          cur0 += inptr[0];
          cur1 += inptr[1];
          cur2 += inptr[2];
          RANGE(cur0, 0, 255);
          RANGE(cur1, 0, 255);
          RANGE(cur2, 0, 255);
          /* Index into the cache with adjusted pixel value */
          cachep = & hist[cur0>>C0_SHIFT][cur1>>C1_SHIFT][cur2>>C2_SHIFT];
          /* If we have not seen this color before, find nearest colormap */
          /* entry and update the cache */
          if (*cachep == 0)
	  fill_inverse_cmap(cur0>>C0_SHIFT, cur1>>C1_SHIFT, cur2>>C2_SHIFT);
          /* Now emit the colormap index for this cell */
          { register int pixcode = *cachep - 1;
	    *outptr = (JSAMPLE) pixcode;
	    /* Compute representation error for this pixel */
	    cur0 -= (int) colormap0[pixcode];
	    cur1 -= (int) colormap1[pixcode];
	    cur2 -= (int) colormap2[pixcode];
          }
          /* Compute error fractions to be propagated to adjacent pixels.
           * Add these into the running sums, and simultaneously shift the
           * next-line error sums left by 1 column.
           */
          { register LOCFSERROR bnexterr, delta;

	    bnexterr = cur0;	/* Process component 0 */
	    delta = cur0 * 2;
	    cur0 += delta;		/* form error * 3 */
	    errorptr[0] = (FSERROR) (bpreverr0 + cur0);
	    cur0 += delta;		/* form error * 5 */
	    bpreverr0 = belowerr0 + cur0;
	    belowerr0 = bnexterr;
	    cur0 += delta;		/* form error * 7 */
	    bnexterr = cur1;	/* Process component 1 */
	    delta = cur1 * 2;
	    cur1 += delta;		/* form error * 3 */
	    errorptr[1] = (FSERROR) (bpreverr1 + cur1);
	    cur1 += delta;		/* form error * 5 */
	    bpreverr1 = belowerr1 + cur1;
	    belowerr1 = bnexterr;
	    cur1 += delta;		/* form error * 7 */
	    bnexterr = cur2;	/* Process component 2 */
	    delta = cur2 * 2;
	    cur2 += delta;		/* form error * 3 */
	    errorptr[2] = (FSERROR) (bpreverr2 + cur2);
	    cur2 += delta;		/* form error * 5 */
 	    bpreverr2 = belowerr2 + cur2;
	    belowerr2 = bnexterr;
	    cur2 += delta;		/* form error * 7 */
          }
          /* At this point curN contains the 7/16 error value to be propagated
           * to the next pixel on the current line, and all the errors for the
           * next line have been shifted over. We are therefore ready to move
	   * on.
           */
          inptr += dir3;	/* Advance pixel pointers to next column */
          outptr += dir;
          errorptr += dir3;	/* advance errorptr to current column */
      }

      /* Post-loop cleanup: we must unload the final error values into the
       * final fserrors[] entry.  Note we need not unload belowerrN because
       * it is for the dummy column before or after the actual array.
       */
      errorptr[0] = (FSERROR) bpreverr0; /* unload prev errs into array */
      errorptr[1] = (FSERROR) bpreverr1;
      errorptr[2] = (FSERROR) bpreverr2;
  }
}


static void
init_error_limit ()
/* Allocate and fill in the error_limiter table */
/* Note this should be done only once. */
{
  int *table;
  int in, out;

  table = (int *) ckalloc((unsigned) ((255 * 2 + 1) * sizeof(int)));
  if (! table) return;

  table += 255;		/* so can index -255 .. +255 */
  error_limiter = table;

#define STEPSIZE ((255 + 1) / 16)
  /* Map errors 1:1 up to +- 255/16 */
  out = 0;
  for (in = 0; in < STEPSIZE; in++, out++) {
      table[in] = out;
      table[-in] = -out;
  }
 
  /* Map errors 1:2 up to +- 3*255/16 */
  for (; in < STEPSIZE*3; in++, out += (in&1) ? 0 : 1) {
      table[in] = out;
      table[-in] = -out;
  }
  /* Clamp the rest to final out value (which is (255+1)/8) */
  for (; in <= 255; in++) { 
      table[in] = out;
      table[-in] = -out;
  }

#undef STEPSIZE

}
