-- Vhdlbnand.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBNand is
	generic (delay: integer);
	port (input1,input2: in integer; output: out integer);
end VHDLBNand;

architecture VHDLBNand_behavior of VHDLBNand is
begin
	process
		variable temp: bit;
	begin
		temp := (bit'val(integer'pos(input1))) nand (bit'val(integer'pos(input2)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION;
	end process;
end VHDLBNand_behavior;

configuration VHDLBNand_parts of VHDLBNand is
for VHDLBNand_behavior
end for;
end VHDLBNand_parts;
