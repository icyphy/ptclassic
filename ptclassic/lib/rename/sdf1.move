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
# This file was auto-generated by kennard on Wed Nov  4 10:59:05 PST 1992.
# The script gen-rename generated this file from config file sdf1.conf
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

src=FloatPad; tgt=Pad; tgtdir=; . $moveprog
src=FloatDC; tgt=Const; tgtdir=; . $moveprog
src=ComplexDC; tgt=ConstCx; tgtdir=; . $moveprog
src=FloatGain; tgt=Gain; tgtdir=; . $moveprog
src=CxGain; tgt=GainCx; tgtdir=; . $moveprog
src=FloatSum; tgt=Add; tgtdir=; . $moveprog
src=ComplexSum; tgt=AddCx; tgtdir=; . $moveprog
src=FloatDiff; tgt=Sub; tgtdir=; . $moveprog
src=CxDiff; tgt=SubCx; tgtdir=; . $moveprog
src=FloatProduct; tgt=Mpy; tgtdir=; . $moveprog
src=ComplexProduct; tgt=MpyCx; tgtdir=; . $moveprog
src=FloatImpulse; tgt=Impulse; tgtdir=; . $moveprog
src=FloatRect; tgt=Rect; tgtdir=; . $moveprog
src=FloatRamp; tgt=Ramp; tgtdir=; . $moveprog
src=CxWaveForm; tgt=WaveFormCx; tgtdir=; . $moveprog
src=FloatTable; tgt=Table; tgtdir=; . $moveprog
src=CxTable; tgt=TableCx; tgtdir=; . $moveprog
src=FloatThresh; tgt=Thresh; tgtdir=; . $moveprog
src=ComplexAverage; tgt=AverageCx; tgtdir=; . $moveprog
src=Quantizer; tgt=Quant; tgtdir=; . $moveprog
src=NewQuant; tgt=LinQuantIdx; tgtdir=; . $moveprog
src=ComplexReal; tgt=CxToRect; tgtdir=; . $moveprog
src=RealComplex; tgt=RectToCx; tgtdir=; . $moveprog
src=PolarRect; tgt=PolarToRect; tgtdir=; . $moveprog
src=RectPolar; tgt=RectToPolar; tgtdir=; . $moveprog
src=IntRamp; tgt=RampInt; tgtdir=; . $moveprog
src=IntTable; tgt=TableInt; tgtdir=; . $moveprog
rm -f $moveprog
