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

#include "port.h"
#include "utility.h"
#include "st.h"
#include "oct.h"
#include "oh.h"
#include "message.h"
#include "windows.h"
/*
  -------------------------------
  $Header$

  $Id$
 
  $Log$
 * Revision 3.0  90/04/03  16:55:10  greer
 * murphy undo stuff ( 4/3/90 )
 * 
 * Revision 1.4  90/03/16  14:54:21  murf
 * The undo updates, and an ID header for everyone
 * 
 * Revision 1.3  90/03/07  12:14:22  murf
 * Re-opportioned the funcs in undo into undo.c and init_change.c.
 * 
 * Revision 1.2  90/03/07  10:46:23  murf
 * undo modifications
 * 
  
  -------------------------------
  */

st_table *get_facet_changelistid_table_addr();

static drsLastWasUndo = 0;

void
drsLastCommandWasUndo()
/*
 * this sets up for a tight loop: any distraction will reset the generator.
 */
{
	drsLastWasUndo = 1;
}


void
drsResetLastCommand()
{
	drsLastWasUndo = 0;
}


int
drsWasLastCommandUndo()
{
	return drsLastWasUndo;
}


void
drsInsertUndoMarker(facetid)
octId facetid;
{
    octObject facet;
    octId clid;
    char *ptr;
	
    facet.objectId = facetid;
    if (octGetById(&facet) != OCT_OK) {
	return;
    }
    
    /* get the change list */
    if (!st_lookup(get_facet_changelistid_table_addr(), (char *) facetid, &ptr)) {
	/* no changelist found. therefore, we can't undo. go home */
	return;
    }
    clid = (octId) ptr;

    (void) oct_make_change_record_marker(clid);
    return;
}



void printproblem(fp,str,arg1,arg2,arg3,arg4,arg5,arg6)
FILE *fp;
char *str;
int arg1,arg2,arg3,arg4,arg5,arg6;
{
#ifdef DEBUGCL
    char buf[300];
    if ( fp ) {
	fprintf(fp,str,arg1,arg2,arg3,arg4,arg5,arg6);
	fprintf(fp,"\n");
	fflush(fp);
    }
#endif
}


enum st_retval printCLSize( key, value, arg )
    char* key;
    char* value;
    char* arg;
{
    octId     facetId = (octId)key;
    octId     clId = (octId)value;
    octObject facet, clist;
    int count;

    *arg += 1;

    if ( ohGetById( &facet, facetId ) != OCT_OK ) return ST_CONTINUE;
    if ( ohGetById( &clist, clId ) != OCT_OK ) return ST_CONTINUE;

    count = ohCountContents( &clist, OCT_CHANGE_RECORD_MASK );
    
    printf( "CL %ld: %d records\n", (long)clId, count );

    return ST_CONTINUE;
}

void drsMeasure()
{
    int clCount = 0;
    st_table* table = get_facet_changelistid_table_addr(); 

    printf( "Begin\n" );
    st_foreach( table, printCLSize, (char*)&clCount );
    printf( "End\n" );
}

