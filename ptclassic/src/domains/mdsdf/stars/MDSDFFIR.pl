defstar {
  name { FIR }
  domain { MDSDF }
  desc {
2 Dimensional FIR filter.
Taps should be specified by five parameters:
 1) firstRowIndex: the index of the first row, negative for past rows
 2) lastRowIndex: the index of the last row, 0 for the current, positive for
                  future rows
 3) firstColIndex: the index of the first column, negative for past columns
 4) lastColIndex: the index of the last column, 0 for the current, positive for
                  future columns
 5) an array of the tap values themselves, starting with the earliest
    tap spcified by (firstRowIndex,firstColIndex) and ending with the
    last tap spcified by (lastRowIndex, lastColIndex)
The default parameters specifiy a 9 tap low pass filter.
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
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

    out = 0;
    int tap = 0;

    for(int row = int(firstRowIndex); row <= int(lastRowIndex); row++) {
      for(int col = int(firstColIndex); col <= int(lastColIndex); col++) {
	 out += input.getFloatInput(row,col) * taps[tap++];
      }
    }
  }
}

