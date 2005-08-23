-------------------------------------------------------------------------------
--  Calculates the exponential function of the input, using a predefined
--  function in another package.
-------------------------------------------------------------------------------

library IEEE;
use IEEE.math_real.all;

entity VHDLBExp is
      Port (  input : In    real;
  	     output : Out   real );
end VHDLBExp;

architecture VHDLBExp_behavior of VHDLBExp is
   begin

   main : PROCESS (input)

   BEGIN

     output <= EXP( input );	-- Calculate a new output when input changes.

   END PROCESS main;

end VHDLBExp_behavior;

configuration CFG_VHDLBExp_BEHAVIORAL of VHDLBExp is
   for VHDLBExp_behavior
   end for;
end CFG_VHDLBExp_BEHAVIORAL;
