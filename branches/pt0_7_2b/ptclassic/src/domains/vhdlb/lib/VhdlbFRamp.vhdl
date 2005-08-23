-------------------------------------------------------------------------------
--  Generates a ramp signal to the output, synchronized by the clock signal.
--   The values starts at zero, and is incremented by "step" at the rising
--   edge of every clock cycle.
-------------------------------------------------------------------------------

entity VHDLBFRamp is
    Generic( step : real );
      Port (  clock : In    integer;
             output : Out   real );
end VHDLBFRamp;

architecture VHDLBFRamp_behavior of VHDLBFRamp is
   begin

   main : PROCESS (clock)

   VARIABLE temp : real := 0;

   BEGIN
     -- Wait until the rising edge of the clock
     WAIT until clock = 0;
     WAIT until clock = 1;

     output <= temp;		-- Generate output
     temp := temp + step;	-- Increment output amplitude for next cycle.

   END PROCESS main;

end VHDLBFRamp_behavior;

configuration CFG_VHDLBFRamp_BEHAVIORAL of VHDLBFRamp is
   for VHDLBFRamp_behavior
   end for;
end CFG_VHDLBFRamp_BEHAVIORAL;
