-- Vhdlbxor3.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBXor3 is
	generic (delay: integer);
	port (input1,input2,input3: in integer; output: out integer);
end VHDLBXor3;

architecture VHDLBXor3_behavior of VHDLBXor3 is
begin
	process
		variable temp: bit;
	begin
		temp := (bit'val(integer'pos(input1))) xor (bit'val(integer'pos(input2))) xor (bit'val(integer'pos(input3)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION, input3'TRANSACTION;
	end process;
end VHDLBXor3_behavior;

configuration VHDLBXor3_parts of VHDLBXor3 is
for VHDLBXor3_behavior
end for;
end VHDLBXor3_parts;
