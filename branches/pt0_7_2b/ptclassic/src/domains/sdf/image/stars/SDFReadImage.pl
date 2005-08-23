defstar {
	name	{ ReadImage }
	domain	{ SDF }
	version	{ @(#)SDFReadImage.pl	1.24 01 Oct 1996 }
	author	{ Paul Haskell }
        copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF image library }
	desc {
Read a sequence of images in Portable GrayMap (PGM) format from
different files and send them out in a float matrix.

If present, the character '#' in the 'fileName' state is replaced with
the frame number to be read next.
For example, if the 'frameId' state is set to 2 and
if the 'fileName' state is 'dir.#/pic#', then the files that
are read are 'dir.2/pic2', 'dir.3/pic3', etc.
	}
	htmldoc {
<a name="PGM image format"></a>
<a name="image format, portable graymap (PGM)"></a>
<a name="image reading"></a>
	}

	ccinclude { "Matrix.h", <std.h>, <stdio.h>, "miscFuncs.h", "StringList.h", "Error.h" }

	output { name { dataOutput } type { FLOAT_MATRIX_ENV } }
	output { name { frameIdOutput } type { int } }

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
		type { "char*" }
		arglist { "(const char* str, const int d)" }
		access { protected }
		code {
			char num[16];
			sprintf(num, "%d", d);
			char* filename = expandPathName(str);
			char* newfilename = subCharByString(filename, '#', num);
			delete [] filename;
			return newfilename;
		} // end genFileName()
	}

	go {
		// Open file containing the image.
		char *expandedName = genFileName(fileName, int(frameId));
		StringList fullName = expandedName;
		delete [] expandedName;
		FILE *fp = fopen(fullName, "r");
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

		// Create an array to read into image data.
		unsigned char* data = (unsigned char*) new char[height*width];
		fread( (unsigned char*)data,
		       width * sizeof(unsigned char),
		       height,
		       fp );
		fclose(fp);

		// Create image matrix and fill it with data.
		LOG_NEW;
		FloatMatrix& result = *(new FloatMatrix(height,width));

		for ( int i=0; i<height; i++) 
		  for ( int j=0; j<width; j++)
                    result[i][j] = float(data[i*width+j]);

		delete [] data;      

		// Write the new frame to output...
		dataOutput%0 << result;
		frameIdOutput%0 << int(frameId);

		frameId = int(frameId) + 1;		// increment frame id

	} // end go{}
} // end defstar{ ReadImage }
