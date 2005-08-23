--V2Cinteger.vhdl
--Integer communication from VHDL to C
--Version:  @(#)V2Cinteger.vhdl	1.2 6/27/96

library SYNOPSYS,IEEE;
use SYNOPSYS.ATTRIBUTES.all;
use IEEE.STD_LOGIC_1164.all;

entity V2CINTEGER is
	generic ( pairid	: INTEGER	;
		  numxfer	: INTEGER	);
	port	( go		: in STD_LOGIC	;
		  data		: in INTEGER	;
		  done		: out STD_LOGIC	);
end V2CINTEGER;

architecture CLI of V2CINTEGER is

	attribute FOREIGN of CLI : architecture is "Synopsys:CLI";

	attribute CLI_ELABORATE	of CLI	: architecture is "v2cinteger_open";
	attribute CLI_EVALUATE	of CLI	: architecture is "v2cinteger_eval";
	attribute CLI_ERROR	of CLI	: architecture is "v2cinteger_error";
	attribute CLI_CLOSE	of CLI	: architecture is "v2cinteger_close";

	attribute CLI_PIN	of go	: signal is CLI_ACTIVE;
	attribute CLI_PIN	of data	: signal is CLI_PASSIVE;

begin
end;
