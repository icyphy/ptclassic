# Tests for SDF Contrib ArrayRLS demo
#
# @Author: Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1997 The Regents of the University of California.
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


######################################################################
#### test ArrayRLSBeamXYSP
test  ArrayRLSBeamXYSP {ArrayRLSBeamXYSP} {
    set star ArrayRLSBeamXYSP
    sdfInitUniverse $star

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
	alias out_elem "AddCx.input=21" output
	connect SubCx1 output Abs1 input
	connect AddCx1 output "AddCx.input=21" "input#1"
	connect IIDGaussian1 output RectToCx2 real
	connect IIDGaussian2 output RectToCx2 imag
	connect RectToCx2 output "AddCx.input=21" "input#2"
	connect ConstCx1ofN1 output SubCx1 neg
}

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
	busconnect MapGr1 output RLSArrayStO1 input NumberElements
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
} {{{15.4414773254157	
-0.2928459489707	
4.55559816431993	
1.58999235299081	
-18.0212307626354	
-21.7375921625936	
-23.6824698860826	
-23.6117216010728	
-29.9401289149185	
-26.1596817698038	
-27.2323034023873	
-41.3142128471832	
-27.1544305094342	
-36.9600387750053	
-25.2602349175368	
-30.5089711141787	
-27.5062577914145	
-23.2993020271337	
-34.4124806758013	
-27.9992752671985	
-28.6205882981682	
-32.63104420022	
-40.3110193276091	
-38.1752202421564	
-31.0279505711404	
-31.8388264800961	
-37.0983961232878	
-33.2656546992235	
-32.7305617861914	
-42.0445389126054	
-31.9334324496928	
-37.7440913659655	
-37.2390247353523	
-38.9625562696141	
-28.1904477668728	
-32.7197635371919	
-28.0957750768826	
-24.8518359701817	
-26.687270550287	
-30.2273721192814	
}} {{3.57754400877626	-1.53726349492705	
1.1437862904828	0.22134049663321	
2.8192436646946	1.5556002197945	
-0.327994615568307	-1.23824402369866	
0.605796287287573	1.5676667427745	
-0.522461174216321	-0.578727738344514	
-0.534540405255773	0.444412149766774	
-0.434037250594426	0.501519323165603	
0.472334010921549	-0.51576306879371	
-1.52566919904494	-0.458020258754483	
1.46430481499174	0.524843389353294	
-0.5085563216402	-0.499176484777047	
-1.45975048308781	-0.482520414319169	
-0.509145302591541	-1.4891494669442	
-1.54015706901967	1.46304374033625	
0.471229297047975	-1.50785228978348	
1.46184702678006	-0.517890487684525	
-1.4391435518262	1.46878148136598	
1.48190346925038	0.494121928860782	
-0.460420381751494	1.50431410643144	
1.52555494685664	0.47315227104314	
-0.480376401952254	1.51267047973425	
1.50957713306045	1.49883061617624	
0.509026383549913	-0.508411099423635	
-1.51665386009161	-1.52262479638297	
-1.49637281611389	1.47466905792923	
1.50911693692823	1.48941992706535	
-0.521375696619826	-1.49618838760381	
0.479176512408122	1.50998239822711	
-1.50753262704495	0.497610126580963	
-1.48754508500135	0.477964163535132	
1.49918498822184	-0.512940043144085	
1.50403333558091	0.51313673251076	
-0.506017325811429	1.50952756192601	
1.53886524961122	-1.49747287969191	
-0.503862731996414	-1.47720366767558	
0.465647563263348	-1.4807585299666	
0.552127896963289	-1.47644780744834	
-0.499438064460246	1.54630250549002	
1.47578475576654	1.48095756150064	
}} {{0.971755509664434	-0.102135619708489	0.0	0.0	
0.957609029655445	-0.203546082767728	0.0	0.0	
0.915345496066358	-0.29741378053004	0.0	0.0	
0.848021024065434	-0.377563285658652	0.0	0.0	
0.760311984777378	-0.438966329079318	0.0	0.0	
0.658013732061251	-0.478074960354023	0.0	0.0	
0.547507386989086	-0.492977865527916	0.0	0.0	
0.435270808511111	-0.483417207272261	0.0	0.0	
0.327467535778399	-0.450720395786635	0.0	0.0	
0.229606214723372	-0.39768962963445	0.0	0.0	
0.146240309608452	-0.328461113207721	0.0	0.0	
0.0806819530292494	-0.248313518585268	0.0	0.0	
0.0347306662030128	-0.163394937940941	0.0	0.0	
0.00845305880351099	-0.0804254822095787	0.0	0.0	
-2.18439518359561e-18	-0.0118912935284223	0.0	0.0	
-0.00612397883144769	-0.058265766512337	0.0	0.0	
-0.021581686006915	-0.101533849801442	0.0	0.0	
-0.0406342136617825	-0.12505925043293	0.0	0.0	
-0.0572848705345794	-0.128663925809007	0.0	0.0	
-0.066204425588009	-0.114669428804344	0.0	0.0	
-0.0635126058631202	-0.0874176024320101	0.0	0.0	
-0.0474014510164575	-0.0526446447199406	0.0	0.0	
-0.0186030107717739	-0.0167502261350215	0.0	0.0	
-0.021026149718825	-0.0152763919709343	0.0	0.0	
-0.0646600096552014	-0.0373314739802343	0.0	0.0	
-0.107081955795164	-0.0476759583989475	0.0	0.0	
-0.141443306184527	-0.0459577160796544	0.0	0.0	
-0.161689601612174	-0.0343681857764793	0.0	0.0	
-0.163422586964467	-0.0171764060280387	0.0	0.0	
-0.144666834357301	1.77165775638449e-17	0.0	0.0	
-0.10637870839063	0.0111808527939476	0.0	0.0	
-0.0534172545659132	0.0113541879643828	0.0	0.0	
-0.0281513685661619	0.00914693412305791	0.0	0.0	
-0.0900889875367718	0.040110201481774	0.0	0.0	
-0.15472794800415	0.0893322224313542	0.0	0.0	
-0.208993789847365	0.151842876413126	0.0	0.0	
-0.2467299602049	0.222156654017937	0.0	0.0	
-0.264292863535086	0.293526961822111	0.0	0.0	
-0.260606264981588	0.358693751482178	0.0	0.0	
-0.237144262651773	0.410745911636329	0.0	0.0	
-0.197647853141765	0.443924346439624	0.0	0.0	
-0.1475978465921	0.454259462579025	0.0	0.0	
-0.0935177585414413	0.439966462605001	0.0	0.0	
-0.0422051705805547	0.401555374756031	0.0	0.0	
2.09203207718768e-17	0.341654765871145	0.0	0.0	
0.0278100854173975	0.264595288164181	0.0	0.0	
0.0373767704207317	0.175843879516438	0.0	0.0	
0.0264597107163704	0.0814346160701928	0.0	0.0	
0.0063733387432168	0.0143147531898134	0.0	0.0	
0.0587908910367125	0.101828810297832	0.0	0.0	
0.130575752836127	0.179722105455557	0.0	0.0	
0.21869820269404	0.242888960882652	0.0	0.0	
0.319703264451236	0.287862112287115	-1.83697019872103e-16	-1.0	
0.429360201975464	0.311948446568146	0.0	0.0	
0.542662972577778	0.313306613297022	-0.743144825477394	-0.669130606358858	
0.653905927775415	0.291137676538907	0.0	0.0	
0.756904124615982	0.245933058247659	0.4067366430758	0.913545457642601	
0.845391482012311	0.179693506681661	0.0	0.0	
0.913569298865951	0.096020002519506	0.866025403784439	0.5	
0.956718687670479	0.0	0.0	0.0	
}}}
