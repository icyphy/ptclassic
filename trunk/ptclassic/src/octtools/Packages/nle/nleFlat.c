#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1994 The Regents of the University of California.
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
/*
 *  FILE: flat.c
 *
 *  DESCRIPTION:
 *
 *
 *  CREATION DATE: xx/xx/xx        AUTHOR: A.Casotto
 *                      AUTHOR: R.Segal (for the basic flat routine)
 *
 *  FUNCTIONS: 
 */
#include "port.h"
#include "utility.h"
#include "errtrap.h"
#include "st.h"
#include "tr.h"
#include "oct.h"
#include "oh.h"
#include "ansi.h"
#include "nle.h"
#include "nleCommon.h"
#include "nleMisc.h"

/* Forward declaration */
int nleFlatInstance
	ARGS((octObject *InstToFlatP, octObject *FlatBagP, int type));

static int FLATprocessInstances();
static int FLATprocessLocalNets();
static int FLATprocessCrossingNets();
static void FLATprocessPaths
	ARGS((octObject *InstToFlatP, int type));
static void FLATprocessGeos
	ARGS((octObject *InstToFlatP, int type));
static int FLATprocessFormalTerminals();

static int putNetTermsOnChipNet();
static int promoteInst();
static int put_terms_on_path();


/*
 *   GLOBAL variables for this file
 */
static int         debug = 0;        /* debug flag for this file */
static octObject     gFacet;            /* facet to work on */
static octObject     gChipInstBag;
static octObject     gChipConnBag;
static char          buffer[256];        /* buffer string for messages */
static st_table *stInst;            /* contains map from original octIds
                         * to sub-insts just copied up */
static st_table *stNet;                /* Same for nets */

static st_table *stGeo;                /* ... and for geos */

static void putTermOnNet();
static void mergeNets();
static void printArgumentsForFlatInstance();

static octObject MasterContents;        /* contents of InstToFlatP */
static octObject SubInstBag;
static octObject SubConnBag;
/*
 * ChipInstBagTmp temporary bag where to put the instances
 * until we find that all the sub inst can be  reinstantiated
 */
static octObject ChipInstBagTmp, ChipConnBagTmp;

static int        numOfSubInst = 0;    /* number of sub instances */


int
nleFlat(ArgInstBagP, FlatBagP, type)
octObject *ArgInstBagP;
octObject *FlatBagP;    /* The bag with all new instances */
int        type;        /* NLE_SOFT or NLE_HARD */
{
    octObject     InstToFlat;        /* Top level instance to expand */
    octObject    Facet;
    octObject    TmpBag;
    octObject    Object;
    octGenerator  genAny;
    octGenerator    instGen;

    /******  main loop  *****/
    OH_ASSERT(octInitGenContents(ArgInstBagP, OCT_INSTANCE_MASK, &instGen));
    octGetFacet(ArgInstBagP,&Facet);
    OH_ASSERT(ohCreateBag(&Facet,&TmpBag, "BAG TO COLLECT THE NEW INSTANCES"));

    while (octGenerate(&instGen, &InstToFlat) == OCT_OK) {

        if (nleFlatInstance(&InstToFlat, &TmpBag, type)!=NLE_OK) {
            (void) sprintf(buffer,"Could not flat %s of %s\n",
            InstToFlat.contents.instance.name,
            ohFormatName(&InstToFlat));
            nleWarning(buffer);
            return(NLE_NOT_OK);
        }
        OH_ASSERT(octInitGenContents(&TmpBag,OCT_ALL_MASK,&genAny));
        while (octGenerate(&genAny,&Object)==OCT_OK) {
            if (FlatBagP != (octObject *) 0 ) {
                OH_ASSERT(octAttach(FlatBagP,&Object));
            }
            OH_ASSERT(octDetach(&TmpBag,&Object));
        }
    }
    OH_ASSERT(octDelete(&TmpBag));
    return(NLE_OK);
}


/*
 * Take the contents of InstToFlat and copy them to the top level.
 * Make appropriate connections and translations.
 * Give the new inst and nets nice names
 */
