-- Vhdlbnand3.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBNand3 is
	generic (delay: integer);
	port (input1,input2,input3: in integer; output: out integer);
end VHDLBNand3;

architecture VHDLBNand3_behavior of VHDLBNand3 is
begin
	process
		variable temp: bit;
	begin
		temp := not (bit'val(integer'pos(input1))) and (bit'val(integer'pos(input2))) and (bit'val(integer'pos(input3)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION, input3'TRANSACTION;
	end process;
end VHDLBNand3_behavior;

configuration VHDLBNand3_parts of VHDLBNand3 is
for VHDLBNand3_behavior
end for;
end VHDLBNand3_parts;
