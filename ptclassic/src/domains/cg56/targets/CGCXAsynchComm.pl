defstar {
name { XAsynchComm }
domain { CGC }
desc { Base star for Asynchronous CGC <-> S56X IPC }
derivedFrom { XBase }
version { @(#)CGCXAsynchComm.pl	1.12 6/7/96 }
author { Jose Luis Pino }
copyright { 
Copyright (c) 1994-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}

location { CG56 Target Directory }

state {
    name { updateRate }
    type { int }
    default { 1 }
    desc {Update the link at most every 'updateRate' schedule iterations.}
}

state {
    name { VariableName }
    type { string }
    default { aioVariable }
    desc {Name of buffer.}
    attributes { A_NONSETTABLE} 
}

state {
    name { blockSize }
    type { int }
    default { 1 }
    desc { number of tokens to be transfered per firing. }
}

protected {
    DataType txType;
}

codeblock(declarations,"const char* cgcType") {
QckItem $starSymbol(s56xBuffer);
int $starSymbol(bufferAddr);
@cgcType $starSymbol(buffer)[@blockSize];
int $starSymbol(bufStart) = 0;
}

codeblock(loadDSPSymbols) {
    if (($starSymbol(s56xBuffer) = qckItem($val(S56XFilePrefix)_dsp,"$val(VariableName)")) == 0)
	EXIT_CGC("CGCXAsychComm address resolution failed");
    $starSymbol(bufferAddr) = QckAddr($starSymbol(s56xBuffer));
}

initCode {
    StringList cgcType;
    if (txType == INT) 
	cgcType = "int";
    else if (txType == FIX)
	cgcType = "fix";
    else if (txType == FLOAT)
	cgcType = "double";
    if (blockSize > 1) addDeclaration(declarations(cgcType));
    CGCXBase::initCode();
}


wrapup {
    CGCXBase::wrapup();
    if (blockSize > 1) addMainInit(loadDSPSymbols);
}

}

