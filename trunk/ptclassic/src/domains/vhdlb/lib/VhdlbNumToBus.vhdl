-------------------------------------------------------------------------------
--  Converts an input integer into a 8-bit, 16-bit, or 32-bit representation.
--  This representation is stored in an array of integers.  The most
--  significant bit (MSB) is the sign bit.  Note that this representation is a
--  sign-magnitude representation, not 2's-complement.
-------------------------------------------------------------------------------

use work.Datatypes.all;

entity VHDLBNumToBus is
   Generic ( O_WIDTH : integer );
      Port (  input : In  integer;
             output : Out INTEGER_VECTOR(0 to (O_WIDTH-1)) );
end VHDLBNumToBus;

architecture VHDLBNumToBus_behavior of VHDLBNumToBus is
   begin

   main : PROCESS (input)

   VARIABLE temp  : integer;
   VARIABLE temp2 : INTEGER_VECTOR(0 to (O_WIDTH-1));
      
   BEGIN

     temp := input;

     -- Store the sign bit.
     IF (temp < 0) THEN
       temp2( O_WIDTH-1 ) := 1;
       temp := abs temp;
     ELSE
       temp2( O_WIDTH-1 ) := 0;
     END IF;

     -- Now, extract the bits, starting from the least significant.
     L1 : FOR i IN 0 to (O_WIDTH-2) LOOP
       temp2(i) := temp AND 0x1;
       temp := temp / 2;
     END LOOP L1;

     output <= temp2;		-- Drive the output bits

   END PROCESS main;

end VHDLBNumToBus_behavior;

configuration CFG_VHDLBNumToBus_BEHAVIORAL of VHDLBNumToBus is
   for VHDLBNumToBus_behavior
   end for;
end CFG_VHDLBNumToBus_BEHAVIORAL;
