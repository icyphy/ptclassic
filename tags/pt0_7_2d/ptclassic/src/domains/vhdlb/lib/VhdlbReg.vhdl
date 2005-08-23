----------------------------------------------------------------------------
--  This register latches and stores a value on the bus at every rising
--  edge of the synchronization clock, after an internal delay.
----------------------------------------------------------------------------

USE work.Datatypes.all;
USE work.Weilun_stuff.all;
USE work.std_logic_1164.all;
USE work.std_logic_signed.all;

entity VHDLBReg is
      Generic( I_WIDTH  : integer;         -- input bus width
               O_WIDTH  : integer;         -- output bus width
               delay    : integer );       -- internal delay
      Port (   clock : In    integer;
               input : In    INTEGER_VECTOR(0 to (I_WIDTH-1) );
              output : Out   INTEGER_VECTOR(0 to (O_WIDTH-1) ) );

end VHDLBReg;

architecture VHDLBReg_behavior of VHDLBReg is
   begin

   Latch : PROCESS (clock)

   BEGIN
     -- Wait until the rising edge of the clock
     WAIT until clock = 0;
     WAIT until clock = 1;

     -- Latch the input after an internal delay.
     output <= input after (1E6*time'val(integer'pos(delay)));

   END PROCESS Latch;

end VHDLBReg_behavior;

configuration CFG_VHDLBReg_BEHAVIORAL of VHDLBReg is
   for VHDLBReg_behavior
   end for;
end CFG_VHDLBReg_BEHAVIORAL;
