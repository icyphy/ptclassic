defstar {
	name { CellUnload }
	domain { DE }
	author { GSWalter }
	version { $Id$ }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
		}
	location { DE main palette }
	desc {
Remove a Message from a NetworkCell.
	}

	input { name { input } type { message } }
	output { name { output } type { message } }

	hinclude { "NetworkCell.h" }

	go {
		while (input.dataNew) {
			Envelope inEnvlp;
			input.get().getMessage(inEnvlp);
			TYPE_CHECK(inEnvlp, "NetworkCell");

// Need "writableCopy()" and "writableData()" below because the Envelope
// constructor below cannot take a const Message as input.
			NetworkCell* cellPtr = (NetworkCell*)
					inEnvlp.writableCopy();
			Message* outMssg = cellPtr->writableData();
			LOG_DEL; delete cellPtr;

			Envelope outEnvlp(*outMssg);
			output.put(arrivalTime) << outEnvlp;

			input.getSimulEvent();
		} // end while()
	} // end go{}
} // end defstar{}
