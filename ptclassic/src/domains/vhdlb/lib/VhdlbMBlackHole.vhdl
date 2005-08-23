-------------------------------------------------------------------------------
--  Discards multiple inputs
-------------------------------------------------------------------------------

entity VHDLBMBlackHole is
	port (input: in INTEGER_ARRAY(0 to (I_WIDTH-1)) );
end VHDLBMBlackHole;

architecture VHDLBMBlackHole_behavior of VHDLBMBlackHole is
begin
end VHDLBMBlackHole_behavior;

configuration VHDLBMBlackHole_parts of VHDLBMBlackHole is
for VHDLBMBlackHole_behavior
end for;
end VHDLBMBlackHole_parts;
