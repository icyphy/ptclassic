defstar {
    name { SDFBase }
    domain { CGC }
    version { @(#)CGCSDFBase.pl	1.8 01/27/97 }
    desc { Base comm star b/w CGC & SDF  }
    author { Jose Luis Pino }
    copyright {
Copyright (c) 1995-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC target library }
    htmldoc {
    }
    public {
	virtual void setSDFPortInfo (const char* /*name*/, DataType /*type*/,
				     int /*numXfer*/, int /*maxDelay*/) {}

	/*virtual*/ Block* clone() const {
	    CGCSDFBase* newStar = (CGCSDFBase*) CGCStar::clone();
	    newStar->sdfPortName = sdfPortName;
	    newStar->sdfPortType = sdfPortType;
	    newStar->numXfer = numXfer;
	    newStar->maxDelay = maxDelay;
	    newStar->portPrecision = portPrecision;
	    return newStar;
	}
    }

    protected {
	StringList sdfPortName;
	DataType sdfPortType;
	int numXfer;
	int maxDelay;
	Precision portPrecision;
	StringList cgcToPtolemyTempVariable;
    }
    
    method {
	name { addCgcToPtolemyDeclaration }
	type { "void" }
	arglist { "(const char* type)" }
	access { protected }
	code {
	    cgcToPtolemyTempVariable << sdfPortName << type;
	    StringList declare;
	    declare << type << " " << cgcToPtolemyTempVariable << ";";
	    addCode(declare,"mainDecls");		
	}
    }
    
    codeblock(sdfParams,"") {
	@(sdfPortName).setSDFParams(@(numXfer),@(maxDelay));
    }

    initCode {
	if (numXfer > 1) addCode(sdfParams(),"starSetup");
	if (sdfPortType == FIX) {
	    addCgcToPtolemyDeclaration("Fix");
	    @	@cgcToPtolemyTempVariable
	    @	    = Fix(@(portPrecision.len()),@(portPrecision.intb()));
	    @	@cgcToPtolemyTempVariable.set_rounding(1);
	}
	else if (sdfPortType == COMPLEX) {
	    addCgcToPtolemyDeclaration("Complex");
	}
    }
}
