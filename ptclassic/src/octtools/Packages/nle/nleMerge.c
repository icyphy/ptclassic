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

/*
 ** These routines take a number of instances from Chip and create
 ** a new macro cell containing those instances.
 **
 ** This is accomplished with the following sequence of steps:
 **   1:  Create the new cell and put all the instances in it
 **       At the same time mark all the terminals of all the instances
 **       to be deleted from Chip
 **   2:  Create all the formal terminals of the new cell. Do this by 
 **       checking all the nets, to see if there is any one that crosses
 **       the boundary of the new cell and also by checking the formal
 **       terminals of Chip.  In this step, for every terminal created,
 **       record all the connections that must be made after the new macro
 **       has been instantiated in Chip
 **   3:  Instantiate the new cell in Chip and make all the connections
 **       previously recorded
 **   4:  Delete all old instances, useless nets, and temporary objects
 **   5:  Finish off, by adding all the properies and bags to the new cell
 */

#include "port.h"
#include "utility.h"
#include "errtrap.h"
#include "st.h"
#include "oct.h"
#include "oh.h"
#include "tap.h"
#include "nle.h"
#include "nleCommon.h"
#include "nleMisc.h"
#include "vulcan.h"

static int    debug = 0;        /* Debug flag for this file */

#define ATT_CONTAINER 0
#define ATT_CONTENTS  1
#define DEFAULT_TECHNOLOGY "scmos"

struct s_attach {
    char     *termName;
    octId id;
    int    type;            /* ATT_CONTAINER or ATT_CONTENTS */
    struct s_attach *next;
};

/* >>> GLOBAL VARIABLES */
static st_table *stInst;        /* Hash table from old instances to new */
static st_table *stTerm;        /* Hash table for terminals */

static int    gFormalTermCounter = 0;        /* counter of formal terminals */
static int    gNetCounter = 0;        /* counter of nets */

/* Forward declarations */
static int doMacroNets
	ARGS((int type));
void att_freeList
	ARGS((struct s_attach **list));
void addFloorplanBag
	ARGS((octObject *FacetP));
int promoteInstanceTerm
	ARGS((octObject *InstTermP, octObject *FTermP));
void att_add
	ARGS((struct s_attach **list, char *name, octId id, int type));
void checkArgumentsForMakeMacro
	ARGS((octObject *NewFacetP,octObject *ArgBagP,octObject *NewInstanceP,int type));


/* To record all the attachments that must be done. Filled in pass1 (doMacroNet()) */
struct s_attach *attachmentsToDo = 0;

static char     buffer[4096];    /* generic string, used also for system command */


static octObject Chip;            /* The facet on which we work */
static octObject ChipInterface;        /* The facet on which we work */
static octObject *MacroContentP;     /* Contents facet of new cell */
static octObject MacroInterface;    /* Interface facet of new cell */
static octObject MacroInstBag;        /* OSP 'INSTANCES' BAG */
static octObject MacroConnBag;        /* OSP 'CONNECTOR' BAG */
/*static octObject MacroJoinBag;*/        /* OSP 'JOINEDTERMS' BAG */
static octObject ChipInstBag;        /* OSP 'INSTANCES' BAG */

static octObject DeleteTermsBag;    /*tmp prop of instance-terms that will be deleted */
static octObject netsToRemoveBag;    /*  */

static int    chipInterfaceIsOpen;


static octStatus
get_connector(facet, master, layer1, layer2, w1, w2)
octObject *facet, *master;
char *layer1, *layer2;
int w1, w2;
{
    tapLayerListElement layerlist[2];

    layerlist[0].layer.type = OCT_LAYER;
    layerlist[0].layer.contents.layer.name = layer1;
    OH_ASSERT_DESCR(octGetOrCreate(facet, &(layerlist[0].layer)), "getting the first layer for the connector");
    layerlist[1].layer.type = OCT_LAYER;
    layerlist[1].layer.contents.layer.name = layer2;
    OH_ASSERT_DESCR(octGetOrCreate(facet, &(layerlist[1].layer)), "getting the second layer for the connector");

    layerlist[0].width = w1;
    layerlist[1].width = w2;
    layerlist[0].direction = TAP_HORIZONTAL;
    layerlist[1].direction = TAP_VERTICAL;

    return tapGetConnector(2, layerlist, master);
}


static void
processInterface()
{
    octObject JoinedTermsInterface;
    octObject JoinedInterface;
    octObject Term;
    octObject TermInterface;
    octObject Prop;            /* generic property */
    octGenerator genTerm;
    octGenerator genProp;


    JoinedTermsInterface.type = OCT_BAG;
    JoinedTermsInterface.contents.bag.name = "JOINEDTERMS";
    JoinedInterface.type = OCT_BAG;
    JoinedInterface.contents.bag.name = "JOINED";


    /* FIX up the interface facet now */

    MacroInterface.contents.facet.mode = "a";
    OH_ASSERT(octOpenFacet(&MacroInterface));

    if (debug) (void) fprintf(stderr, "Processing interface\n");

    /* move the JOINED bags to the interface, where they belong */
    if ( octGetByName(MacroContentP,&JoinedTerms) == OCT_OK) {
        if (debug) (void) fprintf(stderr, "Moving Joined Bags to Interface facet\n");
        OH_ASSERT(octGetOrCreate(&MacroInterface,&JoinedTermsInterface));
        while (octGetByName(&JoinedTerms,&Joined)==OCT_OK) {
            OH_ASSERT(octCreate(&JoinedTermsInterface,&JoinedInterface));
            OH_ASSERT(octInitGenContents(&Joined,OCT_TERM_MASK,&genTerm));
            while (octGenerate(&genTerm,&Term)==OCT_OK) {
                TermInterface.type = OCT_TERM;
                TermInterface.contents.term.name = Term.contents.term.name;
                OH_ASSERT(octGetByName(&MacroInterface,&TermInterface));
                OH_ASSERT(octAttach(&JoinedInterface,&TermInterface));
            }
            OH_ASSERT(octDelete(&Joined));
        }
        OH_ASSERT(octDelete(&JoinedTerms));
    }

    /* 
       * Copy all the properties of the terminal in the interface
       */
    OH_ASSERT(octInitGenContents(MacroContentP,OCT_TERM_MASK,&genTerm));
    while(octGenerate(&genTerm,&Term)==OCT_OK) {
        TermInterface.type = OCT_TERM;
        TermInterface.contents.term.name = Term.contents.term.name;
        OH_ASSERT_DESCR(octGetByName(&MacroInterface,&TermInterface),"master/instance");

        OH_ASSERT(octInitGenContents(&Term,OCT_PROP_MASK,&genProp));
        while (octGenerate(&genProp,&Prop)==OCT_OK) {
	    octId id;
	    id = Prop.objectId;
            OH_ASSERT(octCreate(&TermInterface,&Prop)); /* create in interface facet */
	    OH_ASSERT(ohGetById(&Prop, id));
            OH_ASSERT(octDelete(&Prop));        /* delete from contents facet */
        }
    }
    return;
}

