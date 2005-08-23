-------------------------------------------------------------------------------
--  This star computes the normalized Dirichlet kernel (also called the aliased
--  sinc function):
--
--  d(x) = sin(N x / 2) / (N sin(x/2)), where N is the length of the pulse.
--
--  The Dirichlet kernel is the discrete-time Fourier transform (DTFT) of a
--  sampled pulse function.  The "N" parameter is the length of the pulse.
--
--  The value of the normalized Dirichlet kernel near x = 0 is always 1 (when
--  input is nearly zero, l'Hopital's Rule is used to calculate the Dirichlet
--  kernel ( cos( N * x/2 )/cos( x/2 ) ), and the normalized Dirichlet kernel
--  oscillates between -1 and +1.  Otherwise the normal formula is used to
--  calculate the Dirichlet kernel.
--  The normalized Dirichlet kernel is periodic in x with a period of either
--  2*pi when N is odd or 4*pi when N is even.
-------------------------------------------------------------------------------

library IEEE;
use IEEE.math_real.all;

entity VHDLBDirichlet is
   Generic ( N : integer );
      Port (  input : In    real;
	     output : Out   real );
end VHDLBDirichlet;

architecture VHDLBDirichlet_behavior of VHDLBDirichlet is
   begin

   main : PROCESS (input)

   CONSTANT delta : real := 0.000000001;      -- Almost zero

   VARIABLE x              : real;
   VARIABLE sinInDenom     : real;
   VARIABLE length         : real := N;
   VARIABLE dirichletValue : real;
      
   BEGIN

     x := input;		-- Initialize some values.
     sinInDenom := SIN( x/2 );

     IF (((-delta) < sinInDenom) AND (sinInDenom < delta)) THEN
       -- Use l'Hopital's when sin(x/2) is nearly zero.
       dirichletValue := COS( length * (x/2) )/COS( x/2 );
     ELSE
       -- Else, compute it using the definition.
       dirichletValue := SIN( length * (x/2) )/(length * sinInDenom);
     END IF;

     output <= dirichletValue;

   END PROCESS main;

end VHDLBDirichlet_behavior;

configuration CFG_VHDLBDirichlet_BEHAVIORAL of VHDLBDirichlet is
   for VHDLBDirichlet_behavior
   end for;
end CFG_VHDLBDirichlet_BEHAVIORAL;
