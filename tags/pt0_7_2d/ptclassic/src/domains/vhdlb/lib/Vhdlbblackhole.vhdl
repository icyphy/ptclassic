-- Vhdlbblackhole.vhdl
-- @(#)Vhdlbblackhole.vhdl	1.1 6/23/94
-- Michael C. Williamson

entity VHDLBBlackHole is
	port (input: in integer);
end VHDLBBlackHole;

architecture VHDLBBlackHole_behavior of VHDLBBlackHole is
begin
end VHDLBBlackHole_behavior;

configuration VHDLBBlackHole_parts of VHDLBBlackHole is
for VHDLBBlackHole_behavior
end for;
end VHDLBBlackHole_parts;
