defstar {
	name { Self }
	domain { DDF }
	desc { Realizes recursion. }
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DDF library }
	explanation {
This is a star for recursion.  At runtime, it appears to contain
a clone of the galaxy whose name is given by "recurGal".
.Ir "recursion"
That galaxy is supplied with the input particles and executed.
That galaxy may be one within which this instance of Self resides,
hence realizing recursion.
At compile time, this star appears to just be an atomic star.
	}
	seealso { fibonnacci }
	hinclude { "InterpGalaxy.h",  "Domain.h", "DDFScheduler.h"}
	protected {
		InterpGalaxy* gal;
		DDFScheduler  sched;
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
		desc { Name of galaxy to be invoked. }
	}
// isItSelf method.
	method {
		name { isItSelf }
		access { public }
		type { int }
		arglist { "()" }
		code { return TRUE;}
	}
	method {
		name { readTypeName }
		access { public }
		type { "const char*" }
		code { return "Self"; }
	}
	constructor {
		gal = 0;
	}	
	start {
		if (gal) return;

		StringList msg = "DDFSelf Star \"";
		msg += name();
		msg += "\" :\n";
		int flag = 0;

		// find out which galaxy is recursive.
		if (recurGal.null()) {
			msg += "undefined name for recursion construct\n";
			Error :: abortRun(msg);
			return;
		}

		Block* b = parent();
		while (b && strcmp(b->name(), recurGal) != 0)
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
			DDFPortHole* dp = (DDFPortHole*) portWithName(p.name());
			if (!dp) {
				msg += "wrong galaxy portname for recursion \"";
				msg += p.name();
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
		myGal->setNameParent(gal->name(), this);
		sched.resetFlag();

		// make a image connection between DDFSelf and galaxy.
		BlockPortIter next(*myGal);
		for (int i = myGal->numberPorts(); i > 0; i--) {
			PortHole& p = *next++;
			DDFPortHole* dp = (DDFPortHole*) portWithName(p.name());
			dp->imagePort = &p;

			// make connection
			dp->imageConnect();
		}

		// scheduler setup and run
		sched.setGalaxy(*myGal);
		sched.setup();
		if (Scheduler::haltRequested()) {
			Error :: abortRun("error in setup the scheduler in DDFSelf\n");
			return;
		}

		// move data into galaxy
		MPHIter nexti(input);
		for (i = input.numberPorts(); i > 0; i--) {
			InDDFPort& p = *(InDDFPort*) nexti++;
			p.receiveData();
			p.moveData();
		}

		// scheduler run
		sched.run();	// default stop condition = 1

		// move data out of galaxy
		MPHIter nexto(output);
		for (i = output.numberPorts(); i > 0; i--) {
			OutDDFPort& p = *(OutDDFPort*) nexto++;
			p.moveData();
			p.sendData();
		}

		// delete all
		LOG_DEL; delete myGal;
	}
}

