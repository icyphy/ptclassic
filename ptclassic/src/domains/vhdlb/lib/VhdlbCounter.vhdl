-------------------------------------------------------------------------------
--  This is a simple up-down, 8-bit, resettable counter.
--
--  If the "up-down" bit is 1, this is an up-counter, otherwise this is a
--  down-counter.
--
--  Depending on the "up-down" bit value, "\reset" resets the output to either
--  0 (as an up-counter) or 0xFF (as a down-counter) at the rising edge of the
--  clock.  Notice that "\reset" is positive-low.  The 8-bit output value is
--  maintained (unchanged) if the positive-low "\enable" is disabled, otherwise
--  the output value will be updated after every clock cycle.
--
--  The inputs are sampled at the rising edges of the clock and the output is
--  available after an user-specified internal delay after the rising edges of
--  the clock.
-------------------------------------------------------------------------------

USE work.Datatypes.all;
USE work.Weilun_stuff.all;
USE work.std_logic_1164.all;
USE work.std_logic_arith.all;

entity VHDLBCounter is
    Generic( O_WIDTH  : integer;         -- output bus width
             delay    : integer );       -- internal delay
      Port (   clock : In    integer;
	       reset : In    integer;
	      enable : In    integer;
	     up_down : In    integer;
              output : Out   INTEGER_VECTOR(0 to (O_WIDTH-1) ) );
end VHDLBCounter;

architecture VHDLBCounter_behavior of VHDLBCounter is
   begin

   Count : PROCESS (clock)

     VARIABLE temp  : integer;
     VARIABLE temp2 : std_logic_vector( 7 downto 0 );
     
   BEGIN
     -- Wait until the rising edge of the clock.
     WAIT until clock = 0;
     WAIT until clock = 1;

     IF (reset = 0) THEN                 -- Are we resetting?
       IF (up_down = 1) THEN
         temp := 0;                        -- Reset to 0
       ELSIF (up_down = 0) THEN
         temp := 255;                      -- Reset to 0xFF
       END IF;
     ELSIF (reset = 1) THEN              -- Is it enabled?
       IF (enable = 0) THEN                -- Yes, it's enabled
         IF (up_down = 1) THEN
           temp := temp + 1;                 -- count up
         ELSIF (up_down = 0) THEN
           temp := temp - 1;                 -- count down
         END IF;
       END IF;
     END IF;

     -- Generate the output value.
     temp2 := CONV_STD_LOGIC_VECTOR( temp, 8 );
     output <= Stdvec_2_intvec(temp2) after (1E6*time'val(integer'pos(delay)));

   END PROCESS Count;

end VHDLBCounter_behavior;

configuration CFG_VHDLBCounter_BEHAVIORAL of VHDLBCounter is
   for VHDLBCounter_behavior
   end for;
end CFG_VHDLBCounter_BEHAVIORAL;
