static const char file_id[] = "ACSCGFPGACore.cc";
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
 Version: $Id$
***********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Attribute.h"
#include "ACSCGFPGACore.h"
#include "ACSPortHole.h"
#include "FixState.h"
#include "FixArrayState.h"
#include "Tokenizer.h"
#include "ComplexState.h"
#include "PrecisionState.h"
#include "CGUtilities.h"
#include <ctype.h>
#include "ACSKnownCategory.h"

// Global category string
extern const char ACSCGFPGACategory[] = "CGFPGA";

// isA
ISA_FUNC(ACSCGFPGACore, ACSCGCore);

// add string to KnownCategory list
static ACSCGFPGACore proto;
static ACSKnownCategory entry(proto);


// JMS
ACSCGFPGACore::ACSCGFPGACore()  : 
  ACSCGCore(ACSCGFPGACategory), 
  acs_id(-1), comment(NULL),
  acs_type(UNKNOWN), acs_domain(UNDEFINED), acs_fpga(1),
  acs_existence(UNDEFINED), acs_speed(-1), sg_language(UNDEFINED),
  acs_hwport(-1),
  child_sgs(NULL),
  dest_dir(NULL), black_box(0), bitslice_strategy(PRESERVE_LSB),
  acs_addract(UNSCHEDULED), acs_dataact(UNSCHEDULED),
  alg_delay(-1), pipe_delay(-1),
  acsdelay_count(0), acsdelay_ids(NULL),
  acsdata_address(-1), acsdata_rsize(-1), acsdata_csize(-1),
  initialized(0), queried(0), bw_dirty(1), bw_exempt(0), bw_loop(0),
  sign_convention(UNSIGNED),
  total_count(0), input_count(0), output_count(0), 
  control_count(0), bidir_count(0),
  sg_constants(NULL),
  root_filename(NULL), cost_filename(NULL), 
  numsim_filename(NULL), rangecalc_filename(NULL), natcon_filename(NULL)
{
  child_filenames=new StringArray;
  pipe_alignment=new Pipealign;
  libs=new StringArray;
  incs=new StringArray;
  pins=new Pin;
  ext_signals=new Pin;
  data_signals=new Pin;
  ctrl_signals=new Pin;
  constant_signals=new Pin;
  resources=new Resource;
  sg_capability=new Capability;
  lang=new VHDL_LANG;
}

ACSCGFPGACore::ACSCGFPGACore(const char* category) : 
  ACSCGCore(category), 
  acs_id(-1), comment(NULL),
  acs_type(UNKNOWN), acs_domain(UNDEFINED), acs_fpga(1),
  acs_existence(UNDEFINED), acs_speed(-1), sg_language(UNDEFINED),
  acs_hwport(-1),
  child_sgs(NULL),
  dest_dir(NULL), black_box(0), bitslice_strategy(PRESERVE_LSB),
  acs_addract(UNSCHEDULED), acs_dataact(UNSCHEDULED), 
  alg_delay(-1), pipe_delay(-1),
  acsdelay_count(0), acsdelay_ids(NULL),
  acsdata_address(-1), acsdata_rsize(-1), acsdata_csize(-1), 
  initialized(0), queried(0), bw_dirty(1), bw_exempt(0), bw_loop(0),
  sign_convention(UNSIGNED),
  total_count(0), input_count(0), output_count(0), 
  control_count(0), bidir_count(0),
  sg_constants(NULL),
  root_filename(NULL), cost_filename(NULL), 
  numsim_filename(NULL), rangecalc_filename(NULL), natcon_filename(NULL)
{
  child_filenames=new StringArray;
  pipe_alignment=new Pipealign;
  libs=new StringArray;
  incs=new StringArray;
  pins=new Pin;
  ext_signals=new Pin;
  data_signals=new Pin;
  ctrl_signals=new Pin;
  constant_signals=new Pin;
  resources=new Resource;
  sg_capability=new Capability;
  lang=new VHDL_LANG;
}


ACSCGFPGACore::ACSCGFPGACore(ACSCorona & corona_) : 
  ACSCGCore(ACSCGFPGACategory),
  acs_id(-1), comment(NULL),
  acs_type(UNKNOWN), acs_domain(UNDEFINED), acs_fpga(1), 
  acs_existence(UNDEFINED), acs_speed(-1), sg_language(UNDEFINED), 
  acs_hwport(-1),
  child_sgs(NULL),
  dest_dir(NULL), black_box(0), bitslice_strategy(PRESERVE_LSB),
  acs_addract(UNSCHEDULED), acs_dataact(UNSCHEDULED), 
  alg_delay(-1), pipe_delay(-1),
  acsdelay_count(0), acsdelay_ids(NULL), 
  acsdata_address(-1), acsdata_rsize(-1), acsdata_csize(-1),
  initialized(0), queried(0), bw_dirty(1), bw_exempt(0), bw_loop(0),
  sign_convention(UNSIGNED),
  total_count(0), input_count(0), output_count(0), 
  control_count(0), bidir_count(0),
  sg_constants(NULL),
  root_filename(NULL), cost_filename(NULL), 
  numsim_filename(NULL), rangecalc_filename(NULL), natcon_filename(NULL)
{
}

