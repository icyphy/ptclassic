#ifndef OCTIFC_H
#define OCTIFC_H 1
/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
/* octIfc.h  edg
Version identification:
$Id$
*/

extern char *AkoName();

extern boolean SetFormalParams();
extern boolean GetFormalParams();
extern boolean SetSogParams();
extern boolean GetOrInitSogParams();
extern void GetOrInitDelayProp();
extern void GetOrInitBusProp();
extern void IntizeProp();
extern boolean IsVemConnector();
extern boolean IsIoPort();
extern boolean IsInputPort();
extern boolean IsDelay();
extern boolean IsGal();
extern boolean IsStar();
extern boolean IsUniv();
extern boolean IsGalFacet();
extern boolean GetCommentProp();
extern boolean SetCommentProp();
extern boolean GOCDomainProp();
extern int GetIterateProp();
extern void SetIterateProp();

/* Added for POct.cc */
extern boolean IsUnivFacet();
extern boolean IsBus();


/* FIXME: These Functions are NOT defined in octIfc.c */
extern boolean GetGalTerms();
extern boolean GetHardwareProp();
extern boolean SetHardwareProp();
extern boolean GOCArchProp();

#endif /* OCTIFC_H */