int
nleFlatInstance(InstToFlatP, FlatBagP, type)
octObject *InstToFlatP;        /* instance to expand */
octObject *FlatBagP;        /* The bag with all new instances */
int        type;            /* NLE_SOFT or NLE_HARD */
{
    octObject     NewInst;        /* SubInst promoted in gFacet */
    octGenerator  genInst;
    int        flatStatus = NLE_OK;    /* is NOT_OK if cannot flat some sub instance */
    octStatus  openStatus;

    if (debug) printArgumentsForFlatInstance(InstToFlatP);

    stInst = st_init_table(octIdCmp, octIdHash);
    stNet  = st_init_table(octIdCmp, octIdHash);
    stGeo  = st_init_table(octIdCmp, octIdHash);

    octGetFacet(InstToFlatP,&gFacet);    /* Get the working facet */

    OH_ASSERT(ohGetOrCreateBag(&gFacet, &gChipInstBag, "INSTANCES"));
    OH_ASSERT(ohGetOrCreateBag(&gFacet, &gChipConnBag, "CONNECTORS"));

    MasterContents.type = OCT_FACET;
    MasterContents.contents.facet.cell = InstToFlatP->contents.instance.master;
    MasterContents.contents.facet.view = InstToFlatP->contents.instance.view;
    MasterContents.contents.facet.facet = "contents";
    MasterContents.contents.facet.version = InstToFlatP->contents.instance.version;
    MasterContents.contents.facet.mode = "r";

    /* XXX changed from octOpenFacet to octOpenMaster */
    openStatus = octOpenMaster(InstToFlatP, &MasterContents);
    OH_ASSERT(openStatus);

    /* instances of InstToFlatP to gFacet */
    OH_ASSERT(ohGetOrCreateBag(&MasterContents, &SubInstBag, "INSTANCES"));
    OH_ASSERT(ohGetOrCreateBag(&MasterContents, &SubConnBag, "CONNECTORS"));
    OH_ASSERT(ohCreateBag(&gFacet,&ChipInstBagTmp, "TMP_NEW_INSTANCES"));
    OH_ASSERT(ohCreateBag(&gFacet,&ChipConnBagTmp, "TMP_NEW_CONNECTORS"));

    flatStatus = FLATprocessInstances(InstToFlatP,type);

    if (flatStatus == NLE_NOT_OK) {    /* recover and return */
        OH_ASSERT(octInitGenContents(&ChipInstBagTmp,OCT_INSTANCE_MASK,&genInst));
        while (octGenerate(&genInst,&NewInst)==OCT_OK) {
            OH_ASSERT(octDelete(&NewInst));
        }
        OH_ASSERT(octDelete(&ChipInstBagTmp));
        return(NLE_NOT_OK);            /* error code for this routine */
    }

    if (FLATprocessCrossingNets(InstToFlatP,type)!=NLE_OK) return(NLE_NOT_OK);

    /* process "local" nets of InstToFlatP */
    if (FLATprocessLocalNets(InstToFlatP,type)!=NLE_OK) return(NLE_NOT_OK); 

    if (type == NLE_HARD || type == NLE_PHYSICAL ) {
        FLATprocessPaths(InstToFlatP, type);
    }

    if (type == NLE_PHYSICAL) {
        FLATprocessGeos(InstToFlatP, type);
    }

    /* attach MasterFTerms to ActualTerms */
    if (FLATprocessFormalTerminals(InstToFlatP,type)!=NLE_OK) return(NLE_NOT_OK);

    /* delete the flattened instance */
    OH_ASSERT(octDelete(InstToFlatP));

    /* Move all the new instances to the real INSTANCES bag */
    OH_ASSERT(octInitGenContents(&ChipInstBagTmp,OCT_INSTANCE_MASK,&genInst));
    while (octGenerate(&genInst,&NewInst)==OCT_OK) {
        OH_ASSERT(octAttach(&gChipInstBag,&NewInst));
        if (FlatBagP != (octObject *) 0) {
            OH_ASSERT(octAttach(FlatBagP,&NewInst)); /* attach to bag used to hold new instances */
        }
    }
    OH_ASSERT(octInitGenContents(&ChipConnBagTmp,OCT_INSTANCE_MASK,&genInst));
    while (octGenerate(&genInst,&NewInst)==OCT_OK) {
        OH_ASSERT(octAttach(&gChipConnBag,&NewInst));
        if (FlatBagP != (octObject *) 0) {
            OH_ASSERT(octAttach(FlatBagP,&NewInst)); /* attach to bag used to hold new instances */
        }
    }
    OH_ASSERT(octDelete(&ChipInstBagTmp));
    OH_ASSERT(octDelete(&ChipConnBagTmp));

    if ( openStatus == OCT_ALREADY_OPEN ) {
	/* So facet remains open just once */
	OH_ASSERT(octFreeFacet(&MasterContents));
    }

    st_free_table(stNet);
    st_free_table(stInst);
    st_free_table(stGeo);
    return(NLE_OK);
}
/* added to enable setting of name uniquifier used in FLATprocessLocalNets().
 * Culprit: johnny@tde.lth.se 10/1/89
 */
static void (*UniquifierFunc)() = 0;

/* *funcp takes three arguments: a pointer to the old name, a pointer
 * to the instance, and a pointer to a place where the new name is to
 * be built
 */
void
nleSetUniquifier(funcp)
   void (*funcp)(/* char *oldname, octObject *instp, char *newname */);
{
  UniquifierFunc = funcp; 
}

/* this routine has been changed to call *UniquifierFunc if it has 
 * been set (with the routine nleSetUniquifier() (see above)).
 */
