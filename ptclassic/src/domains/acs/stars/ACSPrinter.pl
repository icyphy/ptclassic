defcorona {
    name { Printer }
    domain { ACS }
    desc {
Print out one sample from each input port per line.  The "fileName"
state specifies the file to be written; the special names
&lt;stdout&gt; and &lt;cout&gt;, which specify the standard output
stream, and &lt;stderr&gt; and &lt;cerr&gt;, which specify the
standard error stream, are also supported.
    }
    version { @(#)ACSPrinter.pl	1.6 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    inmulti {
        name { input }
        type { ANYTYPE }
    }
 
}
