defstar {
//////// INFO ON STAR.
    name { ReadImage }
    domain { SDF }
    version { $Id$ }
    author { Paul Haskell }
    copyright { 1991 The Regents of the University of California }
    location { SDF image library }
    desc {
Read a PGM-format image from a file and
send it out in a Packet (with data of type GrayImage).
    }

    ccinclude { "GrayImage.h", <std.h>, <stdio.h>, "UserOutput.h" }

//////// OUTPUTS AND STATES.
    output {
        name { output }
        type { packet }
    }

    defstate {
        name    { fileName }
        type    { string }
        default { "~ptolemy/src/domains/sdf/demo/ptimage" }
        desc    { Name of file containing PGM-format image. }
    }
    defstate {
        name    { frameId }
        type    { int }
        default { 0 }
        desc    { Starting frame ID value. }
    }


////// Read data into an GrayImage object...
    go {
// Open file containing the image.
        const char* fullFileName =
                expandPathName((const char*) fileName);
        FILE* fp = fopen(fullFileName, "r");
        if (fp == (FILE*) NULL) {
            Error::abortRun(*this, "File not opened:  ", fullFileName);
            return;
        }

// Read header, skipping 1 whitespace character at end.
        char magic[80];
        int width, height, maxval;
        fscanf(fp, "%s %d %d %d%*c", magic, &width, &height, &maxval);
        if (strcmp(magic, "P5")) { // "P5" is PGM magic number.
            Error::abortRun(*this, "File not in PGM format:  ",
                    fullFileName);
            return;
        }
        if (maxval > 255) {
            Error::abortRun(*this, "File not in 8-bit format:  ",
                    fullFileName);
            return;
        }
        GrayImage* imgData = new GrayImage(width, height, int(frameId));
        frameId = int(frameId) + 1; // increment frame id
        fread((char*)imgData->retData(), sizeof(unsigned char),
                unsigned(width*height), fp);
        fclose(fp);
// Write whole frame to output here...
        Packet pkt(*imgData);
        output%0 << pkt;
    } // end go{}

} // end defstar{ ReadImage }
