defstar {
	name { UnixSend }
	domain { CGC }
	desc {
Send star between UNIX processors.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC target library }
	explanation {
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

