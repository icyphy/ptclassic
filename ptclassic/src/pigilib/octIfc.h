#ifndef OCTIFC_H
#define OCTIFC_H 1
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
/* octIfc.h  edg
Version identification:
$Id$
*/

extern char *AkoName();

#ifdef __cplusplus
extern char *AkoName();

extern boolean GetStarName(octObject *instPtr, char** namePtr);
extern boolean SetFormalParams(octObject *galFacetPtr, ParamListType *pListPtr);
extern boolean GetFormalParams(octObject *galFacetPtr, ParamListType *pListPtr);
extern boolean SetSogParams(octObject *sog,ParamListType *plist);
extern boolean GetOrInitSogParams(octObject *instPtr, ParamListType *pListPtr);
extern void GetOrInitDelayProp(octObject *instPtr, octObject *propPtr);
extern void GetOrInitBusProp(octObject *instPtr, octObject *propPtr);
extern void IntizeProp(octObject *propPtr);
extern boolean IsVemConnector(octObject *instPtr);
extern boolean IsIoPort(octObject *instPtr);
extern boolean IsInputPort(octObject *instPtr);
extern boolean IsDelay(octObject *instPtr);
extern boolean IsDelay2(octObject *instPtr); // added 3/2/94
extern boolean IsGal(octObject *instPtr);
extern boolean IsStar(octObject *instPtr);
extern boolean IsUniv(octObject *instPtr);
extern boolean IsGalFacet(octObject *facetPtr);
extern boolean IsMarker(octObject *instPtr);
extern boolean IsCursor(octObject *instPtr);

extern int GetIterateProp(octObject *facetPtr, int *pIterate);
extern void SetIterateProp(octObject *facetPtr, int iterate);
extern boolean GetStringProp(octObject *objPtr, char *propName, 
                             char **valuePtr);
extern boolean SetStringProp(octObject *objPtr, char *propName, char *value);

extern boolean GOCDomainProp(octObject *facetPtr, char **domainPtr, char *defaultDomain);
extern boolean MyOpenMaster( octObject *t, octObject *i, char *f, char *m);
extern boolean IsPal(octObject *instPtr);
extern boolean SetTargetParams(octObject *instPtr, ParamListType *pListPtr);
extern boolean SetTargetProp( octObject *facetPtr, char *target);
extern boolean GetTargetParams(char* targName, octObject *facetPtr, ParamListType *pListPtr);
extern boolean GOCTargetProp(octObject *facetPtr, char **targetPtr, char *defaultTarget);
extern boolean SetDomainProp(octObject *facetPtr, char *domain);
extern boolean StringizeProp(octObject *propPtr, char* buf, int len);
extern boolean AutoLoadCk(octObject *instPtr);
extern boolean GetStringizedProp(octObject *objPtr, char *name, char*dest, int dlen);
extern boolean IsPalFacet(octObject *facetPtr);
extern boolean LoadTheStar(octObject* instPtr, int permB, char* linkArgs);
extern boolean GetDefaultParams(octObject *instPtr, ParamListType *pListPtr);


/* Added for ptkCommands.cc */
extern boolean IsUnivFacet(octObject *facetPtr);
extern boolean IsBus(octObject *instPtr);


/* FIXME: These Functions are NOT defined in octIfc.c */
extern boolean GetGalTerms();
extern boolean GetHardwareProp();
extern boolean SetHardwareProp();
extern boolean GOCArchProp();

extern boolean GetOrCreatePropStr(octObject *c, octObject *t,char *s,
				  char *s1);

extern boolean OpenFacet(octObject *t, char *c, char *v, char *f, char *m);

#else
extern void GetOrInitDelayProp();
extern void GetOrInitBusProp();
extern void IntizeProp();
extern boolean SetFormalParams();
extern boolean GetFormalParams();
extern boolean SetSogParams();
extern boolean GetOrInitSogParams();
extern boolean IsVemConnector();
extern boolean IsIoPort();
extern boolean IsInputPort();
extern boolean IsDelay();
extern boolean IsDelay2();
extern boolean IsGal();
extern boolean IsStar();
extern boolean IsUniv();
extern boolean IsGalFacet();
extern boolean IsMarker();
extern boolean IsCursor();

extern boolean GetStringProp();
extern boolean SetStringProp();
extern boolean GOCDomainProp();
extern int GetIterateProp();
extern void SetIterateProp();

extern boolean FreeOctMembers(/* octObject *o */);

/* Added for POct.cc */
extern boolean IsUnivFacet();
extern boolean IsBus();
extern boolean MyOpenMaster();
extern boolean IsPal();
extern boolean OpenFacet();
extern boolean AutoLoadCk();
extern boolean GetStringizedProp( /* octObject *objPtr, char *name,
				     char*dest, int dlen */);
extern boolean IsPalFacet();
extern boolean LoadTheStar(/* octObject* instPtr, int permB, char*
			      linkArgs */);
extern boolean GetDefaultParams(/* octObject *instPtr, ParamListType
				 *pListPtr */); 
extern boolean SetTargetParams();
extern boolean GetTargetParams();
extern boolean GOCTargetProp();
extern boolean SetDomainProp();
extern boolean SetTargetProp();

/* FIXME: These Functions are NOT defined in octIfc.c */
extern boolean GetGalTerms();
extern boolean GetHardwareProp();
extern boolean SetHardwareProp();
extern boolean GOCArchProp();

/* This is defined in octtools/Packages/oct/gen.c, and used in
 * octIfc.c, mkIcon.c and palette.c
 */
extern int octInitGenContentsSpecial(/* octObject *object,
				      octObjectMaskmask, octGenerator
				      *generator */);
#endif /* __cplusplus*/
#endif /* OCTIFC_H */
