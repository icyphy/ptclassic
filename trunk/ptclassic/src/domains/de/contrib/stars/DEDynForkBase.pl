defstar{
    name { DynFork }
    domain { DE }
    author { J. Voigt }
    version { 1.0 8/26/97 
   	    }   
    copyright { copyright (c) 1997 Dresden University of Technology
                WiNeS-Project }
    desc { On demand a new PortHole is added to or removed from a MultiPortHole during runtime. All particles on one of its inputs are checked for the right
receiver and sent only to it. }
    explanation { 
This star works (only) in connection with DEDynBlock. The "createPortHole()" 
and "removePortHole()" methods are called from there, whenever a block is dynamically instantiated or deleted.

It is an abstract base class. Derived stars must have portholes of a special type. (I can't overload a porthole's datatype in derived stars.)

In "createPortHole()" and "removePortHole()" I again work with the same list-
technique as in DEDynBlock in order to find the right porthole using the
receiver's address.
}
    location { $PTOLEMY/src/domains/de/contrib/stars }
    header {
        // we need a list which contains the addresses of all blocks 
        // and their portholes
        struct receiverAddress {
            int address;
            PortHole *myPort_p;
            receiverAddress *next;
        };     
    }
    pure virtual method {
        name { removePortHole }
        access { public }
        type { void }
        arglist { "(PortHole *delPort_p)" }
    }
    pure virtual method {
        name { createPortHole }
        access { public }
        type { "PortHole*" }
        arglist { "(int myName)" }
    }
}   

