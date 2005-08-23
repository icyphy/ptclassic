-------------------------------------------------------------------------------
--  Convert input to a decibels (dB) scale. Zero and negative values
--  are assigned the value "min" (default -100). The "inputIsPower"
--  parameter should be set to "Yes" if the input signal is a power
--  measurement (the conversion formula is 20 * log(input) in this case).
--  Otherwise if the input is an amplitude measurement, the conversion formula
--  will be 10 * log(input).
--
--  For inputs less than or equal to zero, the output is defaulted to the "min"
--  value.  For inputs greater than zero, the output is either the "min" or the
--  value obtained by the formula gain * log( input ), whichever is greater.
-------------------------------------------------------------------------------

library IEEE;
USE IEEE.math_real.all;

entity VHDLBDB is
   Generic (         min : real;
            inputIsPower : integer );
      Port (  input : In    real;
             output : Out   real );
end VHDLBDB;

architecture VHDLBDB_behavior of VHDLBDB is
   begin

   main : PROCESS (input)

   VARIABLE gain : real;
   VARIABLE temp : real;
      
   BEGIN

     temp := input;

     IF (inputIsPower /= 0) THEN
       gain := 10.0;                 -- If Power measurement, gain = 10
     ELSE
       gain := 20.0;		     -- If Amplitude measurement, gain = 20
     END IF;

     IF (temp <= 0.0) THEN
       output <= min;		       -- If input <= 0, output is "min"
     ELSE
       temp := gain * LOG( 10, temp ); -- Returns log10 of input
       IF (temp < min) THEN            -- If result is less than min, use min
         temp := min;
       END IF;
       output <= temp;		       -- Drive the output.
     END IF;
   END PROCESS main;

end VHDLBDB_behavior;

configuration CFG_VHDLBDB_BEHAVIORAL of VHDLBDB is
   for VHDLBDB_behavior
   end for;
end CFG_VHDLBDB_BEHAVIORAL;
