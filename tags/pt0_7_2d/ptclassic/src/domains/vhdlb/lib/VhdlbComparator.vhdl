----------------------------------------------------------------------------
--  Compares two real-valued inputs.  If inputs are equal, output an integer
--  value of 1 (logical high), else output zero (logical low).
----------------------------------------------------------------------------

USE work.Datatypes.all;
USE work.Weilun_stuff.all;
USE work.std_logic_1164.all;
USE work.std_logic_signed.all;

entity VHDLBComparator is
      Generic( A_WIDTH  : integer;         -- inputa bus width
               B_WIDTH  : integer;         -- inputb bus width
               delay    : integer );       -- internal delay
      Port (  inputa : In    INTEGER_VECTOR(0 to (A_WIDTH-1) );
              inputb : In    INTEGER_VECTOR(0 to (B_WIDTH-1) );
              output : Out   integer );
end VHDLBComparator;

architecture VHDLBComparator_behavior of VHDLBComparator is
   begin

   Compare : PROCESS (inputa, inputb)

     VARIABLE temp1 : integer := CONV_INTEGER( Intvec_2_stdvec(inputa) );
     VARIABLE temp2 : integer := CONV_INTEGER( Intvec_2_stdvec(inputb) );
     
   BEGIN

     IF (temp1 = temp2) THEN
       output <= 1 after (1E6*time'val(integer'pos(delay)));
     ELSE
       output <= 0 after (1E6*time'val(integer'pos(delay)));
     END IF;
     
   END PROCESS Compare;

end VHDLBComparator_behavior;

configuration CFG_VHDLBComparator_BEHAVIORAL of VHDLBComparator is
   for VHDLBComparator_behavior
   end for;
end CFG_VHDLBComparator_BEHAVIORAL;
