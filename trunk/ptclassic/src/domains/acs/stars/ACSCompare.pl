defcorona {
    name { Compare }
    domain { ACS }
    desc { Output 1 (True) if left input is less than right input }
    version { @(#)ACSCompare.pl	1.4 09/08/99}
    author { Eric Pauer }
    copyright {
Copyright (c) 1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    input {
      name { left }
      type { float }
      desc { Left-hand side of the test }
    }
    input {
      name { right }
      type { float }
      desc { Right-hand side of the test }
    }
    output {
      name { result }
      type { int }
      desc { 1 when left > right; otherwise 0 }
    }
}
