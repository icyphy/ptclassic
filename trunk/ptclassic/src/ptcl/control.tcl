# $Id$
# This script shows a simple use of Tcl control structures and the
# statevalue function to impose a stopping condition on a simulation.
# The universe is very simple: a FloatRamp connected to an Xgraph.
# We wish to run the universe until the value of the FloatRamp's
# "value" state (which corresponds to its output) exceeds 20.
# since it is driven by the Poisson star, this will happen within
# "timeres" time units after 20 events arrive.  timeres is a Tcl
# variable.

# Note that the 20'th event may not be graphed.

domain DE
star src Poisson
star ramp FloatRamp
star draw Xgraph
connect src output ramp input
connect ramp output draw input
setstate ramp step 1.0
setstate draw options "-P"
set timeres 0.25
run $timeres
while {[statevalue ramp value] < 20} {
	cont $timeres
}
wrapup
