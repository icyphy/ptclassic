----------------------------------------------------------------------------
--  Multiplies two real-valued numbers.
----------------------------------------------------------------------------

entity VHDLBFMult is
      Port (  inputa : In    real;
	      inputb : In    real;
              output : Out   real );
end VHDLBFMult;

architecture VHDLBFMult_behavior of VHDLBFMult is
   begin

   mult : PROCESS (inputa,inputb)

   BEGIN

     -- When either inputs change, calculate a new output
     output <= inputa * inputb;

   END PROCESS mult;

end VHDLBFMult_behavior;

configuration CFG_VHDLBFMult_BEHAVIORAL of VHDLBFMult is
   for VHDLBFMult_behavior
   end for;
end CFG_VHDLBFMult_BEHAVIORAL;
