--V2Creal.vhdl
--Real communication from VHDL to C
--Version:  @(#)V2Creal.vhdl	1.1 1/30/96

library SYNOPSYS,IEEE;
use SYNOPSYS.ATTRIBUTES.all;
use IEEE.STD_LOGIC_1164.all;

entity V2CREAL is
	generic ( pairid	: INTEGER	;
		  numxfer	: INTEGER	);
	port	( go		: in STD_LOGIC	;
		  data		: in REAL	;
		  done		: out STD_LOGIC	);
end V2CREAL;

architecture CLI of V2CREAL is

	attribute FOREIGN of CLI : architecture is "Synopsys:CLI";

	attribute CLI_ELABORATE	of CLI	: architecture is "v2creal_open";
	attribute CLI_EVALUATE	of CLI	: architecture is "v2creal_eval";
	attribute CLI_ERROR	of CLI	: architecture is "v2creal_error";
	attribute CLI_CLOSE	of CLI	: architecture is "v2creal_close";

	attribute CLI_PIN	of go	: signal is CLI_ACTIVE;
	attribute CLI_PIN	of data	: signal is CLI_PASSIVE;

begin
end;
