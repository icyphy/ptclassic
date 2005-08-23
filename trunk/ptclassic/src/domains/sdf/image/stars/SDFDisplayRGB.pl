defstar {
	name		{ DisplayRGB }
	domain		{ SDF }
	version		{ @(#)SDFDisplayRGB.pl	1.31 01 Oct 1996 }
	author		{ Sun-Inn Shih }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Accept three color images (red, green, and blue) from three input
floating-point matrices and generate a color image file in Portable
Pixmap (PPM) format.  Send the filename to a user-specified command 
(by default, xv).

The user can set the root filename of the displayed image (which will
probably be printed in the image display window title bar) and
can choose whether or not the image file is saved or deleted.
The frameId of the received image is appended to the root filename
to produce the full filename of the displayed image.
	}
	htmldoc {
<a name="image display, red-green-blue (RGB)"></a>
<a name="RGB image display"></a>
<a name="PPM image format"></a>
<a name="image format, portable pixmap (PPM)"></a>
	}

	ccinclude {
		"Matrix.h" , <std.h> , <stdio.h>,  "Error.h"
	}

	input { name { rinput } type { FLOAT_MATRIX_ENV } }
	input { name { ginput } type { FLOAT_MATRIX_ENV } }
	input { name { binput } type { FLOAT_MATRIX_ENV } }
        input { name { frameIdIn } type {int} }
        

	defstate {
		name { command }
		type { string }
		default { "xv" }
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
		// Read data from inputs.
                Envelope rpkt,gpkt,bpkt;
                (rinput%0).getMessage(rpkt);
                (ginput%0).getMessage(gpkt);
                (binput%0).getMessage(bpkt);
                const FloatMatrix& rimage = *(const FloatMatrix*)rpkt.myData();
                const FloatMatrix& gimage = *(const FloatMatrix*)gpkt.myData();
                const FloatMatrix& bimage = *(const FloatMatrix*)bpkt.myData();

		int height = rimage.numRows();
		int width  = rimage.numCols();

		// Create an array to read into image data.
		unsigned char* rgbfp = new unsigned char[3*height*width];

		int i, j, temp1, temp2, temp3;
		for (i = 0; i < height; i++) {
			temp1 = i*width;
			for (j = 0; j < width; j++){
				temp2 = j+temp1;
				temp3 = 3*temp2;
				rgbfp[temp3] = (unsigned char)(rimage.entry(temp2));
				rgbfp[temp3+1] = (unsigned char)(gimage.entry(temp2));
				rgbfp[temp3+2] = (unsigned char)(bimage.entry(temp2));
			}
		}

		// Set filename and save values.
		const char* saveMe = saveColor;
		int del = !((saveMe[0] == 'y') || (saveMe[0] == 'Y'));

		const char* iname = imageName;
		char* nm = 0;
		if (iname && iname[0]) {
		  nm = expandPathName(iname);
		}
		else {
		  nm = tempFileName();
		}
		StringList fileName = nm;
		fileName << "." << int(frameIdIn%0);
		delete [] nm;

		FILE* fptr = fopen(fileName, "w");
		if (fptr == (FILE*) NULL) {
			Error::abortRun(*this, fileName, ": cannot create");
			return;
		}

		// Write the PPM header and then the data.
		fprintf(fptr, "P6\n %d %d 255\n", width, height);
		fwrite((const char*) rgbfp, sizeof(unsigned char),
				unsigned(3*width*height), fptr);
		fclose(fptr);

		delete [] rgbfp;

		// Build up Unix command to display the image
		StringList buf = "(";
		buf << (const char*) command << " " << fileName;
		if (del) {
			buf << "; rm -f " << fileName;
		}
		buf << ")&";		// Run command in the background
		system(buf);
	} // end go{}
} // end defstar { DisplayRgb }
