-- Vhdlbser2par.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBSer2Par is
	generic (delay: integer);
	port (serIn: in integer; clock: in integer;
	bus1: out integer; bus2: out integer; bus3: out integer;
	bus4: out integer; bus5: out integer; bus6: out integer;
	bus7: out integer; bus8: out integer);
end VHDLBSer2Par;

architecture VHDLBSer2Par_behavior of VHDLBSer2Par is
	type byte is array (1 to 8) of integer;
begin
	process
		variable value: byte;
		variable count: integer := 0;
	begin
		wait until clock = 0;
		wait until clock = 1;
		value(count) := serIn;
		count := count + 1;
		if count = 8 then
			bus1 <= value(1) after (1E6*time'val(integer'pos(delay)));
			bus2 <= value(2) after (1E6*time'val(integer'pos(delay)));
			bus3 <= value(3) after (1E6*time'val(integer'pos(delay)));
			bus4 <= value(4) after (1E6*time'val(integer'pos(delay)));
			bus5 <= value(5) after (1E6*time'val(integer'pos(delay)));
			bus6 <= value(6) after (1E6*time'val(integer'pos(delay)));
			bus7 <= value(7) after (1E6*time'val(integer'pos(delay)));
			bus8 <= value(8) after (1E6*time'val(integer'pos(delay)));
			count := 0;
		end if;
	end process;
end VHDLBSer2Par_behavior;

configuration VHDLBSer2Par_parts of VHDLBSer2Par is
for VHDLBSer2Par_behavior
end for;
end VHDLBSer2Par_parts;
