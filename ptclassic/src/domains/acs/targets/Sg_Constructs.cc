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
 Version: @(#)Sg_Constructs.cc      1.0     06/16/99
***********************************************************************/
#include "Sg_Constructs.h"

Sg_Constructs::Sg_Constructs(int* id_ptr,char* design_dir)
: valid(1), target_fpga(NULL), free_netid(NULL)
{
  free_id=id_ptr;
  design_directory=new char[MAX_STR];
  strcpy(design_directory,design_dir);
}
Sg_Constructs::Sg_Constructs(int* id_ptr,int* net_ptr,char* design_dir)
: valid(1), target_fpga(NULL)
{
  free_id=id_ptr;
  free_netid=net_ptr;
  design_directory=new char[MAX_STR];
  strcpy(design_directory,design_dir);
}
Sg_Constructs::Sg_Constructs(int* id_ptr,
			     char* design_dir, 
			     Fpga* fpga_device)
: valid(1), free_netid(NULL)
{
  target_fpga=fpga_device;
  free_id=id_ptr;
  design_directory=new char[MAX_STR];
  strcpy(design_directory,design_dir);
}

Sg_Constructs::~Sg_Constructs()
{
  valid=0;
  target_fpga=NULL;
  free_id=NULL;
  delete []design_directory;
}

///////////////////////////////////////////////
// Ensure that the constructor has been invoked
// ASSUMPTION:Will this work?:)
///////////////////////////////////////////////
void Sg_Constructs::validate(void)
{
  if (valid==0)
    printf("ERROR:Use of invalid Sg_Constructs\n");
}


/////////////////////////////////////////////////////////////////////////
// Designates which device will contain any generated smart generators be 
// targetted towards
/////////////////////////////////////////////////////////////////////////
void Sg_Constructs::set_targetdevice(Fpga* fpga_device)
{
  validate();

  target_fpga=fpga_device;
}

///////////////////////////////////////////////////////////////////////////////
// Methods for adding specialized smart generators
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Add a buffer smart generator and assign it to the sg_list
////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_buffer(const int sign_convention,
					 CoreList* sg_list)
{
  ACSCGFPGACore* buf_core=add_sg("ACS","Buffer","CGFPGA",
				 BOTH,sign_convention,sg_list);

  // Return happy condition
  return(buf_core);
}

//////////////////////////////////////////////////////////////
// Add an io_port smart generator and assign it to the sg_list
// FIX: Need to closely align with the Architecture!
//////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_ioport(const int sign_convention,
					 CoreList* sg_list)
{
  ACSCGFPGACore* io_core=add_sg("ACS","IoPort","CGFPGA",
				IOPORT,sign_convention,sg_list);

  // Return happy condition
  return(io_core);
}



////////////////////////////////////////////////////////////////////////
// Add a mux smart generator and assign it to the sg_list.  The slines 
// parameter determines the number of switched lines that the mux should 
// conform to.  If this is to be a mux/demux, then the dir will specify
// the number of SOURCES that will be needed for the individual lines.
////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_mux(const int slines,
				      const int dir,
				      const int sign_convention,
				      CoreList* sg_list)
{
  if (DEBUG_CONSTRUCTS)
    printf("Adding a %d to 1 mux, of which %d are inputs and %d are outputs\n",
	   slines,
	   dir,
	   slines-dir);

  ACSCGFPGACore* mux_core=add_sg("ACS","Mux","CGFPGA",
				 BOTH,sign_convention,
				 slines,dir,sg_list);

  // Return happy condition
  return(mux_core);
}
ACSCGFPGACore* Sg_Constructs::add_mux(const int slines,
				      const int sign_convention,
				      CoreList* sg_list)
{
  ACSCGFPGACore* mux_core=add_sg("ACS","Mux","CGFPGA",
				 BOTH,sign_convention,
				 slines,sg_list);

  // Return happy condition
  return(mux_core);
}

////////////////////////////////////////////////////////////////////////
// Add a unpacking smart generator and assign it to the sg_list. 
// The output_pins paramter determines the number of outputs that are 
// sliced from the input.
////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_unpacker(const int output_pins,
					   CoreList* sg_list)
{
  if (DEBUG_CONSTRUCTS)
    printf("Adding a %d unpacker\n",output_pins);

  ACSCGFPGACore* unpacker_core=add_sg("ACS","UnPackBits","CGFPGA",
				      BOTH,UNSIGNED,
				      output_pins,0,sg_list);

  // Return happy condition
  return(unpacker_core);
}
////////////////////////////////////////////////////////////////////////
// Add a packing smart generator and assign it to the sg_list. 
// The input_pins paramter determines the number of inputs that are 
// packed into a single vector.
////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_packer(const int input_pins,
					 CoreList* sg_list)
{
  if (DEBUG_CONSTRUCTS)
    printf("Adding a %d packer\n",input_pins);

  ACSCGFPGACore* packer_core=add_sg("ACS","PackBits","CGFPGA",
				    BOTH,UNSIGNED,
				    input_pins,sg_list);

  // Return happy condition
  return(packer_core);
}

//////////////////////////////////////////////////////////////////////////////
// Add a constant smart generator and assign it to the sg_list.  Additionally,
// take the constant parameter and assign it to a new Constants instance for
// this newly generated smart generator instance.
//////////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_const(int constant,
					const int sign_convention,
					CoreList* sg_list)
{
  ACSCGFPGACore* const_core=add_sg("ACS","Const","CGFPGA",
				   BOTH,sign_convention,sg_list);
  const_core->sg_constants=new Constants(sign_convention);
  (const_core->sg_constants)->add(&constant,CINT);

  // Return happy condition
  return(const_core);
}
ACSCGFPGACore* Sg_Constructs::add_const(long constant,
					const int sign_convention,
					CoreList* sg_list)
{
  ACSCGFPGACore* const_core=add_sg("ACS","Const","CGFPGA",
				   BOTH,sign_convention,sg_list);
  const_core->sg_constants=new Constants;
  (const_core->sg_constants)->add(&constant,CLONG);

  // Return happy condition
  return(const_core);
}
ACSCGFPGACore* Sg_Constructs::add_const(double constant,
					const int sign_convention,
					CoreList* sg_list)
{
  ACSCGFPGACore* const_core=add_sg("ACS","Const","CGFPGA",
				   BOTH,sign_convention,sg_list);
  const_core->sg_constants=new Constants;
  (const_core->sg_constants)->add(&constant,CDOUBLE);

  // Return happy condition
  return(const_core);
}


