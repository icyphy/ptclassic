-------------------------------------------------------------------------------
--  Converts a binary representation of an integer into an integer value.
--  The input representation of the integer is stored in an array of integers
--  The MSB (most significant bit) is the sign bit.  Note that this
--  representation is in sign-magnitude form, not 2's-complement.
--  If any of the input bits is tri-state (integer value is 3), then the
--  previous output value is sent to the output again.
-------------------------------------------------------------------------------

use work.Datatypes.all;

entity VHDLBBusToNum is
   Generic (  I_WIDTH : integer;
             previous : integer );
      Port (  input : In  INTEGER_VECTOR(0 to (I_WIDTH-1));
             output : Out integer );
end VHDLBBusToNum;

architecture VHDLBBusToNum_behavior of VHDLBBusToNum is
   begin

   main : PROCESS (input)

   VARIABLE temp1 : INTEGER_VECTOR(0 to (I_WIDTH-1));
   VARIABLE temp2 : integer;	 -- Used for output value
   VARIABLE temp3 : integer;	 -- This is the magnitude of the output value
   VARIABLE sign  : integer;	 -- Sign of the output value
   VARIABLE tris  : integer;     -- Is the integer a tri-state??
               
   BEGIN

     temp1 := input;
     temp3 := 0;
     tris := 0;
     
     -- If there is a tri-state, output previous value.
     L1 : FOR i IN 0 to (I_WIDTH-1) LOOP
       IF (temp1(i) = 3) THEN
         temp2 := previous;
         tris := 1;
       END IF;
     END LOOP L1;

     -- Checking for the sign of the integer
     IF (temp1(I_WIDTH-1) = 0) THEN
       sign := 1;
     ELSE
       sign := -1;
     END IF;

     -- Put together the magnitude of the number.
     L2 : FOR i IN (I_WIDTH-2) downto 0 LOOP
       IF (temp1(i) /= 0) THEN
         temp3 := temp3 + 1;
       END IF;
       temp3 := temp3 * 2;
     END LOOP L2;

     -- Now pick the correct number to send to the output
     IF (tris = 1) THEN
       output <= temp2;		  -- Output the previous value (tri-state)
       previous := temp2;
     ELSE
       output <= temp3 * sign;	  -- Output the calculated value
       previous := temp3 * sign;
     END IF;
     
   END PROCESS main;

end VHDLBBusToNum_behavior;

configuration CFG_VHDLBBusToNum_BEHAVIORAL of VHDLBBusToNum is
   for VHDLBBusToNum_behavior
   end for;
end CFG_VHDLBBusToNum_BEHAVIORAL;
