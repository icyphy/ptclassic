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
    start {
        input.setSDFParams(int(blockSize),int(blockSize)-1);
    }
    codeblock(cbAio) {
graph $ref(input,0) $fullname() "$val(label)"
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
