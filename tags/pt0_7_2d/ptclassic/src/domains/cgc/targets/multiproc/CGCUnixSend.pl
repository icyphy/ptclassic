defstar {
	name { UnixSend }
	domain { CGC }
	desc {
Send star between UNIX processors.
	}
	version { @(#)CGCUnixSend.pl	1.9	04/07/97 }
	author { Soonhoi Ha }
        copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { CGC target library }
	htmldoc {
Produce code for inter-process communication (send-side)
	}
	private {
		friend class CGCMultiTarget;
	}
	input {
		name {input}
		type {FLOAT}
	}
	state {
		name { numData }
		type { int }
		default { 1 }
		desc { number of tokens to be transferred }
		attributes { A_NONSETTABLE }
	}
	state {
		name { socketId }
		type { STRING }
		default { "sId" }
		desc { socket id }
		attributes { A_NONSETTABLE }
	}
	ccinclude { "StringList.h" }
	hinclude { "CGCMultiTarget.h" }

	setup {
		numData = input.numXfer();
	}
	codeblock (block) {
	int i, pos;
	union {
		unsigned long l;
		float f;
	} myData;
	
	for (i = 0; i < $val(numData); i++) {
		pos = $val(numData) - 1 + i;
		myData.f = (float) $ref(input,pos);
		myData.l = htonl(myData.l);
		if (write($val(socketId), &myData.l, sizeof(myData)) != sizeof(myData)) {
			printf("write fails.\n");
			exit(1);
		}
	}
	}
	go {
		addCode(block);
	}
}

