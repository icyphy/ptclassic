defstar{
    name { DynForkInt }
    domain { DE }
    derivedfrom { DEDynForkBase }
    author { J. Voigt }
    version { @(#)DEDynForkInt.pl	1.3	01/15/98 }
    copyright { 
copyright (c) 1997 Dresden University of Technology, WiNeS-Project.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions. 
    }
    desc { This a version of <tt>DEDynForkBase</tt> for integer portholes. 
On demand a new porthole is added to or removed from a MultiPortHole during 
runtime. All particles to its input are checked for the right receiver and sent only to it. 
    }
    htmldoc {
This star is an implementation of the dynamic fork function for integer 
portholes. We define the methods declared in <tt>DEDynForkBase</tt> and provide a simple <i>go</i>-method. This star is used for a proof of concept demo for 
<tt>DynMap</tt> and that stuff and is probably not too usefull anywhere else. 
<p>
More useful stars have to have portholes of another datatype than integer and
to override (at least) the <i>go</i>-method.
<p>        
<h3>Creating a porthole dynamically is as follows (<i>createPortHole</i>):</h3>
<p> 
This method is called from <tt>DEDynMapBase::createDynBlock</tt> whenever a new 
block is to put into the system configuration. It just creates a new porthole 
in the <i>out</i>-multiporthole and puts it into an internal list so that we 
can find it back lateron. A pointer to the porthole is returned.
<p>        
<h3>Removing a porthole dynamically is as follows (<i>removePortHole</i>):</h3>
<p> 
This method is called from <tt>DEDynMapBase::deleteDynBlock</tt> whenever a 
block is to be taken out of the system configuration. From 
<tt>DEDynMapBase::deleteDynBlock</tt> we get a pointer to one of the portholes 
in our<i>out</i>-multiporthole and look for it in a list. If we found this 
porthole, we remove it from the multiporthole's list and delete it.
<p>
<h3>The <i>go()</i>-method:</h3>
This method here is very simple and is just an example on how this kind of 
stars can be used.
<p>
By convention, it has to get an integer, which is the instancenumber of one of the blocks which are connected to one of the portholes in the 
<i>out</i>-multiporthole. It looks for that porthole in the internal list and 
sends exactly that instancenumber to the block. If no porthole for a block with the received instancenumber is found then nothing will happen.
}
    location { $PTOLEMY/src/domains/de/contrib/stars }
    input {
        name { in }
        type { int }
        desc { input for all dynamically created blocks }
    }
    outmulti {
        name { out }
        type { int }
        desc { will get one PortHole for each instance }
    }
    constructor {
        receiverAddress_p = myList_h = myList_t = NULL;
    }
    private {
        int myName;
        receiverAddress *receiverAddress_p, *myList_h, *myList_t;
    }
    destructor {
        // one never can rely on wrapup
        // the same stuff is done in DEDynBlock::clearList()
        while ( myList_h ) {
            receiverAddress_p = myList_h;
            myList_h = myList_h -> next;
            // delete it from the list
            delete receiverAddress_p;
        }
    }            
    method {
        name { createPortHole }
        access { public }
        type { "PortHole*" }
        arglist { "(int myName)" }
        code {
            // create the new PortHole
            PortHole& single_out = out.newPort();
            
            // put it in a list
            receiverAddress_p = new receiverAddress;
            receiverAddress_p->address = myName;
            receiverAddress_p->myPort_p = &single_out;
            if (!myList_h) myList_h = receiverAddress_p;
            else myList_t->next = receiverAddress_p;
            myList_t = receiverAddress_p;
            myList_t->next = NULL;
            
            return &single_out;
        }
    } 
    method {
        name { removePortHole }
        access { public }
        type { void }
        arglist { "(PortHole *delPort_p)" }
        code {
            // remove that PorthHole from my list
            // first check whether or not the list exists
            if (!myList_t || !myList_h) {
                Error::abortRun
                (*this,"No PortHole exists, so I can't delete one");
            }
            else {
                // there is a list
                if ((myList_h->next == NULL) && 
                (myList_h->myPort_p == delPort_p)) {    
                    // delete the only member
                    receiverAddress *deleteAddress_p;
                    deleteAddress_p = myList_h;
                    myList_h = NULL;
                    myList_t = NULL;
                    delete deleteAddress_p;
                }
                if ((myList_t != NULL) && (myList_h->next != NULL)) {
                    // list has at least two members
                    for (receiverAddress_p = myList_h; 
                    receiverAddress_p->next != NULL; 
                    receiverAddress_p = receiverAddress_p->next) {
                        if ((receiverAddress_p == myList_h) && 
                        (receiverAddress_p->myPort_p == delPort_p)) {
                            // delete first member
                            myList_h = receiverAddress_p->next;   
                            delete receiverAddress_p;
                            break;
                        }			
                        if (receiverAddress_p->next->myPort_p == delPort_p){
                            receiverAddress *deleteAddress_p;
                            deleteAddress_p = receiverAddress_p->next;
                            if (receiverAddress_p->next->next == NULL) {
                                // delete last member	
                                delete deleteAddress_p;
                                myList_t = receiverAddress_p;
                                myList_t->next = NULL;
                                break;
                            }
                            else {
                                // delete a member in the middle of others
                                receiverAddress_p->next = 
                                receiverAddress_p->next->next;
                                delete deleteAddress_p;
                                break;
                            }				
                        }
                    }
                }
            }
            // first, remove the PortHole from the Block's PortHoleList
            if (this->removePort(*delPort_p)) {
                // now, remove it from the MultiPortHole's list
                if (out.removePort(delPort_p)) {
                    // once removed everywhere, we can really delete it
                    delete delPort_p;
                }
            }
        } 
    }
    go {
        completionTime = arrivalTime;
        Particle& whatToSend = in.get();
        Particle& myParticle = whatToSend;
        myName = (int) myParticle;
        // look for the right receiver and send the information to it only
        // is this address doesn't exist anymore, nothin' will happen
        receiverAddress_p = myList_h; 
        while (receiverAddress_p) {
            if ( receiverAddress_p->address == myName ) {
                OutDEPort *send_p = (OutDEPort *) receiverAddress_p->myPort_p;
                send_p->put(completionTime) = whatToSend;
                break;
            }
            receiverAddress_p = receiverAddress_p->next;
        }
    }	
}  

