-- Vhdlbblackhole.vhdl
-- $Id$
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
