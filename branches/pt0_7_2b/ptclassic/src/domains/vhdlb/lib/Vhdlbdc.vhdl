-- Vhdlbdc.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBDC is
	generic (level,interval: integer);
	port (output: out integer);
end VHDLBDC;

architecture VHDLBDC_behavior of VHDLBDC is
begin
	main: process
	begin
		output <= level;
		wait for (1E6*time'val(integer'pos(interval)));
	end process;
end VHDLBDC_behavior;

configuration VHDLBDC_parts of VHDLBDC is
for VHDLBDC_behavior
end for;
end VHDLBDC_parts;
