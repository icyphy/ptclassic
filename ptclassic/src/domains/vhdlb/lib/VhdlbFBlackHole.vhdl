-------------------------------------------------------------------------------
--  Discards multiple inputs
-------------------------------------------------------------------------------

entity VHDLBFBlackHole is
	port (input: in REAL_ARRAY(0 to (I_WIDTH-1)) );
end VHDLBFBlackHole;

architecture VHDLBFBlackHole_behavior of VHDLBFBlackHole is
begin
end VHDLBFBlackHole_behavior;

configuration VHDLBFBlackHole_parts of VHDLBFBlackHole is
for VHDLBFBlackHole_behavior
end for;
end VHDLBFBlackHole_parts;