///////////////////////////////////////////////////////////////
// Create an adder smart generator and assign it to the sg_list
///////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_adder(const int sign_convention,
					CoreList* sg_list)
{
  ACSCGFPGACore* adder_core=add_sg("ACS","Add","CGFPGA",
				   BOTH,sign_convention,sg_list);

  // Return happy condition
  return(adder_core);
}


///////////////////////////////////////////////////////////////////
// Create a subtractor smart generator and assign it to the sg_list
///////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_subtracter(const int sign_convention,
					CoreList* sg_list)
{
  ACSCGFPGACore* subtracter_core=add_sg("ACS","Sub","CGFPGA",
					BOTH,sign_convention,sg_list);

  // Return happy condition
  return(subtracter_core);
}


/////////////////////////////////////////////
// Add a delay smart generator to the sg_list
/////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_delay(const int delay_amount,
					CoreList* sg_list)
{
  ACSCGFPGACore* delay_core=add_sg("ACS","Delay","CGFPGA",
				   BOTH,SIGNED,sg_list);
  delay_core->acs_delay=delay_amount;
  delay_core->pipe_delay=delay_amount;

  // Return happy condition
  return(delay_core);
}


/////////////////////////////////////////////////////////////////////////////
// Add a counter that will count from start to an end, with a specific stride
/////////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_counter(const int count_start,
					  const int count_stride,
					  const int count_end,
					  CoreList* sg_list)
{
  ACSCGFPGACore* counter_core=add_sg_core1("ACS","AbsCounter","CGFPGA",BOTH,UNSIGNED);

  // Transfer privaledge data
  counter_core->sg_add_privaledge(count_start);
  counter_core->sg_add_privaledge(count_end);
  counter_core->sg_add_privaledge(count_stride);

  counter_core=add_sg_core2(counter_core,sg_list);

  // Return happy condition
  return(counter_core);
}

////////////////////////////////////////////////////////////////////////////////////////
// Add a variable base counter that will count from start to an end, with a fixed stride
////////////////////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_counter(const int count_end,
					  const int repeater,
					  ACSIntArray* base_addresses,
					  CoreList* sg_list)
{
  ACSCGFPGACore* counter_core=add_sg_core1("ACS","AddressCounter","CGFPGA",BOTH,UNSIGNED);

  // Transfer privaledge data
  counter_core->sg_add_privaledge(count_end);
  counter_core->sg_add_privaledge(repeater);
  for (int loop=0;loop<base_addresses->population();loop++)
    counter_core->sg_add_privaledge(base_addresses->query(loop));

  counter_core=add_sg_core2(counter_core,sg_list);

  // Return happy condition
  return(counter_core);
}

/////////////////////////////////////////////////////////////////////////////////////
// Add a counter that provides control signals for all instances in timing info array
/////////////////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_timer(const int duration,
					CoreList* sg_list)
{
  ACSCGFPGACore* counter_core=add_sg_core1("ACS","Counter","CGFPGA",BOTH,UNSIGNED);

  // Transfer privaledge data
  counter_core->sg_add_privaledge(duration);

  counter_core=add_sg_core2(counter_core,sg_list);

  // Return happy condition
  return(counter_core);
}
ACSCGFPGACore* Sg_Constructs::add_timer(const int duration,
					ACSIntArray* mod_info,
					CoreList* sg_list)
{
  ACSCGFPGACore* counter_core=add_sg_core1("ACS","Counter","CGFPGA",BOTH,UNSIGNED);

  // Transfer privaledge data
  counter_core->sg_add_privaledge(0,duration);
  for (int loop=0;loop<mod_info->population();loop++)
    counter_core->sg_add_privaledge(1,mod_info->query(loop));

  counter_core=add_sg_core2(counter_core,sg_list);

  // Return happy condition
  return(counter_core);
}
ACSCGFPGACore* Sg_Constructs::add_timer(ACSIntArray* timing_info,
					CoreList* sg_list)
{
  ACSCGFPGACore* counter_core=add_sg_core1("ACS","Counter","CGFPGA",BOTH,UNSIGNED);

  // Transfer privaledge data
  for (int loop=0;loop<timing_info->population();loop++)
    counter_core->sg_add_privaledge(timing_info->query(loop));

  counter_core=add_sg_core2(counter_core,sg_list);

  // Return happy condition
  return(counter_core);
}
ACSCGFPGACore* Sg_Constructs::add_timer(ACSIntArray* timing_info,
					ACSIntArray* mod_info,
					CoreList* sg_list)
{
  ACSCGFPGACore* counter_core=add_sg_core1("ACS","Counter","CGFPGA",BOTH,UNSIGNED);

  // Transfer privaledge data
  for (int loop=0;loop<timing_info->population();loop++)
    counter_core->sg_add_privaledge(0,timing_info->query(loop));
  for (int loop=0;loop<mod_info->population();loop++)
    counter_core->sg_add_privaledge(1,mod_info->query(loop));

  counter_core=add_sg_core2(counter_core,sg_list);

  // Return happy condition
  return(counter_core);
}


