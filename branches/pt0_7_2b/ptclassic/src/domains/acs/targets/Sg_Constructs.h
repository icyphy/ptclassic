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
 Version: @(#)Sg_Constructs.h      1.0     06/16/99
***********************************************************************/
#ifndef SG_CONSTRUCTS_H
#define SG_CONSTRUCTS_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "KnownBlock.h"
//#include "SymbolList.h"
#include "CoreList.h"
#include "ACSCGFPGACore.h"
#include "Fpga.h"
#include "Pin.h"
#include "acs_starconsts.h"

class ACSCGFPGACore;
class Fpga;
class CoreList;

class Sg_Constructs
{
  static const int DEBUG_CONSTRUCTS=0;
  static const int DEBUG_BWBALANCE=0;
  static const int DEBUG_SGDECODER=0;
  static const int DEBUG_MATCHPINS=0;

public:
  int valid;
  Fpga* target_fpga;
  int* free_id;
  int* free_netid;
  char* design_directory;
public:
  Sg_Constructs::Sg_Constructs(int*, char*);
  Sg_Constructs::Sg_Constructs(int*, int*, char*);
  Sg_Constructs::Sg_Constructs(int*,
			       char*,
			       Fpga*);
  Sg_Constructs::~Sg_Constructs();

  void Sg_Constructs::validate(void);

  void Sg_Constructs::set_targetdevice(Fpga*);

  ACSCGFPGACore* Sg_Constructs::add_buffer(const int,
					   CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_ioport(const int,
					   CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_mux(const int,
					const int,
					CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_mux(const int,
					const int,
					const int,
					CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_unpacker(const int,
					     CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_packer(const int,
					   CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_const(int,
					  const int,
					  CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_const(long,
					  const int,
					  CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_const(double,
					  const int,
					  CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_adder(const int,
					  CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_subtracter(const int,
					       CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_delay(const int,
					  CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_counter(const int,
					    const int,
					    const int,
					    CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_counter(const int,
					    const int,
					    ACSIntArray*,
					    CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_timer(const int,
					  CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_timer(const int,
					  ACSIntArray*,
					  CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_timer(ACSIntArray*,
					  CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_timer(ACSIntArray*,
					  ACSIntArray*,
					  CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_phaser(const int,
					   CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_phaser(const int,
					   const int,
					   CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_decoder(const int,
					    const int,
					    const int,
					    ACSIntArray*,
					    ACSIntArray**,
					    CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_decoder(const int,
					    const int,
					    ACSIntArray*,
					    ACSIntArray**,
					    CoreList*);


  ACSCGFPGACore* Sg_Constructs::add_sg(char*,
				       int,
				       int,
				       CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_sg(char*,
				       char*,
				       char*,
				       int,
				       int,
				       CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_sg(char*,
				       char*,
				       char*,
				       int,
				       CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_sg(char*, 
				       char*, 
				       char*,
				       int,
				       int,
				       int,
				       CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_sg(char*, 
				       char*, 
				       char*,
				       int,
				       int,
				       int,
				       int,
				       CoreList*);
  ACSCGFPGACore* Sg_Constructs::add_sg_core1(char*,
					     char*,
					     char*,
					     int,
					     int);

  ACSCGFPGACore* Sg_Constructs::add_sg_core2(ACSCGFPGACore*,
					     CoreList*);

  int Sg_Constructs::insert_sg(ACSCGFPGACore*,
			       ACSCGFPGACore*,
			       ACSCGFPGACore*,
			       int,
			       int,
			       const int,
			       int,
			       int,
			       const int);
  int Sg_Constructs::insert_sg(ACSCGFPGACore*,
			       ACSCGFPGACore*,
			       Pin*,
			       int,
			       int,
			       const int,
			       int,
			       int,
			       const int);
  int Sg_Constructs::insert_sg(Pin*,
			       ACSCGFPGACore*,
			       ACSCGFPGACore*,
			       int,
			       int,
			       const int,
			       int,
			       int,
			       const int);
  
  int Sg_Constructs::connect_sg(Pin*,int,
				ACSCGFPGACore*,int,
				int);
  int Sg_Constructs::connect_sg(Pin*,int,int,int,
				ACSCGFPGACore*,int,int,int,
				int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,int,
				Pin*,int,
				int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,int,int,int,
				Pin*,int,int,int,
				int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,
				ACSCGFPGACore*);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,
				const int,
				ACSCGFPGACore*);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,
				ACSCGFPGACore*,
				const int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,int,
				ACSCGFPGACore*,int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,int,
				ACSCGFPGACore*,int,
				const int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,int,int,
				ACSCGFPGACore*,int,int,
				int);
  int Sg_Constructs::connect_sg(CoreList*,
				int*,
				int*,
				ACSCGFPGACore*,
				int,
				int,
				int);
  int Sg_Constructs::connect_sg(ACSCGFPGACore*,
				int,
				int,
				int,
				ACSCGFPGACore*,
				int,
				int,
				int,
				int);
  int Sg_Constructs::connect_sg(CoreList*,
				int*,
				int*,
				int,
				ACSCGFPGACore*,
				int,
				int,
				int,
				int);
  void Sg_Constructs::assign_netlist(Pin*, const int,
				     Pin*, const int);
  void Sg_Constructs::match_pins(Pin*,int,int&,
				 Pin*,int,int&);
  void Sg_Constructs::balance_bw(CoreList*);
  int Sg_Constructs::bit_sizer(const int);
};

#endif
