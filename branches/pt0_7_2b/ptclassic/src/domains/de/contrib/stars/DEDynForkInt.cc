static const char file_id[] = "DEDynForkInt.pl";
// .cc file generated from DEDynForkInt.pl by ptlang
/*
copyright (c) 1997 Dresden University of Technology, WiNeS-Project.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEDynForkInt.h"

const char *star_nm_DEDynForkInt = "DEDynForkInt";

const char* DEDynForkInt :: className() const {return star_nm_DEDynForkInt;}

ISA_FUNC(DEDynForkInt,DEDynForkBase);

Block* DEDynForkInt :: makeNew() const { LOG_NEW; return new DEDynForkInt;}

DEDynForkInt::DEDynForkInt ()
{
	setDescriptor("This a version of <tt>DEDynForkBase</tt> for integer portholes. \nOn demand a new porthole is added to or removed from a MultiPortHole during \nruntime. All particles to its input are checked for the right receiver and sent only to it.");
	addPort(in.setPort("in",this,INT));
	addPort(out.setPort("out",this,INT));

# line 60 "DEDynForkInt.pl"
receiverAddress_p = myList_h = myList_t = NULL;
}

DEDynForkInt::~DEDynForkInt() {
# line 67 "DEDynForkInt.pl"
// one never can rely on wrapup
        // the same stuff is done in DEDynBlock::clearList()
        while ( myList_h ) {
            receiverAddress_p = myList_h;
            myList_h = myList_h -> next;
            // delete it from the list
            delete receiverAddress_p;
        }
}

void DEDynForkInt::go() {
# line 164 "DEDynForkInt.pl"
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


PortHole* DEDynForkInt::createPortHole (int myName)
{
# line 82 "DEDynForkInt.pl"
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


void DEDynForkInt::removePortHole (PortHole *delPort_p)
{
# line 103 "DEDynForkInt.pl"
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


// prototype instance for known block list
static DEDynForkInt proto;
static RegisterBlock registerBlock(proto,"DynForkInt");
