defstar {
	name	{ ReadImage }
	domain	{ SDF }
	version	{ $Id$ }
	author	{ Paul Haskell }
        copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF image library }
	desc {
Read a sequence of images in Portable GrayMap (PGM) format from
different files and send them out in Envelopes (containing data
of type GrayImage).

If present, the character '#' in the 'fileName' state is replaced with
the frame number to be read next.
For example, if the 'frameId' state is set to 2 and
if the 'fileName' state is 'dir.#/pic#', then the files that
are read are 'dir.2/pic2', 'dir.3/pic3', etc.
	}
	explanation {
.Ir "PGM image format"
.Ir "image format, portable graymap (PGM)"
.Ir "image reading"
	}

	ccinclude { "GrayImage.h", <std.h>, <stdio.h> }

	output { name { output } type { message } }

	defstate {
		name	{ fileName }
		type	{ string }
		default { "$PTOLEMY/src/domains/sdf/demo/ptimage" }
		desc	{ Name of file containing PGM-format image. }
	}
	defstate {
		name	{ frameId }
		type	{ int }
		default { 0 }
		desc	{ Starting frame ID value. }
	}

	method {
		name { genFileName }
		type { "char *" }
		arglist { "(const char* str, const int d)" }
		access { protected }
		code {
			char* filename = expandPathName(str);
			char num[16];
			sprintf(num, "%d", d);
			char* newname = subCharByString(filename, '#', num);
			delete [] filename;
			return newname;
		} // end genFileName()
	}

	go {
		// Open file containing the image.
		char expandedName = genFileName(fileName, int(frameId));
		StringList fullname = expandedName;
		delete [] expandedName;
		FILE* fp = fopen(fullName, "r");
		if (fp == (FILE*) NULL) {
			Error::abortRun(*this, "File not opened: ", fullName);
			return;
		}

		// Read header, skipping 1 whitespace character at end.
		char word[80];
		int width, height, maxval;
		fscanf(fp, "%s", word);
		if (strcmp(word, "P5")) { // "P5" is PGM magic number.
			Error::abortRun(*this, fullName, ": not in PGM format");
			return;
		}
		fscanf(fp, "%s", word);
		while (word[0] == '#') {
			fscanf(fp, "%*[^\n]%s", word);
		}
		sscanf(word, "%d", &width);
		fscanf(fp, "%s", word);
		while (word[0] == '#') {
			fscanf(fp, "%*[^\n]%s", word);
		}
		sscanf(word, "%d", &height);
		fscanf(fp, "%s", word);
		while (word[0] == '#') {
			fscanf(fp, "%*[^\n]%s", word);
		}
		sscanf(word, "%d", &maxval);
		if (maxval > 255) {
			Error::abortRun(*this, fullName,
					": not in 8-bit format.");
			return;
		}
		fscanf(fp, "%*c");		// skip one whitespace char.

		// Create image object and fill it with data.
		LOG_NEW;
		GrayImage* imgData = new GrayImage(width, height, int(frameId));
		fread( (char*)imgData->retData(),
		       width * sizeof(unsigned char),
		       height,
		       fp );
		fclose(fp);
		frameId = int(frameId) + 1;		// increment frame id

		// Write the new frame to output...
		Envelope envp(*imgData); output%0 << envp;
	} // end go{}
} // end defstar{ ReadImage }
