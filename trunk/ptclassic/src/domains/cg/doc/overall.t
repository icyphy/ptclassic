.VR $Revision$ $Date$
.TI "CG Domain"
.AU
E. A. Lee
.AE
.H1 "Introduction"
.pp
The
.c CG
domain and derivative domains are used to generate code rather
than run simulations.
Only the derivative domains are of practical use.
The
.c CG
domain itself, in fact, is not selectable,
and has no stars.
It is used exclusively as a model and a collection of base classes
for derivative domains.
This section, therefore, documents the common features and general
structure of all code generation domains.
.pp
The principle mechanism is to define
.c codeblock s
that are processed and written to a code file, which can then
be compiled or assembled.
These code blocks can be parsed and edited in ways that depend
on the target processor.  In the generic
.c CG
domain, no such editing occurs.
.pp
A key feature of code generation domains is the notion of a target
architecture.
Every application must have a user-specified target
architecture, selected from a set of architectures supported
by the user-selected domain.
Every target architecture is derived from the base class
.c Architecture,
and controls such operations as scheduling, compiling, assembling,
and downloading code.
Since it controls scheduling, multiprocessor architectures can be
supported with automated task partitioning and synchronization.
.H1 "Codeblocks
.pp
The preprocessor
.c ptlang
supports
.c CG
domains.
Suppose you wish to define a star that generates assembly code for
the Motorola DSP56001 that adds two numbers stored in memory,
and then writes the result back to memory.
(Note that this would be a very poor code generator, since it takes
control of memory allocation.  But it serves as a simple example.)
Assume the memory locations used have addresses 10, 11, and 12,
respectively, and lie in the "x" memory bank.
This can be done as follows:
.(c
defstar {
	... \fIstar definition directives\fP ...
	codeblock (blockname) {
		move x:10,a
		move x:11,x0
		add x0,a
		move a,x:12
	}
	go {
		gencode(blockname);
	}
}
.)c
The
.c codeblock
directive defines a block of code with an associated identifying name
("blockname" in this case).
The go() method then generates code based on this code block.
In the generic
.c CG
domain (which is of little practical use directly) the gencode() method
produces the code block
verbatim.
In a practical code generation domain based on
.c CG,
the code would be processed in some way first.
For instance, memory and registers would be allocated, code would
be compacted, etc.
.pp
A codeblock consists of lines of code.
Within each line, spaces, tabs, and the
newline are important, and are preserved.
For this reason,
the brackets "{ }" should not be on the same lines with the code.
The following code, for example, will not yield the expected result:
.(c
	codeblock (blockname) { move x:10,a
		move x:11,x0
		add x0,a
		move a,x:12 }
.)c
It is bad form for two reasons.
First, spaces and tabs between the opening "{"
and the first non-space character will be lost.
Second, the code on the last line will be lost.
Anything preceding the closing "}" on the same line is discarded.
.pp
The above
.c codeblock
directive is translated by the preprocessor (ptlang) into a definition
of a static member in the ``.h'' file:
.(c
class CGStarname : public CGStar
{
	...
	static CGCodeBlock blockname;
	...
}
.)c
An associated constructor call will appear in the ``.cc'' file:
.(c
CGCodeBlock CGStarname :: blockname (
"	move x:10,a\n"
"	move x:11,x0\n"
"	add x0,a\n"
"	move a,x:12\n"
);
.)c
The argument is a single string, divided into pieces for convenience.
In a nontrivial code generator, the constructor for the CodeBlock class
could parse this string and create an internal data structure which
is later used by the gencode() method.
The details of this data structure depend highly on the target
processor or language of the generated code.
.pp
The "blockname" argument to the codeblock directive identifies the particular
block.  Any number of blocks can be defined, and conditional code
generation is easily accomplished, as in the following example:
.(c
	go {
		if (parameter == YES)
			gencode(yesblock);
		else
			gencode(noblock);
	}
.)c
.H1 "Architectures"
.pp
A code generation
.c Domain
is specific to the language generated,
such as C, DSP56000 assembly code, and Sproc assembly code.
A target
.c Architecture,
by contrast, is specific to the hardware that will run the code.
A given language, particularly a generic language such as C,
may run on many target architectures.
Code generation functions are therefore cleanly divided
between the domain and the architecture.
.pp
All target architectures are derived from the base class
.c Architecture.
This base class has static members that store pointers
to all known architectures.
Merely creating an instance of a class derived from
.c Architecture
will add that instance to the list of pointers.
Hence, the base class maintains the list of known architectures.
.pp
Like a domain, an architecture
has a scheduler with a setup() and run() method.
The setup() method invokes the go() functions of all the stars
in order to generate code, and supervises the compilation or assembly
of that code.
The run() method downloads the code into the target hardware
(or a simulation of the target hardware) and executes it.
