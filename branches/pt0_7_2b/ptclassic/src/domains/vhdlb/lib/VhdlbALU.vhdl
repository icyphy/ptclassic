-------------------------------------------------------------------------------
--  ALU takes two equal-sized integer vectors, convert them to std_logic
--   vectors, and does required operations depending on the value of "sel":
--
--   sel = "0000"  :   Output = A + B
--         "0001"  :   Output = A - B
--
--         "0010"  :   Output = A << B,  zeros are used to pad
--         "0011"  :   Output = A >> B,  zeros are used to pad
--         "0100"  :   Output = A >> B,  sign bit is used to pad
--
--         "0101"  :   Output = A and B
--         "0110"  :   Output = A nand B
--         "0111"  :   Output = A or B
--         "1000"  :   Output = A nor B
--         "1001"  :   Output = A xor B
--         "1010"  :   Output = not A
--
--  All inputs are sampled at the rising edges of the clock, and the output
--  is available after an user-specified internal delay, measured in nano-
--  seconds.
-------------------------------------------------------------------------------

USE work.Datatypes.all;
USE work.Weilun_stuff.all;
USE work.std_logic_1164.all;
USE work.std_logic_signed.all;

entity VHDLBALU is
      Generic( A_WIDTH  : integer;         -- inputa bus width
               B_WIDTH  : integer;         -- inputb bus width
               O_WIDTH  : integer;         -- output bus width
               S_WIDTH  : integer;         -- selector bus width
               delay    : integer );       -- internal delay
      Port (   clock : In    integer;
              inputa : In    INTEGER_VECTOR(0 to (A_WIDTH-1) );
              inputb : In    INTEGER_VECTOR(0 to (B_WIDTH-1) );
              output : Out   INTEGER_VECTOR(0 to (O_WIDTH-1) );
              sel    : In    INTEGER_VECTOR(0 to (S_WIDTH-1) ) );
end VHDLBALU;

architecture VHDLBALU_behavior of VHDLBALU is
   begin

   ALU : PROCESS (clock)

     VARIABLE op : integer          := CONV_INTEGER( Intvec_2_stdvec(sel) );
     VARIABLE a  : std_logic_vector := Intvec_2_stdvec( inputa );
     VARIABLE b  : std_logic_vector := Intvec_2_stdvec( inputb );
     
     VARIABLE temp1 : integer;
     VARIABLE temp2 : std_logic_vector(O_WIDTH downto 0);
     
   BEGIN
     -- Wait until the rising edge of the clock.
     WAIT until clock = 0;
     WAIT until clock = 1;

     CASE op IS

       -- The arithmetic functions (add and subtract)
       WHEN 0 => temp2 := "+"(a, b);
       WHEN 1 => temp2 := "-"(a, b);

       -- Shift zeros from the right.  (Shift to left)
       WHEN 2 => temp1 := CONV_INTEGER( b );
                 FOR i IN 0 to (temp1-1) LOOP
                   temp2(i) := '0';
                 END LOOP;
                 FOR i IN temp1 to (A_WIDTH-1) LOOP
                   temp2(i) := a(i-temp1);
                 END LOOP;

       -- Shift zeros from the left.  (Shift to right)
       WHEN 3 => temp1 := CONV_INTEGER( b );
                 FOR i IN (A_WIDTH - temp1) to ( A_WIDTH-1 ) LOOP
                   temp2(i) := '0';
                 END LOOP;
                 FOR i IN 0 to ((A_WIDTH-1) - temp1) LOOP
                   temp2(i) := INPUT_A(i+temp1);
                 END LOOP;

       -- Shift sign bit from the left.  (Shift to right)
       WHEN 4 => temp1 := CONV_INTEGER( b );
                 FOR i IN (A_WIDTH - temp1) to ( A_WIDTH-1 ) LOOP
                   IF (a( A_WIDTH-1 ) = '1') THEN
                     temp2(i) := '1';
                   ELSE
                     temp2(i) := '0';
                   END IF;
                 END LOOP;
                 FOR i IN 0 to ((A_WIDTH-1) - temp1) LOOP
                   temp2(i) := INPUT_A(i+temp1);
                 END LOOP;

       -- The logical functions.
       WHEN 5 => temp2 := "and"(a, b);
       WHEN 6 => temp2 := "nand"(a, b);
       WHEN 7 => temp2 := "or"(a, b);
       WHEN 8 => temp2 := "nor"(a, b);
       WHEN 9 => temp2 := "xor"(a, b);
       WHEN 10 => temp2 := "not"(a);

       -- If the select code is incorrect, nothing is done.
       WHEN OTHERS => temp2 := a;
     END CASE;

     -- Generate the output
     output <= Stdvec_2_intvec(temp2) after (1E6*time'val(integer'pos(delay)));

   END PROCESS ALU;

end VHDLBALU_behavior;

configuration CFG_VHDLBALU_BEHAVIORAL of VHDLBALU is
   for VHDLBALU_behavior
   end for;
end CFG_VHDLBALU_BEHAVIORAL;
