defstar {
  name { Fork }
  domain { MDSDF }
  desc { Copies input particles to each output. }
  version { $Id$ }
  author { Mike J. Chen }
  copyright {
1993  The Regents of the University of California.  All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { MDSDF main library }
  explanation {
This star is generally used to connect a single output port
to multiple input ports.  It will be automatically inserted
when multiple inputs are connected to the same output using the graphical
interface, or when the "nodeconnect" command is used in the interpreter.
However, there are times when automatically inserted Fork stars
are not desirable.  For instance, when there is a delay on one
of the arcs, then the Fork must be inserted by the user explicitly
to avoid ambiguity about the location of the delay.
Also, when multi-portHoles are used, auto-forking can cause problems.
In this situation, one may get, for example, two outputs and several
inputs on the same net.  There is currently no way to automatically 
decipher what the user intends.  Hence, the Fork star should be inserted
explicitly.
  }
  input {
    name{input}
    type{ANYTYPE}
  }
  outmulti {
    name{output}
    type{= input}
  }
  setup {
    // set the dimensions of the porthole
    input.setMDSDFParams(ANYSIZE, ANYSIZE);
    output.setMDSDFParams(ANYSIZE, ANYSIZE);
  }
  go {
    MPHIter nextp(output);
    MDSDFPortHole* p;
    PtMatrix* inputData = (PtMatrix*)(input.getInput());
    PtMatrix* outputData;

    while ((p = (MDSDFPortHole*)nextp++) != 0) {
      outputData = p->getOutput();
      *outputData = *inputData;
      delete outputData;
    }
    delete inputData;
  }
}
