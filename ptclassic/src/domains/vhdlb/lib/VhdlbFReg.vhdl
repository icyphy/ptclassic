----------------------------------------------------------------------------
--  This register latches and stores a real-valued number at every rising
--  edge of the synchronization clock, after an internal delay.
----------------------------------------------------------------------------

entity VHDLBFReg is
    Generic( delay : integer );       -- internal delay
      Port (   clock : In    integer;
               input : In    real;
              output : Out   real );
end VHDLBFReg;

architecture VHDLBFReg_behavior of VHDLBFReg is
   begin

   Latch : PROCESS (clock)

   BEGIN
     -- Wait until the rising edge of the clock
     WAIT until clock = 0;
     WAIT until clock = 1;

     -- Latch the input after an internal delay.
     output <= input after (1E6*time'val(integer'pos(delay)));

   END PROCESS Latch;

end VHDLBFReg_behavior;

configuration CFG_VHDLBFReg_BEHAVIORAL of VHDLBFReg is
   for VHDLBFReg_behavior
   end for;
end CFG_VHDLBFReg_BEHAVIORAL;
