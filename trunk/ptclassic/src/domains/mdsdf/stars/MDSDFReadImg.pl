defstar {
  name  	{ ReadImg }
  domain	{ MDSDF }
  version	{ $Id$ }
  author	{ Mike J. Chen, derived from SDFReadImage by Paul Haskell }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { MDSDF library }
  desc {
Read a sequence of images in Portable GrayMap (PGM) format from
different files and send them out as a FloatMatrix.

This star does not support frame numbers.  See ReadImg2.
  }
  explanation {
.Ir "PGM image format"
.Ir "image format, portable graymap (PGM)"
.Ir "image reading"
  }
  output {
    name { imageOutput } 
    type { FLOAT_MATRIX } 
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
  ccinclude { "SubMatrix.h", <std.h>, <stdio.h> }
  setup {
    // set the dimensions of the output
    imageOutput.setMDSDFParams(int(height),int(width));
  }
////// Read data into an GrayImage object...
  go {
// Open file containing the image.
    const char* expandedName = expandPathName(fileName);
    FILE* fp = fopen(expandedName, "r");
    if (fp == (FILE*) NULL) {
      Error::abortRun(*this, "File not opened: ", fileName);
      return;
    }

// Read header, skipping 1 whitespace character at end.
    char word[80];
    int fileImageWidth, fileImageHeight, maxval;
    fscanf(fp, "%s", word);
    if (strcmp(word, "P5")) { // "P5" is PGM magic number.
      Error::abortRun(*this, fileName, ": not in PGM format");
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
      Error::abortRun(*this, fileName,": not in 8-bit format.");
      return;
    }
    fscanf(fp, "%*c"); // skip one whitespace char.

// Create a FloatMatrix object and fill it with data.
// We use a hack for faster file reading: create a buffer to read in
// the data as unsigned char's and then convert that in memory.
    if(fileImageWidth != int(width) ||
       fileImageHeight != int(height)) {
      Error::abortRun("the width and height states of the star do not match those of the file image");
      return;
    }
    unsigned int size = fileImageWidth * fileImageHeight;
    unsigned char *buffer = new unsigned char[size];
    unsigned char *p = buffer;
    fread((char*)buffer, sizeof(unsigned char), size, fp);
    FloatSubMatrix* imgData = (FloatSubMatrix*)imageOutput.getOutput();
    for(unsigned int i = 0; i < size; i++)
      imgData->entry(i) = double(*p++);
    delete [] buffer;
    delete imgData;
    fclose(fp);
    delete [] expandedName;
  } // end go{}
} // end defstar{ ReadImg }
