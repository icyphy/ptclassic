.VR $Id$
.TI "VHDLB Domain"
.ds DO "VHDLB
.AU
Edward A. Lee
Michael C. Williamson
Asawaree P. Kalavade 
.AE
.H1 "Introduction"
.pp

The VHDLB domain is one of two branch domains which support the VHDL
language in Ptolemy, the other being the VHDLF domain.  The VHDLF
domain is limited to stars and applications consisting of functional
VHDL actors, i.e. actors which specify only abstract operations on
data, and not implementation issues.  These actors depend only on the
simulation step number and do not have any real-time- or
clock-dependent behavior.  The semantics of the VHDLF domain are akin
to those of the SDF domain in that the behavior of actors depends only
on their inputs, parameters, and state, and firings occur at discrete
integer simulation step times.  The VHDLB domain is devoted to stars
and applications consisting of models of components which respond to
discrete events at arbitrary times, simulating real hardware elements.
The semantics of the VHDLB domain are akin to those of the DE domain
in that star firings are not constrained to discrete integer step
times, and all events are time-stamped.

.pp
.Id "VHDL"
.Id "VHDL"
.Id "VHDLB domain"
.Id "domain, VHDL"

VHDL is a timed, event-driven language with pre-emptive scheduling.
While primarily suited for specification and simulation of digital
electronic systems, the range of simulation levels allowed in VHDL
makes it possible to simulate systems at a wide range of levels of
abstraction, from the system block level down to the individual gate
level.  The pre-emptive nature of event scheduling allows multiple
outputs being driven onto a single signal line to have priority over
one another, although this is not supported in the current release.
While other hardware description languages, such as Verilog HDL, are
also in wide use, VHDL is standardized by the IEEE [1].  VHDL is a
recognized standard for hardware specification and simulation, and an
increasing number of vendors produce high-level design and synthesis
tools which take VHDL as an input language or an intermediate format.
As a standard, it finds usefulness as a format for switching between
different design tools, a capability which designers increasingly
demand from design tool vendors.

.pp

The VHDLB domain in Ptolemy currently supports only code generation,
without any support for simulation of the code or co-simulation with
other domains.  In future releases, enhancements will be made to
support interaction with VHDL simulators, as well as to support
co-simulation with the VHDLF domain and with simulation domains
through wormholes.  With the code generation capabilities included in
this release, the user can build up the library of VHDL stars and VHDL
code modules in order to manage structure and hierarchy in VHDL code
generation.  For a detailed specification of the VHDL language, the
user is referred to [1].  For a discussion of using VHDL in high-level
design, the user is referred to [2].

.pp
.H1 "Code Generation"

In the VHDLB domain, universes of interconnected stars correspond to
instances of VHDL entities connected via signal lines.  For each VHDLB
star, there is a predefined code block in the library which performs
the functionality of that VHDLB star when instantiated into a VHDL
design.  Galaxies can be constructed in the usual way from basic VHDLB
stars, and these correspond to hierarchical structures in VHDL code.
The connections between stars are generated as signals connecting VHDL
entities through ports.  The only datatype currently supported is
integers.  Most basic hardware elements only manipulate bits or words,
and integers are adequate for this purpose.  Bits are represented as
being of low logic value by the integer value of zero, and nonzero
integer values are used to represent high logic value (typically with
integer value unity).  Undefined or unknown logic values are not
currently supported, although particular integer values could be set
aside for this purpose in writing VHDL code.

.pp

When a complete VHDLB universe is run, the structure of the graph is
traversed and analyzed and a code file is built up which instantiates
all of the entities which occur in the graph.  The hierarchy of
galaxies is similarly represented as hierarchy in the generated code
file.  Arcs connecting VHDLB stars and galaxies in the graph are
represented as signals connecting between ports in the VHDL code.
Once the code generation is completed, the code is written to a file
with the name of the facet.  The directory in which the file is placed
is the directory shown as a parameter of the target when the
edit-target ("T") command is issued.  Finally, the complete code file
contents are displayed in a pop up window which allows the user to
examine and edit the code which has been generated.

.pp
.H1 "Delays"

While there is no concise delay operator in VHDL, delays can be
represented by registers or delay elements.  In the VHDLB domain,
delays are still necessary in feedback loops in order to satisfy the
scheduler which passes over the flowgraph prior to execution.  A delay
icon should be placed on feedback arcs in order to preclude delay-free
feedback loops.  The delay icon will not be reflected in the generated
code, therefore the user must take care to include components with
delay in the feedback loop in order to have VHDL code generated which
will not result in deadlock or infinite loops in execution.

.pp
.H1 "Wormholes"

Wormholes are not currently supported in the VHDLB domain.  Currently,
attempting to nest the VHDLB domain inside another domain or to nest
another domain inside the VHDLB domain will result in an error.  In
the future, however, wormholes will be the mechanism through which
co-simulation is achieved with other domains.

.pp
.H1 "Running VHDLB Demos"

The default VHDLB code generation target has a few parameters that can
be examined and changed with the "edit-target" command (place the
cursor anywhere in the facet and type "T").  The choices of targets
are currently two, "default-VHDLB" and "default-CG".  The
"default-VHDLB" target is the code generation target, while the
"default-CG" target is the baseclass CG target which simply generates
comments.  Future targets will include targets which generate code but
also optionally compile and optionally simulate the generated code.
Other target options will determine which vendor VHDL simulators the
code is run on.  The next parameter is the "directory", which is the
name of the destination directory where the generated VHDL code files
are written.  The "Looping Level" is a parameter for schedulers and is
not used by this domain.

.pp

The parameters of individual stars, galaxies, or universes can be
examined and changed with the "edit-params" command (place the cursor
on the star or galaxy, or in the universe facet of interest and type
"e").  The first parameter of any star is always "procId", which is
used in multiprocessor domains, but not in the VHDLB domain.
Additional parameters, if any, can be modified and will be reflected
in the "generic" clauses of the generated VHDL code.

.pp
.H1 "Warnings"

When creating your own galaxies, you should avoid using names for terminals
such as "in" and "out" because these are reserved words in the VHDL language.
Generated VHDL code which uses these names for signals will likely cause errors
if compiling or running it is attempted.  A complete list of VHDL reserved
words can be found in [1] and in [2].

.pp
.H1 "Bugs"

Because of the way the scheduler interprets signal flow graphs,
multiple signals forking directly after a galaxy input arrow are not
allowed.  This situation would be correctable by inserting an explicit
Fork star between the galaxy input arrow and the forking signal arcs,
but due to a problem with the way forks are parsed by the target, this
results in the signals being named incorrectly.  The current
workaround is to put an explicit Through star between the galaxy input
arrow and the forking signal arcs.  An example of this situation is in
the FIR4 galaxy in the firFilter demo in
$PTOLEMY/src/domains/vhdlf/demo/init.pal.

.UH "References"
.ip [1]
"IEEE Standard VHDL Language Reference Manual",
\fIIEEE Std 1076-1987, Published by the Institute of Electrical and Electronics Engineers, Inc., New York, NY, USA, \fR
1988.

.ip [2]
J. R. Armstrong and F. G. Gray, "Structured Logic Design with VHDL", \fIPTR Prentice Hall, Englewood Cliffs, NJ, \fR
1993.
