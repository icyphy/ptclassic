defstar {
  name      { MatrixFix }
  domain    { SDF }
  desc      { 
Produces a matrix with fixed-point entries. The entries are
read from the array state FixMatrixContents in rasterized order:
i.e. for a MxN matrix, the first row is filled from left to right
using the first N values from the array. All entries have the
same precision, as specifid by the state Precision. }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
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
  go {
    // parse precision value
    precision = FixPrecision;
    length = Fix::get_length(precision);
    intbits = Fix::get_intBits(precision);

    // collect inputs and put into the matrix

    FixMatrix *matrix = new FixMatrix(int(numRows), int(numCols),
                                      length, intBits, FixMatrixContents);
    output%0 << *matrix;
  }
}

