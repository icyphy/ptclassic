.VR $Id$
.TI "VHDLB Domain"
.ds DO "VHDLB
.AU
E. A. Lee
M. C. Williamson
A. P. Kalavade 
.AE
.H1 "Introduction"
.pp

The VHDLB domain is one of two branch domains which support the VHDL
language in Ptolemy, the other being the VHDLF domain.  The VHDLF
domain is limited to stars and applications consisting of purely
"functional" VHDL actors, i.e. actors whose outputs are a function of
their inputs only and do not have any time- or data-dependent
behavior.  The semantics of the VHDLF domain are akin to those of the
SDF domain in that the behavior of actors is static from one firing to
the next, although the firings of actors are dependent on the changes
in input values.  The VHDLB domain is devoted to stars and
applications consisting of "behavioral" actors whose behavior is time-
or data-dependent.  The semantics of the VHDLB domain are akin to
those of the DE domain in that the behavior of actors can be both
time-dependent and/or data dependent.

.pp
.Id "VHDL"
.Id "VHDL"
.Id "VHDLB domain"
.Id "domain, VHDL"

VHDL is a timed, event-driven language with pre-emptive scheduling.
While primarily suited for specification and simulation of digital
electronic systems, the range of simulation levels allowed in VHDL
makes it possible to simulate systems at a wide range of levels of
abstraction, from system block level down to the individual gate
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
co-simulation with the VHDLF domain and with simulation domains through
wormholes.  With the code generation capabilities included in this
release, the user can build up the library of VHDL stars and VHDL code
modules in order to manage structure and hierarchy in VHDL code
generation.  For a detailed specification of the VHDL language, the
user is referred to [1].

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
name of the destination directory where the VHDL files are written.
The "Looping Level" is a parameter for schedulers and is not used by
this domain.

.pp

The parameters of individual stars, galaxies, or universes can be
examined and changed with the "edit-params" command (place the cursor
on the star or galaxy, or in the universe facet of interest and type
"e").  The first parameter of any star is always "procId", which is
used in multiprocessor domains, but not in the VHDLB domain.
Additional parameters, if any, can be modified and will be reflected
in the "generic" clauses of the generated VHDL code.

.UH "References"
.ip [1]
"IEEE Standard VHDL Language Reference Manual",
\fIIEEE Std 1076-1987, Published by the Institute of Electrical and Electronics Engineers, Inc., New York, NY, USA, \fR
1988.
