-------------------------------------------------------------------------------
--  Calclates the square root of the input value, using a predefined
--  function from another package.
-------------------------------------------------------------------------------

library IEEE;
USE IEEE.math_real.all;

entity VHDLBSqrt is
      Port (  input : In    real;
             output : Out   real );
end VHDLBSqrt;

architecture VHDLBSqrt_behavior of VHDLBSqrt is
   begin

   main : PROCESS (input)

   BEGIN

     output <= SQRT( input );	-- Calculate the output

   END PROCESS main;

end VHDLBSqrt_behavior;

configuration CFG_VHDLBSqrt_BEHAVIORAL of VHDLBSqrt is
   for VHDLBSqrt_behavior
   end for;
end CFG_VHDLBSqrt_BEHAVIORAL;
