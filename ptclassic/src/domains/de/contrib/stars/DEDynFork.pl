defstar{
    name { DynFork }
    domain { DE }
    derivedfrom { DEDynForkBase }
    author { Jens Voigt }
    version { @(#)DEDynFork.pl	1.1 05/13/98 }
    copyright { 
copyright (c) 1998 Dresden University of Technology, WiNeS-Project.
All rights reserved.
    }
    desc { This a dynamic Fork function.
On demand a new porthole is added to or removed from a multiporthole during 
runtime. All particles to its input are passed to all outputs.
    }
    htmldoc {
<h3>Creating a porthole dynamically is as follows (<i>createPortHole</i>):</h3>
<p> 
This method is called from <tt>DEDynMapBase::createDynBlock</tt> whenever a new 
block is to put into the system configuration. It just creates a new porthole 
in the <i>out</i>-multiporthole. A pointer to the porthole is returned. The number of the porthole is not needed here, we just need the same interface as in the <tt>DEDynForkXXX</tt>-stars.
<p>        
<h3>Removing a porthole dynamically is as follows (<i>removePortHole</i>):</h3>
<p> 
This method is called from <tt>DEDynMapBase::deleteDynBlock</tt> whenever a 
block is to be taken out of the system configuration. From 
<tt>DEDynMapBase::deleteDynBlock</tt> we get a pointer to one of the portholes 
in our<i>out</i>-multiporthole. We delete it from the block's porthole list as well as from the multiporthole's list and delete the porthole itself.
}
    location { $PTOLEMY/src/domains/de/contrib/stars }
    acknowledge { I use exacly the same go()-method as S. Ha does in DEFork.pl.}
    input {
        name { in }
        type { anytype }
        desc { input for all events }
    }
    outmulti {
        name { out }
        type { =in }
        desc { will get one PortHole for each instance }
    }          
    method {
        name { createPortHole }
        access { public }
        type { "PortHole*" }
        arglist { "(int)" }
        code {
            // create the new PortHole
            PortHole& single_out = out.newPort();
            
            return &single_out;
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
                if (out.removePort(delPort_p)) {
                    // once removed everywhere, we can really delete it
                    delete delPort_p;
                }
            }
        } 
    }
    go {
        completionTime = arrivalTime;
        Particle& pp = in.get();
        OutDEMPHIter nextp(out);
        OutDEPort *oport;
        while ((oport = nextp++) != 0)
        oport->put(completionTime) = pp;
    }	
}  

