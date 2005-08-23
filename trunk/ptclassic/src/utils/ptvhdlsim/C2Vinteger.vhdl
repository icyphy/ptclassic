--C2Vinteger.vhdl
--Integer communication from C to VHDL
--Version:  @(#)C2Vinteger.vhdl	1.2 6/27/96

library SYNOPSYS,IEEE;
use SYNOPSYS.ATTRIBUTES.all;
use IEEE.STD_LOGIC_1164.all;

entity C2VINTEGER is
	generic ( pairid	: INTEGER	;
		  numxfer	: INTEGER	);
	port	( go		: in STD_LOGIC	;
		  data		: out INTEGER	;
		  done		: out STD_LOGIC	);
end C2VINTEGER;

architecture CLI of C2VINTEGER is

	attribute FOREIGN of CLI : architecture is "Synopsys:CLI";

	attribute CLI_ELABORATE	of CLI	: architecture is "c2vinteger_open";
	attribute CLI_EVALUATE	of CLI	: architecture is "c2vinteger_eval";
	attribute CLI_ERROR	of CLI	: architecture is "c2vinteger_error";
	attribute CLI_CLOSE	of CLI	: architecture is "c2vinteger_close";

	attribute CLI_PIN	of go	: signal is CLI_ACTIVE;

begin
end;
