defstar{
    name { DynForkBase }
    domain { DE }
    author { J. Voigt }
    version { @(#)DEDynForkBase.pl	1.4 04/29/98
   	    }   
    copyright { 
copyright (c) 1997-1998 Dresden University of Technology, WiNeS-Project.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.  
}
    desc { This star is the base class for a group of stars which implement
a dynamic fork function.     
    }
    htmldoc { 
This star and its childs work (only) in connection with 
<tt>DEDynMap</tt> and/or <tt>DEDynMapGr</tt>. The <i>createPortHole</i> and 
<i>removePortHole</i> methods are called from there, whenever a block is 
dynamically instantiated or deleted. The first porthole of the 
output-multiporthole, the childs of that star must have, has to be connected to a <tt>DEDynMap</tt> or <tt>DEDynMapGr</tt>-star. The next portholes in the 
output-multiporthole will be created dynamically, whenever a new block is put 
into the system configuration. They are deleted dynamically, whenever a block 
is taken from the system configuration.
<p>
This class is an abstract class, so it can't be instantiated and can not be used on its own. We here just declare methods, which are defined in its childs, in 
order to have access to them from <tt>DEDynMapBase</tt> and derived types. The 
childs of that star must have an input-porthole and a multiporthole as 
output-porthole. These portholes can be of different datatypes, so we provide 
this base class.
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

