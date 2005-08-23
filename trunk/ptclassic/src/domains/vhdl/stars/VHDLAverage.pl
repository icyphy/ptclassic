defstar {
  name { Average }
  domain { VHDL }
  desc {
    Average some number of input samples or blocks of input samples.
      Blocks of successive input samples are treated as vectors.
  }
  version { @(#)VHDLAverage.pl	1.3 03/07/96 }
  author { Michael C. Williamson, E. A. Lee }
  copyright {
    Copyright (c) 1990-1997 The Regents of the University of California.
      All rights reserved.
      See the file $PTOLEMY/copyright for copyright notice,
      limitation of liability, and disclaimer of warranty provisions.
    }
  location { VHDL main library }
  input {
    name { input }
    type { float }
  }
  output {
    name { output }
    type { float }
  }
  defstate {
    name { numInputsToAverage }
    type { int }
    default { 8 }
    desc { The number of input samples or blocks to average. }
  }
  defstate {
    name { blockSize }
    type { int }
    default { 1 }
    desc {
      Input blocks of this size will be averaged to produce an output block.
    }
  }
  setup {
    input.setSDFParams(int(blockSize)*int(numInputsToAverage),
		       int(blockSize)*int(numInputsToAverage)-1);
    output.setSDFParams(int(blockSize), int(blockSize)-1);
  }
  go {
    StringList out;
    for(int i=int(blockSize)-1; i >= 0; i-- ) {
      out << "$temp(avg,float) := 0.0;\n";
      for(int j=int(numInputsToAverage)-1; j >= 0; j-- ) {
	out << "$temp(avg,float) := $temp(avg,float) + ";
	out << "$ref(input, ";
	out << -j*int(blockSize)+i;
	out << ");\n";
      }
      out << "$ref(output, ";
      out << -i;
      out << ") $assign(output) $temp(avg,float) / ";
      // Cast to float indirectly to satisfy VHDL compiler.
      int numInt = int(numInputsToAverage);
      float numFloat = numInt;
      out << numFloat;
      out << ";\n";
    }

    addCode(out);
    out.initialize();
  }
}
