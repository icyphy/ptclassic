-- Vhdlbthrough.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBThrough is
	generic (delay: integer);
	port (input: in integer; output: out integer);
end VHDLBThrough;

architecture VHDLBThrough_behavior of VHDLBThrough is
begin
	main: process
	begin
		wait on input'TRANSACTION;
		output <= input after (1E6*time'val(integer'pos(delay)));
	end process;
end VHDLBThrough_behavior;

configuration VHDLBThrough_parts of VHDLBThrough is
for VHDLBThrough_behavior
end for;
end VHDLBThrough_parts;
