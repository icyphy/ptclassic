-- Vhdlband.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBAnd is
	generic (delay: integer);
	port (input1,input2: in integer; output: out integer);
end VHDLBAnd;

architecture VHDLBAnd_behavior of VHDLBAnd is
begin
	process
		variable temp: bit;
	begin
		temp := (bit'val(integer'pos(input1))) and (bit'val(integer'pos(input2)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION;
	end process;
end VHDLBAnd_behavior;

configuration VHDLBAnd_parts of VHDLBAnd is
for VHDLBAnd_behavior
end for;
end VHDLBAnd_parts;
