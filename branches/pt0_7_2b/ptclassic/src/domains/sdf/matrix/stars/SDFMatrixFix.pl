defstar {
  name      { MatrixFix }
  domain    { SDF }
  desc      { 
Generate a matrix with fixed-point entries.
  }
	htmldoc {
The entries of the generated matrix are read from the array parameter
<i>FixMatrixContents</i> in rasterized order: i.e. for an <i>M</i> &#215; <i>N</i> matrix,
the first row is filled from left to right using the first <i>N</i> values
from the array.  All entries have the same precision, as specified by
the parameter <i>FixPrecision</i>.
  }
  version   { @(#)SDFMatrixFix.pl	1.11 02 Oct 1996 }
  author    { Mike J. Chen }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF matrix library }
  output {
	name { output }
	type { FIX_MATRIX_ENV }
  }
  defstate {
	name { numRows }
	type { int }
	default { 2 }
	desc { The number of rows in the matrix. }
  }
  defstate {
	name { numCols }
	type { int }
	default { 2 }
	desc { The number of columns in the matrix. }
  }
  defstate {
	name { FixMatrixContents }
	type { fixarray }
	default { "1 -2 2 -2" }
	desc { The Fix contents of the matrix. }
  }
  defstate {
        name { FixPrecision }
        type { string }
        default { "8.14" }
        desc {
Precision of all entries in the matrix.
        }
  }
  ccinclude { "Matrix.h" } 
  protected {
        const char* precision;
        int length, intbits;
  }
  go {
    // parse precision value
    precision = FixPrecision;
    length = Fix::get_length(precision);
    intbits = Fix::get_intBits(precision);

    // collect inputs and put into the matrix

    FixMatrix& matrix = *(new FixMatrix(int(numRows), int(numCols),
                                        length, intbits, FixMatrixContents));
    output%0 << matrix;
  }
}
