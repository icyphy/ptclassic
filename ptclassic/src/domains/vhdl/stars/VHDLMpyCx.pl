defstar {
  name { MpyCx }
  domain { VHDL }
  desc { Output the product of the inputs, as a floating value.  }
  version { @(#)VHDLMpyCx.pl	1.3 03/07/96 }
  author { Michael C. Williamson }
  copyright {
    Copyright (c) 1990-1997 The Regents of the University of California.
      All rights reserved.
      See the file $PTOLEMY/copyright for copyright notice,
      limitation of liability, and disclaimer of warranty provisions.
    }
  location { VHDL main library }
  inmulti {
    name { input }
    type { complex }
  }
  output {
    name { output }
    type { complex }
  }
  constructor {
    noInternalState();
  }
  go {
    StringList out;

    if (input.numberPorts() == 1) {
      out << "$refCx(output,real) $assign(output) ";
      out << "$refCx(input#1,real)";
      out << ";\n";

      out << "$refCx(output,imag) $assign(output) ";
      out << "$refCx(input#1,imag)";
      out << ";\n";
    }
    else {
      StringList reA, imA, reB, imB;
      StringList rere, imim, reim, imre;
      StringList rediff, imsum;
      reA = "reA";
      imA = "imA";
      reB = "reB";
      imB = "imB";
      rere = "rere";
      imim = "imim";
      reim = "reim";
      imre = "imre";
      rediff = "rediff";
      imsum = "imsum";

      out << "$temp(" << rediff;
      out << input.numberPorts();
      out << ",float) := ";
      out << "$refCx(input#";
      out << input.numberPorts();
      out << ",real)";
      out << ";\n";

      out << "$temp(" << imsum;
      out << input.numberPorts();
      out << ",float) := ";
      out << "$refCx(input#";
      out << input.numberPorts();
      out << ",imag)";
      out << ";\n";

      for (int i = input.numberPorts() - 2 ; i >= 0 ; i--) {
	// handle case: if one input, vs if more than one input

	out << "$temp(" << reA;
	out << i+1;
	out << ",float) := ";
	out << "$temp(" << rediff;
	out << i+2;
	out << ",float)";
	out << ";\n";

	out << "$temp(" << imA;
	out << i+1;
	out << ",float) := ";
	out << "$temp(" << imsum;
	out << i+2;
	out << ",float)";
	out << ";\n";

	out << "$temp(" << reB;
	out << i+1;
	out << ",float) := ";
	out << "$refCx(input#";
	out << i+1;
	out << ",real)";
	out << ";\n";

	out << "$temp(" << imB;
	out << i+1;
	out << ",float) := ";
	out << "$refCx(input#";
	out << i+1;
	out << ",imag)";
	out << ";\n";
	
	out << "$temp(" << rere;
	out << i+1;
	out << ",float) := ";
	out << "$temp(" << reA;
	out << i+1;
	out << ",float)";
	out << " * ";
	out << "$temp(" << reB;
	out << i+1;
	out << ",float)";
	out << ";\n";

	out << "$temp(" << imim;
	out << i+1;
	out << ",float) := ";
	out << "$temp(" << imA;
	out << i+1;
	out << ",float)";
	out << " * ";
	out << "$temp(" << imB;
	out << i+1;
	out << ",float)";
	out << ";\n";

	out << "$temp(" << reim;
	out << i+1;
	out << ",float) := ";
	out << "$temp(" << reA;
	out << i+1;
	out << ",float)";
	out << " * ";
	out << "$temp(" << imB;
	out << i+1;
	out << ",float)";
	out << ";\n";

	out << "$temp(" << imre;
	out << i+1;
	out << ",float) := ";
	out << "$temp(" << imA;
	out << i+1;
	out << ",float)";
	out << " * ";
	out << "$temp(" << reB;
	out << i+1;
	out << ",float)";
	out << ";\n";

	out << "$temp(" << rediff;
	out << i+1;
	out << ",float) := ";
	out << "$temp(" << rere;
	out << i+1;
	out << ",float)";
	out << " - ";
	out << "$temp(" << imim;
	out << i+1;
	out << ",float)";
	out << ";\n";

	out << "$temp(" << imsum;
	out << i+1;
	out << ",float) := ";
	out << "$temp(" << reim;
	out << i+1;
	out << ",float)";
	out << " + ";
	out << "$temp(" << imre;
	out << i+1;
	out << ",float)";
	out << ";\n";
	
      }
      out << "$refCx(output,real) $assign(output) ";
      out << "$temp(" << rediff;
      out << 1;
      out << ",float)";
      out << ";\n";

      out << "$refCx(output,imag) $assign(output) ";
      out << "$temp(" << imsum;
      out << 1;
      out << ",float)";
      out << ";\n";
    }
    
    addCode(out);
  }
}
