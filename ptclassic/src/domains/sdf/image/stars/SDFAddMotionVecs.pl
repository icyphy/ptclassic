defstar {
	name		{ AddMvecs }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 The Regents of the University of California }
	location	{ SDF image library }
	desc {
Over each block in the input image, superimpose an arrow indicating
the size and direction of the corresponding motion vector.
}

	hinclude { "GrayImage.h", "MVImage.h", "Error.h" }

//////// I/O AND STATES.
	input { name { inimage } type { packet } }
	input { name { inmvec } type { packet } }
	output { name { outimage } type { packet } }

	method {
		name { inputsOk }
		type { "int" }
		access { private }
		arglist { "(const GrayImage& gg, const MVImage& mm)" }
		code {
			int retval = (gg.retWidth() == mm.retWidth() *
					mm.retBlockSize());
			retval &= (gg.retHeight() == mm.retHeight() *
					mm.retBlockSize());
			retval &= !gg.fragmented();
			retval &= !gg.processed();
			retval &= !mm.fragmented();
			retval &= !mm.processed();
			return retval;
	}	}

	go {
// Read data from input.
		Packet imgPkt, mvPkt;
		(inimage%0).getPacket(imgPkt);
		(inmvec%0).getPacket(mvPkt);
		TYPE_CHECK(imgPkt, "GrayImage");
		TYPE_CHECK(mvPkt, "MVImage");
		GrayImage*	inImage	= (GrayImage*) imgPkt.writableCopy();
		const MVImage*	inMv	= (const MVImage*) mvPkt.myData();

// Handle null mvec fields.
		if (!inMv->retSize()) {
			Packet pkt(*inImage); outimage%0 << pkt;
			return;
		}

// Are inputs ok?
		if (!inputsOk(*inImage, *inMv)) {
			delete inImage;
			Error::abortRun(*this, "Problem with input images.");
			return;
		}

// We have a non-NULL mvec field. Add the arrows.
		const int bSize = inMv->retBlockSize();
		unsigned char* ptr = inImage->retData();
		const char* vert = inMv->constVert();
		const char* horz = inMv->constHorz();
		const int width = inImage->retWidth();
		const int height = inImage->retHeight();
		int i, j, k;

		for(i = 0; i < height; i += bSize) {
		for(j = 0; j < width; j += bSize) {
			float ii = i + bSize/2;
			float jj = j + bSize/2;
			unsigned char color;

// Set color of arrow.
			int dcVal =
					ptr[int(ii)*width+int(jj)] +
					ptr[(1+int(ii))*width+int(jj)] +
					ptr[int(ii)*width+int(jj)+1] +
					ptr[(1+int(ii))*width+int(jj)+1];
			if (dcVal > 768) { color = (unsigned char) 0; }
			else { color = (unsigned char) 255; }

// Square in center.
			ptr[int(ii)*width+int(jj)] =
			ptr[(1+int(ii))*width+int(jj)] =
			ptr[int(ii)*width+int(jj)+1] =
			ptr[(1+int(ii))*width+int(jj)+1] = color;

// Arrow
			float vertInc = float(vert[i*width/(bSize*bSize) + j/bSize])
					/ (2.0*bSize);
			float horzInc = float(horz[i*width/(bSize*bSize) + j/bSize])
					/ (2.0*bSize);
			for(k = 0; k < bSize; k++) {
				ptr[int(ii)*width + int(jj)] = color;
				ii += vertInc;	jj += horzInc;
			}
		}
		} // end for(each block)

// Send the output on its way.
		Packet outPkt(*inImage); outimage%0 << outPkt;
	}
} // end defstar { AddMvecs }
