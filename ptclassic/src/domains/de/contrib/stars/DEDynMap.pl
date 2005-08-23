defstar{
    name { DynBlock }
    domain { DE }
    derivedfrom { DEDynBlockBase }
    author { J. Voigt }
    version { 1.1 11/19/97 
    }
    copyright { copyright (c) 1997 Dresden University of Technology
                WiNeS-Project }
    desc { This Star instantiates and deletes any Block given by "blockname" on
 demand from "newBlock" or "delBlock", respectively. In "newBlock" and 
 "delBlock" an integer value as unique identifier for the block to be created is expected.

We use now the states of our mom to get all information we need about the block to create. See the description of DEDynBlockBase.pl, where this star is derived 
from, for further information. }
    location { $PTOLEMY/src/domains/de/contrib/stars }
    hinclude { "DEDynFork.h", "DEDynMerge.h" }
    input {
        name { newBlock }
        type { int }
        desc { gets the number of the instance to create }
    }
    input {
        name { delBlock }
        type { int }
        desc { gets the number of the instances to delete }
    }
    inmulti {
        name { in }
        type { anytype }
        desc { dummy input, we just need a connection to DEDynFork }
    }
    outmulti {
        name { out }
        type { =in }
        desc { dummy output, we just need a connection to DEDynMerge }
    }
    protected {
        int numberOfOutputs, numberOfInputs;
        DEDynMerge *myDynMerge_p;
        DEDynFork *myDynFork_p;
        PortHole *Dest;          
    }
    constructor {
        DEDynBlockBase::DEDynBlockBase();
    }
    destructor {
        if (list_h) deleteList();
    }
    setup {
        // call the method in the mother-star
        DEDynBlockBase::setup();

        // need a pointer to this star's parent
        mom = myParent();

        // how many outputs do we have?
        numberOfOutputs = output_map.size();

        // how many inputs do we have?
        numberOfInputs = input_map.size();

        // how many DEDynMerge-stars are waiting for a connection?
        MultiPortHole* myMultiOutput_p = this->multiPortWithName("out");
        int numberOfPorts = myMultiOutput_p->numberPorts();

        // do they agree?
        if ( numberOfOutputs != numberOfPorts ) {
            Error::abortRun(*this, "Number of DynMerge-Blocks does not fit the number of outputs spezified in the output_map");
            return;
        }

        // how many DEDynFork-stars are waiting for a connection?
        MultiPortHole* myMultiInput_p = this->multiPortWithName("in");
        numberOfPorts = myMultiInput_p->numberPorts();

        // do they agree?
        if ( numberOfInputs != numberOfPorts ) {
            Error::abortRun(*this, "Number of DynFork-Blocks does not fit the number of inputs spezified in the input_map");
            return;
        }
    }
    method {
        name { connectOutputs }
        type { void }
        arglist { "(Block *block_p)" }
        access { private }
        code {
            // take each single porthole and connect it with its DynMerge 
            for ( int i = 1; i <= numberOfOutputs; i++) {
                name = "out#";
                name += i;
                PortHole* myDummyOut_p = this->portWithName((const char *)name);
                PortHole* itsFarside_p = myDummyOut_p->far();

                // are we fully connected?
                if (itsFarside_p == NULL) {
                    connectError();
                    return;
                }
                
                myDynMerge_p = (DEDynMerge *)itsFarside_p->parent();
                if (!myDynMerge_p->isA("DEDynMerge")) {
                    Error::abortRun(*this, "You must put an DEDynMerge-star on my out-porthole.");
                    return;
                }
        
                // get the outputs of the newly created block
                if (!(Dest = block_p->portWithName(output_map[i-1]))) {
                    Error::abortRun(*this,
                    "maybe output_map contains unrecognized name: ",
                    output_map[i-1]);     
                    return;
                }
                if (!Dest) return;

                // create a new PortHole on the farside
                PortHole *DynPortHole_p = myDynMerge_p->createPortHole();
            
                // connect outputs, (Dest will be connected to *DynPortHole_p)
                connectPorts(DynPortHole_p, Dest);
            
                // the porthole is connected, initialize it
                DynPortHole_p->initialize();
            }
            return;
        }
    }
    method {
        name { connectInputs }
        type { void }
        arglist { "(Block *block_p, int number)" }
        access { private }
        code {
            // take each single porthole and connect it with its DynFork 
            for ( int i = 1; i <= numberOfInputs; i++) {
                name = "in#";
                name += i;
                PortHole* myDummy_p = this->portWithName((const char *)name);
                PortHole* itsFarside_p = myDummy_p->far();

                // are we fully connected?
                if (itsFarside_p == NULL) {
                    connectError();
                    return;
                }
            
                myDynFork_p = (DEDynFork *)itsFarside_p->parent();
                if (!myDynFork_p->isA("DEDynFork")) {
                    Error::abortRun(*this, "You must put an DEDynFork-star on my in-porthole.");
                    return;
                }

                // get the input of the newly created block
                if (!(Dest = block_p->portWithName(input_map[i-1]))) {
                    Error::abortRun(*this,
                    "maybe input_map contains unrecognized name: ", input_map[i-1]);            
                }
                if (!Dest) return;

                // create a new PortHole on the farside
                PortHole *DynPortHole_p = myDynFork_p->createPortHole(number);
                
                // connect outputs, (Dest will be connected to *DynPortHole_p)
                connectPorts(DynPortHole_p, Dest);
            
                // the porthole is connected, initialize it
                DynPortHole_p->initialize();
            }
            return;
        }
    }
    method {
        name { disconnectOutputs }
        type { void }
        arglist { "(Block *delBlock_p)" }
        access { protected }
        code {
             for (int i = 1; i <= numberOfOutputs; i++) {
                PortHole *port_p = delBlock_p->portWithName(output_map[i-1]);

                // get its farside (the porthole of DynMerge which 
                // it is connected to)
                PortHole *farside_p = port_p->far();

                // are we fully connected?
                if (farside_p == NULL) {
                    connectError();
                    return;
                }  

                // get the parent
                myDynMerge_p = (DEDynMerge *)farside_p->parent();

                // This farside is an input. We have to check whether there are
                // any events scheduled for that porthole. If there are any, we
                // better delete them. 
                deletePendingEventsOfPortHole(farside_p);

                // disconnect the porthole
                farside_p->disconnect();
                    
                // delete its farside
                myDynMerge_p->removePortHole(farside_p);
            }
            return;
        }
    }
    method {
        name { disconnectInputs }
        type { void }
        arglist { "(Block *delBlock_p)" }
        access { protected }
        code {
            for (int i = 1; i <= numberOfInputs; i++) {
                PortHole *port_p = delBlock_p->portWithName(input_map[i-1]);

                // get its farside (the porthole of DynFork which 
                // it is connected to)
                PortHole *farside_p = port_p->far();

                // are we fully connected?
                if (farside_p == NULL) {
                    connectError();
                    return;
                }

                // get the parent
                myDynFork_p = (DEDynFork *)farside_p->parent();

                // disconnect the porthole
                farside_p->disconnect();
                    
                // delete its farside
                myDynFork_p->removePortHole(farside_p);
            }
            return;
        }
    }
    go {
        if (newBlock.dataNew) {
            // create the new block
            // we always have to deal with the same type here, so we just 
            // need to check the instancenumber
            createDynBlock(newBlock.get());
        }
        if (delBlock.dataNew) {
            // delete the block
            // we always have to deal with the same type here, so we just 
            // need to check the instancenumber
            deleteDynBlock(delBlock.get());
        }                             
    }
    wrapup {
        // delete all still living blocks or the system will crash
        clearList();
    }
}   


