defstar {
	name { Rect }
	domain { CG56 }
	desc { Rectangular Pulse Generator }
	version { $Id$ }
	author { Brian L. Evans and Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	output {
		name {output}
		type {fix}
	}
	state {
		name {height}
		type {fix}
		desc { height of rectangular pulse }
		default { ONE }
		attributes { A_YMEM }
	}
	state {
		name {width}
		type {int}
		desc { width of rectangular pulse }
		default { 1 }
	}
	state {
		name {period}
		type {int}
		desc { period of pulse }
		default { 10 }
	}
	state {
		name { count }
		type { int }
		desc { internal counting state }
		default { 0 }
	}

	codeblock(sendOutput) {
; Register usage:
; x0 = 1
; x1 = width
; y0 = period
; y1 = height
; a = output value
; b = count
	clr	a	move	$ref(count),b	; a = 0, b = count
	move	#$val(width),x1			; x1 = width
	move	$ref(height),y1			; y1 = height
	cmp	x1,b	#1,x0		; if (width > count)
	tgt	y1,a			; then a = height
	move	a,$ref(output)			; output = a
	}

	codeblock(updateCounter) {
	add	x0,b	$ref(period),y0		; b++, y0 = period
	cmp	y0,b	#0,a		; if (period <= count)
	tle	a,b
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
		return 10;
	}
}
