defstar {
	name		{ DisplayImage }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ J. Buck, Paul Haskell }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Accept a black-and-white input GrayImage and generate
output in PGM format. Send the output to a user-specified command
(by default, "xv" is used). Of course, the specified program must
be in your PATH.

The user can set the root filename of the displayed image (which will
probably be printed in the image display window titlebar) and
can choose whether or not the image file is saved or deleted.
The image frame number is appended to the root filename to form the
complete filename of the displayed image.
	}
	explanation {
.Id "image display"
.Ir "PGM image format"
.Ir "image format, PGM"
}

	ccinclude {
		"GrayImage.h" , <std.h> , <stdio.h>, "Error.h"
	}

// INPUT AND STATES.

	input { name { inData } type { message } }
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

// CODE.
	go {
// Read data from input.
		Envelope envp;
		(inData%0).getMessage(envp);
		TYPE_CHECK(envp, "GrayImage");
		const GrayImage* image = (const GrayImage*) envp.myData();
		if (image->fragmented() || image->processed()) {
			Error::abortRun(*this,
					"Can't display fragmented or processed image.");
			return;
		}

// Set filename and save values.
		const char* saveMe = saveImage;
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
		char numstr[16];
		sprintf(numstr, ".%d", image->retId());
		strcat(fileName, numstr);

		FILE* fptr = fopen(fileName, "w");
		if (fptr == (FILE*) NULL) {
			Error::abortRun(*this, "can not create: ", fileName);
			return;
		}

// Write the PGM header and the data, and then run.
		fprintf (fptr, "P5\n %d %d 255\n", image->retWidth(),
				image->retHeight());
		fwrite((const char*)image->constData(), sizeof(unsigned char),
			(unsigned) image->retWidth() * image->retHeight(), fptr);
		fclose(fptr);

		char cmdbuf[256];
		sprintf (cmdbuf, "(%s %s", (const char*) command, fileName);
		if (del) {
			strcat (cmdbuf, "; rm -f ");
			strcat (cmdbuf, fileName);
		}
		strcat (cmdbuf, ")&");		// Run command in the background
		system (cmdbuf);
	} // end go{}
} // end defstar { DisplayImage }
