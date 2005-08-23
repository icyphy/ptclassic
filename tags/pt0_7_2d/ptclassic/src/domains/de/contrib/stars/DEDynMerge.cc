static const char file_id[] = "DEDynMerge.pl";
// .cc file generated from DEDynMerge.pl by ptlang
/*
copyright (c) 1997 Dresden University of Technology, WiNeS-Project.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEDynMerge.h"

const char *star_nm_DEDynMerge = "DEDynMerge";

const char* DEDynMerge :: className() const {return star_nm_DEDynMerge;}

ISA_FUNC(DEDynMerge,DEStar);

Block* DEDynMerge :: makeNew() const { LOG_NEW; return new DEDynMerge;}

DEDynMerge::DEDynMerge ()
{
	setDescriptor("This star implements a dynamic merge function. On demand a new \nporthole is added to or removed from an input-multiporthole during runtime. \nAll particles to one of its inputportholes are sent out unchanged through the \nonly output. Because the particles are sent unchanged, I can use \nanytype-portholes here.");
	addPort(in.setPort("in",this,ANYTYPE));
	addPort(out.setPort("out",this,ANYTYPE));
	out.inheritTypeFrom(in);


}

void DEDynMerge::go() {
# line 66 "DEDynMerge.pl"
completionTime = arrivalTime;
        InDEMPHIter nextport(in);
        InDEPort* port_p;
        while ((port_p = nextport++) != 0) {
            if (port_p->dataNew) out.put(completionTime) = port_p->get();
        }
}


PortHole* DEDynMerge::createPortHole ()
{
# line 43 "DEDynMerge.pl"
// create the new PortHole
            PortHole& single_in = in.newPort();
            
            return &single_in;
}


void DEDynMerge::removePortHole (PortHole *delPort_p)
{
# line 55 "DEDynMerge.pl"
// first, remove the PortHole from the Block's PortHoleList
            if (this->removePort(*delPort_p)) {
                // now, remove it from the MultiPortHole's list
                if (in.removePort(delPort_p)) {
                    // once removed everywhere, we can really delete it
                    delete delPort_p;
                }
            }
}


// prototype instance for known block list
static DEDynMerge proto;
static RegisterBlock registerBlock(proto,"DynMerge");
