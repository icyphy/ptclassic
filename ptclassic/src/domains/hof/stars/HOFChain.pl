defstar {
	name {Chain}
	domain {HOF}
	derivedFrom {BaseHiOrdFn}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	desc {
Create one or more instances of the named block connected in a chain.
This is implemented by replacing the <tt>Chain</tt>
star with instances of the named blocks at preinitialization time.
The replacement block(s) are connected as specified by
<i>input_map</i>, <i>internal_map</i>, and <i>output_map</i>.
Their parameters are determined by <i>parameter_map</i>.
If <i>pipeline</i> is YES then a unit delay is put on all internal connections.
	}
	htmldoc {
This star is a higher-order function mechanism.
<a name="higher-order functions"></a>
See the
<tt>Map</tt>
documentation for background information.
<h3>Number of replacement blocks</h3>
<p>
The star is replaced by one or more instances of the block with name
given by <i>blockname</i> at preinitialization time,
before the scheduler is invoked.
The number of instances of the replacement block is given by the
<i>chain_length</i> parameter.
If the named block is not on the knownlist (e.g., it is not a built-in block),
then the <i>where_defined</i> parameter is taken to be the full path name and
filename of the facet that should be compiled to define the block.
This path name may (and probably should) begin with the environment
variable $PTOLEMY or ~username.
<h3>Connections</h3>
<p>
The input and output connections specified by <i>input_map</i> and
<i>output_map</i> are made to the first and last blocks in the chain.
The internal connections are made as specified by the <i>internal_map</i>
parameter.
This parameter should consist of an alternating list of output names and
input names for the replacement block.
As with the
<tt>Map</tt>
star, if inputs or outputs are multiple, repeated names can be used.
<h3>Setting parameter values</h3>
<p>
The <i>parameter_map</i> parameter can be used to set parameter values
for the replacement blocks.
The syntax is the same as in the
<tt>Map</tt>
star.
	}
	inmulti {
		name {input}
		type {anytype}
		desc {
Whatever is connected to these inputs will be connected
to the first named block inputs according to input_map.
                }
	}
	outmulti {
		name {output}
		type {anytype}
		desc {
Whatever is connected to these outputs will be connected
to the last named block outputs according to output_map.
		}
	}
	defstate {
		name {chain_length}
		type {int}
		default {"2"}
		desc {The length of the chain}
	}
	defstate {
		name {input_map}
		type {stringarray}
		default {"input"}
		desc {The mapping of inputs}
	}
	defstate {
		name {output_map}
		type {stringarray}
		default {"output"}
		desc {The mapping of outputs}
	}
	defstate {
		name {internal_map}
		type {stringarray}
		default {"output input"}
		desc {The internal connections between blocks}
	}
	defstate {
		name {pipeline}
		type {int}
		default {"NO"}
		desc {If YES, put a delay on each internal connection}
	}
	ccinclude { "Galaxy.h" }
	ccinclude { "SimControl.h" }

	method {
	  name { preinitialize }
	  access { public }
	  code {
	    HOFBaseHiOrdFn::preinitialize();

	    // Make sure we know the number of connections on the
	    // input and output multiPortHoles.
	    initConnections(input);
	    initConnections(output);

	    // Check that all maps are the right size
	    int allsizes = input.numberPorts();
	    if (input_map.size() != allsizes ||
		output_map.size() != allsizes ||
		internal_map.size() != allsizes*2 ||
		output.numberPorts() != allsizes) {
		    Error::abortRun(*this,
			"Number of inputs, outputs, and internal connections"
			" need to be the same");
		    return;
	    }
	    // Get the parent galaxy
	    Galaxy* mom = idParent();
	    if(!mom) return;

	    // Create the first block
	    Block* block = createBlock(*mom,
				       (const char*)blockname,
				       (const char*)where_defined);
	    if (!block) return;

	    // Connect the inputs to the first block
	    int inputno = 0;
	    MPHIter nexti(input);
	    PortHole *pi;
	    while ((pi = nexti++) != 0) {
		GenericPort *dest;
		if (!(dest = block->genPortWithName(input_map[inputno++]))) {
		    Error::abortRun(*this,
			"input_map contains unrecognized name: ",
			input_map[inputno-1]);
		    return;
		}
		if(!connectInput(pi,dest)) return;
	    }

	    // Create the rest of the blocks and the internal connections
	    for (int instno = 1; instno < (int)chain_length; instno++) {
		if (SimControl::haltRequested())
		  return;
		Block* newblock = createBlock(*mom,
					      (const char*)blockname,
					      (const char*)where_defined);
		if (!newblock) return;
		if(!connectInternal(block,newblock)) return;
		// set params of prior block just before forgetting it
		if(!setParams(block, instno)) return;
		block = newblock;
	    }

	    // Connect the outputs to the last block
	    int outputno = 0;
	    MPHIter nexto(output);
	    PortHole *po;
	    while ((po = nexto++) != 0) {
		GenericPort *source;
		if (!(source = block->genPortWithName(output_map[outputno++]))) {
		    Error::abortRun(*this,
			"output_map contains unrecognized name: ",
			output_map[outputno-1]);
		    return;
		}
		if(!connectOutput(po,source)) return;
	    }

	    // set params of last block
	    if(!setParams(block, (int)chain_length)) return;

	    mom->deleteBlockAfterInit(*this);
	  }
	}

	method {
	    name { connectInternal }
	    type { int }
	    access { protected }
	    arglist { "(Block* block1, Block* block2)" }
	    code {
		PortHole *port1, *port2;
		int i = 0;
		int numdelays = 0;
		if ((int)pipeline) numdelays = 1;
		while (i < internal_map.size()-1) {
		    if (!(port1 = block1->portWithName(internal_map[i++]))) {
			Error::abortRun(*this,
			    "internal_map contains unrecognized name: ",
			    internal_map[i-1]);
			return 0;
		    }
		    if (!(port2 = block2->portWithName(internal_map[i++]))) {
			Error::abortRun(*this,
			    "internal_map contains unrecognized name: ",
			    internal_map[i-1]);
			return 0;
		    }
		    connectPorts(*port1,*port2,numdelays,NULL);
		}
		return 1;
	    }
	}
}
