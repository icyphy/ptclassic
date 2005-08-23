defstar {
	name { Self }
	domain { DDF }
	desc {
This is a first exploration of recursion and higher-order functions
expanded while the system is running.  This is in contrast to the
higher-order function stars like HOFChain, HOFMap, and HOFMapGr that
expand the system during compilation and incur no run-time penalty.
The DDFSelf star is still experimental, so do not expect it to be
either efficient or bug-free.

The star "represents" the galaxy given by the parameter "recurGal",
which must be above it in the hierarchy.  That is, when the Self
star fires, it actually invokes the galaxy that it represents.
Since that galaxy is above the Self star in the hierarchy, it
contains the Self star somewhere within it.  Thus, this star
implements recursion.  Since the Self star takes an argument
("recurGal") that specifies the function to invoke, it is itself
a higher-order function.

The instance of the "recurGal" galaxy is not created until it is
actually needed, so the number of instances (the depth of the
recursion) does not need to be known a-priori.  If the parameter
"reinitialize" is FALSE, then the instance of the galaxy is created
the first time it fires and reused on subsequent firings.
If "reinitialize" is TRUE, then the galaxy is created on every firing
and destroyed after the firing.  Inputs are sent to the instance
of the galaxy and outputs are retrieved from it.  The inputs of the
named galaxy must be named "input#?" and the outputs must be named
"output#?", where "?" is replaced with an integer starting with zero.
This allows the inputs and outputs of this star to be matched
unambiguously with the inputs and outputs of the referenced galaxy.
	}
	version { @(#)DDFSelf.pl	2.30	12/08/97 }
	author { Soonhoi Ha and Edward A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DDF library }
	htmldoc {
Each input must have exactly one token for the star to fire.
The number of outputs produced can depend on the internal galaxy.
<p>
Creating a new instance of the galaxy is quite an expensive operation,
so setting <i>reinitialize</i> to TRUE will cause dramatically slower
execution.  Thus this should not be used unless it is absolutely necessary.
	}
	seealso { fibonnacci, HOFChain, HOFMap, HOFMapGr }
	hinclude { "InterpGalaxy.h",  "Domain.h", "DDFScheduler.h"}
	inmulti {
		name { input }
		type { ANYTYPE }
		descriptor { Inputs to match with galaxy ports named input#? }
	}
	outmulti {
		name { output }
		type { ANYTYPE }
		num { 0 }
		descriptor { Outputs to match with galaxy ports named input#? }
	}
// made one change: the value "", not NULL, is given to indicate recurGal
// not set.
	defstate {
		name { recurGal }
		type { string }
		default { "" }
		desc { Name of galaxy to be invoked. }
	}
	defstate {
		name { reinitialize }
		type { INT }
		default { "NO" }
		descriptor { Initialize the galaxy on each firing }
	}
// isItSelf method.
	method {
		name { isItSelf }
		access { public }
		type { int }
		arglist { "()" }
		code { return TRUE;}
	}
	protected {
		InterpGalaxy* masterGal;
		InterpGalaxy* myGal;
		DDFScheduler sched;
	}
	constructor {
		masterGal = 0;
		myGal = 0;
	}
	destructor {
		LOG_DEL; delete myGal;
	}
	setup {
	    // If a galaxy already exists, just initialize it
	    if (myGal) {
		myGal->initialize();
	    }
	    else {
		// The following should not be required because of the
		// num { 0 } above.  At one time, this did not work.
		// output.setDDFParams(0);

		StringList msg = "DDFSelf Star \"";
		msg << name() << "\" :\n";
		int flag = 0;

		// find out which galaxy is recursive.
		if (recurGal.null()) {
			msg << "undefined name for recursion construct\n";
			Error :: abortRun(*this, msg);
			return;
		}

		Block* b = this;
		do {
		    b = b->parent();
		    if (!b || (b->isA("InterpUniverse"))) {
			msg << "Unmatched name "
			    << (const char*)recurGal
			    << " for recursion construct\n";
			Error :: abortRun(*this, msg);
			return;
		    }
		    if(!(b->isA("InterpGalaxy"))) {
			msg << "Sorry, recursion only works for InterpGalaxies";
			Error :: abortRun(*this, msg);
			return;
		    }
		    masterGal = (InterpGalaxy*)b;
		} while (strcmp(masterGal->className(), recurGal.currentValue())
			!= 0);

		// match portholes.
		int inNum = 0, outNum = 0;
		BlockPortIter next(*masterGal);
		for (int i = masterGal->numberPorts(); i > 0; i--) {
			PortHole& p = *next++;
			if (p.isItInput()) inNum++;
			else outNum++;
			DDFPortHole* dp = (DDFPortHole*) portWithName(p.name());
			if (!dp) {
				msg << "wrong galaxy portname for recursion \""
				    << p.name()
				    << "\" (should be input#? or output#?)\n";
				flag++;
			}
			else {
				dp->imagePort = &p;
			}
		}
		if (input.numberPorts() != inNum ||
		    output.numberPorts() != outNum) {
			msg << "unmatched number of ports with recursion galaxy\n";
			flag++;
		}
		if (flag) Error :: abortRun(*this, msg);
	    }
	}
	go {
	    int j;
	    if (myGal == 0) {
		// clone the recursion galaxy
		myGal = (InterpGalaxy*) masterGal->clone();
		myGal->setNameParent(masterGal->name(), this);
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
			Error :: abortRun(*this,
					  "error in setting up the scheduler");
			return;
		}
	    } else {
		if (int(reinitialize)) myGal->initialize();
	    }

	    // move data into galaxy
	    MPHIter nexti(input);
	    for (j = input.numberPorts(); j > 0; j--) {
	    	InDDFPort& p = *(InDDFPort*) nexti++;
	    	p.receiveData();
		if (Scheduler :: haltRequested()) return;
	    	p.moveData();
	    }

	    // scheduler run.  Unless we reinitialize, we increment the
	    // stop time by one each time through.
	    sched.setStopTime(sched.getStopTime() + 1.0);
	    sched.run();

	    // move data out of galaxy
	    int numXfred = 0;
	    MPHIter nexto(output);
	    for (j = output.numberPorts(); j > 0; j--) {
	    	OutDDFPort& p = *(OutDDFPort*) nexto++;
	    	numXfred = p.moveData();
		for (int c = numXfred; c > 0; c--) p.sendData();
	    }
	}
}
