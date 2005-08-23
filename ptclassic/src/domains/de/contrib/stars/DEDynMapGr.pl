defstar{
    name { DynMapGr }
    domain { DE }
    derivedfrom { DEDynMap }
    author { J. Voigt }
    version { @(#)DEDynMapGr.pl	1.5	01/15/98 }
    copyright { copyright (c) 1997 Dresden University of Technology 
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.  }
    desc { A graphical Version of the <tt>DEDynMap</tt>-star.  We now use a 
masterblock which we clone instead of granny's states.
    }
    htmldoc { Now we have to put a masterinstance of a block, which we want to multiply dynamically, at our <i>exin</i> and <i>exout</i> portholes instead of declaring it in the states. Otherwise, the same functionality like in <tt>DEDynMap</tt> is implemented. See its description for further explanations.
<p>
We can clone a block which has inputs and outputs or a block 
which has just one type of portholes. If we have a block without inputs to 
clone, we can put a <tt>DEBlackHole</tt> on our exout. If we have a block 
without outputs to clone, we can put a <tt>DENull</tt> on our exin. 
Or we create us icons with the right number of portholes.
<p>
See the descriptions of <tt>DEDynMapBase</tt> and <tt>DEDynMap</tt>, where this star is derived from, for further details. 
<p>
<h3>The current limitations are:</h3>
<p>
The masterblock must not have MultiPortHoles.
We can't have more then one block to clone on our portholes.
}
    location { $PTOLEMY/src/domains/de/contrib/stars }
    inmulti {
        name { exin }
        type { anytype }
        desc { dummy input, we just need a connection to the masterblock }
    }
    outmulti {
        name { exout }
        type { =in }
        desc { dummy output, we just need a connection to the masterblock }
    }
    private {
        Block *masterBlock_p;
        InfString outputMap, inputMap;
    }
    constructor {
        out.inheritTypeFrom(exin);
        input_map.clearAttributes(A_SETTABLE);
        output_map.clearAttributes(A_SETTABLE);
        blockname.clearAttributes(A_SETTABLE);
        where_defined.clearAttributes(A_SETTABLE);
        parameter_map.setInitValue("");
        masterBlock_p = NULL;
    }
    destructor {   
        if (masterBlock_p) {
            delete masterBlock_p;
            masterBlock_p = NULL;
        }
    }
    setup {
        // call the method in the grandmother-star, because mom's setup() deals
        // with states which I just cleared in the constructor...
        DEDynMapBase::setup();
        
        // set our parent-block
        mom = myParent();

        // reset our counter
        nameCounter = 0;

        // how many outputs do we have?
        numberOfInputs = exout.numberPorts();
        
        // how many inputs do we have?
        numberOfOutputs = exin.numberPorts();

        // If we have exins or exouts we have to get a pointer to the 
        // masterblock. I assume that this is the first run. Inside this routine
        // I delete all the exins and exouts. So, the next run will skip
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
            
            // first we check what lies on our exout
            if (numberOfInputs) {   
                // we have at least one exout            

                // iterate over all exouts
                // I delete the portholes of the MPH. 
                // So, I can't use the Iterators.
                for ( int i = 1; i <= numberOfInputs; i++ ) {
                    name = "exout#";
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
                        if (exout.removePort(myPortHole_p)) {
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

            // let's check what's on the exin now
            if (numberOfOutputs) {
                // we have at least one exin
                
                // iterate over all exins
                // I delete the portholes of the MPH. 
                // So, I can't use the Iterators.
                for ( int i = 1; i <= numberOfOutputs; i++ ) {
                    name = "exin#";
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
                        if (exin.removePort(myPortHole_p)) {
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
                // if we don't have an exout or there is a DEBlackHole, the
                // block on our exins is the right one
                if (!numberOfInputs) {
                    masterBlock_p = secondMasterBlock_p;
                }
                // if we don't have an exin or there is a DENull, the
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
            // We have to be careful with the masterblock's parent. It does not
            // need to be the same galaxy this DynMap-star belongs to. So, we 
            // have to ask for it separately.
            Block *itsMom = masterBlock_p->parent();
            if (!itsMom || itsMom->isItAtomic()) {
                Error::abortRun(*this, "Masterblock has no parent!");
                return;
            }
            Galaxy * itsGalMom = &(itsMom->asGalaxy());
            if (!itsGalMom->removeBlock(*masterBlock_p)) {
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
