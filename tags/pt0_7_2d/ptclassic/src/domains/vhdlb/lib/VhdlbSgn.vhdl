-------------------------------------------------------------------------------
--  This star calculates the signum function of the real-valued input.
--  If input is positive (and zero), the integer output is 1; otherwise
--  output is -1.
-------------------------------------------------------------------------------

entity VHDLBSgn is
      Port (  input : In    real;
             output : Out   integer );
end VHDLBSgn;

architecture VHDLBSgn_behavior of VHDLBSgn is
   begin

   main : PROCESS (input)

   BEGIN

     IF( input >= 0.0 ) THEN
       output <= 1;
     ELSE
       output <= -1;
     ENF IF;

   END PROCESS main;

end VHDLBSgn_behavior;

configuration CFG_VHDLBSgn_BEHAVIORAL of VHDLBSgn is
   for VHDLBSgn_behavior
   end for;
end CFG_VHDLBSgn_BEHAVIORAL;
