-- Vhdlbpar2ser.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBPar2Ser is
	generic (delay: integer);
	port (bus1: in integer; bus2: in integer; bus3: in integer;
	bus4: in integer; bus5: in integer; bus6: in integer;
	bus7: in integer; bus8: in integer;
	clock: in integer; serOut: out integer);
end VHDLBPar2Ser;

architecture VHDLBPar2Ser_behavior of VHDLBPar2Ser is
	type byte is array (1 to 8) of integer;
begin
	process
		variable value: byte;
		variable count: integer := 0;
	begin
		wait until clock = 0;
		wait until clock = 1;
		if count = 8 then
			value(1) := bus1;
			value(2) := bus2;
			value(3) := bus3; 
			value(4) := bus4;
			value(5) := bus5;
			value(6) := bus6;
			value(7) := bus7;
			value(8) := bus8;
			count := 0;
		end if;
		count := count + 1;
		serOut <= value(count) after (1E6*time'val(integer'pos(delay)));
	end process;
end VHDLBPar2Ser_behavior;

configuration VHDLBPar2Ser_parts of VHDLBPar2Ser is
for VHDLBPar2Ser_behavior
end for;
end VHDLBPar2Ser_parts;
