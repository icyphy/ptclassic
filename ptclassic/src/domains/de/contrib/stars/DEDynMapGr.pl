defstar{
    name { DynBlockGr }
    domain { DE }
    derivedfrom { DEDynBlock }
    author { J. Voigt }
    version { 1.1 11/19/97 
    }
    copyright { copyright (c) 1997 Dresden University of Technology
                WiNeS-Project }
    desc { A graphical version of the DEDynBlock-star.  We now use a 
masterblock which we clone instead of granny's states.

We can clone a block which has inputs and outputs or a block which has just one
type of portholes. If we have a block without inputs to clone, we can put a 
DEBlackHole on our dummyOut. If we have a block without outputs to clone, we can put a DENull on our dummyIn. Or we create us icons with the right number of 
portholes.

The current limitations are:

The masterblock must not have MultiPortHoles.
We can't have more then one block to clone on our portholes.

See the descriptions of DEDynBlockBase.pl and DEDynBlock.pl, where this star is
derived from, for further details. 

}
    location { $PTOLEMY/src/domains/de/contrib/stars }
    inmulti {
        name { dummyIn }
        type { anytype }
        desc { dummy input, we just need a connection to the prototype-block }
    }
    outmulti {
        name { dummyOut }
        type { =in }
        desc { dummy output, we just need a connection to the prototype-block }
    }
    private {
        Block *masterBlock_p;
        InfString outputMap, inputMap;
    }
    constructor {
        DEDynBlock::DEDynBlock();
        out.inheritTypeFrom(dummyIn);
        input_map.clearAttributes(A_SETTABLE);
        output_map.clearAttributes(A_SETTABLE);
        blockname.clearAttributes(A_SETTABLE);
        where_defined.clearAttributes(A_SETTABLE);
        parameter_map.setInitValue("");
        masterBlock_p = NULL;
    }
    destructor {   
        if (masterBlock_p) delete masterBlock_p;
    }
    setup {
        // call the method in the grandmother-star, because mom's setup() deals
        // with states which I just cleared in the constructor...
        DEDynBlockBase::setup();
        
        // set our parent-block
        mom = myParent();

        // reset our counter
        nameCounter = 0;

        // how many outputs do we have?
        numberOfInputs = dummyOut.numberPorts();
        
        // how many inputs do we have?
        numberOfOutputs = dummyIn.numberPorts();

        // If we have dummyIns or dummyOuts we have to get a pointer to the 
        // masterblock. I assume that this is the first run. Inside this routine
        // I delete all the dummyIns and dummyOuts. So, the next run will skip
        // this and use the masterBlock_p from the previous run instead.
        if (numberOfOutputs || numberOfInputs) {

            // check the number of PortHoles
            if (( numberOfOutputs != out.numberPorts()) ||
            ( numberOfInputs != in.numberPorts())) {
                Error::abortRun(*this,
                "The number of portholes to the prototype block doesn't fit the number of DynMerge or DynFork stars");
                return;
            }
            
            // We need a pointer to the masterblock. This is the top-level 
            // block, which is connected to one of our dummyportholes.
            // Additionally, we have to disconnect the dummyportholes und we 
            // have to store the names of their farsides into the input_map 
            // or output_map.
            Block *myParent_p, *secondMasterBlock_p;
            PortHole *myPortHole_p;
            secondMasterBlock_p = NULL;
            
            // first we check what lies on our dummyout
            if (numberOfInputs) {   
                // we have at least one dummyout            

                // iterate over all dummyouts
                // I delete the portholes of the MPH. 
                // So, I can't use the Iterators.
                for ( int i = 1; i <= numberOfInputs; i++ ) {
                    name = "dummyOut#";
                    name += i;
                    myPortHole_p = this->portWithName((const char *)name);
                    PortHole* itsFarside_p = myPortHole_p->far();
                    // are we fully connected?
                    if (itsFarside_p == NULL) {
                        connectError();
                        return;
                    }
                    // We need to clone the top-level block. 
                    // Let's get a pointer to it.
                    GenericPort *realPort_p = 
                    findTopGenericPort(itsFarside_p);
                    myParent_p = (Block *)realPort_p->parent();
                    // we just support one type of masterblock
                    if (!masterBlock_p) {
                        masterBlock_p = myParent_p;
                    }
                    else {
                        if (myParent_p != masterBlock_p) {
                            Error::abortRun(*this, "Sorry, only one masterblock is allowed at this time");
                            return;
                        }
                    }   
                    // now we disconnect that porthole
                    itsFarside_p->disconnect();
                    
                    // and delete it on our side
                    // first, remove the PortHole from the Block's PortHoleList
                    if (this->removePort(*myPortHole_p)) {
                        // now, remove it from the MultiPortHole's list
                        if (dummyOut.removePort(myPortHole_p)) {
                            // once removed everywhere, we can really delete it
                            delete myPortHole_p;
                        }
                    } 

                    // and we store its farside's name 
                    if (inputMap.numPieces() > 0) inputMap += " ";
                    inputMap += realPort_p->name();
                }
                if (masterBlock_p->isA("DEBlackHole")) {
                    // that is not worth to be cloned
                    masterBlock_p = NULL;
                    numberOfInputs = 0;
                }   
            }

            // let's check what's on the dummyIn now
            if (numberOfOutputs) {
                // we have at least one dummyInt 
                
                // iterate over all dummyints
                // I delete the portholes of the MPH. 
                // So, I can't use the Iterators.
                for ( int i = 1; i <= numberOfOutputs; i++ ) {
                    name = "dummyIn#";
                    name += i;
                    myPortHole_p = this->portWithName((const char *)name);
                    PortHole* itsFarside_p = myPortHole_p->far();
                    // are we fully connected?
                    if (itsFarside_p == NULL) {
                        connectError();
                        return;
                    }
                
                    // we need to clone the top-level block!!!
                    GenericPort *realPort_p = 
                    findTopGenericPort(itsFarside_p);
                    myParent_p = (Block *)realPort_p->parent();
                    // we just support one type of masterblock
                    if (!secondMasterBlock_p) {
                        secondMasterBlock_p = myParent_p;
                    }
                    else {
                        if (myParent_p != secondMasterBlock_p) {
                            Error::abortRun(*this, "Sorry, only one masterblock is allowed at this time");
                            return;
                        }
                    }   
                    // now we disconnect that porthole
                    itsFarside_p->disconnect();
                
                    // and delete it on our side
                    // first, remove the PortHole from the Block's PortHoleList
                    if (this->removePort(*myPortHole_p)) {
                        // now, remove it from the MultiPortHole's list
                        if (dummyIn.removePort(myPortHole_p)) {
                            // once removed everywhere, we can really delete it
                            delete myPortHole_p;
                        }
                    } 
                    
                    // and we store its farside's name
                    if (outputMap.numPieces() > 0) outputMap += " ";
                    outputMap += realPort_p->name();
                }
                
                // is it a good farside-parent?
                if (secondMasterBlock_p->isA("DENull")) {
                    // that is not worth to be cloned
                    secondMasterBlock_p = NULL;
                    numberOfOutputs = 0;
                }
                // if we don't have an dummyOut or there is a DEBlackHole, the
                // block on our dummyInts is the right one
                if (!numberOfInputs) {
                    masterBlock_p = secondMasterBlock_p;
                }
                // if we don't have an dummyInt or there is a DENull, the
                // block on our dummOuts is the right one
                if (!numberOfOutputs) {
                    secondMasterBlock_p = masterBlock_p;
                }
                // do we have different blocks on our ports?
                if (secondMasterBlock_p != masterBlock_p) {
                    Error::abortRun(*this, "Sorry, only one masterblock is allowed at this time");
                    return;
                }
            }
            if (!masterBlock_p) {
                Error::abortRun(*this, "There is no block which is worth to be cloned");
                return;
            }
            
            // remove the block from mom's list, otherwise we have unconnected 
            // portholes and so on
            // attention: it won't be deleted after this line, which is 
            // very good:
            // we need a pointer to a prototype of that block for further 
            // cloning
            if (!mom->removeBlock(*masterBlock_p)) {
                Error::abortRun(*this,
                "MasterBlock is not removed from mom's list");
                return;
            }
        }
        // Let's set and initialize granny's input_map and output_map.
        // We have to do this in each run, not just the first time.
        input_map.setInitValue((const char *)inputMap);
        input_map.initialize();
        output_map.setInitValue((const char *)outputMap);
        output_map.initialize();

        // how many outputs do we really have?
        numberOfOutputs = output_map.size();

        // how many inputs do we really have?
        numberOfInputs = input_map.size();
    }
    method {
        name { createBlock }
        type { "Block *" }
        access { private }
        code {
            Block *block_p;                
            // clone our prototype-block
            block_p = masterBlock_p->clone();
            if (!block_p) {
                Error::abortRun(*this, "I can't clone the masterblock"); 
                return NULL;
            }
            return block_p;
        }
    }
    wrapup {
        clearList();
    }   
}
