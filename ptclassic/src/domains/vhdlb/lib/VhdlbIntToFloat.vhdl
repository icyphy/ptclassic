-------------------------------------------------------------------------------
--  Converts an integer to a real number. (using type-casting)
-------------------------------------------------------------------------------

entity VHDLBIntToFloat is
      Port (  input : In    integer;
             output : Out   real );
end VHDLBIntToFloat;

architecture VHDLBIntToFloat_behavior of VHDLBIntToFloat is
   begin

   main : PROCESS (input)

   BEGIN
     output <= real (input);
   END PROCESS main;

end VHDLBIntToFloat_behavior;

configuration CFG_VHDLBIntToFloat_BEHAVIORAL of VHDLBIntToFloat is
   for VHDLBIntToFloat_behavior
   end for;
end CFG_VHDLBIntToFloat_BEHAVIORAL;
