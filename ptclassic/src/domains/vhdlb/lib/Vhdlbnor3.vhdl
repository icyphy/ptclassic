-- Vhdlbnor3.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBNor3 is
	generic (delay: integer);
	port (input1,input2,input3: in integer; output: out integer);
end VHDLBNor3;

architecture VHDLBNor3_behavior of VHDLBNor3 is
begin
	process
		variable temp: bit;
	begin
		temp := not (bit'val(integer'pos(input1))) or (bit'val(integer'pos(input2))) or (bit'val(integer'pos(input3)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION, input3'TRANSACTION;
	end process;
end VHDLBNor3_behavior;

configuration VHDLBNor3_parts of VHDLBNor3 is
for VHDLBNor3_behavior
end for;
end VHDLBNor3_parts;
