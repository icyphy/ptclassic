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
 Version: @(#)Sequencer.h      1.0     06/16/99
***********************************************************************/
#ifndef SEQUENCER_H
#define SEQUENCER_H

#ifdef __GNUG__
#pragma interface
#endif

//#include "StringList.h"
#include "CoreList.h"
#include "ACSCGFPGACore.h"

// Stave off circular references
class ACSCGFPGACore;
class CoreList;

class Sequencer
{
public:
  ACSCGFPGACore* seq_sg;
  CoreList* src_consts;
  ACSCGFPGACore* src_mux;
  ACSCGFPGACore* src_add;
  CoreList* snk_consts;
  ACSCGFPGACore* snk_mux;
  ACSCGFPGACore* snk_add;
  CoreList* delay_consts;
  ACSCGFPGACore* delay_mux;
  ACSCGFPGACore* delay_add;
  
public:
  Sequencer::Sequencer(void);
  Sequencer::~Sequencer(void);
};

#endif
