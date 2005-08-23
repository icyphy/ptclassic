-------------------------------------------------------------------------------
--  Calculates the natural log of the input value, using a predefined
--  function in another package.  If input is invalid (input <= 0), than a
--  negative value of "min" is used.
-------------------------------------------------------------------------------

library IEEE;
use IEEE.math_real.all;

entity VHDLBLog is
   Generic ( min : real );	   -- This value is used if input is invalid.
      Port ( input : In    real;
            output : Out   real );
end VHDLBLog;

architecture VHDLBLog_behavior of VHDLBLog is
   begin

   main : PROCESS (input)

   BEGIN

     IF (input > 0) THEN
       output <= LOG( input );	      -- Calculate the output normally
     ELSE
       output <= min;                 -- Error!!!!  (input <= 0)
     END IF;

   END PROCESS main;

end VHDLBLog_behavior;

configuration CFG_VHDLBLog_BEHAVIORAL of VHDLBLog is
   for VHDLBLog_behavior
   end for;
end CFG_VHDLBLog_BEHAVIORAL;
