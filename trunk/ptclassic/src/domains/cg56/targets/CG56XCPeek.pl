defstar {
	name { XCAReceive }
	domain { CG56 }
	desc { CGC to S56X Asynchronous Receive star }
	version { $Id$ }
	author { Jose Luis Pino }
 	derivedFrom {XCABase}
	copyright {
Copyright (c) 1994,1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 Target Directory }
	explanation {
	}
protected {
	friend class S56XTargetWH;
	}

	output {
		name {output}
		type {ANYTYPE}
	}

codeblock(receiveData) {
	move	$ref(buffer),x0
	move	x0,$ref(output)
}


go {
	addCode(receiveData);
}

execTime {
	return 2;
}

}
