defstar {
	name		{ DisplayImage }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ J. Buck, Paul Haskell }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Accept a black-and-white input GrayImage and generate
output in PGM format. Send the output to a user-specified command
(by default,
.EQ
delim off
.EN
"xv"
.EQ
delim $$
.EN
is used).

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
		"GrayImage.h" , <std.h> , <stdio.h>, "Error.h", "StringList.h"
	}
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

	go {
		// Read data from input.
		Envelope envp;
		(inData%0).getMessage(envp);
		TYPE_CHECK(envp, "GrayImage");
		const GrayImage* image = (const GrayImage*) envp.myData();
		if (image->fragmented() || image->processed()) {
		  Error::abortRun(*this,
			"Cannot display fragmented or processed image.");
		  return;
		}

		// Set filename and save values.
		const char* saveMe = saveImage;
		int del = !((saveMe[0] == 'y') || (saveMe[0] == 'Y'));

		StringList fileName;
		const char* iname = imageName;
		if (iname && iname[0]) {
		  fileName = iname;
		}
		else {
		  char* nm = tempFileName();
		  fileName = nm;
		  delete [] nm;
		}
		fileName << "." << image->retId();

		FILE* fptr = fopen(fileName, "w");
		if (fptr == (FILE*) NULL) {
		  Error::abortRun(*this, "cannot create: ", fileName);
		  return;
		}

		// Write the PGM header and the data, and then run.
		fprintf(fptr, "P5\n %d %d 255\n", image->retWidth(),
			image->retHeight());
		fwrite( (const char*)image->constData(), sizeof(unsigned char),
			(unsigned) image->retWidth() * image->retHeight(), fptr);
		fclose(fptr);

		StringList cmdbuf = "(";
		cmdbuf << (const char *) command << " " << fileName;
		if (del) {
		  cmdbuf << "; rm -f " << fileName;
		}
		cmdbuf << ")&";		// Run command in the background
		system(cmdbuf);
	} // end go{}
} // end defstar { DisplayImage }
