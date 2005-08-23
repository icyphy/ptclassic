-------------------------------------------------------------------------------
--  Outputs a logical high (an integer value of 1) to the output at "interval"
--  time intervals.  (measured in nanoseconds)
-------------------------------------------------------------------------------

entity VHDLBVdd is
    Generic( interval : integer );   -- interval between output transactions
      Port ( output : Out   integer );

end VHDLBVdd;

architecture VHDLBVdd_behavior of VHDLBVdd is
   begin

   main : PROCESS

   BEGIN

     output <= 1;	-- Output the value every "interval" nanoseconds.
     wait for (1E6*time'val(integer'pos(interval)));

   END PROCESS main;

end VHDLBVdd_behavior;

configuration CFG_VHDLBVdd_BEHAVIORAL of VHDLBVdd is
   for VHDLBVdd_behavior
   end for;
end CFG_VHDLBVdd_BEHAVIORAL;
