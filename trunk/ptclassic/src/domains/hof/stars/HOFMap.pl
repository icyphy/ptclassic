defstar {
	name {Map}
	domain {HOF}
	derivedfrom {BaseHiOrdFn}
	version { @(#)HOFMap.pl	1.12 11/13/97 }
	author { Edward A. Lee, Tom Lane  }
	location { HOF main library }
	copyright {
Copyright (c) 1994-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	desc {
Map one or more instances of the named block to the input stream(s)
to produce the output stream(s).
This is implemented by replacing the Map star with the named block at
preinitialization time.
The replacement block(s) are connected as specified by "input_map"
and "output_map", using the existing connections to the Map star.
Their parameters are determined by "parameter_map".
	}
	htmldoc {
See the documentation for the
<tt>BaseHiOrdFn</tt>
star, from which this is derived, for background information.
The star is replaced by one or more instances of the block with name given
by <i>blockname</i> at preinitialization time, before the scheduler is invoked.
<h3>Number of replacement blocks</h3>
<p>
The number of instances of the replacement block
is determined by the number of input or
output connections that have been made to the
<tt>Map</tt>
star.
Suppose the
<tt>Map</tt>
star has <i>M <sub>I</sub></i> inputs and <i>M <sub>O</sub></i> outputs connected to it.
Suppose further that the replacement block has <i>B <sub>I</sub></i> input
ports and <i>B <sub>O</sub></i> output ports.
Then
<pre>
N = M<sub>I</sub> / B<sub>I</sub> = M<sub>O</sub> / B<sub>O</sub>
</pre>
is the number of instances that will be created.
It is an error for this second equality not to be satisfied.
<h3>How the inputs and outputs are connected</h3>
<p>
The first <i>B <sub>I</sub></i> connections to the
<tt>Map</tt>
star will be connected to the inputs of the first instance of the
replacement blocks.
The names of the inputs to the replacement block should be listed
in the <i>input_map</i> parameter in the order in which they should
be connected.
There should be exactly <i>B <sub>I</sub></i> names in the <i>input_map</i> list.
The next <i>B <sub>I</sub></i> connections will be connected to the next
replacement block, again using the ordering specified in <i>input_map</i>.
Similarly for the outputs.
If there are no inputs at all, then the number of instances
is determined by the outputs, and vice versa.
<h3>Substituting blocks with multiple inputs or outputs</h3>
<p>
When the replacement block has a multiple input port or a multiple
output port, the name given in the input_map parameter should be
the name of the multiple port, repeated for however many instances
of the port are desired.
For example, the
<tt>Add</tt>
block has a multiple input port named "input".
If we want the replacement block(s) to have two inputs,
then <i>input_map</i> should be "input input".
If we want three inputs in each replacement block, then
<i>input_map</i> should be "input input input".
<h3>Bugs</h3>
<p>
Repeated names in the <i>input_map</i> or <i>output_map</i>
for non-multiple inputs are an error.
However, this is not detected.
Results could be unexpected.
	}

	inmulti {
		name {input}
		type {anytype}
		desc {
Whatever is connected to these inputs will be connected
to the named block inputs according to input_map.
		}
	}
	outmulti {
		name {output}
		type {anytype}
		desc {
Whatever is connected to these outputs will be connected
to the named block outputs according to output_map.
		}
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

	ccinclude { "SimControl.h" }

	method {
	  name { doExpansion }
	  type { int }
	  code {
	    // Check that either an input_map or output_map is given
	    if (input_map.size() == 0 && output_map.size() == 0) {
		Error::abortRun(*this,
		    "Must specify at least one of input_map or output_map");
		return 0;
	    }
	    // Make sure we know the number of connections on the
	    // input and output multiPortHoles.
	    if (! initConnections(input)) return 0;
	    if (! initConnections(output)) return 0;

	    MPHIter nexti(input);
	    MPHIter nexto(output);

	    if (!iterateOverPorts(nexti,nexto,1))
	      return 0;

	    return 1;
	  }
	}

	// The following method is separated out so that MapGr stars
	// can iterate over all but the first replacement block.
	method {
	  name { iterateOverPorts }
	  type { int }
	  access { protected }
	  arglist { "(MPHIter &nexti, MPHIter &nexto, int firstInstanceNo)" }
	  code {
	    int instanceno = firstInstanceNo;

	    Galaxy* mom = idParent();
	    if (!mom) return 0;

	    // The following loop creates however many instances of the block
	    // are required to connect all the inputs and outputs.  The number
	    // of inputs and number of outputs of the Map star must be an
	    // integer multiple of the number of inputs and outputs of the block
	    while (!SimControl::haltRequested()) {

	      // Check to see whether we are out of inputs or outputs
	      PortHole *pi, *po;
	      if ((pi = nexti++) == 0 && input_map.size() != 0) {
		// Out of inputs.  Better be out of outputs as well.
		if (nexto++) {	
		  Error::abortRun(*this,
				  "Too many outputs for the number of inputs");
		  return 0;
		}
		break;
	      }

	      // Not out of inputs.  Had better not be out of outputs either
	      if ((po = nexto++) == 0 && output_map.size() != 0) {
		if (input_map.size() == 0) {
		  break;
		} else {
		  Error::abortRun(*this,
				  "Too many inputs for the number of outputs");
		  return 0;
		}
	      }

	      Block* block = createBlock(*mom,
					 (const char*)blockname,
					 (const char*)where_defined);
	      if (!block) return 0;

	      int i;
	      // Connect the inputs, if any.
	      for (i=0; i < input_map.size(); i++) {
		if (i != 0 && (pi = nexti++) == 0) {
		  Error::abortRun(*this,
				  "number of inputs is not a multiple of ",
				  "the input_map size");
		  return 0;
		}
		// Reconnect
		GenericPort *dest;
		if (!(dest = findPortWithName(block, input_map[i]))) {
		    Error::abortRun(*this,
			"input_map contains unrecognized name: ",
			input_map[i]);
		    return 0;
		}
		if(!connectInput(pi,dest)) return 0;
	      }

	      // Connect the outputs, if any
	      for (i=0; i < output_map.size(); i++) {
		if (i != 0 && (po = nexto++) == 0) {
		  Error::abortRun(*this,
				  "number of outputs is not a multiple of ",
				  "the output_map size");
		  return 0;
		}
		// Reconnect
		GenericPort *source;
		if (!(source = findPortWithName(block, output_map[i]))) {
		    Error::abortRun(*this,
			"output_map contains unrecognized name: ",
			output_map[i]);
		    return 0;
		}
		if(!connectOutput(po,source)) return 0;
	      }
	      if(!setParams(block, instanceno++)) return 0;
	    }
	    return 1;
	  }
	}
}
