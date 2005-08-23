-------------------------------------------------------------------------------
--  Limits the real-valued output to the specified upper and lower bounds.
--  ("top" and "bottom", respectively)  All values falling between the bounds
--  are preserved, while values falling outside of the bounds will be clipped
--  to the values at the upper or lower bound.
-------------------------------------------------------------------------------

entity VHDLBLimit is
   Generic (    top : real;
             bottom : real );
      Port (  input : In    real;
             output : Out   real );
end VHDLBLimit;

architecture VHDLBLimit_behavior of VHDLBLimit is
   begin

   main : PROCESS (input)
                      
   BEGIN

     IF (input > top) THEN
       output <= top;
     ELSIF (input < bottom) THEN
       output <= bottom;
     ELSE
       output <= input;
     END IF;
     
   END PROCESS main;

end VHDLBLimit_behavior;

configuration CFG_VHDLBLimit_BEHAVIORAL of VHDLBLimit is
   for VHDLBLimit_behavior
   end for;
end CFG_VHDLBLimit_BEHAVIORAL;
