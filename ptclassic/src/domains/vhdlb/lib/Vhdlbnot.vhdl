-- Vhdlbnot.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBNot is
	generic (delay: integer);
	port (input: in integer; output: out integer);
end VHDLBNot;

architecture VHDLBNot_behavior of VHDLBNot is
begin
	process
	begin
		wait on input'TRANSACTION;
		if input /= 0 then
			output <= 0 after (1E6*time'val(integer'pos(delay)));
		else
			output <= 1 after (1E6*time'val(integer'pos(delay)));
		end if;
	end process;
end VHDLBNot_behavior;

configuration VHDLBNot_parts of VHDLBNot is
for VHDLBNot_behavior
end for;
end VHDLBNot_parts;
