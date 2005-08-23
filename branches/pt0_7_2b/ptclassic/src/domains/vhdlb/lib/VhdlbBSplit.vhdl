-------------------------------------------------------------------------------
--  Splits a wider bus into two smaller-width buses.
--   The bottom output bus has bits 0 to B-1  (B is the width of bottom bus)
--   The top output bus has bits B to N-1 (N is the width of input bus)
-------------------------------------------------------------------------------

USE work.datatypes.all;

entity VhdlbBSplit is
      Generic ( I_WIDTH : integer;
		A_WIDTH : integer;
		B_WIDTH : integer );
      Port (   input : In    INTEGER_VECTOR(0 to (I_WIDTH-1) );
	         top : Out   INTEGER_VECTOR(0 to (A_WIDTH-1) );
	      bottom : Out   INTEGER_VECTOR(0 to (B_WIDTH-1) ) );
end VhdlbBSplit;

architecture VHDLBBSplit_behavior of VhdlbBSplit is
   begin

   Split : PROCESS (input)

   BEGIN

     -- Drive the bottom bus.
     FOR i IN 0 to (B_WIDTH-1) LOOP
       bottom(i) <= input(i);
     END LOOP;

     -- Drive the top bus.
     FOR i IN 0 to (A_WIDTH-1) LOOP
       top(i) <= input( i + B_WIDTH );
     END LOOP;

   END PROCESS Split;

end VHDLBBSplit_behavior;

configuration CFG_VhdlbBSplit_BEHAVIORAL of VhdlbBSplit is
   for VHDLBBSplit_behavior
   end for;
end CFG_VhdlbBSplit_BEHAVIORAL;
