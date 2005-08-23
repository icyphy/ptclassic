# Demonstration of scripting capability in Ptolemy
# Authors: Edward A. Lee and Douglas Niehaus

# Display welcome message
ptkImportantMessage .demo "
Welcome to Ptolemy.
This script will walk you through some of its basic features.
The system you are looking at generates two sine waves, multiplies
them together, and displays the product.
Click in this window to run the system."

run 400
wrapup

ptkImportantMessage .demo "
This program uses a dataflow model of computation.
Execution proceeds in an order determined by the data
dependencies. Click in this window to see how one iteration
of the system proceeds."


# Turn on animation and execute one run
ptkGrAnimation on
run 1

ptkImportantMessage .demo "
Some dataflow systems have a more complicated notion of an \"iteration\".
Click in this window to see an example."

set octID [ptkOpenFacet $PTOLEMY/src/domains/sdf/demo/demomultirate]
pvOpenWindow $octID

newuniverse demomultirate SDF
ptkCompile $octID

ptkImportantMessage .demo "
This system is identical to the original system, except for the
downsampler that has been inserted.  The downsampler requires
two input samples for every one output sample it produces, so
the blocks upstream of it must run twice as often.
Click in this window to see it run through one iteration."

# For animation to work on the new facet, we need to specify the facet
ptkSetHighlightFacet $octID

run 1

ptkGrAnimation off



