defstar {
  name { SerialIn }
  domain { SR }
  desc {
In each instant, a character is read from the serial port.
}
  version { $Id$ }
  author { S. A. Edwards }
  copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}

  htmldoc { In each instant, this reads the given serial port.  If a
character is present, its value is sent to the output.  If no
character is ready, the output is made absent.  }

  output {
    name { output }
    type { int }
  }

  defstate {
    name { deviceName }
    type { string }
    default { "/dev/ttya" }
    desc { The name of the serial device }
  }

  defstate {
    name { baudRate }
    type { int }
    default { "38400" }
    desc { The baud rate }
  }

  ccinclude { <stdio.h>, <fcntl.h>, <termios.h>, <errno.h> }
  hinclude { <termios.h> }

  protected {
    int fileDescriptor;
    struct termios termSettings;    
  }

  constructor {
    fileDescriptor = -1;
  }

  setup {
    char rate[20];

    if ( fileDescriptor != -1 ) {
      close( fileDescriptor );
    }

    // Open the port as read only with non-blocking I/O
    fileDescriptor = open( deviceName, O_RDONLY | O_NONBLOCK, 0 );

    if ( fileDescriptor == -1 ) {
      Error::abortRun( *this, "Can't open device ", deviceName );
    }

    if ( tcgetattr( fileDescriptor, &termSettings ) == -1 ) {
      Error::abortRun( *this, "Can't get terminal attributes.  Is ",
		       deviceName, " a terminal?");
    }

    termSettings.c_iflag = 0;	/* No input processing */
    termSettings.c_oflag = 0;	/* No output processing */
    termSettings.c_cflag = B19200 | CS8 | CREAD;	/* 19200 baud, 8 bits, read */
    termSettings.c_lflag = 0;  	/* Raw mode */
    termSettings.c_cc[VMIN] = 0;	/* No minimum number of characters */
    termSettings.c_cc[VTIME] = 0;	/* No delay before waiting */

    speed_t bbits = B38400;
    switch ( int(baudRate) ) {
    case 110: bbits = B110; break;
    case 300: bbits = B300; break;
    case 1200: bbits = B1200; break;
    case 2400: bbits = B2400; break;
    case 4800: bbits = B4800; break;
    case 9600: bbits = B9600; break;
    case 19200: bbits = B19200; break;
    case 38400: bbits = B38400; break;
    default:
      sprintf(rate,"%d",int(baudRate));
      Error::abortRun( *this, "Unknown baud rate", rate );
    }

    if ( cfsetispeed( &termSettings, bbits ) == -1 ) {
      Error::abortRun( *this, "Unable to set input baud rate" );      
    }

    if ( cfsetospeed( &termSettings, bbits ) == -1 ) {
      Error::abortRun( *this, "Unable to set output baud rate" );      
    }

    if ( tcsetattr( fileDescriptor, TCSANOW, &termSettings ) == -1 ) {
      Error::abortRun( *this, "Unable to set terminal attributes" );
    }

  }

  go {
    char c;
    if ( !output.known() && fileDescriptor != -1 ) {
      // Try to read one character
      int n = read( fileDescriptor, &c, 1 );
      if ( n == 1 ) {
	output.emit() << int(c);
      } else {
	if ( errno == EAGAIN ) {
	  output.makeAbsent();
	} else {
	  char errnum[20];
	  sprintf( errnum, "%d", errno );
	  Error::abortRun( *this, "Unable to read the port, error ", errnum );
	}
      }
    }
  }

  wrapup {
    if ( fileDescriptor != -1 ) {
      close ( fileDescriptor );
      fileDescriptor = -1;
    }
  }

}
