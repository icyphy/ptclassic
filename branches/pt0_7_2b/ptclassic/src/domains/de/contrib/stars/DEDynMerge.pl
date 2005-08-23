defstar{
    name { DynMerge }
    domain { DE }
    author { J. Voigt }
    version { 1.0 9/4/97
        }
    copyright { copyright (c) 1997 Dresden University of Technology
                WiNeS-project }
    desc { On demand a new PortHole is added to or removed from a MultiPortHole during runtime. All particles to one of its inputportholes are sent out 
unchanged through the only output. Because the particles are sent unchanged, 
I can use anytype-portholes here. }
    explanation { 
This star works (only) in connection with DEDynBlock. The "createPortHole()" 
and "removePortHole()" methods are called from there, whenever a block is 
dynamically instantiated or deleted.
}
    location { $PTOLEMY/src/domains/de/contrib/stars }
    inmulti {
        name { in }
        type { anytype }
        desc { will get one PortHole for each instance }
    }
    output {
        name { out }
        type { =in }
        desc { output for all dynamically created blocks }
    }
    method {
        name { createPortHole }
        access { public }
        type { "PortHole*" }
        code {
            // create the new PortHole
            PortHole& single_in = in.newPort();
            
            return &single_in;
        }
    } 
    method {
        name { removePortHole }
        access { public }
        type { void }
        arglist { "(PortHole *delPort_p)" }
        code {
            // first, remove the PortHole from the Block's PortHoleList
            if (this->removePort(*delPort_p)) {
                // now, remove it from the MultiPortHole's list
                if (in.removePort(delPort_p)) {
                    // once removed everywhere, we can really delete it
                    delete delPort_p;
                }
            } 
        }
    }
    go {
        completionTime = arrivalTime;
        InDEMPHIter nextport(in);
        InDEPort* port_p;
        while ((port_p = nextport++) != 0) {
            if (port_p->dataNew) out.put(completionTime) = port_p->get();
        }
    }
}   

