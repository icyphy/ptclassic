static const char file_id[] = "Sequencer.cc";

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
 Version: @(#)Sequencer.cc      1.0     06/16/99
***********************************************************************/
#include "Sequencer.h"

#ifdef __GNUG__
#pragma implementation
#endif


Sequencer::Sequencer(void)
{
  seq_sg=NULL;
  src_consts=new CoreList;
  src_mux=NULL;
  src_add=NULL;
  snk_consts=new CoreList;
  snk_mux=NULL;
  snk_add=NULL;
  delay_consts=new CoreList;
  delay_mux=NULL;
  delay_add=NULL;
}

Sequencer::~Sequencer(void)
{
//  delete seq_sg;
  delete src_consts;
//  delete wc_mux;
//  delete wc_add;
  delete snk_consts;
//  delete snk_mux;
//  delete snk_add;
  delete delay_consts;
//  delete delay_mux;
//  delete delay_add;
}




