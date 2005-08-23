-------------------------------------------------------------------------------
--  Convolves two causal finite sequences.
--
--  Set truncationDepth larger than the number of output samples of interest.
--  In the current implementation, you should set the truncation depth larger
--  than the number of output samples of interest.  If it's smaller, you will
--  get unexpected results after truncationDepth samples.
--
--  Inputs are sampled at the rising edges of the clock.  Two arrays are used
--  to store past input values.  The arrays are flushed (initialized to zero)
--  at the beginning of the simulation or after "truncationDepth" samples have
--  been sent to the output.  Otherwise, the contents of the arrays are shifted
--  back by one element so that the most recent input samples can be stored at
--  the beginning of the arrays.  All transactions occur at the rising edge of
--  the clock cycle.
-------------------------------------------------------------------------------

entity VHDLBConvolve is
   Generic ( truncationDepth : integer );
      Port (  clock : In    integer;
	     inputa : In    real;
	     inputb : In    real;
             output : Out   real );
end VHDLBConvolve;

architecture VHDLBConvolve_behavior of VHDLBConvolve is
   begin

   main : PROCESS (clock)

   VARIABLE iterationCount : integer := 0;   -- Count current iteration.
   VARIABLE count          : integer;
   VARIABLE index          : integer;
   VARIABLE sum        : real;		     -- Convolution sum.
   VARIABLE in_array_a : array (0 to truncationDepth-1) of real;
   VARIABLE in_array_b : array (0 to truncationDepth-1) of real;
      
   BEGIN
     -- Wait until the rising edge of the clock
     WAIT until clock = 0;
     WAIT until clock = 1;

     -- Initialization stuff
     sum := 0.0;

     --------------------------------------------------------------------------
     -- When just starting the convolution, we flush the arrays.
     -- Otherwise, we shift all samples back, and insert the most recent sample
     --  in the smallest position of the array. (latest sample has index zero)
     --------------------------------------------------------------------------
     IF (iterationCount = 0) THEN
       FOR i IN 0 to (truncationDepth-1) LOOP		-- Flush the arrays.
         in_array_a(i) := 0.0;
         in_array_b(i) := 0.0;
       END LOOP;
     ELSE
       FOR j IN (truncationDepth-1) downto 1 LOOP	-- Shift the arrays.
         in_array_a(j) := in_array_a(j-1);
         in_array_b(j) := in_array_b(j-1);
       END LOOP;
       in_array_a(0) := inputa;		 -- Store new samples in the arrays.
       in_array_b(0) := inputb;
     END IF;

     -- Start convolution
     count := iterationCount;
     FOR k IN 0 to (truncationDepth-1) LOOP
       index := count - k;
       IF (index < 0) THEN
         index := index + truncationDepth;
       END IF;
       sum := sum + (in_array_a(index)*in_array_b(k));
     END LOOP;
     output <= sum;

     -- Incrementing the interation count.
     IF (count >= (truncationDepth-1)) THEN
       iterationCount := 0;
     ELSE
       iterationCount := count + 1;
     END IF;
   END PROCESS main;

end VHDLBConvolve_behavior;

configuration CFG_VHDLBConvolve_BEHAVIORAL of VHDLBConvolve is
   for VHDLBConvolve_behavior
   end for;
end CFG_VHDLBConvolve_BEHAVIORAL;
