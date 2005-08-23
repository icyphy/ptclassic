# Tests for SDF Contrib ArrayRLS demo
#
# @Author: Christopher Hylands
#
# @Version: @(#)testSDFArrayRLS.tcl	1.3 03/28/98
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


# Define the galaxies we will use
proc defSDFArrayRLSGals {} {
    defgalaxy bits {
	domain SDF
	newstate probOfZero FLOAT 0.5
	star IIDUniform1 IIDUniform
	setstate IIDUniform1 lower 0.0
	setstate IIDUniform1 upper "1/probOfZero"
	star Quant1 Quant
	setstate Quant1 thresholds 1.0
	setstate Quant1 levels ""
	star FloatToInt1 FloatToInt
	alias out FloatToInt1 output
	connect IIDUniform1 output Quant1 input
	connect Quant1 output FloatToInt1 input
    }

    defgalaxy expjx {
	domain SDF
	star RectToCx1 RectToCx
	star "Fork.output=21" Fork
	numports "Fork.output=21" output 2
	star Cos1 Cos
	star Sin1 Sin
	alias in "Fork.output=21" input
	connect "Fork.output=21" "output#1" Cos1 input
	connect "Fork.output=21" "output#2" Sin1 input
	connect Cos1 output RectToCx1 real
	connect Sin1 output RectToCx1 imag
	alias out RectToCx1 output
    }

    defgalaxy QAM16 {
	domain SDF
	star TableCx1 TableCx
	setstate TableCx1 values "(1,1) (3,1) (1,3) (3,3)  (-1,1) (-1,3) (-3,1) (-3,3)  (-1,-1) (-3,-1) (-1,-3) (-3,-3)  (1,-1) (1,-3) (3,-1) (3,-3)"
	star BitsToInt1 BitsToInt
	setstate BitsToInt1 nBits 2
	star "Distributor.output=21" Distributor
	numports "Distributor.output=21" output 2
	setstate "Distributor.output=21" blockSize 1
	star "AddInt.input=21" AddInt
	numports "AddInt.input=21" input 2
	star ModuloInt1 ModuloInt
	setstate ModuloInt1 modulo 4
	star TableInt1 TableInt
	setstate TableInt1 values "1 0 2 3"
	star "AddInt.input=22" AddInt
	numports "AddInt.input=22" input 2
	star GainInt1 GainInt
	setstate GainInt1 gain 4
	star "Fork.output=21" Fork
	numports "Fork.output=21" output 2
	alias in BitsToInt1 input
	alias out TableCx1 output
	connect BitsToInt1 output "Distributor.output=21" input
	connect "Distributor.output=21" "output#1" TableInt1 input
	connect "AddInt.input=22" output TableCx1 input
	connect TableInt1 output "AddInt.input=21" "input#2"
	connect GainInt1 output "AddInt.input=22" "input#2"
	connect "Distributor.output=21" "output#2" "AddInt.input=22" "input#1"
	connect "AddInt.input=21" output ModuloInt1 input
	connect ModuloInt1 output "Fork.output=21" input
	connect "Fork.output=21" "output#1" "AddInt.input=21" "input#1" "*1"
	connect "Fork.output=21" "output#2" GainInt1 input
    }
    defgalaxy arrayelement {
	domain SDF
	newstate wavelength float 0.30
	newstate NumberSignals int 3
	newstate NoiseLevel float 0.001
	newstate Positions complexarray "(0.0,0.0) (0.0,0.0) "
	newstate Instance int 1
	star Const1 Const
	setstate Const1 level 27000
	star PolarToRect1 PolarToRect
	star RectToCx1 RectToCx
	star SubCx1 SubCx
	star Abs1 Abs
	star Gain1 Gain
	setstate Gain1 gain "-2*PI/wavelength"
	star expjx1 expjx
	star "MpyCx.input=21" MpyCx
	numports "MpyCx.input=21" input 2
	star Distributor1 Distributor
	setstate Distributor1 blockSize 1
	star AddCx1 AddCx
	star CxToRect1 CxToRect
	star BlackHole1 BlackHole
	star "AddCx.input=21" AddCx
	numports "AddCx.input=21" input 2
	star IIDGaussian1 IIDGaussian
	setstate IIDGaussian1 mean 0.0
	setstate IIDGaussian1 variance "NoiseLevel/2"
	star IIDGaussian2 IIDGaussian
	setstate IIDGaussian2 mean 0.0
	setstate IIDGaussian2 variance "NoiseLevel/2"
	star RectToCx2 RectToCx
	star ConstCx1ofN1 ConstCx1ofN
	setstate ConstCx1ofN1 ComplexValues Positions
	setstate ConstCx1ofN1 ValueIndex Instance
	connect PolarToRect1 y RectToCx1 imag
	connect PolarToRect1 x RectToCx1 real

	connect Abs1 output Gain1 input
#	star ForkP Fork
#	star Print1 Printer
#	numports ForkP output 2
#	connect Abs1 output ForkP input
#	connect ForkP output#1 Gain1 input
#	connect ForkP output#2 Print1 input

	connect Gain1 output expjx1 in
	connect "MpyCx.input=21" output Distributor1 input
	busconnect Distributor1 output AddCx1 input NumberSignals
	connect Const1 output PolarToRect1 magnitude
	connect CxToRect1 real PolarToRect1 phase
	connect CxToRect1 imag BlackHole1 input
	alias angle CxToRect1 input
	connect expjx1 out "MpyCx.input=21" "input#2"
	alias signal "MpyCx.input=21" "input#1"

	connect RectToCx1 output SubCx1 pos
#	star ForkP Fork
#	star Print1 Printer
#	numports ForkP output 2
#	connect RectToCx1 output ForkP input
#	connect ForkP output#1 SubCx1 pos
#	connect ForkP output#2 Print1 input

	alias out_elem "AddCx.input=21" output

	connect SubCx1 output Abs1 input
#	star ForkP Fork
#	star Print1 Printer
#	numports ForkP output 2
#	connect SubCx1 output ForkP input
#	connect ForkP output#1 Print1 input
#	connect ForkP output#2 Abs1 input

	connect AddCx1 output "AddCx.input=21" "input#1"
	connect IIDGaussian1 output RectToCx2 real
	connect IIDGaussian2 output RectToCx2 imag
	connect RectToCx2 output "AddCx.input=21" "input#2"

	connect ConstCx1ofN1 output SubCx1 neg
#	star ForkP Fork
#	star Print1 Printer
#	numports ForkP output 2
#	connect ConstCx1ofN1 output ForkP input
#	connect ForkP output#1 SubCx1 neg
#	connect ForkP output#2 Print1 input
    }
}

