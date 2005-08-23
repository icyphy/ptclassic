-------------------------------------------------------------------------------
--  Outputs logical high (integer value 1) if the top input is a multiple of
--  the bottom input.  Else outputs a zero (logical low).
--  Note that if the bottom input is zero, the output is defaulted to be zero.
-------------------------------------------------------------------------------

entity VHDLBMultiple is
      Port (    top : In    integer;
             bottom : In    integer;
               mult : Out   integer );
end VHDLBMultiple;

architecture VHDLBMultiple_behavior of VHDLBMultiple is
   begin

   main : PROCESS (top, bottom)

   BEGIN

     IF( bottom <= 0 ) THEN
       mult <= 0;	-- If bottom is zero, than output is defaulted to zero.
     ELSE
       IF( (top rem bottom) = 0 ) THEN	-- Is the remainder of the division 0?
	 mult <= 1;
       ELSE
	 mult <= 0;
       END IF;
     END IF;

   END PROCESS main;

end VHDLBMultiple_behavior;

configuration CFG_VHDLBMultiple_BEHAVIORAL of VHDLBMultiple is
   for VHDLBMultiple_behavior
   end for;
end CFG_VHDLBMultiple_BEHAVIORAL;
