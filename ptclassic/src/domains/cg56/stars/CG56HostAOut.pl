defstar {
    name { HostAOut }
    domain { CG56 }
    desc { Asynchronous host output. }
    version { $Id$ }
    author { Kennard White }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { CG56 io library }
    explanation {
Output data from DSP to host via host port asynchronously.
    }
    input {
	name { input }
	type { FIX }
    }
    state {
    	name { label }
	type { STRING }
    	desc { Label for graph on host. }
	default { "Label" }
    }
    state {
	name { blockSize }
	type { INT }
	desc { Number of samples per block. }
	default { 1 }
    }
    state {
	name { mode }
	type { STRING }
	desc { Communication mode: one-of DIRECT or HOSTLOCK. }
	default { "DIRECT" }
    }
    state {
	name { bufLock }
	type { INT }
	desc { Semaphore for buffer locking. }
	default { 0 }
	attributes { A_NONCONSTANT|A_NONSETTABLE|A_XMEM }
    }
    state {
	name { buffer }
	type { FIXARRAY }
	desc { Buffer which host reads sample from. }
	attributes { A_NONCONSTANT|A_NONSETTABLE }
    }
    state {
	name { yMin }
	type { FLOAT }
	desc { "Host mapping of -1 for sample(s)." }
	default { "-1.0" }
    }
    state {
	name { yMax }
	type { FLOAT }
	desc { "Host mapping of +1 for sample(s)." }
	default { "1.0" }
    }
    state {
	name { xMin }
	type { FLOAT }
	desc { "Host x-axis mapping of first sample in block." }
	default { "-1.0" }
    }
    state {
	name { xMax }
	type { FLOAT }
	desc { "Host x-axis mapping of (last+1) sample in block." }
	default { "1.0" }
    }
    protected {
	int	doCopyB;
    }
    start {
        input.setSDFParams(int(blockSize),int(blockSize)-1);
	doCopyB = FALSE;
	const char *m = mode;
	/*IF*/ if ( strcmp(m,"DIRECT")==0 ) {
	    buffer.resize(0);
	    buffer.clearAttributes(A_YMEM);
	} else if ( strcmp(m,"HOSTLOCK")==0 ) {
	    buffer.resize(int(blockSize));
	    buffer.setAttributes(A_YMEM);
	    doCopyB = TRUE;
	} else {
	    Error::abortRun(*this,"Invalid mode");
	    return;
	}
    }
    codeblock(cbAioDirect) {
aio_graph $ref(input,0) $fullname() {$val(label)} $val(blockSize) $val(mode) - $val(yMin) $val(yMax) $val(xMin) $val(xMax)
    }
    codeblock(cbAioHostLock) {
aio_graph $ref(buffer,0) $fullname() {$val(label)} $val(blockSize) $val(mode) $ref(bufLock) $val(yMin) $val(yMax) $val(xMin) $val(xMax)
    }
    codeblock(cbCopy) {
	move	#$addr(bufLock),r2
	move	#$addr(input),r0	; copy entire {input} into {buffer}
	move	#$addr(buffer),r1
	jset	#0,x:(r2),$label(skip)
	.LOOP	#$val(blockSize)
	  move	x:(r0)+,x0
	  move	x0,y:(r1)+
	.ENDL
	bset	#0,x:(r2)
$label(skip)		
	nop				; jumps to end MUST have nop (loopsched)
    }
    initCode {
	addCode( doCopyB ? cbAioHostLock : cbAioDirect, "aioCmds");
    }
    go {
	if ( doCopyB ) {
	    addCode(cbCopy);
	}
    }
    execTime { 
        return 1;
    }
}
