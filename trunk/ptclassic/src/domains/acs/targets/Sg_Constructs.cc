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
: valid(1), target_fpga(NULL)
{
  free_id=id_ptr;
  design_directory=new char[MAX_STR];
  strcpy(design_directory,design_dir);
}
Sg_Constructs::Sg_Constructs(int* id_ptr,
			     char* design_dir, 
			     Fpga* fpga_device,
			     const int device_no)
: valid(1)
{
  target_fpga=fpga_device;
  target_device=device_no;
  free_id=id_ptr;
  design_directory=new char[MAX_STR];
  strcpy(design_directory,design_dir);
}

Sg_Constructs::~Sg_Constructs()
{
  valid=0;
  target_fpga=NULL;
  target_device=-1;
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
void Sg_Constructs::set_targetdevice(Fpga* fpga_device,
				     const int device_no)
{
  validate();

  target_fpga=fpga_device;
  target_device=device_no;
}

///////////////////////////////////////////////////////////////////////////////
// Methods for adding specialized smart generators
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Add a buffer smart generator and assign it to the sg_list
////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_buffer(const int sign_convention,
					 SequentialList* sg_list)
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
					 SequentialList* sg_list)
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
				      SequentialList* sg_list)
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
				      SequentialList* sg_list)
{
  ACSCGFPGACore* mux_core=add_sg("ACS","Mux","CGFPGA",
				 BOTH,sign_convention,
				 slines,sg_list);

  // Return happy condition
  return(mux_core);
}


//////////////////////////////////////////////////////////////////////////////
// Add a constant smart generator and assign it to the sg_list.  Additionally,
// take the constant parameter and assign it to a new Constants instance for
// this newly generated smart generator instance.
//////////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_const(int constant,
					const int sign_convention,
					SequentialList* sg_list)
{
  ACSCGFPGACore* const_core=add_sg("ACS","Const","CGFPGA",
				   BOTH,sign_convention,sg_list);
  const_core->sg_constants=new Constants;
  (const_core->sg_constants)->add(&constant,CINT);

  // Return happy condition
  return(const_core);
}
ACSCGFPGACore* Sg_Constructs::add_const(long constant,
					const int sign_convention,
					SequentialList* sg_list)
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
					SequentialList* sg_list)
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
					SequentialList* sg_list)
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
					SequentialList* sg_list)
{
  ACSCGFPGACore* subtracter_core=add_sg("ACS","Sub","CGFPGA",
					BOTH,sign_convention,sg_list);

  // Return happy condition
  return(subtracter_core);
}


/////////////////////////////////////////////
// Add a delay smart generator to the sg_list
/////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_delay(const int sign_convention,
					SequentialList* sg_list)
{
  ACSCGFPGACore* delay_core=add_sg("ACS","Delay","CGFPGA",
				   BOTH,sign_convention,sg_list);

  // Return happy condition
  return(delay_core);
}



///////////////////////////////////////////////////////////////////////////////
// Methods for adding smart generators
///////////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_sg(char* domain,
				     char* corona,
				     char* core,
				     int type,
				     int sign_convention,
				     SequentialList* sg_list)
{
  ACSCGFPGACore* gend_core=add_sg_core1(domain,corona,core,type,sign_convention);
  
  gend_core=add_sg_core2(gend_core,sg_list);

  // Return happy(?) condition
  return(gend_core);
}


/////////////////////////////////////////////////////////////////////////
// This version is used for dark stars as parameter initialize is handled
// later after the dark star has set all the necessary parameters
/////////////////////////////////////////////////////////////////////////
ACSCGFPGACore* Sg_Constructs::add_sg(char* domain,
				     char* corona,
				     char* core,
				     int sign_convention,
				     SequentialList* sg_list)
{
  ACSCGFPGACore* gend_core=add_sg_core1(domain,corona,core,BOTH,sign_convention);
  
  // Add this new core/corona to the list of smart generators
  sg_list->append((Pointer) gend_core);

  // Initialze the new core
  gend_core->sg_initialize(design_directory,ALGORITHM,free_id);
  gend_core->sg_resources(UNLOCKED);


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
				     SequentialList* sg_list)
{
  ACSCGFPGACore* gend_core=
    add_sg_core1(domain,corona,core,type,sign_convention);
  
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
				     SequentialList* sg_list)
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
	  fpga_core->acs_device=target_device;

	  // Notify the fpga of the new core
	  target_fpga->set_child(fpga_core);

	  // Return cleanly
	  (*free_id)++;
	  return(fpga_core);
	}
      else
	printf("Unable to reference core\n");
    }

  // Return error
  if (DEBUG_CONSTRUCTS)
    printf("Unable to add smart generator %s\n",star_name);

  // Cleanup
  delete []star_name;

  // Return unhappy condition
  return(NULL);
}

