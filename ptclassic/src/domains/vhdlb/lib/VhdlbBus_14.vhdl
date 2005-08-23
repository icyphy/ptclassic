----------------------------------------------------------------------------
--  Since ptolemy doesn't allow the user to explicitly extract individual data
--  out of a bus (a multi-port), this star is written to allow the user
--  to convert a 4-bit-wide bus into 4 explicit wires.  If the output bus is
--  not 4 bits wide, an error message is printed.
----------------------------------------------------------------------------

USE work.datatypes.all;

entity VhdlbBus_14 is
      Generic ( I_WIDTH : integer );
      Port (   input : In    INTEGER_VECTOR(0 to (I_WIDTH-1) );
		out_3 : Out   integer;
                out_2 : Out   integer;
                out_1 : Out   integer;
                out_0 : Out   integer );
end VhdlbBus_14;

architecture VHDLBBus_14_behavior of VhdlbBus_14 is
   begin

   Split : PROCESS

   BEGIN
	out_3 := input(3);	-- Extract individual bits from the bus
	out_2 := input(2);
	out_1 := input(1);
	out_0 := input(0);
   END PROCESS Split;

end VHDLBBus_14_behavior;

configuration CFG_VhdlbBus_14_BEHAVIORAL of VhdlbBus_14 is
   for VHDLBBus_14_behavior
   end for;
end CFG_VhdlbBus_14_BEHAVIORAL;
