export PTOLEMY=/tmp/cxh/ptclassic
export PTARCH=linux
PATH=$PTOLEMY/bin:$PTOLEMY/bin.$PTARCH:$PTOLEMY/vendors/bin:$PATH
# To use the X11 pxgraph, follow the instructions
# in $PTOLEMY/src/pxgraph/README.txt and uncomment the next line.
export PT_USE_X11_PXGRAPH=yes
