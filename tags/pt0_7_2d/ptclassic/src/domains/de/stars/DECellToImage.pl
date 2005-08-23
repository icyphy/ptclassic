defstar {
	name		{ CellToImage }
	domain		{ DE }
	version		{ @(#)DECellToImage.pl	1.8 10/28/95 }
	author		{ Paul Haskell }
	location	{ DE main library }
        copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	desc {
This star reads in NetworkCells containing image data and outputs whole
images. The current image is sent to the output when the star reads
image data with a higher frame id than the current image.
For each frame, the fraction of input data that was lost is sent to the
"lossPct" output.
	}

	input { name { input } type { message } }
	output { name { output } type { message } }
	output {
		name { lossPct }
		type { float }
		desc { Fraction of bytes lost from an image. }
	}

//// CODE
	hinclude { "BaseImage.h", "NetworkCell.h", "Error.h" }

	protected {
// For persistency, we must store the image that is being built up
// inside an Envelope variable. Note that we save a BaseImage in
// activeEnvp, not a NetworkCell. For consistency, we must make sure
// the image being built up in activeEnvp is not read-only.
// This involves some games below...

		Envelope activeEnvp;
		BaseImage* activeImage;		// not dynamic memory
		float actual;			// # bytes in images.
		NetworkCell* firstNC;		// dynamic memory
	}

	constructor {
		activeImage = 0;
		firstNC = 0;
	}

	destructor {
		delete firstNC;
	}

	setup {
		Envelope nullEnvp;
		activeEnvp = nullEnvp;
		activeImage = 0;

		delete firstNC;
		firstNC = 0;
	}

	go {
		completionTime = arrivalTime;

		// Read input image.
		Envelope inEnvp;
		input.get().getMessage(inEnvp);
		TYPE_CHECK(inEnvp, "NetworkCell");

		if (!activeImage) {
			// Here, activeEnvp is set to contain a WRITABLE
			// copy of the incoming BaseImage.
			NetworkCell* tmpNC =
				(NetworkCell*) inEnvp.writableCopy();
			if (!tmpNC->contentsType("BaseImage")) {
				LOG_DEL; delete tmpNC;
				Error::abortRun(*this, "Cell doesn't hold BaseImage.");
				return;
			}
			firstNC = tmpNC;	    // keep for later deletion
			activeImage = (BaseImage*) tmpNC->writableData();
			Envelope tmpE(*activeImage);
			activeEnvp = tmpE;
			actual = activeImage->retSize();

		} else { // If here, we have already read one envelope.
			NetworkCell* tmpNC =
				(NetworkCell*) inEnvp.writableCopy();
			if (!tmpNC->contentsType("BaseImage")) {
				LOG_DEL; delete tmpNC;
				Error::abortRun(*this,
						"Cell doesn't hold BaseImage.");
				return;
			}
			BaseImage* newPtr = (BaseImage*) tmpNC->writableData();

			if (*activeImage == *newPtr) { // same frame id's...
				activeImage->assemble(newPtr);
				actual += newPtr->retSize();
				LOG_DEL; delete newPtr;
			} else {	// Send old image to output port.
				float total = activeImage->retFullSize();
				Envelope outEnvp(*activeImage);
				output.put(completionTime) << outEnvp;
				if (total > 0.0)
					lossPct.put(completionTime) << (1.0-(actual/total));
				else
					lossPct.put(completionTime) << 0.0;

				// Set activeEnvp to contain a writable copy
				// of the new BaseImage.
				activeImage = newPtr;
				Envelope tmpE(*activeImage); activeEnvp = tmpE;
				actual = activeImage->retSize();
			}
			LOG_DEL; delete tmpNC; // we "own" it, must clean it up!
		}
	} // end go{}
} // end defstar{ CellToImage }