void drsUndo(facetid)
octId facetid;
{
    octObject facet, cl, cr, obj;
    octId clid;
    static octGenerator *crg = 0;
    int start, retval = 0;
    char *ptr;
    int eventCount = 0;		/* Count events that are undone. */
    int eventLength = 0;	/* Length of change list. */
#ifdef DEBUGCL
    FILE *dump = (FILE *)NULL;
    octGenerator pcrg;
#else
    int dump = 0; /* so the printproblem stuff doesn't complain */
#endif
	
    /*
     * Is meant to undo the last set of operations that occurred.
     * if consecutive undo's are issued, then keep working down the list.
     * Keep going till the list ends or a marker is hit. Leave pointer.
     */

#ifdef DEBUGCL
    drsMeasure();		/* Print stats about size of Change lists. */
#endif
    /* get the change list */
    if (!st_lookup(get_facet_changelistid_table_addr(), (char *) facetid, &ptr)) {
	/* no changelist found. therefore, we can't undo. go home */
	return;
    }
    clid = (octId) ptr;
    cl.objectId = clid;
    if (octGetById(&cl) != OCT_OK) {
	/* 
	 * there's no changeList anymore, for some reason,
	 * and the best action is no action.
	 */
	return;
    }
    if (!drsLastWasUndo) {
	if (!crg) {
	    crg = ALLOC(octGenerator, 1);
	} else {
	    /* the last chain is broken, free it up */
	    octFreeGenerator(crg);
	}

	/* 
	 * we need a reverse generator -- one that starts at the
	 * end of a list and moves back to the beginning.
	 *
	 * the pattern: STOP when you hit a marker, and move the 
	 * generator past it
	 */

	/* 
	 * the following is kind of gory, but this is a home-brew 
	 * generator, and you have to supply your own state structure,
	 * which has to contain a ptr to a chain so you can traverse
	 * the contents of the changeList.
	 */
	octInitBackwardsGenContents(&cl,crg);
    }
#ifdef DEBUGCL
    octInitBackwardsGenContents(&cl,&pcrg);
    dump = fopen("cl.dump","w");
    if ( dump == NIL(FILE) ) {
	printf( "Cannot open cldump file." );
    } else {
	while (octGenerate(&pcrg,&cr) == OCT_OK) {
	    octPrintObject(dump,&cr,0);
	    fflush(dump);
	}
	fprintf(dump,"\nFINISHED\n");
    }
    octFreeGenerator(&pcrg);
#endif
    eventLength = ohCountContents( &cl, OCT_CHANGE_RECORD_MASK );
    facet.objectId = facetid;
    octGetById(&facet);
    start = 1;
    while (octGenerate(crg,&cr) == OCT_OK) {
	octBox bbox, bbox2;
	octObject container, objn;
	octPoint pt_array[10];
	int retval1, retval2;
	int32 xid, num_points;

	octExternalId(&cr,&xid);
	eventCount++;
	switch (cr.contents.changeRecord.changeType) {
	    case OCT_MARKER:
	    /* this is a null record, to split up groups of commands */
	    if (start) {
		break; /* ignore the mark if heads a list */
	    } else {
		vemMsg( MSG_C, "%d events undone (%d in queue)\n", eventCount, eventLength );
		return;
	    }
	    
	    case OCT_CREATE:
	    /*
	     * do a delete, but no detach?  If all is well,
	     * the detach is already done.
	     */
	    octGetByExternalId(&facet, cr.contents.changeRecord.objectExternalId, &obj);
	    retval = octBB(&obj,&bbox);
	    if (octUnCreate(&obj) != OCT_OK) {
		printproblem(dump,"Change Record [%d,x%d]: Could not delete object to undo Create Operation.",
			     cr.objectId,xid);
	    } else if (retval == OCT_OK) {
		wnQRegion(facetid,&bbox);
	    }
	    printproblem(dump,"Change Record [%d,x%d]: octUnCreate",
			 cr.objectId,xid);
	    break;
	    
	    case OCT_DELETE:
	    /* do a create */
	    octGenFirstContent(&cr, OCT_ALL_MASK, &obj); /* get it back */
	    /* octBB(&obj,&bbox);  why, because it's in there? */
	    /* 
	     * Actual terminals are a pain: if this object is one,
	     * then skip it, they are automatically dealt with
	     */
	    
	    /* 
	     * this routine won't attach it to anything
	     *
	     *  ARGH!!!!  XXXX!!!!  This requires use of an internal Oct function...
	     */
	    if ((retval = oct_do_undelete(&facet, &obj, cr.contents.changeRecord.objectExternalId, obj.objectId)) != OCT_OK )
	      {
		  errRaise("vem/undo", -1,
			   "Change Record [%d,x%d]: Could not create object (%d) to undo Delete Operation.",
			   cr.objectId, xid, retval);
	      } else {
		  int32 xid;
		  
		  octExternalId(&obj, &xid);
		  if (cr.contents.changeRecord.objectExternalId != xid) {
		      errRaise("vem/undo", -1,
			       "Re-created object has different external id [old=%d] [new=%d]",
			       cr.contents.changeRecord.objectExternalId, xid);
		  }
		  printproblem(dump, "Recreated object has id = %d", obj.objectId);
		  /* well, we have an object that isn't attached to anything. Is it good to try
		     region and bounding box stuff on it just now? */
		  if (octBB(&obj, &bbox) == OCT_OK) {
		      wnQRegion(facetid, &bbox);
		  }
		  printproblem(dump, "Change Record [%d,x%d]: octUnDelete",
			       cr.objectId, xid);
	      }
	    break;
	    
	    case OCT_MODIFY:
	    /* unmodify */
	    /* get the current version of the modified object */
	    octGetByExternalId(&facet, cr.contents.changeRecord.objectExternalId, &obj);
	    octGenFirstContent(&cr, OCT_ALL_MASK, &objn); /* get the old version back */
	    /* Hmmm, well, use the current version */
	    if (objn.objectId != obj.objectId) {
		objn.objectId = obj.objectId;
	    }
	    if ((retval2 = octBB(&obj, &bbox2)) != OCT_OK && retval != OCT_NO_BB )  /* bb before modified */
	      {
		  printproblem(dump, "Change Record [%d,x%d]: Container BB not OK (%d) in Modify in Undo operation.",
			       cr.objectId, xid, retval);
	      }
	    if ((retval=octUnModify(&objn)) == OCT_OK) {
		if (retval2 == OCT_OK) {
		    wnQRegion(facetid, &bbox2);
		}
		if((retval = octBB(&obj, &bbox)) != OCT_OK && retval != OCT_NO_BB ) /* bb of current object */
		  {
		      printproblem(dump,"Change Record [%d,x%d]: Object BB not OK (%d) in Modify in Undo operation.",
				   cr.objectId,xid,retval);
		  } else if (retval == OCT_OK) {
		      wnQRegion(facetid,&bbox);
		  }
		printproblem(dump,"Change Record [%d,x%d]: OctUnModify",
			     cr.objectId,xid);
	    } else {
		printproblem(dump,"Change Record [%d,x%d]: OctUnModify not OK (%d) in undo operation.",
			     cr.objectId,xid,retval);
	    }
	    break;
	    
	    case OCT_PUT_POINTS:
	    /* yank out points ? I'd have to replace with no points at all? No, I'd have to replace
	       with the old points list... Are they attached to the change record? */
	    octGetByExternalId(&facet, cr.contents.changeRecord.objectExternalId, &obj);
	    /* now, talking w. David Harrison, he indicated that we should be able to call octGetPoints
	       on the change record and get the goodies...
	       */
	    retval1 = octBB(&obj, &bbox2); /* the way the object is now */
	    
	    num_points = 10;
	    retval = octGetPoints(&cr, &num_points, pt_array);
	    if (retval == OCT_TOO_SMALL) {
		octPoint *x;
		x = (octPoint *) malloc((sizeof(struct octPoint))*num_points);
		retval = octGetPoints(&cr,&num_points,x);
		if( octPutPoints(&obj,num_points,x) != OCT_OK )
		  {
		      printproblem(dump,"Couldn't put %d points on object %d", num_points, cr.contents.changeRecord.objectExternalId);
		  }
		free(x);
	    } else if (retval == OCT_OK) {
		if(octPutPoints(&obj,num_points,pt_array) != OCT_OK )
		  {
		      printproblem(dump,"Couldn't put %d points on object %d", num_points, cr.contents.changeRecord.objectExternalId);
		  } else {
		      if( retval1 == OCT_OK) {  /* bb before modified */
			  wnQRegion(facetid,&bbox2);
		      }
		      if( octBB(&obj, &bbox) == OCT_OK ) { /* bb of current object */
			  wnQRegion(facetid,&bbox);
		      }
		  }
	    } else {
		printproblem(dump,"Change Record [%d,x%d]: octGetPoints not OK (%d) in undo operation.",
			     cr.objectId,xid,retval);
	    }
	    printproblem(dump,"Change Record [%d,x%d]: OctPutPoints",
			 cr.objectId,xid);
	    break;
	    
	    case OCT_ATTACH_CONTENT:
	    /* detach contents spec'd */
	    octGetByExternalId(&facet,
			       cr.contents.changeRecord.objectExternalId, &obj);
	    octGetByExternalId(&facet,
			       cr.contents.changeRecord.contentsExternalId,
			       &container);
	    /* 
	     * is this too simple minded?  Do I need to get into
	     * the guts of octDetach and do it one link at a time?
	     * -- Because octUnAttach does both.  No, because I
	     * only record one side of the events.
	     */
	    if ((retval=octUnattach(&obj,&container)) != OCT_OK) {
		/*
		 * unattach because facets wouldn't allow formals
		 * to be detached. Unattach doesn't care.
		 */
		printproblem(dump,"Change Record [%d,x%d]: Oct UnAttach Not OK (%d) in Undo operation of an Attach.",
			     cr.objectId,xid,retval);
	    } else {
		printproblem(dump,"Change Record [%d,x%d]: OctUnAttach",
			     cr.objectId,xid);
	    }
	    break;
	    
	    case OCT_DETACH_CONTENT:
	    /* attach to contents */
	    octGetByExternalId(&facet, 
			       cr.contents.changeRecord.objectExternalId, &obj);
	    octGetByExternalId(&facet,
			       cr.contents.changeRecord.contentsExternalId,
			       &container);
	    /* 
	     * is this too simple minded?  Do I need to get into 
	     * the guts of octAttach and do it one link at a time?
	     * -- Because octUnDetach does both.  No, because I
	     * only record one side of the events.
	     */
	    if ((retval=octUnDetach(&obj,&container)) != OCT_OK) {
		printproblem(dump,"Change Record [%d,x%d]: Oct UnDetach Not OK (%d) in Undo operation of an Detach.",
			     cr.objectId,xid,retval);
	    } else {
		printproblem(dump,"Change Record [%d,x%d]: OctUnDetach",
			     cr.objectId,xid);
	    }
	    break;
	}
	start = 0;
    }
    wnFlush();
#ifdef DEBUGCL
    fclose(dump);
#endif
    return;
}

