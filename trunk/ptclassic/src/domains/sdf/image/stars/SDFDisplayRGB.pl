defstar {
	name		{ DisplayRGB }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Sun-Inn Shih }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Accept three ColorImages (Red, Green, and Blue) from three input
GrayImages and generate a color image file in Portable Pixmap (PPM) format.
Send the filename to a user-specified command 
(by default,
.EQ
delim off
.EN
"$PTOLEMY/bin.$ARCH/xv"
.EQ
delim $$
.EN
is used).

The user can set the root filename of the displayed image (which will
probably be printed in the image display window title bar) and
can choose whether or not the image file is saved or deleted.
The frameId of the received image is appended to the root filename
to produce the full filename of the displayed image.
	}
	explanation {
.Id "image display, red-green-blue (RGB)"
.Id "RGB image display"
.Ir "PPM image format"
.Ir "image format, portable pixmap (PPM)"
	}

	ccinclude {
		"GrayImage.h" , <std.h> , <stdio.h>,  "Error.h"
	}

// INPUT AND STATES.
	input { name { rinput } type { message } }
	input { name { ginput } type { message } }
	input { name { binput } type { message } }

	defstate {
		name { command }
		type { string }
		default { "$PTOLEMY/bin.$ARCH/xv" }
		desc { Program to run on PGM data }
	}
	defstate {
		name { imageName }
		type { string }
		default { "" }
		desc { If non-null, name for PGM file }
	}
	defstate {
		name { saveColor }
		type { string }
		default { "n" }
		desc { If 'y' or 'Y', then save the file }
	}


	go {
// Read data from input.
		Envelope renvp, genvp, benvp;
		(rinput%0).getMessage(renvp);
		TYPE_CHECK(renvp, "GrayImage");
		(ginput%0).getMessage(genvp);
		TYPE_CHECK(genvp, "GrayImage");
		(binput%0).getMessage(benvp);
		TYPE_CHECK(benvp, "GrayImage");
		const GrayImage* imgR = (const GrayImage*) renvp.myData();
		const GrayImage* imgG = (const GrayImage*) genvp.myData();
		const GrayImage* imgB = (const GrayImage*) benvp.myData();
		if (imgR->fragmented() || imgR->processed() ||
				imgG->fragmented() || imgG->processed() ||
				imgB->fragmented() || imgB->processed()) {
			Error::abortRun(*this,
					"Can't display fragmented or processed images.");
			return;
		}


// Set filename and save values.
		const char* saveMe = saveColor;
		int del = !((saveMe[0] == 'y') || (saveMe[0] == 'Y'));

		char fileName[256]; fileName[0] = '\000';
		if ((const char*) imageName) {
			strcpy(fileName, (const char*) imageName);
		}
		if (fileName[0] == '\000') {
			char* nm = tempFileName();
			strcpy(fileName, nm);
			LOG_DEL; delete nm;
		}
		char numtmp[16];
		sprintf(numtmp, ".%d", imgR->retId());
		strcat(fileName, numtmp);

		FILE* fptr = fopen(fileName, "w");
		if (fptr == (FILE*) NULL) {
			Error::abortRun(*this, fileName, ": can't create");
			return;
		}

// change into RGB format
		int Width = imgR->retWidth();
		int Height = imgR->retHeight();

		unsigned const char* rdata = imgR->constData();
		unsigned const char* gdata = imgG->constData();
		unsigned const char* bdata = imgB->constData();

		LOG_NEW; unsigned char* rgbfp =
				new unsigned char[3*Width*Height];

		int i, j, temp1, temp2, temp3;
		for (i = 0; i < Height; i++) {
			temp1 = i*Width;
			for (j = 0; j < Width; j++){
				temp2 = j+temp1;
				temp3 = 3*temp2;
				rgbfp[temp3] = rdata[temp2];
				rgbfp[temp3+1] = gdata[temp2];
				rgbfp[temp3+2] = bdata[temp2];
		}	}

// Write the PPM header and then the data.
		fprintf(fptr, "P6\n %d %d 255\n", Width, Height);
		fwrite((const char*) rgbfp, sizeof(unsigned char),
				unsigned(3*Width*Height), fptr);
		fclose(fptr);

		LOG_DEL; delete rgbfp;

		char buf[256];
		sprintf (buf, "(%s %s", (const char*) command, fileName);
		if (del) {
			strcat(buf, "; rm -f ");
			strcat(buf, fileName);
		}
// Run command in the background
		strcat (buf, ")&");
		system (buf);
	} // end go{}
} // end defstar { DisplayRgb }