static int
FLATprocessInstances(InstToFlatP,type)
octObject     *InstToFlatP;    /* instance to expand */
int        type;        /* NLE_SOFT or NLE_HARD */
{
    octObject     SubInst;        /* Inst contained by InstToFlatP */
    octObject     NewInst;        /* SubInst promoted in gFacet */
    octGenerator     genSubInst;
    tr_stack     *trans;            /* To compute transformation of the new instances */
    octId *id1, *id2;

    trans = tr_create();
    tr_add_transform(trans, &(InstToFlatP->contents.instance.transform), 0);

    /* Copy all the sub-instances  */
    numOfSubInst = 0;            /* we want to flat at least one instance */
    /* Do all instances first */
    if (  type == NLE_SOFT || type == NLE_HARD ) {
	OH_ASSERT(octInitGenContents(&SubInstBag, OCT_INSTANCE_MASK, &genSubInst));
	while (octGenerate(&genSubInst, &SubInst) == OCT_OK) {
	    numOfSubInst++;
	    if (promoteInst(&SubInst,&NewInst, trans) != NLE_OK) {
		return(NLE_NOT_OK);
	    }
	    OH_ASSERT(octAttach(&ChipInstBagTmp,&NewInst));
	    id1 = ALLOC(octId, 1);
	    id2 = ALLOC(octId, 1);
	    *id1 = SubInst.objectId;
	    *id2 = NewInst.objectId;
	    (void) st_add_direct(stInst, (char *) id1, (char *) id2);
	}
    }
    /* Do all connectors if type == NLE_HARD */
    if (type == NLE_HARD) { 
	/* Do all instances */
        OH_ASSERT(octInitGenContents(&SubConnBag, OCT_INSTANCE_MASK, &genSubInst));
        while (octGenerate(&genSubInst, &SubInst) == OCT_OK) {
            numOfSubInst++;
            if (promoteInst(&SubInst,&NewInst, trans) != NLE_OK) {
                return(NLE_NOT_OK);
            }
            OH_ASSERT(octAttach(&ChipConnBagTmp,&NewInst));
	    id1 = ALLOC(octId, 1);
	    id2 = ALLOC(octId, 1);
	    *id1 = SubInst.objectId;
	    *id2 = NewInst.objectId;
            (void) st_add_direct(stInst, (char *) id1, (char *) id2);
        }
    }
    if (type == NLE_PHYSICAL) { 
	/* Do all instances */
        OH_ASSERT(octInitGenContents(&MasterContents, OCT_INSTANCE_MASK, &genSubInst));
        while (octGenerate(&genSubInst, &SubInst) == OCT_OK) {
            numOfSubInst++;
            if (promoteInst(&SubInst,&NewInst, trans) != NLE_OK) {
                return(NLE_NOT_OK);
            }
            /* OH_ASSERT(octAttach(&ChipConnBagTmp,&NewInst)); */
	    id1 = ALLOC(octId, 1);
	    id2 = ALLOC(octId, 1);
	    *id1 = SubInst.objectId;
	    *id2 = NewInst.objectId;
            (void) st_add_direct(stInst, (char *) id1, (char *) id2);
        }
    }

    if (numOfSubInst == 0 && type != NLE_PHYSICAL) {
        nleWarning("No sub-instances in this instance");
        return(NLE_NOT_OK);
    }

    tr_free(trans);        /* Free all storage associated with trans */
    return(NLE_OK);
}

