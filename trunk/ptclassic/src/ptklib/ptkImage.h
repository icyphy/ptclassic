#ifndef _PTKIMAGE_H
#define _PTKIMAGE_H

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

This file contains declarations for Tk ptimage related things that
are visible outside of the Tk module itself.
 */


/* Taken from ptk.h */

#include "tcl.h"

/* Note that if we are including ptkImage.h in a C++ file, we must include
 * ptkImage.h last because ptkImage.h includes tk.h which eventually includes
 * X11/X.h, which on Solaris2.4 there is a #define Complex 0, which
 * causes no end of trouble.
 */
#include "tk.h"

extern Tcl_Interp *ptkInterp;
extern Tk_Window ptkW;


/* Pull in X window colormap definitions and other data structures */

#include <X11/Xutil.h>


/*
 *------------------------------------------------------------------
 * An new Tk image type, PTIMAGE, is defined here. 
 * The reason for this extension to Tk is that the existing Tk's
 * imaging mechanism is not able to precisely display images on
 * PseudoColor visual model, which is the case with most of nowaday's
 * workstations and other popular machines, and this new image type
 * will try hard to display images in a could-not-be-better manner by
 * cleverly arguing with X server to optimize the use of the scarce color
 * resource on PseudoColor visual model, which is used on most color X
 * displays, meanwhile coping with image * display on TrueColor/DirectColor
 * visual model.  
 *--------------------------------------------------------------------
 *
 * The set of colors required to display a "ptimage" image in a window
 * depends on:
 *      - the visual used by the window, and
 *      - the gamma value for the image.
 *
 * Pixel values allocated for specific colors are valid only for the
 * colormap in which they were allocated.  Sets of pixel values
 * allocated for displaying ptimages are re-used in other windows if
 * possible, that is, if the display, colormap, and gamma values match.
 * A hash table is used to locate these sets of pixel values, using the
 * following data structure as key:
 */

/*
 * The following type is used to identify a particular ptimage image
 * to be manipulated:
 */

typedef void *Tk_PtimageHandle;
typedef unsigned char byte;

/*
 * For a particular (display, colormap, gamma, imgDataChanged) combination,
 * a data structure of the following type is used to store the allocated
 * pixel values and other information:
 */

typedef struct {
    Display *display;                /* Qualifies the colormap resource ID */
    Colormap colormap;               /* Colormap that the windows are using. */
    struct PtimageMaster *masterPtr; /* ColorTable is strongly associated with
                                      * a ptimage's pixel value and gamma
				      * value                               */
} ColorTableId;

typedef struct ColorTable {
    ColorTableId id;            /* Information used in selecting this
                                 * color table */
    int flags;                  /* See below. */
    int refCount;               /* Number of instances using this map. */
    int liveRefCount;           /* Number of instances which are actually
                                 * in use, using this map. */
    XVisualInfo visualInfo;     /* Information about the visual for windows
                                 * using this color table. */
    unsigned long *freeColors;  /* Actual pixel values allocated and
                                 * to be freed later */
    int nFreeColors;            /* Number of colors allocated for this map
                                 * and to be freed later */
    int *FC2PColor;             /* Maps freeColors into the image's pixel
                                 * values  */
    unsigned long *xPixVals;    /* Maps the image's original pixel values to
                                 * X pixel values
                                 */
} ColorTable;

/*
 * Bit definitions for the flags field of a ColorTable.
 * BLACK_AND_WHITE:             1 means only black and white colors are
 *                              available.
 * COLOR_WINDOW:                1 means a full 3-D color cube has been
 *                              allocated.
 * DISPOSE_PENDING:             1 means a call to DisposeColorTable has
 *                              been scheduled as an idle handler, but it
 *                              hasn't been invoked yet.
 * MAP_COLORS:                  1 means pixel values should be mapped
 *                              through pixelMap.
 */

#define BLACK_AND_WHITE         1
#define COLOR_WINDOW            2
#define DISPOSE_PENDING         4
#define MAP_COLORS              8

/*
 * Definition of the data associated with each ptimage image master.
 */

typedef struct PtimageMaster {
    Tk_ImageMaster tkMaster;    /* Tk's token for image master.  NULL means
                                 * the image is being deleted. */
    Tcl_Interp *interp;         /* Interpreter associated with the
                                 * application using this image. */
    Tcl_Command imageCmd;       /* Token for image command (used to delete
                                 * it when the image goes away).  NULL means
                                 * the image command has already been
                                 * deleted. */
    int flags;                  /* Sundry flags, defined below. */
    int width, height;          /* Dimensions of image. */
    double gamma;               /* Display gamma value to correct for. */
    char *fileString;           /* Name of file to read into image. */
    char *dataString;           /* String value to use as contents of image. */
    char *format;               /* User-specified format of data in image
                                 * file or string value. */
    int formatFlag;             /* Flag for image file format, defined below */
    unsigned char *pix8;        /* Local storage for PGM original data */
    unsigned char *pix24;       /* Local storage for PPM original data */
    unsigned char *newPix8;     /* Image data after colormap manipulation */
    unsigned char *newPix24;    /* Actually not necessary */
    int numColors;              /* Number of desired colors */
    byte *rMap;                 /* The desired colormap, which will */
    byte *gMap;                 /* later be used to store sorted    */
    byte *bMap;                 /* R, G, B color-tables             */
    byte *allocOrder;
    struct PtimageInstance *instancePtr;
                                /* First in the list of instances
                                 * associated with this master. */
} PtimageMaster;

