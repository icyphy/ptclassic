defstar {
  name { CReceive }
  domain { VHDL }
  desc { CGC to VHDL Receive star }
  version { $Id$ }
  author { Michael C. Williamson, Jose L. Pino }
  derivedFrom {CSynchComm}
  copyright {
Copyright (c) 1994,1993 The Regents of the University of California.
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

  codeblock (C2Vinteger) {
--C2Vinteger.vhdl

library SYNOPSYS,IEEE;
use SYNOPSYS.ATTRIBUTES.all;
use IEEE.STD_LOGIC_1164.all;

entity C2Vinteger is
	generic ( pairid	: INTEGER	;
		  numxfer	: INTEGER	);
	port	( go		: in STD_LOGIC	;
		  data		: out INTEGER	;
		  done		: out STD_LOGIC	);
end C2Vinteger;

architecture CLI of C2Vinteger is

	attribute FOREIGN of CLI : architecture is "Synopsys:CLI";

	attribute CLI_ELABORATE	of CLI	: architecture is "c2vinteger_open";
	attribute CLI_EVALUATE	of CLI	: architecture is "c2vinteger_eval";
	attribute CLI_ERROR	of CLI	: architecture is "c2vinteger_error";
	attribute CLI_CLOSE	of CLI	: architecture is "c2vinteger_close";

	attribute CLI_PIN	of go	: signal is CLI_ACTIVE;

begin
end;
  }

  codeblock (C2Vreal) {
--C2Vreal.vhdl

library SYNOPSYS,IEEE;
use SYNOPSYS.ATTRIBUTES.all;
use IEEE.STD_LOGIC_1164.all;

entity C2Vreal is
	generic ( pairid	: INTEGER	;
		  numxfer	: INTEGER	);
	port	( go		: in STD_LOGIC	;
		  data		: out REAL	;
		  done		: out STD_LOGIC	);
end C2Vreal;

architecture CLI of C2Vreal is

	attribute FOREIGN of CLI : architecture is "Synopsys:CLI";

	attribute CLI_ELABORATE	of CLI	: architecture is "c2vreal_open";
	attribute CLI_EVALUATE	of CLI	: architecture is "c2vreal_eval";
	attribute CLI_ERROR	of CLI	: architecture is "c2vreal_error";
	attribute CLI_CLOSE	of CLI	: architecture is "c2vreal_close";

	attribute CLI_PIN	of go	: signal is CLI_ACTIVE;

begin
end;
  }

  codeblock (C2VintegerComp) {
component C2Vinteger
  generic ( pairid  : INTEGER ;
	    numxfer : INTEGER );
  port ( go   : in  STD_LOGIC ;
	 data : out INTEGER   ;
	 done : out STD_LOGIC );
end component;
  }

  codeblock (C2VrealComp) {
component C2Vreal
  generic ( pairid  : INTEGER ;
	    numxfer : INTEGER );
  port ( go   : in  STD_LOGIC ;
	 data : out REAL      ;
	 done : out STD_LOGIC );
end component;
  }

  codeblock (C2VintegerConfig) {
for all:C2Vinteger use entity work.C2Vinteger(CLI); end for;
  }
  
  codeblock (C2VrealConfig) {
for all:C2Vreal use entity work.C2Vreal(CLI); end for;
  }
  
// Called only once, after the scheduler is done
  begin {
//    printf("VHDLCReceive.pl begin method called!!\n");
    // Call method to wire up a C2V VHDL entity
    targ()->registerC2V(int(pairNumber), numXfer, output.resolvedType());

    if (strcmp(output.resolvedType(), "INT") == 0) {
      addCode(C2Vinteger, "cli_models", "c2vint");
      addCode(C2VintegerComp, "cli_comps", "c2vintcomp");
      addCode(C2VintegerConfig, "cli_configs", "c2vintconfig");
    }
    else if (strcmp(output.resolvedType(), "FLOAT") == 0) {
      addCode(C2Vreal, "cli_models", "c2vreal");
      addCode(C2VrealComp, "cli_comps", "c2vrealcomp");
      addCode(C2VrealConfig, "cli_configs", "c2vrealconfig");
    }
    else
      Error::abortRun(*this, output.resolvedType(), ": type not supported");
  }

  go {
    // Add code to synch at beginning of main.
    StringList preSynch;
    for (int i = 0 ; i < numXfer ; i++) {
      preSynch << "C2V" << rtype << int(pairNumber) << "_go" << " <= '0';\n";
      preSynch << "wait on " << "C2V" << rtype << int(pairNumber) << "_done" << "'transaction;\n";
      preSynch << "$ref(output,";
      preSynch << -i;
      preSynch << ")" << " := " << "C2V" << rtype << int(pairNumber) << "_data;\n";
    }
    
    addCode(preSynch);
  }

}
