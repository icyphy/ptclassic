.VR $Revision$	$Date$
.TI "CG56 Domain"
.ds DO "CG
.EQ
delim off
.EN
.AU
Joseph Buck
Jos\o"e\'" Pino
.AE
.H1 "Introduction"
.pp
.Id "CG56, domain"
.Id "CG96, domain"
In this chapter, we will document the basic functionality of \fBCG56\fR and 
\fBCG96\fR domains. We do this because both of these domains target similar 
assembly languages (Motorola 56000 and 96000 assembly). Additionally, 
56000-specific stars and targets will be described in this chapter -- 
96000-specific stars and targets are documented in CG96 chapter. The only 
difference in the two domains is that the CG96 domain also supports the 
float and float array data types.
.pp
.H1 "Writing a 56000/96000 Code Generation Star"
.pp
.Id "CG56Star"
.Id "CG96Star"
In this section, we will assume that the reader is familiar with the
``Writing a Code Generation'' section in the \fBCG\fR chapter.
.H2 "State"
.pp
.Id "state, assembly language specification"
The first thing a star write must decide on is the \fIstates\fR that a star 
will contain. In assembly language code generation domains, a star 
\fIstate\fR can either be a parameter that occupies no target memory or an 
internal state requiring target memory (such as tables and buffers). 
State specification for parameters 
is documented in the \fBPreprocessor\fR section. In this section, states 
requiring memory allocation are documented. A example of a state 
specification requiring memory follows:
.(c
state {
	name { array }
	type { fixarray }
	desc { a generic fix-type array }
	default { "-0.5 0.5 ONE" }
	attributes { A_YMEM }
}
.)c
.pp
Here the star writer has defined a fix type array state of length three.
Memory for this state will automatically be allocated in three consecutive
y-data memory locations.  Furthermore, code to initialize the array
to -0.5, 0.5, ONE will automatically be generated (where ONE is expanded
to the maximum fix point number in the range of \fI[-1,1)\fR).
.H3 "Type"
.Ir "type, CG56/CG96 state"
.pp
In the last example we demonstrated the use of only one of the valid
CG56 data types, the others are:
.(c
int
intarray
fix
fixarray
.)c
.pp
The addition CG96 data types are:
.(c
float
floatarray
.)c
.pp
.Ir "attributes, assembly code generation"
.H2 "Attributes"
.pp
While attributes are used in simulation stars, they are far more important 
in assembly code generation. Attributes can be thought of as assertions 
about the object they are applied to. Both states and portholes can have 
attributes. Attributes that apply to states have the prefix ``A_''. 
Attributes that apply to portholes have the attribute ``P_''.
.pp
The following attributes may be used with any state, 
.UH "A_CONSTANT"
.Ir "attribute, A_CONSTANT"
The state value is not changed by the star's execution.
.UH "A_SETTABLE"
.Ir "attribute, A_SETTABLE"
The user may set the value of this state from a user interface.
.UH "A_NONCONSTANT"
.Ir "attribute, A_NONCONSTANT"
The state value is changed by the star's execution.
.UH "A_NONSETTABLE"
.Ir "attribute, A_NONSETTABLE"
The user may not set the value of this state from a user interface 
(e.g. edit-parameters doesn't show it).
.pp
.pp
Applying an attribute to an object implies that some bits are to be
``turned on'', and others are to be ``turned off''.  The underlying attribute
bits have names beginning with AB_ for states, and PB_ for portholes.
The only two bits that exist in all states are AB_CONST and AB_SETTABLE.
By default, they are on for states, which means that the default state
works like a parameter (you can set it from the user interface, and
the star's execution does not change it).
.pp
For code generation, a whole new set of attributes are defined.
First, the attribute bits are:
.UH AB_CIRC
.Id "AB_CIRC attribute"
If set, the memory for this state is allocated as a circular buffer, whose 
address is aligned to the next power of two greater than or equal to its 
length
.UH "AB_CONSEC"
.Id "attribute, AB_CONSEC"
.Id "AB_CONSEC attribute"
If set, allocate the memory for the NEXT state in this star consecutively, 
starting immediately after the memory for this star.
.UH "AB_MEMORY "
.Id "attribute, AB_MEMORY"
If set, memory is allocated for this state.
.UH "AB_NOINIT"
.Id "attribute, AB_NOINIT"
If set, the state is not be automatically initialized.  The
default is that all states that occupy memory are initialized to their
default values.
.UH "AB_REVERSE"
.Id "attribute, AB_REVERSE"
If set, write out the values for this state in reverse order.
.UH "AB_SYMMETRIC"
.Id "attribute, AB_SYMMETRIC"
If set, and if the target has dual data memory banks (e.g. M56000, Analog 
Devices 2100, etc), allocate a buffer for this object in BOTH memories.
.pp
.pp
Given these bits, attributes correspond to requests to turn some bits
off and to turn other bits on.  For example:
.UH "A_ROM"
.Id "attribute, A_ROM"
Allocate memory for this state in memory, and the value will not change -- 
AB_MEMORY and AB_CONSTANT on. 
.UH "A_RAM"
.Id "attribute, A_RAM"
AB_MEMORY on, AB_CONST off.
.pp
.pp
In addition, for each attribute bit AB_xxx, A_xxx is an attribute that
turns AB_xxx on.
.pp
For portholes in code generation stars, we have:
.UH "PB_CIRC"
.Id "attribute, PB_CIRC"
If set, then allocate the buffer for this porthole as a circular buffer, 
even if this is not required because of any other consideration.
.UH "PB_SHARED"
.Id "attribute, PB_SHARED"
Equivalent to AB_SHARED, only for portholes.
.pp
For 56000 and 96000 stars, we define the following additional attributes:
.pp
.UH A_XMEM
.Id "attribute, A_XMEM"
Allocate this state in X memory
.UH A_YMEM
.Id "attribute, A_YMEM"
Allocate this state in Y memory
.pp
.pp
The underlying bits are AB_XMEM, and AB_YMEM.  Each attribute above turns
one off and turns the other on.
.pp
Also for 56000 and 96000 stars, portholes can assert attributes P_XMEM and 
P_YMEM, which work in exactly the same way. The default attribute for a 
56000 and 96000 portholes is P_XMEM, which allocates the porthole's buffer 
in X memory. Specifying the P_YMEM attribute puts the porthole's buffer in 
Y memory. 
.pp
Attributes can be combined with the "|" operator.  For example,
to allocate a ROM state in X memory, I can say
.(c
	state {
		name { foo }
		type { fix }
		default { "-1.0" }
		attributes { A_ROM|A_XMEM }
	}
.)c
.H2 "Code Blocks"
.Ir "code blocks, assembly language"
.pp
Here is an example codeblock, from the BiQuad star:
.(c
	codeblock (std) {
	move	#$addr(d1),r0
	move	#$addr(state),r5
	move	$ref(input),a
	rnd	a		x:(r0)+,x0	y:(r5)+,y0
	mac	-x0,y0,a	x:(r0)+,x0	y:(r5),y1
	mac	-x0,y1,a	x:(r0)+,x0	y0,y:(r5)-
	mac	x0,y0,a		x:(r0)+,x0	a,y:(r5)
	mac	x0,y1,a
	move	a,$ref(output)
	}
.)c
.pp
Notice several things about this codeblock. First off, explicit registers 
are being used. Eventually, we will move over to a style where Ptolemy will 
support the register allocation for you; we don't have it yet, so for now, 
porthole buffers are in memory and registers must be assumed to be purged 
between star calls. A star, inside of its execution code, is allowed to
use any of the registers available.  However, it is assumed that the star
will restore any modifier(m) registers that are changed back to ``-1'' 
before exiting the star.  Second, notice that 
there are macros named $addr and $ref. These are used to refer to states 
and portholes in the code.
.pp
Here we will present the additional and redefined macros 
available that have special meaning in assembly language code generation:
.UH "$addr(name,<offset>)"
.Id "macro, addr"
This macro returns the numeric address in memory of the named object,
without anything like (for the 56000) an ``x:'' or ``y:'' prefix.  If the
given quantity is allocated in a register (not yet supported) this
function returns an error.  It is also an error if the argument is
undefined or is a state that is not assigned to memory (e.g. a parameter).
.pp
Note that this does NOT necessarily return the address of the beginning of
a porthole buffer; it returns the ``access point'' to be used by this
star invocation, and in cases where the star is fired multiple times,
this will typically be different from execution to execution.
.pp
If the optional argument offset is specified, the macro returns an 
expression that references the location at the specified offset -- wrapping 
around to the beginning of the buffer if that is necessary. Note that this 
wrapping works independently of whether the buffer is circularly aligned or 
not. 
.UH "$ref(name,<offset>)"
.Ir "macro, ref (assembly)"
This macro is much like $addr(name), only the full expression used to refer 
to this object is returned, e.g. ``x:23'' for a 56000 if name is in x 
memory. If name is assigned to a register, this expression will return the 
corresponding register. The error conditions are the same as for $addr. 
.UH "$mem(name)"
.Id "macro, mem"
.H2 "Assembly PortHoles"
.Ir "AsmPortHole, class"
Here are some methods of class AsmPortHole that might be useful in
code generation stars:
.UH "bufSize()"
.Id "bufSize, method"
Returns an integer, the size of the buffer associated with the porthole.
.UH "baseAddr()"
.Id "baseAddr, method"
Returns the base address of the porthole's buffer
.UH "bufPos()"
.Id "bufPos, method"
Returns the offset position in the buffer, which ranges from 0 to bufSize()-1.
.UH "circAccessThisTime()"
.Id "circAccessThisTime, method"
This method returns true (nonzero) if the data to be read or written on 
this execution "wrap around", so that accessing them in a linear order will 
not work. 
.EQ
delim $$
.EN