//////////////////////////////////////////////////////////////////////////
// Add a phaser that will divide a clock signal as given by the phase rate
//////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_phaser(const int phase_rate,
					 CoreList* sg_list)
{
  ACSCGFPGACore* phase_core=add_sg_core1("ACS","Phaser","CGFPGA",BOTH,UNSIGNED);
  phase_core->sg_add_privaledge(phase_rate);
  phase_core=add_sg_core2(phase_core,sg_list);

  // Return happy condition
  return(phase_core);
}
///////////////////////////////////////////////////////////////////
// Add a phaser that will may repeat and output specific phase info
///////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_phaser(const int phase_rate,
					 const int repeat,
					 CoreList* sg_list)
{
  ACSCGFPGACore* phase_core=add_sg_core1("ACS","AbsPhaser","CGFPGA",BOTH,UNSIGNED);
  phase_core->sg_add_privaledge(phase_rate);
  phase_core->sg_add_privaledge(repeat);
  phase_core=add_sg_core2(phase_core,sg_list);

  // Return happy condition
  return(phase_core);
}

//////////////////////////////////////
// Methods for adding smart generators
//////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_sg(char* corona,
				     int type,
				     int sign_convention,
				     CoreList* sg_list)
{
  ACSCGFPGACore* gend_core=add_sg_core1("ACS",corona,"CGFPGA",type,sign_convention);
  
  gend_core=add_sg_core2(gend_core,sg_list);

  // Return happy(?) condition
  return(gend_core);
}
ACSCGFPGACore* Sg_Constructs::add_sg(char* domain,
				     char* corona,
				     char* core,
				     int type,
				     int sign_convention,
				     CoreList* sg_list)
{
  ACSCGFPGACore* gend_core=add_sg_core1(domain,corona,core,type,sign_convention);
  
  gend_core=add_sg_core2(gend_core,sg_list);

  // Return happy(?) condition
  return(gend_core);
}


///////////////////////////////////////////////////////////////////////////
// Add a n to m decoder, where n is determined by the extent of the decoder
// info array
///////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_decoder(const int input_size,
					  const int output_size,
					  const int output_activation_type,
					  ACSIntArray* phase_info,
					  ACSIntArray** decoder_info,
					  CoreList* sg_list)
{
  ACSCGFPGACore* decoder_core=add_sg_core1("ACS","Decoder","CGFPGA",BOTH,UNSIGNED);

  // Transfer privaledge data
  decoder_core->total_count=input_size+output_size+1;
  decoder_core->input_count=input_size;

  // Transfer phase information
  for (int loop=0;loop<phase_info->population();loop++)
    decoder_core->sg_add_privaledge(0,phase_info->query(loop));

  // Transfer output activation info
  for (int oloop=0;oloop<output_size;oloop++)
    decoder_core->sg_add_privaledge(1,output_activation_type);

  // Transfer output SOP equations
  for (int oloop=0;oloop<output_size;oloop++)
    for (int iloop=0;iloop<decoder_info[oloop]->population();iloop++)
      decoder_core->sg_add_privaledge(oloop+2,decoder_info[oloop]->query(iloop));
  
  decoder_core=add_sg_core2(decoder_core,sg_list);

  // Return happy condition
  return(decoder_core);
}
ACSCGFPGACore* Sg_Constructs::add_decoder(const int input_size,
					  const int output_size,
					  ACSIntArray* phase_info,
					  ACSIntArray** decoder_info,
					  CoreList* sg_list)
{
  ACSCGFPGACore* decoder_core=add_sg_core1("ACS","Decoder","CGFPGA",BOTH,UNSIGNED);

  // Transfer privaledge data
  decoder_core->total_count=input_size+output_size+1;
  decoder_core->input_count=input_size;
  if (DEBUG_SGDECODER)
    phase_info->print("phase_info");
  for (int loop=0;loop<phase_info->population();loop++)
    decoder_core->sg_add_privaledge(0,phase_info->query(loop));

  // Transfer output activation info
  for (int oloop=0;oloop<output_size;oloop++)
    decoder_core->sg_add_privaledge(1,AH);

  // Transfer output SOP equations
  for (int oloop=0;oloop<output_size;oloop++)
    {
      if (DEBUG_SGDECODER)
	decoder_info[oloop]->print("sg_constructs:add_decoder");
      for (int iloop=0;iloop<decoder_info[oloop]->population();iloop++)
	decoder_core->sg_add_privaledge(oloop+2,decoder_info[oloop]->query(iloop));
    }
  
  decoder_core=add_sg_core2(decoder_core,sg_list);

  // Return happy condition
  return(decoder_core);
}


/////////////////////////////////////////////////////////////////////////
// This version is used for dark stars as parameter initialize is handled
// later after the dark star has set all the necessary parameters
/////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_sg(char* domain,
				     char* corona,
				     char* core,
				     int sign_convention,
				     CoreList* sg_list)
{
  ACSCGFPGACore* gend_core=add_sg_core1(domain,corona,core,BOTH,sign_convention);
  
  // Add this new core/corona to the list of smart generators
  sg_list->append(gend_core);

  // Initialze the new core
  gend_core->sg_initialize(design_directory,free_id);
  gend_core->update_sg(UNLOCKED,UNLOCKED);


  // Return happy(?) condition
  return(gend_core);
}


/////////////////////////////////////////////////////////////////////
// This version dictates the distinct number of inputs and directions
// ASSUMPTION: The sg_list is ordered with all sources first!
// WARNING: A smart generator could override type direction!
/////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_sg(char* domain, 
				     char* corona, 
				     char* core,
				     int type,
				     int sign_convention,
				     int inputs,
				     CoreList* sg_list)
{
  ACSCGFPGACore* gend_core=add_sg_core1(domain,corona,core,type,sign_convention);
  
  if ((gend_core != NULL) && (inputs!=DEFAULT))
    {
      gend_core->total_count=inputs;
      gend_core->input_count=inputs;
    }

  gend_core=add_sg_core2(gend_core,sg_list);
  
  // Return happy condition
  return(gend_core);
}
ACSCGFPGACore* Sg_Constructs::add_sg(char* domain, 
				     char* corona, 
				     char* core,
				     int type,
				     int sign_convention,
				     int inputs,
				     int srcs,
				     CoreList* sg_list)
{
  validate();

  ACSCGFPGACore* gend_core=add_sg_core1(domain,corona,core,type,sign_convention);
  
  if ((gend_core != NULL) && (inputs!=DEFAULT))
    {
      gend_core->total_count=inputs;
      gend_core->input_count=srcs;
    }

  gend_core=add_sg_core2(gend_core,sg_list);
  
  // Return happy condition
  return(gend_core);
}


