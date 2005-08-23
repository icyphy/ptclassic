-------------------------------------------------------------------------------
--  Stores the incoming bits (integers) at constant intervals (synchronized by
--  the clock) into a file.  A zero input value is treated as a low bit and
--  all nonzero values are treated as high bits.
--
--  A user-specified file is opened to store the incoming data at constant
--  intervals (at the rising edges of the synchronization clock signal).  All
--  values are stored on separate lines in the text file.  When the "stop"
--  signal goes high (from zero to non-zero), the file is closed and all
--  following data are ignored (This is based on the assumption that the 
--  lincoming signal is no longer of interest to us once the "stop" signal
--  is asserted)
-------------------------------------------------------------------------------

USE STD.textio.all;

entity VHDLBFILE_STORE is
    Generic( Out_File : string );
      Port ( clock : In    integer;
              stop : In    integer;    -- Goes high if EOF is reached @ input
             input : In    integer );
end VHDLBFILE_STORE;

architecture VHDLBFile_Store_behavior of VHDLBFILE_STORE is
   begin

   FS : PROCESS (clock)

     file OV : TEXT is out Out_File;
     
   BEGIN  --  PROCESS FS 
     -- Wait for the rising edge of the clock
     WAIT until clock = 0;
     WAIT until clock = 1;

     IF (stop = 1) THEN       -- EOF is reached from input file
       wait;
     ELSE		      -- Otherwise store the input bits
       IF (input = 0) THEN
         write( OV, '0' );
       ELSE
         write( OV, '1' );
       END IF;
     END IF;

   END PROCESS FS;

end VHDLBFile_Store_behavior;

configuration CFG_VHDLBFILE_STORE_BEHAVIORAL of VHDLBFILE_STORE is
   for VHDLBFile_Store_behavior
   end for;
end CFG_VHDLBFILE_STORE_BEHAVIORAL;