int
nleMakeMacro(NewFacetP, ArgBagP, NewInstanceP, type)
octObject    *NewFacetP;
octObject    *ArgBagP;        /* Bag containing all the instances to be merged */
octObject    *NewInstanceP;        /* RETURN:: New instance of the new macro */
int        type;            /* soft or hard */
{
    int        numberOfInstances;    /*  */
    octObject     MacroInst;        /* instance in the new cell */
    octObject     ChipInst;        /* instance in the old cell */
    octObject     MacroInstTerm;        /* generic, in the new cell */
    octObject     MacroFTerm;        /* a formal terminal of the macro cell */
    octObject     MacroNet;        /* a net within the Macro cell */
    octObject     Oterm;            /* generic, in the new cell */
    octGenerator     genInst, genTerm;
    octGenerator     genInstTerm;
    octObject     InstTerm;
    octObject    master;            /* master of the connector */
    octObject     Connector;        /* instance implementing a FTerm in a Macro */
    octObject     ConnTerm;        /* the term of Connector */
    octStatus     acOctInstTermBB();    /* function to get BB of an inst term */
    octStatus     s;

    octObject     Object;
    struct s_attach *att;            /*  */
    extern void    initPlacementObjects();
    octId *id1, *id2;


    if (strcmp(NewFacetP->contents.facet.mode,"r")==0) {
        nleFatalError("The new facet should be writable");
    }

    numberOfInstances = countObjectsInContainer(ArgBagP,OCT_INSTANCE_MASK);
    if (numberOfInstances == 0) {
        nleWarning("No instances in the bag");
        return(NLE_NOT_OK);
    }

    checkArgumentsForMakeMacro(NewFacetP, ArgBagP, NewInstanceP, type);
    initPlacementObjects();

    /* Get the facet we are working on */
    octGetFacet(ArgBagP,&Chip);

    ChipInterface.type = OCT_FACET;
    ChipInterface.contents.facet = Chip.contents.facet;
    ChipInterface.contents.facet.facet = "interface";

    if (octOpenFacet(&ChipInterface) < OCT_OK) {
        octError("Problems opening chip interface");
        nleWarning("Problems with the interface");
        chipInterfaceIsOpen = NLE_FALSE;
    } 
    else {
        chipInterfaceIsOpen = NLE_TRUE;
    }

    ChipInstBag.type = OCT_BAG;
    ChipInstBag.contents.bag.name = "INSTANCES";
    if (octGetByName(&Chip,&ChipInstBag) < OCT_OK) {
        nleFatalError("Can not find the INSTANCES bag in the facet");
    }

    /* init a whole bunch of stuff, open the log file, etc .. */
    MacroContentP = NewFacetP;

    MacroInterface.type = OCT_FACET;
    (MacroInterface.contents.facet = MacroContentP->contents.facet);
    MacroInterface.contents.facet.facet = "interface";


    MacroInstBag.type = OCT_BAG;
    MacroInstBag.contents.bag.name = "INSTANCES";

    MacroConnBag.type = OCT_BAG;
    MacroConnBag.contents.bag.name = "CONNECTORS";

    if (octGetByName(&Chip,&Technology)!=OCT_OK) {
        (void) sprintf(buffer,"TECHNOLOGY prop not found: assuming %s", DEFAULT_TECHNOLOGY);
        nleWarning(buffer);
        Technology.contents.prop.value.string = DEFAULT_TECHNOLOGY;
    }
    EditStyle.contents.prop.value.string = "SYMBOLIC";
    ViewType.contents.prop.value.string = "SYMBOLIC";

    /* create the hash tables */
    stInst  = st_init_table(octIdCmp, octIdHash);
    stTerm  = st_init_table(octIdCmp, octIdHash);

    OH_ASSERT_DESCR(octGetOrCreate(MacroContentP,&MacroInstBag), "create INST bag in macro");
    OH_ASSERT_DESCR(octGetOrCreate(MacroContentP,&MacroConnBag), "create CONN bag in macro");
    OH_ASSERT_DESCR(octGetOrCreate(MacroContentP,&ViewType), "create VIEWTYPE prop");
    OH_ASSERT_DESCR(octGetOrCreate(MacroContentP,&EditStyle), "create EDITSTYLE prop");
    OH_ASSERT_DESCR(octGetOrCreate(MacroContentP,&Technology), "create TECHNOLOGY prop");

    /* Create a bag to contain all actual terminal that have to be deleted from Chip */
    DeleteTermsBag.type = OCT_BAG;
    DeleteTermsBag.contents.bag.name = "TERMS_TO_BE_DELETED";
    OH_ASSERT(octGetOrCreate(&Chip,&DeleteTermsBag));

    /* Copy all the instances in ArgBagP into the new Facet */

    OH_ASSERT(octInitGenContents(ArgBagP, OCT_INSTANCE_MASK, &genInst));
    while (octGenerate(&genInst, &ChipInst) == OCT_OK) {
        MacroInst.type = OCT_INSTANCE;
        MacroInst.contents.instance.name = ChipInst.contents.instance.name;
        MacroInst.contents.instance.master = ChipInst.contents.instance.master;
        MacroInst.contents.instance.view = ChipInst.contents.instance.view;
        MacroInst.contents.instance.facet = ChipInst.contents.instance.facet;
        MacroInst.contents.instance.version = ChipInst.contents.instance.version;
        MacroInst.contents.instance.transform = ChipInst.contents.instance.transform;
        OH_ASSERT(octCreate(MacroContentP, &MacroInst));

	id1 = ALLOC(octId, 1);
	id2 = ALLOC(octId, 2);
	*id1 = ChipInst.objectId;
	*id2 = MacroInst.objectId;
        (void) st_insert(stInst, (char *) id1, (char *) id2);
        /* Attach to instance bag or to connector bag */
        if (octIsAttached(&ChipInstBag,&ChipInst)==OCT_OK) {
            OH_ASSERT(octAttach(&MacroInstBag, &MacroInst));
        } 
        else {
            OH_ASSERT(octAttach(&MacroConnBag, &MacroInst));
        }
        (void) nleCopyProperties(&ChipInst,&MacroInst);

        /* now generate all the terminals of the instances that are to disappear */
	id1 = ALLOC(octId, 1);
	id2 = ALLOC(octId, 2);
        OH_ASSERT(octInitGenContents(&ChipInst, OCT_TERM_MASK, &genTerm));
        while (octGenerate(&genTerm,&Oterm) == OCT_OK) {
            MacroInstTerm.type = OCT_TERM;
            MacroInstTerm.contents.term.name = Oterm.contents.term.name;
            OH_ASSERT(octGetByName(&MacroInst,&MacroInstTerm));
            /*
                   * use the hash table to create a map between octId of terms in Chip
                   * and octId of corresponding terms in Macro
                   */
	    id1 = ALLOC(octId, 1);
	    id2 = ALLOC(octId, 2);
	    *id1 = Oterm.objectId;
	    *id2 = MacroInstTerm.objectId;
            st_insert(stTerm, (char *) id1, (char *) id2);
            /* flag this terminal to be deleted */
            OH_ASSERT(octAttach(&DeleteTermsBag,&Oterm));
        }

/*	{ octObject ChipInterf,
	            joinBag, jBag, mBag,
	            jTerm, mTerm;
	  octGenerator bagGen, tGen;
	  char  *ptr;

	  OH_ASSERT(ohOpenMaster(&ChipInterf, &ChipInst, "interface", "r"));
	  if(ohGetByBagName(&ChipInterf, &joinBag, "JOINEDTERMS") == OCT_OK) {
	    OH_ASSERT(ohGetOrCreateBag(MacroContentP, &MacroJoinBag, "JOINEDTERMS"));
	    OH_ASSERT(octInitGenContents(&joinBag, OCT_BAG_MASK, &bagGen));
	    while (octGenerate(&bagGen, &jBag) == OCT_OK) {
	      OH_ASSERT(ohCreateBag(&MacroJoinBag, &mBag, "JOINED"));
	      OH_ASSERT(octInitGenContents(&jBag, OCT_TERM_MASK, &tGen));
	      while(octGenerate(&tGen, &jTerm) == OCT_OK) {
                st_lookup(stTerm, (char *) &jTerm.objectId, &ptr);
		mTerm.objectId = *(octId *) ptr;
		OH_ASSERT(ohGetById(&mTerm, mTerm.objectId));
		OH_ASSERT(octAttach(&mBag, &mTerm));
	      }
	    }
	  }
	  OH_ASSERT(octCloseFacet(&ChipInterf));
	}
 */
    }


    if (doMacroNets(type)!=NLE_OK) {
        nleWarning("problems creting nets in the new macro");
        return(NLE_NOT_OK);
    }

    /*
       * Now all the terminals of MacroCell have been created
       * we can instantiate the new cell and remove the old ones
       */
    OH_ASSERT(ohCreateInstance(&Chip, NewInstanceP,
			       MacroContentP->contents.facet.cell,
			       MacroContentP->contents.facet.cell,
			       MacroContentP->contents.facet.view,
			       0, 0, OCT_NO_TRANSFORM));

    OH_ASSERT_DESCR(octGetByName(&Chip,&ChipInstBag), "can not get the INST bag");
    OH_ASSERT(octAttach(&ChipInstBag,NewInstanceP));

    /* set up all the new connections */

    att = attachmentsToDo;

    while (att != NULL) {
        if (debug) (void) fprintf(stderr, "Attachement: term %s , to %d\n",
        att->termName, (int)att->id);

        InstTerm.type = OCT_TERM;
        InstTerm.contents.term.name = att->termName;
        OH_ASSERT(octGetByName(NewInstanceP,&InstTerm));
        Object.objectId = att->id;
        OH_ASSERT(octGetById(&Object));
        if (att->type == ATT_CONTENTS) {
            s = octAttachOnce(&Object,&InstTerm) ;
        } 
        else {
            s = octAttachOnce(&InstTerm,&Object);
        }
        if (s == OCT_ALREADY_ATTACHED && debug) nleWarning("already attached");
        OH_ASSERT(s);
        att = att->next;
    }


    /* delete all the old instances from Chip */
    OH_ASSERT(octInitGenContents(ArgBagP, OCT_INSTANCE_MASK, &genInst));
    while (octGenerate(&genInst, &ChipInst) == OCT_OK) {
        OH_ASSERT(octDelete(&ChipInst));
    }

    att_freeList(&attachmentsToDo);

    /* delete all the temporary bags and props */
    OH_ASSERT(octDelete(&DeleteTermsBag));

    /*
       * adjust the new cell and its interface according to the OSP
       * -- get formal terminals and attach them correctly to the nets
       * -- add all the properties that you can to the facet
       */
    if (type == NLE_SOFT) {
        /* generate all terminals of the macro and make them floating  */
        OH_ASSERT(octInitGenContents(MacroContentP, OCT_TERM_MASK, &genTerm));
        while (octGenerate(&genTerm,&MacroFTerm)==OCT_OK) {
            /* XXX BUG!!! OCT_GEN_DONE */
            if (octGenFirstContainer(&MacroFTerm,OCT_NET_MASK,&MacroNet)!=OCT_OK) continue;
            if (octGetByName(&MacroFTerm,&FloatingEdge)!=OCT_OK) continue;

            /* This terminal is floating. give it an implementation */
            /* XXX gets com2.4x4 - technology dependent */
            if (get_connector(&Chip, &master, "MET1", "MET2", 80, 80) < OCT_OK) {
                char sbuffer[2048];

                (void) sprintf(sbuffer, "Can not get the MET1/MET2 connector (%s)",
                octErrorString());
                nleFatalError(sbuffer);
            }

            Connector.contents.instance.name = MacroFTerm.contents.term.name;
            Connector.contents.instance.master = master.contents.facet.cell;
            Connector.contents.instance.view = master.contents.facet.view;
            Connector.contents.instance.facet = master.contents.facet.facet;
            Connector.contents.instance.version = master.contents.facet.version;
            Connector.contents.instance.transform.translation.x = 0;
            Connector.contents.instance.transform.translation.y = 0;
            Connector.contents.instance.transform.transformType = OCT_NO_TRANSFORM;
            OH_ASSERT(octCreate(MacroContentP,&Connector));
            OH_ASSERT(octAttach(&MacroConnBag,&Connector));
            /* get connector terminal */
            OH_ASSERT(octInitGenContents(&Connector,OCT_TERM_MASK,&genInstTerm));
            PlacementClass.contents.prop.value.string = "PAD";
            OH_ASSERT(octCreate(&Connector,&PlacementClass));
            OH_ASSERT(octGenerate(&genInstTerm,&ConnTerm));
            OH_ASSERT(octAttach(&MacroNet,&ConnTerm));
            OH_ASSERT(octAttach(&MacroFTerm,&ConnTerm));
        }
    }

    st_free_table(stInst);
    st_free_table(stTerm);


    /* now do the interface */

    MacroInterface.contents.facet.mode = "w";
    OH_ASSERT(octOpenFacet(&MacroInterface));

    if (debug) (void) fprintf(stderr, "Calling vulcan\n");
    (void) vulcan(MacroContentP,&MacroInterface);

    processInterface();

    if (type == NLE_SOFT) {
        addFloorplanBag(&MacroInterface);
    }

    OH_ASSERT(octFlushFacet(MacroContentP));
    OH_ASSERT(octFlushFacet(&MacroInterface));

    return(NLE_OK);
}