///////////////////////////////////////////////
// Primitives for adding a smart generator core
///////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_sg_core1(char* domain,
					   char* corona,
					   char* core,
					   int type,
					   int sign_convention)
{
  ACSCorona* acs_corona=(ACSCorona*) KnownBlock::find(corona,domain);
  char* star_name=new char[MAX_STR];
  sprintf(star_name,"%s%s%s%d",domain,corona,core,*free_id);
  if (DEBUG_CONSTRUCTS)
    printf("Adding star:%s\n",star_name);

  if (acs_corona!=NULL)
    {
      // Build a new smart generator
      Block* new_ref=acs_corona->makeNew();
      ACSCorona* new_corona=(ACSCorona*) new_ref;
      new_corona->setCore(core);
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) (new_corona->currentCore);
      if (fpga_core != NULL)
	{
	  fpga_core->acs_id=*free_id;
	  fpga_core->acs_type=type;
	  fpga_core->sign_convention=sign_convention;
	  fpga_core->acs_existence=SOFT;
	  fpga_core->sg_language=VHDL_BEHAVIORAL;

	  // White/Dark stars would not know of fpga assignments
	  // NOTE:The target must ensure that this information is set!
	  if (target_fpga!=NULL)
	    {
	      fpga_core->acs_device=target_fpga->retrieve_acsdevice();
	      fpga_core->target_type=target_fpga->part_type;
	      if (fpga_core->acs_device==-1)
		{
		  fprintf(stderr,"Sg_Constructs::add_sg_core1:Error, null Fpga device targetted\n");
		  abort();
		}
	      if (DEBUG_CONSTRUCTS)
		printf("assigning to device %d\n",fpga_core->acs_device);

	      // Notify the fpga of the new core
	      target_fpga->set_child(fpga_core);
	    }

	  // Return cleanly
	  (*free_id)++;
	  return(fpga_core);
	}
      else
	printf("Unable to reference core\n");

      printf("free_id=%d\n",*free_id);

    }

  // Return error
  printf("Sg_Constructs::Error:Unable to add smart generator %s%s%s\n",domain,corona,core);

  // Cleanup
  delete []star_name;

  // Return unhappy condition
  return(NULL);
}

ACSCGFPGACore* Sg_Constructs::add_sg_core2(ACSCGFPGACore* fpga_core,
					   CoreList* sg_list)
{
  if (fpga_core != NULL)
    {
      // Populate Port_Ids 
      fpga_core->sg_initialize(design_directory,free_id);

      fpga_core->update_sg(UNLOCKED,UNLOCKED);

      // Since this would be a non-algorithmic smart generator,
      // it's delay will be of the pipe alignment variety
      fpga_core->pipe_delay=fpga_core->acs_delay;

      // Add this new core/corona to the list of smart generators
      sg_list->append(fpga_core);

      return(fpga_core);
    }

  // Return error
  if (DEBUG_CONSTRUCTS)
    printf("Unable to add smart generator %s to the smart generator list\n",
	   fpga_core->name());

  // Return unhappy condition
  return(NULL);
}

