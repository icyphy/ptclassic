defstar{
    name { DynBlockBase }
    domain { DE }
    author { J. Voigt }
    version { 1.1 11/19/97 
    }   
    copyright { copyright (c) 1997 Dresden University of Technology
                WiNeS-Project  }
    desc { Creates and deletes blocks dynamically. It has to be connected 
    to a DynMerge-Star and/or a DynFork-Star in order to get and release all 
    the PortHoles needed for the dynamically created blocks.
    }
    explanation {
This is the base star for all stars which support a mutable system 
configuration. It is an abstract class, so it can't be instantiated. See the 
descriptions of its childs for further explanation.

The current limitations are:
A block which is to instantiate must not have multiportholes.

Creating a block dynamically is as follows:

Using some source code from "HOFBase::createBlock" I get a pointer to a newly created block. This block is instantiated and put in a list. The input 
and output portholes of that newly created block are looked for and  
approriate farsides for these portholes are created in "DEDynMerge" and 
"DEDynFork" using their "createPortHole()"-method. These portholes are 
connected afterwards. Once they are connected, the newly created portholes
are initialized. If "parameter_map" is not empty, the parameters in the new 
block are set. If it is empty, the "initialization()"-method sets the default 
values. The entire galaxy's connectivity is checked. The next actions here 
are checking for delayfreeloops and updating the porthole priorities. We have 
to take special care of source-stars in the dynamically created blocks. If 
there are any, we have to give them an inital event at the current time. That 
means, all source-stars derived from DERepeatStar give now out an initial event at the time of their creation (instead of time zero, when they don't exist).
Otherwise these source-stars would never give out any event. See the description
of DERepeatStar for details.

Deleting a block dynamically is as follows:

I get a unique identifier for a block to be deleted. This block is looked for 
in my list of currently existing blocks. I get a pointer to my deleteBlock from the list and remove that block from the list. Then, the eventqueue is checked 
for any events pointing to deleteblock. If any events are found, they are 
deleted from the eventqueue. Then I look for input and output of the 
deleteblock. I again check the eventqueue for any events which are scheduled 
to the farside of the deleteblock's output. Then I disconnect the portholes 
from their farside and delete their farside using "removePortHole()" in 
"DEDynMerge" and "DEDynFork", respectively. Next, I remove deleteblock from 
the galaxy's blocklist, where it was added to by "createBlock". 
Once removed there, I can delete it. The entire galaxy's connectivity is 
checked. The last actions here are checking for delayfreeloops and updating 
the porthole priorities.

Scanning the eventqueue is as follows:

Using "scanEventList()" and "scanGalaxyForContents()" I go down the block's 
hierarchy until I only deal with atomic elements ("stars"). For each star I 
check the current eventQueue for any pending pointer to it (CQScheduler) or for any pending pointer to any of its input-portholes (DEScheduler) using 
"deletePendingEventsOfStar", which should be self-exploratory.
}
	acknowledge { I use some code from Edward A. Lee's older version of HOFBase.pl. Former versions of that star called that code directly. But, is was removed there by Tom Lane. So, I copied the older code directly into this file. }
    location { $PTOLEMY/src/domains/de/contrib/stars }
    hinclude { "checkConnect.h", "CQScheduler.h", "DEScheduler.h", "DERepeatStar.h", "DEDynMerge.h", "KnownBlock.h", "InfString.h" }
    ccinclude { "ptk.h" }
    defstate {
        name { blockname }
        type { string }
        default { "" }
        desc { The name of the block which 'll be instantiated. }
    }
    defstate {
        name { where_defined }
        type { string }
        default { "" }
        desc {
            The full path and facet name for the definition of blockname.
        }
    }
    defstate {
        name { output_map }
        type { stringarray }
        default { "" }
        desc { The names of the block's output
               FIXME: multiports are not supported 
        }
    }
    defstate {
        name { input_map }
        type { stringarray }
        default { "" }
        desc { The names of the block's input
               FIXME: multiports are not supported 
        }
    }
    defstate {
        name { parameter_map }
        type { stringarray }
        default { "" }
        desc { The mapping of parameters. }
    }
    protected {
        Galaxy* mom;
        DEBaseSched *myScheduler_p;
        blockCounter *list_h;
        static int nameCounter;
        StringList name;
    }
    private { 
        PortHole *po;
        blockCounter *blockCounter_p, *list_t;
        Block *delBlock_p;
        DEScheduler *myDEScheduler_p;
        CQScheduler *myCQScheduler_p;
    }
    header {
        struct blockCounter {
            int blockName;
            Block *myBlock_p;
            blockCounter *next;
        };
    }
    code {
        int DEDynBlockBase::nameCounter = 0;
    }
    constructor { 
        list_h = list_t =  blockCounter_p = NULL;
        delBlock_p = NULL;
        
    }
    // The following two methods are copied from an older version of HOFBase.
    // They have been removed in the HOF-stars, at least in that form. So,
    // I decided to copy them here having the advantage of being completely
    // independent from HOF now.

    // This method returns the generic port at the top level
    // of the alias chain for the given porthole.  In other
    // words, given a star porthole, it will find the highest
    // galaxy porthole aliased to the star porthole.
    method {
        name { findTopGenericPort }
        type { "GenericPort*" }
        access { protected }
        arglist { "(PortHole *ph)" }
        code {
            MultiPortHole *mph;
            GenericPort *gp, *gpt;
            if ( (mph = ph->getMyMultiPortHole()) ) {
                gp = mph;
            } else {
                gp = ph;
            }
            // Get the top-level port that we are connected to
	    while ( (gpt = gp->aliasFrom()) ) {
	      gp = gpt;
	    }
	    // If the multiporthole did not have an aliasFrom
	    // porthole, it could still be that the original
	    // porthole did.
	    if (gp == mph) {
	      gp = ph;
	      while ( (gpt = gp->aliasFrom()) ) {
		gp = gpt;
	      }
	      // If the original porthole also did not have an aliasFrom,
	      // then make sure to return a pointer to its mph.
	      if (gp == ph) gp = mph;
	    }
	    return gp;
        }
    }
    method {
        name { connectError }
        type { "void" }
        access { protected }
        arglist { "()" }
        code {
            Error::abortRun(*this,
            "Star is not fully connected");
        }
    }
    // The portholes are in derived stars only. 
    pure virtual method {
        name { connectInputs }
        type { void }
        arglist { "(Block*, int)" }
        access { protected }
    }
    pure virtual method {
        name { connectOutputs }
        type { void }
        arglist { "(Block*)" }
        access { protected }
    }
    pure virtual method {
        name { disconnectInputs }
        type { void }
        arglist { "(Block*)" }
        access { protected }
    }
    pure virtual method {
        name { disconnectOutputs }
        type { void }
        arglist { "(Block*)" }
        access { protected }
    }
    method {
        name { myParent }
        type { "Galaxy*" }
        access { protected }
        code {
            // Identify the parent, which is the galaxy this star belongs to
            Block *dad = parent();
            if (!dad || dad->isItAtomic()) {
                Error::abortRun(*this, "No parent!");
                return NULL;
            }
            return &(dad->asGalaxy());
        }
    }
    method {
        name { connectPorts }
        type { void }
        access { protected }
        arglist { "(PortHole* dest, PortHole* source)" }
        code {
            // connect two portholes

            // Do I really have to deal with aliases here ?
            // *source, which is the PortHole of the new block, is aliased 
            // from a GalPortHole. This GalPortHole ist aliased to *source.
            // This means, when a particle is sent to GalPortHole, it is
            // automatically forwarded to *source. 
            // bottomline: I think that dealing with aliases isn't 
            // necessary here.

            // GenericPort *genPort_p = myBase.aliasPointingAtThis(source);
            int numdelays = dest->numInitDelays();
            const char* initDelayVals = dest->initDelayValues();
            source->connect(*dest, numdelays, initDelayVals);

            // myBase.fixAllAliases(genPort_p, dest, (GenericPort *) source);
            return;
        }
    } 
    method {
        name { deleteDynBlock }
        type { void }
        access { protected }
        arglist { "(int delName)" }
        code {
            // get the block from the list and remove it from the list
            // first check whether or not the list exists
            if (!list_t) {
                Error::abortRun(*this,"No DynBlock exists, so I can't 
                delete one");
                return;
            }
            else {
                // there is a list
                if ((list_h->next == NULL) && 
                (list_h->blockName == delName)) {    
                    // delete the only member
                    delBlock_p = list_h->myBlock_p;
                    blockCounter *deleteBlockCounter_p;
                    deleteBlockCounter_p = list_h;
                    list_h = NULL;
                    list_t = NULL;
                    delete deleteBlockCounter_p;
                }
                if ((list_t != NULL) && (list_h->next != NULL)) {
                    // list has at least two members
                    for (blockCounter_p = list_h; 
                    blockCounter_p->next != NULL; 
                    blockCounter_p = blockCounter_p->next) {
                        if ((blockCounter_p == list_h) && 
                        (blockCounter_p->blockName == delName)) {
                            // delete first member
                            list_h = blockCounter_p->next;   
                            delBlock_p = blockCounter_p->myBlock_p;
                            delete blockCounter_p;
                            break;
                        }			
                        if (blockCounter_p->next->blockName == delName) {
                            blockCounter *deleteBlockCounter_p;
                            deleteBlockCounter_p = blockCounter_p->next;
                            delBlock_p = deleteBlockCounter_p->myBlock_p;
                            if (blockCounter_p->next->next == NULL) {
                                // delete last member	
                                delete deleteBlockCounter_p;
                                list_t = blockCounter_p;
                                list_t->next = NULL;
                                break;
                            }
                            else {
                                // delete a member in the middle of others
                                blockCounter_p->next = 
                                blockCounter_p->next->next;
                                delete deleteBlockCounter_p;
                                break;
                            }				
                        }
                    }
                }
                // First, we have to scan the global eventlist for any 
                // events pointing to any porthole of *delBlock_p, since
                // we have to avoid any dangling pointer from being 
                // left around.
                // Normally, this can only happen, if *delBlock_p is a 
                // galaxy and the stars inside this galaxy fire each other.
                // Firings from outside should be caught in DEDynFork.
                scanEventList(delBlock_p);
                
                // We have to disconnect the Portholes.
                // let's start with the outputs
                // we have to call our child, because it has the portholes
                disconnectOutputs(delBlock_p);
              
                // let's go on with the inputs
                // we have to call our child, because it has the portholes
                disconnectInputs(delBlock_p);

                // remove the block from mom's list
                // attention: it won't be deleted after this line !!!
                if (!mom->removeBlock(*delBlock_p)) {
                    Error::abortRun(*this,
                    "DynBlock is not removed from mom's list");
                    return;
                }
  
                // once removed, it can be recycled
                delete delBlock_p;
      
                // check connectivity
                if (warnIfNotConnected(*mom)) {
                    Error::abortRun(*this,
                    "connectivity error after deleting of a DynBlock");
                    return;
                }
                
                // we have to detect any delay free loop and 
                // to set the depth of the PortHoles
                if (((myDEScheduler_p != NULL) && 
                (!myDEScheduler_p->checkDelayFreeLoop())) || 
                ((myCQScheduler_p != NULL) && 
                (!myCQScheduler_p->checkDelayFreeLoop()))) {
                    Error::abortRun(*this,
                    "DelayFreeLoop after deletion of a dynamic block detected");
                    return;
                }

                // the porthole priorities (depth) have to be updated
                if (((myDEScheduler_p != NULL) && 
                (!myDEScheduler_p->computeDepth())) || 
                ((myCQScheduler_p != NULL) && 
                (!myCQScheduler_p->computeDepth()))) {
                    Error::abortRun(*this,
                  "Error in computing depth after deletion of a dynamic block");
                    return;
                }
            }
            return;
        }  
    }
    method {
        name { setParameter }
        type { void }
        access { private }
        arglist { "(Block *block_p)" }
        code {
            for ( int i = 0; i < parameter_map.size()-1; i++ ) {
                const char *name = parameter_map[i++];
                const char *value = parameter_map[i];
                block_p->setState(name, value);
            }
            return;
        }
    }
    // we override this method for the graphical version
    virtual method {
        name { createBlock }
        type { "Block *" }
        access { protected }
        code {
            Block *block_p;                  
            // the following code is copied from HOFBase::createBlock
            // Create the star and install in the galaxy
            if (!KnownBlock::find(blockname, mom->domain())) {

                // Failed to find star master.  Try compiling the
                // facet specified in the where_defined argument
                InfString command = "ptkOpenFacet ";
                command += where_defined;
                if(Tcl_GlobalEval(ptkInterp, (char*)command) == TCL_ERROR) {
                    Error::abortRun(*this,
                    "Unable to open the where_defined facet: ",
                    where_defined);
                    return NULL;
                }
                command = "ptkCompile ";
                command += ptkInterp->result;
                if(Tcl_GlobalEval(ptkInterp, (char*)command) == TCL_ERROR) {
                    Error::abortRun(*this,
                    "Unable to compile the where_defined facet: ",
                    where_defined);
                    return NULL;
                }
            }
            // Should now have a master of the star -- clone it
            block_p = KnownBlock::clone(blockname, mom->domain());
            if (!block_p) {
                Error::abortRun(*this,
                "Unable to create an instance of block: ",
                blockname);
                return NULL;
            }
            return block_p;
        }
    }
    method {
        name { createDynBlock }
        type { void }
        access { protected }
        arglist { "(int number)" }
        code {
            // createBlock 
            Block *block_p = createBlock();
            if (!block_p) {
                Error::abortRun(*this, "I didn't get a new block to connect");
                return;
            }
            
            // once we have a clone, we need to perform some tasks from
            // HOFBase::createBlock

            // first, we might need to set the target for that block
            // I'm not quite sure whether this is necessary, clone() should 
            // do it. But, in DE we just have one target, so we can't make 
            // anything wrong here while setting it again.
            if (target()) block_p->setTarget(target());

            // the new block needs a unique name, use the static "count" for it
            name = "Dynamic_";
            name += (const char *) block_p->name();
            name += "_";
            name += nameCounter++;
            const char *instance = hashstring(name);

            // add this block to mom's list
            mom->addBlock(*block_p, instance);

            // put it in our list
            blockCounter_p = new blockCounter;
            blockCounter_p->blockName = number;
            blockCounter_p->myBlock_p = block_p;
            if (!list_h) list_h = blockCounter_p;
            else list_t->next = blockCounter_p;
            list_t = blockCounter_p;
            list_t->next = NULL;

            // we have to connect all of the new block's portholes 
            // let's start with the outputs
            // we have to call our child, because it has the portholes
            connectOutputs(block_p);
              
            // let's go on with the inputs
            // we have to call our child, because it has the portholes
            connectInputs(block_p, number);

            // the ports are done, set its parameters now
            // if parameter_map is empty, but the block has parameters,
            // then their default values are set during initialization
            setParameter(block_p);

            // the block is fully connected now, initialize it
            if (!Scheduler::haltRequested()) block_p->initialize();
            if (Scheduler::haltRequested()) return;

            // check connectivity
            if (warnIfNotConnected(*mom)) {
                Error::abortRun(*this, "the new DynPortHole is not connected");
                return;
            }
                
            // we have to detect any delay free loop and 
            // to set the PortHole depths
            if (((myDEScheduler_p != NULL) && 
            !(myDEScheduler_p->checkDelayFreeLoop())) || 
            ((myCQScheduler_p != NULL) && 
            !(myCQScheduler_p->checkDelayFreeLoop()))) {
                Error::abortRun(*this,
               "DelayFreeLoop after instantiation of a dynamic block detected");
                return;
            }
            
            // the porthole priorities (depth) have to be updated
            if (((myDEScheduler_p != NULL) && 
            (!myDEScheduler_p->computeDepth())) || 
            ((myCQScheduler_p != NULL) && 
            (!myCQScheduler_p->computeDepth()))) {
                Error::abortRun(*this,
             "Error in computing depth after instantiation of a dynamic block");
                return;
            }
            
            // We have to take care of our source stars. A source-star which is
            // derived from DERepeatStar is fired first during DERepeat::begin()
            // We have to call an appropriate method for all source-stars in our
            // newly created block now explicitely. Otherwise, no source-star 
            // in our block would give any event out ever. In other words, we 
            // have to synchronize the global time and the local time in the 
            // newly created block.
            // I put DERepeatStar::start() as a new method into DERepeatStar.
            // Moreover, somebody forgot to implement isA(DERepeatStar). I 
            // fixed this.
            if (!Scheduler::haltRequested()) {
                if (block_p->isItAtomic()) {
                    // our block is just a star, that's easy to deal with
                    if (block_p->isA("DERepeatStar")) {
                        DERepeatStar& mySource = (DERepeatStar&) block_p->asStar();
                        DERepeatStar *mySource_p = &mySource;
                        mySource_p->arrivalTime = myScheduler_p->now();
                        mySource_p->completionTime = myScheduler_p->now();
                        mySource_p->start(myScheduler_p->now());
                    }
                }
                else {
                    // it is a galaxy
                    Galaxy& myGalaxy = block_p->asGalaxy();
                    scanGalaxyForSources(myGalaxy);
                }
            }
            return;
        }
    }
    method { 
        name { scanGalaxyForSources }
        type { void }
        access { protected }
        arglist { "(Galaxy& myGalaxy)" }
        code {
            GalTopBlockIter next(myGalaxy);
            Block *myBlock_p;
            while ((myBlock_p = next++) != NULL) {
                if (myBlock_p->isItAtomic()) {
                    // Block is atomic -> must be a star
                    // is it a RepeatStar?
                    if (myBlock_p->isA("DERepeatStar")) {
                        // it is
                        DERepeatStar& mySource = (DERepeatStar&) myBlock_p->asStar();
                        DERepeatStar *mySource_p = &mySource;
                        mySource_p->arrivalTime = myScheduler_p->now();
                        mySource_p->completionTime = myScheduler_p->now();
                        mySource_p->start(myScheduler_p->now());
                    }
                }
                else {
                    // this galaxy contains another galaxy  
                    Galaxy& myGalaxy = myBlock_p->asGalaxy();
                    scanGalaxyForSources(myGalaxy);
                }
            }
        }
    }
    method { 
        name { scanGalaxyForBlockWithName }
        type { "Block *" }
        access { protected }
        arglist { "(Galaxy& myGalaxy, const char *itsName)" }
        code {
            GalTopBlockIter next(myGalaxy);
            Block *myBlock_p, *return_p;
            return_p = NULL;
            while ((myBlock_p = next++) != NULL) {
                if (myBlock_p->isItAtomic()) {
                    // Block is atomic -> must be a star
                    if (myBlock_p->isA(itsName)) {
                        // it is, return a pointer to it
                        return myBlock_p;
                    }
                }
                else {
                    // this galaxy contains another galaxy  
                    Galaxy& myGalaxy = myBlock_p->asGalaxy();
                    return_p = scanGalaxyForBlockWithName(myGalaxy, itsName);
                    // if the return_p != Null, we've already found what we are
                    // looking for
                    if (return_p) return return_p;
                }
            }
            // we can only come to this point if an atomic block with "itsName"
            // is not in myGalaxy. Then we return a Null-pointer
            return return_p;
        }
    }
    method {
        name { deletePendingEventsOfStar }
        type { void }
        access { private }
        arglist { "(Star *myStar_p)" }
        code {
            // we have "read" access to the block's private PortHoleList 
            // using the Iterator
            BlockPortIter next(*myStar_p);
            PortHole *myPortHole_p;
            // again, we have to deal with two different schedulers in DE
            if (myDEScheduler_p) {
                // we use the simple DEscheduler
                // for all PortHoles of that Star
                while ((myPortHole_p = next++) != NULL) {
                    // there's usually no event fired to an output
                    if (myPortHole_p->isItInput()) {
                        // scan EventList for DEScheduler
                        // reset the reference pointer
                        myDEScheduler_p->eventQ.reset();
                        // get the list's length
                        int numberOfEvents = myDEScheduler_p->eventQ.length();
                        // check all pending events
                        while ( numberOfEvents ) {
                            numberOfEvents--;
                            // get the next entry
                            LevelLink *myLL_p = myDEScheduler_p->eventQ.next();
                            // look for its Destination
                            // e is void Pointer*,
                            // so I have to do this in an extra line
                            Event *event_p = (Event *)myLL_p->e;
                            PortHole *dest_p = event_p->dest;
                            // if it points to myPortHole...
                            if ( dest_p == myPortHole_p ) {
                                // ... extract it from the list
                                myDEScheduler_p->eventQ.extract(myLL_p);  
                            }
                        }
                    }
                }
            }
            else if (myCQScheduler_p) {
                // we use the CQ Scheduler
                // CQLevelLink stores the destination star, so we don't need
                // to scan the PortHoleList here
                // BTW, we won't have any problems with process-stars neither
                
                // how long is the current eventlist ?
                int numberOfEvents = myCQScheduler_p->eventQ.length();
                CqLevelLink *store = NULL;
                //  scan all events
                while ( numberOfEvents ) {
                    numberOfEvents--;
                    // get the first entry from the list
                    CqLevelLink *cqll_p = myCQScheduler_p->eventQ.get();
                    // for which star is that entry ?
                    Star *thatStar_p = cqll_p->dest;
                    // if this is the star under consideration ...
                    if ( myStar_p == thatStar_p ) {
                        // ... put it to the free links
                        myCQScheduler_p->eventQ.putFreeLink(cqll_p);
                    }  
                    else {
                        // check the next entry
                        cqll_p->next = store;
                        store = cqll_p;
                    }
                }
                // put all still needed entries back into the queue
                while (store != NULL) {
                    CqLevelLink *temp = store;
                    store = (CqLevelLink *)store->next;
                    myCQScheduler_p->eventQ.pushBack(temp);
                }
            } 
            return;
        }
    }
    method {
        name { deletePendingEventsOfPortHole }
        type { void }
        access { protected }
        arglist { "(PortHole *myPortHole_p)" }
        code {
            // again, we have to deal with two different schedulers in DE
            if (myDEScheduler_p) {
                // we use the simple DEscheduler
                myDEScheduler_p->eventQ.reset();
                // get the list's length
                int numberOfEvents = myDEScheduler_p->eventQ.length();
                // check all pending events
                while ( numberOfEvents ) {
                    numberOfEvents--;
                    // get the next entry
                    LevelLink *myLL_p = myDEScheduler_p->eventQ.next();
                    // look for its Destination
                    // e is void Pointer*,
                    // so I have to do this in an extra line
                    Event *event_p = (Event *)myLL_p->e;
                    PortHole *dest_p = event_p->dest;
                    // if it points to myPortHole...
                    if ( dest_p == myPortHole_p ) {
                        // ... extract it from the list
                        myDEScheduler_p->eventQ.extract(myLL_p);  
                    }
                }
            }
            else if (myCQScheduler_p) {
                // we use the CQ Scheduler
                // Now, I have to check single portholes, not just stars as 
                // destination

                // how long is the current eventlist ?
                int numberOfEvents = myCQScheduler_p->eventQ.length();
                CqLevelLink *store = NULL;
                // we need the star our porthole belongs to
                Star* myStar_p = (Star *)myPortHole_p->parent();
                //  scan all events
                while ( numberOfEvents ) {
                    numberOfEvents--;
                    // get the first entry from the list
                    CqLevelLink *cqll_p = myCQScheduler_p->eventQ.get();
                    // for which star is that entry ?
                    Star *thatStar_p = cqll_p->dest;
                    // if this is the star under consideration ...
                    if ( myStar_p == thatStar_p ) {
                        // then we have to check the destination porthole
                        // e is void Pointer*,
                        // so I have to do this in an extra line
                        Event *event_p = (Event *)cqll_p->e;
                        PortHole *dest_p = event_p->dest;
                        // if it points to myPortHole...
                        if ( dest_p == myPortHole_p ) {
                            // ... put it to the free links
                            myCQScheduler_p->eventQ.putFreeLink(cqll_p);
                        }  
                        else {
                            // check the next entry
                            cqll_p->next = store;
                            store = cqll_p;
                        }
                    }
                    else {
                        // check the next entry
                        cqll_p->next = store;
                        store = cqll_p;
                    }
                }
                // put all still needed entries back into the queue
                while (store != NULL) {
                    CqLevelLink *temp = store;
                    store = (CqLevelLink *)store->next;
                    myCQScheduler_p->eventQ.pushBack(temp);
                }
            } 
            return;
        }
    }
    method {
        name { scanGalaxyForContents }
        type { void }
        access { private }
        arglist { "(Galaxy& myGalaxy)" }
        code {
            GalTopBlockIter next(myGalaxy);
            Block *myBlock_p;
            while ((myBlock_p = next++) != NULL) {
                if (myBlock_p->isItAtomic()) {
                    // Block is atomic -> must be a star
                    // delete all pending events to that star
                    Star& myStar = myBlock_p->asStar();
                    deletePendingEventsOfStar(&myStar);
                }
                else {
                    // this galaxy contains another galaxy  
                    Galaxy& myGalaxy = myBlock_p->asGalaxy();
                    scanGalaxyForContents(myGalaxy);
                }
            }
            return;
        }
    }
    method {
        name { clearList }
        type { void }
        access { protected }
        code {
            while ( list_h ) {
                blockCounter_p = list_h;
                list_h = list_h->next;
                // get the delBlock
                delBlock_p = blockCounter_p->myBlock_p;
                // delete it from the list
                delete blockCounter_p;
                // disconnect and delete its output-portholes
                disconnectOutputs(delBlock_p);
                // now, we disconnect and delete its input-portholes
                disconnectInputs(delBlock_p);
                // remove the block from mom's list, it won't be deleted 
                // after this line !!!
                if (!mom->removeBlock(*delBlock_p)) {
                    Error::abortRun(*this,
                    "DynBlock is not removed from mom's list");
                    return;
                }
                // once removed, it can be deleted
                delete delBlock_p;
            }
            return;
        }
    }
    method {
        name { deleteList }
        type { void }
        access { protected }
        code {
            // A "hard" version of clearList().
            // This version should be used in destructors only. The problem is
            // that one never knows which destructor is being invoked first...
            while ( list_h ) {
                blockCounter_p = list_h;
                list_h = list_h->next;
                // get the delBlock
                delBlock_p = blockCounter_p->myBlock_p;
                // delete it from the list
                delete blockCounter_p;
                // remove the block from mom's list, it won't be deleted 
                // after this line !!!
                if (!mom->removeBlock(*delBlock_p)) {
                    Error::abortRun(*this,
                    "DynBlock is not removed from mom's list");
                    return;
                }
                // once removed, it can be deleted
                delete delBlock_p;
            }
            return;
        }
    }
    method {
        name { scanEventList }
        type { void }
        access { private }
        arglist { "(Block *myBlock_p)" }
        code {
            if (myBlock_p->isItAtomic()) {
                // Block is atomic -> must be a star
                // delete all pending events to that star
                Star& myStar = myBlock_p->asStar();
                deletePendingEventsOfStar(&myStar);
            }
            else {
                // Block contains more than one Block -> must be a galaxy
                Galaxy& myGalaxy = myBlock_p->asGalaxy();
                scanGalaxyForContents(myGalaxy);
            }
            return;
        }
    }
    setup {
        // need a pointer to this star's parent
        mom = myParent();
                
        // we need a pointer to the current scheduler
        // scheduler() is recursively called untill a block with a scheduler
        // is reached
        // At this time (0.7) we have two different types of DE-scheduler.
        // We have to check which one is currently used and have to use a 
        // correct pointer. The other pointer is set to NULL for safety 
        // reasons.
        myDEScheduler_p = NULL;
        myCQScheduler_p = NULL;
        myScheduler_p = (DEBaseSched *)scheduler();
        if ( myScheduler_p->isA("DEScheduler") ) {
            myDEScheduler_p = (DEScheduler *)scheduler();
        }
        else {
            if  ( myScheduler_p->isA("CQScheduler") ) {
                myCQScheduler_p = (CQScheduler *)scheduler();
            }
            else {
                Error::abortRun(*this,
                "Mr. Ptolemy doesn't know this type of DEScheduler");
                return;
            }
        }
    }	
}   

