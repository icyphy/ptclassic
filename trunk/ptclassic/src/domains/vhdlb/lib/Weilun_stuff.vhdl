USE work.Datatypes.all;
USE work.std_logic_1164.all;

-------------------------------------------------------------------------------
--  Highest order bit comes in last (has highest bit position)
--  Note that in integer vectors, all nonzero values are considered a logical
--  high, while a zero value is considered a logical low.
-------------------------------------------------------------------------------
PACKAGE Weilun_stuff IS

  -- Convert an INTEGER_VECTOR to a std_logic_vector.
  FUNCTION Intvec_2_stdvec( Input : INTEGER_VECTOR ) RETURN std_logic_vector;

  -- Convert a std_logic_vector to an INTEGER_VECTOR
  FUNCTION Stdvec_2_intvec( Input : std_logic_vector ) RETURN INTEGER_VECTOR;
  
END Weilun_stuff;

PACKAGE BODY Weilun_stuff IS

-------------------------------------------------------------------------------
  FUNCTION Intvec_2_stdvec (Input : INTEGER_VECTOR) RETURN std_logic_vector IS

    VARIABLE result : std_logic_vector((Input'LENGTH-1) downto 0);
    
  BEGIN
    -- Does the conversion
    FOR i IN (Input'LENGTH-1) downto 0 LOOP
      IF (Input(i) /= 0) THEN
        result(i) = '1';
      ELSE
        result(i) = '0';
      END IF;
    END LOOP;

    RETURN (result);	-- Return the results
  END Intvec_2_stdvec;

-------------------------------------------------------------------------------
  FUNCTION Stdvec_2_intvec (Input : std_logic_vector) RETURN INTEGER_VECTOR IS

    VARIABLE result : INTEGER_VECTOR(0 to (Input'LENGTH-1));
    
  BEGIN
    -- Does the conversion
    FOR i IN (Input'LENGTH-1) downto 0 LOOP
      IF (Input(i) = '1') THEN
        result(i) = 1;
      ELSE
        result(i) = 0;
      END IF;
    END LOOP;

    RETURN (result);	-- Return the result.
  END Stdvec_2_intvec;

END Weilun_stuff;
