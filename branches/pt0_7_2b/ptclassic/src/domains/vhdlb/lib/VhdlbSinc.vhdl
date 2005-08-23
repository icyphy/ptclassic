-------------------------------------------------------------------------------
--  Calculate the "sinc" function of the input (in radians), using the
--  predefined SIN() function in another package.  If input is zero,
--  the default output is 1.0
-------------------------------------------------------------------------------

library IEEE;
use IEEE.math_real.all;

entity VHDLBSinc is
      Port (  input : In    real;
	     output : Out   real );
end VHDLBSinc;

architecture VHDLBSinc_behavior of VHDLBSinc is
   begin

-------------------------------------------------------------------------------
--  If input is zero, output is 1.0.  Otherwise calculate the sinc function
-------------------------------------------------------------------------------
   main : PROCESS (input)
   CONSTANT delta : real := 0.000000001;      -- Almost zero
   
   BEGIN

     IF ((input > -delta) or (input < delta)) THEN
       output <= 1.0;			-- In case the input is nearly zero.
     ELSE
       output <= SIN( input )/input;	-- Otherwise calculate the output
     END IF;

   END PROCESS main;

end VHDLBSinc_behavior;

configuration CFG_VHDLBSinc_BEHAVIORAL of VHDLBSinc is
   for VHDLBSinc_behavior
   end for;
end CFG_VHDLBSinc_BEHAVIORAL;
