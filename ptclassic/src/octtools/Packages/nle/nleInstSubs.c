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
#include "utility.h"
#include "errtrap.h"
#include "oct.h"
#include "oh.h"
#include "nle.h"
#include "nleCommon.h"

/*   Routines to determine if two facets are equivalent */
#define NLE_NULL 0
typedef struct termElement {
    char *name;
    char *type;
    octObject *Term;
    struct termElement *prec;
    struct termElement *next;
    struct termElement *equivPrec; /* pointer to electrically equiv pin */
    struct termElement *equivNext; /* pointer to electrically equiv pin */
} 
termElement;




/* Forward declarations */



static termElement* listFormalTerminals();


static int debug = 1;

static int
powerTerm(t)
termElement *t;
{
    return(strcmp(t->type,"SUPPLY")== 0 || strcmp(t->type,"GROUND")==0 );
}





static int
findEquivalentTerms(Facet, firstTermPtr)
octObject *Facet;
termElement *firstTermPtr;
{
    octObject JoinedTerms, Joined;
    octObject Term;
    octGenerator genBag,genTerm;
    termElement *last, *this;
    char buf[128];

    JoinedTerms.type = OCT_BAG;
    JoinedTerms.contents.bag.name = "JOINEDTERMS";

    if (octGetByName(Facet,&JoinedTerms) == OCT_OK) {
        OH_ASSERT(octInitGenContents(&JoinedTerms, OCT_BAG_MASK, &genBag));
        while (octGenerate(&genBag,&Joined) == OCT_OK) {
            if (strcmp(Joined.contents.bag.name, "JOINED") != 0) {
                (void) sprintf(buf,"Extraneous bag %s found in JOINEDTERMS bag\n", Joined.contents.bag.name);
                if (debug) (void) fprintf(stderr, buf);
            } 
            else {
                OH_ASSERT(octInitGenContents(&Joined,OCT_TERM_MASK,&genTerm));
                last = NLE_NULL;
                while (octGenerate(&genTerm,&Term) == OCT_OK) {
                    if (!octIdIsNull(Term.contents.term.instanceId)) {
                        (void) sprintf(buf,"Actual terminal %s in JOINED bag", Term.contents.term.name);
                        nleWarning(buf);
                        return(NLE_NOT_OK);
                    }
                    /* find this terminal in the list of the formal terminals */
                    this = firstTermPtr;
                    while (this != NLE_NULL && strcmp(this->name,Term.contents.term.name)!=0) this= this->next;
                    if (this == NLE_NULL) {
                        (void) sprintf(buf,"No formal terminal with name %s\n",Term.contents.term.name);
                        nleWarning(buf);
                        return(NLE_NOT_OK);
                    } 
                    else if (this->equivNext != NLE_NULL) {
                        (void) sprintf(buf,"Error: term %s of %s:%s is contained in a joined bag more than once\n",
                        this->name, Facet->contents.facet.cell, Facet->contents.facet.view);
                        nleWarning(buf);
                        return(NLE_NOT_OK);
                    } 
                    else {
                        if (last == NLE_NULL ) {
                            last = this;
                            this->equivNext = this->equivPrec = this;    /* point to itself */
                        } 
                        else {        /* add at the end of the list */
                            this->equivPrec = last;
                            this->equivNext = last->equivPrec; /* the list is a ring */
                            this->equivPrec->equivNext = this;
                            this->equivNext->equivPrec = this;
                            last = this;
                        }
                    }
                }
            }
        }
    }
    return(NLE_OK);
}