static int
FLATprocessCrossingNets(InstToFlatP, type)
octObject *InstToFlatP;        /* instance to expand */
int        type;        /* NLE_SOFT or NLE_HARD */
{
    octGenerator     genFTerm;
    octObject     subNet;
    octObject     ChipNet, net2;
    octObject     InstToFlatTerm;        /* terminal of the instance to flat */
    octObject     MasterFTerm;
    octObject    ActualTerm;
    octId *id1, *id2;
    char *ptr, *ptr2;

    /*
       ** 1 - process nets of InstToFlatP that are continuations of outside nets and do 
       **     merging as necessary
       ** 2 - process all other nets
       */
    OH_ASSERT(octInitGenContents(&MasterContents, OCT_TERM_MASK, &genFTerm));
    while (octGenerate(&genFTerm, &MasterFTerm) == OCT_OK) {
        /* Get global ChipNet attached to a terminal of InstToFlatP */
        InstToFlatTerm.type = OCT_TERM;
        InstToFlatTerm.contents.term.name = MasterFTerm.contents.term.name;
        OH_ASSERT(octGetByName(InstToFlatP, &InstToFlatTerm));

        /* If the terminal is not connected to any net go on to next terminal. */
        if (octGenFirstContainer(&InstToFlatTerm,OCT_NET_MASK,&ChipNet)!= OCT_OK) continue;

        /* Two possibilities:
	 * 1: terminal is attached to a net (subNet)
	 *    Move all the terminals in subNet to ChipNet
	 * 2: terminal is not attached to a net but it has one terminal attached
	 *
	 */
        if (octGenFirstContainer(&MasterFTerm, OCT_NET_MASK, &subNet)==OCT_OK) {
            if (debug) {
                (void) printf("NLE:: Net %s of %s is a continuation of net %s through term %s\n",
                subNet.contents.net.name,
                InstToFlatP->contents.instance.name,
                ChipNet.contents.net.name,
                MasterFTerm.contents.term.name);
            }
            /* case 1 */
            /*
                   * subNet is a continuation of ChipNet
                   * If the subNet is already in the hash table, then it has already been processed
                   */
            if (st_lookup(stNet, (char *) &subNet.objectId, &ptr)) {
		net2.objectId = *(octId *) ptr;
                if (!octIdsEqual(ChipNet.objectId, net2.objectId)) {
                    OH_ASSERT(octGetById(&net2));
                    (void) sprintf(buffer,"Nets '%s' and '%s' are shorted by net '%s' inside instance '%s' of %s",
                    ChipNet.contents.net.name,
                    net2.contents.net.name,
                    subNet.contents.net.name,
                    InstToFlatP->contents.instance.name,
                    ohFormatName(InstToFlatP));
                    nleWarning(buffer);
                    mergeNets(&ChipNet,&net2); /* This subNet merges two global nets */
		    
		    /* Make sure the stNet table is consistent. 
		     * NOTE: we do not know which net got merged into which.
		     * If the id of net2 is still valid, that's fine, otherwise
		     * replace the entry with the id of ChipNet.
		     */
		    
		    if ( octGetById( &net2 ) != OCT_OK ) {
			ptr = (char *) &subNet.objectId;
			if (!st_delete(stNet, &ptr, &ptr2)) {
			    nleFatalError("BUG:cannot delete entry in stNet");
			}
			net2.objectId = *(octId *) ptr2;
			id1 = ALLOC(octId, 1);
			id2 = ALLOC(octId, 1);
			*id1 = subNet.objectId;
			*id2 = ChipNet.objectId;
			(void) st_add_direct(stNet, (char *) id1, (char *) id2);
			(void)sprintf(buffer, "Replaced (%s->%s) with (%s->%s) in stNet",
				      octFormatId(subNet.objectId),
				      octFormatId(net2.objectId),
				      octFormatId(subNet.objectId),
				      octFormatId(ChipNet.objectId));
			nleWarning( buffer );
		    }
		    
                }
            } 
            else {
                /*
		 * create a new entry in the hash table stNet, that associates
		 * to every subNet the ChipNet that is attached
		 */
		id1 = ALLOC(octId, 1);
		id2 = ALLOC(octId, 1);
		*id1 = subNet.objectId;
		*id2 = ChipNet.objectId;
                (void) st_add_direct(stNet, (char *) id1, (char *) id2);
                if (putNetTermsOnChipNet(&ChipNet,&subNet,type)!=NLE_OK) return(NLE_NOT_OK);
            }
        } else if (octGenFirstContent(&MasterFTerm, OCT_TERM_MASK, &ActualTerm)==OCT_OK) {
            if (debug) {
                (void) printf("NLE:: Net %s connents to instance terminal %s through term %s\n",
			      ChipNet.contents.net.name,
			      ActualTerm.contents.term.name,
			      MasterFTerm.contents.term.name);
            }
            /* case 2 */
            /* no net , just an actual terminal */
            putTermOnNet(&ChipNet,&ActualTerm,type);
        } else {
            if (type != NLE_PHYSICAL) {
                (void) sprintf(buffer,"%s connects to nothing through %s\n\tin %s.",
			       ohFormatName(&ChipNet),
			       ohFormatName(&MasterFTerm),
			       ohFormatName(&MasterContents));
                nleWarning(buffer);
            }
        }
    }
    return NLE_OK;
}

/*ARGSUSED*/
static int
FLATprocessLocalNets(InstToFlatP, type)
    octObject  *InstToFlatP;	/* Instance to expand */
    int        type;            /* NLE_SOFT or NLE_HARD */
{
    octGenerator     genSubNet;
    octObject     subNet;
    octObject     ChipNet;
    /* the next variable must be static because otherwise the algorithm
         to determine a unique name would take O(n^2) in the case that
         all the nets had the samme name to start with */
    static int    count = 0;        /* a counter to determine a unique name */
    char        uniquename[256];     /* for a net */
    octId *id1, *id2;
    char *ptr;

    ChipNet.type=OCT_NET;
    OH_ASSERT(octInitGenContents(&MasterContents, OCT_NET_MASK, &genSubNet));
    while (octGenerate(&genSubNet, &subNet) == OCT_OK) {
        /* Has net already been processed? */
        if (st_lookup(stNet, (char *) &subNet.objectId, &ptr)) {
	    ChipNet.objectId = *(octId *) ptr;
            OH_ASSERT(octGetById(&ChipNet));
            continue;
        }
        /* Try to preserve the net name during flattening */
        ChipNet.contents.net.name = subNet.contents.net.name;
        if (octGetByName(&gFacet,&ChipNet)==OCT_OK) {
            if (debug) {
                (void) sprintf(buffer, "Name collision for net %s",
                ohFormatName(&subNet));
                nleWarning(buffer);
            }
	    ChipNet.contents.net.name = uniquename;
	    if (UniquifierFunc) 
		(*UniquifierFunc)(subNet.contents.net.name, InstToFlatP,
			uniquename); 
	    else do {
                (void) sprintf(uniquename,"%s.%d", subNet.contents.net.name,
			       count++);
            } 
            while (octGetByName(&gFacet,&ChipNet)==OCT_OK);
        }
        OH_ASSERT(octCreate(&gFacet, &ChipNet));

        if (debug) (void) fprintf(stderr, "Creating net %s\n",
				  ChipNet.contents.net.name);

        (void) nleCopyProperties(&ChipNet, &subNet);
	id1 = ALLOC(octId, 1);
	id2 = ALLOC(octId, 1);
	*id1 = subNet.objectId;
	*id2 = ChipNet.objectId;
        (void) st_add_direct(stNet, (char *) id1, (char *) id2);
        if (putNetTermsOnChipNet(&ChipNet, &subNet,type)!=NLE_OK) {
            return(NLE_NOT_OK);
        }
    }
    return(NLE_OK);
}

