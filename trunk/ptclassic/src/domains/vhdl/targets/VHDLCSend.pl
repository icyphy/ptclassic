defstar	{
  name { CSend }
  domain { VHDL }
  desc { VHDL to CGC synchronous send star }
  version { $Id$ }
  author { Michael C. Williamson, Jose Luis Pino }
  derivedFrom { CSynchComm }
  copyright {
Copyright (c) 1993-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright	for copyright notice,
limitation of liability, and disclaimer	of warranty provisions.
}
  location { VHDL Target Directory }
  explanation { }
  input {
    name {input}
    type {ANYTYPE}
  }
  setup {
    if (strcmp(input.resolvedType(), "INT") == 0) 
      rtype = "integer";
    else if (strcmp(input.resolvedType(), "FLOAT") == 0) 
      rtype = "real";
    else
      Error::abortRun(*this, input.resolvedType(), ": type not supported");

    numXfer = input.numXfer();
    VHDLCSynchComm::setup();
  }

  codeblock (V2CintegerComp) {
component V2Cinteger
  generic ( pairid  : INTEGER ;
	    numxfer : INTEGER );
  port ( go   : in  STD_LOGIC ;
	 data : in  INTEGER   ;
	 done : out STD_LOGIC );
end component;
  }

  codeblock (V2CrealComp) {
component V2Creal
  generic ( pairid  : INTEGER ;
	    numxfer : INTEGER );
  port ( go   : in  STD_LOGIC ;
	 data : in  REAL      ;
	 done : out STD_LOGIC );
end component;
  }

  codeblock (V2CintegerConfig) {
for all:V2Cinteger use entity PTVHDLSIM.V2Cinteger(CLI); end for;
  }

  codeblock (V2CrealConfig) {
for all:V2Creal use entity PTVHDLSIM.V2Creal(CLI); end for;
  }

// Called only once, after the scheduler is done
  begin {
    // FIXME: This is a hack to get around unsynthesizable code.
    if(!(targ()->isA("SynthTarget"))) {

    if (strcmp(input.resolvedType(), "INT") == 0) {
      addCode(V2CintegerComp, "cli_comps", "v2cintcomp");
      addCode(V2CintegerConfig, "cli_configs", "v2cintconfig");
    }
    else if (strcmp(input.resolvedType(), "FLOAT") == 0) {
      addCode(V2CrealComp, "cli_comps", "v2crealcomp");
      addCode(V2CrealConfig, "cli_configs", "v2crealconfig");
    }
    else
      Error::abortRun(*this, input.resolvedType(), ": type not supported");

    }
  }

  go {
    // FIXME: This is a hack to get around unsynthesizable code.
    if(!(targ()->isA("SynthTarget"))) {

    // Added this in here instead of in begin().
    int direction = 1;
    targ()->registerComm(direction, int(pairNumber), numXfer,
			 input.resolvedType());

    // Add code to synch at end of main.
    StringList dataSynch;
    for (int i = 0 ; i < numXfer ; i++) {
      dataSynch << "V2C" << rtype << int(pairNumber) << "_data" << " <= "
		<< "$ref(input,";
      dataSynch << -i;
      dataSynch << ")" << ";\n";
      dataSynch << "V2C" << rtype << int(pairNumber) << "_go" << " <= "
		<< "'0';\n";
      dataSynch << "wait on " << "V2C" << rtype << int(pairNumber) << "_done"
		<< "'transaction;\n";
    }
    
    addCode(dataSynch);

    }
  }

}