/*  returns list of all formal terminals sorted by name */
static termElement *
listFormalTerminals(Facet)
octObject *Facet;
{
    termElement *firstTermPtr = NLE_NULL;
    termElement *lastTermPtr = NLE_NULL;
    termElement *newTermPtr, *termPtr;
    octGenerator genTerm;
    octObject Term;
    octObject TermType;
    octObject Interface;
    char buf[256];

    Interface.type = OCT_FACET;
    Interface.contents.facet = Facet->contents.facet;
    Interface.contents.facet.facet = "interface";
    Interface.contents.facet.mode = "r";
    if (octOpenFacet(&Interface)<OCT_OK) {
        (void) sprintf(buf,"Opening %s:%s:%s",
        Interface.contents.facet.cell,
        Interface.contents.facet.view,
        Interface.contents.facet.facet);
        nleFatalError(buf);
    }

    TermType.type = OCT_PROP;
    TermType.contents.prop.name = "TERMTYPE";

    OH_ASSERT(octInitGenContents(&Interface,OCT_TERM_MASK,&genTerm));
    while (octGenerate(&genTerm,&Term) == OCT_OK) {
        if (octIdIsNull(Term.contents.term.instanceId)) {
            newTermPtr = ALLOC(termElement, 1);
            newTermPtr->next = NLE_NULL;
            newTermPtr->prec = NLE_NULL;
            newTermPtr->equivNext = NLE_NULL;
            newTermPtr->equivPrec = NLE_NULL;
            /* fill up all the fields in the record of this terminal */
            newTermPtr->name = Term.contents.term.name;
            if (octGetByName(&Term,&TermType) == OCT_NOT_FOUND) {
                newTermPtr->type = "SIGNAL";    /* default type is SIGNAL */
            } 
            else {
                newTermPtr->type = strsave(TermType.contents.prop.value.string);
            }
            /* insert in the list in alphabetical order */
            if (firstTermPtr == NLE_NULL) {
                firstTermPtr =  lastTermPtr = newTermPtr;
            } 
            else if (strcmp(firstTermPtr->name,newTermPtr->name)>=0) {
                /*  insert at the beginning of the list */
                newTermPtr->next = firstTermPtr;
                firstTermPtr->prec = newTermPtr;
                firstTermPtr= newTermPtr;
            } 
            else if  (strcmp(lastTermPtr->name,newTermPtr->name)<0) {
                /* insert at the end of the list */
                newTermPtr->prec = lastTermPtr;
                newTermPtr->prec->next = newTermPtr;
                lastTermPtr = newTermPtr;
            } 
            else {
                termPtr = firstTermPtr;
                while (termPtr != NLE_NULL && strcmp(termPtr->name, newTermPtr->name)<0){
                    termPtr = termPtr->next;
                }
                /* if (termPtr == NLE_NULL) abort(); *//* debug stoppoint */

                newTermPtr->next = termPtr;
                newTermPtr->prec = termPtr->prec;
                newTermPtr->next->prec = newTermPtr;
                newTermPtr->prec->next = newTermPtr;
            }
        }
    }
    if (findEquivalentTerms(&Interface, firstTermPtr)==NLE_OK) {
        return(firstTermPtr);
    } 
    else {
        return(NLE_NULL);
    }
}



