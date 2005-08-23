-------------------------------------------------------------------------------
--  Outputs a logical low (a zero integer value) to the output at "interval"
--  time intervals.  (measured in nanoseconds)
-------------------------------------------------------------------------------

entity VHDLBGnd is
    Generic( interval : integer );   -- interval between output transactions
      Port ( output : Out   integer );

end VHDLBGnd;

architecture VHDLBGnd_behavior of VHDLBGnd is
   begin

   main : PROCESS

   BEGIN

     output <= 0;	-- Output the value every "interval" nanoseconds.
     wait for (1E6*time'val(integer'pos(interval)));

   END PROCESS main;

end VHDLBGnd_behavior;

configuration CFG_VHDLBGnd_BEHAVIORAL of VHDLBGnd is
   for VHDLBGnd_behavior
   end for;
end CFG_VHDLBGnd_BEHAVIORAL;
