-- Vhdlbnor.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBNor is
	generic (delay: integer);
	port (input1,input2: in integer; output: out integer);
end VHDLBNor;

architecture VHDLBNor_behavior of VHDLBNor is
begin
	process
		variable temp: bit;
	begin
		temp := (bit'val(integer'pos(input1))) nor (bit'val(integer'pos(input2)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION;
	end process;
end VHDLBNor_behavior;

configuration VHDLBNor_parts of VHDLBNor is
for VHDLBNor_behavior
end for;
end VHDLBNor_parts;
