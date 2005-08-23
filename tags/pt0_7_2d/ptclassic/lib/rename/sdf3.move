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
# This file was auto-generated by kennard on Wed Nov  4 10:59:36 PST 1992.
# The script gen-rename generated this file from config file sdf3.conf
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

src=Dct; tgt=DCTImage; tgtdir=image; . $moveprog
src=DctInv; tgt=DCTImageInv; tgtdir=image; . $moveprog
src=DctCode; tgt=DCTImageCode; tgtdir=image; . $moveprog
src=DctCodeInv; tgt=DCTImageCodeInv; tgtdir=image; . $moveprog
src=Dpcm; tgt=DPCMImage; tgtdir=image; . $moveprog
src=DpcmInv; tgt=DPCMImageInv; tgtdir=image; . $moveprog
src=Yuv2Rgb; tgt=YUVToRGB; tgtdir=image; . $moveprog
src=Rgb2Yuv; tgt=RGBToYUV; tgtdir=image; . $moveprog
src=ImageToPix; tgt=ImageToPix; tgtdir=image; . $moveprog
src=PixToImage; tgt=PixToImage; tgtdir=image; . $moveprog
src=ZigZag; tgt=ZigZagImage; tgtdir=image; . $moveprog
src=ZigZagInv; tgt=ZigZagImageInv; tgtdir=image; . $moveprog
src=MedianImage; tgt=MedianImage; tgtdir=image; . $moveprog
src=DiffImage; tgt=DiffImage; tgtdir=image; . $moveprog
src=DisplayImage; tgt=DisplayImage; tgtdir=image; . $moveprog
src=ReadImage; tgt=ReadImage; tgtdir=image; . $moveprog
src=ReadRgb; tgt=ReadRGB; tgtdir=image; . $moveprog
src=DisplayRgb; tgt=DisplayRGB; tgtdir=image; . $moveprog
src=DisplaySeq; tgt=DisplayVideo; tgtdir=image; . $moveprog
src=RunLen; tgt=RunLenImage; tgtdir=image; . $moveprog
src=RunLenInv; tgt=RunLenImageInv; tgtdir=image; . $moveprog
src=AddMvecs; tgt=AddMotionVecs; tgtdir=image; . $moveprog
src=MotionCmp; tgt=MotionCmp; tgtdir=image; . $moveprog
src=MotionCmpInv; tgt=MotionCmpInv; tgtdir=image; . $moveprog
rm -f $moveprog
