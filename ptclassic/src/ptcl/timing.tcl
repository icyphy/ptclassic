# $Id$
# This file is produced from the "pigiLog.pt" output by pigi version 0.3.1
# when running the "timing" DDF demo.  Because pigi writes out setstate
# commands for every star state, even though most of the states have
# default values, this file is longer than necessary.
#
# It is transformed to ptcl's input language by adding curly braces around
# the "defgalaxy" stuff, removing all the parentheses, and changing the
# "state" command to "newstate" (this change will make way for other
# state inquiry functions).

reset
domain DDF
defgalaxy PLLControl {
	domain SDF
	star FloatGain1 FloatGain
	setstate FloatGain1 gain "0.03"
	star Fork.output=21 Fork
	numports Fork.output=21 output 2
	star FloatDC1 FloatDC
	setstate FloatDC1 level "0.0"
	star Integrator1 Integrator
	setstate Integrator1 feedbackGain "1.0"
	setstate Integrator1 top "1.0"
	setstate Integrator1 bottom "0.0"
	setstate Integrator1 saturate "NO"
	setstate Integrator1 state "1/3"
	star FloatDiff1 FloatDiff
	star FloatThresh1 FloatThresh
	setstate FloatThresh1 threshold "0.0"
	star Xgraph1 Xgraph
	setstate Xgraph1 title "Timing control"
	setstate Xgraph1 options "-0 adjustment =800x200+0+675"
	star Integrator2 Integrator
	setstate Integrator2 feedbackGain "0.99"
	setstate Integrator2 top "1/8"
	setstate Integrator2 bottom "-1/8"
	setstate Integrator2 saturate "YES"
	setstate Integrator2 state "0.0"
	star FloatSum.input=21 FloatSum
	numports FloatSum.input=21 input 2
	star FloatDC2 FloatDC
	setstate FloatDC2 level "1/8"
	alias in FloatGain1 "input"
	node node1
	nodeconnect FloatDC1 "output" node1
	nodeconnect Integrator1 "reset" node1
	nodeconnect Integrator2 "reset" node1
	alias out FloatThresh1 "output"
	node node2
	nodeconnect Integrator2 "output" node2
	nodeconnect FloatSum.input=21 "input#1" node2
	nodeconnect Xgraph1 "input" node2
	connect Integrator1 "output" Fork.output=21 "input"
	connect Fork.output=21 "output#1" FloatDiff1 "pos" 1
	connect Fork.output=21 "output#2" FloatDiff1 "neg"
	connect FloatDiff1 "output" FloatThresh1 "input"
	connect FloatGain1 "output" Integrator2 "data"
	connect FloatSum.input=21 "output" Integrator1 "data"
	connect FloatDC2 "output" FloatSum.input=21 "input#2"
}
domain SDF
defgalaxy bits {
	domain SDF
	newstate probOfZero FLOAT "0.5"
	star IIDUniform1 IIDUniform
	setstate IIDUniform1 lower "0.0"
	setstate IIDUniform1 upper "1/probOfZero"
	star FloatThresh2 FloatThresh
	setstate FloatThresh2 threshold "1.0"
	connect IIDUniform1 "output" FloatThresh2 "input"
	alias out FloatThresh2 "output"
}
domain DDF
defgalaxy ask {
	domain SDF
	newstate level FLOAT "1.0"
	star bits1 bits
	setstate bits1 probOfZero "0.5"
	star FloatGain2 FloatGain
	setstate FloatGain2 gain "2.0*level"
	star FloatDiff2 FloatDiff
	star FloatDC3 FloatDC
	setstate FloatDC3 level "level"
	star FIR1 FIR
	setstate FIR1 taps "< ~ptolemy/src/domains/sdf/demo/rc100"
	setstate FIR1 decimation "1"
	setstate FIR1 decimationPhase "0"
	setstate FIR1 interpolation "8"
	connect bits1 "out" FloatGain2 "input"
	connect FloatGain2 "output" FloatDiff2 "pos"
	connect FloatDC3 "output" FloatDiff2 "neg"
	connect FloatDiff2 "output" FIR1 "signalIn"
	alias out FIR1 "signalOut"
}
domain DDF
defgalaxy baud {
	domain SDF
	star Fork.output=31 Fork
	numports Fork.output=31 output 3
	star FloatDiff3 FloatDiff
	star FloatProduct.input=21 FloatProduct
	numports FloatProduct.input=21 input 2
	star Xgraph2 Xgraph
	setstate Xgraph2 title "Baud-rate samples"
	setstate Xgraph2 options "-P -nl =800x200+0+450 -0 ''"
	star FloatThresh3 FloatThresh
	setstate FloatThresh3 threshold "0.0"
	star FloatGain3 FloatGain
	setstate FloatGain3 gain "2.0"
	star FloatSum.input=22 FloatSum
	numports FloatSum.input=22 input 2
	star FloatDC4 FloatDC
	setstate FloatDC4 level "-1.0"
	alias deriv FloatProduct.input=21 "input#1"
	alias signal Fork.output=31 "input"
	connect FloatSum.input=22 "output" FloatDiff3 "neg"
	connect Fork.output=31 "output#1" FloatDiff3 "pos"
	connect FloatDiff3 "output" FloatProduct.input=21 "input#2"
	alias out FloatProduct.input=21 "output"
	connect Fork.output=31 "output#3" Xgraph2 "input"
	connect Fork.output=31 "output#2" FloatThresh3 "input"
	connect FloatThresh3 "output" FloatGain3 "input"
	connect FloatGain3 "output" FloatSum.input=22 "input#1"
	connect FloatDC4 "output" FloatSum.input=22 "input#2"
}
reset
# Creating universe 'timing'
domain DDF
target default-DDF
	star BlackHole1 BlackHole
	star BlackHole2 BlackHole
	star FloatDC5 FloatDC
	setstate FloatDC5 level "0.0"
	star Xgraph3 Xgraph
	setstate Xgraph3 title "PLL error signal"
	setstate Xgraph3 options "-0 control =800x200+0+225"
	star FIR2 FIR
	setstate FIR2 taps "1.0 0.0 -1.0"
	setstate FIR2 decimation "1"
	setstate FIR2 decimationPhase "0"
	setstate FIR2 interpolation "1"
	star XMgraph.input=21 XMgraph
	numports XMgraph.input=21 input 2
	setstate XMgraph.input=21 title "Line signal and its estimated derivative"
	setstate XMgraph.input=21 options "-0 derivative -1 signal =800x200+0+0"
	star Fork.output=22 Fork
	numports Fork.output=22 output 2
	star PLLControl1 PLLControl
	star Fork.output=32 Fork
	numports Fork.output=32 output 3
	star ask1 ask
	setstate ask1 level "1.0"
	star baud1 baud
	star Case.output=21 Case
	numports Case.output=21 output 2
	star Case.output=22 Case
	numports Case.output=22 output 2
	star EndCase.input=21 EndCase
	numports EndCase.input=21 input 2
	node node3
	nodeconnect FIR2 "signalOut" node3
	nodeconnect XMgraph.input=21 "input#1" node3
	nodeconnect Case.output=21 "input" node3
	connect Fork.output=22 "output#1" Xgraph3 "input"
	connect Fork.output=22 "output#2" PLLControl1 "in" 1
	connect Fork.output=32 "output#1" FIR2 "signalIn"
	connect Fork.output=32 "output#2" XMgraph.input=21 "input#2" 1
	connect ask1 "out" Fork.output=32 "input"
	connect baud1 "out" EndCase.input=21 "input#2"
	connect FloatDC5 "output" EndCase.input=21 "input#1"
	node node4
	nodeconnect PLLControl1 "out" node4
	nodeconnect Case.output=22 "control" node4
	nodeconnect EndCase.input=21 "control" node4
	nodeconnect Case.output=21 "control" node4
	connect Case.output=22 "output#1" BlackHole2 "input"
	connect Case.output=22 "output#2" baud1 "signal"
	connect Case.output=21 "output#1" BlackHole1 "input"
	connect Case.output=21 "output#2" baud1 "deriv"
	connect EndCase.input=21 "output" Fork.output=22 "input"
	connect Fork.output=32 "output#3" Case.output=22 "input" 1
run 200
wrapup
