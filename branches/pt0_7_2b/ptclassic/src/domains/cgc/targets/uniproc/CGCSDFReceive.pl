defstar {
	name { SDFReceive }
	domain { CGC }
	version { $Id$  }
	derivedfrom {SDFBase}
	desc { CGC input from SDF communication  }
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
	output {
	    name {output}
	    type {anytype}
	}
	setup {
	    maxDelay = output.maxDelay();
	    numXfer = output.numXfer();
	    output.setResolvedType(sdfPortType);
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
		$ref(output,$label(i)) = @(sdfPortName)%$label(i) ;
	}
	codeblock(tokenXfr,"") {
	    $ref(output) = @(sdfPortName)%0;
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
	    else {
		if (numXfer > 1) addCode(blockXfr()); 
		else addCode(tokenXfr());
	    }
	}
}
