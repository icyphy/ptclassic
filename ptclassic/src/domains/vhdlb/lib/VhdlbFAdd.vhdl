----------------------------------------------------------------------------
--  Adds two real-valued numbers together.
----------------------------------------------------------------------------

entity VHDLBFAdd is
      Port (  inputa : In    real;
	      inputb : In    real;
              output : Out   real );
end VHDLBFAdd;

architecture VHDLBFAdd_behavior of VHDLBFAdd is
   begin

   addition : PROCESS (inputa,inputb)

   BEGIN

     -- When either inputs change, calculate a new output
     output <= inputa + inputb;

   END PROCESS addition;

end VHDLBFAdd_behavior;

configuration CFG_VHDLBFAdd_BEHAVIORAL of VHDLBFAdd is
   for VHDLBFAdd_behavior
   end for;
end CFG_VHDLBFAdd_BEHAVIORAL;
