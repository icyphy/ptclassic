defstar {
  name  	{ ReadImg }
  domain	{ MDSDF }
  version	{ @(#)MDSDFReadImg.pl	1.9 01 Oct 1996 }
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

This star does not support frame numbers.  See ReadImg2.
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

  go {
    // Open file containing the image.
    char* nm = expandPathName(fileName);
    StringList expandedName = nm;
    delete [] nm;
    FILE* fp = fopen(expandedName, "r");
    if (fp == (FILE*) NULL) {
      Error::abortRun(*this, "cannot open '", expandedName, "' for reading.");
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
      Error::abortRun(*this,
		      "the width and height states do not ",
		      "match those of the file image read from ",
		      fileName);
      return;
    }
    unsigned int size = fileImageWidth * fileImageHeight;
    unsigned char *buffer = new unsigned char[size];
    unsigned char *p = buffer;
    fread( (char*)buffer,
    	   int(width) * sizeof(unsigned char),
	   int(height),
	   fp );
    FloatSubMatrix* imgData = (FloatSubMatrix*)imageOutput.getOutput();
    for(unsigned int i = 0; i < size; i++)
      imgData->entry(i) = double(*p++);
    delete [] buffer;
    delete imgData;
    fclose(fp);
  } // end go{}
} // end defstar{ ReadImg }
