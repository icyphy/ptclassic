 defstar {
//////// INFO ON STAR.
    name      { ReadRgb }
    domain    { SDF }
    version   { $Id$ }
    author    { Sun-Inn Shih }
    copyright { 1991 The Regents of the University of California }
    location  { SDF image library }
    desc {
Read a PPM-format image from a file and
send it out in three Packets--a Red, Green, and Blue packet.
Each packet is of type GrayImage.
    }

    ccinclude { "GrayImage.h", <std.h>, <stdio.h>, "Error.h" }

//////// OUTPUTS AND STATES.
    output {
        name { output1 }
        type { packet }
    }
    output {
        name { output2 }
        type { packet }
    }
    output {
        name { output3 }
        type { packet }
    }

   defstate {
        name    { fileName }
        type    { string }
        default { "~ptolemy/src/domains/sdf/demo/ppimage" }
        desc    { Name of file containing PPM-format image. }
    }

    defstate {
         name      { frameId }
        type       { int }
        default    { 0 }
        desc       { Starting frame ID value. }
    }

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
        fscanf(fp, "%s %d %d %d%*c", magic, &width, &height,
                &maxval);
        if (strcmp(magic, "P6")) { // "P6" is PPM magic number.
            Error::abortRun(*this, "File not in PPM format:  ",
                    fullFileName);
            return;
        }
        if (maxval > 255) {
            Error::abortRun(*this, "File not in 8-bit format:  ",
                    fullFileName);
            return;
        }

        unsigned char* rgbfp = new unsigned char[3*width*height];
        fread(rgbfp, sizeof(unsigned char), 3*width*height, fp);
        fclose(fp);

        GrayImage* rColor = new GrayImage(width, height, int(frameId));
        GrayImage* gColor = new GrayImage(width, height, int(frameId));
        GrayImage* bColor = new GrayImage(width, height, int(frameId));


        unsigned char* rfp = rColor->retData();
        unsigned char* gfp = gColor->retData();
        unsigned char* bfp = bColor->retData();

        int i, j, temp1, temp2, temp3;
        for (i = 0; i < height; i++) {
            temp1 = i*width;
            for (j = 0; j < width; j++){
                temp2 = j + temp1;
                temp3 = 3*temp2;
                rfp[temp2] = rgbfp[temp3];
                gfp[temp2] = rgbfp[temp3+1];
                bfp[temp2] = rgbfp[temp3+2];
        }   }
        delete rgbfp;

// Write whole frame to output here...
        Packet pktr(*rColor);
        Packet pktg(*gColor);
        Packet pktb(*bColor);
        output1%0 << pktr;
        output2%0 << pktg;
        output3%0 << pktb;
        frameId = int(frameId) + 1; //increment frame id
    } // end go{}
} // end defstar{ ReadRgb }
