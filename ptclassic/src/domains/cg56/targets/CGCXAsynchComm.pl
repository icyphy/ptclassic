defstar {
    name { XABase }
    domain { CGC }
    desc { Base star for Asynchronous CGC <-> S56X IPC }
    ccinclude { "CGCS56XTarget.h" }
    derivedFrom { XBase }
    version { $Id$ }
    author { Jose Luis Pino }
    copyright { 
Copyright (c) 1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    location { CG56 Target Directory }

    state {
	    name { VariableName }
	    type { string }
	    default { "aioVariable" }
	    desc {The name used to identify the asynchronous connection.}
    }

    state {
	    name { updateRate }
	    type { int }
	    default { 1 }
	    desc {Update the link at most every 'updateRate' schedule iterations.}
    }

    state {
	    name { blockSize }
	    type { int }
	    default { 1 }
	    desc { number of tokens to be transfered per firing. }
    }

protected {
	StringList txType;
}

codeblock(updateLink,"const char* code") {
	if ($starSymbol(iterationCount)++ == $val(updateRate)) {
		$starSymbol(iterationCount) = 1;
	{
		@code;
	}
	}
}

codeblock(declarations,"") {
QckItem $starSymbol(semaphore);
QckItem $starSymbol(s56xBuffer);
int $starSymbol(bufferAddr);
@txType $starSymbol(buffer)[@blockSize];
int $starSymbol(bufStart) = 0;
}

codeblock(loadDSPSymbols) {
    if (($starSymbol(semaphore) = qckItem(dsp,"$val(VariableName)_sem")) == 0) {
	    fprintf(stderr,"$val(VariableName) semaphore address resolution failed\n");
	    exit(1);
    }
    if (($starSymbol(s56xBuffer) = qckItem(dsp,"$val(VariableName)")) == 0) {
	    fprintf(stderr,"$val(VariableName) semaphore address resolution failed\n");
	    exit(1);
    }
    $starSymbol(bufferAddr) = QckAddr($starSymbol(s56xBuffer));
}
	    
initCode {
	addDeclaration("int $starSymbol(iterationCount) = 0;");
	if (blockSize > 1) addDeclaration(declarations());
	CGCXBase::initCode();
}


setup {
	CGCS56XTarget* parent = (CGCS56XTarget*)(myTarget()->parent());
	if(parent) S56XFilePrefix.setCurrentValue(parent->s56xFilePrefix());
	CGCXBase::setup();
}

wrapup {
	CGCXBase::wrapup();
	if (blockSize > 1) addMainInit(loadDSPSymbols);
}

}
