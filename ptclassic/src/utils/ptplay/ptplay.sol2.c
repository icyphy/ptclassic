/*******************************************************************
SCCS version identification
$Id$

Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/

/* ptplay for Suns running Solaris2 */

#include "compat.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifdef PTSUN4
#include <sun/audioio.h>
#else
#include <sys/audioio.h>
#endif

int main (int argc, char *argv[])
{
  FILE *FIn;			/* Input File */
  int fDevAudio, fDevAudioctl;	/* /dev/audio and /dev/audioctl */
  audio_info_t info;
  char dataBuf[BUFSIZ];
  int count;

  if (argc == 1 ) {
    FIn = stdin;
  } else if (argc == 2 ) {
    if ( (FIn = fopen(argv[1], "r")) == NULL) {
      char buf[1024];
      sprintf(buf, "Can't open %s for reading:", argv[1]);
      perror(buf);
      return(1);
    }
  } else {
    fprintf(stderr,"%s: Usage ptplay [file]\n", argv[0]);
    return(1);
  }

  if ((fDevAudio = open("/dev/audio",O_WRONLY|O_CREAT,0666)) == -1) {
    perror("/dev/audio");
    return(1);
  }
  /* Open control device. */
  if ((fDevAudioctl = open("/dev/audioctl",O_RDWR,0777)) == -1) {
    perror("/dev/audioctl");
    return (1);
  }

  while( (count = fread(dataBuf, sizeof(char), BUFSIZ, FIn)) != 0) {
    ioctl(fDevAudio, AUDIO_GETINFO, &info);
    /* Wait for some samples to drain */
    while ((info.play.samples) > 2000)
      ioctl(fDevAudio, AUDIO_GETINFO, &info);

    /* Write data to file. */
    if (write(fDevAudio, dataBuf, count) != count) {
      perror("/dev/audio");
      return(1);
    }
  }

  if (close(fDevAudio) != 0) {
    perror("/dev/audio");
    return(1);
  }

  /* Close control device. */
  if (close(fDevAudioctl) != 0) {
    perror("/dev/audioctl");
    return(1);
  }

  return 0;
}

