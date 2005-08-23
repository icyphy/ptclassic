defstar{
    name { DynMap }
    domain { DE }
    derivedfrom { DEDynMapBase }
    author { J. Voigt }
    version { @(#)DEDynMap.pl	1.7	03/27/98 }
    copyright { copyright (c) 1997 Dresden University of Technology
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.  
}
    desc { This star implements a dynamic map-function. It is inherited from 
<tt>DEDynMapBase</tt>. It instantiates and deletes any block given by the 
parameter <i>blockname</i> on demand from <i>newBlock</i> or <i>delBlock</i>, 
respectively. In <i>newBlock</i> and <i>delBlock</i> an integer value as unique identifier for the block to be created is expected.
    }
    htmldoc { This star creates and deletes blocks dynamically. It must be 
connected to <tt>DynMerge</tt>-star(s) and/or <tt>DynFork</tt>-star(s) in order 
to get and release all the portholes needed for the dynamically created blocks.
<p>
It is a real implementation of a dynamic map-function. Each time 
<i>newBlock</i> gets an event, a new instance of a block of type 
<i>blockname</i> is put into the system's configuration. Each time 
<i>delBlock</i> gets an event, a block is taken from the configuration.
<p>
Actually, this star is just an example on how to do this. We here use just 
plain integers for the <i>instancename</i> of the block we create dynamically. 
Other, very similar stars can also be inherited from <tt>DEDynMapBase</tt> and 
might have portholes of other datatypes. Especially the message-type will be 
very useful to send more information about and to the dynamically created block.
<p>
In <i>setup()</i> we check whether or not the number of outputs of 
<i>blockname</i> fits to the number of <tt>DEDynMerge</tt> this star is 
connected to. And so we do with the number of inputs <i>blockname</i>
 has and the number of <tt>DEDynMerge</tt> this star is connected to.
If there are any differences, an errormessage is given out. In fact, we do need
one <tt>DEDynMerge</tt> for each output of <i>blockname</i> and one 
<tt>DEDynFork</tt> for each input <i>blockname</i> has.
<p>
We use now the states of the baseclass <tt>DEDynMapBase</tt> to get all 
information we need about the block to create. See the description of 
<tt>DEDynMapBase</tt>, where this star is derived from, for further information.
<p>
<h3>Connecting the output-portholes is as follows (<i>connectOutputs</i>):</h3>
<p>
We check how many output-portholes our block with <i>blockname</i> has. From 
<i>setup</i> we know that we have exactly as many <tt>DEDynMerge</tt> connected to the <i>out</i>-multiporthole of that star. Each of the portholes in <i>out</i> is connected to the first porthole of the input-multiporthole of its 
<tt>DEDynMerge</tt>. 
<p>
So, each output-porthole of our newly created block of type <tt>blockname</tt>
is to connect to another <tt>DEDynMerge</tt>. For each output-porthole we do:
We look for the farside of the next porthole in <i>out</i>. The parent of this farside is exactly a pointer to the <tt>DEDynMerge</tt>-star this porthole in <i>out</i> is connected to. If this parent is not a <tt>DEDynMerge</tt> an 
errormessage is given out. Then we look in the <i>output_map</i> for the name of the porthole of <i>blockname</i> and get a pointer to this porthole. We create a new porthole in the input-multiporthole of our <tt>DEDynMerge</tt> and connect both portholes.
<p>
The same procedure is done for every output-porthole of our newly created 
instance of type <i>blockname</i>.
<p>
<h3>Connecting the input-portholes is as follows (<i>connectInputs</i>):</h3>
<p>
We check how many input-portholes our block with <i>blockname</i> has. From 
<i>setup</i> we know that we have exactly as many <tt>DEDynFork</tt> connected to the <i>out</i>-multiporthole of that star. Each of the portholes in <i>out</i> is connected to the first porthole of the output-multiporthole of its 
<tt>DEDynFork</tt>. 
<p>
So, each input-porthole of our newly created block of type <tt>blockname</tt>
is to connect to another <tt>DEDynFork</tt>. For each input-porthole we do:
We look for the farside of the next porthole in <i>in</i>. The parent of this farside is exactly a pointer to the <tt>DEDynFork</tt>-star this porthole in 
<i>in</i> is connected to. If this parent is not a <tt>DEDynFork</tt> an 
errormessage is given out. Then we look in the <i>input_map</i> for the name of the porthole of <i>blockname</i> and get a pointer to this porthole. We create a new porthole in the output-multiporthole of our <tt>DEDynFork</tt> and connect both portholes.
<p>
The same procedure is done for every input-porthole of our newly created 
instance of type <i>blockname</i>.
<p>
<h3>Disconnection of portholes is as follows (<i>disconnectInputs</i> and <i>disconnectOutputs</i>):</h3>
<p>
We do the same procedure as in <i>connectXXXputs</i>, but we do it in the reverse order. We
disconnect the portholes and delete the portholes on the farside in the <tt>DEDynMerge</tt> and <tt>DEDynFork</tt>-stars.
<p>
However, we have to be careful with deleting the input-portholes of <tt>DEDynMerge</tt>. Some events might be scheduled to them. If there are any, we better delete them.
}
    location { $PTOLEMY/src/domains/de/contrib/stars }
    hinclude { "DEDynForkBase.h", "DEDynMerge.h" }
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
        DEDynForkBase *myDynFork_p;
        PortHole *Dest;          
    }
    destructor {
        if (list_h) deleteList();
    }
    setup {
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
                myDynFork_p = (DEDynForkBase *)farside_p->parent();

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


