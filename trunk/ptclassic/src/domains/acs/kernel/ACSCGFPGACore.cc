static const char file_id[] = "ACSCGFPGACore.cc";
/**********************************************************************
Copyright (c) 1999-2001 Sanders, a Lockheed Martin Company
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
 Version: @(#)ACSCGFPGACore.cc	1.2 09/08/99
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
  acs_id(-1), comment_flag(0), acs_origin(ACS_GENERATED), 
  unique_component(0), declaration_flag(0),
  acs_type(UNKNOWN), acs_domain(UNDEFINED), acs_state(NO_SAVE_STATE),
  acs_device(UNASSIGNED), target_type(XC4000), memory_device(UNASSIGNED),
  acs_existence(UNDEFINED), acs_speed(-1), sg_language(UNDEFINED),
  child_sgs(NULL),
  target_implementation(0), implementation_lock(UNLOCKED),
  dest_dir(NULL), child_filenames(NULL), 
  black_box(0), bitslice_strategy(PRESERVE_LSB),
  act_input(UNSCHEDULED), act_output(UNSCHEDULED), 
  act_launch(UNSCHEDULED), act_complete(UNSCHEDULED), 
  acs_delay(-1), alg_delay(-1), pipe_delay(-1), phase_dependent(0), delay_offset(0),
  pipe_alignment(NULL),
  acsdelay_count(0), acsdelay_ids(NULL),
  address_start(-1), address_step(1), word_count(-1),
  initialized(0), lib_queried(0), macro_queried(0),
  bw_dirty(1), bw_exempt(0), bw_loop(0),
  sign_convention(UNSIGNED), total_count(0), input_count(0), output_count(0), 
  control_count(0), bidir_count(0),
  libs(NULL), incs(NULL), 
  pins(NULL), ext_signals(NULL), data_signals(NULL), 
  ctrl_signals(NULL), constant_signals(NULL),
  resources(NULL), sg_capability(NULL), sg_constants(NULL), lang(NULL),
  root_filename(NULL), cost_filename(NULL), 
  numsim_filename(NULL), rangecalc_filename(NULL), natcon_filename(NULL)
{
}

ACSCGFPGACore::ACSCGFPGACore(const char* category) : 
  ACSCGCore(category), 
  acs_id(-1), comment_flag(0), acs_origin(ACS_GENERATED), 
  unique_component(0), declaration_flag(0),
  acs_type(UNKNOWN), acs_domain(UNDEFINED), acs_state(NO_SAVE_STATE),
  acs_device(UNASSIGNED), target_type(XC4000), memory_device(UNASSIGNED), 
  acs_existence(UNDEFINED), acs_speed(-1), sg_language(UNDEFINED),
  child_sgs(NULL),
  target_implementation(0), implementation_lock(UNLOCKED),
  dest_dir(NULL),  child_filenames(NULL), 
  black_box(0), bitslice_strategy(PRESERVE_LSB),
  act_input(UNSCHEDULED), act_output(UNSCHEDULED), 
  act_launch(UNSCHEDULED), act_complete(UNSCHEDULED), 
  acs_delay(-1), alg_delay(-1), pipe_delay(-1),  phase_dependent(0), delay_offset(0),
  pipe_alignment(NULL),
  acsdelay_count(0), acsdelay_ids(NULL),
  address_start(-1), address_step(1), word_count(-1),
  initialized(0), lib_queried(0),  macro_queried(0),
  bw_dirty(1), bw_exempt(0), bw_loop(0),
  sign_convention(UNSIGNED), total_count(0), input_count(0), output_count(0), 
  control_count(0), bidir_count(0),
  libs(NULL), incs(NULL), 
  pins(NULL), ext_signals(NULL), data_signals(NULL), 
  ctrl_signals(NULL), constant_signals(NULL),
  resources(NULL), sg_capability(NULL), sg_constants(NULL), lang(NULL),
  root_filename(NULL), cost_filename(NULL), 
  numsim_filename(NULL), rangecalc_filename(NULL), natcon_filename(NULL)
{
}


ACSCGFPGACore::ACSCGFPGACore(ACSCorona & corona_) : 
  ACSCGCore(ACSCGFPGACategory),
  acs_id(-1), comment_flag(0), acs_origin(ACS_GENERATED), 
  unique_component(0), declaration_flag(0),
  acs_type(UNKNOWN), acs_domain(UNDEFINED), acs_state(NO_SAVE_STATE), 
  acs_device(UNASSIGNED), target_type(XC4000), memory_device(UNASSIGNED), 
  acs_existence(UNDEFINED), acs_speed(-1), sg_language(UNDEFINED), 
  child_sgs(NULL),
  target_implementation(0), implementation_lock(UNLOCKED),
  dest_dir(NULL),  child_filenames(NULL), 
  black_box(0), bitslice_strategy(PRESERVE_LSB),
  act_input(UNSCHEDULED), act_output(UNSCHEDULED), 
  act_launch(UNSCHEDULED), act_complete(UNSCHEDULED), 
  acs_delay(-1), alg_delay(-1), pipe_delay(-1),  phase_dependent(0), delay_offset(0),
  pipe_alignment(NULL),
  acsdelay_count(0), acsdelay_ids(NULL), 
  address_start(-1), address_step(1), word_count(-1),
  initialized(0), lib_queried(0),  macro_queried(0),
  bw_dirty(1), bw_exempt(0), bw_loop(0),
  sign_convention(UNSIGNED), total_count(0), input_count(0), output_count(0), 
  control_count(0), bidir_count(0),
  libs(NULL), incs(NULL), 
  pins(NULL), ext_signals(NULL), data_signals(NULL), 
  ctrl_signals(NULL), constant_signals(NULL),
  resources(NULL), sg_capability(NULL), sg_constants(NULL), lang(NULL),
  root_filename(NULL), cost_filename(NULL), 
  numsim_filename(NULL), rangecalc_filename(NULL), natcon_filename(NULL)
{
}

ACSCGFPGACore::~ACSCGFPGACore()
{
  acsdelay_count=0;
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
  if (comment_flag)
    return(comment.str());
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
char* ACSCGFPGACore::make_name(const char* root_name, const int param)
{
  ostrstream name;
  name << root_name << param << ends;
  return(name.str());
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
  else
    {
      char* err_message=new char[MAX_STR];
      sprintf(err_message,"Unable to find state %s for sg %s\n",
	      state_name,
	      comment_name());
      Error::error(*this,err_message);
      
      // If it survives:) cleanup
      delete []err_message;
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


///////////////////////////////////////////////////////////////////////////
// Query all the standard smart generator states and assign to core storage
///////////////////////////////////////////////////////////////////////////
int ACSCGFPGACore::sg_param_query(void)
{
  //
  // Determine sign convention
  //
  State* sign_state=stateWithName("Sign_Convention");
  if (sign_state!=NULL)
    {
      char* sign_name=new char[MAX_STR];
      strcpy(sign_name,sign_state->currentValue());
      
      if (DEBUG_STARIO)
	printf("Recovered a sign convention of %s\n",sign_name);

      if ((strcmp(sign_name,"signed")==0) ||
	  (strcmp(sign_name,"SIGNED")==0) ||
	  (strcmp(sign_name,"SIGN")==0) ||
	  (strcmp(sign_name,"sign")==0) ||
	  (strcmp(sign_name,"Signed")==0) ||
	  (strcmp(sign_name,"Sign")==0))
	sign_convention=SIGNED;
      else
	sign_convention=UNSIGNED;

      delete []sign_name;
    }
  else
    {
      if (DEBUG_STARIO)
	printf("sg_param_query:Unable to find state:Sign_Convention for star %s, assuming signed\n",
	       comment_name());
      sign_convention=SIGNED;
    }
  

  //
  // Determine technology specifications
  //
  State* domain_state=stateWithName("Domain");
  if (domain_state!=NULL)
    {
      char* domain_name=new char[MAX_STR];
      strcpy(domain_name,domain_state->currentValue());

      if ((strcmp(domain_name,"HW")==0) ||
	  (strcmp(domain_name,"hw")==0) ||
	  (strcmp(domain_name,"h")==0) ||
	  (strcmp(domain_name,"H")==0))
	acs_domain=HW;
      else if ((strcmp(domain_name,"SW")==0) ||
	  (strcmp(domain_name,"sw")==0) ||
	  (strcmp(domain_name,"s")==0) ||
	  (strcmp(domain_name,"S")==0))
	{
	  acs_domain=SW;

	  // Although in the graph, this star should not be scheduled
	  act_input=0;
	  act_output=0;
	}
      else  // FIX: For now, if unfilled it will be HW
	acs_domain=HW;
      
      if (DEBUG_STARIO)
	printf("Recovered a domain name of %s\n",domain_name);
      delete []domain_name;
    }
  else
    {
      if (DEBUG_STARIO)
	printf("sg_param_query:Unable to find state:Domain for star %s, assuming HW\n",
	       comment_name());
      acs_domain=HW;
    }
  
  
  //
  // Determine where on the hardware this smart generator is linked
  //
  acs_device=intparam_query("Device_Number");
  
  //
  // Determine language specifications
  //
  State* lang_state=stateWithName("Language");
  if (lang_state!=NULL)
    {
      char* lang_name=new char[MAX_STR];
      strcpy(lang_name,lang_state->currentValue());
      
      if (DEBUG_STARIO)
	printf("Recovered a language name of %s\n",lang_name);
      
      if ((strcmp(lang_name,"VHDL")==0) ||
	  (strcmp(lang_name,"vhdl")==0) ||
	  (strcmp(lang_name,"vhd")==0))
	sg_language=VHDL_BEHAVIORAL;
      delete []lang_name;
    }
  else
    {
      if (DEBUG_STARIO)
	printf("sg_param_query:Unable to find state:Language for star %s, assuming VHDL\n",
	       comment_name());
      sg_language=VHDL_BEHAVIORAL;
    }

  //
  // Determine and establish the comment field
  //
  State* comment_state=stateWithName("Comment");
  if (comment_state!=NULL)
    {
      StringList current_comment=comment_state->currentValue();
      const char* comment_str=(const char *)current_comment;
      if (strlen(comment_str)!=0)
	{
	  comment_flag=1;
	  comment << comment_str << ends;
	}
    }

  // Return happy condition
  return(1);
}


////////////////////////////////////////////////////////
// Allow a smart generator to add it's own comment field
// This method overrides any previous contents
////////////////////////////////////////////////////////
int ACSCGFPGACore::add_comment(const char* comment_field)
{
  comment_flag=1;
  comment << comment_field << ends;

  // Return happy condition
  return(1);
}
int ACSCGFPGACore::add_comment(const char* comment_field,
			       const int rank)
{
  comment_flag=1;
  comment << comment_field << " number " << rank << ends;

  // Return happy condition
  return(1);
}
int ACSCGFPGACore::add_comment(const char* comment_field,
			       const char* ref_name)
{
  comment_flag=1;
  comment << comment_field << " in support of " << ref_name << ends;

  // Return happy condition
  return(1);
}
int ACSCGFPGACore::add_comment(const char* ref_name,
			       const char* comment_field,
			       const int rank)
{
  comment_flag=1;
  comment << comment_field << " number " << rank;
  comment << " in support of " << ref_name << ends;

  // Return happy condition
  return(1);
}


int ACSCGFPGACore::update_sg(const int bitwidth_lock, const int design_lock)
{
  // Determine bitwidths
  sg_bitwidths(bitwidth_lock);

  // Determine pipe delay
  sg_delay();

  // Determine optimal design
  sg_designs(design_lock);

  // Return happy condition
  return(1);
}

int ACSCGFPGACore::sg_delay(void)
{
  sg_delays();
  pipe_delay=acs_delay;

  // Return happy condition
  return(1);
}


int ACSCGFPGACore::sg_delay_query(void)
{
  //
  // Determine how the delay of this sg affects the algorithm
  //
  char* debug_name=new char[MAX_STR];
  strcpy(debug_name,className());

  if (DEBUG_STARIO)
    printf("sg_delay_query invoked for sg %s\n",comment_name());

  // If the user added a delay, then it needs to be ignored for pipe alignment
  if (acs_origin==USER_GENERATED)
    {
      // Trap for the varietys of user-generated delays
      if ((strncmp(name(),"ACSRamDelay",11)==0) ||
	  (strncmp(name(),"ACSDelay",8)==0) ||
	  (strncmp(name(),"ACSUnitDelay",12)==0))
	{
	  alg_delay=acs_delay;

	  // FIX:This was only for the RamDelay block to work!!
	  pipe_delay=1;
	}
      else
	{
	  alg_delay=0;
	  pipe_delay=acs_delay;
	}
    }
  else
    {
      alg_delay=0;
      pipe_delay=acs_delay;
    }
  
  // Return happy condition
  return(1);
}

int ACSCGFPGACore::sg_io_query(void)
{
  char* err_message=new char[MAX_STR];
  
  // Trap for smart generators that didn't set their types correctly
  if (acs_type==UNKNOWN)
    {
      sprintf(err_message,"HWstar_queryio:sg %s didn't set its type\n",
	      comment_name());
      Error::error(*this,err_message);
      return(0);
    }
  
  if ((acs_type==SOURCE) || (acs_type==SINK))
    {
      // Determine starting address address
      address_start=intparam_query("address_start");
      
      // Determine address step size
      address_step=intparam_query("address_step");

      // Determine the total number of words sourced or sinked on this node
      word_count=intparam_query("word_count");
    }

  // If it survives:) cleanup
  delete []err_message;
  
  // Return happy condition
  return(1);
}


// Perform all mundane functions here
int ACSCGFPGACore::sg_initialize(void)
{
  sg_setup();
  initialized=1;

  // Return happy condition
  return(1);
}

int ACSCGFPGACore::sg_initialize(char* set_dir,int* free_acsid)
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

  // Query user io
  if (acs_origin==USER_GENERATED)
    sg_param_query();
  
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

      // Determine if they are io stars, if so, determine their needs
      int rc_io_query=sg_io_query();
      if (rc_io_query==0)
	return(0);

      // FIX:For now assuming algorithmic stars will follow a
      //     preserve MSB bitslicing strategy. Star parameter?
      if (acs_type==BOTH)
	bitslice_strategy=PRESERVE_MSB;
      else
	bitslice_strategy=PRESERVE_LSB;
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
  schedule_filename=new char[MAX_STR];
  sprintf(root_filename,"%sinst%d_",dest_dir,inst_id);
  sprintf(cost_filename,"%scost.m",root_filename);
  sprintf(numsim_filename,"%snumsim.m",root_filename);
  sprintf(rangecalc_filename,"%srangecalc.m",root_filename);
  sprintf(natcon_filename,"%snatcon.m",root_filename);
  sprintf(schedule_filename,"%sschedgen.m",root_filename);

  ofstream cost_file(cost_filename);
  ofstream numsim_file(numsim_filename);
  ofstream rangecalc_file(rangecalc_filename);
  ofstream natcon_file(natcon_filename);
  ofstream schedule_file(schedule_filename);

  // Matlab preambles
  cost_file << "function cost=inst" << inst_id 
	    << "_cost(insizes,outsizes,msbranges,numforms)" << endl;
  numsim_file << "function y=inst" << inst_id
	      << "_numsim(x)" << endl;
  rangecalc_file << "function orr=inst" << inst_id
		 << "_rangecalc(inputrange)" << endl;
  natcon_file << "function yesno=inst" << inst_id
	      << "_natcon(insizes,outsizes,msbranges)" << endl;
  schedule_file << "function [racts,minlr]=inst" << inst_id
		<< "_schedgen(insizes,outsizes,msbranges,numforms,veclengs)"
		<< endl;

  // Add core-specific functions
  sg_cost(cost_file,numsim_file,rangecalc_file,natcon_file,schedule_file);

  // Matlab postambles
  cost_file << "return;" << endl
	    << "% " << comment_name() << endl;
  numsim_file << "return;" << endl
	      << "% " << comment_name() << endl;
  rangecalc_file << "return;" << endl
		 << "% " << comment_name() << endl;
  natcon_file << "return;" << endl
	      << "% " << comment_name() << endl;
  schedule_file << "return;" << endl
		<< "% " << comment_name() << endl;
  
  // Wrapup
  cost_file.close();
  numsim_file.close();
  rangecalc_file.close();
  natcon_file.close();
  schedule_file.close();
  
  // Cleanup
  delete []root_filename;
  delete []cost_filename;
  delete []numsim_filename;
  delete []rangecalc_filename;
  delete []natcon_filename;
  delete []schedule_filename;

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

int ACSCGFPGACore::fpga_type(void)
{
  return(target_type);
}


///////////////////////////////////////////////////
// Return a handle to a given core's pins
// NOTE:Retreiver should know what they are doing;)
///////////////////////////////////////////////////
Pin* ACSCGFPGACore::get_pin_handle(void)
{
  return(pins);
}

///////////////////////////////////////////////////////////////////////
// Create a duplicate pin class instance for smart generator developers
// to create an internal wrapper 
// NOTE: Having the pin constructor here should help to alleviate the
//       issues observed with dynamic pin generation at the start of
//       pigiRpc when no graphs are defined!  
///////////////////////////////////////////////////////////////////////
Pin* ACSCGFPGACore::dup_pins(void)
{
  Pin* new_pin=new Pin;
  *new_pin=*pins;
  return(new_pin);
}

///////////////////////////////////////////////////////////////////////////
// Modify batches of pin types given in the pin list.  Pin list should
// be parsed in pairs, first number is the pin_no, the second entry will be
// the requested pin type for that pin
///////////////////////////////////////////////////////////////////////////
int ACSCGFPGACore::replace_pintype(const int pin,
				   const int new_type)
{
  pins->set_pintype(pin,new_type);

  // Return happy condition
  return(1);
}
int ACSCGFPGACore::replace_pintypes(ACSIntArray* pin_list)
{
  for (int loop=0;loop<pin_list->population();loop+=2)
    {
      int pin_no=pin_list->query(loop);
      int pin_type=pin_list->query(loop+1);
      pins->set_pintype(pin_no,pin_type);
    }

  // Return happy condition
  return(1);
}

////////////////////////////////////////////////////
// Convert a pin from a given type to a desired type
// Return pin_no if successfull, otherwise -1
////////////////////////////////////////////////////
int ACSCGFPGACore::alter_pintype(const int old_type, 
				 const int new_type)
{
  int old_pin=pins->retrieve_type(old_type);
  if (old_pin >= 0)
    pins->set_pintype(old_pin,new_type);
  return(old_pin);
}

/////////////////////////////////////////////////////////
// Core accessor for finding a pin of a given 'hard' type
/////////////////////////////////////////////////////////
int ACSCGFPGACore::find_hardpin(const int pin_type)
{
  return(pins->retrieve_type(pin_type));
}
int ACSCGFPGACore::find_hardpin(const int pin_type,
				const int priority)
{
  return(pins->retrieve_type(pin_type,priority));
}

///////////////////////////////////////////////////////
// Core accessor for modifying a given pin's precisions
///////////////////////////////////////////////////////
int ACSCGFPGACore::set_precision(const int pin_no,
				 const int major_bit,
				 const int bitlength,
				 const int lock_state)
{
  pins->set_precision(pin_no,major_bit,bitlength,lock_state);
  
  // Return happy condition...at least from this level;)
  return(1);
}

//////////////////////////////////////////////////////
// Core accessor for modifying a given's pins priority
//////////////////////////////////////////////////////
int ACSCGFPGACore::set_pinpriority(const int pin_no,
				   const int priority)
{
  pins->set_pinpriority(pin_no,priority);
  
  // Return happy condition...at least from this level;)
  return(1);
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

