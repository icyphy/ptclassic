defstar {
	name { UnixReceive }
	domain { CGC }
	desc {
Receive star between Unix processors.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC local library }
	explanation {
Produce code for inter-process communication (receive-side)
	}
	private {
		friend class CGCMultiTarget;
	}
	output {
		name {output}
		type {FLOAT}
	}
	ccinclude { "StringList.h" }

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
		default { "sid" }
		desc { socket id }
		attributes { A_NONSETTABLE }
	}
	setup {
		numData = output.numXfer();
	}
	codeblock (block) {
	int i, pos;
	union {
		unsigned long l;
		float f;
	} myData;

	for (i = 0; i < $val(numData); i++) {
		pos = $val(numData) - 1 + i;
		if (read($val(socketId), &myData.l, sizeof(myData)) != sizeof(myData)) {
			printf("write fails.\n");
			break;
		}
		myData.l = ntohl(myData.l);
		$ref(output,pos) = myData.f;
	}
	}
	go {
		addCode(block);
	}
}

