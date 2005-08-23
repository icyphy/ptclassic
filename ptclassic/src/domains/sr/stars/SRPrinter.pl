defstar {
  name { Printer }
  domain { SR }

  desc {
In each instant, print the value of the input to the filename in the
``fileName'' state.  The special names &lt;stdout&gt; &lt;cout&gt;
&lt;stderr&gt; &lt;cerr&gt; may also be used.
  }

  version { @(#)SRPrinter.pl	1.9 02/14/97 }
  author { S. A. Edwards }

  copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }

	htmldoc {
This star prints its input, which may be any supported data type.
<p>
If output is directed to a file, then flushing does not occur until the
wrapup method is called.
Before the first data are flushed, the file will not exist.
}
  inmulti {
    name { input }
    type { ANYTYPE }
  }

  defstate {
    name { fileName }
    type { string }
    default { "<stdout>" }
    desc { Output filename }
  }

  defstate {
      name { prefix }
      type { string }
      default { "" }
      desc { Prefix to print before each line of output }
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
    LOG_NEW; p_out = new pt_ofstream(fileName);
    reactive();
  }

  tick {
    pt_ofstream & output = *p_out;

    // Print the prefix

    output << (const char *)prefix;

    // Print the value of each output

    MPHIter nexti(input);
    InSRPort * p;   
    while ( (p = (InSRPort *)(nexti++)) != NULL ) {
      if ( !(p->known()) ) {
	output << "unknown ";
      } else {
	if ( p->present() ) {
	  output << p->get().print() << ' ';
	} else {
	  output << "absent ";
	}	    
      }
    }
    output << '\n';
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