/*ARGSUSED*/
static void
FLATprocessPaths(InstToFlatP, type)
octObject *InstToFlatP;        /* instance to expand */
int        type;        /* NLE_SOFT or NLE_HARD */
{
    octObject     SubInst;            /* inst contained by InstToFlatP */
    octObject    Facet;        /* used just for fatal errors */
    octGenerator     netGen, pathgen;
    octGenerator     genFTerm, genATerm;
    octObject     subNet, subaterm;
    octObject     ChipNet;
    octObject     path, oldpath, layer;
    octObject     MasterFTerm, ActualTerm;

    struct octPoint vertices[2];        /* vertices for octGetPoints */
    int32         num_points;        /* # of vertices for octGetPoints */
    char *ptr;


    /* 
       * Check all the paths in chip that were attached to terminals in
       * the instance that is being flattened
       */
    OH_ASSERT(octInitGenContents(&MasterContents, OCT_TERM_MASK, &genFTerm));
    while (octGenerate(&genFTerm, &MasterFTerm) == OCT_OK) {
        /* get new ActualTerm replacing the one to be deleted (subaterm) */
        OH_ASSERT(octInitGenContents(&MasterFTerm, OCT_TERM_MASK, &genATerm));
        if (octGenerate(&genATerm, &subaterm) != OCT_OK) {
            continue;      /* MasterFTerm has no implementation */
        }
        if (!st_lookup(stInst, (char *) &subaterm.contents.term.instanceId, &ptr)) {
            /* Fatal ERROR HERE: Allow complete description of the error */
            SubInst.objectId = subaterm.contents.term.instanceId;
            OH_ASSERT(octGetById(&SubInst));
            octGetFacet(&SubInst, &Facet);
            (void) sprintf(buffer, "In %s QUITE PROBABLY, instance %s is neither an INSTANCE nor a CONNECTOR",
            ohFormatName(&Facet),
            ohFormatName(&SubInst));
            nleFatalError(buffer);
        }
	SubInst.objectId = *(octId *) ptr;
        OH_ASSERT(octGetById(&SubInst));
        ActualTerm.type = OCT_TERM;
        ActualTerm.contents.term.name = subaterm.contents.term.name;
        OH_ASSERT(octGetByName(&SubInst, &ActualTerm));

        /* Get paths */
        OH_ASSERT_DESCR(octGetByName(InstToFlatP, &MasterFTerm),
        "master and instance don't match");

        OH_ASSERT(octInitGenContainers(&MasterFTerm, OCT_PATH_MASK, &pathgen));
        while (octGenerate(&pathgen, &path) == OCT_OK) {
            OH_ASSERT(octAttach(&path, &ActualTerm));
        }
    }

    /* Copy up sub-paths (lower level) */
    OH_ASSERT(octInitGenContents(&MasterContents, OCT_NET_MASK, &netGen));
    while (octGenerate(&netGen, &subNet) == OCT_OK) {
        /* get the corresponding net in the chip */
        if (!st_lookup(stNet, (char *) &subNet.objectId, &ptr))
            nleFatalError("Could not find ChipNet in hash table in 'processPath'");
	ChipNet.objectId = *(octId *) ptr;
        OH_ASSERT(octGetById(&ChipNet));
        OH_ASSERT(octInitGenContents(&subNet, OCT_PATH_MASK, &pathgen));
        while (octGenerate(&pathgen, &oldpath) == OCT_OK) {
            /* make new path */
            path.type = OCT_PATH;
            path.contents.path.width = oldpath.contents.path.width;
            OH_ASSERT(octCreate(&ChipNet, &path));

            /* get and translate points for path */
            num_points = 2;
            OH_ASSERT(octGetPoints(&oldpath, &num_points, vertices));

            tr_oct_transform(&(InstToFlatP->contents.instance.transform),
            (int32 *) &vertices[0].x,
            (int32 *) &vertices[0].y);
            tr_oct_transform(&(InstToFlatP->contents.instance.transform),
            (int32 *) &vertices[1].x,
            (int32 *) &vertices[1].y);
            OH_ASSERT(octPutPoints(&path, 2, vertices));

            /* attach path to layer */
            if (octGenFirstContainer(&oldpath, OCT_LAYER_MASK, &layer) == OCT_OK) {
                OH_ASSERT(octGetOrCreate(&gFacet, &layer));
                OH_ASSERT(octAttach(&layer, &path));
            }
            if (put_terms_on_path(&path, &oldpath)!=NLE_OK) {
                (void) sprintf(buffer, "Problems with path on net %s",
                ohFormatName(&subNet));
                nleWarning(buffer);
            }
        }
    }
}

