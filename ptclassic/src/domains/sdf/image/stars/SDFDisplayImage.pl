defstar {
    name      { DisplayImage }
    domain    { SDF }
    version   { $Id$ }
    author    { J. Buck, Paul Haskell }
    copyright { 1991 The Regents of the University of California }
    location  { SDF image library }
    desc {
Accept a black-and-white image from an input packet, and generate
output in PGM format.  Send the output to a user-specified command
(by default, "xv" is used).  Of course, the specified program must
be in your PATH.

The user can set the filename of the displayed image (which will
probably be printed in the image display window titlebar) and
can choose whether or not the image file is saved or deleted.
    }

    ccinclude { "GrayImage.h" , <std.h> , <stdio.h>, "UserOutput.h" }

// INPUT AND STATES.

    input {
        name { inData }
        type { packet }
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
    go {
// Read data from input.
        Packet pkt;
        (inData%0).getPacket(pkt);
        if (!StrStr(pkt.myData()->dataType(), "GrayI")) {
            Error::abortRun(*this, pkt.typeError("GrayI"));
            return;
        }

// Set filename and save values.
        const char* saveMe = saveImage;
        int del = !((saveMe[0] == 'y') || (saveMe[0] == 'Y'));
        const char* fileName = imageName;
        if (*fileName == 0) {
            fileName = tempFileName();
        }
        FILE* fd = fopen(fileName, "w");
        if (fd == 0) {
            Error::abortRun(*this," can't create file ", fileName);
            return;
        }
        GrayImage* imD = (GrayImage*) pkt.myData();

// Write the PGM header and then the data.
        fprintf (fd, "P5\n %d %d 255\n", imD->retWidth(),
             imD->retHeight());
        fwrite(imD->retData(), sizeof(unsigned char), (unsigned)
                        imD->retWidth() * imD->retHeight(), fd);
        fclose(fd);

        char buf[256];
        sprintf (buf, "(%s %s", (const char*)command, fileName);
        if (del) {
            strcat (buf, "; rm -f ");
            strcat (buf, fileName);
        }
// Run command in the background
        strcat (buf, ")&");
        system (buf);
    } // end go{}
} // end defstar { DisplayImage }
