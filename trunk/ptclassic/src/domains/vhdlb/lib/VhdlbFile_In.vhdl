-------------------------------------------------------------------------------
--  Reads integers from a user-specified file.  The file is opened and the data
--  (one on each line of the text file) are read and then sent to the output
--  port at the rising edges of the synchronization clock.  All zero
--  characters are treated as a low bit and a zero value is sent to the
--  output.  All other characters read are considered high bits and a value of
--  1 is sent to the output.  When the EOF is reached, the "stop" signal is
--  asserted (it changes from zero to a non-zero value).
--
--  Also see "File_Store" star.  (VhdlfFile_Store.vhdl)
-------------------------------------------------------------------------------

USE STD.textio.all;

entity VHDLBFILE_IN is
    Generic( In_File : string );
      Port (   clock : In    integer;
                done : Out   integer;
              output : Out   integer );
end VHDLBFILE_IN;

architecture VHDLBFile_In_behavior of VHDLBFILE_IN is
   begin

   FI : PROCESS (clock)

     file TV         : TEXT is in In_File;
     VARIABLE In_bit : character;
     VARIABLE temp   : integer;
     
   BEGIN  --  PROCESS FI 
     -- Wait for the rising edge of the clock
     WAIT until clock = 0;
     WAIT until clock = 1;

     readline(In_File,In_bit);	 -- Read a line from the file

     IF (endfile(In_File)) THEN
       done <= 1;		 -- If reached EOF, turn on the "done" bit.
       wait;
     ELSE			 -- Otherwise, output the bits.
       IF (In_bit = '0') THEN
         temp = 0;
       ELSE
         temp = 1;
       END IF;
       output <= temp;
       done   <= 0;
     END IF;
   END PROCESS FI;

end VHDLBFile_In_behavior;

configuration CFG_VHDLBFILE_IN_BEHAVIORAL of VHDLBFILE_IN is
   for VHDLBFile_In_behavior
   end for;
end CFG_VHDLBFILE_IN_BEHAVIORAL;
