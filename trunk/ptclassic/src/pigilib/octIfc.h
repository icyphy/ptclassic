#ifndef _OCTIFC_H
#define _OCTIFC_H 1

/* 
Copyright (c) 1990-1996 The Regents of the University of California.
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
/* octIfc.h  edg
Version identification:
@(#)octIfc.h	1.27	11/26/95
*/

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

/* Length of an oct facet handle string */
#define	POCT_FACET_HANDLE_LEN	16

#include "paramStructs.h"
#include "oct.h"

extern boolean GetStarName ARGS((octObject *instPtr, char** namePtr));
extern boolean MyOpenMaster ARGS((octObject *t, octObject *i,
				  char *f, char *m));
extern boolean IsVemConnector ARGS((octObject *instPtr));
extern boolean IsIoPort ARGS((octObject *instPtr));
extern boolean IsInputPort ARGS((octObject *instPtr));
extern boolean IsDelay ARGS((octObject *instPtr));
extern boolean IsDelay2 ARGS((octObject *instPtr));
extern boolean IsBus ARGS((octObject *instPtr));
extern boolean IsMarker ARGS((octObject *instPtr));
extern boolean IsGal ARGS((octObject *instPtr));
extern boolean IsStar ARGS((octObject *instPtr));
extern boolean IsUniv ARGS((octObject *instPtr));
extern boolean IsPal ARGS((octObject *instPtr));
extern boolean IsCursor ARGS((octObject *instPtr));
extern boolean IsGalFacet ARGS((octObject *facetPtr));
extern boolean IsPalFacet ARGS((octObject *facetPtr));
extern boolean IsUnivFacet ARGS((octObject *facetPtr));
extern char *AkoName ARGS((char *master));
extern boolean SetParamProp ARGS((octObject *galFacetPtr,
				  ParamListType *pListPtr,
				  char* propname));
extern boolean SetFormalParams ARGS((octObject *galFacetPtr,
				     ParamListType *pListPtr));
extern boolean GetFormalParams ARGS((octObject *galFacetPtr,
				     ParamListType *pListPtr));
extern boolean SetSogParams ARGS((octObject *sog,ParamListType *plist));
extern boolean SetTargetParams ARGS((octObject *sog,ParamListType *plist));
extern boolean AutoLoadCk ARGS((octObject *instPtr));
extern boolean LoadTheStar ARGS((octObject* instPtr, int permB,
				 char* linkArgs));
extern ParamType* findParam ARGS((const char* name, ParamListType *pList));
extern boolean GetTargetParams ARGS((char* targName, octObject *facetPtr,
			       ParamListType *pListPtr));
extern boolean GetDefaultParams ARGS((octObject *instPtr,
				      ParamListType *pListPtr));
extern void GetOrInitDelayProp ARGS((octObject *instPtr, octObject *propPtr));
extern void GetOrInitBusProp ARGS((octObject *instPtr, octObject *propPtr));
extern boolean StringizeProp ARGS((octObject *propPtr, char* buf, int len));
extern boolean GetStringizedProp ARGS((octObject *objPtr, char *name,
				       char *dest, int dlen));
extern void IntizeProp ARGS((octObject *propPtr));
extern boolean GetStringProp ARGS((octObject *objPtr, char *propName, 
				   const char **valuePtr));
extern boolean SetStringProp ARGS((octObject *objPtr, char *propName,
				   char *value));
extern boolean GOCDomainProp ARGS((octObject *facetPtr, const char **domainPtr,
				   const char *defaultDomain));
extern boolean SetDomainProp ARGS((octObject *facetPtr, char *domain));
extern boolean GOCTargetProp ARGS((octObject *facetPtr, const char **targetPtr,
				   const char *defaultTarget));
extern boolean SetTargetProp ARGS((octObject *facetPtr, char *target));
extern int GetIterateProp ARGS((octObject *facetPtr, int *pIterate));
extern void SetIterateProp ARGS((octObject *facetPtr, int iterate));
extern boolean CreateOrModifyPropInt ARGS((octObject *c, octObject *t,
					   char *s, int i));
extern boolean CreateOrModifyPropStr ARGS((octObject *c, octObject *t,
					   char *s, char *s1));
extern boolean CreateOrModifyPropReal ARGS((octObject *c, octObject *t,
					    char *s, double d));
extern boolean GetOrInitSogParams ARGS((octObject *instPtr,
					ParamListType *pListPtr));
extern boolean GetOrCreatePropStr ARGS((octObject *c, octObject *t,
					char *s, char *s1));
extern boolean GetOrCreatePropReal ARGS((octObject *c, octObject *t,
					 char *s, double d));
extern boolean OpenFacet ARGS((octObject *t, char *c, char *v,
			       char *f, char *m));
extern boolean FreeOctMembers ARGS((octObject *o));

/*
 * This is defined in octtools/Packages/oct/gen.c, and used in
 * octIfc.c, mkIcon.c and palette.c
 */
extern int octInitGenContentsSpecial ARGS((octObject *object,
					   octObjectMask mask,
					   octGenerator *generator));

#endif /* _OCTIFC_H */
