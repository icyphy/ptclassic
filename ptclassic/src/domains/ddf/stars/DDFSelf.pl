ident {
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90
 Converted to use preprocessor, 10/5/90

 DDFSelf is a star for recursion. At runtime, it appears to contain
 a clone of the galaxy (recursion) and execute it. At compile time,
 it is just a atomic star.

*******************************************************************/
}
defstar {
	name { Self }
	domain { DDF }
	desc { "This star realizes recursion" }
	hinclude { "InterpGalaxy.h" }
	ccinclude { "KnownBlock.h", "DDFScheduler.h" }
	protected {
		InterpGalaxy* gal;
	}
	inmulti {
		name { input }
		type { ANYTYPE }
	}
	outmulti {
		name { output }
		type { ANYTYPE }
	}
// made one change: the value "", not NULL, is given to indicate recurGal
// not set.
	defstate {
		name { recurGal }
		type { string }
		default { "" }
		desc { "name of recursive galaxy" }
	}
// isItSelf method.
	method {
		name { isItSelf }
		access { public }
		type { int }
		arglist { "()" }
		code { return TRUE;}
	}
	start {
		StringList msg = "DDFSelf Star \"";
		msg += readName();
		msg += "\" :\n";
		int flag = 0;

		// find out which galaxy is recursive.
		if (*(recurGal.getInitValue()) == 0) {
			msg += "undefined name for recursion construct\n";
			Error :: abortRun(msg);
			return;
		}

		Block* b = parent();
		while (b && strcmp(b->readName(), recurGal.getInitValue()))
			b = b->parent();
		if (!b) {
			msg += "unmatched name for recursion construct\n";
			Error :: abortRun(msg);
			return;
		}
		gal = (InterpGalaxy*) &(b->asGalaxy());

		// match portholes.
		int inNum = 0, outNum = 0;
		BlockPortIter next(*gal);
		for (int i = gal->numberPorts(); i > 0; i--) {
			PortHole& p = *next++;
			if (p.isItInput()) inNum++;
			else outNum++;
			DDFPortHole* dp = (DDFPortHole*) portWithName(p.readName());
			if (!dp) {
				msg += "wrong galaxy portname for recursion \"";
				msg += p.readName();
				msg += "\" (should be input#? or output#?)\n";
				flag++;
			} else {
				dp->imagePort = &p;
			}
		}
		if (input.numberPorts() != inNum || output.numberPorts() != outNum) {
			msg += "unmatched number of ports with recursion galaxy\n";
			flag++;
		}
		if (flag) Error::abortRun(msg);
	}
	go {
		// clone the recursion galaxy
		InterpGalaxy* myGal = (InterpGalaxy*) gal->clone();
		myGal->setNameParent(readName(), this);

		// create its own scheduler
		DDFScheduler* sched = new DDFScheduler;

		// make a image connection between DDFSelf and galaxy.
		BlockPortIter next(*myGal);
		for (int i = myGal->numberPorts(); i > 0; i--) {
			PortHole& p = *next++;
			DDFPortHole* dp = (DDFPortHole*) portWithName(p.readName());
			dp->imagePort = &p;

			// make connection
			dp->imageConnect();
		}

		// scheduler setup and run
		if (!sched->setup(*myGal)) {
			Error :: abortRun("error in setup the scheduler in DDFSelf\n");
			return;
		}

		// move data into galaxy
		MPHIter nexti(input);
		for (i = input.numberPorts(); i > 0; i--) {
			InDDFPort& p = (InDDFPort&) *nexti++;
			p.grabData();
			p.moveData();
		}

		// scheduler run
		sched->run(*myGal);	// default stop condition = 1

		// move data out of galaxy
		MPHIter nexto(output);
		for (i = output.numberPorts(); i > 0; i--) {
			OutDDFPort& p = (OutDDFPort&) *nexto++;
			p.moveData();
			p.sendData();
		}

		// delete all
		delete myGal;
		delete sched;
	}
}

