static const char file_id[] = "DEDynFork.pl";
// .cc file generated from DEDynFork.pl by ptlang
/*
copyright (c) 1998 Dresden University of Technology, WiNeS-Project.
All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEDynFork.h"

const char *star_nm_DEDynFork = "DEDynFork";

const char* DEDynFork :: className() const {return star_nm_DEDynFork;}

ISA_FUNC(DEDynFork,DEDynForkBase);

Block* DEDynFork :: makeNew() const { LOG_NEW; return new DEDynFork;}

DEDynFork::DEDynFork ()
{
	setDescriptor("This a dynamic Fork function.\nOn demand a new porthole is added to or removed from a multiporthole during \nruntime. All particles to its input are passed to all outputs.");
	addPort(in.setPort("in",this,ANYTYPE));
	addPort(out.setPort("out",this,ANYTYPE));
	out.inheritTypeFrom(in);


}

void DEDynFork::go() {
# line 70 "DEDynFork.pl"
completionTime = arrivalTime;
        Particle& pp = in.get();
        OutDEMPHIter nextp(out);
        OutDEPort *oport;
        while ((oport = nextp++) != 0)
        oport->put(completionTime) = pp;
}


PortHole* DEDynFork::createPortHole (int)
{
# line 47 "DEDynFork.pl"
// create the new PortHole
            PortHole& single_out = out.newPort();
            
            return &single_out;
}


void DEDynFork::removePortHole (PortHole *delPort_p)
{
# line 59 "DEDynFork.pl"
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
static DEDynFork proto;
static RegisterBlock registerBlock(proto,"DynFork");
