defstar {
name { FFTCx }
domain { CG56 }
desc {
}
version { $Id$ }
author { Jose Luis Pino, ported from Gabriel }
copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
location { CG56 dsp library }
explanation {
}
input {
	name {input}
	type {COMPLEX}
	attributes {A_CIRC}
}
output {
	name {output}
	type {COMPLEX}
}
state { 
	name{order}
	type{int}
	desc { FFT order, must be a power of 2}
	default {128}
}
state {
	name {direction}
	type {Float}
	default {"1.0"}
	desc { Forward (1.0) or Reverse (-1.0)FFT. }
}
state {
	name {twdfct}
	type {FIXARRAY}
	attributes {A_RAM|A_CONSTANT|A_NONSETTABLE|A_SYMMETRIC|A_NOINIT|A_CIRC}
}
state {
	name {halfOrder}
	type {int}
	attributes{A_CONSTANT|A_NONSETTABLE}
	default{64}
}
setup {
	twdfct.resize(order/2);
	input.setSDFParams(order,order-1);
	output.setSDFParams(order,order-1);
}
initCode {
	halfOrder.setInitValue(order/2);
	if (addCode("; Include FFT macro","code","FFT_Macro")) {
		StringList macro = "	include '";
		macro << expandPathName("$PTOLEMY/lib/cg56/fftr2a.asm'\n");
		addCode(macro);
	}
	StringList twcos, twsin;
	for(int i = 0; i < int(halfOrder); i++) {
		double dtwcos,dtwsin;
		dtwcos = -1*cos (i*2.0*M_PI/double(order));
		dtwsin = double(direction)* -1*sin(i*2.0*M_PI/double(order));
		twcos << "	dc	"<< dtwcos << "\n";
		twsin << "	dc	"<< dtwsin << "\n";
	}
	addCode("; Twiddle factors\n    org     x:$addr(twdfct)");
	addCode(twcos);
	addCode("	org	y:$addr(twdfct)");
	addCode(twsin);
	addCode("	org	p:\n");
}
go {
	addCode(dofft);
}
codeblock (dofft) {
; Perform the FFT
	fftr2a	$val(order),$addr(input),$addr(twdfct)

; Copy data into output buffers and bit reverse
	move	#$val(halfOrder),n6
	move	#$addr(input),r6
;	m6 is already set to 0 by fft macro
	move	#$addr(output),r0
	do	#$val(order),$label(FFTLoop)
	move	L:(r6)+n6,x
	move	x,L:(r0)+
$label(FFTLoop)
	move	m0,m6
}
}
          
