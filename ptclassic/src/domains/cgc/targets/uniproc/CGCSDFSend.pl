defstar {
    name { SDFSend }
    domain { CGC }
    version { @(#)CGCSDFSend.pl	1.8 01/27/97  }
    derivedfrom {SDFBase}
    desc { CGC output to SDF communication  }
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
    input {
	name {input}
	type {anytype}
    }
    method {
	name { setSDFPortInfo }
	type { "void" }
	arglist { "(const char *name, DataType type, int nXfer, int mDelay)" }
	access { public }
	code {
	  sdfPortName = name;
	  sdfPortType = type ? type : ANYTYPE;
	  numXfer = nXfer;
	  maxDelay = mDelay;
	  // note trickiness: do NOT change the name assigned to "input".
	  // sdfPortName is the name to be used in the compiled star.
	  input.setPort(input.name(), this, sdfPortType, numXfer);
	  input.setSDFParams(numXfer,maxDelay);
	}
    }
    codeblock(portInit,"") {
	output {
	    name{@sdfPortName}
	    type{@sdfPortType}
	}
    }
    initCode {
	if (sdfPortType == FIX) portPrecision =input.precision();
	addCode(portInit(),"starPorts");
    CGCSDFBase::initCode();
    }
    codeblock(blockXfr,"") {
	for (int $label(i) = 0; $label(i) < @numXfer ; $label(i)++)
	    @(sdfPortName)%$label(i) << $ref(input,$label(i));
    }
    codeblock(tokenXfr,"") {
	@(sdfPortName)%0 << $ref(input);
    }
    codeblock(complexBlockXfr,"") {
	for (int $label(i) = 0; $label(i) < @numXfer ; $label(i)++) {
	    @(cgcToPtolemyTempVariable) = Complex($ref(input,$label(i)).real,
						  $ref(input,$label(i)).imag);
	    @(sdfPortName)%$label(i) << @(cgcToPtolemyTempVariable);
	}
    }
    codeblock(complexTokenXfr,"") {
	@(cgcToPtolemyTempVariable) = Complex($ref(input).real,
					      $ref(input).imag);
	@(sdfPortName)%0 << @(cgcToPtolemyTempVariable);
    }
    codeblock(fixBlockXfr,"") {
	for (int $label(i) = 0; $label(i) < @numXfer ; $label(i)++) {
	    @(cgcToPtolemyTempVariable) = 
		FIX_Fix2Double($ref(input,$label(i)));
	    @(sdfPortName)%$label(i) << @(cgcToPtolemyTempVariable);
	}    
    }
    codeblock(fixTokenXfr,"") {
	@(cgcToPtolemyTempVariable) = FIX_Fix2Double($ref(input));
	@(sdfPortName)%0 << @(cgcToPtolemyTempVariable);
    }
    go {
	if (sdfPortType == FIX) {
	    if (numXfer > 1) addCode(fixBlockXfr()); 
	    else addCode(fixTokenXfr());
	}
	else if (sdfPortType == COMPLEX) {
	    if (numXfer > 1) addCode(complexBlockXfr()); 
	    else addCode(complexTokenXfr());
	}
	else {
	    if (numXfer > 1) addCode(blockXfr()); 
	    else addCode(tokenXfr());
	}
    }
}
