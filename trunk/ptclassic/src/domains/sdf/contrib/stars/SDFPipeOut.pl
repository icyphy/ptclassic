defstar {
  name { PipeOut }
  domain { SDF } 
  desc { Pipe outputs to shell command }
  author { N. Becker }
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
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
   out = popen( (char *)(command), "w" );
   x = (float)Xinit;
   if( !out )
     Error::abortRun( *this, "Error opening pipe to command", (char *)(command) );
  }
  wrapup {
    pclose( out );
  }
  go {
    int count = 0;		// count how many fields have been output
    if( (int)XY ) {
      fprintf( out, "%g", x );
      x += (float)Xinc;
      count++;
    }
    MPHIter nextp( in );
    PortHole* p;
    while( (p = nextp++) ) {
      if( count++ == 0 )
	fprintf( out, "%g", (float)((*p)%0) );
      else
	fprintf( out, "\t%g", (float)((*p)%0) );
    }
    fputs( "\n", out );
  }
}

