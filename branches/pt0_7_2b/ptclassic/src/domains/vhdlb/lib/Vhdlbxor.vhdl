-- Vhdlbxor.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBXor is
	generic (delay: integer);
	port (input1,input2: in integer; output: out integer);
end VHDLBXor;

architecture VHDLBXor_behavior of VHDLBXor is
begin
	process
		variable temp: bit;
	begin
		temp := (bit'val(integer'pos(input1))) xor (bit'val(integer'pos(input2)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION;
	end process;
end VHDLBXor_behavior;

configuration VHDLBXor_parts of VHDLBXor is
for VHDLBXor_behavior
end for;
end VHDLBXor_parts;
