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
This is implemented by replacing the Chain star with instances of
the named blocks at setup time.  The replacement block(s) are connected
as specified by "input_map", "internal_map", and "output_map".
Their parameters are determined by "parameter_map".
If "pipeline" is YES then a unit delay is put on all internal connections.
	}
	explanation {
This star is a higher-order function mechanism.
.IE "higher-order functions"
See the
.c Map
documentation for background information.
.UH "Number of replacement blocks"
.pp
The star is replaced by one or more instances of the block with
name given by \fIblockname\fR at setup time, before the scheduler is invoked.
The number of instances of the replacement block
is given by the \fIchain_length\fR parameter.
If the named block is not on the knownlist (e.g., it is not a built-in
block), then the where_defined parameter is taken to be the full
path and filename of facet that should be compiled to define the block.
This path name may (and probably should) begin with the environment
.EQ
delim off
.EN
variable $PTOLEMY or ~username.
.EQ
delim $$
.EN
.UH "Connections"
.pp
The input and output connections specified by \fIinput_map\fR
and \fIoutput_map\fR are made to the first and last blocks in
the chain.
The internal connections are made as specified by the
\fIinternal_map\fR parameter.
This parameter should consist of an alternating list of
output names and input names for the replacement block.
As with
.c Map ,
if inputs or outputs are multiple, repeated names can be used.
.UH "Setting parameter values"
.pp
The \fIparameter_map\fR parameter can be used to set parameter values
for the replacement blocks.  The syntax is the same as in the
.c Map
star.
.UH "A note about data types"
.pp
The output data type is inherited from the input data type.
	}
	inmulti {
		name {input}
		type {anytype}
		desc {
Whatever is connected to these inputs will  be connected
to the first named block inputs according to input_map.
                }
	}
	outmulti {
		name {output}
		type {=input}
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
	setup {
            HOFBaseHiOrdFn::setup();
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
		Block* newblock;
		if(!(newblock = createBlock(*mom,
				       (const char*)blockname,
				       (const char*)where_defined)))
		   return;
		if(!(connectInternal(block,newblock))) return;

		// The previous block is now fully connected.  Initialize it.
		if(!setParams(block, instno)) return;
		block->setTarget(target());
		block->initialize();

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
			"maybe output_map contains unrecognized name: ",
			output_map[outputno-1]);
		    return;
		}
		if(!connectOutput(po,source)) return;
	    }

	    // The last block is now fully connected.  Initialize it.
	    if(!setParams(block, (int)chain_length)) return;
	    block->setTarget(target());
	    block->initialize();

	    mom->deleteBlockAfterInit(*this);
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
		    port1->connect(*port2,numdelays);
		}
		// No need to initialize delays because block1 has not
		// been initialized yet.
		return 1;
	    }
	}
}
