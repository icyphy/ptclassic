defstar {
  name { Quantizer }
  domain { SDF }
  desc {
Scalar quantizer. It will quantize the input value to one of possible 
output levels in the given codebook, ie, it will find the neareast value
in the given codebook. Output levels in the codebook is not necessary
to be in specific order. This star will do the full search in the
codebook, and will be slower than Quant star as expected. 
  }
  version { $Id$ }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
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
    desc { Output the corresponding level in the codebook }
  }
  output {
    name { outIndex }
    type { int }
    desc { Output the corresponding index in the codebook }
  }
  defstate {
    name { floatCodebook }
    type { floatarray }
    default { "0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0" }
    desc { Output levels. }
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
