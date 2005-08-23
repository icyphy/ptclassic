static const char file_id[] = "DEDynMapBase.pl";
// .cc file generated from DEDynMapBase.pl by ptlang
/*
copyright (c) 1997 Dresden University of Technology, WiNeS-Project              All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEDynMapBase.h"
#include "ptk.h"

const char *star_nm_DEDynMapBase = "DEDynMapBase";

const char* DEDynMapBase :: className() const {return star_nm_DEDynMapBase;}

ISA_FUNC(DEDynMapBase,DEStar);

# line 175 "DEDynMapBase.pl"
int DEDynMapBase::nameCounter = 0;

DEDynMapBase::DEDynMapBase ()
{
	setDescriptor("This is the base star for a family of stars which supports a mutable \nsystem configuration in the DE-domain. This star family implements the \nfunctionality of a dynamic map-function for the discrete-event domain. A dynamic\n map-function works like the map-function in the HOF-domain, but we can change \nthe number of blocks it multiplies during runtime of the simulation. Thus, we \nsupport a mutable system configuration. Note, that this star family only works with the two standard DE schedulers.\n<p>\nThis class is an abstract class, so it can't be instantiated and can not be used on its own. It contains methods which are used by its childs.");
	addState(blockname.setState("blockname",this,"","The name of the block which 'll be instantiated."));
	addState(where_defined.setState("where_defined",this,"","The full path and facet name for the definition of blockname."));
	addState(output_map.setState("output_map",this,"","The names of the block's outputs for mapping."));
	addState(input_map.setState("input_map",this,"","The names of the block's inputs for mapping."));
	addState(parameter_map.setState("parameter_map",this,"","The mapping of parameters."));

# line 178 "DEDynMapBase.pl"
list_h = list_t = blockCounter_p = NULL;
        delBlock_p = NULL;
}

void DEDynMapBase::setup() {
# line 1064 "DEDynMapBase.pl"
// need a pointer to this star's parent
        mom = myParent();
                
        // We need a pointer to the current scheduler.
        // scheduler() is recursively called until a block with a scheduler
        // is reached. At this time we have three different DE schedulers.
        // We have to check which one is currently used and have to use a 
        // correct pointer. The other pointers are set to NULL for safety 
        // reasons.
        myDEScheduler_p = NULL;
        myCQScheduler_p = NULL;
        myMutCQScheduler_p = NULL;
        myScheduler_p = (DEBaseSched *)scheduler();
        if ( myScheduler_p->isA("DEScheduler") ) {
            myDEScheduler_p = (DEScheduler *)scheduler();
        } else if ( myScheduler_p->isA("CQScheduler") ) {
            myCQScheduler_p = (CQScheduler *)scheduler();
	} else if ( myScheduler_p->isA("MutableCQScheduler") ) {
            myMutCQScheduler_p = (MutableCQScheduler *)scheduler();
        } else {
            Error::abortRun(*this,
            "Mr. Ptolemy doesn't know this type of DEScheduler");
            return;
        }
}


GenericPort* DEDynMapBase::findTopGenericPort (PortHole *ph)
{
# line 196 "DEDynMapBase.pl"
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


void DEDynMapBase::connectError ()
{
# line 228 "DEDynMapBase.pl"
Error::abortRun(*this,
            "Star is not fully connected");
}


void DEDynMapBase::callStartMethods (Block*)
{
# line 265 "DEDynMapBase.pl"
return;
}


void DEDynMapBase::callStopMethods (Block*, double)
{
# line 274 "DEDynMapBase.pl"
return;
}


Galaxy* DEDynMapBase::myParent ()
{
# line 282 "DEDynMapBase.pl"
// Identify the parent, which is the galaxy this star belongs to
            Block *dad = parent();
            if (!dad || dad->isItAtomic()) {
                Error::abortRun(*this, "No parent!");
                return NULL;
            }
            return &(dad->asGalaxy());
}


void DEDynMapBase::connectPorts (PortHole* dest, PortHole* source)
{
# line 297 "DEDynMapBase.pl"
// connect two portholes

            // Do I really have to deal with aliases here ?
            // *source, which is the PortHole of the new block, is aliased 
            // from a GalPortHole. This GalPortHole ist aliased to *source.
            // This means, when a particle is sent to GalPortHole, it is
            // automatically forwarded to *source. 
            // bottomline: I think that dealing with aliases isn't 
            // necessary here.

            int numdelays = dest->numInitDelays();
            const char* initDelayVals = dest->initDelayValues();
            source->connect(*dest, numdelays, initDelayVals);

            return;
}


void DEDynMapBase::deleteDynBlock (int delName)
{
# line 320 "DEDynMapBase.pl"
// get the block from the list and remove it from the list
            // first check whether or not the list exists
            if (!list_t) {
                Error::abortRun(*this,
                    "No DynBlock exists, so I can't delete one");
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
  
                // once removed, it can be deleted
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
                ((myMutCQScheduler_p != NULL) && 
                (!myMutCQScheduler_p->checkDelayFreeLoop())) || 
                ((myCQScheduler_p != NULL) && 
                (!myCQScheduler_p->checkDelayFreeLoop()))) {
                    Error::abortRun(*this,
                    "DelayFreeLoop after deletion of a dynamic block detected");
                    return;
                }

                // the porthole priorities (depth) have to be updated
                if (((myDEScheduler_p != NULL) && 
                (!myDEScheduler_p->computeDepth())) || 
                ((myMutCQScheduler_p != NULL) && 
                (!myMutCQScheduler_p->computeDepth())) || 
                ((myCQScheduler_p != NULL) && 
                (!myCQScheduler_p->computeDepth()))) {
                    Error::abortRun(*this,
                  "Error in computing depth after deletion of a dynamic block");
                    return;
                }

		// Resort the calendar queue because of
		// porthole priority changes.
                if( myMutCQScheduler_p != NULL ) {
                    myMutCQScheduler_p->resortEvents();
		} else if( myCQScheduler_p != NULL ) {
                    myCQScheduler_p->resortEvents();
		} else if( myDEScheduler_p != NULL ) {
                    myDEScheduler_p->resortEvents();
		}

            }
            return;
}


void DEDynMapBase::setParameter (Block *block_p)
{
# line 454 "DEDynMapBase.pl"
for ( int i = 0; i < parameter_map.size()-1; i++ ) {
                const char *myName = parameter_map[i++];
                const char *value = parameter_map[i];
                block_p->setState(myName, value);
            }
            return;
}


void DEDynMapBase::makeStarsMutable (Block *block_p)
{
# line 468 "DEDynMapBase.pl"
// Block is a star
	    if( block_p->isItAtomic() ) {
		if( block_p->isA("DEStar") ) {
		    DEStar *deStar = (DEStar *)block_p;
		    deStar->makeMutable();
		    return;
		}
	    }
	    // Block is a galaxy
	    else {
		GalTopBlockIter next( (block_p->asGalaxy()) );
		Block *nextBlock;
		while( (nextBlock = next++) != NULL ) {
		    makeStarsMutable( nextBlock );
		}
	    }
            return;
}


Block * DEDynMapBase::createBlock ()
{
# line 493 "DEDynMapBase.pl"
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


void DEDynMapBase::createDynBlock (int number)
{
# line 534 "DEDynMapBase.pl"
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

	    // If the Mutable target is being used, make the DE 
	    // Stars mutable. Note here that we assume that if
	    // one wants to use the MutableCalendarQueue, then
	    // they will take advantage of it w.r.t. all stars
	    // involved.
	    DETarget *tar = (DETarget *)target();
	    if( tar->isMutable() ) {
	        makeStarsMutable(block_p);
	    }

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
            ((myMutCQScheduler_p != NULL) && 
            !(myMutCQScheduler_p->checkDelayFreeLoop())) || 
            ((myCQScheduler_p != NULL) && 
            !(myCQScheduler_p->checkDelayFreeLoop()))) {
                Error::abortRun(*this,
               "DelayFreeLoop after instantiation of a dynamic block detected");
                return;
            }
            
            // the porthole priorities (depth) have to be updated
            if (((myDEScheduler_p != NULL) && 
            (!myDEScheduler_p->computeDepth())) || 
            ((myMutCQScheduler_p != NULL) && 
            (!myMutCQScheduler_p->computeDepth())) || 
            ((myCQScheduler_p != NULL) && 
            (!myCQScheduler_p->computeDepth()))) {
                Error::abortRun(*this,
             "Error in computing depth after instantiation of a dynamic block");
                return;
            }
            
	    // Resort the calendar queue because of
	    // porthole priority changes.  
	    if( myMutCQScheduler_p != NULL ) { 
		myMutCQScheduler_p->resortEvents();
	    } else if( myCQScheduler_p != NULL ) {
                myCQScheduler_p->resortEvents();
	    } else if( myDEScheduler_p != NULL ) {
                myDEScheduler_p->resortEvents();
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
                        DERepeatStar& mySource = 
		                (DERepeatStar&) block_p->asStar();
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


void DEDynMapBase::scanGalaxyForSources (Galaxy& myGalaxy)
{
# line 675 "DEDynMapBase.pl"
GalTopBlockIter next(myGalaxy);
            Block *myBlock_p;
            while ((myBlock_p = next++) != NULL) {
                if (myBlock_p->isItAtomic()) {
                    // Block is atomic -> must be a star
                    // is it a RepeatStar?
                    if (myBlock_p->isA("DERepeatStar")) {
                        // it is
                        DERepeatStar& mySource = 
		                (DERepeatStar&) myBlock_p->asStar();
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


Block * DEDynMapBase::scanGalaxyForBlockWithName (Galaxy& myGalaxy, const char *itsName)
{
# line 705 "DEDynMapBase.pl"
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


void DEDynMapBase::deletePendingEventsOfStar (Star *myStar_p)
{
# line 736 "DEDynMapBase.pl"
// In the case of the mutable calendar queue, things are
	    // relatively simple.
            if (myMutCQScheduler_p) {
		// DE Stars are easiest
		if( myStar_p->isA("DEStar") ){
		    DEStar * deStar = (DEStar *)myStar_p;
		    deStar->clearAllPendingEvents(); 
		    return;
		}

		// Check to see if non-DE stars are on a wormhole boundary.
		// If not, then there should be no pending events. If so,
		// we'll have to remove pending events using a brute force
		// approach.
		else { 
		    BlockPortIter next(*myStar_p); 
		    PortHole *myPortHole_p; 
		    while ((myPortHole_p = next++) != NULL) {

                        if( myPortHole_p->atBoundary() ){
			    int numberOfEvents = 
				    myMutCQScheduler_p->eventQ.length();
                            CqLevelLink *store = NULL;
                            while ( numberOfEvents ) {
                                numberOfEvents--; 
				// get the first entry from the list 
				CqLevelLink *cqll_p = 
					myMutCQScheduler_p->eventQ.get(); 
				// for which star is that entry ?
                                Star *thatStar_p = cqll_p->dest; 
				// if this is the star under consideration ...
				if ( myStar_p == thatStar_p ) { 
				    // ... put it to the free links 
				    myMutCQScheduler_p->
					    eventQ.putFreeLink(cqll_p);
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
				myMutCQScheduler_p->eventQ.pushBack(temp); 
			    } 

			    // No need to check other portholes as we
			    // have removed all pending events from the Queue.
			    return;
                        }     

		    } 

	        } 
		// We have exhausted all possibilities given that
		// the MutableCQScheduler is being used. 
		return;
	    } 


            // We have "read" access to the block's private PortHoleList 
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


void DEDynMapBase::deletePendingEventsOfPortHole (DEPortHole *myPortHole_p)
{
# line 877 "DEDynMapBase.pl"
// In the case of the mutable calendar queue, things are
	    // relatively simple.
            if (myMutCQScheduler_p) {
                // we need the star our porthole belongs to
		// FIXME: Are we sure that the parent of this PortHole
		// will be a DEStar??
                DEStar* myStar_p = (DEStar *)myPortHole_p->parent();
		myStar_p->clearPendingEventsOfPortHole(myPortHole_p);
		return;
	    }

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


void DEDynMapBase::scanGalaxyForContents (Galaxy& myGalaxy)
{
# line 968 "DEDynMapBase.pl"
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


void DEDynMapBase::clearList ()
{
# line 991 "DEDynMapBase.pl"
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


void DEDynMapBase::deleteList ()
{
# line 1020 "DEDynMapBase.pl"
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


void DEDynMapBase::scanEventList (Block *myBlock_p)
{
# line 1049 "DEDynMapBase.pl"
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


// DEDynMapBase is an abstract class: no KnownBlock entry
