-------------------------------------------------------------------------------
--  Calculates the reciprocal of the input.  If the "magLimit" is zero, there
--  is no limit to the output's magnitude.  Otherwise, the absolute value of 
--  the output is limited to "magLimit".  In this case, even if the input
--  is zero, no divide-by-zero exception will occur.
-------------------------------------------------------------------------------

entity VHDLBReciprocal is
   Generic ( magLimit : real );
      Port (  input : In    real;
	     output : Out   real );
end VHDLBReciprocal;

architecture VHDLBReciprocal_behavior of VHDLBReciprocal is
   begin

   main : PROCESS (input)

   VARIABLE temp : real;
   
   BEGIN

     IF (magLimit = 0.0) THEN
       output <= 1/input;               -- No magnitude limit
     ELSE
       IF (input = 0.0) THEN
         output <= magLimit;            -- Zero input is limited!
       ELSE
         temp := 1/input;
         IF (temp > magLimit) THEN	-- Confine the output to +/- "magLimit"
           output <= magLimit;
         ELSIF (temp < -magLimit) THEN
           output <= -magLimit
         ELSE
           output <= temp;
         END IF;
       END IF;
     END IF;

   END PROCESS main;

end VHDLBReciprocal_behavior;

configuration CFG_VHDLBReciprocal_BEHAVIORAL of VHDLBReciprocal is
   for VHDLBReciprocal_behavior
   end for;
end CFG_VHDLBReciprocal_BEHAVIORAL;
