defstar {
	name { SDFBase }
	domain { CGC }
	version { $Id$ }
	desc { Base comm star b/w CGC & SDF  }
	author { Jose Luis Pino }
	copyright {
Copyright (c) 1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC target library }
	explanation {
	}
	public {
	    DataType sdfPortType;
	    StringList sdfPortName;
	    int numXfer;
	    int maxDelay;

	    /*virtual*/ Block* clone() const {
		CGCSDFBase* newStar = (CGCSDFBase*) CGCStar::clone();
		newStar->sdfPortType = sdfPortType;
		newStar->sdfPortName = sdfPortName;
		newStar->numXfer = numXfer;
		newStar->maxDelay = maxDelay;
		newStar->fixName = fixName;
		newStar->portPrecision = portPrecision;
		return newStar;
	    }
	}

	protected {
	    StringList fixName;
	    StringList cxName;
	    Precision portPrecision;
	}

	codeblock(sdfParams,"") {
	    @(sdfPortName).setSDFParams(@(numXfer),@(maxDelay));
	}

	initCode {
	    if (numXfer > 1) addCode(sdfParams(),"starSetup");
	    if (sdfPortType == FIX) {
		fixName << sdfPortName << "Fix";

		StringList declare;
		declare << "Fix " << fixName << ";";
		addCode(declare,"mainDecls");

		@ @fixName = Fix(@(portPrecision.len()),@(portPrecision.intb()));
		@ @fixName.set_rounding(1);
	    }
	    else if (sdfPortType == COMPLEX) {
	      cxName << sdfPortName << "Cx";

	      StringList declare;
	      declare << "Complex " << cxName << ";";
	      addCode(declare,"mainDecls");
	    }
	}
}