ACSCGFPGACore::~ACSCGFPGACore()
{
  acsdelay_count=0;
  delete []comment;
  delete acsdelay_ids;
  delete []dest_dir;
  delete child_filenames;
  delete pipe_alignment;
  delete libs;
  delete incs;
  delete pins;
  delete ext_signals;
  delete data_signals;
  delete ctrl_signals;
  delete constant_signals;
  delete resources;
  delete sg_capability;
  delete lang;
}

// Return a stars identifier else returns its acs name
char* ACSCGFPGACore::comment_name()
{
  if (comment != NULL)
    return(comment);
  else
    {
      ostrstream sg_name;
      sg_name << className() << acs_id << ends;
      return(sg_name.str());
    }
}

// Return a star's acs name
char* ACSCGFPGACore::name()
{
  ostrstream sg_name;
  sg_name << className() << acs_id << ends;
  return(sg_name.str());
}

// Return the star's acs name in lowercase
char* ACSCGFPGACore::lc_name()
{
  ostrstream sg_name, out_name;
  sg_name << className() << acs_id << ends;

  char* tmp_name=new char[MAX_STR];
  strcpy(tmp_name,sg_name.str());
  tolowercase(tmp_name);
  out_name << tmp_name << ends;
  delete []tmp_name;

  return(out_name.str());
}

// Convert the BW into a vector length.
int ACSCGFPGACore::vector_length(double precision) 
{
  int vector_len=0;
  int decimal_len=0;
  double remains;
  
  vector_len=(int) floor(precision);
  remains=precision-(double)vector_len;
 
  while ((remains)!=0.0)
    {
      decimal_len*=10;
      remains*=10;
      decimal_len+=(int) floor(remains);
      remains-=floor(remains);
    }
  vector_len+=decimal_len;
  return(vector_len);
}

// Query an integer Ptolemy parameter from the named state parameter
int ACSCGFPGACore::intparam_query(const char* state_name)
{
  int val=-1;
  State* state=stateWithName(state_name);
  if (state!=NULL)
    {
      IntState* int_state=(IntState *)state;
      val=(int) (*int_state);
    }
  return(val);
}

void ACSCGFPGACore::intparam_set(const char* state_name,int val)
{
  State* state=stateWithName(state_name);
  if (state!=NULL)
    {
      IntState* int_state=(IntState *)state;
      *int_state=val;
    }
}


// Perform all mundane functions here
int ACSCGFPGACore::sg_initialize(void)
{
  sg_setup();
  initialized=1;

  // Return happy condition
  return(1);
}

int ACSCGFPGACore::sg_initialize(char* set_dir,int type,int* free_acsid)
{
  // Setup data structures, once needed
  // FIX:Is this needed?
  child_filenames=new StringArray;
  pipe_alignment=new Pipealign;
  libs=new StringArray;
  incs=new StringArray;
  pins=new Pin;
  ext_signals=new Pin;
  data_signals=new Pin;
  ctrl_signals=new Pin;
  constant_signals=new Pin;
  resources=new Resource;
  sg_capability=new Capability;
  lang=new VHDL_LANG;

  // Setup output directory
  dest_dir=new char[MAX_STR];
  strcpy(dest_dir,set_dir);

  // Setup Port_Ids and Capabilities
  if (sg_language==UNDEFINED)
    {
      printf("ACSCGFPGACore::sg_initialize:Error attempting to initialize ");
      printf("smart generator without having a language specified!\n");
      exit(0);
    }
  else
    {
      // Allocate and set pin information
      sg_setup();

      // If this is a dark star, then let's build it's interior
      if (macro_query()==DARK_STAR)
	{
	  // ASSUMPTION: If this IS a dark star it should 
	  //             return a list of additional smart generators
	  macro_build(free_acsid);

	}

      // Set initial precisions, but dont lock them
      sg_resources(UNLOCKED);
    }
  
  initialized=1;

  // Return happy condition
  return(1);
}

