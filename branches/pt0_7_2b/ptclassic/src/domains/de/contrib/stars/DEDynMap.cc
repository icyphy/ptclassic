static const char file_id[] = "DEDynMap.pl";
// .cc file generated from DEDynMap.pl by ptlang
/*
copyright (c) 1997 Dresden University of Technology
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEDynMap.h"

const char *star_nm_DEDynMap = "DEDynMap";

const char* DEDynMap :: className() const {return star_nm_DEDynMap;}

ISA_FUNC(DEDynMap,DEDynMapBase);

Block* DEDynMap :: makeNew() const { LOG_NEW; return new DEDynMap;}

DEDynMap::DEDynMap ()
{
	setDescriptor("This star implements a dynamic map-function. It is inherited from \n<tt>DEDynMapBase</tt>. It instantiates and deletes any block given by the \nparameter <i>blockname</i> on demand from <i>newBlock</i> or <i>delBlock</i>, \nrespectively. In <i>newBlock</i> and <i>delBlock</i> an integer value as unique identifier for the block to be created is expected.");
	addPort(newBlock.setPort("newBlock",this,INT));
	addPort(delBlock.setPort("delBlock",this,INT));
	addPort(in.setPort("in",this,ANYTYPE));
	addPort(out.setPort("out",this,ANYTYPE));
	out.inheritTypeFrom(in);


}

void DEDynMap::wrapup() {
# line 332 "DEDynMap.pl"
// delete all still living blocks or the system will crash
        clearList();
}

DEDynMap::~DEDynMap() {
# line 109 "DEDynMap.pl"
if (list_h) deleteList();
}

void DEDynMap::setup() {
# line 112 "DEDynMap.pl"
// call the method in the mother-star
        DEDynMapBase::setup();

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
            Error::abortRun(*this, "Number of DynMerge-Blocks does not fit the number of outputs specified in the output_map");
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

void DEDynMap::go() {
# line 318 "DEDynMap.pl"
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


void DEDynMap::connectOutputs (Block *block_p)
{
# line 150 "DEDynMap.pl"
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

		// Make the DEDynMerge star a mutable star.
		DETarget *tar = (DETarget *)target();
		if( tar->isMutable() ) {
		    myDynMerge_p->makeMutable();
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


void DEDynMap::connectInputs (Block *block_p, int number)
{
# line 202 "DEDynMap.pl"
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
            
                myDynFork_p = (DEDynForkBase *)itsFarside_p->parent();
                if (!myDynFork_p->isA("DEDynForkBase")) {
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


void DEDynMap::disconnectOutputs (Block *delBlock_p)
{
# line 246 "DEDynMap.pl"
for (int i = 1; i <= numberOfOutputs; i++) {
                PortHole *port_p = delBlock_p->portWithName(output_map[i-1]);

                // get its farside (the porthole of DynMerge which 
                // it is connected to)
                PortHole *farside_p = port_p->far();

		// The far side porthole better be a DEPortHole since
		// the farside should be a DynMerge star.
		DEPortHole *defarside_p;
		if( farside_p->isA("DEPortHole") ) {
		    defarside_p = (DEPortHole *)farside_p;
		}
		else {
		    Error::abortRun("Attempting to disconnect non-DE PortHole of DynMerge Star");
		}

                // are we fully connected?
                if (defarside_p == NULL) {
                    connectError();
                    return;
                }  

                // get the parent
                myDynMerge_p = (DEDynMerge *)defarside_p->parent();

                // This farside is an input. We have to check whether there are
                // any events scheduled for that porthole. If there are any, we
                // better delete them. 
                deletePendingEventsOfPortHole(defarside_p);

                // disconnect the porthole
                defarside_p->disconnect();
                    
                // delete its farside
                myDynMerge_p->removePortHole(defarside_p);
            }
            return;
}


void DEDynMap::disconnectInputs (Block *delBlock_p)
{
# line 292 "DEDynMap.pl"
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
                myDynFork_p = (DEDynForkBase *)farside_p->parent();

                // disconnect the porthole
                farside_p->disconnect();
                    
                // delete its farside
                myDynFork_p->removePortHole(farside_p);
            }
            return;
}


// prototype instance for known block list
static DEDynMap proto;
static RegisterBlock registerBlock(proto,"DynMap");
