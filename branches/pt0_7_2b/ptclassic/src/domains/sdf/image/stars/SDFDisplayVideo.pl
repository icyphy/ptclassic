defstar {
	name		{ DisplaySeq }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 The Regents of the University of California }
	location	{ ~haskell/Ptolemy }
	desc {
Accept a stream of black-and-white images from input packets
and generate output in URT-RLE format. Send the output to the command
"getx11 -m". Of course, this program must be in your PATH, as well
as the commands "rawtorle" and "rleflip". These commands are all
part of the "URT" video tool library.

The user can set the root filename of the displayed images (which will
probably be printed in the display window titlebar) and
can choose whether or not the image files are saved or deleted.
The image frame number is appended to the root filename to form the
complete filename of the displayed images.
	}

	ccinclude { "GrayImage.h" , <std.h> , <stdio.h>, "UserOutput.h" }

// INPUT AND STATES.

	input { name { inData } type { packet } }

	defstate {
		name { ImageName }
		type { string }
		default { "" }
		desc { If non-null, name for RLE file. }
	}
	defstate {
		name { Save }
		type { string }
		default { "n" }
		desc { If 'y' or 'Y', then save the file }
	}

	protected {
		const int LINELEN = 100;
		char allFileNames[40*LINELEN], rootName[LINELEN],
				temp1[LINELEN];
	}

	start {
		allFileNames[0] = '\000';

		char* t = ImageName;
		if (t && t[0]) { strcpy(rootName, ImageName); }
		else { strcpy(rootName, tempFileName()); }
		strcpy(temp1, tempFileName());
	}

	method { // Remove all appropriate files.
		name { cleanUp }
		type { "void" }
		access { private }
		arglist { "()" }
		code {
			LOG_NEW; char* cmd = new char[20 + strlen(allFileNames)];
			sprintf(cmd, "rm -f %s", temp1); system(cmd);
			char* u = Save;
			if ((allFileNames[0]) &&
					(!u || ((u[0] != 'y') && (u[0] != 'Y')))) {
				sprintf(cmd, "rm -f %s", allFileNames);
				system(cmd);
			}
			LOG_DEL; delete [] cmd;
	}	}

	wrapup { // Display the video here.
		LOG_NEW; char* cmd = new char[20 + strlen(allFileNames)];
		sprintf(cmd, "rm -f %s", temp1); system(cmd);

		if (!(allFileNames[0])) { // No files to show.
			LOG_DEL; delete [] cmd; cleanUp(); return;
		}
		sprintf(cmd, "(getx11 -m %s)&", allFileNames);
		system (cmd);

		LOG_DEL; delete [] cmd;
		cleanUp();
	}

	go {
// Read data from input.
		Packet pkt;
		(inData%0).getPacket(pkt);
		TYPE_CHECK(pkt, "GrayImage");
		const GrayImage* imD = (const GrayImage*) pkt.myData();
		if (imD->fragmented() || imD->processed()) {
			Error::abortRun(*this,
					"Need unfragmented and unprocessed input images.");
			return;
		}

// Open file, write data, close file.
		char name[LINELEN];
		sprintf(name, "%s%d", rootName, imD->retId());

		FILE* fptr = fopen(temp1, "w");
		if (fptr == (FILE*) NULL) {
			Error::abortRun(*this, "can not create: ", temp1);
			return;
		}

		fwrite(imD->constData(), sizeof(unsigned char),
			(unsigned) imD->retWidth() * imD->retHeight(), fptr);
		fclose(fptr);

// Translate data.
		char cmd[LINELEN];
		sprintf(cmd,
				"rawtorle -n 1 -w %d -h %d < %s | rleflip -v -o %s",
				imD->retWidth(), imD->retHeight(), temp1, name);
		system(cmd);

// Add file to list.
		strcat(allFileNames, " ");
		strcat(allFileNames, name);
	} // end go{}
} // end defstar { DisplaySeq }