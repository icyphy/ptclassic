defstar {
  name { Quantizer }
  domain { SDF }
  desc {
This star quantizes the input value to the nearest output value
in the given codebook.  The nearest value is found by a full search
of the codebook, so the star will be significantly slower than
either Quant or LinQuantIdx.  The absolute value of the difference
is used as a distance measure.
  }
  version { $Id$ }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF main library }
  input {
    name {input}
    type {float}
  }
  output {
    name { output }
    type { float }
    desc { Closest value in the codebook }
  }
  output {
    name { outIndex }
    type { int }
    desc { Index of the closest value in the codebook }
  }
  defstate {
    name { floatCodebook }
    type { floatarray }
    default { "0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0" }
    desc { Possible output values }
  }
  go {
    double in = double(input%0);
    int size = floatCodebook.size();

    int index = 0;
    double distance = fabs(in-floatCodebook[0]);

    double sum = 0;
    for (int i=1; i<size; i++) {
      sum = fabs(in-floatCodebook[i]);
      if (sum < distance) {
	index = i;
	distance =sum;
      }
    } 

    output%0 << floatCodebook[index];
    outIndex%0 << index;
  } // end of go method
}
