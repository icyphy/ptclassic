/*
 * Technology data structures
 *
 * David Harrison,  August 1985
 * UC Berkeley
 *
 * The technology is specified in the form given in version 1.0.0 of the
 * Electronic Design Interchange Format document (p. 114).  The only
 * sections this parser is interested in are: 'define', 'figuregroupdefault',
 * and 'userdata'.  See tech.c for more specific parsing information.
 *
 * Requires the following header files:
 *	general.h
 *	fillPat.h
 *	list.h
 */

#ifndef TECH
#define TECH

#include "general.h"
#include "fill_pat.h"
#include "scan.h"
#include "list.h"

/****************************************************************************\
*			Technology Description				     *
\****************************************************************************/

typedef unsigned int Options;

typedef int vemAttrs;

typedef struct _vemTech {	/* Technology description	     */
    STR techName;		/* Name of technology inside file    */
    STR techFile;		/* Technology file                   */
    lsList layerList;		/* Priority ordered list of layers   */
} vemTech;


typedef struct _veTechLayer {	/* Layer description		     */
    STR layerName;		/* Name 			     */
    int priority;		/* Integer priority level            */
#ifdef ENDTYPES
    Options endType;		/* One of EXTEND, TRUNCATE, or ROUND */
    Options cornerType;		/* Same options as above 	     */
#endif
    int width;			/* Width of path (default oct coords)*/
    int red, green, blue;	/* Color intensities (0-1000) 	     */
    flPattern fillData;		/* Fill pattern                      */
    int borderLength;		/* Length of line style pattern	     */
    char  *borderData;		/* Line style pattern data	     */

    /* Attached device information */

    vemAttrs layerAttrs;	/* X specific layer attributes       */
} vemTechLayer;

/* endType and cornerType options: */

#define	EXTEND		1
#define TRUNCATE	2
#define	ROUND		3



/**************************************************************************\
*			   Technology Routines			   	   *
\**************************************************************************/


extern vemStatus loadTech();
  /* Loads a technology */

extern vemTech *getTech();
  /* Gets a technology */

/*
 * Note:  getTech is provided as a convenience.  For general purpose
 * access and modification of the layers of a technology,  it is assmed
 * the user will use the functions provided by the list package along
 * with the list handle given in the technology record.
 */

extern vemTechLayer *getLayer();
  /* Gets a layer description given a technology */

extern vemStatus saveTech();
  /* Saves a technology */


#endif
