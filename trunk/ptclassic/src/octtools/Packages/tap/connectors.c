#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#include "copyright.h"
#include "port.h"
#include "oct.h"
#include "tap.h"
#include "st.h"
#include "vov.h"
#include "utility.h"

#define OCT_TO_LAMBDA 20
#define CONTACTS_DIR "special_contacts"

static int
  create_new_via(lay1, lay2, wid, height)
char* lay1;
char* lay2;
int wid;
int height;
{
    char command[1024];
    
    (void) sprintf(command, "mkdir %s >& /dev/null; cd %s; prim %s-%s %d %d;",
		   CONTACTS_DIR, CONTACTS_DIR, lay1, lay2,
		   wid, height);

    printf( "Executing: %s\n", command );
    if (util_csystem(command) != 0) {
	return 0;
    }
    return 1;
}

/*********************************************************************/


/* This is a time critical function, because it determines if a 
* facet exists by touching the file system, with either 
* octGetFacetInfo or octGetFacet
* The hash table has been added to limit the calls to octOpenFacet().
*/
static int
  checkViaExist(name)
char *name;
{
    octObject facet;
    octStatus status;
    static st_table* table = 0;	/* To save on octOpenFacets */
    char* value;

    if ( table == 0 ) {
	table = st_init_table( strcmp, st_strhash );
    }

    if ( st_lookup( table, name, &value ) ) {
	return 1;		/* A hit, we saved an octOpenFacet(). */
    }

    facet.type = OCT_FACET;
    facet.contents.facet.cell = name;
    facet.contents.facet.view = "physical";
    facet.contents.facet.facet = "contents";
    facet.contents.facet.version = OCT_CURRENT_VERSION;
    facet.contents.facet.mode = "r";
    status = octOpenFacet(&facet); /* Expensive, always touches the file system. */


    if (status < OCT_OK) {
	return 0;
    }

    VOVinputFacet( &facet );

    st_add_direct( table, util_strsav(name), 0 ); /* Next time we'll have a hit. */
    return 1;
}

/****************************************************************/

static void
  make_via_name(buffer, lay1, lay2, wid, height)
char *buffer;
char* lay1;
char* lay2;
int wid;
int height;
{
  (void) sprintf(buffer,"%s-%s.%dx%d", lay1, lay2, wid, height);
}

/****************************************************************/

static char
  *getOrCreateVia(tech_dir, h_wid, v_wid, lay1, lay2, trans)
char* tech_dir;
int h_wid;
int v_wid;
octObject* lay1;
octObject* lay2;
octTransformType *trans;
{
    char* name;
    char buf[1024];
    char via_name[1024];
    char* l1name = lay1->contents.layer.name;
    char* l2name = lay2->contents.layer.name;


    if ( strcmp( l1name , l2name ) <= 0 ) {
	char* t=l1name; l1name=l2name; l2name=t; /* Swap layer names */
    }


    if (h_wid == 0) {
	h_wid = tapGetMinWidth(lay1);
	h_wid = (h_wid > tapGetMinWidth(lay2))?h_wid:tapGetMinWidth(lay2);
    }
    if (v_wid == 0) {
	v_wid = tapGetMinWidth(lay1);
	v_wid = (v_wid > tapGetMinWidth(lay2))?v_wid:tapGetMinWidth(lay2);
    }
  
    /* make sure it is at least of min size */
    if(h_wid < 2 * OCT_TO_LAMBDA) h_wid = 2 * OCT_TO_LAMBDA;
    if(v_wid < 2 * OCT_TO_LAMBDA) v_wid = 2 * OCT_TO_LAMBDA;
  
    /* convert to lambda and round up */
  
    h_wid = (h_wid + OCT_TO_LAMBDA - 1) / OCT_TO_LAMBDA;
    v_wid = (v_wid + OCT_TO_LAMBDA - 1) / OCT_TO_LAMBDA;
  
    /* change sizes so that the horizintal one is always the largest */
  
    if(h_wid >= v_wid) {
	(*trans) = OCT_NO_TRANSFORM;
    } else {
	int tmp = v_wid; v_wid = h_wid; h_wid = tmp;
	(*trans) = OCT_ROT90;
    }
  
    make_via_name(via_name, l1name, l2name, h_wid, v_wid);

    /* (void) sprintf(buf,"%s/contacts/%s", tech_dir, via_name); */
    /* if(!checkViaExist(buf)) { */
  
    (void) sprintf(buf,"%s/%s", CONTACTS_DIR, via_name);
    if ( !checkViaExist(buf) ) {
      
	if (!create_new_via( l1name, l2name, h_wid, v_wid)) {
	    return 0;		/* Failed  */
	}
    }

    name = util_strsav(buf);	/* Save this name. */
    return name;		/* Via found. */
}

int
  tapGetStrangeConnector(nLayers, layerList, instance)
int nLayers;
tapLayerListElement* layerList;
octObject* instance;
{
  int horiz = 0, vert = 0, i;
  enum octTransformType trans;
  octObject* lay1 = 0;
  octObject* lay2 = 0;
  
  for(i = 0; i < nLayers;i++) {
    if((lay1 == 0) || 
       (strcmp(lay1->contents.layer.name,
	       layerList[i].layer.contents.layer.name) == 0))
      lay1 = &(layerList[i].layer);
    else
      if((lay2 == 0) || 
	 (strcmp(lay2->contents.layer.name,
		 layerList[i].layer.contents.layer.name) == 0))
	lay2 = &(layerList[i].layer);
      else {
	return 0;
      }
    
    if (layerList[i].direction & TAP_HORIZONTAL) {
      vert = (vert > layerList[i].width)?vert : layerList[i].width;
    }
    if (layerList[i].direction & TAP_VERTICAL) {
      horiz = (horiz > layerList[i].width)?horiz : layerList[i].width;
    }
    if(layerList[i].direction & TAP_WEIRD_DIR) {
      printf(" Can not have non-mahattan geometry\n");
      return 0;
    }
  }
  if(lay1 == 0) {
    return 0;
  }
  if(lay2 == 0)
    lay2 = lay1;
  
  instance->type = OCT_INSTANCE;
  instance->contents.instance.master = 
    getOrCreateVia(tapGetDirectory(&(layerList[0].layer)),horiz,vert,lay1,lay2,&trans);
  instance->contents.instance.view = "physical";
  instance->contents.instance.facet = "contents";
  instance->contents.instance.version = OCT_CURRENT_VERSION;
  instance->contents.instance.transform.transformType = trans;
  if (instance->contents.instance.master == 0) {
    return 0;
  }
  return 1;
}

/****************************************************************/



