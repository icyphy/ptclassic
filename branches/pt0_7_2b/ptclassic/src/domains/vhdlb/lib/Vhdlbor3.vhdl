-- Vhdlbor3.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBOr3 is
	generic (delay: integer);
	port (input1,input2,input3: in integer; output: out integer);
end VHDLBOr3;

architecture VHDLBOr3_behavior of VHDLBOr3 is
begin
	process
		variable temp: bit;
	begin
		temp := (bit'val(integer'pos(input1))) or (bit'val(integer'pos(input2))) or (bit'val(integer'pos(input3)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION, input3'TRANSACTION;
	end process;
end VHDLBOr3_behavior;

configuration VHDLBOr3_parts of VHDLBOr3 is
for VHDLBOr3_behavior
end for;
end VHDLBOr3_parts;