/*
 * Bit definitions for the flags field of a PtimageMaster.
 * COLOR_IMAGE:                 1 means that the image has different color
 *                              components.
 * IMAGE_CHANGED:               1 means that the instances of this image
 *                              need to be redithered.
 */

#define COLOR_IMAGE             1
#define IMAGE_CHANGED           2

/*
 * Bit definitions for the formatFlag field of a PtimageMaster.
 * PGM_FORMAT:               1 means that the image file is Portable Graymap.
 * PPM_FORMAT:               2 means that the iamge file is Portable Pixmap.
 */

#define PGM_FORMAT           1
#define PPM_FORMAT           2

/*
 * The following data structure represents all of the instances of
 * a ptimage image in windows on a given screen that are using the
 * same colormap.
 */

typedef struct PtimageInstance {
    PtimageMaster *masterPtr;    /* Pointer to master for image. */
    Display *display;            /* Display for windows using this instance.*/
    Colormap colormap;           /* The image may only be used in windows with
                                  * this particular colormap. */
    struct PtimageInstance *nextPtr;
                                 /* Pointer to the next instance in the list
                                  * of instances associated with this master*/
    int refCount;                /* Number of instances using this structure*/
    double gamma;               /* Gamma value for these instances. */
    ColorTable *colorTablePtr;  /* Pointer to information about colors
                                 * allocated for image display in windows
                                 * like this one. */
    Pixmap pixmap;              /* X pixmap representing this image  */
    int width, height;          /* Dimensions of the pixmap. */
    XImage *imagePtr;           /* Image structure for converted pixels. */
    XVisualInfo visualInfo;     /* Information about the visual that these
                                 * windows are using. */
    GC gc;                      /* Graphics context for writing images
                                 * to the pixmap. */
} PtimageInstance;

/*
 *--------------------------------------------------------------
 *
 * Additional definitions used to manage images of type "ptimage".
 *
 *--------------------------------------------------------------
 */

/*
 * Procedure prototypes and structures used in reading and
 * writing ptimage images:
 */

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
        Tk_PtimageHandle imageHandle));

/*
 * The following structure represents a particular file format for
 * storing images (e.g., PPM, GIF, JPEG, etc.).  It provides information
 * to allow image files of that format to be recognized and read into
 * a ptimage image or a ptimage image to be written into an image file
 * of that format.
 */

struct Tk_PtimageImageFormat {
    char *name;                 /* Name of image file format */
    Tk_PtimageFileMatchProc *fileMatchProc;
                                /* Procedure to call to determine whether
                                 * an image file matches this format. */
    Tk_PtimageStringMatchProc *stringMatchProc;
                                /* Procedure to call to determine whether
                                 * the data in a string matches this format. */
    Tk_PtimageFileReadProc *fileReadProc;
                                /* Procedure to call to read data from
                                 * an image file into a ptimage image. */
    Tk_PtimageStringReadProc *stringReadProc;
                                /* Procedure to call to read data from
                                 * a string into a ptimage image. */
    Tk_PtimageFileWriteProc *fileWriteProc;
                                /* Procedure to call to write data from
                                 * a ptimage image to a file. */
    Tk_PtimageStringWriteProc *stringWriteProc;
                                /* Procedure to call to obtain a string
                                 * representation of the data in a ptimage
                                 * image.*/
    struct Tk_PtimageImageFormat *nextPtr;
                                /* Next in list of all ptimage image formats
                                 * currently known.  Filled in by Tk, not
                                 * by image format handler. */
};

extern void             Tk_CreatePtimageImageFormat _ANSI_ARGS_((
                            Tk_PtimageImageFormat *formatPtr));
extern Tk_PtimageHandle Tk_FindPtimage _ANSI_ARGS_((char *imageName));
extern void Tk_PtimageConfigureInstance _ANSI_ARGS_((
                            PtimageInstance *instancePtr));
extern void             Tk_PtimageSetSize _ANSI_ARGS_((
                            Tk_PtimageHandle handle, int width, int height));
extern void             Tk_PtSortColormap _ANSI_ARGS_((byte *pic8, int width,
                            int height, int *numColors, byte *rmap, byte *gmap,
                            byte *bmap, byte *order, byte *trans));
extern void             Tk_PtColorCompress8 _ANSI_ARGS_((byte *pic8,
                            byte *trans, int width, int height));
extern byte *           Tk_PtConv24to8 _ANSI_ARGS_((byte *pic24, int width,
                            int height, int maxcols, byte *rmap, byte *gmap,
                            byte *bmap));

/*
 * The following was in my version of tkInt.h:
 */

extern Tk_PtimageImageFormat    tkPtImgFmtPPM;
extern Tk_ImageType             tkPtimageImageType;

#endif /* _PTKIMAGE */
