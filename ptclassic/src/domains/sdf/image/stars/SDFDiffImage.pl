defstar {
    name      { DiffImage }
    domain    { SDF }
    version   { $Id$ }
    author    { Paul Haskell }
    copyright { 1991 The Regents of the University of California }
    location  { SDF image library }
    desc {
Accept two black-and-white images from input packets, take
their absolute difference, scale the absolute difference,
and pass the result to the output.
    }

    ccinclude { "GrayImage.h", "Error.h" }

// INPUT AND STATES.
    input {
        name { input1 }
        type { packet }
    }
    input {
        name { input2 }
        type { packet }
    }
    output {
        name { outData }
        type { packet }
    }
    defstate {
        name    { Scale }
        type    { float }
        default { 1.0 }
        desc    { Amount by which to scale the image difference. }
    }

// CODE.
    go {
// Read inputs.
        Packet inPkt1, inPkt2;
        (input1%0).getPacket(inPkt1);
        (input2%0).getPacket(inPkt2);
	TYPE_CHECK(inPkt1,"GrayImage");
	TYPE_CHECK(inPkt2,"GrayImage");
        GrayImage* inImage1 = (GrayImage*) inPkt1.writableCopy();
        const GrayImage* inImage2 = (const GrayImage*) inPkt2.myData();

// Calc the difference.
        int width, height;
        if ((width = inImage1->retWidth()) != inImage2->retWidth()) {
            Error::abortRun(*this, "Image widths do not match.");
            return;
        }
        if ((height = inImage1->retHeight()) != inImage2->retHeight()) {
            Error::abortRun(*this, "Image heights do not match.");
            return;
        }
        unsigned char* ptr1 = inImage1->retData();
        unsigned const char* ptr2 = inImage2->constData();
        float  diff;
        double scale = Scale;
        for(int travel = 0; travel < width*height; travel++) {
            diff = float(ptr1[travel]) - float(ptr2[travel]);
            diff = abs(diff*scale);
            if (diff < 0.5)        ptr1[travel] = (unsigned char) 0;
            else if (diff > 254.5) ptr1[travel] = (unsigned char) 255;
            else ptr1[travel] = (unsigned char) (diff + 0.5);
        }

// Send the result.
        Packet outPkt(*inImage1);
        (outData%0) << outPkt;
    } // end go{}
} // end defstar { DiffImage }
