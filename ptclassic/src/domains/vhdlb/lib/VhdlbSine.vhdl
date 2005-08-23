-------------------------------------------------------------------------------
--  Calculate the sine function of the input angle (in radians) using a pre-
--  defined function in another package.
-------------------------------------------------------------------------------

library IEEE;
use IEEE.math_real.all;

entity VHDLBSine is
      Port (  input : In    real;
	     output : Out   real );
end VHDLBSine;

architecture VHDLBSine_behavior of VHDLBSine is
   begin

   main : PROCESS (input)

   BEGIN

     output <= SIN( input );	-- Calculate the output.

   END PROCESS main;

end VHDLBSine_behavior;

configuration CFG_VHDLBSine_BEHAVIORAL of VHDLBSine is
   for VHDLBSine_behavior
   end for;
end CFG_VHDLBSine_BEHAVIORAL;
