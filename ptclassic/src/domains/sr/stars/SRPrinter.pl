defstar {
  name { Printer }
  domain { SR }

  desc {
In each instant, print the value of the input to the filename in the
``fileName'' state.  The special names <stdout> <cout> <stderr> <cerr> may
also be used.
  }

  version {@(#)SRPrinter.pl }
  author { S. A. Edwards }

  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }

  explanation {
This star prints its input, which may be any supported data type.
.pp
If output is directed to a file, then flushing does not occur until the
wrapup method is called.
Before the first data are flushed, the file will not exist.
}
  in {
    name { input }
    type { ANYTYPE }
  }

  defstate {
    name { fileName }
    type { string }
    default { "<stdout>" }
    desc { Output filename }
  }

  hinclude { "pt_fstream.h" }

  protected {
     pt_ofstream * p_out;
  }

  constructor {
     p_out = 0;
  }

  setup {
    // In case the file was not closed earlier by wrapup()..
    LOG_DEL; delete p_out;
    LOG_NEW; p_out = new ptofstream(fileName);
  }

  go {
    pt_ofstream & output = *p_out;
    output << input.get().print() << "\n";
  }

  wrapup {
    // flush the output
    LOG_DEL; delete p_out;
    p_out = 0;
  }

  destructor {
    // flush the output
    LOG_DEL; delete p_out;
  }

}
