defstar {
//////// INFO ON STAR.
	name	{ ReadImage }
	domain	{ SDF }
	version	{ $Id$ }
	author	{ Paul Haskell }
	copyright	{ 1991, 1992 The Regents of the Univ. of California }
	location	{ SDF image library }
	desc {
Read a sequence of PGM-format images from different files and
send them out in Packets (with data of type GrayImage).

If present, the character '#' in the 'fileName' state is replaced with
the frame number to be read next. For example, if the 'frameId' state is
set to 2 and if the 'fileName' state is 'dir.#/pic#' then the files that
are read and output are 'dir.2/pic2', 'dir.3/pic3', etc.
	}
	explanation {
.Ir "PGM image format"
.Ir "image format, PGM"
.Ir "image reading"
	}

	ccinclude { "GrayImage.h", <std.h>, <stdio.h> }

//////// OUTPUTS AND STATES.
	output {
		name { output }
		type { packet }
	}

	defstate {
		name	{ fileName }
		type	{ string }
		default { "~ptolemy/src/domains/sdf/demo/ptimage" }
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
		type { "void" }
		arglist { "(char* outstr, const char* str, const int d)" }
		access { protected }
		code {
			char* p = outstr;
			const char* q = expandPathName(str);
			char num[16];

			sprintf(num, "%d", d);
			int len = strlen(num);
			while (*q != '\000') { // Replace '#' with 'num's value.
				if (*q == '#') {
					strcpy(p, num);
					p += len;
					q++;
				} else {
					*p = *q; p++; q++;
				}
			}
			*p = '\000';
		} // end genFileName()
	}

////// Read data into an GrayImage object...
	go {
// Open file containing the image.
		char fullName[256];
		genFileName(fullName, fileName, int(frameId));
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
			Error::abortRun(*this, fullName,": not in 8-bit format.");
			return;
		}
		fscanf(fp, "%*c"); // skip one whitespace char.

// Create image object and fill it with data.
		LOG_NEW; GrayImage* imgData = new GrayImage(width, height,
				int(frameId));
		fread((char*)imgData->retData(), sizeof(unsigned char),
				unsigned(width*height), fp);
		fclose(fp);
		frameId = int(frameId) + 1; // increment frame id

// Write the new frame to output...
		Packet pkt(*imgData);
		output%0 << pkt;
	} // end go{}
} // end defstar{ ReadImage }
