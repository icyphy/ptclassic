----------------------------------------------------------------------------
--  Since ptolemy doesn't allow the user to explicitly merge several wires
--  into a bus, this star is written to allow the user to explicitly create an
--  8-bit-wide bus from 8 input wires.  If the input bus is not 8 bits wide, an
--  error message will be printed.
----------------------------------------------------------------------------

USE work.datatypes.all;

entity VhdlbBus_81 is
      Generic ( O_WIDTH : integer );
      Port (    in_7 : In    integer;
                in_6 : In    integer;
                in_5 : In    integer;
                in_4 : In    integer;
                in_3 : In    integer;
                in_2 : In    integer;
                in_1 : In    integer;
                in_0 : In    integer;
              output : Out   INTEGER_VECTOR(0 to (O_WIDTH-1) ) );
end VhdlbBus_81;

architecture VHDLBBus_81_behavior of VhdlbBus_81 is
   begin

   Merge : PROCESS

   BEGIN
     output(0) := in_0;		-- Create the bus from individual bits
     output(1) := in_1;
     output(2) := in_2;
     output(3) := in_3;
     output(4) := in_4;
     output(5) := in_5;
     output(6) := in_6;
     output(7) := in_7;
   END PROCESS Merge;

end VHDLBBus_81_behavior;

configuration CFG_VhdlbBus_81_BEHAVIORAL of VhdlbBus_81 is
   for VHDLBBus_81_behavior
   end for;
end CFG_VhdlbBus_81_BEHAVIORAL;
