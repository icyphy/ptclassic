defstar {
	name { TkImageDisplay }
	domain { SDF }
	derived { TclScript }
	version { $Id$ }
	author { Mei Xiao }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF Tcl/Tk library }
	desc {
Invoke a Tcl/Tk script to render an image by adapting to the state
of the color map for robust image rendering.
	}
	explanation { 
.IR "image rendering"
When the star has one input port, the star treats inputs matrices as
black-and-white images.  When the star has three input ports, the
star treats the matrices as given the red, green, blue intensities,
respectively, of a color image.
	}
	state {
		name { ptimage_name }
		type { string }
		default { "Img" }
		desc {
The base name given to create a ptimage image for the image data in memory
		}
		attributes { A_CONSTANT|A_NONSETTABLE }
	}
	hinclude { "Matrix.h", "InfString.h" }
	header {
extern "C" {
#include "ptkImage.h"
}

#define GRAYSCALE 1
#define FULLCOLOR 2
/* maximum # of color entries in colormap, 256 for PseudoColor Visual */
#define NUM_COLORCELLS 256
	}

	public {
		int width, height, colorChoice;
	}

	protected {
		int numInputs;

		// Static variable to help create unique names.
		static int starSuffix;
		static int frameSuffix; 

		// A unique string for each instance of this star class.
		InfString dispStarID;

		// A unique string for each instance of the image this
		// star instance displays
		InfString frameID;

		// A pointer to the image of type "ptimage", whose name
		// is Img_$dispStarID$frameID
		PtimageMaster* masterPtr;

		// Full name of the image, i.e. it has a base name, "Img"
		// plus dispStarID "dispStar#" plus frameId "frame#"
		InfString fullnm;
	}

	code {
		int SDFTkImageDisplay::starSuffix = 0;   
		int SDFTkImageDisplay::frameSuffix = 0;
	}

	constructor {
		// Reset data type of input port to be floating-point matrix
		input.setPort("output", this, FLOAT_MATRIX_ENV);

		dispStarID = "Display";
		dispStarID += ++starSuffix;
	}

	destructor {
		// Reset member starSuffix back to zero so that things don't
		// go wrong in next simulation
		starSuffix = 0;
	}
 
	setup {
		if (output.numberPorts() > 0) {
		    Error::abortRun(*this, "Outputs not supported");
		    return;
		}
	}

	begin {
		tcl_file = "$PTOLEMY/src/domains/sdf/tcltk/stars/tkImageDisplay.tcl";

		// Make sure the image has a unique ID when this run starts
		frameID = "_Frame";
		frameID += ++frameSuffix;

		numInputs = input.numberPorts();

		// Determine the number of the input ports 
		colorChoice = 0;
		if (numInputs == 1) colorChoice = GRAYSCALE;
		else if (numInputs == 3) colorChoice = FULLCOLOR;
		else {
		    Error::abortRun(*this,
				    "The number of input ports must ",
				    "be 1 for grayscale image, or 3 ",
				    "for full color image.");
		    return;
		}

		// Set up business for a ptimage that the star is going to use 

		// I --Create an empty Tk's "ptimage" image, named
		//     Img_$dispStarID$frameID, if it does not exist yet,
		//     otherwise skip the creation step. Then get the
		//     pointer to the master of this image. 
		fullnm = (const char *)ptimage_name;
		fullnm += "_";
		fullnm += dispStarID;
		fullnm += frameID;
		InfString command = "image type ";
		command += fullnm;
		if (Tcl_GlobalEval(ptkInterp, command) != TCL_OK) {
		    // which means the image doesn't exist yet, so create it
		    InfString command = "image create ptimage";
		    command += " ";
		    command += fullnm;
		    if (Tcl_GlobalEval(ptkInterp, command) == TCL_ERROR) {
			Tcl_GlobalEval(ptkInterp, "ptkDisplayErrorInfo");
			Error::abortRun(*this, 
					"Failed to create ptimage image");
			return;
		    }
		}

		//II -- Before sourcing a tcl script, we set global variable
		//      dispStarID and frameID, which will be used by the tcl
		//      script.
		if ( Tcl_SetVar(ptkInterp, "dispStarID", (char *)dispStarID,
				TCL_GLOBAL_ONLY) == NULL) {
		    Error::abortRun(*this, "Failed to set dispStarID");
		    return;
		}
		if ( Tcl_SetVar(ptkInterp, "frameID", (char *)frameID,
				TCL_GLOBAL_ONLY) == NULL) {
		    Error::abortRun(*this, "Failed to set frameID");
		    return;
		}

		// The base star TclScript assumes floating point inputs
		// and outputs which are inappropriate here, so we
		// claim no input and no outputs
		tcl.setup(this, 0, 0, (const char*)tcl_file);
	}
 
	go {
		byte trans[NUM_COLORCELLS];
		byte *newPix8 = 0, *pix8 = 0, *pix24 = 0;

		// Allocate space to hold R, G, B color table entries
		byte* rMap = (byte*) malloc(sizeof(byte) * NUM_COLORCELLS); 
		byte* gMap = (byte*) malloc(sizeof(byte) * NUM_COLORCELLS); 
		byte* bMap = (byte*) malloc(sizeof(byte) * NUM_COLORCELLS); 

		// An array to hold the "best" allocating order for
		// the image's colors 
		byte* allocOrder = (byte*) malloc(sizeof(byte) * NUM_COLORCELLS);

		// Generate image pixel data appropriately from matrix to get
		// ready for colormap manipulation and image display
		if (colorChoice == GRAYSCALE) {
		    Envelope GrayPkt;
		    MPHIter mphiter(input);
		    PortHole *p = mphiter++;
		    ((*p)%0).getMessage(GrayPkt);
		    const FloatMatrix& GrayMatrix =
			*(const FloatMatrix *)GrayPkt.myData();
		    width  = GrayMatrix.numCols();
		    height = GrayMatrix.numRows();
		    int size = width * height;

		    // Figure out the maximum entry in the matrix
		    int maxValue = FM_maxEntry(GrayMatrix, size);

		    // Convert the maxtrix to image's pixel data;
		    // pix8 will store the original information.
		    pix8 = (byte *) malloc(sizeof(byte) * size);
		    byte *nextPixel = pix8;
		    for (int i = 0; i < size; i++) {
			if (GrayMatrix.entry(i) < 0) {
			    *nextPixel++ = 0;
			}
			else if (maxValue < NUM_COLORCELLS) {
			    *nextPixel++ = int(GrayMatrix.entry(i));
			}
			else {
			    *nextPixel++ = int(GrayMatrix.entry(i) *
					       (NUM_COLORCELLS - 1) / maxValue);
			}
		    }

		    // Create a place which will later store pixel data after
		    // colormap manipulation: 
		    newPix8 = (byte *) malloc(sizeof(byte) * size);
		    byte *pixels = pix8, *npixels = newPix8;
		    for (int j = 0; j < height; j++)
		        for (int m = 0; m < width; m++)
			    *npixels++ = *pixels++;

		    // Fill in the RGB color tables for this grayscale image
		    for (i = 0; i < NUM_COLORCELLS; i++) {
		        rMap[i] = gMap[i] = bMap[i] = i;
		    }
		}
		else {				// Must be FULLCOLOR
		    int wide[3], high[3], maxv = 0, size = 0;

		    // Go through RGB input matrices to check their dimensions
		    MPHIter nexti(input);
		    for (int i = 0; i < numInputs; i++) {    // numInputs = 3
		        Envelope Component;
			PortHole* p = nexti++;
			((*p)%0).getMessage(Component);
		        const FloatMatrix& RGBMatrix =
				*(const FloatMatrix *)Component.myData();
			wide[i] = RGBMatrix.numCols();
			high[i] = RGBMatrix.numRows();
			if (i > 0 && (wide[i] != wide[i-1] ||
				      high[i] != high[i-1])) {
			    Error::abortRun(*this,
					"The matrices representing the ",
					"R,G,B components must have the ",
					"same dimensions.");
			    return;
			}
			width = wide[i];
			height = high[i];
			size = wide[i] * high[i];
			int maxValue = FM_maxEntry(RGBMatrix, size);
			if ( maxValue > maxv ) maxv = maxValue;
		    }

		    // Copy the values of the RGB matrices into one array
		    pix24 = (byte *) malloc(sizeof(byte) * size * 3);
		    nexti.reset();
		    for (int j = 0; j < numInputs; j++) {    // numInputs = 3
		        Envelope Component;
			PortHole* p = nexti++;
			((*p)%0).getMessage(Component);
		        const FloatMatrix& RGBMatrix =
				*(const FloatMatrix *)Component.myData();
		    	byte* nextPixel = &pix24[j];
			for (int m = 0; m < size; m++) {
			    if ( RGBMatrix.entry(i) < 0 )
				*nextPixel = 0;
			    else if (maxv > 0.9*NUM_COLORCELLS &&
				     maxv < NUM_COLORCELLS)
				*nextPixel = int(RGBMatrix.entry(i));
			    else
				*nextPixel = int(RGBMatrix.entry(i) *
					    (NUM_COLORCELLS - 1) / maxv);
			    nextPixel += numInputs;
			}
		    }

		    // Do 24-bit to 8-bit conversion to generate good colormap
		    // for the non-colormapped 24-bit RGB image data, using
		    // Heckbert's Median Cut algorithm
		    newPix8 = Tk_PtConv24to8(pix24, width, height,
					     NUM_COLORCELLS, rMap, gMap, bMap);
		} // end of FULLCOLOR
 
		// Sort desired colormap in order to argue with X for better
		// use of the scarce color resource
		int numColors;
		Tk_PtSortColormap(newPix8, width, height, &numColors,
				  rMap, gMap, bMap, allocOrder, trans);
		Tk_PtColorCompress8(newPix8, trans, width, height);

		// FIXME: What the heck is this mess?  Looks like a kludge.
		// Configure the image's master; and first of all, free some
		// previously allocated memory when needed
		masterPtr = (PtimageMaster *) Tk_FindPtimage((char *) fullnm);
		if (!masterPtr->pix8) free(masterPtr->pix8);
		if (!masterPtr->newPix8) free(masterPtr->newPix8);
		if (!masterPtr->pix24) free(masterPtr->pix24);
		if (!masterPtr->rMap) free(masterPtr->rMap);
		if (!masterPtr->gMap) free(masterPtr->gMap);
		if (!masterPtr->bMap) free(masterPtr->bMap);
		if (!masterPtr->allocOrder) free(masterPtr->allocOrder);

		masterPtr->width = width;
		masterPtr->height = height;
		masterPtr->formatFlag = colorChoice;
		masterPtr->pix8 = pix8;
		masterPtr->newPix8 = newPix8;
		masterPtr->pix24 = pix24;
		masterPtr->rMap = rMap;
		masterPtr->gMap = gMap;
		masterPtr->bMap = bMap;
		masterPtr->allocOrder = allocOrder;
		masterPtr->numColors = numColors;
		Tk_ImageChanged(masterPtr->tkMaster, 0, 0, width, height,
				width, height);

		// Inform the generic image code of TK that this image has
		// (potentially) changed:
		masterPtr->flags |= IMAGE_CHANGED;

		// Cycle through all instances of this image, regenerating
		// the information for each instance. Then force the image
		// to be redisplayed everywhere that it is used.
		PtimageInstance *instancePtr;
		for (instancePtr = masterPtr->instancePtr;
		     instancePtr != NULL;
		     instancePtr = instancePtr->nextPtr) {
		    Tk_PtimageConfigureInstance(instancePtr);
		}
		Tk_ImageChanged(masterPtr->tkMaster, 0, 0, width, height,
				width, height);

		// Invoke the "goTcl" procedure in the given Tcl script,
		// if defined, Unset the master's flags
		masterPtr->flags &= ~IMAGE_CHANGED;

		// which *has to be* true for this display star
		InfString command = "info procs goTcl_";
		command += dispStarID;
		if (Tcl_GlobalEval(ptkInterp, (char *)command) == TCL_OK &&
		    strlen(ptkInterp->result) != 0)
		    // then a goTcl_dispStarID procedure is defined
		    callTclProc("goTcl");

		tcl.go();
	}

	// It is for invoking a Tcl procedure *procnm_dispStarID, which is
	// assumed to be defined, with two arguments dispStarID and frameID 
	method {
		name { callTclProc }
		arglist { "(const char* procnm)" }
		type { void }
		code {
			InfString command = procnm;
			command += "_";
			command += dispStarID;
			command += " ";
			command += dispStarID;
			command += " ";
			command += frameID;
			if (Tcl_GlobalEval(ptkInterp, command) != TCL_OK) {
			    Error::abortRun(*this,
					    "Failed to run Tcl procedure ",
					    command);
			}
			return;
	       }
	}

	method {
		name { FM_maxEntry }
		arglist { "(const FloatMatrix& Matrix, int size)" }
		type { int }
		code {
			int maxv = 0;
			for (int i = 0; i < size; i++) {
			    if (Matrix.entry(i) > maxv)
				maxv = int(Matrix.entry(i));
		    	}
			return maxv;
		}
	}
}
