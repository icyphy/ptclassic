defstar {
	name { FIR }
	domain { CG56 }
	desc { 
A Finite Impulse Response (FIR) filter.  Coefficients are in the "taps"
state variable.  Default coefficients give an 8th order, linear phase
lowpass filter. To read coefficients from a file, replace the default
coefficients with "<fileName".

Does not support interpolation or decimation yet.
	}
	version { $Id$ }
	author { J. Pino, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {

	}
	seealso { BiQuad, UpSample, DownSample }
	input {
		name {input}
		type {FIX}
		attributes { P_CIRC }
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {taps}
		type {FIXARRAY}
		default {
	"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
		}
		desc { Filter tap values. }
		attributes { A_ROM|A_YMEM }
		
	}
	state {
	// Not Supported Yet
		name {zeroPadding}
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