######################################################################
#### test arrayelement
test arrayelement {arrayelement galaxy} {
    set star arrayelement
    sdfInitUniverse $star

    # Define the galaxies we will use
    defSDFArrayRLSGals

    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star
    sdfTestRampCx RectToCxA
    star ConstCx1 ConstCx
    connect RectToCxA output "$star.a" signal
    connect ConstCx1 output "$star.a" angle
    connect "$star.a" out_elem Printa input


    setstate ConstCx1 imag 0.5 
    setstate ConstCx1 real 0.5 
    newstate Positions complexarray {< $PTOLEMY/src/domains/sdf/contrib/demo/poscirc.param}
    setstate "$star.a" Positions Positions
    run 10
    wrapup
        readTmpFile $tmpfile
} {{(-0.892174592458311,3.19873703422385)	
(-3.55080099837456,12.9278795161051)	
(-6.29722648524423,22.5985637698243)	
(-8.97772247106825,32.3453114293465)	
(-11.6659146712164,42.037285886732)	
(-14.3647212847264,51.6945892558405)	
(-17.0803065577838,61.4062163191861)	
(-19.762864871476,71.1049569621396)	
(-22.4958558518923,80.7541074113509)	
(-25.1882573442178,90.4823289485435)	
}}

######################################################################
#### test ArrayRLSBeamXYSP
test  ArrayRLSBeamXYSP {ArrayRLSBeamXYSP} {
    set star ArrayRLSBeamXYSP
    sdfInitUniverse $star

    # Define the galaxies we will use
    defSDFArrayRLSGals

	targetparam logFile ""
	targetparam loopScheduler "DEF #choices: DEF, CLUST,ACYLOOP"
	targetparam schedulePeriod 0.0
	newstate NumberSignals int 4
	newstate NumberElements int 4
	newstate wavelength float 0.29
	newstate Positions complexarray {< $PTOLEMY/src/domains/sdf/contrib/demo/poscirc.param}
	star IIDGaussian1 IIDGaussian
	setstate IIDGaussian1 mean 0.0
	setstate IIDGaussian1 variance 0.2
	pragma ArrayRLSBeamXYSP IIDGaussian1  
	star IIDGaussian2 IIDGaussian
	setstate IIDGaussian2 mean 0.0
	setstate IIDGaussian2 variance 0.2
	pragma ArrayRLSBeamXYSP IIDGaussian2  

#	star "TkSlider.output=11" TkSlider
#	numports "TkSlider.output=11" output 1
#	setstate "TkSlider.output=11" low "-180"
#	setstate "TkSlider.output=11" high 180
#	setstate "TkSlider.output=11" value "-155"
#	setstate "TkSlider.output=11" identifier "SOI     "
#	setstate "TkSlider.output=11" put_in_control_panel YES
#	pragma ArrayRLSBeamXYSP TkSlider.output=11  
#	star "TkSlider.output=12" TkSlider
#	numports "TkSlider.output=12" output 1
#	setstate "TkSlider.output=12" low "-180"
#	setstate "TkSlider.output=12" high 180
#	setstate "TkSlider.output=12" value "-58"
#	setstate "TkSlider.output=12" identifier "SNOI 1"
#	setstate "TkSlider.output=12" put_in_control_panel YES
#	pragma ArrayRLSBeamXYSP TkSlider.output=12  
#	star "TkSlider.output=13" TkSlider
#	numports "TkSlider.output=13" output 1
#	setstate "TkSlider.output=13" low "-180"
#	setstate "TkSlider.output=13" high 180
#	setstate "TkSlider.output=13" value 40
#	setstate "TkSlider.output=13" identifier "SNOI 2"
#	setstate "TkSlider.output=13" put_in_control_panel YES
#	pragma ArrayRLSBeamXYSP TkSlider.output=13  
	star FloatToCx1 FloatToCx
	pragma ArrayRLSBeamXYSP FloatToCx1  
	star FloatToCx2 FloatToCx
	pragma ArrayRLSBeamXYSP FloatToCx2  
	star FloatToCx3 FloatToCx
	pragma ArrayRLSBeamXYSP FloatToCx3  
	star FloatToCx4 FloatToCx
	pragma ArrayRLSBeamXYSP FloatToCx4  
	star FloatToCx5 FloatToCx
	pragma ArrayRLSBeamXYSP FloatToCx5  

	star Commutator1 Commutator
	setstate Commutator1 blockSize 1
	pragma ArrayRLSBeamXYSP Commutator1  
	star Commutator2 Commutator
	setstate Commutator2 blockSize 1
	pragma ArrayRLSBeamXYSP Commutator2  
	star Repeat1 Repeat
	setstate Repeat1 numTimes NumberElements
	setstate Repeat1 blockSize 1
	pragma ArrayRLSBeamXYSP Repeat1  
	star Repeat2 Repeat
	setstate Repeat2 numTimes NumberElements
	setstate Repeat2 blockSize 1
	pragma ArrayRLSBeamXYSP Repeat2  
	star Distributor1 Distributor
	setstate Distributor1 blockSize 1
	pragma ArrayRLSBeamXYSP Distributor1  
	star Distributor2 Distributor
	setstate Distributor2 blockSize 1
	pragma ArrayRLSBeamXYSP Distributor2  
	star BusInterleave1 BusInterleave
	pragma ArrayRLSBeamXYSP BusInterleave1  
	star MapGr1 MapGr
	setstate MapGr1 parameter_map "Instance = instance_number"
	pragma ArrayRLSBeamXYSP MapGr1  
	star CxToRect1 CxToRect
	pragma ArrayRLSBeamXYSP CxToRect1  
	star "Fork.output=21" Fork
	numports "Fork.output=21" output 2
	pragma ArrayRLSBeamXYSP Fork.output=21  
	star QAM161 QAM16
	pragma ArrayRLSBeamXYSP QAM161  
	star bits1 bits
	setstate bits1 probOfZero 0.5
	pragma ArrayRLSBeamXYSP bits1  
	star CxToRect2 CxToRect
	pragma ArrayRLSBeamXYSP CxToRect2  
	star RectToPolar1 RectToPolar
	pragma ArrayRLSBeamXYSP RectToPolar1  
	star BlackHole1 BlackHole
	pragma ArrayRLSBeamXYSP BlackHole1  
	star DB1 DB
	setstate DB1 min "-100"
	setstate DB1 inputIsPower FALSE
	pragma ArrayRLSBeamXYSP DB1  

#	star "TkPlot.Y=11" TkPlot
#	numports "TkPlot.Y=11" Y 1
#	setstate "TkPlot.Y=11" label "Error Signal"
#	setstate "TkPlot.Y=11" geometry "688x348--4+0"
#	setstate "TkPlot.Y=11" xTitle n
#	setstate "TkPlot.Y=11" yTitle dB
#	setstate "TkPlot.Y=11" xRange "0 100"
#	setstate "TkPlot.Y=11" yRange "-100 10"
#	setstate "TkPlot.Y=11" persistence 100
#	setstate "TkPlot.Y=11" style connect
#	setstate "TkPlot.Y=11" updateSize 10
#	setstate "TkPlot.Y=11" repeat_border_points YES
#	pragma ArrayRLSBeamXYSP TkPlot.Y=11  

	# Replace TkPlot with a printer
	set tmpfile [sdfSetupPrinter Printa1]

	star GainCx1 GainCx
	setstate GainCx1 gain "(0.5,0)"
	pragma ArrayRLSBeamXYSP GainCx1  
	star Quant1 Quant
	setstate Quant1 thresholds "-1 0 1"
	setstate Quant1 levels "-1.5 -0.5 0.5 1.5"
	pragma ArrayRLSBeamXYSP Quant1  
	star Quant2 Quant
	setstate Quant2 thresholds "-1 0 1"
	setstate Quant2 levels "-1.5 -0.5 0.5 1.5"
	pragma ArrayRLSBeamXYSP Quant2  

#	star "TkXYPlot.X=1.Y=11" TkXYPlot
#	numports "TkXYPlot.X=1.Y=11" X 1
#	numports "TkXYPlot.X=1.Y=11" Y 1
#	setstate "TkXYPlot.X=1.Y=11" label "Output Signal"
#	setstate "TkXYPlot.X=1.Y=11" geometry "460x540-0-0"
#	setstate "TkXYPlot.X=1.Y=11" xTitle I
#	setstate "TkXYPlot.X=1.Y=11" yTitle Q
#	setstate "TkXYPlot.X=1.Y=11" xRange "-2.0 2.0"
#	setstate "TkXYPlot.X=1.Y=11" yRange "-2.0 2.0"
#	setstate "TkXYPlot.X=1.Y=11" persistence 100
#	setstate "TkXYPlot.X=1.Y=11" style dot
#	setstate "TkXYPlot.X=1.Y=11" updateSize 10
#	pragma ArrayRLSBeamXYSP TkXYPlot.X=1.Y=11  

	# Replace TkXYPlot with a printer
	set tmpfile2 [sdfSetupPrinter Printb]
	numports Printb input 2

	star RectToCx1 RectToCx
	pragma ArrayRLSBeamXYSP RectToCx1  
	star "Fork.output=22" Fork
	numports "Fork.output=22" output 2
	pragma ArrayRLSBeamXYSP Fork.output=22  
	star Gain1 Gain
	setstate Gain1 gain "PI/180"
	pragma ArrayRLSBeamXYSP Gain1  
	star Gain2 Gain
	setstate Gain2 gain "PI/180"
	pragma ArrayRLSBeamXYSP Gain2  
	star Gain3 Gain
	setstate Gain3 gain "PI/180"
	pragma ArrayRLSBeamXYSP Gain3  
#	star "TkSlider.output=14" TkSlider
#	numports "TkSlider.output=14" output 1
#	setstate "TkSlider.output=14" low "-180"
#	setstate "TkSlider.output=14" high 180
#	setstate "TkSlider.output=14" value 130
#	setstate "TkSlider.output=14" identifier "SNOI 3"
#	setstate "TkSlider.output=14" put_in_control_panel YES
#	pragma ArrayRLSBeamXYSP TkSlider.output=14  
	star Gain4 Gain
	setstate Gain4 gain "PI/180"
	pragma ArrayRLSBeamXYSP Gain4 

	star FloatToCx6 FloatToCx
	pragma ArrayRLSBeamXYSP FloatToCx6  

    star Const1 Const
    star Const2 Const
    star Const3 Const
    star Const4 Const
    setstate Const1 level 32
    setstate Const2 level 67
    setstate Const3 level -140
    setstate Const4 level -90
    connect Const1 output Gain1 input
    connect Const2 output Gain2 input
    connect Const3 output Gain3 input
    connect Const4 output Gain4 input


	star "Nop.input=41" Nop
	numports "Nop.input=41" input 4
	pragma ArrayRLSBeamXYSP Nop.input=41  
	star "Nop.input=42" Nop
	numports "Nop.input=42" input 4
	pragma ArrayRLSBeamXYSP Nop.input=42  
	star IIDGaussian3 IIDGaussian
	setstate IIDGaussian3 mean 0.0
	setstate IIDGaussian3 variance 0.2
	pragma ArrayRLSBeamXYSP IIDGaussian3  
	star FloatToCx7 FloatToCx
	pragma ArrayRLSBeamXYSP FloatToCx7  
	star "Fork.output=23" Fork
	numports "Fork.output=23" output 2
	pragma ArrayRLSBeamXYSP Fork.output=23  
	star SubCx1 SubCx
	pragma ArrayRLSBeamXYSP SubCx1  
	star Trainer1 Trainer
	setstate Trainer1 trainLength 10000
	pragma ArrayRLSBeamXYSP Trainer1  
	star BusFork1 BusFork
	pragma ArrayRLSBeamXYSP BusFork1  

#	star TkXYPlot1 TkXYPlot
#	setstate TkXYPlot1 label "Beam Pattern"
#	setstate TkXYPlot1 geometry "466x546+0+0"
#	setstate TkXYPlot1 xTitle x
#	setstate TkXYPlot1 yTitle y
#	setstate TkXYPlot1 xRange "-1.0 1.0"
#	setstate TkXYPlot1 yRange "-1.0 1.0"
#	setstate TkXYPlot1 persistence 60
#	setstate TkXYPlot1 style connect
#	setstate TkXYPlot1 updateSize 60

	# Replace TkXYPlot with a printer
	set tmpfile3 [sdfSetupPrinter Printc]
	numports Printc input 2


	pragma ArrayRLSBeamXYSP TkXYPlot1  
	star "Nop.input=21" Nop
	numports "Nop.input=21" input 2
	pragma ArrayRLSBeamXYSP Nop.input=21  
	star "Nop.input=22" Nop
	numports "Nop.input=22" input 2
	pragma ArrayRLSBeamXYSP Nop.input=22  
	star BeamXYNorm1 BeamXYNorm
	setstate BeamXYNorm1 Positions Positions
	setstate BeamXYNorm1 AngRes 60
	setstate BeamXYNorm1 wavelength wavelength
	setstate BeamXYNorm1 Decimation "10*NumberElements"
	pragma ArrayRLSBeamXYSP BeamXYNorm1  
	star RLSArrayStO1 RLSArrayStO
	setstate RLSArrayStO1 lambda 0.98
	setstate RLSArrayStO1 delta 0.1
	setstate RLSArrayStO1 steering "(1.0,0.0) (1.0,0.0) (1.0,0.0) (1.0,0.0)"
	setstate RLSArrayStO1 errorDelay 1
	setstate RLSArrayStO1 SaveSteeringFile ""
	pragma ArrayRLSBeamXYSP RLSArrayStO1  
	star SrcPosXY1 SrcPosXY
	setstate SrcPosXY1 AngRes 60
	setstate SrcPosXY1 Decimation "NumberElements*10"
	pragma ArrayRLSBeamXYSP SrcPosXY1  
	star arrayelement1 arrayelement
	setstate arrayelement1 wavelength wavelength
	setstate arrayelement1 NumberSignals NumberSignals
	setstate arrayelement1 NoiseLevel 0.001
	setstate arrayelement1 Positions Positions
	setstate arrayelement1 Instance 1
	pragma ArrayRLSBeamXYSP arrayelement1  
	connect IIDGaussian1 output FloatToCx1 input
	connect IIDGaussian2 output FloatToCx2 input
	connect Commutator1 output Repeat1 input
	connect Commutator2 output Repeat2 input

	connect Repeat1 output Distributor1 input
	connect Repeat2 output Distributor2 input

	busconnect Distributor1 output BusInterleave1 top NumberElements

	busconnect BusInterleave1 output MapGr1 input "2*NumberElements"

	set tmpfile4 [sdfSetupPrinter Printd]
	numports Printd input 8
	star BusForkPrintd BusFork
	busconnect BusInterleave1 output BusForkPrintd input "2*NumberElements"
	busconnect BusForkPrintd outputA Printd input "2*NumberElements"
	busconnect BusForkPrintd outputB MapGr1 input "2*NumberElements"

	connect arrayelement1 out_elem MapGr1 exin
	node node1
	nodeconnect MapGr1 exout node1
	nodeconnect arrayelement1 signal node1
	nodeconnect arrayelement1 angle node1
	connect "Fork.output=21" "output#1" CxToRect2 input
	connect bits1 out QAM161 in
	connect CxToRect2 imag RectToPolar1 y
	connect CxToRect2 real RectToPolar1 x
	connect RectToPolar1 phase BlackHole1 input
#	connect DB1 output "TkPlot.Y=11" "Y#1"
	connect DB1 output Printa1 input

	connect RectToPolar1 magnitude DB1 input
	connect QAM161 out GainCx1 input
	node node2
	nodeconnect CxToRect1 real node2
	nodeconnect Quant1 input node2
	#nodeconnect TkXYPlot.X=1.Y=11 X#1 node2
	nodeconnect Printb input#1 node2

	node node3
	nodeconnect CxToRect1 imag node3
	nodeconnect Quant2 input node3
	#nodeconnect TkXYPlot.X=1.Y=11 Y#1 node3
	nodeconnect Printb input#2 node3

	connect Quant2 output RectToCx1 imag
	connect Quant1 output RectToCx1 real
	connect GainCx1 output "Fork.output=22" input

#	connect "TkSlider.output=11" "output#1" Gain1 input
	connect Gain1 output FloatToCx3 input
#	connect "TkSlider.output=12" "output#1" Gain2 input
	connect Gain2 output FloatToCx4 input
#	connect "TkSlider.output=13" "output#1" Gain3 input
	connect Gain3 output FloatToCx5 input
#	connect "TkSlider.output=14" "output#1" Gain4 input
	connect Gain4 output FloatToCx6 input

	connect FloatToCx3 output "Nop.input=41" "input#1"
	connect FloatToCx4 output "Nop.input=41" "input#2"
	connect FloatToCx5 output "Nop.input=41" "input#3"
	connect FloatToCx6 output "Nop.input=41" "input#4"
	busconnect "Nop.input=42" output Commutator1 input NumberSignals
	connect FloatToCx1 output "Nop.input=42" "input#2"
	connect "Fork.output=22" "output#2" "Nop.input=42" "input#1"
	connect FloatToCx2 output "Nop.input=42" "input#3"
	connect IIDGaussian3 output FloatToCx7 input
	connect FloatToCx7 output "Nop.input=42" "input#4"
	connect "Fork.output=23" "output#1" SubCx1 neg
	connect "Fork.output=23" "output#2" CxToRect1 input
	connect Trainer1 output SubCx1 pos
	connect SubCx1 output "Fork.output=21" input
	connect RectToCx1 output Trainer1 decision
	connect "Fork.output=22" "output#1" Trainer1 train
	connect RLSArrayStO1 output "Fork.output=23" input

	#busconnect MapGr1 output RLSArrayStO1 input NumberElements
	set tmpfile5 [sdfSetupPrinter Printe]
	numports Printe input 4
	star BusForkPrinte BusFork
	busconnect MapGr1 output BusForkPrinte input NumberElements
	busconnect BusForkPrinte outputA Printe input NumberElements
	busconnect BusForkPrinte outputB RLSArrayStO1 input NumberElements


	connect "Fork.output=21" "output#2" RLSArrayStO1 error "*1"
	busconnect "Nop.input=41" output BusFork1 input NumberSignals
	busconnect BusFork1 outputA Commutator2 input NumberSignals
	busconnect BusFork1 outputB SrcPosXY1 input NumberSignals
	busconnect Distributor2 output BusInterleave1 bottom NumberElements

#	connect "Nop.input=21" output TkXYPlot1 X
#	connect "Nop.input=22" output TkXYPlot1 Y
	connect "Nop.input=21" output Printc input#1
	connect "Nop.input=22" output Printc input#2

	connect SrcPosXY1 outputy "Nop.input=22" "input#2"
	connect BeamXYNorm1 outputy "Nop.input=22" "input#1"
	connect SrcPosXY1 outputx "Nop.input=21" "input#2"
	connect BeamXYNorm1 outputx "Nop.input=21" "input#1"
	connect RLSArrayStO1 StO BeamXYNorm1 steering
	run 1
	wrapup
    list [readTmpFile $tmpfile] [readTmpFile $tmpfile2] [readTmpFile $tmpfile3]
    list [readTmpFile $tmpfile5]
} {{{(1.38089943629998,0.972441382035465)	(2.42238719751431,0.457790244747125)	(-0.924428930870639,-1.44716006682269)	(0.698686305832611,-1.52033505488695)	
(-0.879292895521676,-1.24299905833206)	(-0.426412152941345,-1.17607539819528)	(-0.608882802884313,0.916749340694624)	(-0.542549386665472,1.55173818627743)	
(0.979017586166439,-1.62169428459928)	(0.145233172852948,-2.49397016840957)	(-1.06853198435598,1.36089728084631)	(-1.96956286295652,1.26482396529321)	
(-0.0400701616680362,-0.00703889209060293)	(0.278458816130075,0.15416196045224)	(0.844667749594964,0.484105429789597)	(0.847419620233196,0.918646624945943)	
(-1.73283422763208,-0.536365588494532)	(-0.843568887871031,0.843766120105427)	(0.909126470235643,0.874799401467747)	(1.39181393885187,1.42221434307175)	
(0.948776276573172,1.81326705394013)	(1.21325676095111,1.24412801601115)	(0.982852136084095,-1.28497843737819)	(1.20379699715819,-1.10685354188549)	
(-1.87202505312752,1.05995803311602)	(-0.792204586260524,2.46034394176693)	(1.75594005447065,-0.50182741206931)	(2.34063048726428,0.182316995724517)	
(-0.418501090723592,-1.21642695786799)	(-0.275528804362637,-0.707422973035906)	(-0.230463569479962,1.23339858274412)	(-0.0905205342842138,1.22357622904846)	
(1.14232370786687,2.10253221508607)	(1.58488532625759,2.10136794293335)	(-0.450330538422978,-2.48536180450553)	(1.03868103941408,-3.33550619784985)	
(-0.674911910558447,-1.21939913470878)	(-0.342500968202754,-1.08321008493452)	(-0.387621946640843,1.05851255791722)	(-0.391688297051188,1.4907191770769)	
(0.42188445805607,-0.505198902327105)	(0.251279484074967,0.00780073845852309)	(-0.837198287995103,0.247405242813304)	(-0.228322442951166,-0.705736426576512)	
(0.751849264325935,1.35356453955315)	(1.9015404332147,0.131666301242036)	(-1.17760383437669,-2.13691430303877)	(0.0537954959993318,-1.50858596760081)	
(-1.44478954809354,1.71292093775367)	(-0.959670599322543,1.64065898467958)	(2.51072534565122,-0.75020399560966)	(1.77287845240656,0.667255822794274)	
(-1.79232681060484,-0.804345745351687)	(-1.65806105402894,-0.00831440379151725)	(0.708184854749945,1.01367021029098)	(0.186709318528836,1.56515838288998)	
(0.399505225179379,-1.90988070293576)	(-0.451414741456373,-1.58895411204001)	(0.00350606383213741,2.34062602203702)	(-0.830147120806851,1.87794801796882)	
(1.07171643797578,1.568293295031)	(1.95426906668383,0.395682644079905)	(-0.789825486889654,-2.03102645736587)	(0.256594209662542,-1.73491899940512)	
(-1.51889752138742,-0.88018838410787)	(-1.50735838200436,-0.415203942507809)	(0.828515404687188,1.25690217135945)	(0.105240338104761,2.01763736553182)	
(0.45620919518142,1.25090012674476)	(0.762195582787004,1.06630370656434)	(-1.51801083781672,-2.16368720923187)	(-0.27452629059719,-2.93167005531757)	
(-1.29225560767572,-3.02837855816173)	(-0.604538562489567,-1.3916607616194)	(-1.98039004071482,2.10615542929693)	(-0.827132984871624,1.58008288807071)	
(-1.5731199656321,-0.974697108669986)	(-1.44844925316657,-0.591589918748772)	(0.737848860363554,1.32245990461593)	(0.00340126356130274,2.16227869301738)	
(-1.26705601225026,0.858327498478767)	(0.183314546985955,1.10246178282924)	(0.337340980935523,-0.965294947845532)	(1.29203667247837,0.0794221907545446)	
(0.238792850552333,1.91281899055889)	(0.811913716296463,0.667951963444838)	(0.510767547760868,-1.87843134554468)	(0.527762374924904,-0.926389711808049)	
(-0.582761208725959,1.71383559073401)	(0.000236069526832573,1.57250281772179)	(1.48803086548168,-1.22482649223734)	(1.44929434685533,-0.417847024220993)	
(-0.284457381823602,-1.49180406760231)	(-1.04597759147177,-1.70741124823737)	(-0.248668821355097,1.5166030266506)	(-1.38167220092609,1.54215728613907)	
(0.258812432329406,1.29296599779028)	(-0.450659891862087,1.12761496154857)	(1.78065894159325,-0.389878296445104)	(0.840722069126113,-0.299506462320954)	
(-1.1470965736029,-0.163202744547718)	(-1.79943338371573,0.0387164948996881)	(1.63446661734951,0.925813654758457)	(0.166456065764081,1.44767492596327)	
(-1.73269294537473,2.45478216540712)	(-1.53910192878466,3.38816741988392)	(2.71485114513381,-1.52575284520307)	(2.31990394516612,-1.04326659171429)	
(0.0078693045014568,1.58017198722292)	(-0.188427758663486,1.45676784424108)	(0.0942505242534134,-1.76430717070636)	(0.203408345318721,-2.05844812072298)	
(0.982789831694433,-1.25142441416978)	(1.5442049875051,-2.4031583770269)	(-2.53568993158714,0.165214041922754)	(-1.76356062354114,0.328963823096244)	
(0.511284210258713,-0.416118667780798)	(0.791186129428937,-0.0215301115590398)	(-2.49528826504546,-0.849252156541316)	(-0.896568627008096,-2.1411612651886)	
(-0.972681063504698,0.456161811400776)	(-1.04885197961555,1.02034550281625)	(2.07700092837524,0.480739346541572)	(1.26131620821012,1.09904378393971)	
(-0.0605883669990642,-0.673715890101163)	(0.0680575211892903,0.705856702083674)	(-1.29285286575969,-0.0211317794071515)	(0.0527378224373716,-1.39400188570147)	
(-1.68839440632117,-1.2188717589718)	(-1.38332405183264,-0.623688500795979)	(0.526998650430517,1.45145086359678)	(0.028252344815122,2.24932221520431)	
(-2.21728231233101,1.50897214404652)	(-1.17469200651298,1.65521800687897)	(1.72943436522705,-1.03269805632705)	(1.59193875596467,0.580883286814188)	
(0.900351741252275,1.27380327014007)	(1.51187250746057,-0.0596754622253812)	(0.614342091808677,-0.955313102292428)	(0.652380457214512,0.0790086858956557)	
(-0.148611621171659,2.01247466468761)	(0.483255464921601,1.87369077290003)	(0.234368747029034,-2.19949056434576)	(1.05088270162514,-2.04449403894709)	
(-2.22607952648258,-1.05946561468358)	(-1.72992739913502,0.319844167970867)	(0.168532855446234,0.968711579161415)	(0.387318265475973,1.23627466856446)	
(-0.168117638535208,-1.58188159977838)	(-1.52854451743331,-1.23389960718143)	(1.4877956967967,2.63036096265505)	(-0.402046331960281,2.65138731792337)	
(-0.705763860510433,-0.338318960762076)	(-0.933423676525001,-0.52352845069249)	(0.110915753510567,0.31042491931906)	(-0.504640928665216,0.692178725650838)	
(-0.0745997078234156,-0.75249517952281)	(-0.363493347218986,-0.546199665487688)	(0.400529145502724,1.04121600569162)	(-0.150814929403842,1.05920419792287)	
}}}