int
nleModifyInstanceView(OldInstance, NewInstance)
octObject *OldInstance;
octObject *NewInstance;    /* The view must have been specified */
{
    octObject    Chip;
    octObject    OldMaster;
    octObject    NewMaster;
    octObject    NewTerm,OldTerm;
    octObject    Object;        /* generic oct object */
    octGenerator    genOldTerm;
    octGenerator    genAll;
    char        buf[128];

    octGetFacet(OldInstance,&Chip);
    if (debug) (void) fprintf(stderr, "Substitute view %s with %s for instance %s in %s:%s\n",
    OldInstance->contents.instance.view,
    NewInstance->contents.instance.view,
    OldInstance->contents.instance.name,
    Chip.contents.facet.cell,
    Chip.contents.facet.view);

    NewInstance->type = OCT_INSTANCE;
    NewInstance->contents.instance.master= OldInstance->contents.instance.master;
    NewInstance->contents.instance.facet=OldInstance->contents.instance.facet;
    NewInstance->contents.instance.version=OldInstance->contents.instance.version;
    NewInstance->contents.instance.name = OldInstance->contents.instance.name;
    (NewInstance->contents.instance.transform = OldInstance->contents.instance.transform); 

    OldMaster.type = OCT_FACET;
    OldMaster.contents.facet.facet ="interface";
    OldMaster.contents.facet.version = OCT_CURRENT_VERSION;
    OldMaster.contents.facet.mode ="r";

    NewMaster.type = OCT_FACET;
    NewMaster.contents.facet.facet ="interface";
    NewMaster.contents.facet.version = OCT_CURRENT_VERSION;
    NewMaster.contents.facet.mode ="r";

    if (octOpenMaster(OldInstance,&OldMaster)< OCT_OK) {
        octError("The old instance has no master");
        return(NLE_NOT_OK);
    }

    if (octCreate(&Chip,NewInstance) != OCT_OK ||
        octOpenMaster(NewInstance,&NewMaster)< OCT_OK) {
        octError("The new instance view has not master");
        OH_ASSERT(octDelete(NewInstance));
        return(NLE_NOT_OK);
    }

    if (nleIOequivalent(&OldMaster,&NewMaster)!= NLE_OK) {
        nleWarning("The two views are not equivalent");
        return(NLE_NOT_OK);
    }


    OH_ASSERT(octInitGenContents(OldInstance, OCT_ALL_MASK , &genAll));

    while (octGenerate(&genAll, &Object)==OCT_OK) {
        if (Object.type != OCT_TERM) {      /* because terms are treated separately */
            OH_ASSERT(octAttach(NewInstance,&Object));
        }
    }

    OH_ASSERT(octInitGenContainers(OldInstance, OCT_ALL_MASK , &genAll));

    while (octGenerate(&genAll, &Object)==OCT_OK) {
        if (Object.type != OCT_FACET) { /* Because NewInstance is already attached to facet */
            OH_ASSERT_DESCR(octAttach(&Object, NewInstance),
            "attaching instance to container");
        }
    }
    /*
       *  now, do the terminals
       */
    NewTerm.type = OCT_TERM;
    OH_ASSERT(octInitGenContents(OldInstance,OCT_TERM_MASK,&genOldTerm));
    while (octGenerate(&genOldTerm, &OldTerm) == OCT_OK) {
        NewTerm.contents.term.name = OldTerm.contents.term.name;
        if (octGetByName(NewInstance,&NewTerm) == OCT_NOT_FOUND) {
            /* XXX BUG?? */
            (void) sprintf(buf,"Terminal %s does not exist in view %s\n",
            OldTerm.contents.term.name,
            NewInstance->contents.instance.view);
            OH_ASSERT_DESCR(octDelete(NewInstance),
            "Deleting instance on abort action");
            return(NLE_NOT_OK);
        }
        OH_ASSERT(octInitGenContents(&OldTerm,OCT_ALL_MASK,&genAll));
        while (octGenerate(&genAll,&Object) == OCT_OK) {
            OH_ASSERT_DESCR(octAttach(&NewTerm,&Object),
            "attaching something to newterm");
        }
        OH_ASSERT(octInitGenContainers(&OldTerm,OCT_ALL_MASK,&genAll));
        while (octGenerate(&genAll,&Object) == OCT_OK) {
            if (Object.type != OCT_INSTANCE && Object.type != OCT_FACET) {
                OH_ASSERT_DESCR(octAttach(&Object,&NewTerm),
                "attaching newterm to object");
            }
        }
    }
    /*
       *  Remove the old instance. Everything attached to it will also disappear
       */
    OH_ASSERT_DESCR(octDelete(OldInstance),"Deleting old instance");

    if (debug) (void) fprintf(stderr, "----- Instance %s substituted\n",
    OldInstance->contents.instance.name);
    return(NLE_OK);
}





