-- Vhdlband3.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBAnd3 is
	generic (delay: integer);
	port (input1,input2,input3: in integer; output: out integer);
end VHDLBAnd3;

architecture VHDLBAnd3_behavior of VHDLBAnd3 is
begin
	process
		variable temp: bit;
	begin
		temp := (bit'val(integer'pos(input1))) and (bit'val(integer'pos(input2))) and (bit'val(integer'pos(input3)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION, input3'TRANSACTION;
	end process;
end VHDLBAnd3_behavior;

configuration VHDLBAnd3_parts of VHDLBAnd3 is
for VHDLBAnd3_behavior
end for;
end VHDLBAnd3_parts;
