-- Vhdlbramp.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBRamp is
	generic (init,step,interval: integer);
	port (output: out integer);
end VHDLBRamp;

architecture VHDLBRamp_behavior of VHDLBRamp is
begin
	main: process
		variable level : integer := init;
	begin
		output <= level;
		level := level + step;
		wait for (1E6*time'val(integer'pos(interval)));
	end process;
end VHDLBRamp_behavior;

configuration VHDLBRamp_parts of VHDLBRamp is
for VHDLBRamp_behavior
end for;
end VHDLBRamp_parts;
