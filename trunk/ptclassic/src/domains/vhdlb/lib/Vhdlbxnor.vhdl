-- Vhdlbxnor.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBXnor is
	generic (delay: integer);
	port (input1,input2: in integer; output: out integer);
end VHDLBXnor;

architecture VHDLBXnor_behavior of VHDLBXnor is
begin
	process
		variable temp: bit;
	begin
		temp := not (bit'val(integer'pos(input1))) xor (bit'val(integer'pos(input2)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION;
	end process;
end VHDLBXnor_behavior;

configuration VHDLBXnor_parts of VHDLBXnor is
for VHDLBXnor_behavior
end for;
end VHDLBXnor_parts;
