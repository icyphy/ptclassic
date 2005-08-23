defstar {
    name { SDFReceive }
    domain { CGC }
    version { @(#)CGCSDFReceive.pl	1.8 01/27/97  }
    derivedfrom {SDFBase}
    desc { CGC input from SDF communication  }
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
    output {
	name {output}
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
	  // note trickiness: do NOT change the name assigned to "output".
	  // sdfPortName is the name to be used in the compiled star.
	  output.setPort(output.name(), this, sdfPortType, numXfer);
	  output.setSDFParams(numXfer,maxDelay);
	}
    }
    codeblock(portInit,"") {
	input {
	    name{@sdfPortName}
	    type{@sdfPortType}
	}
    }
    initCode {
	if (sdfPortType == FIX) portPrecision = output.precision();
	addCode(portInit(),"starPorts");
    CGCSDFBase::initCode();
    }
    codeblock(blockXfr,"") {
	for (int $label(i) = 0; $label(i) < @numXfer ; $label(i)++)
	    $ref(output,$label(i)) = @(sdfPortName)%$label(i);
    }
    codeblock(tokenXfr,"") {
	$ref(output) = @(sdfPortName)%0;
    }
    codeblock(complexBlockXfr,"") {
	for (int $label(i) = 0; $label(i) < @numXfer ; $label(i)++) {
	    @(cgcToPtolemyTempVariable) = @(sdfPortName)%$label(i);
	    $ref(output,$label(i)).real = @(cgcToPtolemyTempVariable).real();
	    $ref(output,$label(i)).imag = @(cgcToPtolemyTempVariable).imag();
	}
    }
    codeblock(complexTokenXfr,"") {
	@(cgcToPtolemyTempVariable) = @(sdfPortName)%0;
	$ref(output).real = @(cgcToPtolemyTempVariable).real();
	$ref(output).imag = @(cgcToPtolemyTempVariable).imag();
    }
    codeblock(fixBlockXfr,"") {
	for (int $label(i) = 0; $label(i) < @numXfer ; $label(i)++) {
	    // It would be better here to get direct access to the
		// bits of the Fix class and copy them over.
		    // Unfortunately, there is no public method to do this
			// in Fix.h.  FIXME
			    double temp = @(sdfPortName)%$label(i);
	    FIX_DoubleAssign($ref(output,$label(i)),temp);
	}
    }
    codeblock(fixTokenXfr,"") {
	// It would be better here to get direct access to the
	    // bits of the Fix class and copy them over.
		// Unfortunately, there is no public method to do this
		    // in Fix.h.  FIXME
			double temp = @(sdfPortName)%0;
	FIX_DoubleAssign($ref(output),temp);
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


