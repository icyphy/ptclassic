-- Vhdlbclock.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBClock is
	generic (period: integer);
	port (output: out integer);
end VHDLBClock;

architecture VHDLBClock_behavior of VHDLBClock is
begin
	main: process
	begin
		output <= 1;
		wait for (5E5*time'val(integer'pos(period)));
		output <= 0;
		wait for (5E5*time'val(integer'pos(period)));
	end process;
end VHDLBClock_behavior;

configuration VHDLBClock_parts of VHDLBClock is
for VHDLBClock_behavior
end for;
end VHDLBClock_parts;
