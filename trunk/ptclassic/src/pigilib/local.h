/* local.h  edg
Version identification:
$Id$
This is a local include file for useful macros and definitions.
*/

/* boolean data type */
typedef int boolean;
#define TRUE	(1)
#define FALSE	(0)

#define EOS '\0' /* end of string */

extern char *calloc();
/*
extern void free();
*/
extern char *sprintf();

/* buffer length constants */
#define MSG_BUF_MAX 512 /* for messages */

#ifndef FILENAME_MAX
#define FILENAME_MAX 512 /* for filename paths */
#endif
