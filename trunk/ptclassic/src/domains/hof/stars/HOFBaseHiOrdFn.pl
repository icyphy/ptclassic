defstar {
	name {BaseHiOrdFn}
	domain {HOF}
	derivedFrom {Base}
	desc {
This is a base class containing utility methods shared by HOF stars,
and defining their common functionality.
	}
	explanation {
This star provides the base class for a family of \fIhigher-order functions\fR
in Ptolemy.
.IE "higher-order functions"
A higher-order function is a function that takes a function as
an argument and/or returns a function.
Stars and galaxies in Ptolemy have two kinds of arguments:
signals and parameters.
The higher-order functions supported by this base class
take functions as parameters, not signals.
The basic mechanism is that a star or galaxy is statically
specified, and the higher-order star replaces itself with
one or more instances of the specified replacement block.
.pp
The name of the replacement block is given by the \fIblockname\fR parameter.
If the replacement block is a galaxy, then the \fIwhere_defined\fR parameter
gives the full name (including the full path) of the definition of the galaxy.
This path name may (and probably should) begin with the environment
.EQ
delim off
.EN
variable $PTOLEMY or ~username.
.EQ
delim $$
.EN
Currently, this must be an oct facet, although in the future, other
specifications (like ptcl files) may be allowed.
The oct facet should contain the replacement galaxy,
or it could simply be the facet that defines the replacement galaxy.
If the replacement block is a built-in star, then there
is no need to give a value to the \fIwhere_defined\fR parameter.
.pp
For all higher-order stars derived from this base class, the replacement
blocks are substituted for the higher-order star at setup time.
Thus, the scheduler will never see the higher-order star.
For this reason, these stars can be used in any Ptolemy domain,
since their semantics are independent of the model of computation.
.pp
The stars derived from this base class differ mostly in the way
that replacement blocks get wired into the graph.
They share a common mechanism for specifying the value
of parameters in the replacement block.
.UH "Setting parameter values"
.pp
The \fIparameter_map\fR parameter can be used to set parameter values
for the replacement blocks.
There are four acceptable forms:
.(c
name value
name(number) value
name = value
name(number) = value
.)c
There should be no spaces between "name" and "(number)", and the name
cannot contain spaces, "=", or "(".
In all cases, \fIname\fR is the name of a parameter in the
replacement block.
In the first case, the value is applied to all instances
of the replacement block.
In the second case, it is applied only to the specified instance
number (which starts with 1).  The third and fourth cases just
introduce an optional equal sign, for readability.
If the "=" is used, there must be spaces around it.
.pp
The value can be any usual Ptolemy expression for giving the value
of a parameter.
However, if the string "instance_number" appears anywhere in the
expression, it will be replaced with the instance number of the
replacement block.
Note that it need not be a separate token.
For example, the value "xxxinstance_numberyyy" will become "xxx9yyy"
for instance number 9.
After this replacement, the value is evaluated using the usual
Ptolemy expression evaluator for initializing states.
	}
	version {$Id$}
	author { E. A. Lee  }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	defstate {
		name {blockname}
		type {string}
		default {"Gain"}
		desc {The name of the replacement block}
	}
	defstate {
		name {where_defined}
		type {string}
		default {""}
		desc {
The full path and facet name for the definition of blockname.
		}
	}
	defstate {
		name {parameter_map}
		type {stringarray}
		default {""}
		desc {The mapping of parameters}
	}
	location { HOF main library }
	method {
	    name { setParams }
	    type { int }
	    access { protected }
	    arglist { "(Block* block, int instanceno)" }
	    code {
		// Set the specified parameter values.
		for (int i = 0; i < parameter_map.size()-1; i++) {

		    const char* parameter_name = parameter_map[i++];

		    // Check for the optional "=".  Ignore it.
		    if (strcmp(parameter_map[i],"=") == 0) {
		      if(i < parameter_map.size()-1) i++;
		    }

		    if (!setParameter(block, instanceno, parameter_name,
				      (const char*)parameter_map[i])) {
		      return 0;
		    }
		}
		return 1;
	    }
	}
}
