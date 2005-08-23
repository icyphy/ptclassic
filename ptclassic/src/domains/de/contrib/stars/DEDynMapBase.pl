defstar{
    name { DynMapBase }
    domain { DE }
    author { J. Voigt }
    version { @(#)DEDynMapBase.pl	1.8	03/27/98 }
    copyright {
copyright (c) 1997 Dresden University of Technology, WiNeS-Project              All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.  
}
    desc { This is the base star for a family of stars which supports a mutable 
system configuration in the DE-domain. This star family implements the 
functionality of a dynamic map-function for the discrete-event domain. A dynamic
 map-function works like the map-function in the HOF-domain, but we can change 
the number of blocks it multiplies during runtime of the simulation. Thus, we 
support a mutable system configuration. Note, that this star family only works with the two standard DE schedulers.
<p>
This class is an abstract class, so it can't be instantiated and can not be used on its own. It contains methods which are used by its childs.
    }
    htmldoc { This star contains methods which are used by <tt>DEDynMap</tt> and <tt>DEDynMapGr</tt> for an instantiation of other blocks during runtime of the simulation. The descriptions of these stars explain their application. We here just
 explain the single methods. 
<p>        
<h3>Creating a block dynamically is as follows (<i>createDynBlock</i>):</h3>
<p>
We create blocks using code from <tt>HOFBase::createBlock</tt> in 
<i>createBlock</i>. 
This method returns a pointer to a newly created block. 
This block is instantiated, gets a unique name, and is added to the galaxy's 
blocklist. 
Furthermore, we put it into an internal list, so that we can find it back 
lateron when we want to delete it. 
<p>
Then we look for input and output portholes of that newly created block and 
create approriate farsides for these portholes in <tt>DEDynMerge</tt> and 
<tt>DEDynFork</tt> using their <i>createPortHole()</i>-method. These portholes 
are connected afterwards. We here just have pure virtual methods <i>
connectXXXports</i>, because these portholes can 
be of different datatypes and are members of our childs only. Once they are 
connected, the newly created portholes are initialized. 
<p>
If <i>parameter_map</i> is not empty, the parameters in the new block are set in <i>setParameter</i>. If it is empty, the <i>initialization()</i>-method sets 
the default values. 
<p>
The entire galaxy's connectivity is checked. Then we check for delayfreeloops 
and update the porthole priorities.
<p>
We have to take special care of source-stars in the dynamically created blocks. 
We look for them in <i>scanGalaxyForSources</i>. If there are any, we have to 
give them an initial event at the current time. That means, all source-stars 
derived from <tt>DERepeat</tt>-star give now out an initial event at the time of their creation (instead of time zero, when they probably don't exist). Otherwise these source-stars would never give out any event. See the description of the <tt>DERepeat</tt>-star for details.
<p>
<h3>Deletion of a block dynamically is as follows (<i>deleteDynBlock</i>):</h3>
<p>
I get a unique identifier for a block to be deleted as parameter. 
This block is looked for in my internal list of currently existing blocks. I get a pointer to my deleteBlock from this list and remove that block from the list.
<p>
Then, the eventqueue is checked for any events pointing to deleteblock using 
<i>scanEventList</i>. If any events are found, they are deleted from the 
eventqueue.
<p> 
Then I look for input and output of the deleteblock. Again, the
<i>disconnectXXXPorts</i> are here just pure virtual, because the portholes are 
members of our childs and of a specila datatype. Inside 
<i>disconnectXXXPorts</i> we check the eventqueue for any events which are 
scheduled to the farside of the deleteblock's outputs. These farside are inputs, so events might be sent to them.Since we delete these farside, we better also 
delete the events. Then I disconnect the portholes from their farside and delete
 their farside using <i>removePortHole()</i> in <tt>DEDynMerge</tt> and 
<tt>DEDynFork</tt>, respectively. 
<p>
Next, I remove deleteblock from the galaxy's blocklist, where it was added to by the code from <i>createDynBlock</i>. Once removed there, it can be recycled into our blockpool. 
<p>
The entire galaxy's connectivity it checked again. Then I check for 
delayfreeloops again and update the porthole priorities.
<p>
<h3>Scanning the eventqueue is as follows (<i>scanEventList</i>):</h3>
<p>
Using <i>scanEventList()</i> and <i>scanGalaxyForContents()</i> I go down the 
block's hierarchy until I only deal with atomic elements (<tt>stars</tt>). 
For each star I check the current eventQueue for any pending pointer to it 
(CQScheduler and MutableCQSCheduler) or for any pending pointer to any of its 
input-portholes (DEScheduler) using <i>deletePendingEventsOfStar</i> or 
<i>deletePendingEventsOfPortHole</i>, which should be self-exploratory both.
<p>
<h3>Setting parameter values:</h3>
<p>
All parameters are used in the derived class <tt>DEDynMap</tt>.
<p>
<h4>blockname:</h4> Name of the block, which is to multiply. The masterinstance. 
This block can be any galaxy or star. However, it must not have multiportholes.
<p>
<h4>where_defined:</h4> The full path and facet name for the definition of 
blockname.
<p>
<h4>output_map:</h4> The names of the block's outputs for mapping. The names of 
the outputs from the masterinstance should be listed in the order in which they 
should be connected.
<p>
<h4>input_map:</h4> The names of the block's inputs for mapping. The names of 
the inputs to the masterblock should be listed in the order in which they should
be connected.
<p>
<h4>parameter_map:</h4> The mapping of parameters like in HOF::Map.
<p>
<h3>The current limitations are:</h3>
<p>
A block which is to instantiate must not have multiportholes.
<p>
FIXME: The eventlist is to re-sort after the configuration changed. Thanks to Tom Lane for pointing this out.
}
    acknowledge { I use some code from Edward A. Lee's older version of 
<tt>HOFBase</tt>. Former versions of <tt>DEDynMapBase</tt> called that code 
directly. But, Tom Lane changed the HOF-stars. So, I copied the older code 
directly into this file, having the advantage of being independent of further 
changes in HOF now. MutableCQScheduler incorporated by John S. Davis II}
    location { $PTOLEMY/src/domains/de/contrib/stars }
    hinclude { "checkConnect.h", "MutableCQScheduler.h", "CQScheduler.h", "DEScheduler.h", "DERepeatStar.h", "DEDynMerge.h", "DETarget.h", "KnownBlock.h", "InfString.h"}
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
        desc { The names of the block's outputs for mapping.
        }
    }
    defstate {
        name { input_map }
        type { stringarray }
        default { "" }
        desc { The names of the block's inputs for mapping.
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
        MutableCQScheduler *myMutCQScheduler_p;
    }
    header {
        struct blockCounter {
            int blockName;
            Block *myBlock_p;
            blockCounter *next;
        };
    }
    code {
        int DEDynMapBase::nameCounter = 0;
    }
    constructor { 
        list_h = list_t = blockCounter_p = NULL;
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
    // The start- and stop-methods are very specific.
    // The default implementations do nothing.
    virtual method {
        name { callStartMethods }
        type { void }
        arglist { "(Block*)" }
        access { protected }
        code {
            return;
        }
    }
    virtual method {
        name { callStopMethods }
        type { void }
        arglist { "(Block*, double)" }
        access { protected }
        code {
            return;
        }
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

            int numdelays = dest->numInitDelays();
            const char* initDelayVals = dest->initDelayValues();
            source->connect(*dest, numdelays, initDelayVals);

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
    }
    method {
        name { setParameter }
        type { void }
        access { private }
        arglist { "(Block *block_p)" }
        code {
            for ( int i = 0; i < parameter_map.size()-1; i++ ) {
                const char *myName = parameter_map[i++];
                const char *value = parameter_map[i];
                block_p->setState(myName, value);
            }
            return;
        }
    }
    method {
        name { makeStarsMutable }
        type { void }
        access { private }
        arglist { "(Block *block_p)" }
        code {
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
    }
    method {
        name { deletePendingEventsOfPortHole }
        type { void }
        access { protected }
        arglist { "(DEPortHole *myPortHole_p)" }
        code {
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
}   

