defstar {
	name { Limit }
	domain { C50 }
	desc { Hard limiter }
	version { @(#)C50Limit.pl	1.5	06 Oct 1996 }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="hard limiter"></a>
The star hard limits the values of input samples to be in the range
(<i>bottom</i>, <i>top</i>).
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {bottom}
		type {FIX}
		default {0}
		desc {Lower limit of the output}
	}
	state {
		name {top}
		type {FIX}
		default {ONE}
		desc {Upper limit of the output}
	}

	codeblock(limitblock) {
	mar	*,AR1				;
	lar	AR1,#$addr(top)			;Address top		=> AR1
	lar	AR0,*,AR0			;AR0 = value top
	lmmr	ARCR,#$addr(input)		;ARCR = input
	cmpr	2				;AR0 > ARCR ?
	bcnd	$label(end),TC			;if yes jump to label(end)
	mar 	*,AR1				;
	lar	AR1,#$addr(bottom)		;Address bottom		=> AR1
	lar	AR0,*,AR0			;AR0 = value top
	cmpr	1				;AR0 < ARCR
 	bcnd	$label(end),TC			;if yes fump to label(end)
	lar	AR1,#$addr(input)		;Address input		=> AR1
$label(end)
	mar	*,AR1				;
	bldd	*,#$addr(output)		;output = val at address in AR1
	}

	constructor {
		noInternalState();
	}
 	go {
 		addCode(limitblock);
 	}

	exectime {
		return 14;
 	}
}
