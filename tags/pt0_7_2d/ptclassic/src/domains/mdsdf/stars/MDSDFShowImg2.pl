defstar {
  name		{ ShowImg2 }
  domain	{ MDSDF }
  version	{ @(#)MDSDFShowImg2.pl	1.9 01 Oct 1996 }
  author	{ J. Buck & Paul Haskell, modified for MDSDF by Mike J. Chen }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location	{ MDSDF library }
  desc {
Accept a black-and-white input FloatMatrix image and generate
output in PGM format. Send the output to a user-specified command
(by default,
.EQ
delim off
.EN
"xv"
.EQ
delim $$
.EN
is used).

The user can set the root filename of the displayed image (which will
probably be printed in the image display window titlebar) and
can choose whether or not the image file is saved or deleted.
The image frame number is appended to the root filename to form the
complete filename of the displayed image.
  }
	htmldoc {
<a name="image display"></a>
<a name="PGM image format"></a>
<a name="image format, PGM"></a>
  }
  ccinclude {
    "SubMatrix.h" , <std.h> , <stdio.h>, "Error.h"
  }

// INPUT AND STATES.
  input { 
    name { imageInput } 
    type { FLOAT_MATRIX } 
  }
  input { 
    name { frameIdInput } 
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
    name { command }
    type { string }
    default { "xv" }
    desc { Program to run on PGM data }
  }
  defstate {
  name { imageName }
    type { string }
    default { "" }
    desc { If non-null, name for PGM file }
  }
  defstate {
    name { saveImage }
    type { string }
    default { "n" }
    desc { If 'y' or 'Y', then save the file }
  }

// CODE.
  setup {
    // set the dimensions of the output
    imageInput.setMDSDFParams(int(height),int(width));
    frameIdInput.setMDSDFParams(1,1);
  }
  go {
    // Read data from input.
    FloatSubMatrix* image = (FloatSubMatrix*)(imageInput.getInput());
    IntSubMatrix* frameId = (IntSubMatrix*)(frameIdInput.getInput());
    int frame = frameId->entry(0);

    // Set filename and save values.
    const char* saveMe = saveImage;
    int del = !((saveMe[0] == 'y') || (saveMe[0] == 'Y'));

    const char* iname = imageName;
    char* nm = 0;
    if (iname && *iname) {
      nm = expandPathName(iname);
    }
    else {
      nm = tempFileName();
    }
    StringList fileName = nm;
    fileName << "." << frame;
    delete [] nm;

    FILE* fp = fopen(fileName, "w");
    if (fp == 0) {
      Error::abortRun(*this, "cannot open '", fileName, "' for writing.");
      delete image;
      delete frameId;
      return;
    }

    // Write the PGM header and the data, and then run.
    fprintf(fp, "P5\n %d %d 255\n", int(width), int(height));

    // Reverse of the hack used in ReadImage, first copy the data to
    // a buffer of the unsigned char's, then do a block fwrite
    unsigned int size = int(width) * int(height);
    unsigned char* buffer = new unsigned char[size];
    unsigned char* p = buffer;
    for(unsigned int i = 0; i < size; i++) {
      // limit range to be between 0 and 255
      double tmp = image->entry(i);
      if(tmp < 0)
        *p++ = 0;
      else if(tmp > 255)
        *p++ = 255;
      else
        *p++ = (unsigned char)tmp;
    }
    fwrite( (const char*) buffer,
    	    sizeof(unsigned char) * int(width),
	    int(height),
	    fp );
    fclose(fp);
    delete [] buffer;

    StringList cmdbuf = "(";
    cmdbuf << (const char*) command << " " << fileName;
    if (del) {
      cmdbuf << "; rm -f " << fileName;
    }
    cmdbuf << ")&"; 			// Run command in the background
    system(cmdbuf);

    delete image;
    delete frameId;
  } // end go{}
} // end defstar { ShowImg2 }
