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

  codeblock (V2Cinteger) {
--V2Cinteger.vhdl

library SYNOPSYS,IEEE;
use SYNOPSYS.ATTRIBUTES.all;
use IEEE.STD_LOGIC_1164.all;

entity V2Cinteger is
	generic ( pairid	: INTEGER	;
		  numxfer	: INTEGER	);
	port	( go		: in STD_LOGIC	;
		  data		: in INTEGER	;
		  done		: out STD_LOGIC	);
end V2Cinteger;

architecture CLI of V2Cinteger is

	attribute FOREIGN of CLI : architecture is "Synopsys:CLI";

	attribute CLI_ELABORATE	of CLI	: architecture is "v2cinteger_open";
	attribute CLI_EVALUATE	of CLI	: architecture is "v2cinteger_eval";
	attribute CLI_ERROR	of CLI	: architecture is "v2cinteger_error";
	attribute CLI_CLOSE	of CLI	: architecture is "v2cinteger_close";

	attribute CLI_PIN	of go	: signal is CLI_ACTIVE;
	attribute CLI_PIN	of data	: signal is CLI_PASSIVE;

begin
end;
  }

  codeblock (V2Creal) {
--V2Creal.vhdl

library SYNOPSYS,IEEE;
use SYNOPSYS.ATTRIBUTES.all;
use IEEE.STD_LOGIC_1164.all;

entity V2Creal is
	generic ( pairid	: INTEGER	;
		  numxfer	: INTEGER	);
	port	( go		: in STD_LOGIC	;
		  data		: in REAL	;
		  done		: out STD_LOGIC	);
end V2Creal;

architecture CLI of V2Creal is

	attribute FOREIGN of CLI : architecture is "Synopsys:CLI";

	attribute CLI_ELABORATE	of CLI	: architecture is "v2creal_open";
	attribute CLI_EVALUATE	of CLI	: architecture is "v2creal_eval";
	attribute CLI_ERROR	of CLI	: architecture is "v2creal_error";
	attribute CLI_CLOSE	of CLI	: architecture is "v2creal_close";

	attribute CLI_PIN	of go	: signal is CLI_ACTIVE;
	attribute CLI_PIN	of data	: signal is CLI_PASSIVE;

begin
end;
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
for all:V2Cinteger use entity work.V2Cinteger(CLI); end for;
  }

  codeblock (V2CrealConfig) {
for all:V2Creal use entity work.V2Creal(CLI); end for;
  }

// Called only once, after the scheduler is done
  begin {
//    printf("VHDLCSend.pl begin method called!!\n");
    // Call method to wire up a V2C VHDL entity
    targ()->registerV2C(int(pairNumber), numXfer, input.resolvedType());

    if (strcmp(input.resolvedType(), "INT") == 0) {
      addCode(V2Cinteger, "cli_models", "v2cint");
      addCode(V2CintegerComp, "cli_comps", "v2cintcomp");
      addCode(V2CintegerConfig, "cli_configs", "v2cintconfig");
    }
    else if (strcmp(input.resolvedType(), "FLOAT") == 0) {
      addCode(V2Creal, "cli_models", "v2creal");
      addCode(V2CrealComp, "cli_comps", "v2crealcomp");
      addCode(V2CrealConfig, "cli_configs", "v2crealconfig");
    }
    else
      Error::abortRun(*this, input.resolvedType(), ": type not supported");
  }

  go {
    // Add code to synch at end of main.
    StringList postSynch;
    for (int i = 0 ; i < numXfer ; i++) {
      postSynch << "V2C" << rtype << int(pairNumber) << "_data" << " <= " << "$ref(input,";
      postSynch << -i;
      postSynch << ")" << ";\n";
      postSynch << "V2C" << rtype << int(pairNumber) << "_go" << " <= " << "'0';\n";
      postSynch << "wait on " << "V2C" << rtype << int(pairNumber) << "_done" << "'transaction;\n";
    }
    
    addCode(postSynch);
  }

}
