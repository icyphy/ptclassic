defstar {
	name		{ ImageToCell }
	domain		{ DE }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ DE main library }
	desc {
This star packetizes an image. Each image is divided up into chunks
no larger than "CellSize". Each cell is delayed from its
predecessor by "TimePerCell". If a new input arrives while an
older one is being processed, the new input is queued.
	}

	input { name { input } type { message } }
	output { name { output } type { message } }
	defstate {
		name { Priority }
		type { int }
		default { 0 }
		desc { Priority level for outgoing cells. }
	}
	defstate {
		name	{ CellSize }
		type	{ int }
		default { 256 }
		desc	{ Cell size in bytes. }
	}
	defstate {
		name	{ TimePerCell }
		type	{ float }
		default { 0.005 }
		desc	{ Time to process one cell. }
	}
	defstate {
		name	{ destination }
		type	{ int }
		default { 0 }
		desc	{ Destination addresses. }
	}

// CODE
	hinclude { "BaseImage.h", "NetworkCell.h", "Error.h", <minmax.h> }

	go {
		completionTime = max(completionTime, arrivalTime);

// Read input image.
		Envelope inEnvp;
		input.get().getMessage(inEnvp);
		TYPE_CHECK(inEnvp, "BaseImage");
		const BaseImage* image = (const BaseImage*) inEnvp.myData();

// Do something.
		int counter = 0;
		BaseImage* imagePtr = image->fragment(int(CellSize), counter++);
		while (imagePtr != (BaseImage*) NULL) {
			completionTime += double(TimePerCell);

			LOG_NEW; NetworkCell* cell = new NetworkCell(*imagePtr,
					int(Priority), int(destination),
					int(imagePtr->retSize()));
			Envelope outEnvp(*cell);
			output.put(completionTime) << outEnvp;

			imagePtr = image->fragment(int(CellSize), counter++);
		}
	}
} // end defstar{ ImageToCell }
