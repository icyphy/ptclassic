# Tests for SDF Contrib stars
#
# @Author: Christopher Hylands
#
# @Version: @(#)testSDFContrib.tcl	1.3 03/28/98
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


######################################################################
# Actual start of the tests.  We use one test per star so that
# the output is easier to read.


######################################################################
#### test SDFBeamAbs
#
test SDFBeamAbs {SDFBeamAbs} {
    set star BeamAbs
    sdfInitUniverse $star

    sdfTestRampCx RectToCxA
    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star

    connect "$star.a" output Printa input
    connect RectToCxA output "$star.a" steering
    setstate "$star.a" AngRes 5
    run 3
    wrapup
    readTmpFile $tmpfile

} {{0.0	
0.0	
0.0	
0.0	
0.0	
1.11803398874989	
1.11803398874989	
1.11803398874989	
1.11803398874989	
1.11803398874989	
2.23606797749979	
2.23606797749979	
2.23606797749979	
2.23606797749979	
2.23606797749979	
}}

######################################################################
#### test SDFBeamXYNorm
#
#test SDFBeamXYNorm {SDFBeamXYNorm} {\
#    set star BeamXYNorm
#    sdfInitUniverse $star
#    sdfTestRampCx
#    star "$star.a" $star
#    set tmpfile [sdfSetupPrinter]
#    connect "$star.a" steering RectToCx.a output
#    numports Printa input 2
#    connect  "$star.a" outputx Printa input#1
#    connect  "$star.a" outputy Printa input#2
#
#    run 500
#    
#    star "$star.a" $star
#    set tmpfile [sdfSetupPrinter]
#
#    list [readTmpFile $tmpfile]
#
#} {}


######################################################################
#### test SDFCIIDGaussian
#

######################################################################
#### test SDFConstCx1ofN
# 
test SDFConstCx1ofN {SDFConstCx1ofN} {
    set star ConstCx1ofN
    sdfInitUniverse $star

    set tmpfile [sdfSetupPrinter]

    star "$star.a" $star
    connect "$star.a" output Printa input
    setstate "$star.a" ComplexValues "(1.1,2.2) (3.3,4.4)"
    setstate "$star.a" ValueIndex 2
    run 2
    wrapup
    readTmpFile $tmpfile
}  {{(3.3,4.4)	
(3.3,4.4)	
}}

