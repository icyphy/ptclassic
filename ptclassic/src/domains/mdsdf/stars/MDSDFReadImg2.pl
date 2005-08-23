defstar {
  name  	{ ReadImg2 }
  domain	{ MDSDF }
  version	{ @(#)MDSDFReadImg2.pl	1.9 01 Oct 1996 }
  author	{ Mike J. Chen, derived from SDFReadImage by Paul Haskell }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { MDSDF library }
  desc {
Read a sequence of images in Portable GrayMap (PGM) format from
different files and send them out as a FloatMatrix.

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
  output {
    name { imageOutput } 
    type { FLOAT_MATRIX } 
  }
  output { 
    name { frameIdOutput }
    type { INT_MATRIX }
  }
  defstate {
    name    { width }
    type    { int }
    default { 100 }
    desc    { width of the image, cols of matrix }
  }
  defstate {
    name    { height }
    type    { int }
    default { 100 }
    desc    { height of the image, rows of matrix }
  }
  defstate {
    name    { fileName }
    type    { string }
    default { "$PTOLEMY/src/domains/sdf/demo/ptimage" }
    desc    { Name of file containing PGM-format image. }
  }
  defstate {
    name    { frameId }
    type    { int }
    default { 0 }
    desc    { Starting frame ID value. }
  }
  ccinclude { "SubMatrix.h", <std.h>, <stdio.h>, "miscFuncs.h", "StringList.h" }
  setup {
    // set the dimensions of the output
    imageOutput.setMDSDFParams(int(height),int(width));
    frameIdOutput.setMDSDFParams(1,1);
  }
  method {
    name { genFileName }
    type { "char *" }
    arglist { "(const char* str, const int d)" }
    access { protected }
    code {
      char *filename = expandPathName(str);
      char num[16];
      sprintf(num, "%d", d);
      char *newname = subCharByString(filename, '#', num);
      delete [] filename;
      return newname;
    } // end genFileName()
  }

  go {
    // Open file containing the image.
    char *expandedName = genFileName(fileName, int(frameId));
    StringList fullName = expandedName;
    delete [] expandedName;
    FILE* fp = fopen(fullName, "r");
    if (fp == 0) {
      Error::abortRun(*this, "cannot open '", fullName, "' for reading.");
      return;
    }

    // Read header, skipping 1 whitespace character at end.
    char word[80];
    int fileImageWidth, fileImageHeight, maxval;
    fscanf(fp, "%s", word);
    if (strcmp(word, "P5")) { // "P5" is PGM magic number.
      fclose(fp);
      Error::abortRun(*this, fullName, ": not in PGM format");
      return;
    }
    fscanf(fp, "%s", word);
    while (word[0] == '#') {
      fscanf(fp, "%*[^\n]%s", word);
    }
    sscanf(word, "%d", &fileImageWidth);
    fscanf(fp, "%s", word);
    while (word[0] == '#') {
      fscanf(fp, "%*[^\n]%s", word);
    }
    sscanf(word, "%d", &fileImageHeight);
    fscanf(fp, "%s", word);
    while (word[0] == '#') {
      fscanf(fp, "%*[^\n]%s", word);
    }
    sscanf(word, "%d", &maxval);
    if (maxval > 255) {
      fclose(fp);
      Error::abortRun(*this, fullName, ": not in 8-bit format.");
      return;
    }
    fscanf(fp, "%*c"); // skip one whitespace char.

    // Create a FloatMatrix object and fill it with data.
    // We use a hack for faster file reading: create a buffer to read in
    // the data as unsigned char's and then convert that in memory.
    if (fileImageWidth != int(width) ||
        fileImageHeight != int(height)) {
      fclose(fp);
      Error::abortRun(*this,
		      "the width and height states of the star do ",
		      "not match those of the image file ",
		      fullName);
      return;
    }
    unsigned int size = fileImageWidth * fileImageHeight;
    unsigned char *buffer = new unsigned char[size];
    unsigned char *p = buffer;
    fread( (char*)buffer,
    	   fileImageWidth * sizeof(unsigned char),
	   fileImageHeight,
	   fp );
    FloatSubMatrix* imgData = (FloatSubMatrix*)imageOutput.getOutput();
    for (unsigned int i = 0; i < size; i++) {
      imgData->entry(i) = double(*p++);
    }
    delete[] buffer;
    delete imgData;
    fclose(fp);

    IntSubMatrix* floatIdOut = (IntSubMatrix*)frameIdOutput.getOutput();
    floatIdOut->entry(0) = frameId;
    frameId = int(frameId) + 1;			// increment frame id
    delete floatIdOut;
  } // end go{}
} // end defstar{ ReadImg2 }