int Sg_Constructs::insert_sg(ACSCGFPGACore* src_core,
			     ACSCGFPGACore* tween_core,
			     ACSCGFPGACore* dest_core,
			     int src_pin,
			     int src_node,
			     const int src_type,
			     int dest_pin,
			     int dest_node,
			     const int dest_type)
{
  Pin* src_pins=src_core->pins;
  Pin* dest_pins=dest_core->pins;
  if (src_pin==UNKNOWN)
    src_pin=src_pins->match_acstype(dest_core->acs_id,OUTPUT_PIN);
  if (src_node==UNKNOWN)
    src_node=src_pins->match_acsnode(dest_core->acs_id,src_pin);
  if (dest_pin==UNKNOWN)
    dest_pin=dest_pins->match_acstype(src_core->acs_id,INPUT_PIN);
  if (dest_node==UNKNOWN)
    dest_node=dest_pins->match_acsnode(src_core->acs_id,dest_pin);

  // Carry over the precisions
  int src_blen=src_pins->query_bitlen(src_pin);
  int src_mbit=src_pins->query_majorbit(src_pin);
  int dest_blen=dest_pins->query_bitlen(dest_pin);
  int dest_mbit=dest_pins->query_majorbit(dest_pin);
  
  connect_sg(src_core,
	     src_pin,
	     src_node,
	     tween_core,
	     UNASSIGNED,
	     UNASSIGNED,
	     src_type);

  connect_sg(tween_core,
	     UNASSIGNED,
	     UNASSIGNED,
	     dest_core,
	     dest_pin,
	     dest_node,
	     dest_type);

  // Identify and assign precisions to the tween core
  Pin* tween_pins=tween_core->pins;
  int tween_inpin=tween_pins->match_acstype(src_core->acs_id,INPUT_PIN);
  int tween_outpin=tween_pins->match_acstype(dest_core->acs_id,OUTPUT_PIN);
  tween_pins->set_precision(tween_inpin,src_mbit,src_blen,LOCKED);
  tween_pins->set_precision(tween_outpin,dest_mbit,dest_blen,LOCKED);

  // Return happy condition
  return(1);
}
int Sg_Constructs::insert_sg(ACSCGFPGACore* src_core,
			     ACSCGFPGACore* tween_core,
			     Pin* dest_pins,
			     int src_pin,
			     int src_node,
			     const int src_type,
			     int dest_pin,
			     int dest_node,
			     const int dest_type)
{
  if (dest_pin==UNKNOWN)
    dest_pin=dest_pins->match_acstype(src_core->acs_id,INPUT_PIN);
  if (dest_node==UNKNOWN)
    dest_node=dest_pins->match_acsnode(src_core->acs_id,dest_pin);

  // Carry over the precisions
  Pin* src_pins=src_core->pins;
  int src_blen=src_pins->query_bitlen(src_pin);
  int src_mbit=src_pins->query_majorbit(src_pin);

  connect_sg(src_core,
	     src_pin,
	     src_node,
	     tween_core,
	     UNASSIGNED,
	     UNASSIGNED,
	     src_type);

  connect_sg(tween_core,
	     UNASSIGNED,
	     UNASSIGNED,
	     UNASSIGNED,
	     dest_pins,
	     dest_pin,
	     dest_node,
	     dest_pin,
	     dest_type);
  
  // Identify and assign precisions to the tween core
  Pin* tween_pins=tween_core->pins;
  int tween_inpin=tween_pins->match_acstype(src_core->acs_id,INPUT_PIN);
  tween_pins->set_precision(tween_inpin,src_mbit,src_blen,LOCKED);

  // Return happy condition
  return(1);
}
int Sg_Constructs::insert_sg(Pin* src_pins,
			     ACSCGFPGACore* tween_core,
			     ACSCGFPGACore* dest_core,
			     int src_pin,
			     int src_node,
			     const int src_type,
			     int dest_pin,
			     int dest_node,
			     const int dest_type)
{
  // Carry over the precisions
  Pin* dest_pins=dest_core->pins;
  int dest_blen=dest_pins->query_bitlen(dest_pin);
  int dest_mbit=dest_pins->query_majorbit(dest_pin);
  
  connect_sg(src_pins,
	     src_pin,
	     src_node,
	     src_pin,
	     tween_core,
	     UNASSIGNED,
	     UNASSIGNED,
	     UNASSIGNED,
	     src_type);
  
  connect_sg(tween_core,
	     UNASSIGNED,
	     UNASSIGNED,
	     dest_core,
	     dest_pin,
	     dest_node,
	     DATA_NODE);

  // Identify and assign precisions to the tween core
  Pin* tween_pins=tween_core->pins;
  int tween_outpin=tween_pins->match_acstype(dest_core->acs_id,OUTPUT_PIN);
  tween_pins->set_precision(tween_outpin,dest_mbit,dest_blen,LOCKED);

  // Return happy condition
  return(1);
}





