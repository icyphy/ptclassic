defstar	{
  name { CSynchComm }
  domain { VHDL }
  desc { Base class for VHDL-CGC synchronous communication }
  version { $Id$ }
  author { Michael C. Williamson, Jose Luis Pino }
  copyright {
Copyright (c) 1993-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright	for copyright notice,
limitation of liability, and disclaimer	of warranty provisions.
}
  location { VHDL Target Directory }
  explanation { }
  public {
    int numXfer;
  }
  protected {
    friend class VHDLTarget;
  }
  hinclude { "VHDLTarget.h" }
  defstate {
    name {pairNumber}
    type {int}
    default {0}
  }
  defstate {
    name {rtype}
    type {string}
    default {"type"}
  }
  setup {
  }
}
