-- Vhdlbor.vhdl
-- $Id$
-- Michael C. Williamson

entity VHDLBOr is
	generic (delay: integer);
	port (input1,input2: in integer; output: out integer);
end VHDLBOr;

architecture VHDLBOr_behavior of VHDLBOr is
begin
	process
		variable temp: bit;
	begin
		temp := (bit'val(integer'pos(input1))) or (bit'val(integer'pos(input2)));
		output <= (integer'val(bit'pos(temp))) after (1E6*time'val(integer'pos(delay)));
		wait on input1'TRANSACTION, input2'TRANSACTION;
	end process;
end VHDLBOr_behavior;

configuration VHDLBOr_parts of VHDLBOr is
for VHDLBOr_behavior
end for;
end VHDLBOr_parts;
