static const char file_id[] = "bin2ascii.c";
/**************************************************************************
Version identification:
$Id$

Copyright (c) %Q% The Regents of the University of California.
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

 Programmer: Christopher Hylands
 Date of creation: 4/25/96
*/

#include <stdio.h>
#define MAXBUFSIZE 1024

int ReadBinaryData(stream, filename)
FILE *stream;
char *filename;
/*
 * Reads in the data sets in binary form from the supplied stream.
 * If the format
 * is correct,  it returns the current maximum number of points across
 * all data sets.  If there is an error,  it returns -1.
 */
{
    char buffer[MAXBUFSIZE];
    float pointPair[2];
    int line_count = 0;
    int errors = 0;
    int cmd;
    
    /*
    if (!rdSet(filename)) {
	(void) fprintf(stderr, "Error in file `%s' at line %d:\n  %s\n",
		       filename, line_count,
		       "Too many data sets - extra data ignored");
	return -1;
    }
    */
    while (!errors && (cmd = getc(stream)) != EOF) {
	line_count++;
	switch (cmd) {
	case 'e':		/* end of set, start new one */
	  /*
	    if (!rdSet(filename)) {
		(void) fprintf(stderr, "Error in file `%s' at line %d:\n  %s\n",
			       filename, line_count,
			       "Too many data sets - extra data ignored");
		return -1;
	    }
	    */
	    break;
	case 'n':		/* new set name: ends in \n */
	    fgets (buffer, MAXBUFSIZE, stream);
	    /*rdSetName(buffer);*/
	    break;
	case 'm':		/* move to point */
	    /*rdGroup();*/
	    getc(stream);	/* skip next character */
	    /* the actual text is "md x y" */
	    /* fall through */
	case 'd':		/* draw point */
	    fread((char*)pointPair,sizeof (float), 2, stream);
	    printf("%f %f\n", pointPair[0],pointPair[1]);
	    /*rdPoint(pointPair[0],pointPair[1]);*/
	    break;
	default:
	    if (filename) {
		(void) fprintf(stderr, "Error in file `%s' at line %d: '%c'\n",
			       filename, line_count, cmd);
		errors++;
	    }
	    break;
	}
    }
    if (errors) return 0; else return 1;
}


main(int argc, char **argv)
{
  FILE *fd;
  int i;
  for (i=1;i<argc;i++) {
    if ( (fd = fopen(argv[i],"r")) == NULL) {
      fprintf(stderr, "%s: Can't open %s for reading:", argv[0], argv[i]);
      perror(" ");
    }
    ReadBinaryData(fd,argv[i]);
  }
}