/*ARGSUSED*/
static void
FLATprocessGeos(InstToFlatP, type)
octObject *InstToFlatP;        /* instance to expand */
int        type;        /* NLE_SOFT or NLE_HARD */
{
    octGenerator     layergen, geogen;
    octObject     layer, sublayer, geo;
    octId        SubGeoId;

    /* copy up geoes */
    OH_ASSERT(octInitGenContents(&MasterContents, OCT_LAYER_MASK, &layergen));
    while (octGenerate(&layergen, &sublayer) == OCT_OK) {
        OH_ASSERT(ohGetOrCreateLayer(&gFacet, &layer, sublayer.contents.layer.name));
	/* Paths are taken care of in FLATprocessPath() */
        OH_ASSERT(octInitGenContents(&sublayer, OCT_GEO_MASK & ~OCT_PATH_MASK, &geogen));
        while (octGenerate(&geogen, &geo) == OCT_OK) {
	    octId *id1, *id2;
            SubGeoId = geo.objectId; /* Save old ID. */
	    if ( geo.type == OCT_PATH || geo.type == OCT_POLYGON ) {
		int32 num;
		struct octPoint* array;
		ohGetPoints( &geo, &num, &array );
		octTransformPoints( num, array, &(InstToFlatP->contents.instance.transform) );
		OH_ASSERT(octCreate(&layer, &geo));
		OH_ASSERT(octPutPoints( &geo, num, array ) );
	    } else {
		octTransformGeo( &geo, &(InstToFlatP->contents.instance.transform) );
		OH_ASSERT(octCreate(&layer, &geo));
	    }
	    id1 = ALLOC(octId, 1);
	    id2 = ALLOC(octId, 1);
	    *id1 = SubGeoId;
	    *id2 = geo.objectId;
            (void) st_add_direct(stGeo, (char *) id1, (char *) id2);
        }
    }
}




static void
mergeNets(Net1P,Net2P)
octObject *Net1P;
octObject *Net2P;
{
    octGenerator     genTermOrPath;
    octObject    TermOrPath;

    /* Which net should be retained ? The one with the shortest name  */
    /* unless the name has 0 length */

    if (strlen(Net1P->contents.net.name) < strlen(Net2P->contents.net.name) && 
        strlen(Net1P->contents.net.name) > 0 ) {
        octObject *tmp;        /* Swapping temporary variable */
        tmp = Net1P ; 
        Net1P = Net2P ; 
        Net2P = tmp ;    /* swap the two nets */
    }
    (void) sprintf(buffer,"Merging net '%s' into '%s'",
    Net1P->contents.net.name,Net2P->contents.net.name);
    nleWarning(buffer);
    OH_ASSERT(octInitGenContents(Net1P, OCT_TERM_MASK|OCT_PATH_MASK, &genTermOrPath));
    while(octGenerate(&genTermOrPath, &TermOrPath) == OCT_OK) {
        OH_ASSERT(octAttach(Net2P, &TermOrPath));
    }
    OH_ASSERT(octDelete(Net1P));
    return;
}

/* 
 * take inst of InstToFlatP (SubInstP) and put it at the top level
 * promoted terms of InstToFlatP should be promoted
 */
static int
promoteInst(SubInstP, NewInstP, trans)
octObject *SubInstP;
octObject *NewInstP;
tr_stack     *trans;
{
    octObject ATermOld, ATermNew;
    octGenerator genATerm;
    static int        count = 0;        /*  */
    char        uniquename[256];     /*  */

    ATermNew.type = OCT_TERM;
    /*
       * Try to preserve the name of the instance.
       */
    NewInstP->type = OCT_INSTANCE;
    NewInstP->contents.instance.name = SubInstP->contents.instance.name;
    if (octGetByName(&gFacet,NewInstP)==OCT_OK) {    /* collision !! */
        do {
            (void) sprintf(uniquename, "%s.%d", SubInstP->contents.instance.name, count++);
            NewInstP->contents.instance.name = uniquename;
        } 
        while (octGetByName(&gFacet,NewInstP)==OCT_OK);
    }
    NewInstP->contents.instance.master  = SubInstP->contents.instance.master;
    NewInstP->contents.instance.view    = SubInstP->contents.instance.view;
    NewInstP->contents.instance.facet    = SubInstP->contents.instance.facet;
    NewInstP->contents.instance.version = SubInstP->contents.instance.version;

    if (debug) {
        (void) fprintf(stderr, "nle:flat: created instance %s of %s:%s\n",
        NewInstP->contents.instance.name,
        NewInstP->contents.instance.master,
        NewInstP->contents.instance.view);
    }
    tr_push(trans);
    tr_add_transform(trans, &(SubInstP->contents.instance.transform), 0);
    tr_get_transform(trans, 
    NewInstP->contents.instance.transform.generalTransform,
    (int32 *) &(NewInstP->contents.instance.transform.translation.x),
    (int32 *) &(NewInstP->contents.instance.transform.translation.y),
    (int *) &(NewInstP->contents.instance.transform.transformType));

    /*
       * find the best name to instantiate with
       */

    ohBestName(&gFacet, SubInstP, NIL(octObject));
    NewInstP->contents.instance.master = SubInstP->contents.instance.master;
    OH_ASSERT(octCreate(&gFacet,NewInstP)); 
    tr_pop(trans);

    (void) nleCopyProperties(SubInstP, NewInstP);
    OH_ASSERT(octInitGenContents(SubInstP, OCT_TERM_MASK, &genATerm));
    while (octGenerate(&genATerm, &ATermOld) == OCT_OK) {
        ATermNew.contents.term.name = ATermOld.contents.term.name;
        if ( octGetByName(NewInstP, &ATermNew) != OCT_OK ) {
	    /* This error is caused by brutal violations of OSP rules, but it happened. */
	    /* This happens if formal terminal are attached to OldInstP !!! */
	    (void)sprintf(buffer,"BUG: inconsistent terminals: %s, %s",
			  ohFormatName( NewInstP) , ohFormatName( &ATermNew ));
	    nleFatalError( buffer );
	}
        nleRecursiveCopyPropAndBags(&ATermOld, &ATermNew);
    }
    return(NLE_OK);
}


