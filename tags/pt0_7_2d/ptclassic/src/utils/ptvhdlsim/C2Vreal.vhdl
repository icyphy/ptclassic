--C2Vreal.vhdl
--Real communication from C to VHDL
--Version:  @(#)C2Vreal.vhdl	1.2 6/27/96

library SYNOPSYS,IEEE;
use SYNOPSYS.ATTRIBUTES.all;
use IEEE.STD_LOGIC_1164.all;

entity C2VREAL is
	generic ( pairid	: INTEGER	;
		  numxfer	: INTEGER	);
	port	( go		: in STD_LOGIC	;
		  data		: out REAL	;
		  done		: out STD_LOGIC	);
end C2VREAL;

architecture CLI of C2VREAL is

	attribute FOREIGN of CLI : architecture is "Synopsys:CLI";

	attribute CLI_ELABORATE	of CLI	: architecture is "c2vreal_open";
	attribute CLI_EVALUATE	of CLI	: architecture is "c2vreal_eval";
	attribute CLI_ERROR	of CLI	: architecture is "c2vreal_error";
	attribute CLI_CLOSE	of CLI	: architecture is "c2vreal_close";

	attribute CLI_PIN	of go	: signal is CLI_ACTIVE;

begin
end;
