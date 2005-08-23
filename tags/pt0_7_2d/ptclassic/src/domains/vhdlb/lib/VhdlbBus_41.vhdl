----------------------------------------------------------------------------
--  Since ptolemy doesn't allow the user to explicitly merge several wires
--  into a bus, this star is written to allow the user to explicitly create a
--  4-bit-wide bus from 4 input wires.  If the input bus is not 4 bits wide,
--  an error message is printed.
----------------------------------------------------------------------------

USE work.datatypes.all;

entity VhdlbBus_41 is
      Generic ( O_WIDTH : integer );
      Port (    in_3 : In    integer;
                in_2 : In    integer;
                in_1 : In    integer;
                in_0 : In    integer;
              output : Out   INTEGER_VECTOR(0 to (O_WIDTH-1)) );
end VhdlbBus_41;

architecture VHDLBBus_41_behavior of VhdlbBus_41 is
   begin

   Merge : PROCESS

   BEGIN
     output(0) := in_0;		-- Create the bus from individual bits
     output(1) := in_1;
     output(2) := in_2;
     output(3) := in_3;
   END PROCESS Merge;

end VHDLBBus_41_behavior;

configuration CFG_VhdlbBus_41_BEHAVIORAL of VhdlbBus_41 is
   for VHDLBBus_41_behavior
   end for;
end CFG_VhdlbBus_41_BEHAVIORAL;
