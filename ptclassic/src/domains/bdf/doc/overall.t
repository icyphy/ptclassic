.VR 0.$Revision$ "%G"
.TI "BDF Domain"
.ds DO "BDF
.AU
J. T. Buck
.AE
.H1 "Introduction"
.pp
Boolean-controlled dataflow (BDF)
.Id "Boolean-controlled dataflow"
.Id "BDF domain"
.Id "domain, BDF"
is a domain that can be thought of as a generalization of synchronous
dataflow (SDF) that supports dynamic actors, but that still permits much
of the scheduling work to be performed at compile time, unlike the DDF
domain, which is still more general.
BDF is a generalization of SDF, and DDF is still more general;
accordingly, the BDF domain permits SDF actors to be used, and the DDF
domain permits SDF actors to be used.
This section will assume that the reader is familiar with \*(PT's SDF domain.
.pp
The BDF domain can execute any actor that falls into the class of
Boolean-controlled dataflow actors.
For an actor to be SDF, the number of particles read by each input porthole,
or written by each output porthole, must be constant.
Under BDF, a generalization is permitted: the number of particles read or
written by a port may be either a constant or may be a two-valued function
of a particle read on a control port for the same star.
One of the two values of the function must be zero.
The effect of this is that a port might read tokens only if the corresponding
control particle is zero (false) or nonzero (true).
The control port is always of type integer, and it must read or write exactly
one particle.
Although the particles on the control port are of integer type, we treat
them as Boolean, using the C/C++ convention: zero is false and nonzero is true.
.pp
We say that a porthole that conditionally transfers data based on a
control token is conditional.
A conditional input porthole must be controlled by a control input.
A conditional output porthole may be controlled by either a control input
or a control output.
The reason for these restrictions is to permit the scheduler to determine
the schedule by looking only at the values of particles on control ports.
.pp
The theory that describes graphs of BDF actors and their properties is
called the token flow model.
Its properties are summarized in [1] and developed in much more detail in [2].
.pp
The BDF scheduler performs the following functions: first, it performs a
consistency check somewhat analogous to the one performed by the SDF
scheduler to detect certain types of errors corresponding to mismatches in
particle flow rates.
Assuming that no error occurs, it then applies a clustering algorithm to
the graph, attempting to map it into traditional control structures such
as if-then-else and do-while.
If this clustering process succeeds in reducing the entire graph to a
single cluster, the graph is then executed with the quasi-static schedule
corresponding to the clusters (it is not completely static since some actors
will be conditionally executed based on control particle values, but the
result is "as static as possible").
If the clustering does not succeed, then the resulting clusters may
optionally be executed by the same dynamic scheduler as is used in the
DDF domain.
Dynamic execution of clusters is enabled or disabled by setting the
\fIallowDynamic\fR parameter of the default-BDF target.
.H1 "Writing BDF Stars"
.pp
BDF stars are written in almost exactly the same way as SDF stars are written.
When the go() method of the star is executed, it is guaranteed that all
required input data are present, and after execution, any particles generated
by the star are correctly sent off to their destinations.
The only additional thing the star writer must know is how to specify that
a porthole is conditional on other portholes.
This is accomplished with a method of the class BDFPortHole called
setBDFParams.
.pp
The setBDFParams method takes four arguments.
The first argument is the number of particles transferred by the port when
the port is enabled (for conditional ports: this number is transferred all
the time for unconditional ports).
The second argument is either a pointer or a reference to another
BDFPortHole, which is called the associated port (the function has two
overloaded forms, which is why the argument may be specified either as
a pointer or as a reference).
The third argument is a code specifying the relation between the porthole
this method is called on and the associated port:
.ip BDF_NONE
This code indicates no relation at all.
.ip BDF_TRUE
This code indicates that data are transferred by the port only when the
conditional port has a true particle.
.ip BDF_FALSE
This code indicates that data are transferred by the port only when the
conditional port has a false particle.
.ip BDF_SAME
This code indicates that the stream transferred by the associated port is
the same as the stream transferred by this port.
This relationship is specified for the BDF Fork actor and aids the operation
of the clustering algorithm.
.ip BDF_COMPLEMENT
This code indicates that the stream transferred by the associated port is
the logical complement of the stream transferred by this port.
This relationship is specified for the BDF Not actor and aids the operation of
the clustering algorithm.
.pp
The fourth argument for setBDFParams is the maximum delay, that is, the
largest value that the star may specify as an argument to the % operator
on that porthole.
The default value is zero.
This argument serves the same purpose as the second argument to setSDFParams.
.pp
The setSDFParams function may be used on BDF portholes; it does not alter
the associated port or the relation type, but does alter the other two
parameters of setBDFParams.
By default, BDF portholes transfer one token, unconditionally.
.pp
Calls to setBDFParams may be placed in a star's setup() method, or
alternatively in the constructor if the call does not depend on any
parameters of the star.
Consider as an example a
.c Switch
star.
This star's functionality is as follows: on each execution, it reads a
particle from its control input port.
If the value is true, it reads a particle from its trueInput port;
otherwise it reads a particle from its \fIfalseInput\fR port.
In any case, the particle is copied to the \fIoutput\fR port.
Using the
.i ptlang
preprocessor, the setup method could be written
.(c
setup {
	trueInput.setBDFParams(1, control, BDF_TRUE, 0);
	falseInput.setBDFParams(1, control, BDF_FALSE, 0);
}
.)c
and the go method could be written
.(c
go {
	if (int(control%0))
		output%0 = trueInput%0;
	else
		output%0 = falseInput%0;
}
.)c
.UH "References"
.ip [1]
J. T. Buck and
E. A. Lee,
"Scheduling Dynamic Dataflow Graphs with Bounded Memory Using the Token
Flow Model",
.i "Proc. ICASSP '93" ,
Minneapolis, MN, April 1993.
.ip [2]
J. T. Buck,
"Scheduling Dynamic Dataflow Graphs with Bounded Memory Using the Token
Flow Model",
Ph.D. Thesis, University of California, Berkeley, August 1993.
.\" Local variables:
.\" mode: nroff
.\" End:
