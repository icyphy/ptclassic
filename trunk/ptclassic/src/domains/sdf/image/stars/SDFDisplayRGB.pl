defstar {
	name		{ DisplayRgb }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Sun-Inn Shih }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
Accept three ColorImages (Red, Green, and Blue) from three input
packets, and generate a PPM format color image file.  Send the filename
to a user-specified command (by default, "xv" is used).

The user can set the root filename of the displayed image (which will
probably be printed in the image display window titlebar) and
can choose whether or not the image file is saved or deleted.
The frameId of the received image is appended to the root filename
to produce the full filename of the displayed image.
	}

	ccinclude { "GrayImage.h" , <std.h> , <stdio.h>, "UserOutput.h" }

// INPUT AND STATES.
	input {
		name { rinput }
		type { packet }
	}
	input {
		name { ginput }
		type { packet }
	}
	input {
		name { binput }
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
		name { saveColor }
		type { string }
		default { "n" }
		desc { If 'y' or 'Y', then save the file }
	}


	go {
// Read data from input.
		Packet rpkt, gpkt, bpkt;
		(rinput%0).getPacket(rpkt);
	TYPE_CHECK(rpkt,"GrayImage");
		(ginput%0).getPacket(gpkt);
		TYPE_CHECK(gpkt,"GrayImage");
		(binput%0).getPacket(bpkt);
		TYPE_CHECK(bpkt,"GrayImage");
		GrayImage* tempyD = (GrayImage*) rpkt.myData();
		GrayImage* tempuD = (GrayImage*) gpkt.myData();
		GrayImage* tempvD = (GrayImage*) bpkt.myData();

// Set filename and save values.
		const char* saveMe = saveColor;
		int del = !((saveMe[0] == 'y') || (saveMe[0] == 'Y'));

		char fileName[256];
		if (!(const char*) imageName) {
			strcpy(fileName, tempFileName());
		} else {
			strcpy(fileName, (const char*) imageName);
		}
		char numtmp[16];
		sprintf(numtmp, ".%d", tempyD->retId());
		strcat(fileName, numtmp);

		FILE* fp = fopen(fileName, "w");
		if (fp == (FILE*) NULL) {
			Error::abortRun(*this, fileName, ": can't create");
			return;
		}

// change into RGB format
		int Width  = tempyD->retWidth();
		int Height = tempyD->retHeight();

		unsigned char* rfp = tempyD->retData();
		unsigned char* gfp = tempuD->retData();
		unsigned char* bfp = tempvD->retData();

		unsigned char* rgbfp= new unsigned char[3*Width*Height];

		int i, j, temp1, temp2, temp3;
		for (i = 0; i < Height; i++) {
			temp1 = i*Width;
			for (j = 0; j < Width; j++){
				temp2 = j+temp1;
				temp3 = 3*temp2;
				rgbfp[temp3] = rfp[temp2];
				rgbfp[temp3+1] = gfp[temp2];
				rgbfp[temp3+2] = bfp[temp2];
		}	}

// Write the PGM header and then the data.
		fprintf (fp, "P6\n %d %d 255\n", Width, Height);
		fwrite((char*)rgbfp, sizeof(unsigned char), unsigned(3*Width*Height),
				fp);
		fclose(fp);

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
