defstar {
	name		{ DisplayVideo }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image palette }
	desc {
Accept a stream of black-and-white images from input GrayImages,
save the images to files, and display the resulting files as a
moving video sequence. This star requires that programs from
the "Utah Raster Toolkit" be in your "path" variable. Although this
toolkit is not included with Ptolemy it is available for free.
See this star's long description (with the "look-inside"
or "manual" commands in the Ptolemy menu) for info on how to get
the toolkit.

The user can set the root filename of the displayed images (which
probably will be printed in the display window titlebar) with
the 'ImageName' state. If no filename is set, a default will
be chosen.

The 'Save' state can be set to "YES" or "NO" to choose whether
the created image files should be saved or deleted.
Each image's frame number is appended to the root filename
to form the image's complete filename.
	}
	explanation {
.pp
At the end of a simulation this star pops up an X window and
loads in a sequence of video frames for display.
Pressing the left or right mouse buttons inside the window plays
the video sequence backwards or forwards.
The middle mouse button allows single-stepping through frames.
If you hold down the shift key while pressing the left mouse button,
you loop through the sequence.
The shift key and middle mouse button lets you alter the frame rate
of the displayed video.
The shift key and right mouse button loops through the video
sequence alternately forwards and backwards.
To end a loop playback, press any mouse button in the video window.
To close the window type "q" inside.
.pp
This star uses programs from the
\fIUtah Raster Toolkit\fR
to display moving video in an X window.
The Utah Raster Toolkit is a collection of software tools from the
University of Utah.
These programs are available free via anonymous ftp.
To get the software:
.sp
unix> ftp cs.utah.edu
.sp
ftp name> anonymous
.br
ftp passwd> YOUR EMAIL ADDRESS
.br
ftp> binary
.br
ftp> cd pub
.br
ftp> get urt-3.0.tar.Z
.br
ftp> quit
.sp
unix> uncompress urt-3.0.tar.Z
.br
unix> tar xvf urt-3.0.tar
.sp
Then, change directories to the new urt_3.0 directory and build the
software.
To use the software, put the name of the directory with the URT
executable files into the definition of the \fIpath\fR variable
inside the .cshrc file in your home directory.
.pp
These instructions are appropriate as of December 1992 but may change
in the future.
	}

	ccinclude { "GrayImage.h" , <std.h> , <stdio.h> }

// INPUT AND STATES.

	input { name { inData } type { message } }

	defstate {
		name { ImageName }
		type { string }
		default { "" }
		desc { If non-null, name for RLE file. }
	}
	defstate {
		name { Save }
		type { int }
		default { "NO" }
		desc { If true (YES), then save the file }
	}

	header {
		const int LINELEN = 100;
	}
	protected {
		char allFileNames[40*LINELEN], rootName[LINELEN],
				temp1[LINELEN];
	}

	start {
		allFileNames[0] = '\000';

		const char* t = ImageName;
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
			if ((allFileNames[0]) && !int(Save)) {
				sprintf(cmd, "rm -f %s", allFileNames);
				system(cmd);
			}
			LOG_DEL; delete [] cmd;
	}	}

	wrapup { // Display the video here.
		LOG_NEW; char* cmd = new char[20 + strlen(allFileNames)];

		if (!(allFileNames[0])) { // No files to show.
			LOG_DEL; delete [] cmd; cleanUp(); return;
		}
		sprintf(cmd, "getx11 -m %s", allFileNames);
		system (cmd);

		LOG_DEL; delete [] cmd;
		cleanUp();
	}

	go {
// Read data from input.
		Envelope envp;
		(inData%0).getMessage(envp);
		TYPE_CHECK(envp, "GrayImage");
		const GrayImage* imD = (const GrayImage*) envp.myData();
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

		fwrite((const char*)imD->constData(), sizeof(unsigned char),
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
