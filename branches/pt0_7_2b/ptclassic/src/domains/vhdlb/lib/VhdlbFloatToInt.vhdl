-------------------------------------------------------------------------------
--  Converts a real number to an integer (rounding to the nearest integer)
-------------------------------------------------------------------------------

entity VHDLBFloatToInt is
      Port (  input : In    real;
             output : Out   integer );
end VHDLBFloatToInt;

architecture VHDLBFloatToInt_behavior of VHDLBFloatToInt is
   begin

   main : PROCESS (input)

   BEGIN
     output <= integer (input);
   END PROCESS main;

end VHDLBFloatToInt_behavior;

configuration CFG_VHDLBFloatToInt_BEHAVIORAL of VHDLBFloatToInt is
   for VHDLBFloatToInt_behavior
   end for;
end CFG_VHDLBFloatToInt_BEHAVIORAL;
