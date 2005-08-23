defstar {
	name { HostSldrGX }
	domain { CG56 }
	desc { Host port Slider }
	version { @(#)CG56HostSldrGX.pl	1.7	03/29/97 }
	acknowledge { Gabriel version by Phil Lapsley }
	author { Chih-Tsung Huang, ported from Gabriel }
	ccinclude { "StringState.h" "CGTarget.h" "StringList.h"}
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="slider bar"></a>
<a name="gxslider program"></a>
Host port slider.
The parameters <i>hostMin</i>, <i>hostMax</i>, and <i>hostInitialValue</i>
control the values that the slider is labeled with under X windows.
They are mapped in a linear or decibel fashion into the range
of the parameters <i>dspMin</i> and <i>dspMax</i> on the DSP,
depending on whether <i>scale</i> is linear or log.
	}
	output {
		name {output}
		type { FIX }
	}
	state {
		name { description }
		type { string }
		desc { put a description }
		default { "Slider" }
	}
	state {
		name { hostMin }
		type { int }
		desc { minimum }
		default { 0 }
	}
	state {
		name { hostMax }
		type { int }
		desc { maximum }
		default { 100 }
	}
	state {
		name { hostInitialValue }
		type { int }
		desc { initial value }
		default { 0 }
	}
	state {
		name { dspMin }
		type { fix }
		desc { minimum }
		default { 0.0 }
	}
	state {
		name { dspMax }
		type { fix }
		desc { maximum }
		default { ONE }
	}
	state {
		name { scale }
		type { string }
		desc { type of scaler }
		default { "linear" }
	}
	state {
		name { value }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	state {
		name { initVal }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE}
	}
	state {
		name { initValdB }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE}
	}    
	codeblock(aio) {
slider $starSymbol(SLIDER) $fullname() $val(hostMin) $val(hostMax) $val(hostInitialValue) $val(dspMin) $val(dspMax) $val(scale) "$val(description)"
	}

codeblock(orgout) {
	org	$ref(output)
$starSymbol(SLIDER)
	dc	$val(initVal)
	org	p:
}

	codeblock(orgval) {
	org	$ref(value)
$starSymbol(SLIDER)
	dc	$val(initVal)
	org	p:
	}

	codeblock (done) {
	move	$ref(value),x0		; move value to output
	move	x0,$ref(output)
	}

	setup {
		if (1) output.setAttributes(P_NOINIT);
	}
	
	initCode {
		StringList cmd, base;
		base << *((StringState*) cgTarget()->stateWithName("file"));
		cmd << "gxslider -t " <<  base << " " << base << ".lod "
		    << base << ".aio " << "\n";
		addCode(cmd,"shellCmds","Slider");
		addCode(aio,"aioCmds");
		int t1 = hostInitialValue - hostMin;
		int hrange = hostMax - hostMin;
		double drange = dspMax.asDouble() - dspMin.asDouble();
		double ival = double(t1) / hrange * drange + dspMin.asDouble();
		initVal = ival;
		initValdB= pow(10.0, ival / 20.0);

		const char* p=scale;
		switch(*p) {
			case 'l': case 'L':	// linear
				break;
			case 'd': case 'D':	//dB
				initVal=initValdB;
				break;
			default:
				Error::abortRun(*this, "string must be linear or db");
			return;
	}
		if(1) 
			addCode(orgout);
		else 
			addCode(orgval);
		}
 
	go {
		if (1)
			return; // no runtime code;
		else
			addCode(done);
	}

	exectime {
			return 1;
	}

}
