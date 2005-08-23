defstar {
  name { DisplayVideo }
  domain { SDF }
  version { @(#)SDFDisplayVideo.pl	1.27 06 Oct 1996 }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  } 
  location { SDF image library }
  desc {
Accept a stream of black-and-white images from input Float Matrix,
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
the created image files should be saved or deleted. Each image’s
frame number is appended to the root filename to form the image’s
complete filename.

The ByFields parameter can be set to either "YES" or "NO" to choose
whether the input images should be treated as interlaced fields
that make up a frame or as entire frames.
If the inputs are fields, then the first field should contain
frame lines 1, 3, 5, etc. and the second field should contain
lines 0, 2, 4, 6, etc
  }
	htmldoc {
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
<p>
This star uses programs from the <i>Utah Raster Toolkit</i>
to display moving video in an X window.
The Utah Raster Toolkit is a collection of software tools from the
University of Utah.
These programs are available free via anonymous ftp.
To get the software:
<p>
unix&gt; ftp cs.utah.edu
<p>
ftp name&gt; anonymous
<br>
ftp passwd&gt; YOUR EMAIL ADDRESS
<br>
ftp&gt; binary
<br>
ftp&gt; cd pub
<br>
ftp&gt; get urt-3.0.tar.Z
<br>
ftp&gt; quit
<p>
unix&gt; uncompress urt-3.0.tar.Z
<br>
unix&gt; tar xvf urt-3.0.tar
<p>
Then, change directories to the new urt_3.0 directory and build the
software.
To use the software, put the name of the directory with the URT
executable files into the definition of the <i>path</i> variable
inside the .cshrc file in your home directory.
<p>
These instructions are appropriate as of December 1992 but 
may have changed.
  }

  hinclude { "Matrix.h", "StringList.h" }
  ccinclude { <std.h> , <stdio.h> }

  input { name { inData } type { FLOAT_MATRIX_ENV } }
  input { name { inFrameId } type {int} }


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
    char *nm = 0;
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
    arglist { "(const FloatMatrix& img)" }
    code {
      width = img.numCols();
      height = img.numRows();
      if (ByFields) {
	height *= 2;
      }
      delete [] tempFrame;
      tempFrame = new unsigned char[width*height];

      firstTime = FALSE;
    }
  }


  method {
    name { SizeCheck }
    type { "int" }
    arglist { "(const FloatMatrix& img)" }
    access { protected }
    code {
      int err = (img.numCols() != width);
      if (ByFields) { err |= (img.numRows() != height/2); }
      else { err |= (img.numRows() != height); }
      return err;
    }
  }

  method {
    name { DataToFile }
    type { "void" }
    access { protected }
    arglist { "(const unsigned char* data, const int id)" }
    code {
      // Open file, write data, close file.
      FILE* fptr = fopen(tmpFile, "w");
      if (fptr == (FILE*) NULL) {
	Error::abortRun(*this, "cannot create: ", tmpFile);
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
    const FloatMatrix& imD = *(const FloatMatrix*)envp.myData();

    if (envp.empty()) {
      Error::abortRun(*this, "Input is a dummyMessage.");
      return;
    }

    // Do size check.
    if (firstTime) {
      DoFirstTime(imD);
    }
    else {
      if (SizeCheck(imD)) {
	Error::abortRun(*this, "Different input image sizes.");
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
	    tempFrame[t1 + j] = (unsigned char)(imD.entry(t2 + j));
	  }
	}
	fieldNum = 1;
      } else {
	for(int i = 0; i < height/2; i++) {
	  const int t2 = i*width;
	  const int t1 = 2*t2;
	  for(int j = 0; j < width; j++) {
	    tempFrame[t1 + j] = (unsigned char)(imD.entry(t2 + j));
	  }
	}
	fieldNum = 0;
	DataToFile(tempFrame, int(inFrameId%0));
      }

    } else { // Data is NOT by fields.
      for (int i=0; i<height*width ; i++) {
	tempFrame[i] = (unsigned char)(imD.entry(i));
      }
      DataToFile(tempFrame, int(inFrameId%0));
    }
  } // end go{}
} // end defstar { DisplayVideo }


