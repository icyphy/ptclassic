defstar {
	name { Rect }
	domain { CG56 }
	desc { Rectangular Pulse Generator }
	version { @(#)CG56Rect.pl	1.13 3/27/96 }
	author { Brian L. Evans and Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	output {
		name { output }
		type { fix }
	}
	state {
		name { height }
		type { fix }
		desc { height of rectangular pulse }
		default { ONE }
		attributes { A_YMEM }
	}
	state {
		name { width }
		type { int }
		desc { width of rectangular pulse }
		default { 1 }
	}
	state {
		name { period }
		type { int }
		desc { period of pulse }
		default { 10 }
	}
	state {
		name { count }
		type { int }
		desc { internal counting state }
		default { 0 }
		attributes { A_YMEM|A_NONCONSTANT|A_NONSETTABLE }
	}

	codeblock(sendOutput) {
; Register usage:
; x0 = 0
; x1 = count
; y1 = height
; a = output value
; b = width
	clr	b	$ref(count),x1		; a = 0, x1 = count, b = 0
	clr	a	#$val(width),b1		; b1 = width
	move	a,x0	$ref(height),y1		; x0 = 0, y1 = height
	cmp	x1,b	#1,b1		; if (count < width)
	tgt	y1,a			; then a = height
	move	a,$ref(output)			; output = a
	}

	codeblock(updateCounter) {
; a = period
; b = updated count
	add	x1,b			; b = count + 1
	move	#$val(period),a1		; a = period
	cmp	a,b			; if (period <= count)
	tge	x0,b			; then b = 0
	move	b,$ref(count)			; count = b
	}

	setup {
		int w = int(width);
		int p = int(period);

		if (w < 1) {
			Error::abortRun(*this, "Invalid width value.");
		}
		else if (p < 1) {
			Error::abortRun(*this, "Invalid period value.");
		}
		else if (p <= w) {
			Error::abortRun(*this,
					"Period must be greater than width");
		}
	}			      
	go {
		addCode(sendOutput);
		addCode(updateCounter);
	}
	execTime {
		return 11;
	}
}
