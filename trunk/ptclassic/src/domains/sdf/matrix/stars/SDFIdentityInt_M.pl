defstar {
  name { IdentityInt_M }
  domain { SDF }
  desc { Output a integer identity matrix. }
  version { @(#)SDFIdentityInt_M.pl	1.6 10/6/95 }
  author { Mike J. Chen }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF matrix library }
  output {
    name { output }
    type { INT_MATRIX_ENV }
  }
  defstate {
    name { rowsCols }
    type { int }
    default { 2 }
    desc { Number of rows and columns of the output square matrix. }
  }
  ccinclude { "Matrix.h" }
  go {
    IntMatrix& result = *(new IntMatrix(int(rowsCols),int(rowsCols)));
    result.identity();
    output%0 << result;
  }
}