######################################################################
#### test SDFRLSArrayStO
test SDFRLSArrayStO {SDFRLSArrayStO} {
    set star RLSArrayStO
    sdfInitUniverse $star
    star "$star.a" $star

    sdfTestRampCx RectToCxA
    sdfTestRampCx RectToCxB
    set tmpfile [sdfSetupPrinter]
    set tmpfile2 [sdfSetupPrinter Printb]
    connect RectToCxA output "$star.a" input
    connect RectToCxB output "$star.a" error
    connect  "$star.a" output Printa input
    connect  "$star.a" StO Printb input

    setstate "$star.a" lambda 0.98
    setstate "$star.a" delta 0.1
    setstate "$star.a" steering "(1.0,0.0) (1.0,0.0) (1.0,0.0) (1.0,0.0)"
    setstate "$star.a" errorDelay 1
    setstate "$star.a" SaveSteeringFile ""

    run 100
    wrapup
    list [readTmpFile $tmpfile] [readTmpFile $tmpfile2]
} {{{(0.0,0.0)	
(1.0,0.5)	
(5.71990817481069,2.85995408740534)	
(12.1415460577345,6.07077302886726)	
(19.6289190790156,9.81445953950779)	
(27.9061410254715,13.9530705127357)	
(36.8157601374585,18.4078800687293)	
(46.2551435611923,23.1275717805962)	
(56.151852285678,28.075926142839)	
(66.4519421947392,33.2259710973696)	
(77.1136501656182,38.5568250828091)	
(88.1036729078795,44.0518364539398)	
(99.3948267091107,49.6974133545553)	
(110.964499951725,55.4822499758626)	
(122.793588058634,61.3967940293168)	
(134.865736057379,67.4328680286894)	
(147.166784976148,73.5833924880742)	
(159.684357719178,79.8421788595888)	
(172.407543037063,86.2037715185314)	
(185.326650135825,92.6633250679126)	
(198.433015211893,99.2165076059463)	
(211.718846855433,105.859423427716)	
(225.17710101955,112.588550509775)	
(238.801378804624,119.400689402312)	
(252.585842077278,126.292921038639)	
(266.525143194434,133.262571597217)	
(280.614366001782,140.307183000891)	
(294.848975931756,147.424487965878)	
(309.224777511111,154.612388755555)	
(323.737877951482,161.868938975741)	
(338.384655771629,169.192327885815)	
(353.161733610843,176.580866805422)	
(368.065954556153,184.032977278077)	
(383.094361433228,191.547180716614)	
(398.244178611128,199.122089305564)	
(413.512795950604,206.756397975302)	
(428.897754589236,214.448877294618)	
(444.396734307938,222.198367153969)	
(460.007542264804,230.003771132402)	
(475.728102916182,237.864051458091)	
(491.556448972555,245.778224486277)	
(507.49071325974,253.74535662987)	
(523.529121374883,261.764560687441)	
(539.669985042515,269.834992521257)	
(555.911696089212,277.955848044606)	
(572.252720966475,286.126360483238)	
(588.69159576088,294.34579788044)	
(605.226921638469,302.613460819234)	
(621.857360677176,310.928680338588)	
(638.58163204681,319.290816023405)	
(655.398508501124,327.699254250562)	
(672.306813150724,336.153406575362)	
(689.30541648928,344.65270824464)	
(706.393233648662,353.196616824331)	
(723.569221861382,361.784610930691)	
(740.832378111156,370.416189055578)	
(758.181736954454,379.090868477227)	
(775.616368497777,387.808184248888)	
(793.135376516987,396.567688258494)	
(810.737896706441,405.368948353221)	
(828.423095046919,414.21154752346)	
(846.190166282451,423.095083141225)	
(864.038332497102,432.019166248551)	
(881.966841783658,440.983420891829)	
(899.974966996917,449.987483498459)	
(918.062004584963,459.031002292481)	
(936.227273492434,468.113636746217)	
(954.470114130324,477.235057065162)	
(972.789887407347,486.394943703674)	
(991.185973818339,495.59298690917)	
(1009.65777258557,504.828886292784)	
(1028.20470084916,514.102350424581)	
(1046.8261929032,523.413096451599)	
(1065.5216994743,532.760849737149)	
(1084.29068703977,542.145343519887)	
(1103.13263718271,551.566318591355)	
(1122.04704598147,561.023522990737)	
(1141.03342343139,570.516711715697)	
(1160.09129289655,580.045646448274)	
(1179.22019058969,589.610095294845)	
(1198.41966507856,599.209832539281)	
(1217.68927681693,608.844638408465)	
(1237.02859769882,618.51429884941)	
(1256.43721063454,628.218605317271)	
(1275.91470914718,637.957354573588)	
(1295.46069698832,647.730348494162)	
(1315.07478777198,657.537393885991)	
(1334.75660462547,667.378302312733)	
(1354.50577985643,677.252889928217)	
(1374.32195463508,687.160977317541)	
(1394.20477869066,697.102389345329)	
(1414.15391002148,707.076955010742)	
(1434.16901461779,717.084507308894)	
(1454.24976619659,727.124883098295)	
(1474.39584594805,737.197922974023)	
(1494.60694229261,747.303471146306)	
(1514.88275064846,757.441375324229)	
(1535.22297320862,767.611486604311)	
(1555.62731872736,777.813659363682)	
(1576.09550231529,788.047751157646)	
}} {{(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(2.85995408740534,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(6.13596002290975,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(7.71136501656182,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(8.77097057561668,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(9.57819683539237,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(10.2353227736159,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(10.7928602308378,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(11.279488525721,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(11.7130640767979,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(12.1054616385475,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(12.4649790803543,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(12.7976433861061,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(13.1079701700225,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(13.39943003447,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(13.6747478709825,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(13.93610213705,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(14.1852617195823,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(14.4236824655081,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(14.6525768518888,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(14.8729652935455,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(15.0857146060832,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(15.2915672996742,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(15.491164207674,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(15.6850621909367,0.0)	
(1.0,0.0)	
(1.0,0.0)	
(1.0,0.0)	
}}}

######################################################################
#### test SDFRLSCx
test SDFRLSCx { SDFRLSCx} {
    set star RLSCx
    sdfInitUniverse $star

    sdfTestRampCx RectToCxA
    sdfTestRampCx RectToCxB
    set tapfile [::tycho::tmpFileName RLSCx]
    set tmpfile [sdfSetupPrinter]
    star "$star.a" $star
    connect  "$star.a" output Printa input
    connect RectToCxA output "$star.a" input
    connect RectToCxB output "$star.a" error
    setstate "$star.a" SaveTapsFile $tapfile
    run 16
    wrapup
    list [readTmpFile $tmpfile] [readTmpFile $tapfile]
} {{{(0.0,0.0)	
(0.0,0.0)	
(3.72781771388895,1.86390885694447)	
(11.9713167161342,5.98565835806709)	
(24.5968645608257,12.2984322804128)	
(42.4760292892306,21.2380146446153)	
(66.4879898335967,33.2439949167983)	
(93.7656881630318,46.8828440815159)	
(123.219780550127,61.6098902750634)	
(154.31809763061,77.1590488153048)	
(186.745274181424,93.3726370907118)	
(220.292658489675,110.146329244837)	
(254.812022180775,127.406011090387)	
(290.192675573729,145.096337786865)	
(326.348847233177,163.174423616589)	
(363.212151111664,181.606075555832)	
}} {{(2.20039 ,-0)
(2.58438 ,-0)
(1.22381 ,-0)
(-5.7703 ,-0)
(31.5781 ,-0)
}}}

######################################################################
#### test SDFRLSCxTO
test SDFRLSCxTO { SDFRLSCxTO} {
    set star RLSCxTO
    sdfInitUniverse $star

    sdfTestRampCx RectToCxA
    sdfTestRampCx RectToCxB
    set tapfile [::tycho::tmpFileName RLSCx]
    set tmpfile [sdfSetupPrinter]
    set tmpfile2 [sdfSetupPrinter Printb]
    star "$star.a" $star
    connect  "$star.a" output Printa input
    connect  "$star.a" TO Printb input
    connect RectToCxA output "$star.a" input
    connect RectToCxB output "$star.a" error
    setstate "$star.a" SaveTapsFile $tapfile
    run 16
    wrapup
    list [readTmpFile $tmpfile] [readTmpFile $tmpfile2] [readTmpFile $tapfile]
} {{{(0.0,0.0)	
(0.0,0.0)	
(3.72781771388895,1.86390885694447)	
(11.9713167161342,5.98565835806709)	
(24.5968645608257,12.2984322804128)	
(42.4760292892306,21.2380146446153)	
(66.4879898335967,33.2439949167983)	
(93.7656881630318,46.8828440815159)	
(123.219780550127,61.6098902750634)	
(154.31809763061,77.1590488153048)	
(186.745274181424,93.3726370907118)	
(220.292658489675,110.146329244837)	
(254.812022180775,127.406011090387)	
(290.192675573729,145.096337786865)	
(326.348847233177,163.174423616589)	
(363.212151111664,181.606075555832)	
}} {{(0.0,0.0)	
(0.0,0.0)	
(1.0,0.0)	
(0.0,0.0)	
(0.0,0.0)	
(2.16790862838969,-0.0)	
(2.23379541548255,-0.0)	
(1.0,-0.0)	
(0.0,-0.0)	
(0.0,-0.0)	
(2.02252562598673,-0.0)	
(2.67501224103193,-0.0)	
(4.01898712752365,-0.0)	
(3.47244868322755,-0.0)	
(11.7095809184324,-0.0)	
(2.14502800165847,-0.0)	
}} {{(2.20039 ,-0)
(2.58438 ,-0)
(1.22381 ,-0)
(-5.7703 ,-0)
(31.5781 ,-0)
}}}

######################################################################
#### test SDFSrcPosXY
test SDFSrcPosXY {SDFSrcPosXY} {
    set star SrcPosXY
    sdfInitUniverse $star

    star "$star.a" $star
    set tmpfile [sdfSetupPrinter]

    star Const1 Const
    star Const2 Const
    star Const3 Const
    star Const4 Const
    setstate Const1 level 32
    setstate Const2 level 67
    setstate Const3 level -140
    setstate Const4 level -90
    star FloatToCx1 FloatToCx 
    connect Const1 output FloatToCx1 input
    star FloatToCx2 FloatToCx 
    connect Const2 output FloatToCx2 input
    star FloatToCx3 FloatToCx 
    connect Const3 output FloatToCx3 input
    star FloatToCx4 FloatToCx 
    connect Const4 output FloatToCx4 input
    star "Nop.input=41" Nop
    numports "Nop.input=41" input 4
    connect FloatToCx1 output "Nop.input=41" "input#1"
    connect FloatToCx2 output "Nop.input=41" "input#2"
    connect FloatToCx3 output "Nop.input=41" "input#3"
    connect FloatToCx4 output "Nop.input=41" "input#4"

    setstate "$star.a" AngRes 8
    setstate "$star.a" Decimation 10
    busconnect "Nop.input=41" output "$star.a" input 4
    numports Printa input 2
    connect  "$star.a" outputx Printa input#1
    connect  "$star.a" outputy Printa input#2

    run 50
    
    #set retval [exec wc -l $tmpfile]
    # This creates 366 lines of output, so we don't bother displaying it.
    readTmpFile $tmpfile
    list {}
} {{}}

