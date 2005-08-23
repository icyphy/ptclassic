# Tests for SDF DSP stars that does a fourier transform.
#
# @Author: Christopher Hylands
#
# @Version: @(#)testSDFFourier.tcl	1.3 03/28/98
#
# @Copyright (c) 1997-1998 The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
# 
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# 
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
#######################################################################

# Tycho test bed, see $TYCHO/doc/coding/testing.html for more information.

# Load up the test definitions.
if {[string compare test [info procs test]] == 1} then { 
    source [file join $env(PTOLEMY) src domains sdf test testSDFDefs.tcl]
} {}

# Alphabetical by starname

######################################################################
#### test SDFFourier
# 
test SDFFourier { SDFFourier application, based on $PTOLEMY/demo/eecs20/lec2/fourier} {
    set star FourierTest
    reset __empty__
    domain SDF
    newuniverse ${star}Test SDF
    target loop-SDF

    set tmpfile [sdfSetupPrinter]
    set tmpfile2 [sdfSetupPrinter Printb]

    # These commands were grabbed from pigiLog.pt and then the 
    # TclScript and TkPlot stars were commented out and replaced with
    # non-graphical stars.

    targetparam logFile ""
#    targetparam loopScheduler "DEF #choices: DEF, CLUST,ACYLOOP"
    targetparam schedulePeriod 0.0
    star FFTCx1 FFTCx
    setstate FFTCx1 order 6
    setstate FFTCx1 size 64
    setstate FFTCx1 direction 1
    pragma fourier FFTCx1  
    star Commutator1 Commutator
    setstate Commutator1 blockSize 1
    pragma fourier Commutator1  
    star Reverse1 Reverse
    setstate Reverse1 N 32
    pragma fourier Reverse1  
    star "Commutator.input=21" Commutator
    numports "Commutator.input=21" input 2
    setstate "Commutator.input=21" blockSize 32
    pragma fourier Commutator.input=21  
    star "MpyCx.input=21" MpyCx
    numports "MpyCx.input=21" input 2
    pragma fourier MpyCx.input=21  
    star FFTCx2 FFTCx
    setstate FFTCx2 order 6
    setstate FFTCx2 size 64
    setstate FFTCx2 direction "-1"
    pragma fourier FFTCx2  
    star CxToRect1 CxToRect
    pragma fourier CxToRect1  
    star "Nop.input=21" Nop
    numports "Nop.input=21" input 2
    pragma fourier Nop.input=21  
    #	star TkPlot1 TkPlot
    #	setstate TkPlot1 label "Time Domain"
    #	setstate TkPlot1 geometry "600x400+0+0"
    #	setstate TkPlot1 xTitle n
    #	setstate TkPlot1 yTitle y
    #	setstate TkPlot1 xRange "0 128"
    #	setstate TkPlot1 yRange "-1.5 1.5"
    #	setstate TkPlot1 persistence 129
    #	setstate TkPlot1 style connect
    #	setstate TkPlot1 updateSize 10
    #	setstate TkPlot1 repeat_border_points YES
    #	pragma fourier TkPlot1  
    #	star "TclScript.input=01" TclScript
    #	numports "TclScript.input=01" input 0
    #	setstate "TclScript.input=01" tcl_file {$PTOLEMY/demo/eecs20/lec2/tkSelectFreqs.tcl}
    #	pragma fourier TclScript.input=01  
    star NumToBus1 NumToBus
    star Const2 Const
    setstate Const2 level 7
    connect Const2 output NumToBus1 input
    star FloatToCx1 FloatToCx
    pragma fourier FloatToCx1  
    star BlackHole1 BlackHole
    pragma fourier BlackHole1  
    star Mux1 Mux
    setstate Mux1 blockSize 1
    pragma fourier Mux1  
    star "Nop.input=31" Nop
    numports "Nop.input=31" input 3
    pragma fourier Nop.input=31  
    star FloatToCx2 FloatToCx
    pragma fourier FloatToCx2  
    star Chop1 Chop
    setstate Chop1 nread 32
    setstate Chop1 nwrite 32
    setstate Chop1 offset 1
    setstate Chop1 use_past_inputs NO
    pragma fourier Chop1  
    star Chop2 Chop
    setstate Chop2 nread 64
    setstate Chop2 nwrite 31
    setstate Chop2 offset 0
    setstate Chop2 use_past_inputs YES
    pragma fourier Chop2  
    star UpSample1 UpSample
    setstate UpSample1 factor 4
    setstate UpSample1 phase 0
    setstate UpSample1 fill 0.0
    pragma fourier UpSample1  
    #	star TkPlot2 TkPlot
    #	setstate TkPlot2 label "Frequency Domain"
    #	setstate TkPlot2 geometry "600x400+0+400"
    #	setstate TkPlot2 xTitle freq
    #	setstate TkPlot2 yTitle ampl
    #	setstate TkPlot2 xRange "0 123"
    #	setstate TkPlot2 yRange "0 20"
    #	setstate TkPlot2 persistence 124
    #	setstate TkPlot2 style connect
    #	setstate TkPlot2 updateSize 10
    #	setstate TkPlot2 repeat_border_points NO
    #	pragma fourier TkPlot2  
    star CxToFloat1 CxToFloat
    pragma fourier CxToFloat1  
    star Gain1 Gain
    setstate Gain1 gain "1/64"
    pragma fourier Gain1  
    star PcwzLinear1 PcwzLinear
    setstate PcwzLinear1 breakpoints "(0, 1) (32, -1) (64, 1)"
    pragma fourier PcwzLinear1  
    star PcwzLinear2 PcwzLinear
    setstate PcwzLinear2 breakpoints "(0, 1) (31, 1)  (32, -1) (63, -1)"
    pragma fourier PcwzLinear2  
    star ModuloInt1 ModuloInt
    setstate ModuloInt1 modulo 64
    pragma fourier ModuloInt1  
    star RampInt1 RampInt
    setstate RampInt1 step 1
    setstate RampInt1 value 0.0
    pragma fourier RampInt1  
    #	star "TclScript.input=02" TclScript
    #	numports "TclScript.input=02" input 0
    #	setstate "TclScript.input=02" tcl_file {$PTOLEMY/demo/eecs20/lec2/tkSelectWaveform.tcl}
    #	pragma fourier TclScript.input=02  
    star Repeat1 Repeat
    setstate Repeat1 numTimes 64
    setstate Repeat1 blockSize 1
    pragma fourier Repeat1  
    connect FFTCx2 output CxToRect1 input
    busconnect NumToBus1 output Commutator1 input 32
    #	busconnect "TclScript.input=01" output Commutator1 input 32
    connect FloatToCx1 output FFTCx1 input
    connect CxToRect1 imag BlackHole1 input
    #	connect "Nop.input=21" output TkPlot1 Y
    connect "Nop.input=21" output Printa input
    connect CxToRect1 real "Nop.input=21" "input#2"
    node node1
    nodeconnect Mux1 output node1
    nodeconnect FloatToCx1 input node1
    nodeconnect Nop.input=21 input#1 node1
    connect "Nop.input=31" output Mux1 input
    connect FloatToCx2 output "MpyCx.input=21" "input#1"
    connect "Commutator.input=21" output FloatToCx2 input
    connect Chop1 output "Commutator.input=21" "input#2"
    connect Reverse1 output Chop1 input
    node node2
    nodeconnect Commutator1 output node2
    nodeconnect Reverse1 input node2
    nodeconnect Commutator.input=21 input#1 node2
    connect Chop2 output UpSample1 input
    #	connect UpSample1 output TkPlot2 Y
    connect UpSample1 output Printb input 
    connect CxToFloat1 output Chop2 input
    node node3
    nodeconnect MpyCx.input=21 output node3
    nodeconnect CxToFloat1 input node3
    nodeconnect FFTCx2 input node3
    connect FFTCx1 output "MpyCx.input=21" "input#2"
    connect RampInt1 output ModuloInt1 input
    star Const1 Const
    setstate Const1 level 1	
    #	connect "TclScript.input=02" output Repeat1 input
    connect Const1 output Repeat1 input
    connect Repeat1 output Mux1 control
    node node4
    nodeconnect ModuloInt1 output node4
    nodeconnect PcwzLinear1 input node4
    nodeconnect Gain1 input node4
    nodeconnect PcwzLinear2 input node4
    connect PcwzLinear1 output "Nop.input=31" "input#1"
    connect Gain1 output "Nop.input=31" "input#2"
    connect PcwzLinear2 output "Nop.input=31" "input#3"

    run 1
    wrapup
    list [readTmpFile $tmpfile] [readTmpFile $tmpfile2]
} {{{0.0	0.4609375	
0.015625	0.399188437278893	
0.03125	0.339667661875314	
0.046875	0.283779850999228	
0.0625	0.232811528696171	
0.078125	0.187889919403422	
0.09375	0.149947636757559	
0.109375	0.119694487526969	
0.125	0.0975974006148893	
0.140625	0.0838691830477795	
0.15625	0.078466470527326	
0.171875	0.0810968922833761	
0.1875	0.0912351219340468	
0.203125	0.108147151154299	
0.21875	0.130921813982273	
0.234375	0.158508318359575	
0.25	0.189758318359575	
0.265625	0.223470894007475	
0.28125	0.258438701961457	
0.296875	0.293493523510113	
0.3125	0.327549467683602	
0.328125	0.359642185491277	
0.34375	0.388962612534548	
0.359375	0.414883975227666	
0.375	0.436981062139746	
0.390625	0.455041066244327	
0.40625	0.469065634367373	
0.421875	0.479264104096396	
0.4375	0.486038252575295	
0.453125	0.489959212682471	
0.46875	0.491737517304095	
0.484375	0.4921875	
0.5	0.4921875	
0.515625	0.492637482695905	
0.53125	0.494415787317529	
0.546875	0.498336747424705	
0.5625	0.505110895903604	
0.578125	0.515309365632627	
0.59375	0.529333933755673	
0.609375	0.547393937860254	
0.625	0.569491024772334	
0.640625	0.595412387465452	
0.65625	0.624732814508723	
0.671875	0.656825532316398	
0.6875	0.690881476489887	
0.703125	0.725936298038543	
0.71875	0.760904105992525	
0.734375	0.794616681640425	
0.75	0.825866681640425	
0.765625	0.853453186017727	
0.78125	0.876227848845701	
0.796875	0.893139878065953	
0.8125	0.903278107716624	
0.828125	0.905908529472674	
0.84375	0.90050581695222	
0.859375	0.886777599385111	
0.875	0.864680512473031	
0.890625	0.834427363242441	
0.90625	0.796485080596578	
0.921875	0.751563471303829	
0.9375	0.700595149000772	
0.953125	0.644707338124686	
0.96875	0.585186562721107	
0.984375	0.5234375	
}} {{31.5	
0.0	
0.0	
0.0	
10.1900081235481	
0.0	
0.0	
0.0	
5.10114861868916	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
0.0	
}}}
