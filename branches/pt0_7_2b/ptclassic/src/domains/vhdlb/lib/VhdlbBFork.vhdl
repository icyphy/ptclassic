-------------------------------------------------------------------------------
--  Makes two identical copies of the input integer-valued bus.
-------------------------------------------------------------------------------

USE work.datatypes.all;

entity VhdlbBFork is
      Generic ( I_WIDTH : integer;
		A_WIDTH : integer;
		B_WIDTH : integer );
      Port (   input : In    INTEGER_VECTOR(0 to (I_WIDTH-1) );
	     outputa : Out   INTEGER_VECTOR(0 to (A_WIDTH-1) );
	     outputb : Out   INTEGER_VECTOR(0 to (B_WIDTH-1) ) );
end VhdlbBFork;

architecture VHDLBBFork_behavior of VhdlbBFork is
   begin

   Split : PROCESS (input)

   BEGIN

     -- Makes two identical copies of the input bus.
     FOR i IN 0 to (I_WIDTH-1) LOOP
       outputa(i) <= input(i);
       outputb(i) <= input(i);
     END LOOP;

   END PROCESS Split;

end VHDLBBFork_behavior;

configuration CFG_VhdlbBFork_BEHAVIORAL of VhdlbBFork is
   for VHDLBBFork_behavior
   end for;
end CFG_VhdlbBFork_BEHAVIORAL;
