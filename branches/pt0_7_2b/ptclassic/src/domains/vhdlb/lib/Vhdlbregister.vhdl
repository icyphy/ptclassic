-- Vhdlbregister.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBRegister is
	generic (delay: integer);
	port (input: in integer; clock: in integer; output: out integer);
end VHDLBRegister;

architecture VHDLBRegister_behavior of VHDLBRegister is
begin
	process
		variable value: integer;
	begin
		wait until clock = 0;
		wait until clock = 1;
		value := input;
		output <= value after (1E6*time'val(integer'pos(delay)));
	end process;
end VHDLBRegister_behavior;

configuration VHDLBRegister_parts of VHDLBRegister is
for VHDLBRegister_behavior
end for;
end VHDLBRegister_parts;
