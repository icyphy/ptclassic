defstar{
    name { DynForkInt }
    domain { DE }
    derivedfrom { DEDynFork }
    author { J. Voigt }
    version { 1.0 9/5/97 
    	    }
    copyright { copyright (c) 1997 Dresden University of Technology
                WiNeS-Project }
    desc { This a version of DEDynFork for integer portholes. See the description of DEDynFork.pl, where this star is derived from, for details. It was used for a proof of concept demo for DynBlock and that stuff. It is probably not too useful anywhere else, because the only data I can send to my dynamically created block is here their instancenumber. One should use some kind of derived "message" the have the chance for a real contents.}
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