///////////////////////////////////////////////////////////////////////////////
// Methods for connecting smart generators
///////////////////////////////////////////////////////////////////////////////
int Sg_Constructs::connect_sg(Pin* src_pins,
			      int src_signal,
			      ACSCGFPGACore* dest_sg,
			      int dest_type,
			      int node_type)
{
  Pin* dest_pins=dest_sg->pins;
  int dest_pin=dest_pins->free_pintype(dest_type,src_pins->query_pinpriority(src_signal));

  src_pins->connect_pin(src_signal,
			dest_sg->acs_id,
			dest_pin,
			node_type,
			src_signal);

  dest_pins->connect_pin(dest_pin,
			 UNASSIGNED,
			 src_signal,
			 node_type,
			 src_signal);

  assign_netlist(src_pins,src_signal,
		 dest_pins,dest_pin);

  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(Pin* src_pins,
			      int src_pin,
			      int src_node,
			      int src_signal,
			      ACSCGFPGACore* dest_sg,
			      int dest_pin,
			      int dest_node,
			      int dest_signal,
			      int type)
{
  Pin* dest_pins=dest_sg->pins;
      
  if (src_pin==UNASSIGNED)
    src_pin=src_pins->retrieve_type(OUTPUT_PIN);

  if (dest_pin==UNASSIGNED)
    dest_pin=dest_pins->free_pintype(INPUT_PIN,src_pins->query_pinpriority(src_pin));

  // Make connection from source to destination
  if (src_node==UNASSIGNED)
    src_pins->connect_pin(src_pin,
			  dest_sg->acs_id,
			  dest_pin,
			  type,
			  dest_signal);
  else
    {
      Connectivity* tmp_con=src_pins->query_connection(src_pin);
      tmp_con->assign_node(src_node,
			   dest_sg->acs_id,
			   dest_pin,
			   type,
			   dest_signal);
    }
  
  // Make connection from destination to source
  if (dest_node==UNASSIGNED)
    dest_pins->connect_pin(dest_pin,
			   UNASSIGNED,
			   src_pin,
			   type,
			   src_signal);
  else
    {
      Connectivity* tmp_con=dest_pins->query_connection(dest_pin);
      tmp_con->assign_node(dest_node,
			   UNASSIGNED,
			   src_pin,
			   type,
			   src_signal);
    }

  assign_netlist(src_pins,src_pin,
		 dest_pins,dest_pin);

  // Return happy condition
  return(1);
}

int Sg_Constructs::connect_sg(ACSCGFPGACore* src_sg,
			      int src_type,
			      Pin* dest_pins,
			      int dest_signal,
			      int node_type)
{
  Pin* src_pins=src_sg->pins;
  int src_pin=src_pins->retrieve_type(src_type);

  src_pins->connect_pin(src_pin,
			UNASSIGNED,
			dest_signal,
			node_type,
			dest_signal);
  dest_pins->connect_pin(dest_signal,
			 src_sg->acs_id,
			 src_pin,
			 node_type,
		         dest_signal);

  assign_netlist(src_pins,src_pin,
		 dest_pins,dest_signal);

  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(ACSCGFPGACore* src_sg,
			      int src_pin,
			      int src_node,
			      int src_signal,
			      Pin* dest_pins,
			      int dest_pin,
			      int dest_node,
			      int dest_signal,
			      int type)
{
  Pin* src_pins=src_sg->pins;
  if (src_pin==UNASSIGNED)
    src_pin=src_pins->retrieve_type(OUTPUT_PIN);

  if (dest_pin==UNASSIGNED)
    dest_pin=dest_pins->free_pintype(INPUT_PIN,src_pins->query_pinpriority(src_pin));

  // Make connection from source to destination
  if (src_node==UNASSIGNED)
    src_pins->connect_pin(src_pin,
			  UNASSIGNED,
			  dest_pin,
			  type,
			  dest_signal);
  else
    {
      Connectivity* tmp_con=src_pins->query_connection(src_pin);
      tmp_con->assign_node(src_node,
			   UNASSIGNED,
			   dest_pin,
			   type,
			   dest_signal);
    } 

  
  // Make connection from destination to source
  if (dest_node==UNASSIGNED)
    {
      dest_pins->connect_pin(dest_pin,
			     src_sg->acs_id,
			     src_pin,
			     type,
			     src_signal);
    }
  else
    {
      Connectivity* tmp_con=dest_pins->query_connection(dest_pin);
      tmp_con->assign_node(dest_node,
			   src_sg->acs_id,
			   src_pin,
			   type,
			   dest_signal);
    }

  assign_netlist(src_pins,src_pin,
		 dest_pins,dest_pin);

/*
  dest_pins->match_precision(src_pins,
			     src_pin,
			     dest_pin);
			     */

  // Return happy condition
  return(1);
}

int Sg_Constructs::connect_sg(ACSCGFPGACore* src_sg,
			      ACSCGFPGACore* dest_sg)
{
  Pin* src_pins=src_sg->pins;
  Pin* dest_pins=dest_sg->pins;
  int src_pin=src_pins->retrieve_type(OUTPUT_PIN);
  int dest_pin=dest_pins->free_pintype(INPUT_PIN,src_pins->query_pinpriority(src_pin));

  if (DEBUG_CONSTRUCTS)
    printf("connecting %s pin %d, to %s pin %d\n",
	   src_sg->comment_name(),src_pin,
	   dest_sg->comment_name(),dest_pin);

  src_pins->connect_pin(src_pin,
			dest_sg->acs_id,
			dest_pin,
			DATA_NODE);

  dest_pins->connect_pin(dest_pin,
			 src_sg->acs_id,
			 src_pin,
			 DATA_NODE);

  assign_netlist(src_pins,src_pin,
		 dest_pins,dest_pin);

  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(ACSCGFPGACore* src_sg,
			      const int src_type,
			      ACSCGFPGACore* dest_sg)
{
  Pin* src_pins=src_sg->pins;
  Pin* dest_pins=dest_sg->pins;
  int src_pin=src_pins->retrieve_type(src_type);
  int dest_pin=dest_pins->free_pintype(INPUT_PIN,src_pins->query_pinpriority(src_pin));

  if (DEBUG_CONSTRUCTS)
    printf("connecting %s pin %d, to %s pin %d\n",
	   src_sg->comment_name(),src_pin,
	   dest_sg->comment_name(),dest_pin);

  src_pins->connect_pin(src_pin,
			dest_sg->acs_id,
			dest_pin,
			DATA_NODE);

  dest_pins->connect_pin(dest_pin,
			 src_sg->acs_id,
			 src_pin,
			 DATA_NODE);

  assign_netlist(src_pins,src_pin,
		 dest_pins,dest_pin);

  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(ACSCGFPGACore* src_sg,
			      ACSCGFPGACore* dest_sg,
			      const int dest_type)
{
  Pin* src_pins=src_sg->pins;
  Pin* dest_pins=dest_sg->pins;
  int src_pin=src_pins->retrieve_type(OUTPUT_PIN);
  int dest_pin=-1;
  if (src_pins->query_pinpriority(src_pin)!=UNASSIGNED)
    dest_pin=dest_pins->free_pintype(dest_type,src_pins->query_pinpriority(src_pin));
  else
    dest_pin=dest_pins->free_pintype(dest_type);

  if (DEBUG_CONSTRUCTS)
    printf("connecting %s pin %d, to %s pin %d\n",
	   src_sg->comment_name(),src_pin,
	   dest_sg->comment_name(),dest_pin);

  src_pins->connect_pin(src_pin,
			dest_sg->acs_id,
			dest_pin,
			DATA_NODE);

  dest_pins->connect_pin(dest_pin,
			 src_sg->acs_id,
			 src_pin,
			 DATA_NODE);

  assign_netlist(src_pins,src_pin,
		 dest_pins,dest_pin);

  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(ACSCGFPGACore* src_sg,
			      int src_type,
			      ACSCGFPGACore* dest_sg,
			      int dest_type)
{
  connect_sg(src_sg,src_type,dest_sg,dest_type,DATA_NODE);

  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(ACSCGFPGACore* src_sg,
			      int src_type,
			      ACSCGFPGACore* dest_sg,
			      int dest_type,
			      const int mode)
{
  Pin* src_pins=src_sg->pins;
  Pin* dest_pins=dest_sg->pins;
  int src_pin, dest_pin;
  match_pins(src_pins,src_type,src_pin,dest_pins,dest_type,dest_pin);

  src_pins->connect_pin(src_pin,
			dest_sg->acs_id,
			dest_pin,
			mode);

  dest_pins->connect_pin(dest_pin,
			 src_sg->acs_id,
			 src_pin,
			 mode);

  assign_netlist(src_pins,src_pin,
		 dest_pins,dest_pin);

  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(ACSCGFPGACore* src_sg,
			      int src_pin,
			      int src_node,
			      ACSCGFPGACore* dest_sg,
			      int dest_pin,
			      int dest_node,
			      int type)
{
  if (DEBUG_CONSTRUCTS)
    printf("Connecting sg %s, pin %d, node %d to sg %s, pin %d, node %d of type %d\n",
	   src_sg->name(),src_pin,src_node,
	   dest_sg->name(),dest_pin,dest_node,
	   type);

  Pin* src_pins=src_sg->pins;
  Pin* dest_pins=dest_sg->pins;

  if (src_pin==UNASSIGNED)
    src_pin=src_pins->retrieve_type(OUTPUT_PIN);

  if (dest_pin==UNASSIGNED)
    {
      dest_pin=dest_pins->free_pintype(INPUT_PIN,src_pins->query_pinpriority(src_pin));
      if (dest_pin==UNASSIGNED)
	dest_pin=dest_pins->free_pintype(INPUT_PIN);
    }
      
  // Make connection from source to destination
  if (src_node==UNASSIGNED)
    src_pins->connect_pin(src_pin,
			  dest_sg->acs_id,
			  dest_pin,
			  type);
  else
    {
      Connectivity* tmp_con=src_pins->query_connection(src_pin);
      tmp_con->assign_node(src_node,
			   dest_sg->acs_id,
			   dest_pin,
			   type);
    }

  
  // Make connection from destination to source
  if (dest_node==UNASSIGNED)
    dest_pins->connect_pin(dest_pin,
			   src_sg->acs_id,
			   src_pin,
			   type);
  else
    {
      Connectivity* tmp_con=dest_pins->query_connection(dest_pin);
      tmp_con->assign_node(dest_node,
			   src_sg->acs_id,
			   src_pin,
			   type);
    }

  assign_netlist(src_pins,src_pin,
		 dest_pins,dest_pin);

  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(CoreList* src_sgs,
			      int* src_pins,
			      int* src_nodes,
			      ACSCGFPGACore* dest_sg,
			      int dest_pin,
			      int dest_node,
			      int type)
{
  // FIX: This infers that these arguments must be unassigned!!!
  int orig_destpin=dest_pin;
  int orig_destnode=dest_node;

  Pin* dest_pins=dest_sg->pins;

  for (int sg_loop=0;sg_loop<src_sgs->size();sg_loop++)
    {
      int src_pin;
      ACSCGFPGACore* src_sg=(ACSCGFPGACore*) src_sgs->elem(sg_loop);
      Pin* src_pins_class=src_sg->pins;

      if ((src_pins==NULL) || (src_pins[sg_loop]==UNASSIGNED))
	src_pin=src_pins_class->retrieve_type(OUTPUT_PIN);
      else
	src_pin=src_pins[sg_loop];

      if (dest_pin==UNASSIGNED)
	dest_pin=dest_pins->free_pintype(INPUT_PIN,src_pins_class->query_pinpriority(src_pin));

      // Make connection from source to destination
      if (src_nodes==NULL)
	src_pins_class->connect_pin(src_pin,dest_sg->acs_id,dest_pin,type);
      else if (src_nodes[sg_loop]==UNASSIGNED)
	src_pins_class->connect_pin(src_pin,dest_sg->acs_id,dest_pin,type);
      else
	{
	  Connectivity* tmp_con=src_pins_class->query_connection(src_pin);
	  tmp_con->assign_node(src_nodes[sg_loop],
			       dest_sg->acs_id,
			       dest_pin,
			       type);
	}

  
      // Make connection from destination to source
      if (dest_node==UNASSIGNED)
	dest_pins->connect_pin(dest_pin,
			       src_sg->acs_id,
			       src_pin,
			       type);
      else
	{
	  Pin* dest_pins=dest_sg->pins;
	  Connectivity* tmp_con=dest_pins->query_connection(dest_pin);
	  tmp_con->assign_node(dest_node,
			       src_sg->acs_id,
			       src_pin,
			       type);
	}

      assign_netlist(src_pins_class,src_pin,
		     dest_pins,dest_pin);

      dest_node=orig_destnode;
      dest_pin=orig_destpin;
    }

  // Return happy condition
  return(1);
}

// This version is more explicit at finding UNASSIGNED pins by matching to
// parametered type
int Sg_Constructs::connect_sg(ACSCGFPGACore* src_sg,
				int src_pin,
				int src_node,
				int src_pin_type,
				ACSCGFPGACore* dest_sg,
				int dest_pin,
				int dest_node,
				int dest_pin_type,
				int type)
{
  Pin* src_pins=src_sg->pins;
  Pin* dest_pins=dest_sg->pins;

  if (src_pin==UNASSIGNED)
    src_pin=src_pins->retrieve_type(src_pin_type);

  if (dest_pin==UNASSIGNED)
    dest_pin=dest_pins->free_pintype(dest_pin_type,src_pins->query_pinpriority(src_pin));

  // Make connection from source to destination
  if (src_node==UNASSIGNED)
    src_pins->connect_pin(src_pin,
			  dest_sg->acs_id,
			  dest_pin,
			  type);
  else
    {
      Pin* src_pins=src_sg->pins;
      Connectivity* tmp_con=src_pins->query_connection(src_pin);
      tmp_con->assign_node(src_node,
			   dest_sg->acs_id,
			   dest_pin,
			   type);
    }

  
  // Make connection from destination to source
  if (dest_node==UNASSIGNED)
    dest_pins->connect_pin(dest_pin,
			   src_sg->acs_id,
			   src_pin,
			   type);
  else
    {
      Pin* dest_pins=dest_sg->pins;
      Connectivity* tmp_con=dest_pins->query_connection(dest_pin);
      tmp_con->assign_node(dest_node,
			   src_sg->acs_id,
			   src_pin,
			   type);
    }
      
  assign_netlist(src_pins,src_pin,
		 dest_pins,dest_pin);
  

  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(CoreList* src_sgs,
			      int* src_pins,
			      int* src_nodes,
			      int src_pin_type,
			      ACSCGFPGACore* dest_sg,
			      int dest_pin,
			      int dest_node,
			      int dest_pin_type,
			      int type)
{
  // FIX: This infers that these are unassigned!
  int orig_destpin=dest_pin;
  int orig_destnode=dest_node;
  
  Pin* dest_pins=dest_sg->pins;

  for (int sg_loop=0;sg_loop<src_sgs->size();sg_loop++)
    {
      ACSCGFPGACore* src_sg=(ACSCGFPGACore*) src_sgs->elem(sg_loop);
      Pin* src_pins_class=src_sg->pins;

      if (DEBUG_CONSTRUCTS)
	printf("Attaching sg %s to %s\n",src_sg->name(),dest_sg->name());

      int src_pin;
      if ((src_pins==NULL) || (src_pins[sg_loop]==UNASSIGNED))
	src_pin=src_pins_class->retrieve_type(src_pin_type);
      else
	src_pin=src_pins[sg_loop];

      if (dest_pin==UNASSIGNED)
	dest_pin=dest_pins->free_pintype(dest_pin_type,src_pins_class->query_pinpriority(src_pin));

      // Make connection from source to destination
      if (src_nodes==NULL)
	src_pins_class->connect_pin(src_pin,dest_sg->acs_id,dest_pin,type);
      else
	{
	  Pin* src_pins=src_sg->pins;
	  Connectivity* tmp_con=src_pins->query_connection(src_pin);
	  tmp_con->assign_node(src_nodes[sg_loop],
			       dest_sg->acs_id,
			       dest_pin,
			       type);
	}

  
      // Make connection from destination to source
      if (dest_node==UNASSIGNED)
	dest_pins->connect_pin(dest_pin,
			       src_sg->acs_id,
			       src_pin,
			       type);
      else
	{
	  Pin* dest_pins=dest_sg->pins;
	  Connectivity* tmp_con=dest_pins->query_connection(dest_pin);
	  tmp_con->assign_node(dest_node,
			       src_sg->acs_id,
			       src_pin,
			       type);
	}

      assign_netlist(src_pins_class,src_pin,
		     dest_pins,dest_pin);

      dest_pin=orig_destpin;
      dest_node=orig_destnode;
    }

  // Return happy condition
  return(1);
}

void Sg_Constructs::assign_netlist(Pin* src_pins, const int src_pin,
				   Pin* dest_pins, const int dest_pin)
{
    /////////////////////////////
  // Assign netlist information
  /////////////////////////////
  int netlist_id=src_pins->query_netlistid(src_pin);
  if (netlist_id==UNASSIGNED)
    {
      if (free_netid==NULL)
	netlist_id=-1;
      else
	{
	  netlist_id=*free_netid;
	  *free_netid=*free_netid+1;
	}
      src_pins->set_netlistid(src_pin,netlist_id);
    }
  dest_pins->set_netlistid(dest_pin,netlist_id);
}

// First find a source pin that matches, then find a destination which meets both
// destination pin type and any appropriate source pin priority
void Sg_Constructs::match_pins(Pin* src_pins,
			       int src_type,
			       int& src_pin,
			       Pin* dest_pins,
			       int dest_type,
			       int& dest_pin)
{
  // First match a first found source to any destination
  src_pin=src_pins->retrieve_type(src_type);
  int src_priority=src_pins->query_pinpriority(src_pin);
  if (DEBUG_MATCHPINS)
    printf("match_pins::src(%d,%d)\n",src_pin,src_priority);
  
  dest_pin=dest_pins->free_pintype(dest_type,src_priority);

  // If unsuccessful, then check if the destinations has the known priority and
  // then find the src that matches
  if (dest_pin==UNASSIGNED)
    {
      if (DEBUG_MATCHPINS)
	printf("no dest found\n");

      if (src_priority==UNASSIGNED)
	dest_pin=dest_pins->retrieve_type(dest_type);
      else
	dest_pin=dest_pins->retrieve_type(dest_type,src_priority);
      int dest_priority=dest_pins->query_pinpriority(dest_pin);
      if (DEBUG_MATCHPINS)
	printf("match_pins::dest(%d,%d)\n",dest_pin,dest_priority);
      src_pin=src_pins->free_pintype(src_type,dest_priority);
    }
}

// Works for Constants classes with only a single constant instance!
// FIX:Need to trap, or better yet generalize
void Sg_Constructs::balance_bw(CoreList* sg_list)
{
  // Determine max bitwidth of the group
  int max_bw=-1;
  for (int sg_loop=0;sg_loop<sg_list->size();sg_loop++)
    {
      ACSCGFPGACore* smart_generator=(ACSCGFPGACore*) sg_list->elem(sg_loop);
      int curr_bw=(smart_generator->sg_constants)->query_bitsize(0,-1);
      if (curr_bw > max_bw)
	max_bw=curr_bw;
      
      if (DEBUG_BWBALANCE)
	printf("HWbalance_bw, sg %s has bw of %d\n",
	       smart_generator->comment_name(),
	       curr_bw);
    }

  // Now set all precisions to match bit widths
  for (int sg_loop=0;sg_loop<sg_list->size();sg_loop++)
    {
      ACSCGFPGACore* smart_generator=(ACSCGFPGACore*) sg_list->elem(sg_loop);
      (smart_generator->pins)->set_precision(0,max_bw-1,max_bw,LOCKED);
    }
}

////////////////////////////////////////////////////////////////////////////
// Return the total number of bits needed to represent this value (UNSIGNED)
////////////////////////////////////////////////////////////////////////////
int Sg_Constructs::bit_sizer(const int value)
{
  int abs_value=(int) abs(value);
  int bit_count=0;
  if (abs_value==0)
    bit_count=1;
  else
    bit_count=(int) ceil(log(abs_value+1.0)/log(2.0));

  return(bit_count);
}


