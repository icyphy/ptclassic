/* Version Identification:
 * $Id$
 */
/*
Copyright (c) 1990-%Q% The Regents of the University of California.
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
/* map between IO and stdio, some might use IO and WFS */

#if !defined(MCC_DMS)

#define IOFILE FILE

#define IOfputs fputs
#define IOgetc getc
#define IOungetc ungetc
#define IOputc putc
#define IOfprintf fprintf
#define IOfscanf fscanf

#define IOferror ferror

#define IOfopen fopen
#define IOfflush fflush
#define IOfclose fclose

#define IOfeof feof

/* pathname delimeters */
#define PND_CHAR	'/'
#define PND_STRING	"/"

#else /* MCC_DMS is defined */

#include "dms.h"

/* this is the wrong way to do this, but until the definitions are moved from */
/* dms_internal to dms.h it has to be done like this */
#define dmsDELIMITER            '/'
#define dmsDELIMITER_STRING     "/"

/* The character used to delimited path name components. */
#define PND_CHAR	dmsDELIMITER
#define PND_STRING	dmsDELIMITER_STRING

#define IOFILE dmsWFD

#define IOfputs wioWFputs
#define IOgetc wioGetc
#define IOungetc wioUngetc
#define IOputc wioPutc
#define IOfprintf wioWFprintf
#define IOfscanf wioWFscanf

#define IOferror wioWFerror

#define IOfopen wioWFopen
#define IOfflush wioWFflush
#define IOfclose wioWFclose

#define IOfeof wioWFeof

#endif /* defined(MCC_DMS) */
