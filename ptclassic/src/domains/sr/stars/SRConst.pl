defstar {
  name { Const }
  domain { SR }
  desc {
In each instant, output a signal with value given by the ``level'' parameter.
  }
  version {@(#)SRConst.pl }
  author { S. A. Edwards }
  copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  output {
    name { output }
    type { int }
  }
  defstate {
    name { level }
    type { int }
    default { "0" }
    desc { The constant value }
  }
  go {
    output.emit() << int(level);
  }
}