ACSCGFPGACore* Sg_Constructs::add_sg_core2(ACSCGFPGACore* fpga_core,
					   SequentialList* sg_list)
{
  if (fpga_core != NULL)
    {
      // Populate Port_Ids 
      fpga_core->sg_initialize(design_directory,DYNAMIC,free_id);

      fpga_core->sg_resources(UNLOCKED);

      // Since this would be a non-algorithmic smart generator,
      // it's delay will be of the pipe alignment variety
      fpga_core->pipe_delay=fpga_core->acs_delay;

      // Add this new core/corona to the list of smart generators
      sg_list->append((Pointer) fpga_core);

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
  int dest_pin=(dest_sg->pins)->free_pintype(dest_type);

  src_pins->connect_pin(src_signal,
			dest_sg->acs_id,
			dest_pin,
			node_type,
			src_signal);

  Pin* dest_pins=dest_sg->pins;
  dest_pins->connect_pin(dest_pin,
			 UNASSIGNED,
			 src_signal,
			 node_type,
			 src_signal);

/*
  dest_pins->match_precision(src_pins,
			     src_signal,
			     dest_pin);
			     */

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
    src_pin=src_pins->free_pintype(OUTPUT_PIN);

  if (dest_pin==UNASSIGNED)
    dest_pin=dest_pins->free_pintype(INPUT_PIN);

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

/*
  dest_pins->match_precision(src_pins,
			     src_pin,
			     dest_pin);
			     */

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
  int src_pin=src_pins->free_pintype(src_type);
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

/*
  dest_pins->match_precision(src_pins,
			     src_pin,
			     dest_signal);
			     */

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
    src_pin=src_pins->free_pintype(OUTPUT_PIN);

  if (dest_pin==UNASSIGNED)
    dest_pin=dest_pins->free_pintype(INPUT_PIN);

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

/*
  dest_pins->match_precision(src_pins,
			     src_pin,
			     dest_pin);
			     */

  // Return happy condition
  return(1);
}

int Sg_Constructs::connect_sg(ACSCGFPGACore* src_sg,
			      int src_type,
			      ACSCGFPGACore* dest_sg,
			      int dest_type)
{
  Pin* src_pins=src_sg->pins;
  Pin* dest_pins=dest_sg->pins;
  int src_pin=src_pins->free_pintype(src_type);
  int dest_pin=dest_pins->free_pintype(dest_type);

  src_pins->connect_pin(src_pin,
			dest_sg->acs_id,
			dest_pin,
			DATA_NODE);

  dest_pins->connect_pin(dest_pin,
			 src_sg->acs_id,
			 src_pin,
			 DATA_NODE);

/*
  dest_pins->match_precision(src_pins,
			     src_pin,
			     dest_pin);
			     */

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
    src_pin=src_pins->free_pintype(OUTPUT_PIN);

  if (dest_pin==UNASSIGNED)
    dest_pin=dest_pins->free_pintype(INPUT_PIN);

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

/*  
  dest_pins->match_precision(src_pins,
			     src_pin,
			     dest_pin);
			     */

  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(SequentialList* src_sgs,
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

      if (src_pins==NULL)
	src_pin=src_pins_class->free_pintype(OUTPUT_PIN);
      else if (src_pins[sg_loop]==UNASSIGNED)
	src_pin=src_pins_class->free_pintype(OUTPUT_PIN);
      else
	src_pin=src_pins[sg_loop];

      if (dest_pin==UNASSIGNED)
	dest_pin=dest_pins->free_pintype(INPUT_PIN);

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

/*
      dest_pins->match_precision(src_pins_class,
				 src_pin,
				 dest_pin);
				 */
      
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
    src_pin=src_pins->free_pintype(src_pin_type);

  if (dest_pin==UNASSIGNED)
    dest_pin=dest_pins->free_pintype(dest_pin_type);

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

/*
  dest_pins->match_precision(src_pins,
			     src_pin,
			     dest_pin);
			     */
      
  // Return happy condition
  return(1);
}
int Sg_Constructs::connect_sg(SequentialList* src_sgs,
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
      if (src_pins==NULL)
	src_pin=src_pins_class->free_pintype(src_pin_type);
      else if (src_pins[sg_loop]==UNASSIGNED)
	src_pin=src_pins_class->free_pintype(src_pin_type);
      else
	src_pin=src_pins[sg_loop];

      if (dest_pin==UNASSIGNED)
	dest_pin=dest_pins->free_pintype(dest_pin_type);

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

/*
      dest_pins->match_precision(src_pins_class,
				 src_pin,
				 dest_pin);
				 */
      
      dest_pin=orig_destpin;
      dest_node=orig_destnode;
    }

  // Return happy condition
  return(1);
}

