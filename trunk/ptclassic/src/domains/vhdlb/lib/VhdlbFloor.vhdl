-------------------------------------------------------------------------------
--  Outputs the biggest integer value smaller than or equal to the
--  real-valued input, using a predefined function in a package.
-------------------------------------------------------------------------------

library IEEE;
use IEEE.math_real.all;

entity VHDLBFloor is
      Port (  input : In    real;
             output : Out   integer );
end VHDLBFloor;

architecture VHDLBFloor_behavior of VHDLBFloor is
   begin

   main : PROCESS (input)

   BEGIN

     output <= integer (FLOOR(input));	-- Obtain the output.

   END PROCESS main;

end VHDLBFloor_behavior;

configuration CFG_VHDLBFloor_BEHAVIORAL of VHDLBFloor is
   for VHDLBFloor_behavior
   end for;
end CFG_VHDLBFloor_BEHAVIORAL;