int ACSCGFPGACore::sg_costfunc(int inst_id)
{
  // Setup and generate separate file streams
  root_filename=new char[MAX_STR];
  cost_filename=new char[MAX_STR];
  numsim_filename=new char[MAX_STR];
  rangecalc_filename=new char[MAX_STR];
  natcon_filename=new char[MAX_STR];
  sprintf(root_filename,"%sinst%d_",dest_dir,inst_id);
  sprintf(cost_filename,"%scost.m",root_filename);
  sprintf(numsim_filename,"%snumsim.m",root_filename);
  sprintf(rangecalc_filename,"%srangecalc.m",root_filename);
  sprintf(natcon_filename,"%snatcon.m",root_filename);

  ofstream cost_file(cost_filename);
  ofstream numsim_file(numsim_filename);
  ofstream rangecalc_file(rangecalc_filename);
  ofstream natcon_file(natcon_filename);

  // Matlab preambles
  cost_file << "function cost=inst" << inst_id 
	    << "_cost(insizes,outsizes,msbranges)" << endl;
  numsim_file << "function y=inst" << inst_id
	      << "_numsim(x)" << endl;
  rangecalc_file << "function orr=inst" << inst_id
		 << "_rangecalc(inputrange)" << endl;
  natcon_file << "function yesno=inst" << inst_id
	      << "_natcon(insizes,outsizes,msbranges)" << endl;

  // Add core-specific functions
  sg_cost(cost_file,numsim_file,rangecalc_file,natcon_file);

  // Matlab postambles
  cost_file << "return;" << endl
	    << "% " << name() << endl;
  numsim_file << "return;" << endl
	      << "% " << name() << endl;
  rangecalc_file << "return;" << endl
		 << "% " << name() << endl;
  natcon_file << "return;" << endl
	      << "% " << name() << endl;
  
  // Wrapup
  cost_file.close();
  numsim_file.close();
  rangecalc_file.close();
  natcon_file.close();
  
  // Cleanup
  delete []root_filename;
  delete []cost_filename;
  delete []numsim_filename;
  delete []rangecalc_filename;
  delete []natcon_filename;

  // Return happy condition
  return(1);
}

int ACSCGFPGACore::bits_overlap(const int src_mbit, const int src_len,
				const int snk_mbit, const int snk_len)
{
  int result=NO_OVERLAP;

  int larger_mbit=0;
  int larger_bitlen=0;
  int smaller_mbit=0;
  int smaller_bitlen=0;
  if (src_mbit > snk_mbit)
    {
      larger_mbit=src_mbit;
      larger_bitlen=src_len;
      smaller_mbit=snk_mbit;
      smaller_bitlen=snk_len;
      result=SRC_LARGER;
    }
  else
    {
      larger_mbit=snk_mbit;
      larger_bitlen=snk_len;
      smaller_mbit=src_mbit;
      smaller_bitlen=src_len;
      result=SNK_LARGER;
    }
  if (smaller_mbit > (larger_mbit-larger_bitlen))
    result=result | OVERLAP;
  return(result);
}

char* ACSCGFPGACore::tolowercase(char* capped_string)
{
  for (unsigned int loop=0;loop<strlen(capped_string);loop++)
    capped_string[loop]=tolower(capped_string[loop]);

  return(capped_string);
}


//JR
//Add definitions for virtuals declared in ACSCGCore

StringList ACSCGFPGACore::setargStates(Attribute a)
{
  StringList setarg_proc;
  setarg_proc << "setargStates"; 
  return setarg_proc;
}


StringList ACSCGFPGACore::declareBuffer(const ACSPortHole* port)
{
    StringList dec;     // declarations
    dec << "declareBuffer";
    return dec;
}

StringList ACSCGFPGACore::declareOffset(const ACSPortHole* port)
{
    StringList dec;     // declarations
    dec << "declareOffset";
    return dec;
}

StringList ACSCGFPGACore::declareState(const State* state)
{
    StringList dec;     // declarations
    dec << "declareState";
    return dec;
}

StringList ACSCGFPGACore::cmdargState(const State* state)
{
  StringList struct_mem;
  struct_mem << "cmdargState";
  return struct_mem;
}

StringList ACSCGFPGACore::cmdargStatesInit(const State* state)
{
  StringList struct_init;
  struct_init << "cmdargStatesInit";
  return struct_init;
}

StringList ACSCGFPGACore::setargState(const State* state)
{
  StringList setarg_proc;
  setarg_proc << "setargState";
  return setarg_proc;
}

StringList ACSCGFPGACore::setargStatesHelp(const State* state)
{
  StringList setarg_proc;
  setarg_proc << "setargStatesHelp";
  return setarg_proc;
}

StringList ACSCGFPGACore::initCodeBuffer(ACSPortHole* port)
{
    StringList code;
    code << "initCodeBuffer";
    return code;
}

StringList ACSCGFPGACore::initCodeOffset(const ACSPortHole* port)
{
    StringList code;    // initialization code
    code << "initCodeOffset";
    return code;
}

StringList ACSCGFPGACore::initCodeState(const State* state)
{
    StringList code;
    code << "initCodeState";
    return code;
}

StringList ACSCGFPGACore::portBufferIndex( const ACSPortHole* port )
{
  StringList index;
  index << "portBufferIndex";
  return index;
}

StringList ACSCGFPGACore::portBufferIndex( const ACSPortHole* port, const State* offsetState, const char* offset )
{
  StringList index;
  index << "portBufferIndex";
  return index;
}

StringList ACSCGFPGACore::arrayStateIndexRef(const State* state, const char* offset)
{
	StringList index;

	index << "arrayStateIndexRef";
	return index;
}