static int
FLATprocessFormalTerminals(InstToFlatP,type)
octObject    *InstToFlatP;        /* instance being expanded */
int     type;
{
    octObject ActualTerm;
    octObject FormalTerm;
    octObject subaterm;
    octObject subFormalTerm;
    octObject SubInst;
    octObject inst;
    octGenerator genATerm;
    octObject Geo;
    octObject NewGeo;
    octGenerator genGeo;
    char *ptr;

    /* 
       * Generate all terminals of the instance to be flattened and  check if
       * they are implementing a formal terminal of Chip
       */
    OH_ASSERT(octInitGenContents(InstToFlatP, OCT_TERM_MASK, &genATerm));
    while (octGenerate(&genATerm, &ActualTerm) == OCT_OK) {

        /* If this terminal is not implementing a formal terminal, continue */
        if (octGenFirstContainer(&ActualTerm, OCT_TERM_MASK, &FormalTerm) != OCT_OK) {
            continue;
        }

        /* Find the implementation for ActualTerm in master */
        subFormalTerm.type = OCT_TERM;
        subFormalTerm.contents.term.name = ActualTerm.contents.term.name;
        OH_ASSERT(octGetByName(&MasterContents, &subFormalTerm));
        if (octGenFirstContent(&subFormalTerm, OCT_TERM_MASK, &subaterm)==OCT_OK) {
            SubInst.objectId = subaterm.contents.term.instanceId;
            OH_ASSERT(octGetById(&SubInst));
            if (st_lookup(stInst, (char *) &SubInst.objectId, &ptr)) {
		inst.objectId = *(octId *) ptr;
                OH_ASSERT(octGetById(&inst));
                ActualTerm.type = OCT_TERM;
                ActualTerm.contents.term.name = subaterm.contents.term.name;
                OH_ASSERT(octGetByName(&inst, &ActualTerm));
                OH_ASSERT(octAttach(&FormalTerm, &ActualTerm));
            } 
            else {
                /* We should check for the case in which SubInst is a connector */
                nleWarning("Small screw up with formal terminals"); /* just for debug */
            }
        } 
        else if (type == NLE_PHYSICAL) {
            OH_ASSERT(octInitGenContents(&subFormalTerm, OCT_GEO_MASK, &genGeo));
            while (octGenerate(&genGeo,&Geo)==OCT_OK) {
                if (st_lookup(stGeo, (char *) &Geo.objectId, &ptr)) {
		    NewGeo.objectId = *(octId *) ptr;
                    OH_ASSERT(octGetById(&NewGeo));
                    OH_ASSERT(octAttach(&FormalTerm, &NewGeo));
                } 
                else {
		    printf( "%s in %s contains geo %s\n", ohFormatName( &subFormalTerm),
			   ohFormatName( &MasterContents ),  ohFormatName( &Geo ) );
                    nleFatalError("hashing failed");
                }
            }
        }
    }
    return(NLE_OK);
}


/*
 **  Go through all the terminals in SubNet, find the equivalent terminal in Chip
 **  and attach it to NewNetP
 */
static int
putNetTermsOnChipNet(ChipNetP, SubNetP, type)
octObject *ChipNetP;            /* net to put terms on */
octObject *SubNetP;            /* net where connect info is */
int        type;
{
    octObject SubNetTerm;            /* Term of SubNet */
    octGenerator genNetTerm;

    OH_ASSERT(octInitGenContents(SubNetP, OCT_TERM_MASK, &genNetTerm));
    while (octGenerate(&genNetTerm, &SubNetTerm) == OCT_OK) {
        if (debug) (void) fprintf(stderr, "Putting term %s on net %s\n",
        ohFormatName(&SubNetTerm),
        ohFormatName(ChipNetP));
        putTermOnNet(ChipNetP,&SubNetTerm,type);
    }
    return(NLE_OK);
}


