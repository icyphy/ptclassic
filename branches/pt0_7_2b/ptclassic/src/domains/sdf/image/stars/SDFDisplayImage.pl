defstar {
	name		{ DisplayImage }
	domain		{ SDF }
	version		{ @(#)SDFDisplayImage.pl	1.30 01 Oct 1996 }
	author		{ Joe Buck, Paul Haskell, and Brian Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Accept a black-and-white input gray-level image represented by a
floating-point matrix and generate output in PGM (portable graymap) format.
Send the output to a user-specified command (by default, "xv" is used).

The user can set the root filename of the displayed image (which will
probably be printed in the image display window titlebar) and
can choose whether or not the image file is saved or deleted.
The image frame number is appended to the root filename to form the
complete filename of the displayed image.
	}
	htmldoc {
<a name="image display"></a>
<a name="PGM image format"></a>
<a name="image format, PGM"></a>
}

	ccinclude {
		"Matrix.h" , <std.h> , <stdio.h>, "Error.h", "StringList.h"
	}

	input { name { dataInput } type { FLOAT_MATRIX_ENV } }
        input { name { frameIdInput } type {int} }

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
		name { saveImage }
		type { string }
		default { "n" }
		desc { If 'y' or 'Y', then save the file }
	}
	go {
		// Read data from input.
                Envelope pkt;
                (dataInput%0).getMessage(pkt);
                const FloatMatrix& matrix = *(const FloatMatrix*)pkt.myData();

		int height = matrix.numRows();
		int width  = matrix.numCols();

		// Create an array to read into image data.
		unsigned char* data = new unsigned char[height*width];

		for (int i=0; i<height*width ; i++) {
			if (matrix.entry(i) < 0)
				data[i] = 0;
			else if(matrix.entry(i) > 255)
				data[i] = 255;
			else data[i] = int(matrix.entry(i)); // convert to int
		}

		// Set filename and save values.
		const char* saveMe = saveImage;
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
		fileName << "." << int(frameIdInput%0);
		delete [] nm;

		FILE* fptr = fopen(fileName, "w");
		if (fptr == (FILE*) NULL) {
		  Error::abortRun(*this, "cannot create: ", fileName);
		  return;
		}

		// Write PGM header and image data (row by row)
		fprintf( fptr,
			 "P5\n %d %d 255\n",
			 width,
			 height );
		fwrite( data,
			width * sizeof(unsigned char),
			height,
			fptr );

		delete [] data;
		fclose(fptr);

		// Display the image using an external viewer
		StringList cmdbuf = "(";
		cmdbuf << ((const char *) command) << " " << fileName;
		if (del) {
		  cmdbuf << "; rm -f " << fileName;
		}
		cmdbuf << ")&";		// Run command in the background
		system(cmdbuf);
	} // end go{}
} // end defstar { DisplayImage }
