defstar {
	name		{ DisplayVideo }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image palette }
	desc {
Accept a stream of black-and-white images from input GrayImages,
save the images to files, and display the resulting files as a
moving video sequence.
This star requires that programs from the "Utah Raster Toolkit"
be in your path.
Although this toolkit is not included with Ptolemy, it is available for free.
See this star's long description (with the "look-inside" or
"manual" commands in the Ptolemy menu) for information on how
to get the toolkit.

The user can set the root filename of the displayed images
(which probably will be printed in the display window titlebar)
with the ImageName parameter. If no filename is set, a default
will be chosen.

The Save parameter can be set to "YES" or "NO" to choose whether
the created image files should be saved or deleted. Each image�s
frame number is appended to the root filename to form the image�s
complete filename.

The ByFields parameter can be set to either "YES" or "NO" to choose
whether the input images should be treated as interlaced fields
that make up a frame or as entire frames.
If the inputs are fields, then the first field should contain
frame lines 1, 3, 5, etc. and the second field should contain
lines 0, 2, 4, 6, etc
	}
	explanation {
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
This star uses programs from the \fIUtah Raster Toolkit\fR
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
These instructions are appropriate as of December 1992 but 
may have changed.
	}

	hinclude { "GrayImage.h" }
	ccinclude { <std.h> , <stdio.h> }

// INPUT AND STATES.

	input { name { inData } type { message } }

	defstate {
		name { ImageName }
		type { string }
		default { "" }
		desc { If non-null, name for run-length encoded file. }
	}
	defstate {
		name { Save }
		type { int }
		default { "NO" }
		desc { If true (YES), then save the file. }
	}
	defstate {
		name { ByFields }
		type { int }
		default { "NO" }
		desc { If true (YES), then inputs are interlaced fields. }
	}

	protected {
		unsigned char* tempFrame;
		int width, height, fieldNum, firstTime;
		StringList allFileNames, rootName, tmpFile;
	}

	constructor {
		tempFrame = 0;
	}

	setup {
		CleanUp();

		fieldNum = 0;
		firstTime = TRUE;

		delete [] tempFrame;
		tempFrame = 0;

		// Set rootName
		const char* t = ImageName;
		const char *nm = 0;
		if (t && t[0]) {
			nm = expandPathName(t);
		}
		else {
			nm = tempFileName();
		}
		rootName = nm;
		delete [] nm;

		// Set tmpFile
		nm = tempFileName();
		tmpFile = nm;
		delete [] nm;
	}

	method {			// Remove all appropriate files.
		name { CleanUp }
		type { "void" }
		access { private }
		arglist { "()" }
		code {
			StringList cmd = "rm -f";
			int shellout = FALSE;
			if ( tmpFile.length() > 0 ) {
				cmd << " " << tmpFile;
				shellout = TRUE;
			}
			tmpFile.initialize();
			if ((allFileNames.length() > 0) && !int(Save)) {
				cmd << " " << allFileNames;
				shellout = TRUE;
			}
			if (shellout) {
				system(cmd);
			}
			allFileNames.initialize();	// Clear the file list.
			LOG_DEL; delete [] tempFrame;
			tempFrame = 0;
		}
	}

	destructor {
		CleanUp();
	}

	wrapup {
		// Display the video here.
		if ( allFileNames.length() > 0 ) {
		  StringList cmd = "getx11 -m ";
		  cmd << allFileNames;
		  system(cmd);
		}
	}


	method {
		name { DoFirstTime }
		type { "void" }
		access { protected }
		arglist { "(const GrayImage& img)" }
		code {
			width = img.retWidth();
			height = img.retHeight();
			if (ByFields) {
				height *= 2;
				delete [] tempFrame;
				tempFrame = new unsigned char[width*height];
			}
			firstTime = FALSE;
	}	}


	method {
		name { SizeCheck }
		type { "int" }
		arglist { "(const GrayImage& img)" }
		access { protected }
		code {
			int err = (img.retWidth() != width);
			if (ByFields) { err |= (img.retHeight() != height/2); }
			else { err |= (img.retHeight() != height); }
			return err;
	}	}


	method {
		name { DataToFile }
		type { "void" }
		access { protected }
		arglist { "(const unsigned char* data, const int id)" }
		code {
			// Open file, write data, close file.
			FILE* fptr = fopen(tmpFile, "w");
			if (fptr == (FILE*) NULL) {
				Error::abortRun(*this, "cannot create: ",
						tmpFile);
				return;
			}

			fwrite((const char*) data, sizeof(unsigned char),
				(unsigned) (width*height), fptr);
			fclose(fptr);

			// Translate data.
			StringList fileName = rootName;
			fileName << id;

			StringList cmd = "rawtorle -n 1";
			cmd << " -w " << width;
			cmd << " -h " << height;
			cmd << " < "  << tmpFile;
			cmd << " | rleflip -v -o " << fileName;
			system(cmd);

			// Add file to list.
			allFileNames << " " << fileName;
		}
	} // end { DataToFile }


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

		// Do size check.
		if (firstTime) {
			DoFirstTime(*imD);
		}
		else {
			if (SizeCheck(*imD)) {
				Error::abortRun(*this,
						"Different input image sizes.");
				return;
			}
		}

		// Process the input data.
		if (ByFields) {
			if (!fieldNum) {
				for(int i = 0; i < height/2; i++) {
					const int t2 = i*width;
					const int t1 = 2*t2 + width;
					for(int j = 0; j < width; j++) {
						tempFrame[t1 + j] = (imD->constData())[t2 + j];
				}	}
				fieldNum = 1;
			} else {
				for(int i = 0; i < height/2; i++) {
					const int t2 = i*width;
					const int t1 = 2*t2;
					for(int j = 0; j < width; j++) {
						tempFrame[t1 + j] = (imD->constData())[t2 + j];
				}	}
				fieldNum = 0;
				DataToFile(tempFrame, imD->retId());
			}

		} else { // Data is NOT by fields.
			DataToFile(imD->constData(), imD->retId());
		}
	} // end go{}
} // end defstar { DisplayVideo }