int
nleIOequivalent(FacetA, FacetB)
octObject *FacetA, *FacetB;
{
    termElement *termsOfA, *termsOfB;
    termElement *a, *b;
    termElement *equivA, *equivB, *firstA , *firstB;
    termElement *listFormalTerminals();
    int  IOeq = NLE_TRUE;
    int  found;
    char  buf[128];

    termsOfA = listFormalTerminals(FacetA);
    termsOfB = listFormalTerminals(FacetB);

    if (termsOfA == NLE_NULL || termsOfB == NLE_NULL) {
        return(NLE_NOT_OK);
    }

    a = termsOfA;
    b = termsOfB;

    while( a != NLE_NULL && b != NLE_NULL) {
        if (strcmp(a->name, b->name)==0) {
            a = a->next;
            b = b->next;
        } 
        else {
            /*
                   ** may be either a or b are not terminals of type SUPPLY or GROUND
                   ** in which case they can be ignored at this time
                   */
            if (powerTerm(a)) {
                a = a->next;
                continue;        /* while loop */
            }
            if (powerTerm(b)) {
                b = b->next;
                continue;        /* while loop */
            }

            /* may be the two terminals have equivalent pins */
            found = NLE_FALSE;
            firstB = NLE_NULL;
            equivB = b;
            while(found==NLE_FALSE && equivB != firstB && equivB != NLE_NULL) {
                if (firstB==NLE_NULL) firstB = equivB;
                firstA = NLE_NULL;
                equivA = a;
                while (found == NLE_FALSE && equivA!=firstA && equivA != NLE_NULL) {
                    if (firstA==NLE_NULL) firstA = equivA;
                    if (strcmp(equivA->name,equivB->name)==0)    found = NLE_TRUE ;
                    if (found == NLE_FALSE) equivA=equivA->equivNext;
                }
                if (found==NLE_FALSE) equivB=equivB->equivNext;
            }
            if (found) {
                if (debug) {
                    (void) sprintf(buf,"Term %s of %s <--> Term %s of %s\n",
                    a->name,
                    ohFormatName(FacetA),
                    b->name,
                    ohFormatName(FacetB));
                    (void) sprintf(buf,"\t by virtue of electrically equivalent terminals\n");
                    if (debug) (void) fprintf(stderr, buf);
                }
                a = a->next;
                b = b->next;
            } 
            else if (strcmp(a->name, b->name)<0) {
                (void) sprintf(buf,"Term %12s of %s:%s is NOT in %s:%s\n",
                a->name,
                FacetA->contents.facet.view,
                FacetA->contents.facet.facet,
                FacetB->contents.facet.view,
                FacetB->contents.facet.facet);
                if (debug) (void) fprintf(stderr, buf);
                IOeq = NLE_FALSE;
                a = a->next;
            } 
            else {
                (void) sprintf(buf,"Term %12s of  %s:%s is NOT in %s:%s\n",
                b->name,
                FacetB->contents.facet.view,
                FacetB->contents.facet.facet,
                FacetA->contents.facet.view,
                FacetA->contents.facet.facet);
                if (debug) (void) fprintf(stderr, buf);
                IOeq = NLE_FALSE;
                b = b->next;
            }
        }
    }
    /* are there any terminals left over ? */
    while (a != NLE_NULL) {
        if (!powerTerm(a)) {
            (void) sprintf(buf,"Term %12s of %s:%s is NOT in %s:%s\n",
            a->name,
            FacetA->contents.facet.view,
            FacetA->contents.facet.facet,
            FacetB->contents.facet.view,
            FacetB->contents.facet.facet);
            if (debug) (void) fprintf(stderr, buf);
            IOeq = NLE_FALSE;
        }
        a = a->next;
    }
    while (b != NLE_NULL) {
        if (!powerTerm(b)) {
            (void) sprintf(buf,"Term %12s of  %s:%s is NOT in %s:%s\n",
            b->name,
            FacetB->contents.facet.view,
            FacetB->contents.facet.facet,
            FacetA->contents.facet.view,
            FacetA->contents.facet.facet);
            if (debug) (void) fprintf(stderr, buf);
            IOeq = NLE_FALSE;
        }
        b = b->next;
    }
    return(IOeq);
}





