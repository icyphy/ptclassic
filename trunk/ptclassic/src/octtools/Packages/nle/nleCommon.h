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

#define strsave(s)  strcpy(malloc((unsigned) strlen(s)+1),s)

#define NLE_PKG_NAME "nle"

#define NULL_STRING ""
#define STRNULL(s) (strcmp((s),NULL_STRING)==0)

#define NLE_TRUE 1
#define NLE_FALSE 0

extern void nleFatalError();
extern void nleWarning();

extern	octObject	LayerPlace; 
/***** properties *** */
extern	octObject	CellSelect;
extern  octObject	PlacementClass;
extern	octObject	Mobility;
extern  octObject       FloorplanBag; /* This is a bag */
extern	octObject	MinHeightOverWidth;
extern  octObject	MaxHeightOverWidth;
extern  octObject       FloatingEdge;
extern  octObject	TermEdge;
extern	octObject	TermType;
extern  octObject	Direction;
extern	octObject	JoinedTerms;
extern	octObject	Joined;
extern	octObject	OrderTerms;
extern	octObject	PermTerms;
extern	octObject	NetType;
extern	octObject	Criticality;

 /* OSP required properties */
extern  octObject	EditStyle;
extern  octObject	ViewType;
extern  octObject	Technology;

extern char *TermTypeString[]; 
extern char *NetTypeString[]; 


#define  VEM_MIN_FEATURE 20 
#define  DEFAULT_CRITICALITY 0

