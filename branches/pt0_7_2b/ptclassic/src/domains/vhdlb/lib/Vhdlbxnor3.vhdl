-- Vhdlbxnor3.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBXnor3 is
	generic (delay: integer);
	port (input1,input2,input3: in integer; output: out integer);
end VHDLBXnor3;

architecture VHDLBXnor3_behavior of VHDLBXnor3 is
begin
	process
		variable temp: bit;
	begin
		temp := not (bit'val(integer'pos(input1))) xor (bit'val(integer'pos(input2))) xor (bit'val(integer'pos(input3)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION, input3'TRANSACTION;
	end process;
end VHDLBXnor3_behavior;

configuration VHDLBXnor3_parts of VHDLBXnor3 is
for VHDLBXnor3_behavior
end for;
end VHDLBXnor3_parts;
