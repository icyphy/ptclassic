defstar {
//////// INFO ON STAR.
	name { ReadImage }
	domain { SDF }
	version { $Id$ }
	author { Paul Haskell }
	copyright { 1991 The Regents of the University of California }
	location { SDF image library }
	desc {
Read a sequence of PGM-format images from different files and
send them out in Packets (with data of type GrayImage).

If present, the character '#' in the 'fileName' state is replaced with
the frame number to be read next. For example, if the 'frameId' state is
set to 2 and if the 'fileName' state is 'dir.#/pic#' then the files that
are read and output are 'dir.2/pic2', 'dir.3/pic3', etc.
	}

	ccinclude { "GrayImage.h", <std.h>, <stdio.h>, "UserOutput.h" }

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
		char magic[80];
		int width, height, maxval;
		fscanf(fp, "%s %d %d %d%*c", magic, &width, &height, &maxval);
		if (strcmp(magic, "P5")) { // "P5" is PGM magic number.
			Error::abortRun(*this, fullName, ": not in PGM format");
			return;
		}
		if (maxval > 255) {
			Error::abortRun(*this, fullName,": not in 8-bit format.");
			return;
		}
		LOG_NEW; GrayImage* imgData = new GrayImage(width, height, int(frameId));
		frameId = int(frameId) + 1; // increment frame id
		fread((char*)imgData->retData(), sizeof(unsigned char),
				unsigned(width*height), fp);
		fclose(fp);
// Write whole frame to output here...
		Packet pkt(*imgData);
		output%0 << pkt;
	} // end go{}
} // end defstar{ ReadImage }
