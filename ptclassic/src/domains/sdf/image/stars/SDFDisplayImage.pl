defstar {
	name		{ DisplayImage }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ J. Buck, Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
Accept a black-and-white image from an input packet, and generate
output in PGM format. Send the output to a user-specified command
(by default, "xv" is used). Of course, the specified program must
be in your PATH.

The user can set the root filename of the displayed image (which will
probably be printed in the image display window titlebar) and
can choose whether or not the image file is saved or deleted.
The image frame number is appended to the root filename to form the
complete filename of the displayed image.
	}

	ccinclude { "GrayImage.h" , <std.h> , <stdio.h>, "UserOutput.h" }

// INPUT AND STATES.

	input {
		name { inData }
		type { packet }
	}
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
		Packet pkt;
		(inData%0).getPacket(pkt);
		TYPE_CHECK(pkt,"GrayImage");
		GrayImage* imD = (GrayImage*) pkt.myData();

// Set filename and save values.
		const char* saveMe = saveImage;
		int del = !((saveMe[0] == 'y') || (saveMe[0] == 'Y'));

		char fileName[256];
		if (!(const char*) imageName) {
			strcpy(fileName, tempFileName());
		} else {
			strcpy(fileName, imageName);
		}
		char numstr[16];
		sprintf(numstr, ".%d", imD->retId());
		strcat(fileName, numstr);

		FILE* fd = fopen(fileName, "w");
		if (fd == (FILE*) NULL) {
			Error::abortRun(*this, "can not create: ", fileName);
			return;
		}

// Write the PGM header and the data, and then run.
		fprintf (fd, "P5\n %d %d 255\n", imD->retWidth(),
				imD->retHeight());
		fwrite((char*)imD->retData(), sizeof(unsigned char), (unsigned)
				imD->retWidth() * imD->retHeight(), fd);
		fclose(fd);

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