/*
 *  Check all nets, looking for those nets which have terminals
 *  on the instances to be merged
 */
static int doMacroNets(type)
int type;                /* soft or hard */
{
    octObject     ChipTerm;        /*  */
    octObject    MacroInst;
    octObject     MacroInstTerm;        /* generic, in the new cell */
    octObject     MacroFTerm;        /* a formal terminal of the macro cell */
    octObject     FloatMacroFTerm;    /* a formal terminal of the macro cell */
    octObject    InterfaceTerm;
    octObject     MacroNet;        /* a net within the Macro cell */
    octObject     ChipNet;        /* generic, in the new cell */
    octObject    ChipNetType;
    octObject    *TermP;            /* temporary pointer to a terminal */
    octObject     InstTerm;        /*  */
    octObject    MasterTerm;
    octObject     Term;            /*  */
    octGenerator     genTerm, genNet;    /*  */
    int         internalTerms;        /*  */
    int         externalTerms;        /*  */
    char        *internalTermsType;
    char        *internalTermsDirection;
    extern void    OSPFormalTerminal();
    extern void    getMasterTerm();
    char *ptr;

    ChipNetType.type = OCT_PROP;
    ChipNetType.contents.prop.name = "NETTYPE";
    ChipNetType.contents.prop.type = OCT_STRING;

    netsToRemoveBag.type = OCT_BAG;
    netsToRemoveBag.contents.bag.name = "NETS_TO_REMOVE";
    if (octGetByName(&Chip,&netsToRemoveBag)==OCT_OK) {
        /* This is garbage from a previous dirty run of NLE */
        OH_ASSERT(octDelete(&netsToRemoveBag));
    }
    OH_ASSERT(octCreate(&Chip, &netsToRemoveBag));
    /*
     * Generate all the nets and determine if they are internal to the macro cell (HIDDEN)
     * or if they cross the boundary of the new partition
     */
    OH_ASSERT(octInitGenContents(&Chip, OCT_NET_MASK, &genNet));
    while (octGenerate(&genNet, &ChipNet) == OCT_OK) {
        if (octGetByName(&ChipNet,&ChipNetType)!=OCT_OK) {
            ChipNetType.contents.prop.value.string = "SIGNAL"; /* Default */
        }

        internalTerms = externalTerms = 0;
        internalTermsType = NULL_STRING;
        internalTermsDirection = NULL_STRING;
        OH_ASSERT(octInitGenContents(&ChipNet,OCT_TERM_MASK,&genTerm));
        while (octGenerate(&genTerm, &Term) == OCT_OK)  {
            if (!octIdIsNull(Term.contents.term.instanceId)) {
                TermP = &Term;
            } 
            else {            /* This is a formal terminal */
                if (octGenFirstContent(&Term,OCT_TERM_MASK,&InstTerm)!=OCT_OK) {
                    externalTerms++;
                    continue;        /* Formal term has no implementation */
                }
                TermP = &InstTerm;
            }

            if (octIsAttached(&DeleteTermsBag,TermP)==OCT_OK) {
                internalTerms++;
                /* This terminal goes inside the new macro. Determine its type and direction */
                getMasterTerm(TermP,&MasterTerm);
                if (octGetByName(&MasterTerm,&TermType)!=OCT_OK) {
                    TermType.contents.prop.value.string = "SIGNAL"; /* Default */
                }
                if (STRNULL(internalTermsType)) {
                    internalTermsType = strsave(TermType.contents.prop.value.string);
                } 
                else if (strcmp(internalTermsType,TermType.contents.prop.value.string)!=0) {
                    (void) sprintf(buffer,"Type of term %s is %s, but I was expecting %s",
                    TermP->contents.term.name,
                    TermType.contents.prop.value.string, internalTermsType);
                    nleWarning(buffer);
                    internalTermsType = "SIGNAL";    /* The default */
                }

                /*
		 * Direction: If any internal term is an OUTPUT
		 * then the formal terminal will be OUTPUT
		 */
                if (STRNULL(internalTermsDirection) || strcmp(internalTermsDirection,"OUTPUT")!=0){
                    if (octGetByName(&MasterTerm,&Direction)!=OCT_OK) {
                        if (debug) {
                            (void) sprintf(buffer,"DIRECTION property not found on term %s",
                                           MasterTerm.contents.term.name);
                            nleWarning(buffer);
                        }
                    } 
                    else {
                        /* DIRECTION property found */
                        if (STRNULL(internalTermsDirection) || strcmp(Direction.contents.prop.value.string,"OUTPUT")==0) {
                            internalTermsDirection = strsave(Direction.contents.prop.value.string);
                        } 
                        else if (strcmp(internalTermsDirection,Direction.contents.prop.value.string)!=0) {
                            (void) sprintf(buffer,"Direction of term %s is %s, but I was expecting %s",
                            TermP->contents.term.name,
                            Direction.contents.prop.value.string,
                            internalTermsDirection);
                            nleWarning(buffer);
                            internalTermsDirection = "INOUT"; /* the default */
                        }
                    }
                }

            } 
            else {
                externalTerms++;
            }
        }

        /*  */
        MacroNet.type = OCT_NET;
        MacroNet.contents.net.name = ChipNet.contents.net.name;
        MacroNet.contents.net.width = 1;

        if ((externalTerms == 0 && internalTerms > 0) && (type != NLE_FULL)) {
            /*********************************************/
            /*   The net is COMPLETELY INTERNAL          */
            /* the net can be deleted from Chip          */
            /* and must be 'moved' within the Macro cell */
            /*********************************************/
            if (debug) (void) fprintf(stderr, "Creating net %s\n", MacroNet.contents.net.name);
            gNetCounter++;
            OH_ASSERT(octCreate(MacroContentP,&MacroNet));
            OH_ASSERT(octAttach(&netsToRemoveBag, &ChipNet));
            (void) nleCopyProperties(&ChipNet,&MacroNet);

            /* Attach all terminals to the new net */
            OH_ASSERT(octInitGenContents(&ChipNet,OCT_TERM_MASK,&genTerm));
            while (octGenerate(&genTerm, &InstTerm) == OCT_OK)  {
                if (octIdIsNull(InstTerm.contents.term.instanceId)) {
                    if (octGenFirstContent(&InstTerm,OCT_TERM_MASK,&InstTerm)!=OCT_OK) {
                        continue;        /* Formal term %s has no implementation */
                    }
                }
                st_lookup(stTerm, (char *) &InstTerm.objectId, &ptr);
		MacroInstTerm.objectId = *(octId *) ptr;
                OH_ASSERT(octGetById(&MacroInstTerm));
                OH_ASSERT(octAttach(&MacroNet,&MacroInstTerm));
            }
        }   
        else     if (internalTerms == 1) {
            /*********************************************/
            /*   The net is has only ONE internal term   */
            /* if HARD make one formal terminal but no nets */
            /* if SOFT or FULL make one formal terminal AND one net */
            /*********************************************/
            /* Find the internal terminal */
            OH_ASSERT(octInitGenContents(&ChipNet,OCT_TERM_MASK,&genTerm));
            while (octGenerate(&genTerm, &InstTerm) == OCT_OK)  {
                if (octIdIsNull(InstTerm.contents.term.instanceId)) {
                    if (octGenFirstContent(&InstTerm,OCT_TERM_MASK,&InstTerm)!=OCT_OK) continue;
                }
                if (octIsAttached(&DeleteTermsBag,&InstTerm) == OCT_OK) {
                    st_lookup(stTerm, (char *) &InstTerm.objectId, &ptr);
		    MacroInstTerm.objectId = *(octId *) ptr;
                    OH_ASSERT(octGetById(&MacroInstTerm));
                    break;        /* terminal found, exit while loop */
                }
            }

            if ((type == NLE_HARD) || (type == NLE_FULL)) {    /* Create one formal terminal, with the same name as the net */
		octObject termtype, direction, mfterm;
		int master = 0;

                (void) promoteInstanceTerm(&MacroInstTerm,&MacroFTerm);

		/* get the TERMTYPE and DIRECTION properties */
		if (ohGetByPropName(&MacroInstTerm, &termtype, "TERMTYPE") == OCT_NOT_FOUND) {
		    (void) getMasterTerm(&MacroInstTerm, &mfterm);
		    master = 1;
		    if (ohGetByPropName(&mfterm, &termtype, "TERMTYPE") == OCT_NOT_FOUND) {
			termtype.contents.prop.value.string = "SIGNAL";
		    }
		}

		if (ohGetByPropName(&MacroInstTerm, &direction, "DIRECTION") == OCT_NOT_FOUND) {
		    if (!master) {
			(void) getMasterTerm(&MacroInstTerm, &mfterm);
		    }
		    if (ohGetByPropName(&mfterm, &direction, "DIRECTION") == OCT_NOT_FOUND) {
			direction.contents.prop.value.string = "INOUT";
		    }
		}

                OSPFormalTerminal(&MacroFTerm,
				  termtype.contents.prop.value.string,
				  direction.contents.prop.value.string);
                gFormalTermCounter++;
                att_add(&attachmentsToDo, MacroFTerm.contents.term.name,
			ChipNet.objectId,ATT_CONTENTS);
            } 
            else {            /* NLE_SOFT create net and new FT */
                if (debug) (void) fprintf(stderr, "Creating net %s\n", MacroNet.contents.net.name);
                OH_ASSERT(octCreate(MacroContentP,&MacroNet));
                (void) nleCopyProperties(&ChipNet,&MacroNet);
                gNetCounter++;

                OH_ASSERT(octAttach(&MacroNet,&MacroInstTerm));
                FloatMacroFTerm.type = OCT_TERM;
                FloatMacroFTerm.contents.term.instanceId = oct_null_id;
                FloatMacroFTerm.contents.term.width = 1;
                FloatMacroFTerm.contents.term.name =  MacroNet.contents.net.name;
                OH_ASSERT(octCreate(MacroContentP,&FloatMacroFTerm));
                gFormalTermCounter++;
                OSPFormalTerminal(&FloatMacroFTerm,
                internalTermsType,
                internalTermsDirection);
                OH_ASSERT(octAttach(&MacroNet,&FloatMacroFTerm));

                att_add(&attachmentsToDo,FloatMacroFTerm.contents.term.name,ChipNet.objectId,ATT_CONTENTS);
            } 
        } 
        else  if ( internalTerms > 1) {
            /* The net goes across the boundary: we need to create the net and new formal terminal */
            /* The new formal terminal has no implementation if type == NLE_SOFT */
	    if (type == NLE_SOFT) {
		if (debug) (void) fprintf(stderr, "Creating net %s\n", MacroNet.contents.net.name);
		gNetCounter++;
		OH_ASSERT(octCreate(MacroContentP,&MacroNet));
		(void) nleCopyProperties(&ChipNet,&MacroNet);
	    }

            OH_ASSERT(octInitGenContents(&ChipNet,OCT_TERM_MASK,&genTerm));
            while (octGenerate(&genTerm, &InstTerm) == OCT_OK)  {
                if (octIdIsNull(InstTerm.contents.term.instanceId)) {
                    if (octGenFirstContent(&InstTerm,OCT_TERM_MASK,&InstTerm)!=OCT_OK) {
                        continue;        /* Formal term %s has no implementation */
                    }
                }
                if (octIsAttached(&DeleteTermsBag,&InstTerm) == OCT_OK) {
                    st_lookup(stTerm, (char *) &InstTerm.objectId, &ptr);
		    MacroInstTerm.objectId = *(octId *) ptr;
                    OH_ASSERT(octGetById(&MacroInstTerm));
                    MacroInst.objectId = MacroInstTerm.contents.term.instanceId;
                    OH_ASSERT(octGetById(&MacroInst));
                    if (type == NLE_SOFT) {
			OH_ASSERT(octAttach(&MacroNet,&MacroInstTerm));
		    } else {
			octObject termtype, direction, mfterm;
			int master = 0;

                        (void) promoteInstanceTerm(&MacroInstTerm,&MacroFTerm);

			/* get the TERMTYPE and DIRECTION properties */
			if (ohGetByPropName(&MacroInstTerm, &termtype, "TERMTYPE") == OCT_NOT_FOUND) {
			    (void) getMasterTerm(&MacroInstTerm, &mfterm);
			    master = 1;
			    if (ohGetByPropName(&mfterm, &termtype, "TERMTYPE") == OCT_NOT_FOUND) {
				termtype.contents.prop.value.string = "SIGNAL";
			    }
			}

			if (ohGetByPropName(&MacroInstTerm, &direction, "DIRECTION") == OCT_NOT_FOUND) {
			    if (!master) {
				(void) getMasterTerm(&MacroInstTerm, &mfterm);
			    }
			    if (ohGetByPropName(&mfterm, &direction, "DIRECTION") == OCT_NOT_FOUND) {
				direction.contents.prop.value.string = "INOUT";
			    }
			}

			OSPFormalTerminal(&MacroFTerm,
					  termtype.contents.prop.value.string,
					  direction.contents.prop.value.string);
			gFormalTermCounter++;
#ifdef notdef
                        OH_ASSERT(octAttach(&MacroNet,&MacroFTerm));
                        if (octIsAttached(&MacroInstBag,&MacroInst)==OCT_OK) {
                            /* 
                             * If the implementation of the formal terminal is an instance terminal
                             * attach the implementation to the net too
                             */
                            OH_ASSERT(octAttach(&MacroNet,&MacroInstTerm));
                        }
#endif
                        att_add(&attachmentsToDo,MacroFTerm.contents.term.name,ChipNet.objectId,ATT_CONTENTS);
                    } /* endif (type == NLE_SOFT) */
                }
            }

            if (type == NLE_SOFT) {
                /* Create one formal terminal, with the same name as the net */
                FloatMacroFTerm.type = OCT_TERM;
                FloatMacroFTerm.contents.term.instanceId = oct_null_id; /* it's a formal term */
                FloatMacroFTerm.contents.term.width = 1;
                FloatMacroFTerm.contents.term.name = MacroNet.contents.net.name;
                OH_ASSERT(octCreate(MacroContentP,  &FloatMacroFTerm));
                OSPFormalTerminal(&FloatMacroFTerm,
                internalTermsType,
                internalTermsDirection);
                gFormalTermCounter++;
                /*  attach the formal terminal to the net */
                OH_ASSERT(octAttach(&MacroNet,&FloatMacroFTerm));
                /* Remember to connect to the floating terminal */
                att_add(&attachmentsToDo,FloatMacroFTerm.contents.term.name,ChipNet.objectId,ATT_CONTENTS);
            } 
        } 
        else {
            /* No internal terminals */
        }
    }

    /* Check the formal terminals of Chip */

    OH_ASSERT(octInitGenContents(&Chip,OCT_TERM_MASK,&genTerm));
    while (octGenerate(&genTerm,&ChipTerm)==OCT_OK) {
        if (!octIdIsNull(ChipTerm.contents.term.instanceId)) continue;
        if (octGenFirstContent(&ChipTerm,OCT_TERM_MASK,&InstTerm)!=OCT_OK) {
            /* "FTerm %s has no ATerm attached", ChipTerm.contents.term.name */
            continue;            /* while loop */
        }
        /* If the implementation of this fterm is to disappear, we must find, or create a 
             * fterm in MacroContents that will be the new implementation of ChipTerm
             */
        if (octIsAttached(&DeleteTermsBag,&InstTerm) == OCT_OK) {
            st_lookup(stTerm, (char *) &InstTerm.objectId, &ptr);
	    MacroInstTerm.objectId = *(octId *) ptr;
            OH_ASSERT(octGetById(&MacroInstTerm));
            if (octGenFirstContainer(&MacroInstTerm,OCT_TERM_MASK,&MacroFTerm)!=OCT_OK) {
                /*
		 * MacroInstTerm is not attached to a formal terminal of the new macro 
		 * but we need a formal terminal to implement ChipTerm
		 */
                MacroFTerm.type = OCT_TERM;
		MacroFTerm.contents.term.instanceId = OCT_NULL_ID; /* Because it is a formal term. */
                MacroFTerm.contents.term.name = ChipTerm.contents.term.name;
                MacroFTerm.contents.term.width = 1;
                OH_ASSERT(octCreate(MacroContentP,&MacroFTerm));
                gFormalTermCounter++;

                /* determine type and direction of the new term  (if you can) */

                if (chipInterfaceIsOpen) {
                    InterfaceTerm.type = OCT_TERM;
                    InterfaceTerm.contents.term.name = ChipTerm.contents.term.name;
                    OH_ASSERT(octGetByName(&ChipInterface,&InterfaceTerm));
                    if (octGetByName(&InterfaceTerm,&TermType)==OCT_OK) {
                        OSPFormalTerminal(&MacroFTerm,
                        TermType.contents.prop.value.string,
                        (char *) 0);
                    } 
                    else {            /* give it the default value */
                        OSPFormalTerminal(&MacroFTerm,
                        "SIGNAL",
                        (char *) 0);
                    }
                    if (octGetByName(&InterfaceTerm,&Direction)==OCT_OK) {
                        OSPFormalTerminal(&MacroFTerm,
                        Direction.contents.prop.value.string,
                        (char *) 0);
                    }
                } 
                else {
                    OSPFormalTerminal(&MacroFTerm,
                    "SIGNAL",
                    "INOUT"); /* temporary defaults */
                }
                OH_ASSERT(octAttach(&MacroFTerm,&MacroInstTerm));
                (void) nleCopyProperties(&ChipTerm,&MacroFTerm);

            }
            att_add(&attachmentsToDo,MacroFTerm.contents.term.name,ChipTerm.objectId,ATT_CONTENTS);
        }
    }
    if (debug) (void) fprintf(stderr, "Created %d nets and %d formal terminals\n",
    gNetCounter, gFormalTermCounter);

    OH_ASSERT(octInitGenContents(&netsToRemoveBag,OCT_NET_MASK,&genNet));
    while(octGenerate(&genNet,&ChipNet)==OCT_OK) {
        OH_ASSERT(octDelete(&ChipNet));
    }
    OH_ASSERT(octDelete(&netsToRemoveBag));

    return(NLE_OK);
}
int
promoteInstanceTerm(InstTermP,FTermP)
octObject *InstTermP;
octObject *FTermP;
{    
    octObject Instance;
    octObject Facet;
    char         termName[128];
    int         counter = 0;

    Instance.objectId = InstTermP->contents.term.instanceId;
    OH_ASSERT(octGetById(&Instance));

    octGetFacet(&Instance,&Facet);

    FTermP->type = OCT_TERM;
    FTermP->contents.term.instanceId = oct_null_id;    /* it's a formal term */
    FTermP->contents.term.width = 1;

    (void) sprintf(termName, "FT_%s.%s", Instance.contents.instance.name, InstTermP->contents.term.name);
    FTermP->contents.term.name = strsave(termName);
    if (octGetByName(&Facet,FTermP)==OCT_OK) {
        (void) sprintf(buffer,"Name collision: Term %s of %s:%s exists already",
        FTermP->contents.term.name,
        Facet.contents.facet.cell,
        Facet.contents.facet.view);
        nleWarning(buffer);
        /* Find another name */
        do {
            (void) sprintf(termName, "FT_%s.%s.%d",
            Instance.contents.instance.name, InstTermP->contents.term.name, counter++);
            FTermP->contents.term.name = strsave(termName);
        } 
        while (octGetByName(&Facet,FTermP)==OCT_OK);
        (void) sprintf(buffer,"Using alternative name %s",termName);
        nleWarning(buffer);
    }
    OH_ASSERT(octCreate(&Facet,  FTermP));
    OH_ASSERT(octAttach(FTermP, InstTermP));
    return(NLE_OK);
}

