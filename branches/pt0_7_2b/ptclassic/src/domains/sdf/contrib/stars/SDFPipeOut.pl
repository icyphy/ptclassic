defstar {
  name { PipeOut }
  domain { SDF } 
  desc { Pipe outputs to shell command }
  version { @(#)SDFPipeOut.pl	1.6	02/04/99 }
  author { N. Becker }
  copyright {
Copyright (c) 1990-1999 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF user contribution library }
  inmulti {
    name { in }
    type { float }
  }
  state {
    name { XY }
    type { int }
    default { NO }
    desc { 
If YES output x, tab, y, return.
If no output y, return.}
  }
  state {
    name { command }
    type { string }
    default { "" }
    desc { the command to run }
  }
  state {
    name { Xinc }
    type { float }
    default { 1 }
    desc { increment to use for X when XY == YES }
  }
  state {
    name { Xinit }
    type { float }
    default { 0 }
    desc { initial value for X when XY == YES }
  }
  hinclude { <stdio.h> }
  private {
    float x;
    FILE *out;
  }
  setup {
    const char *commandstr = command;
#ifdef PT_NT4VC
    out = _popen( commandstr, "w" );
#else
    out = popen( commandstr, "w" );
#endif
    x = Xinit;
    if( !out ) {
      Error::abortRun( *this, "Error opening pipe to command",
		       commandstr );
    }
  }
  go {
    int count = 0;		// count how many fields have been output
    if( int(XY) ) {
      fprintf( out, "%g", x );
      x += Xinc;
      count++;
    }
    MPHIter nextp( in );
    PortHole* p;
    double value;
    while( (p = nextp++) ) {
      value = ((*p)%0);
      if( ! count )
	fprintf( out, "%g", value );
      else
	fprintf( out, "\t%g", value );
      count++;
    }
    fputs( "\n", out );
  }
  wrapup {
#ifdef PT_NT4VC
    _pclose( out );
#else
    pclose( out );
#endif
  }
}
