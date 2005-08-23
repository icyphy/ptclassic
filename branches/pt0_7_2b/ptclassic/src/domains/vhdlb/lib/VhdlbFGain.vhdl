----------------------------------------------------------------------------
--  Multiplies the input with a real-valued gain.
----------------------------------------------------------------------------

entity VHDLBFGain is
    Generic( gain : real );
      Port (  input : In    real;
             output : Out   real );
end VHDLBFGain;

architecture VHDLBFGain_behavior of VHDLBFGain is
   begin

   mult : PROCESS (input)

   BEGIN

     -- Calculate a new output when the input changes.
     output <= input * gain;

   END PROCESS mult;

end VHDLBFGain_behavior;

configuration CFG_VHDLBFGain_BEHAVIORAL of VHDLBFGain is
   for VHDLBFGain_behavior
   end for;
end CFG_VHDLBFGain_BEHAVIORAL;
