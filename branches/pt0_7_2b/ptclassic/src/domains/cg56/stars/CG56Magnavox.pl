defstar {
	name { Magnavox }
	domain { CG56 }
	desc { 
DSP56000 -  A combined input/output star for the Magnavox CD player.
	}
	version { $Id$ }
	author { J. Pino, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
DSP56000 -  A combined input/output star for the Magnavox CD player.
	}
	input {
		name {in1}
		type {FIX}
	}
	input {
		name {in2}
		type {FIX}
	}
	output {
		name {out2}
		type {FIX}
	}
	output {
		name {out2}
		type {FIX}
	}
	state {
		name {forceInterrupts}
		type {int}
		default {NO}
		desc { If YES use interrupts, otherwise use polling. }
		
	}
	state {
		name {in}
		type {FIXARRAY}
		default {""}
		desc { Zero Padding. }
		attributes { A_NONSETTABLE|A_ROM|A_XMEM }
	}
	state {
	// Not Supported Yet
		name {decimation}
		type {int}
		default {1}
		desc {Decimation ratio.}
		attributes { A_NONSETTABLE }
	}
	state {
	// Not Supported Yet
		name {decimationPhase}
		type {int}
		default {0}
		desc {Downsampler phase.}
		attributes { A_NONSETTABLE }
	}
	state {
	// Not Supported Yet
		name {interpolation}
		type {int}
		default {1}
		desc {Interpolation ratio.}
		attributes { A_NONSETTABLE }
	}
	protected {
		int phaseLength;
	}
	start {
		int d = decimation;
		int i = interpolation;
		int dP = decimationPhase;
		input.setSDFParams(d, d+1+(taps.size()/i));
		output.setSDFParams(i, i-1);
		if (dP >= d) {
			Error::abortRun (*this, ": decimationPhase too large");
			return;
		}
		phaseLength = ceil(double(int(taps.size()))/double(i));
	}
	codeblock (fir1) {
        move	#$addr(input),r5
        move	#$addr(taps),r0
	nop
        clr	a	$mem(input):(r5)-,x1	$mem(taps):(r0)+,y1
        rep	#$size(taps)
        mac	x1,y1,a	$mem(input):(r5)-,x1	$mem(taps):(r0)+,y1
	move	a,$ref(output)
	}
	go {
		gencode(fir1);
	}
	execTime {
		return 1;
	}
}
