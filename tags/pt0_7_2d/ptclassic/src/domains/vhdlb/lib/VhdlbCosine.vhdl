-------------------------------------------------------------------------------
--  Calculates the cosine function of the input angle (in radians), using
--  a predefined function in another package.
-------------------------------------------------------------------------------

library IEEE;
use IEEE.math_real.all;

entity VHDLBCosine is
      Port (  input : In    real;
	     output : Out   real );
end VHDLBCosine;

architecture VHDLBCosine_behavior of VHDLBCosine is
   begin

   main : PROCESS (input)

   BEGIN

     output <= COS( input );	-- Calculate the output

   END PROCESS main;

end VHDLBCosine_behavior;

configuration CFG_VHDLBCosine_BEHAVIORAL of VHDLBCosine is
   for VHDLBCosine_behavior
   end for;
end CFG_VHDLBCosine_BEHAVIORAL;
