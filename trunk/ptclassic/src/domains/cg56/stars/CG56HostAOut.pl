defstar {
    name { HostAOut }
    domain { CG56 }
    desc { Asyncronous Host Output. }
    version { $Id$ }
    author { Kennard White }
    copyright { 1992 The Regents of the University of California }
    location { CG56 library }
    explanation {
blah
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
    start {
        input.setSDFParams(int(blockSize),int(blockSize)-1);
    }
    codeblock(cbAio) {
aio_graph $ref(input,0) $fullname() {$val(label)} $val(blockSize) $val(yMin) $val(yMax) $val(xMin) $val(xMax)
    }
    initCode {
	addCode(cbAio,"aioCmds");
    }
    go {
	; // nothing to do
    }
    execTime { 
        return 1;
    }
}
