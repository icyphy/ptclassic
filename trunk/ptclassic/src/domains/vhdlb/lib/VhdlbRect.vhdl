-------------------------------------------------------------------------------
--  Generates a rectangular signal with amplitude of "height" (default 1.0)
--  and "width" samples (default 8).  If "period" parameter is greater than
--  zero, then the rectangular pulse is repeated after "period" samples.
--  The real-valued output is sent to the output port at the rising
--  edges of the synchronization clock.
-------------------------------------------------------------------------------

entity VHDLBRect is
   Generic ( height : real;
              width : integer;
             period : integer );
      Port (  clock : In    integer;
             output : Out   real );
end VHDLBRect;

architecture VHDLBRect_behavior of VHDLBRect is
   begin

   main : PROCESS (clock)
                      
   VARIABLE count : integer := 0;
   
   BEGIN
     -- Wait until rising edge of the clock
     WAIT until clock = 0;
     WAIT until clock = 1;

     -- Generate the output.
     IF (count < width) THEN
       output <= height;
     END IF;

     count := count + 1;

     -- Are we repeating the waveform after "period" samples?
     IF ((period > 0) and (count >= period)) THEN
       count := 0;
     END IF;

   END PROCESS main;

end VHDLBRect_behavior;

configuration CFG_VHDLBRect_BEHAVIORAL of VHDLBRect is
   for VHDLBRect_behavior
   end for;
end CFG_VHDLBRect_BEHAVIORAL;
