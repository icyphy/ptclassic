defstar {
  name { IdentityFix_M }
  domain { SDF }
  desc { Output a fix-point identity matrix. }
  version { $Id$ }
  author { Mike J. Chen }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
    name { rowsCols }
    type { int }
    default { 2 }
    desc { Number of rows and columns of the output square matrix. }
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

    FixMatrix& result = *(new FixMatrix(int(rowsCols),int(rowsCols),
                                        length, intbits));
    result.identity();
    output%0 << result;
  }
}
