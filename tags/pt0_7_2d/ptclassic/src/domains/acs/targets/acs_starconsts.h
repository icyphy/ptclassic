/**********************************************************************
Copyright (c) 1999 Sanders, a Lockheed Martin Company
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE SANDERS, A LOCKHEED MARTIN COMPANY BE LIABLE TO ANY 
PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
SANDERS, A LOCKHEED MARTIN COMPANY HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

SANDERS, A LOCKHEED MARTIN COMPANY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND SANDERS, A LOCKHEED MARTIN
COMPANY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
                                                        COPYRIGHTENDKEY


 Programmers:  Ken Smith
 Date of creation: 3/23/98
 Version: @(#)acs_starconsts.h      1.0     06/16/99
***********************************************************************/
#ifndef ACS_STARCONSTS_H
#define ACS_STARCONSTS_H
// General constants
static const int MAX_STR=80;
static const int MAX_PATH=160;

// FPGA types
static const int UNKNOWN_DEVICE=0;
static const int XC4000=1;
static const int VIRTEX=2;

// General star constants
static const int UNDEFINED=-1;
static const int DEBUG_STARS=0;
static const int DEBUG_BUILD=0;
static const int DEBUG_MEM=0;

// Scheduler constants
// FIX:This constant should be N^2 proporational to the number of smart 
// generators
static const int RUNAWAY_SCHEDULER=1000;
static const int UNSCHEDULED=-1;
static const int NONSCHEDULED=-2;

// BW propagation constants
// FIX:This constant should be N^2 proporational to the number of smart generators
static const int RUNAWAY_BW=10;

// Where was the star derived from, implies different operations
static const int DYNAMIC=0;
static const int ALGORITHM=1;

// Star origin constants
static const int USER_GENERATED=0;
static const int ACS_GENERATED=1;

// Star domain constants
static const int SW=0;
static const int HW=1;

// Star existence constants
static const int SOFT=0;
static const int HARD=1;

// Star language constants
static const int ANY_LANG=0;
static const int VHDL_BEHAVIORAL=1;
static const int VHDL_STRUCTURAL=2;
static const int XNF=3;
static const int C=4;
static const int CC=5;

// Star macro constants
static const int NORMAL_STAR=0;
static const int WHITE_STAR=1;
static const int DARK_STAR=2;

// Smart generator convention constants
static const int UNSIGNED=0;
static const int SIGNED=1;

// Smart generator bit slicing strategies
static const int PRESERVE_LSB=0;
static const int PRESERVE_MSB=1;

// Add star constants
static const int DEFAULT=0;

// Star node constants
static const int DATA_NODE=0;
static const int CTRL_NODE=1;
static const int EXT_NODE=2;
static const int CONSTANT_NODE=3;
static const int ACS_NODE=4;

// Bit precision comparisons
static const int NO_OVERLAP=0;
static const int OVERLAP=1;
static const int SRC_LARGER=2;
static const int SNK_LARGER=4;


// Signal type constants
static const int STANDALONE=0;
static const int SHAREABLE=1;

// ACS_Type Constants
static const int SOURCE_LUT=5;
static const int IOPORT=4;
static const int FORK=3;
static const int BOTH=2;        // Designates that this is an algorithmic component
static const int SOURCE=1;
static const int SINK=0;
static const int UNKNOWN=-1;

// ACS_State Constants
static const int SAVE_STATE=1;
static const int NO_SAVE_STATE=0;

// Directivity Constants
static const int BIDIR=3;
static const int OUTPUT=2;
static const int INPUT=1;

// HWfind_port constants
static const int FIRST_AVAIL=1;
static const int USED_PORT=0;

// Host Constants
static const int HOST_INTERFACE=0;  // Designates wether a host interface needs to be generated
static const int HOST_DELAY=1;      // If there is a host interface, what is the pipe delay for outputs

// Node Constants
static const int ASSIGNED=1;
static const int UNASSIGNED=-1;
static const int EXTERNAL=-2;
static const int INTERNAL=-3;


#endif


