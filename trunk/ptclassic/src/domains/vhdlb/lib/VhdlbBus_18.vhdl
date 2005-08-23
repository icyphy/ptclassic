----------------------------------------------------------------------------
--  Since ptolemy doesn't allow the user to explicitly extract individual data
--  out of a bus (a multi-port), this star is written to allow the user
--  to convert an 8-bit-wide bus into 8 explicit wires.  If the output bus is
--  not 8 bits wide, an error message is printed.
----------------------------------------------------------------------------

USE work.datatypes.all;

entity VhdlbBus_18 is
      Generic ( I_WIDTH : integer );
      Port (   input : In    INTEGER_VECTOR(0 to (I_WIDTH-1) );
		out_7 : Out   integer;
		out_6 : Out   integer;
		out_5 : Out   integer;
		out_4 : Out   integer;
		out_3 : Out   integer;
                out_2 : Out   integer;
                out_1 : Out   integer;
                out_0 : Out   integer );
end VhdlbBus_18;

architecture VHDLBBus_18_behavior of VhdlbBus_18 is
   begin

   Split : PROCESS

   BEGIN
	out_7 := input(7);	-- Extract the individual bits from the bus.
	out_6 := input(6);
	out_5 := input(5);
	out_4 := input(4);
	out_3 := input(3);
	out_2 := input(2);
	out_1 := input(1);
	out_0 := input(0);
   END PROCESS Split;

end VHDLBBus_18_behavior;

configuration CFG_VhdlbBus_18_BEHAVIORAL of VhdlbBus_18 is
   for VHDLBBus_18_behavior
   end for;
end CFG_VhdlbBus_18_BEHAVIORAL;