static void
putTermOnNet(ChipNetP, SubTermP, type)
octObject *ChipNetP;            /* net to put terms on */
octObject *SubTermP;            /* net where connect info is */
int        type;
{
    octObject FTermImplementation;
    octObject *TermToAttach;
    octObject ActualTerm;        /* of an inst in Chip */
    octObject Instance;        /* Instance in Chip */
    char *ptr;

    /*
       * Handle both formal terminals and actual terminals
       * Also worry about not attaching connector terminals to the net
       */
    if (octIdIsNull(SubTermP->contents.term.instanceId)) {
        if (octGenFirstContent(SubTermP,OCT_TERM_MASK,&FTermImplementation)==OCT_OK) {
            TermToAttach = &FTermImplementation;
        } 
        else {
            return;        /* Formal term has no implementation, skip it */
        }
    } 
    else {
        TermToAttach = SubTermP;
    }
    if (!octIdIsNull(TermToAttach->contents.term.instanceId)) {
        if (st_lookup(stInst, (char *) &TermToAttach->contents.term.instanceId, &ptr)) {
	    Instance.objectId = *(octId *) ptr;
	    OH_ASSERT(octGetById(&Instance));
            if (octIsAttached(&ChipConnBagTmp,&Instance) == OCT_OK) {
                /* This is a connector terminal. Skip it */
                return;
            }
            ActualTerm.type = OCT_TERM;
            ActualTerm.contents.term.name = TermToAttach->contents.term.name;
            OH_ASSERT(octGetByName(&Instance, &ActualTerm));
            OH_ASSERT(octAttachOnce(ChipNetP, &ActualTerm));
        } 
        else {
            /* Make sure things are right */
            Instance.objectId = TermToAttach->contents.term.instanceId;
            OH_ASSERT(octGetById(&Instance));
            if (type == NLE_SOFT) {
                if (octIsAttached(&SubConnBag,&Instance)!=OCT_OK) {
                    (void) sprintf(buffer,"Term %s of instance %s should have been connected",
                    TermToAttach->contents.term.name,
                    Instance.contents.instance.name);
                    nleFatalError(buffer);
                }
            }
        }
    }
    return;
}

static int
put_terms_on_path(NewPathP, OldPathP)
octObject *NewPathP;            /* path to put terms on */
octObject *OldPathP;            /* path where connect info is */
{
    octObject ActualTerm;
    octObject subAterm;
    octObject Inst, Facet;    /* used only in case of error */
    octGenerator genATerm;
    octObject layer;
    int    termCounter = 0;
    char *ptr;

    /* there must be two terminals on each path */
    ActualTerm.type = OCT_TERM;
    OH_ASSERT(octInitGenContents(OldPathP, OCT_TERM_MASK, &genATerm));
    while (octGenerate(&genATerm, &subAterm) == OCT_OK) {
        termCounter++;
        if (!st_lookup(stInst,(char *) &subAterm.contents.term.instanceId, &ptr)) {
            /* FATAL ERROR: Allow a full report of it */
            layer.type = OCT_LAYER;
            OH_ASSERT(octGenFirstContainer(OldPathP,OCT_LAYER_MASK,&layer));
            Inst.objectId =  subAterm.contents.term.instanceId;
            if (octIdIsNull(Inst.objectId))  {
                (void) sprintf(buffer,"A path on %s contains formal terminal %s",
                layer.contents.layer.name,
                subAterm.contents.term.name);
                nleFatalError(buffer);
            }
            OH_ASSERT(octGetById(&Inst));
            octGetFacet(&Inst,&Facet);
            (void) sprintf(buffer,"A path on %s in %s contains a term %s from instance '%s', which , QUITE PROBABLY is neither a CONNECTOR nor an INSTANCE",
            ohFormatName(&layer),
            ohFormatName(&Facet),
            subAterm.contents.term.name,
            Inst.contents.instance.name);
            nleFatalError(buffer);
        }
        /* else this is an instance terminal */
	Inst.objectId = *(octId *) ptr;
        OH_ASSERT(octGetById(&Inst));
        ActualTerm.contents.term.name = subAterm.contents.term.name;
        OH_ASSERT(octGetByName(&Inst, &ActualTerm));
        OH_ASSERT(octAttach(NewPathP, &ActualTerm));
    }
    if (termCounter != 2) {
        layer.type = OCT_LAYER;
        OH_ASSERT(octGenFirstContainer(OldPathP,OCT_LAYER_MASK,&layer));
        (void) sprintf(buffer,"Found path on %s with %d terminals (2 were expected)",
        layer.contents.layer.name,
        termCounter);
        nleWarning(buffer);
        return(NLE_NOT_OK);
    }
    return(NLE_OK);
}


static void
printArgumentsForFlatInstance(InstToFlatP)
octObject *InstToFlatP;        /* instance to expand */
{
    octObject Chip;

    (void) fprintf(stderr, "Flattening instance %s\n", ohFormatName(InstToFlatP));

    /* XXX BUG octGetFacet */
    OH_ASSERT(octGenFirstContainer(InstToFlatP,OCT_FACET_MASK,&Chip));
    (void) fprintf(stderr, "Working facet is %s\n", ohFormatName(&Chip));
    return;
}
