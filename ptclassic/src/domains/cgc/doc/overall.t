.VR 0.$Revision$ "$Date$"
.EQ
delim off
.EN
.TI "CGC Domain"
.ds DO "CGC
.AU
S. Ha
.AE
.H1 "Introduction"
.pp
The
.c CGC
domain is used to generate C code. The generated c-code is
compiled, downloaded, and run in the target machine, by default
the \*(PT host machine. Before reading this document, please
read the CG domain document first for the common features and
general structure of code generation domains. In this chapter, we
will explain the features specific to the c-code generation.
.H1 "Single-Processor Target"
.pp
The default target,
.c CGCTarget 
.Id "CGCTarget, class"
class, is a single processor target. The target machine is specified
by the
.c hostMachine
.Ir "hostMachine, parameter"
parameter, which is set to the \*(PT host machine by default.
The CGCTarget contains several other parameters.
The
.c funcName
.Ir "funcName, parameter"
parameter gives the name to the generated code. Though the default name is
"main", we need to assign a different name in case of multi-processor
targets. In a multi-processor target, it serves as only a child target that
produces a function, and the main function is generated in the parent
target. The generated code is written to a file that is named after the
program graph name appended by ".c". If the
.c saveFileName
.Ir "saveFileName, parameter"
parameter is given a file name, the generated code is also copied to
the file.
The compilation command is generated by three target parameters:
.c compileCommand (
.Ir "compileCommand, parameter"
\fIcc\fR, by default), 
.c compileOptions ,
.Ir "compileOption, parameter"
and 
.c linkOptions .
.Ir "linkOptions, parameter"
Then, the compile command will be
.(c
	compileCommand compileOptions fileName linkOptions
.)c
In the development stage, the user may want to examine the code before
compiling the code. If we set the parameter
.c doCompile
.Ir "doCompile, parameter"
NO, the generated code will not be compiled nor run.
.pp
Besides the
.c myCode
and the
.c procedures
code streams defined in the CGTarget class, the CGCTarget class
defines other code streams for proper code organization.
They are 
.c include ,
.c globalDecls ,
.c mainDecls ,
.c mainInit ,
.c wormIn ,
and
.c wormOut .
In the 
.c frameCode()
method, the code streams are arranged to form the final code.
The "include" code streams goes first, "globalDecls" and
"procedures" streams next,
the function definition (for example, main()), "mainDecls" stream,
"mainInit", and the main loop body of the code. The "wormIn" and
"wormOut" streams are placed at the beginning and at the end of
the main loop body respectively.
To add code strings to these code streams, we can use
.c getStream(stream-name)->put(code-string, optional name) .
For star writers, some methods are defined the 
.c CGCStar
class:
.c addInclude()
to include file names into the "include" code stream,
.c addGlobal()
to add codes into the "globalDecls" stream,
.c addDeclaration()
to add codes into the "mainDecls" stream, and
.c addMainInit()
to add codes into the "mainInit" stream.
.pp
One of the main task of the target is to allocate resources. In c-code
generation domain, the global names are the resources to be managed.
We need to assign a unique name for each block and their portholes, states,
and variable. One approach was to use a structure (
.c struct
) hierarchy that is the same as the hierachical dataflow program
representation. This approach was rejected since it takes away from the 
compiler any chance of optimization with these global variables. 
Instead, we assign a unique number to each star instance. Using that number
and the name of the galaxy that contains the star, we generate a unique name
for each star instance. All variables inside a star instance is
prepended by the unique name of that star instance.  
.pp
Another task of resource management is to determine the buffer size of
.Id "buffer-size, CGC domain"
each connection of blocks.  Let's consider an example: star A produces
two tokens and star B consumes three tokens per each firing. A simple
schedule is AABAB (another schedule might be AAABB, or 3A2B if loop
scheduling option is taken). The generated code will look like:
.(c
	for (,,) {
		star-A code
		star-A code
		star-B code
		star-A code
		star-B code
	}
.)c
The minimum buffer size of the connection
between A and B is 4. Unfortunately, the runtime cost of this 
minimum allocation is prohibitively high. First of all, at the second,
the starting location to write a token to the buffer, which is 3,
is not the same as that of the first iteration, 1. Thus, we need to
use an indirect addressing through an index pointer. Second, the second
invocation of B has to access the buffer locations in the following order:
4,1,2. It implies that we need to wrap around the index pointer after
checking that the pointer reaches the end of buffer location.
Hence, our decision is to sacrifice the buffer size to reduce the runtime
overhead. To remove the need of wrap-around indexing, we use "linear
buffering". And we use "static-buffering" to remove the need of
index pointers. For example, we can use the least common multiple (\fIlcm\fR)
value of the number of tokens produced and consumed on that connection.
In the above example, the \fIlcm\fR value is 6. Then, we achieve both
static buffering and linear buffering. Moreover, this choice of
buffer size is not a waste since we do not need two index pointers which
would be necessary if we allocate only 4. 
.pp
If the buffer size becomes too large compared with the minimum buffer
requirements, we may want to sacrifice the runtime efficiency to save
memory. It can be done by setting a target parameter
.c staticBuffering
.Ir "staticBuffering, parameter"
NO. If this parameter is set to YES (by default), we enforce static
and linear buffering if possible. If NO, we do not increase the buffer
size more than twice of the minimum requirement to reduce the runtime
overhead. 
.pp
If there is some delays (or initial tokens) on the connection, or
the consumer or producer wants to access the past samples, it is
usually impossible to achieve linear buffering. Even in those
cases, we try to achieve static buffering. In the generated code,
the user will see index pointers, porthole name appended by "_ix", if
we do not achieve static or linear buffering.

.H1 "Programming Stars in the CGC Domain:
.pp
Programming a CGC star is rather straightforward compared with other
code generation domain stars if there is a corresponding 
SDF star. It is recommended to use the same name for a code generation
star as the corresponding SDF star if functionality is the same:
same names for portholes and even for states. Then, we can retarget the
given application by just changing the domain in the graphical user
interface. Since the body of the simulation(SDF) star is coded in C++, and
the c-code generation star generates a C code, there is a great deal of
similarity between two stars. For example, let's consider
.c Ramp
star. First the Ramp star in the SDF domain looks like
.(c
defstar {
	name { Ramp }
	domain { SDF }
	desc {
Generates a ramp signal, starting at "value" (default 0)
with step size "step" (default 1).
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { step }
		type { float }
		default { 1.0 }
		desc { Increment from one sample to the next. }
	}
	defstate {
		name { value }
		type { float }
		default { 0.0 }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go {
		double t = value;
		output%0 << t;
		t += step;
		value = t;
	}
}
.)c
And the Ramp star in the CGC domain is as follows:
.(c
defstar {
	name { Ramp }
	domain { CGC }
	desc {
Generates a ramp signal, starting at "value" (default 0)
with step size "step" (default 1).
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { step }
		type { float }
		default { 1.0 }
		desc { Increment from one sample to the next. }
	}
	defstate {
		name { value }
		type { float }
		default { 0.0 }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go { addCode(std); }
	codeblock (std) {
	$ref(output) = $ref(value);
	$ref(value) += $val(step);
	}
}
.)c
The body of go() method of the 
.c SDFRamp 
star is translated into
the body of codeblock() macro in the 
.c CGCRamp 
star. The value of
the state \fBstep\fR is accessed by the 
.c val
macro, and the references of porthole 
.c output 
and state
.c value
are made by
.c ref
macro as explained in the CG domain documentation. In go() method
of the CGCRamp star, the code is added to a stream in the target
by addCode() method.
.pp
Our next example is the 
.c Add 
star in both domains. In the SDF domain,
.(c
defstar {
	name {Add}
	domain {SDF}
	desc { Output the sum of the inputs, as a floating value.  }
	inmulti {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	go {
		MPHIter nexti(input);
		PortHole *p;
		double sum = 0.0;
		while ((p = nexti++) != 0)
			sum += double((*p)%0);
		output%0 << sum;
	}
}
.)c
and in the CGC domain,
.(c
defstar {
	name {Add}
	domain {CGC}
	desc { Output the sum of the inputs, as a floating value.  }
	inmulti {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	state {
		name {ix}
		type { int }
		default { 1 }
		desc { index for multiple input trace }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	constructor {
		noInternalState();
	}
	go {
		StringList out;
		out << "\t$ref(output) = ";
		for (int i = 1; i <= input.numberPorts(); i++) {
			ix = i;
			out << "$ref(input#ix)";
			if (i < input.numberPorts()) out << " + ";
			else out << ";\n";
			addCode((const char*)out);
			out.initialize();
		}
	}
}
.)c
This example shows some difference between simulation stars and 
C-code generation stars. In the simulation star, we can access
each input port by \fIiterating\fR the input multi-ports. We can not
do the same thing in the generated C-code since each input porthole
is assigned a unique name and we have to list the names one by one.
Thus, the generated code will not contain any loop structure.
Each input porthole is access by 
.c $ref(input#ix)
macro, where ix is the internal state that is not visible to the
user. Using 
.c StringList
classes to form codes is the basic technique in the code generation domains.
By 
.c "<<"
operator of StringList class, we can concatenate strings, integers,
and floats conveniently. In the above example, examine
which code is formed in the "out" StringList.
.pp
As explained in the previous section, there are four methods to add
the code to the proper section besides 
.c addCode() 
and 
.c addProcedure()
explained in the CG documentation.
Here is an example of 
.c addGlobal()
.Ir "addGlobal, method"
call, which is found in 
.c CGCIIDUniform
star.
.(c
	initCode {
		addGlobal("double drand48();\n", "drand48");
		addCode(initSeed);
	}
	codeblock(initSeed) {
    srand48($val(seed));
	}
.)c
The library routine to generate a random number, \fIdrand48()\fR,
is declared in the global section of the generated code. To
make this declaration only once, we give the name "drand48" to
that code segment. The next code shows an example of 
.c addDeclaration()
.Ir "addDeclaration, method"
and 
.c addInclude()
.Ir "addInclude, method"
methods, which is quoted from 
.c CGCXgraph
star.
.(c
        initCode {
                addDeclaration("    FILE* $starSymbol(fp);");
                addInclude("<stdio.h>");
		...
	}
.)c
Since the
.c starSymbol
macro guarantees the uniqueness of the variable, we do not need to
give the unique name for that declaration statement. On the other
hand, a included file is always checked for uniqueness, so we don't
have to give a unique name. The 
.c addMainInit()
.Ir "addMainInit, method"
method is seldom used since we can get the same effect by using
.c addCode()
method in the 
.c initCode
section of the ptlang file. In other words, if we call addCode()
methods in the initCode(), those codes  are put into the
initialization section automatically. If some code should be unique
in the initialization section, use addMainInit() method with the
second argument that indicates the unique name of that code.
.pp
Unlike Ptolemy writtenin C++, the automatic 
.Id "complex type, CGC domain"
type conversion that C language supports is very restricted.
The type of porthole or state determine the type of the
corresponding variable in the generated code. Currently, we
support interger, float or double, string, and complex type.
the complex type is automatically generated as follows:
.(c
 typedef struct complex_data { double real; double imag; } complex;
.)c
Here is an example,
.c CGCRectToCx ,
to show how to use complex type variables.
.(c
defstar {
	name { RectToCx }
	domain { CGC }
	desc {Convert real and imaginary parts to a complex output.}
	input {
		name { real }
		type { float }
	}
	input {
		name { imag }
		type { float }
	}
	output {
		name { output }
		type { complex }
	}
	constructor {
		noInternalState();
	}
	codeblock(body) {
	$ref(output).real = $ref(real);
	$ref(output).imag = $ref(imag);
	}
	
	go {
		addCode(body);
	}
}
.)c
Since the output porthole is complex, we use
.c ref
macro appended by ".real" or ".imag" to access the real or imaginary
part of the predefined \fBcomplex\fR structure.
Type conversion between complex data types and integer or float data
types are automatically supported in the CGCTarget class by one extra
copying of buffers. The good thing is that the user doesn't have to
worry about type conversion. Moreover, the same application gragh
can be run in the simulation SDF domain and in the C-code
generation domain by just changing the domain in the user interface.
.pp
As shown with some examples, a CGC star is easy to write if there
is a corresponding SDF star. The best way to learn writing a new
star is to refer to the library stars.

.H1 "Multi-Processor Targets"
.pp
An examplary multi-processor target,
.c CGCMultiTarget ,
.Id "CGCMultiTarget, class"
is implemented in the target directory. It models multiple computers
working together, communicating with each other by Unix socket
mechanism. The computer names are given by a string state,
.c machineNames ,
.Ir "machineNames, parameter"
in which machine names are separated by a comma. 
The 
.c nameSuffix
.Ir "nameSuffix, parameter"
parameter indicates the common suffix of the machine names.
Since the child targets
are fixed to be the default CGCTargets, the
.c childType
parameter is declared as a non-settable state. To make socket connections,
we need to specify the port numbers for each computers. As a temporary
workaround, we provide a parameter, called
.c portNumber ,
in the target class. We use the consecutive port numbers starting 
from this number for connections. This workaround works as long as no
port number for a connection is already used in the pair of computers 
on the connection.
.pp
The send and receive stars are designed in the same directory:
.c CGCUnixSend
and
.c CGCUnixReceive
stars. When the sub-universes are created for child targets, the
send and receive stars are paired in the
.c pairSendReceive()
method in the target class. In that method, we specify the port number for
each connection, and insert the function calls for connection
establishment between computers in the 
.c mainInit
code stream of the child targets. The order of the connection establishments
between computers are crucial so that the parent target arranges them.
.pp
After codes are generated for each child target, the codes are written
to the files named after the child target name (refer to the MultiProcessor
Target section in the CG domain documentation for details). The files
are shipped to the actual computers via remote shell commands. They
are compiled and run there.

.H1 "Customized Targets
.pp
For a specific target, the user should design a target class.
Unlike a star, designing a target is supported by any preprocessor.
For a single processor target, make a target class derived from the
.c CGCTarget
class, and redefine some virtual methods in the target specific way.
The methods,
.c compileCode()
and
.c runCode() ,
are good candidates to be redefined.
For a multi-processor target, refer to the
.c CGCMultiTarget
class. The new target would be derived from 
.c CGMultiTarget
or
.c CGSharedBus
class depending on the interconnection topology.
The communication mechanism should be specified in 
.c createSend() ,
.c createReceive() ,
and
.c pairSendReceive()
methods. And how to write, compile and run the generated codes,
should be specified. Designing a customized multiprocessor target
is not cleanly defined in this release. Hopefully in the next
release, we will contain more examples of multi-processor
targets to be referred to.
.pp
Sometimes, we want to design a new domain for a new target.
For some digital signal processors, for example, we should generate a
special C-code. We have to redesign some library stars (to use
different library routines or to improve efficiency for a special
c-compiler). Then, using 
.c subDomain
.Ir "subDomain"
facility is helpful. An example of the "subDomain" is the
SDF domain inside the DDF domain. Design a new domain and let the CGC domain
be a subDomain. Then, all CGC stars can be used in the new Domain.
If stars with the same name as the CGC stars are designed in the new
domain, these stars will hide the CGC stars. 
For multi-processor targets which we are currently implementing
such as cm5 and dsp3, we design new domains and let the CGC domain
be a subDomain. We found out that if we need to redesign or add
quite a few stars, making a new Domain is a better choice.
In the next release, we will have some examples of this subDomain idea.
.EQ
delim $$
.EN
