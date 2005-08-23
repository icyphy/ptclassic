defstar {
	name		{ TkPicture }
	domain		{ SDF }
	version		{ @(#)SDFTkPicture.pl	1.1 06/25/97 }
	author		{ Luis Gutierrez }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF tcltk library }
	desc {
Display a greyscale image on a Tk widget
	}
	htmldoc {
<a name="image display"></a>
<a name="greyscale images"></a>
}

	hinclude {
		"Matrix.h" , <std.h> , <stdio.h>, "Error.h", "StringList.h"
	}
	hinclude { "Picture.h" }

	input { name { dataInput } type { FLOAT_MATRIX_ENV } }
        input { name { frameIdInput } type {int} }


	defstate {
		name { imageName  }
		type { string }
		default { "TkPicture:" }
		desc { title for the image }
	}

	defstate {
		name { palette }
		type { int  }
		default { 256 }
		desc { number of grey levels to try to use }
	}
	protected {
	    char* title;
	    u_char* buffer;
	    picObj pic;
	    int i, height, width, msize;
	    double pixvalue;
	}
        method {
	    name { setSize }
	    access { protected }
	    arglist { "( int height, int width)" }
	    code {
	        msize = width*height;
		this->height = height;
		this->width  = width;
		delete [] buffer;
		buffer = new u_char[msize];
		pic.resizePicture(height, width, buffer);
	    }
	}

	constructor {
	    title = NULL;
	    buffer = NULL;
	}

       	begin {
	    delete [] title;
	    title = savestring( (const char*) imageName );
	    // make sure title is not set to a null pointer
	    if (title == NULL) title = savestring("TkPicture:");
	    pic.setup(this, title,
		      0,
		      0,
		      (int) palette, buffer, 1);
	    width = -1;
	    height = -1;
	}

        destructor {
	    delete [] title;
	    delete [] buffer;
	}

	go {
		// Read data from input.
                Envelope pkt;
                (dataInput%0).getMessage(pkt);
                const FloatMatrix& matrix = *(const FloatMatrix*)pkt.myData();
                // check to see if size has changed and resize if so
		if ((matrix.numRows() != height) || 
		    (matrix.numCols() != width)) {
		    setSize(matrix.numRows(), matrix.numCols());
		}
		for (i = 0; i < msize; i++){
		    pixvalue = matrix.entry(i);
		    buffer[i] = (u_char) pixvalue;
		}
		pic.updatePicture();
	    }
} // end defstar { TkPicture }
