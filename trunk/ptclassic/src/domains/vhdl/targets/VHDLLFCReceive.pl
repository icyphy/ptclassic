defstar {
  name { LFCReceive }
  domain { VHDL }
  desc { CGC to VHDL Receive star }
  version { $Id$ }
  author { Michael C. Williamson, Jose L. Pino }
  derivedFrom {CSynchComm}
  copyright {
Copyright (c) 1993-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
  location { VHDL Target Directory }
  explanation { }
  output {
    name {output}
    type {ANYTYPE}
  }
  setup {
    if (strcmp(output.resolvedType(), "INT") == 0) 
      rtype = "integer";
    else if (strcmp(output.resolvedType(), "FLOAT") == 0) 
      rtype = "real";
    else
      Error::abortRun(*this, output.resolvedType(), ": type not supported");

    numXfer = output.numXfer();     
    VHDLCSynchComm::setup();      
  }

  codeblock (C2VintegerConfig) {
for all:C2Vinteger use entity PTVHDLSIM.C2Vinteger(CLI); end for;
  }
  
  codeblock (C2VrealConfig) {
for all:C2Vreal use entity PTVHDLSIM.C2Vreal(CLI); end for;
  }
  
// Called only once, after the scheduler is done
  begin {
    // FIXME: This is a hack to get around unsynthesizable code.
    if(!(targ()->isA("SynthTarget"))) {

    if (strcmp(output.resolvedType(), "INT") == 0) {
      addCode(C2VintegerConfig, "cli_configs", "c2vintconfig");
    }
    else if (strcmp(output.resolvedType(), "FLOAT") == 0) {
      addCode(C2VrealConfig, "cli_configs", "c2vrealconfig");
    }
    else
      Error::abortRun(*this, output.resolvedType(), ": type not supported");

    }
  }

  go {
    // FIXME: This is a hack to get around unsynthesizable code.
    if(!(targ()->isA("SynthTarget"))) {

    // Added this in here instead of in begin().
    int direction = 0;
    targ()->registerComm(direction, int(pairNumber), numXfer,
			 output.resolvedType());

    // Add code to synch at beginning of main.
    StringList dataSynch;
    for (int i = 0 ; i < numXfer ; i++) {
      dataSynch << "C2V" << rtype << int(pairNumber) << "_go" << " <= '0';\n";
      dataSynch << "wait on " << "C2V" << rtype << int(pairNumber) << "_done"
		<< "'transaction;\n";
      dataSynch << "$ref(output,";
      dataSynch << -i;
      dataSynch << ") $assign(output) " << "C2V" << rtype << int(pairNumber) << "_data;\n";
    }
    
    addCode(dataSynch);

    }
  }

}
