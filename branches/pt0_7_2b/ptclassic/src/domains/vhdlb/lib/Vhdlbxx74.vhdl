-- Vhdlbxx74.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBxx74 is
	generic (delay: integer);
	port (D: in integer; clock: in integer; notPre: in integer; notClr: in integer; Q: out integer; notQ: out integer);
end VHDLBxx74;

architecture VHDLBxx74_behavior of VHDLBxx74 is
begin
	main: process
		variable Qval, notQval, temp: integer;
	begin
		p1: while clock = 0 loop
			if notPre = 0 then
				Qval := 1;
			elsif notPre = 1 and notClr = 0 then
				Qval := 0;
			end if;
			if Qval = 0 then
				notQval := 1;
			else
				notQval := 0;
			end if;
			Q <= Qval after (1E6*time'val(integer'pos(delay)));
			notQ <= notQval after (1E6*time'val(integer'pos(delay)));
			exit p1 when clock = 1;
		end loop p1;

		if notPre = 1 and notClr = 1 and D = 1 then
			Qval := 1;
		elsif notPre = 1 and notClr = 1 and D = 0 then
			Qval := 0;
		end if;

		p2: while clock = 0 loop
			if notPre = 0 then
				Qval := 1;
			elsif notPre = 1 and notClr = 0 then
				Qval := 0;
			end if;
			if Qval = 0 then
				notQval := 1;
			else
				notQval := 0;
			end if;
			Q <= Qval after (1E6*time'val(integer'pos(delay)));
			notQ <= notQval after (1E6*time'val(integer'pos(delay)));
			exit p2 when clock = 0;
		end loop p2;
	end process main;
end VHDLBxx74_behavior;

configuration VHDLBxx74_parts of VHDLBxx74 is
for VHDLBxx74_behavior
end for;
end VHDLBxx74_parts;
