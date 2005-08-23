defstar {
  name { FIR }
  domain { MDSDF }
  desc {
Two-dimensional finite impulse response (FIR) filter.  Taps should be
specified by the following five parameters:
1) firstRowIndex: index of the first row, negative for past rows;
2) lastRowIndex: index of the last row, 0 for the current, positive for
future rows;
3) firstColIndex: the index of the first column, negative for past columns;
4) lastColIndex: the index of the last column, 0 for the current, positive for
future columns; and
5) taps: an array of the tap values themselves, starting with the earliest
tap specified by (firstRowIndex,firstColIndex) and ending with the last tap
spcified by (lastRowIndex, lastColIndex).

The default parameters specify a normalized 3 x 3 lowpass filter.
  }
  version { @(#)MDSDFFIR.pl	1.4 12/1/95 }
  author { Mike J. Chen }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { MDSDF library }
  input {
    name { input }
    type { FLOAT_MATRIX }
  }
  output {
    name { output }
    type { FLOAT_MATRIX }
  }
  defstate {
    name { firstRowIndex }
    type { int }
    default { "-1" }
    desc { The index of the first row of tap values }
  }
  defstate {
    name { lastRowIndex }
    type { int }
    default { 1 }
    desc { The index of the last row of tap values }
  }
  defstate {
    name { firstColIndex }
    type { int }
    default { "-1" }
    desc { The index of the first column of tap values }
  }
  defstate {
    name { lastColIndex }
    type { int }
    default { 1 }
    desc { The index of the last column of tap values }
  }
  defstate {
    name { taps }
    type { floatarray }
    default { ".1 .1 .1 .1 .2 .1 .1 .1 .1" }
    desc { The taps of the 2-D FIR filter. }
  }
  ccinclude { "SubMatrix.h" }
  setup {
    input.setMDSDFParams(1,1);
    output.setMDSDFParams(1,1);
  }
  go {
    // get a SubMatrix from the buffer
    double& out = output.getFloatOutput();

    out = 0.0;
    int tap = 0;

    int lastrow = int(lastRowIndex);
    int lastcol = int(lastColIndex);

    for(int row = int(firstRowIndex); row <= lastrow; row++) {
      for(int col = int(firstColIndex); col <= lastcol; col++) {
	 out += input.getFloatInput(row,col) * taps[tap++];
      }
    }
  }
}