void
addFloorplanBag(FacetP)
octObject *FacetP;
{
    struct octBox BBox;
    octCoord  h,w;

    octObject    FloorplanBag;
    octObject    TargetWidth;
    octObject    TargetHeight;
    octObject    MinHeightOverWidth;
    octObject    MaxHeightOverWidth;

    FloorplanBag.type = OCT_BAG;
    FloorplanBag.contents.bag.name = "FLOORPLAN";

    TargetWidth.type = OCT_PROP;
    TargetWidth.contents.prop.name = "TARGETWIDTH";
    TargetWidth.contents.prop.type = OCT_INTEGER;

    TargetHeight.type = OCT_PROP;
    TargetHeight.contents.prop.name = "TARGETHEIGHT";
    TargetHeight.contents.prop.type = OCT_INTEGER;

    MinHeightOverWidth.type = OCT_PROP;
    MinHeightOverWidth.contents.prop.name = "MINHEIGHTOVERWIDTH";
    MinHeightOverWidth.contents.prop.type = OCT_REAL;

    MaxHeightOverWidth.type = OCT_PROP;
    MaxHeightOverWidth.contents.prop.name = "MAXHEIGHTOVERWIDTH";
    MaxHeightOverWidth.contents.prop.type = OCT_REAL;

    OH_ASSERT(octCreate(FacetP,&FloorplanBag));
    if (octBB(FacetP,&BBox) == OCT_OK) {
        h = BBox.upperRight.y - BBox.lowerLeft.y;
        w = BBox.upperRight.x - BBox.lowerLeft.x;
        TargetWidth.contents.prop.value.integer = w / VEM_MIN_FEATURE;
        TargetHeight.contents.prop.value.integer = h / VEM_MIN_FEATURE;
        MinHeightOverWidth.contents.prop.value.real = h / w;
        MaxHeightOverWidth.contents.prop.value.real = h / w;
    }  
    else {
        TargetWidth.contents.prop.value.integer = 10; /* default */
        TargetHeight.contents.prop.value.integer = 10; /* default */
        MinHeightOverWidth.contents.prop.value.real = 0.2;
        MaxHeightOverWidth.contents.prop.value.real = 5;
    }
    OH_ASSERT(octCreate(&FloorplanBag,&TargetWidth));
    OH_ASSERT(octCreate(&FloorplanBag,&MinHeightOverWidth));
    OH_ASSERT(octCreate(&FloorplanBag,&MaxHeightOverWidth));
}


