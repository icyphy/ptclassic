defstar {
	name		{ ReadRGB }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Sun-Inn Shih }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Read an image in Portable Pixmap (PPM) format from a file and
separate the colors into three different images a red, green, and
blue image.  Each image is represented as a float matrix.  If present,
the character '#' in the 'fileName' state is replaced with the frame
number to be read next.  For example, if the 'frameId' state is set
to 2 and if the 'fileName' state is 'dir.#/pic#', then the files that
are read are 'dir.2/pic2', 'dir.3/pic3', etc.
	}
	htmldoc {
<a name="image reading"></a>
<a name="PPM image format"></a>
<a name="image format, portable pixmap (PPM)"></a>
	}

	ccinclude { "Matrix.h", <std.h>, <stdio.h>, "Error.h", "StringList.h", "miscFuncs.h" }

	output { name {output1} type {FLOAT_MATRIX_ENV} desc { Red image. } }
	output { name {output2} type {FLOAT_MATRIX_ENV} desc { Green image. } }
	output { name {output3} type {FLOAT_MATRIX_ENV} desc { Blue image. } }
        output { name {frameIdOut} type { int } }

	defstate {
		name	{ fileName }
		type	{ string }
		default { "$PTOLEMY/src/domains/sdf/demo/ppimage" }
		desc	{ Name of file containing PPM-format image. }
	}

	defstate {
		name	{ frameId }
		type	{ int }
		default	{ 0 }
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
		char* expandedName = genFileName(fileName, int(frameId));
		StringList fullName = expandedName;
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
		if (strcmp(word, "P6")) { // "P6" is PPM magic number.
			Error::abortRun(*this, fullName, ": not in PPM format");
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
			Error::abortRun(*this, fullName, ": not in 8-bit format");
			return;
		}
		fscanf(fp, "%*c"); // skip one whitespace char.

// Create new images and fill them with data.
		LOG_NEW;
		unsigned char* rgbfp = new unsigned char[3*width*height];
		fread((char*)rgbfp, sizeof(unsigned char), 3*width*height, fp);
		fclose(fp);

		LOG_NEW;
		FloatMatrix& rColor = *(new FloatMatrix(height,width));
		LOG_NEW;
		FloatMatrix& gColor = *(new FloatMatrix(height,width));
		LOG_NEW;
		FloatMatrix& bColor = *(new FloatMatrix(height,width));

		int i, j, temp1, temp2, temp3;
		for (i = 0; i < height; i++) {
			temp1 = i*width;
			for (j = 0; j < width; j++){
				temp2 = j + temp1;
				temp3 = 3*temp2;
				rColor.entry(temp2) = rgbfp[temp3];
				gColor.entry(temp2) = rgbfp[temp3+1];
				bColor.entry(temp2) = rgbfp[temp3+2];
		}	}
		LOG_DEL; delete [] rgbfp;

// Write whole frame to output here...
		output1%0 << rColor;
		output2%0 << gColor;
		output3%0 << bColor;
		frameIdOut%0 << int(frameId);

		frameId = int(frameId) + 1; //increment frame id
	} // end go{}
} // end defstar{ ReadRgb }
