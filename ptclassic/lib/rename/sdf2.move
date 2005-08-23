#! /bin/sh
#
###################  DO NOT EDIT  ##########################
# This shell script was automatically generated.
###################  DO NOT EDIT  ###########################
#
PTOLEMY=${PTOLEMY-`csh -c 'echo ~ptolemy'`}; export PTOLEMY
PATH=$PTOLEMY/octtools/bin.$ARCH:$PATH; export PATH
domain=SDF
domainpath=sdf
# This file was auto-generated by kennard on Wed Nov  4 10:59:22 PST 1992.
# The script gen-rename generated this file from config file sdf2.conf
# Shell script to move ptlang files and OCT icon facets.
# This should be invoked in the top-level directory for the domain.
if [ ! -d icons -o ! -d stars ] ; then
    echo "Missing the icons and/or stars directory."
    echo "Invoke this script in the toplevel directory for the domain."
    exit 0
fi
moveprog=/tmp/moveprog$$
cat - > $moveprog <<\BEOF
# This script segment assumes that src,tgt,tgtdir have already
# been defined.
tgtdir=${tgtdir:=.}
srcbase=${domain}$src; srcpl=$srcbase.pl
tgtbase=${domain}$tgt; tgtpl=$tgtbase.pl
if [ -n "$tgtdir" ] ; then
    mkdir $tgtdir $tgtdir/stars $tgtdir/stars/SCCS $tgtdir/icons 2> /dev/null
fi
if [ ! -f stars/$srcpl ] ; then
    echo Warning: ptlang $srcpl missing
else
    echo moving stars/$srcpl '-->' $tgtdir/stars/$tgtpl
    mv -i stars/$srcpl $tgtdir/stars/$tgtpl
    if [ -f stars/SCCS/s.$srcpl ] ; then
	mv -i stars/SCCS/s.$srcpl $tgtdir/stars/SCCS/s.$tgtpl
    fi
    if [ -f stars/SCCS/p.$srcpl ] ; then
	mv -i stars/SCCS/p.$srcpl $tgtdir/stars/SCCS/p.$tgtpl
    fi
    rm -f stars/$srcbase.cc stars/$srcbase.h
fi
for icon in icons/$src icons/$src.*; do
    if [ ! -d "$icon" ] ; then continue ; fi
    newname=`echo $icon | sed -e "s+/$src+/$tgt+"`
    if [ -f $tgtdir/$newname -o -d $tgtdir/$newname ] ; then
	echo Warning: icon $tgtdir/$newname already exists...skipping
    else
    	echo moving icon $icon '-->' $tgtdir/$newname
    	mv -i $icon $tgtdir/$newname
    fi
done
BEOF

src=Autocor; tgt=Autocor; tgtdir=dsp; . $moveprog
src=BiQuad; tgt=Biquad; tgtdir=dsp; . $moveprog
src=BlockAllPole; tgt=BlockAllPole; tgtdir=dsp; . $moveprog
src=BlockFIR; tgt=BlockFIR; tgtdir=dsp; . $moveprog
src=BlockLattice; tgt=BlockLattice; tgtdir=dsp; . $moveprog
src=BlockRLattice; tgt=BlockRLattice; tgtdir=dsp; . $moveprog
src=Burg; tgt=Burg; tgtdir=dsp; . $moveprog
src=Compress; tgt=MuLaw; tgtdir=dsp; . $moveprog
src=ComplexFFT; tgt=FFTCx; tgtdir=dsp; . $moveprog
src=ComplexFIR; tgt=FIRCx; tgtdir=dsp; . $moveprog
src=Convolve; tgt=Convolve; tgtdir=dsp; . $moveprog
src=CxLMS; tgt=LMSCx; tgtdir=dsp; . $moveprog
src=CxLMSPlot; tgt=LMSPlotCx; tgtdir=dsp; . $moveprog
src=CxRaisedCos; tgt=RaisedCosCx; tgtdir=dsp; . $moveprog
src=DTFT; tgt=DTFT; tgtdir=dsp; . $moveprog
src=FIR; tgt=FIR; tgtdir=dsp; . $moveprog
src=Hilbert; tgt=Hilbert; tgtdir=dsp; . $moveprog
src=LMS; tgt=LMS; tgtdir=dsp; . $moveprog
src=LMSPlot; tgt=LMSPlot; tgtdir=dsp; . $moveprog
src=LMSLeak; tgt=LMSLeak; tgtdir=dsp; . $moveprog
src=Lattice; tgt=Lattice; tgtdir=dsp; . $moveprog
src=RLattice; tgt=RLattice; tgtdir=dsp; . $moveprog
src=LevDur; tgt=LevDur; tgtdir=dsp; . $moveprog
src=IIR; tgt=IIR; tgtdir=dsp; . $moveprog
src=Unwrap; tgt=Unwrap; tgtdir=dsp; . $moveprog
src=Window; tgt=Window; tgtdir=dsp; . $moveprog
src=RaisedCos; tgt=RaisedCos; tgtdir=dsp; . $moveprog
rm -f $moveprog
