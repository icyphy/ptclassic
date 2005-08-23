defstar {
	name { Rect }
	domain {VHDLB}
	desc {
Generate a rectangular pulse of height "height" (default 1.0).
and width "width" (default 8).  If "period" is greater than zero,
then the pulse is repeated with the given period.
	}
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
The real-valued output is sent to the output port at the rising
edges of the synchronization clock.
        }
	input {
		name { clock }
		type { int }
	}
	output {
		name { output }
		type { float }
	}
	state {
		name { height }
		type { float }
		desc { Height of the rectangular pulse }
		default { 1.0 }
	}
	state {
		name { width }
		type { int }
		desc { Width of the rectangular pulse }
		default { 8 }
	}
	state {
		name { period }
		type { int }
		default { 0 }
		desc { If greater than zero, the period of the pulse stream. }
	}
	go {
	}
}