void
att_add(list,name,id,type)
struct s_attach **list;
char *name;
octId id;
int type;
{
    struct s_attach *new;

    new = (struct s_attach *) malloc(sizeof(struct s_attach));
    new->next = *list;
    new->termName = strsave(name);
    new->id = id;
    new->type = type;
    *list = new;
}

void
att_freeList(list)
struct s_attach **list;
{
    while (*list != NULL) {
        free((*list)->termName);
        free((char *) (*list));
        *list = (*list)->next;
    }
}


/*ARGSUSED*/
void
checkArgumentsForMakeMacro(NewFacetP, ArgBagP, NewInstanceP, type)
octObject    *NewFacetP;
octObject    *ArgBagP;        /* Bag containing all the instances to be merged */
octObject    *NewInstanceP;        /* RETURN:: New instance of the new macro */
int        type;            /* soft or hard */
{
    octObject Instance;
    octObject Chip;
    octGenerator genInst;
    octObject Flag;

    Flag.type = OCT_PROP;
    Flag.contents.prop.type = OCT_STRING;
    Flag.contents.prop.value.string = "<not relevant>";
    Flag.contents.prop.name = "FLAG: INSTANCE TO BE MERGED";

    octGetFacet(ArgBagP,&Chip);
    OH_ASSERT(octCreate(&Chip,&Flag));

    if (debug) {
        (void) fprintf(stderr, "Creating MacroCell %s:%s\n",
        NewFacetP->contents.facet.cell,
        NewFacetP->contents.facet.view);
        (void) fprintf(stderr, "The following instances will be merged:\n");
    }
    OH_ASSERT(octInitGenContents(ArgBagP,OCT_INSTANCE_MASK,&genInst));
    while (octGenerate(&genInst,&Instance)==OCT_OK) {
        if (octGetByName(&Instance,&Flag)==OCT_OK) {
            (void) sprintf(buffer,"Instance %s contained more than once in `merge bag'",
            Instance.contents.instance.name);
            nleFatalError(buffer);

        } 
        else {
            OH_ASSERT(octAttach(&Instance,&Flag));
        }
        if (debug) (void) fprintf(stderr, "\t%s of %s:%s\n",
        Instance.contents.instance.name,
        Instance.contents.instance.master,
        Instance.contents.instance.view);
    }

    if (debug) {
        (void) fprintf(stderr, "Type = %s\n", (type == NLE_HARD) ? "HARD" : "SOFT");
        (void) fprintf(stderr, "Working facet is:  %s:%s\n",
        Chip.contents.facet.cell,
        Chip.contents.facet.view);
    }
    OH_ASSERT(octDelete(&Flag));
}


