-- Vhdlbxx73.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBxx73 is
	generic (delay: integer);
	port (J: in integer; K: in integer; clock: in integer; notClr: in integer; Q: out integer; notQ: out integer);
end VHDLBxx73;

architecture VHDLBxx73_behavior of VHDLBxx73 is
begin
	main: process
		variable Qval, notQval, temp: integer;
	begin
		p1: while clock = 0 loop
			if notClr = 0 then
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

		if notClr = 1 and J = 1 and K = 0 then
			Qval := 1;
		elsif notClr = 1 and J = 0 and K = 1 then
			Qval := 0;
		elsif notClr = 1 and J = 1 and K = 1 then
			temp := Qval;
			Qval := notQval;
			notQval := temp;
		end if;

		p2: while clock = 1 loop
			if notClr = 0 then
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
end VHDLBxx73_behavior;

configuration VHDLBxx73_parts of VHDLBxx73 is
for VHDLBxx73_behavior
end for;
end VHDLBxx73_parts;
