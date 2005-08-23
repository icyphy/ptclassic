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
 Version: @(#)Arch.cc	1.6 09/07/01
***********************************************************************/
#include "Arch.h"

inline Arch::Arch(void) 
: fpga_count(0), fpgas(NULL), 
  total_solutions(0), src_fpga(-1), dest_fpga(-1), 
  route_solutions(NULL), current_solution(NULL), 
  mem_count(0), mem_ports(NULL),
  systolic_count(0), systolic_connections(NULL), sys_ports(NULL),
  archsynthesis_libname(NULL), archsynthesis_files(NULL), archsynthesis_path(NULL),
  archsupport_files(NULL), archsupport_path(NULL),
  misc_files(NULL), misc_path(NULL), misc_origin(NULL),
  simulation_commands(NULL)
{}

Arch::Arch(const char* arch_path, const char* arch_filename)
: fpga_count(0), fpgas(NULL), 
  total_solutions(0), src_fpga(-1), dest_fpga(-1), 
  route_solutions(NULL), current_solution(NULL), 
  mem_count(0), mem_ports(NULL),
  systolic_count(0), systolic_connections(NULL), sys_ports(NULL),
  archsynthesis_libname(NULL), archsynthesis_files(NULL), archsynthesis_path(NULL),
  archsupport_files(NULL), archsupport_path(NULL),
  misc_files(NULL), misc_path(NULL), misc_origin(NULL),
  simulation_commands(NULL)
{
  arch_directory=new char[MAX_STR];
  char* total_path=new char[MAX_STR];
  strcpy(arch_directory,arch_path);
  sprintf(total_path,"%s/src/domains/acs/utils/%s",arch_directory,arch_filename);

  arch_stream=new ifstream(total_path);
  if (arch_stream->bad())
    {
      fprintf(stderr,"Unable to open arch_file\n");
      abort();
    }
  delete []total_path;
  
  read_header();

  set_fpga();
  set_memory();
  set_systolic();
  if (DEBUG_ARCH)
    print_arch("Allocated architecture:\n");
}

Arch::~Arch(void)
{
  delete []arch_directory;

  fpga_count=0;
  delete []fpgas;

  mem_count=0;
  delete []mem_ports;

  for (int loop=0;loop<systolic_count;loop++)
    delete systolic_connections[loop];
  systolic_count=0;
  delete []systolic_connections;
  delete []sys_ports;

  delete archsynthesis_libname;
  delete archsynthesis_files;
  delete archsynthesis_path;
  delete archsupport_files;
  delete archsupport_path;
  delete misc_files;
  delete misc_path;
  delete misc_origin;
  delete simulation_commands;

  wipe_solution();
}

void Arch::read_header(void)
{
  // Read the first line contain main architecture info
  char* line_buf=new char[MAX_STR];
  arch_stream->getline(line_buf,MAX_STR);
  istrstream arch_info(line_buf,MAX_STR);
  
  arch_info >> fpga_count >> mem_count >> systolic_count;

  // Read in supporting VHDL files for the board architecture
  // First, the preamble synthesis
  archsynthesis_libname=new StringArray;
  archsynthesis_files=new StringArray;
  archsynthesis_path=new StringArray;
  int synth_count;
  arch_stream->getline(line_buf,MAX_STR);
  istrstream arch_synthcount(line_buf,MAX_STR);
  arch_synthcount >> synth_count;
  char* libname=new char[MAX_STR];
  char* filename=new char[MAX_STR];
  char* path=new char[MAX_STR];
  for (int loop=0;loop<synth_count;loop++)
    {
      arch_stream->getline(line_buf,MAX_STR);
      istrstream synthfile(line_buf,MAX_STR);
      synthfile >> libname >> path >> filename;
      archsynthesis_libname->add(libname);
      archsynthesis_path->add(path);
      archsynthesis_files->add(filename);
    }
  // Next the simulation preamble files
  archsupport_files=new StringArray;
  archsupport_path=new StringArray;
  arch_stream->getline(line_buf,MAX_STR);
  istrstream arch_supportcount(line_buf,MAX_STR);
  int support_count;
  arch_supportcount >> support_count;
  for (int loop=0;loop<support_count;loop++)
    {
      arch_stream->getline(line_buf,MAX_STR);
      istrstream sfile(line_buf,MAX_STR);
      sfile >> path >> filename;
      archsupport_path->add(path);
      archsupport_files->add(filename);
    }
  // Next the post_amble files:P
  misc_files=new StringArray;
  misc_path=new StringArray;
  misc_origin=new ACSIntArray;
  int misc_count;
  arch_stream->getline(line_buf,MAX_STR);
  istrstream arch_mcount(line_buf,MAX_STR);
  arch_mcount >> misc_count;
  int origin;
  for (int loop=0;loop<misc_count;loop++)
    {
      arch_stream->getline(line_buf,MAX_STR);
      istrstream mfile(line_buf,MAX_STR);
      mfile >> path >> filename >> origin;
      misc_path->add(path);
      misc_files->add(filename);
      misc_origin->add(origin);
    }

  // Cleanup
  delete []libname;
  delete []filename;
  delete []path;
  delete []line_buf;

  // Generate Fpga instances
  fpgas=new Fpga[fpga_count];

  // Generate memory port instances
  mem_ports=new Port[mem_count];
  for (int mem_loop=0;mem_loop<mem_count;mem_loop++)
    mem_ports[mem_loop].designate_memory();

  sys_ports=new Port[systolic_count];
  systolic_connections=new ACSIntArray*[systolic_count]; 
  for (int sys_loop=0;sys_loop<systolic_count;sys_loop++)
    {
      sys_ports[sys_loop].designate_sysinterconnect();
      systolic_connections[sys_loop]=new ACSIntArray;
    }
}

void Arch::wipe_solution(void)
{
  delete current_solution;
  for (int loop=0;loop<total_solutions;loop++)
    delete route_solutions[loop];
  delete route_solutions;
  total_solutions=0;
}

//////////////////////////////////////////////////////////////////////
// Get information for the fpga's currently used in this architecture
// NOTE: The architecture file should be generated by an ACS tool that
//       is aware of the constants used herein for consistency.  
//       Especially pin-type constants!
// FIX: The memory definition forces pin type constraints, perhaps
//      this should be done here as well to avoid problems
//////////////////////////////////////////////////////////////////////
void Arch::set_fpga(void)
{
  char* line_buf=new char[MAX_STR];
  char* part_mk=new char[MAX_STR];
  char* part_tk=new char[MAX_STR];
  char* part_tp=new char[MAX_STR];
  char* part_pk=new char[MAX_STR];
  char* ext_name=new char[MAX_STR];
  char* ext_bindname=new char[MAX_STR];
  char* sequencer_name=new char[MAX_STR];
  char* full_sequencer=new char[MAX_STR];
  char* filename=new char[MAX_STR];
  char* path=new char[MAX_STR];
  char* library_name=new char[MAX_STR];
  int speed_grade;
  int reserve_state;

  // Identify each part, and setup external control pin information
  for (int fpga_no=0;fpga_no<fpga_count;fpga_no++)
    {
      // Assign a unique identifier at the architecture level
      fpgas[fpga_no].set_acsdevice(fpga_no);

      // Initialize fpga interconnect info
      fpgas[fpga_no].add_interconnect(fpga_count);

      // Obtain global information about this fpga
      arch_stream->getline(line_buf,MAX_STR);
      istrstream arch_info(line_buf,MAX_STR);
      arch_info >> part_mk >> part_tk >> part_tp >> part_pk >> speed_grade >> reserve_state;
      fpgas[fpga_no].set_part(part_mk,part_tk,part_tp,part_pk,speed_grade);
      if (reserve_state)
	fpgas[fpga_no].usage_state=FPGA_RESERVED;

      // Classify the FPGA for core use
      if (strcmp(part_tk,"XC4000XL")==0)
	fpgas[fpga_no].part_type=XC4000;
      else if (strcmp(part_tk,"Virtex")==0)
	fpgas[fpga_no].part_type=VIRTEX;

      // Delegate sequencer information to each fpga
      arch_stream->getline(line_buf,MAX_STR);
      istrstream seq_name(line_buf,MAX_STR);
      seq_name >> sequencer_name;
      sprintf(full_sequencer,"%s/%s",arch_directory,sequencer_name);
      fpgas[fpga_no].set_iofilename(full_sequencer);

      // Obtain external pin information about this fpga
      int extpin_count;
      int bitlen;
      int pin_type;
      int pin_assertion;
      arch_stream->getline(line_buf,MAX_STR);
      istrstream extpin_stream(line_buf,MAX_STR);
      extpin_stream >> extpin_count;
      for (int pin_loop=0;pin_loop<extpin_count;pin_loop++)
	{
	  arch_stream->getline(line_buf,MAX_STR);
	  istrstream extpin_info(line_buf,MAX_STR);
	  extpin_info >> ext_name >> ext_bindname >> bitlen >> pin_type 
		      >> pin_assertion;
	  fpgas[fpga_no].set_control_pins(ext_name,ext_bindname,bitlen,pin_type,
					  pin_assertion);
	}

      // Add synthesis files
      int sfile_count;
      arch_stream->getline(line_buf,MAX_STR);
      istrstream synth_countstream(line_buf,MAX_STR);
      synth_countstream >> sfile_count;
      for (int loop=0;loop<sfile_count;loop++)
	{
	  arch_stream->getline(line_buf,MAX_STR);
	  istrstream synth_stream(line_buf,MAX_STR);
	  synth_stream >> library_name >> path >> filename;
	  fpgas[fpga_no].set_synthesis_file(library_name,path,filename);
	}

      // Add preamble support files
      int file_count;
      int origin;
      arch_stream->getline(line_buf,MAX_STR);
      istrstream arch_countstream(line_buf,MAX_STR);
      arch_countstream >> file_count;
      for (int loop=0;loop<file_count;loop++)
	{
	  arch_stream->getline(line_buf,MAX_STR);
	  istrstream file_stream(line_buf,MAX_STR);
	  file_stream >> path >> filename >> origin;
	  fpgas[fpga_no].set_preamble_file(path,filename,origin);
	}
      // Add postamble support files
      int pfile_count;
      arch_stream->getline(line_buf,MAX_STR);
      istrstream arch_pcountstream(line_buf,MAX_STR);
      arch_pcountstream >> pfile_count;
      for (int loop=0;loop<pfile_count;loop++)
	{
	  arch_stream->getline(line_buf,MAX_STR);
	  istrstream file_stream(line_buf,MAX_STR);
	  file_stream >> path >> filename >> origin;
	  fpgas[fpga_no].set_postamble_file(path,filename,origin);
	}
    }
  
  // Cleanup
  delete []line_buf;
  delete []part_mk;
  delete []part_tk;
  delete []part_tp;
  delete []part_pk;
  delete []ext_name;
  delete []ext_bindname;
  delete []sequencer_name;
  delete []full_sequencer;
  delete []filename;
  delete []path;
  delete []library_name;
}

///////////////////////////////////////////////////////////////////////
// Get information for the memories currently used in this architecture
// FIX:For now, the memories stats are fixed.
///////////////////////////////////////////////////////////////////////
void Arch::set_memory(void)
{
  // Prepare each fpga prior to storing info about local memories
  for (int fpga_loop=0;fpga_loop<fpga_count;fpga_loop++)
    fpgas[fpga_loop].add_memory(mem_count);

  // Specify all the memories
  char* line_buf=new char[MAX_STR];
  char* datain_name=new char[MAX_STR];
  char* dataout_name=new char[MAX_STR];
  char* addr_name=new char[MAX_STR];
  char* read_name=new char[MAX_STR];
  char* write_name=new char[MAX_STR];
  char* ce_name=new char[MAX_STR];
  for (int mem_loop=0;mem_loop<mem_count;mem_loop++)
    {
      // Determine and set static port characteristics
      int data_bits;
      int addr_bits;
      int low_address;
      int high_address;
      int bidirectional_data;
      int rw_separate;
      int read_skew;
      int write_skew;
      int reserved;
      arch_stream->getline(line_buf,MAX_STR);
      istrstream port_info(line_buf,MAX_STR);
      port_info >> data_bits >> addr_bits >> low_address >> high_address >> bidirectional_data 
		>> rw_separate >> read_skew >> write_skew >> reserved;
      mem_ports[mem_loop].set_port(data_bits,addr_bits,low_address,high_address,bidirectional_data,
				   rw_separate,read_skew,write_skew);
      if (reserved)
	mem_ports[mem_loop].reserve_port();

      // Determine and set VHDL namings
      arch_stream->getline(line_buf,MAX_STR);
      istrstream vhdl_info(line_buf,MAX_STR);
      vhdl_info >> datain_name >> dataout_name >> addr_name;
      if (rw_separate)
	vhdl_info >> read_name;
      vhdl_info >> write_name >> ce_name;

      // Determine and set fpga connection info
      int connection_count;
      int connected_fpga;
      arch_stream->getline(line_buf,MAX_STR);
      istrstream pc_count(line_buf,MAX_STR);
      pc_count >> connection_count;
      mem_ports[mem_loop].add_connected(connection_count);
      for (int cloop=0;cloop<connection_count;cloop++)
	{
	  arch_stream->getline(line_buf,MAX_STR);
	  istrstream pc_info(line_buf,MAX_STR);
	  pc_info >> connected_fpga;
	  mem_ports[mem_loop].set_connected(cloop,fpgas[connected_fpga]);

	  // Notify information about pin information regarding this memory
	  fpgas[connected_fpga].set_memory_pins(mem_loop,
						datain_name,dataout_name,data_bits,
						addr_name,addr_bits,low_address,high_address,
						bidirectional_data,rw_separate,
						read_name,write_name,ce_name,
						read_skew,write_skew);
						
	}
    }
  
  // Cleanup
  delete []line_buf;
  delete []datain_name;
  delete []dataout_name;
  delete []addr_name;
  delete []read_name;
  delete []write_name;
  delete []ce_name;
}

////////////////////////////////////////////////////////////////////////////////////////
// Get information for the systolic interconnects currently used in this architecture.
// FIX:Need to improve structure for concept of bidirectionality, it is implicit for now
////////////////////////////////////////////////////////////////////////////////////////
void Arch::set_systolic(void)
{
  char* line_buf=new char[MAX_STR];
  char* output_name=new char[MAX_STR];
  char* input_name=new char[MAX_STR];
  char* enable_name=new char[MAX_STR];

  for (int loop=0;loop<systolic_count;loop++)
    {
      // Determine systolic interconnect characteristics
      Port* sys_port=get_systolic_handle(loop);
      arch_stream->getline(line_buf,MAX_STR);
      istrstream sys_stats(line_buf,MAX_STR);
      int bitwidth;
      int bidir;
      int rw_control;
      int read_skew;
      int write_skew;
      sys_stats >> bitwidth >> bidir >> rw_control >> read_skew >> write_skew;
      if (DEBUG_ARCH)
	{
	  printf("line_buf=%s\n",line_buf);
	  printf("loop=%d, bitwidth=%d, bidir=%d, rw_control=%d, read_skew=%d, write_skew=%d\n",
		 loop,
		 bitwidth,
		 bidir,
		 rw_control,
		 read_skew,
		 write_skew);
	}
      sys_port->set_port(bitwidth,bidir,rw_control,read_skew,write_skew);

      // Determine and assign pins for the interconnects for each fpga (two total)
      int fpga_one, fpga_two;
      int data_bitwidth;
      int enable_bitwidth;
      arch_stream->getline(line_buf,MAX_STR);
      istrstream right_info(line_buf,MAX_STR);
      right_info >> fpga_one >> fpga_two >> data_bitwidth >> enable_bitwidth;
      if (DEBUG_ARCH)
	{
	  printf("line_buf=%s\n",line_buf);
	  printf("loop=%d, fpga_one=%d, fpga_two=%d, data_bitwidth=%d, enable_bitwidth=%d\n",
		 loop,
		 fpga_one,
		 fpga_two,
		 data_bitwidth,
		 enable_bitwidth);
	}

      arch_stream->getline(line_buf,MAX_STR);
      istrstream right_names(line_buf,MAX_STR);
      right_names >> output_name >> input_name >> enable_name;
      if (DEBUG_ARCH)
	{
	  printf("line_buf=%s\n",line_buf);
	  printf("from right names (%s,%s,%s)\n",output_name,
		 input_name,
		 enable_name);
	}
      fpgas[fpga_one].set_interconnect_pins(fpga_two,
					    output_name,input_name,data_bitwidth,
					    BIDIR_PIN_INTERCONNECT,
					    enable_name,enable_bitwidth,
					    OUTPUT_PIN_INTERCONNECT_ENABLE,1);

      arch_stream->getline(line_buf,MAX_STR);
      istrstream left_names(line_buf,MAX_STR);
      left_names >> output_name >> input_name >> enable_name;
      if (DEBUG_ARCH)
	{
	  printf("line_buf=%s\n",line_buf);
	  printf("from left names (%s,%s,%s)\n",output_name,
		 input_name,
		 enable_name);
	}
      fpgas[fpga_two].set_interconnect_pins(fpga_one,
					    output_name,input_name,data_bitwidth,
					    BIDIR_PIN_INTERCONNECT,
					    enable_name,enable_bitwidth,
					    OUTPUT_PIN_INTERCONNECT_ENABLE,0);

      systolic_connections[loop]->add(fpga_one);
      systolic_connections[loop]->add(fpga_two);
    }

  // Cleanup
  delete []line_buf;
  delete []output_name;
  delete []input_name;
  delete []enable_name;
}

//////////////////////////////////////////////////////////////////
// Evaluate core assignments to the fpgas
// FIX: Could improve the defaulting function for a bad assignment
//////////////////////////////////////////////////////////////////
int Arch::evaluate_fpga_assignment(int& acs_device)
{
  int bad_assignment=0;
  if ((acs_device<=UNASSIGNED) || (acs_device > fpga_count))
    bad_assignment=1;
  if (bad_assignment)
    {
      acs_device=1;
      activate_fpga(acs_device);
      return(0);
    }
  else
    {
      int good_assignment=activate_fpga(acs_device);
      if (!good_assignment)
	{
	  acs_device=1;
	  activate_fpga(acs_device);
	  return(0);
	}
    }

  // Original placement is valid, activate it
  activate_fpga(acs_device);

  // Return happy condition
  return(1);
}

////////////////////////////////////////////////
// Set the fpga for usage in this implementation
////////////////////////////////////////////////
int Arch::activate_fpga(const int fpga_no)
{
  if (fpgas[fpga_no].usage_state==FPGA_RESERVED)
    return(0);

  fpgas[fpga_no].usage_state=FPGA_USED;

  // Return happy condition
  return(1);
}

//////////////////////////////////////////////////////////////////
// Evaluate core assignments to the memories
// FIX: Could improve the defaulting function for a bad assignment
//////////////////////////////////////////////////////////////////
int Arch::evaluate_memory_assignment(int& acs_device)
{
  int bad_assignment=0;
  if ((acs_device<=UNASSIGNED) || (acs_device > mem_count))
    bad_assignment=1;
  if (bad_assignment)
    {
      acs_device=1;
      activate_memory(acs_device);
      return(0);
    }
  else
    {
      // Activate the default Fpga for the assigned memory
      // NOTE: defaults to the first fpga that is designated for access
      Fpga* local_fpga=mem_ports[acs_device].get_fpga_handle(0);
      int good_assignment=activate_fpga(local_fpga->fpga_id);
      if (!good_assignment)
	{
	  acs_device=1;
	  activate_memory(acs_device);
	  return(0);
	}
    }

  // Original placement is valid, activate it
  activate_memory(acs_device);

  // Return happy condition
  return(1);
}

//////////////////////////////////////////////////
// Set the memory for usage in this implementation
//////////////////////////////////////////////////
int Arch::activate_memory(const int mem_no)
{
  int status=mem_ports[mem_no].activate_port();
  return(status);
}

////////////////////////////////////
// For a given fpga, return a handle
////////////////////////////////////
Fpga* Arch::get_fpga_handle(const int fpga_no)
{
  if (fpga_no<fpga_count)
    return(&fpgas[fpga_no]);
  else
    {
      fprintf(stderr,"Arch::get_fpga_handle:Error, request for invalid fpga %d\n",fpga_no);
      return(NULL);
    }
}

//////////////////////////////////////
// For a given memory, return a handle
//////////////////////////////////////
Port* Arch::get_memory_handle(const int memory_no)
{
  if (memory_no<mem_count)
    return(&mem_ports[memory_no]);
  else
    {
      fprintf(stderr,"Arch::get_memory_handle:Error, request for invalid memory %d\n",
	     memory_no);
      return(NULL);
    }
}

/////////////////////////////////////////////////////
// For a given systolic interconnect, return a handle
/////////////////////////////////////////////////////
Port* Arch::get_systolic_handle(const int sys_no)
{
  if (sys_no<systolic_count)
    return(&sys_ports[sys_no]);
  else
    {
      fprintf(stderr,
	      "Arch::get_systolic_handle:Error, request for invalid systolic interconnect %d\n",
	      sys_no);
      return(NULL);
    }
}

//////////////////////////////////////////////////////////////////
// Given two device numbers, determine a unique systolic connector
// FIX:Need to be able to resolve amongst solutions
// FIX:Connection only handles single point to point case
//////////////////////////////////////////////////////////////////
int Arch::determine_systolic_handle(const int fpga1, const int fpga2)
{
  for (int loop=0;loop<systolic_count;loop++)
    {
      ACSIntArray* sys_connection=systolic_connections[loop];
      int first_con=sys_connection->query(0);
      int second_con=sys_connection->query(1);
      if (fpga1==first_con)
	if (fpga2==second_con)
	  return(loop);
      if (fpga2==first_con)
	if (fpga1==second_con)
	  return(loop);
    }

  // Troubles
  fprintf(stderr,"Arch::determine_systolic_handle:Error, no systolic connection found\n");
  abort();
}


////////////////////////////////////////////////
// Set the fpga for usage in this implementation
////////////////////////////////////////////////
int Arch::unassign_all()
{
  for (int fpga_loop=0;fpga_loop<fpga_count;fpga_loop++)
    if (fpgas[fpga_loop].usage_state!=FPGA_RESERVED)
      fpgas[fpga_loop].usage_state=FPGA_UNUSED;

  for (int mem_loop=0;mem_loop<mem_count;mem_loop++)
    {
      Port* mem=get_memory_handle(mem_loop);
      mem->reset_cores();
    }

  for (int sys_loop=0;sys_loop<systolic_count;sys_loop++)
    {
      Port* sys=get_systolic_handle(sys_loop);
      sys->reset_cores();
    }

  // Return happy condition
  return(1);
}


///////////////////////////////////////////////////////////////////
// Given a memory handle, find a route to an fpga and return its id
// FIX:This will simply return the first connected memory
///////////////////////////////////////////////////////////////////
int Arch::mem_to_fpga(const int mem_id)
{
  int fpga_cons=mem_ports[mem_id].query_fpga_connections();
  for (int loop=0;loop<fpga_cons;loop++)
    {
      Fpga* fpga_elem=mem_ports[mem_id].get_fpga_handle(loop);
      return(fpga_elem->fpga_id);
    }

  // Trouble
  fprintf(stderr,"Arch::mem_to_fpga:Error, unable to correlate memory with an fpga\n");
  abort();
}


////////////////////////////////////////////////////////////////////////
// Root method for determining all inter-fpgas routes between a starting
// and ending fpga number
////////////////////////////////////////////////////////////////////////
void Arch::find_routes(const int start_fpga, const int end_fpga)
{
  wipe_solution();

  if (DEBUG_ROUTES)
    printf("find_routes looking between fpga %d and fpga %d\n",
	   start_fpga,
	   end_fpga);
  runaway=0;
  total_solutions=0;
  src_fpga=start_fpga;
  dest_fpga=end_fpga;
  current_solution=new ACSIntArray;
  current_solution->add(start_fpga);
  find_route(src_fpga);
}

////////////////////////////////////////////////////////////////
// Return an integer array indicating which fpgas should be used
// for interconnecting between source and destination fpgas
// FIX: Need to test to see if route can be shared/used
////////////////////////////////////////////////////////////////
void Arch::find_route(const int start_fpga)
{
  if (DEBUG_ROUTES)
    printf("finding route based from %d\n",start_fpga);

  //
  // Watch for out of control situations
  //
  if (runaway>100)
    return;
  else
    runaway++;

  //
  // Track options
  //
  ACSIntArray* fpga_attempt=new ACSIntArray(fpga_count,0);

  // Constrain search space, removing current, reserved and previous fpgas
  fpga_attempt->set(src_fpga,1);
  fpga_attempt->set(start_fpga,1);
  for (int fpga_no=0;fpga_no<fpga_count;fpga_no++)
    if (fpgas[fpga_no].usage_state==FPGA_RESERVED)
      {
	fpga_attempt->set(fpga_no,1);
	if (DEBUG_ROUTES)
	  printf("fpga %d is reserved..excluding\n",fpga_no);
      }
  for (int sol_loop=0;sol_loop<current_solution->population();sol_loop++)
    {
      int fpga_sol=current_solution->get(sol_loop);
      fpga_attempt->set(fpga_sol,1);
      if (DEBUG_ROUTES)
	printf("fpga %d is part of current solution..excluding\n",fpga_sol);
    }

  //
  // Do these fpgas connect?
  //
  while (!fpga_attempt->full(1))
    {
      // Find the first untested fpga in the architecture
      int next_fpga=fpga_attempt->find_index(0);
      if (DEBUG_ROUTES)
	printf("will see if %d is connected to %d\n",next_fpga,start_fpga);

      fpga_attempt->set(next_fpga,1);
      if (fpgas[start_fpga].test_route(next_fpga))
	{
	  // If it is our destination, then we have a solution...add it
	  if (next_fpga==dest_fpga)
	    {
	      current_solution->add(dest_fpga);
	      add_solution();
	    }
	  else
	    {
	      // Otherwise, a good start, add it to our solution list and 
	      // search from here
	      current_solution->add(next_fpga);
	      if (DEBUG_ROUTES)
		printf("is connected, will see if %d connects to dest %d\n",
		       next_fpga,dest_fpga);
	      
	      find_route(next_fpga);
	      
	      if (DEBUG_ROUTES)
		fpga_attempt->print("fpga_attempts");

	      current_solution->pop();
	    }
	}
    }
  delete fpga_attempt;
}

void Arch::add_solution(void)
{
  if (DEBUG_ROUTES)
    current_solution->print("Solution found");
  ACSIntArray** new_solutions=new ACSIntArray*[total_solutions+1];
  for (int swap_loop=0;swap_loop<total_solutions;swap_loop++)
    new_solutions[swap_loop]=route_solutions[swap_loop];

  new_solutions[total_solutions]=new ACSIntArray;
  for (int swap_loop=0;swap_loop<current_solution->population();swap_loop++)
    new_solutions[total_solutions]->add(current_solution->query(swap_loop));

  route_solutions=new_solutions;
  total_solutions++;
}


/////////////////////////////////////////////////////////////////////
// Examine all I/O ports looking for ways of compressing data signals
/////////////////////////////////////////////////////////////////////
int Arch::assess_io(void)
{
  for (int loop=0;loop<mem_count;loop++)
    {
      Port* mem_port=get_memory_handle(loop);
      if ((mem_port->total_sgs > 1) && ((mem_port->source_cores)->size() > 1)
	  && ((mem_port->sink_cores)->size() > 1))
	{
	  int rc=mem_port->assess_port();
	  if ((rc) && (DEBUG_ARCH))
	    printf("Memory %d, will utilize bit-packing to reduce latency\n",loop);
	}
    }

  for (int loop=0;loop<systolic_count;loop++)
    {
      Port* sys_port=get_systolic_handle(loop);
      if (sys_port->total_sgs > 2)
	{
	  int rc=sys_port->assess_port();
	  if (!rc)
	    {
	      if (DEBUG_ARCH)
		printf("Systolic interconnect %d, unable to bit-pack\n",loop);

	      // Currently unable to support this capability:(
	      return(0);
	    }
	  else
	    if (DEBUG_ARCH)
	      printf("Systolic interconnect %d, will utilize bit-packing to reduce latency\n",loop);
	}
    }
  
  // Return happy condition
  return(1);
}

/////////////////////////////////////////////////////////////////////////////////////
// If a port can be compressed, return a list of handles for the respective port_type
/////////////////////////////////////////////////////////////////////////////////////
ACSIntArray* Arch::compressed_io(const int type)
{
  ACSIntArray* packed_ports=new ACSIntArray;
  if (type==MEMORY)
    for (int loop=0;loop<mem_count;loop++)
      {
	Port* mem_port=get_memory_handle(loop);
	if ((mem_port->sources_packed) || (mem_port->sinks_packed))
	  packed_ports->add(loop);
      }
  else if (type==SYSTOLIC)
    for (int loop=0;loop<systolic_count;loop++)
      {
	Port* sys_port=get_systolic_handle(loop);
	if ((sys_port->sources_packed) || (sys_port->sinks_packed))
	  packed_ports->add(loop);
      }
  else
    delete packed_ports;

      
  // Return list, NULL if invalide
  return(packed_ports);
}


/////////////////////////////////////////////////////////////////////////////////////
// This will determine where the io bottleneck occurs for the algorithm configuration
// and return the effective launch rate as a result of this
/////////////////////////////////////////////////////////////////////////////////////
int Arch::calc_launchrate(void)
{
  launch_rate=0;

  ///////////////////////
  // Examine the memories
  ///////////////////////
  int worst_memory=-1;
  for (int loop=0;loop<mem_count;loop++)
    {
      Port* mem_port=get_memory_handle(loop);
      if (mem_port->port_active())
	{
	  CoreList* source_cores=mem_port->source_cores;
	  CoreList* sink_cores=mem_port->sink_cores;

	  if (DEBUG_LAUNCHRATE)
	    printf("memory %d has %d sources out of %d total\n",
		   loop,source_cores->size(),mem_port->total_sgs);

	  // If the port hits are homogenous then a launch rate of one can be achieved
	  int source_count=source_cores->size();
	  int sink_count=sink_cores->size();
	  if ((source_count > 0) && (sink_count > 0))
	    {
	      int memory_rate=mem_port->total_sgs+mem_port->read_skew;
	      if (memory_rate > launch_rate)
		{
		  launch_rate=memory_rate;
		  worst_memory=loop;
		  if (DEBUG_LAUNCHRATE)
		    printf("launch_rate increased to %d due to memory to %d\n",launch_rate,loop);
		}
	    }
	  else // HOMOGENEOUS
	    {
	      if (DEBUG_LAUNCHRATE)
		printf("memory is homogeneous\n");
	      if ((mem_port->sources_packed) || (mem_port->sinks_packed))
		{
		  if (DEBUG_LAUNCHRATE)
		    printf("memory was packed\n");
		  if (launch_rate==0)
		    {
		      launch_rate=1;
		      worst_memory=loop;
		    }
		}
	      else
		if (mem_port->total_sgs > launch_rate)
		  {
		    launch_rate=mem_port->total_sgs;
		    worst_memory=loop;
		  }
	    }
	}
    }
  Port* mem_port=get_memory_handle(worst_memory);
  mem_slice=(int) floor(mem_port->get_highaddress()/launch_rate);

  ///////////////////////////////////////////////////////////////////////////
  // Examine the systolic interconnects
  // FIX: If given a sense of polarity, the systolic interconnect launch rate
  //      can be reduced if homogeneous polarities are detected.
  // FIX: Activate once systolic MUXing is supported
  ///////////////////////////////////////////////////////////////////////////
  if (0)
    for (int loop=0;loop<systolic_count;loop++)
      {
	Port* sys_port=get_systolic_handle(loop);
	
	// Divide the port hit count by a factor of two since a single
	// port hit represents two cores.  i.e., one data stream
	int port_hits=sys_port->total_sgs/2+sys_ports->write_skew;
	if (DEBUG_LAUNCHRATE)
	  printf("port %d, has %d port hits\n",loop,port_hits);
	
	if (port_hits > launch_rate)
	  {
	    launch_rate=port_hits;
	    if (DEBUG_LAUNCHRATE)
	      printf("launch_rate increased due to systolic to %d\n",launch_rate);
	  }
      }
  
  // Return the net rate
  return(launch_rate);
}

////////////////////////////////////////////////////////////////////////////////////
// Determine if automatic address generation is needed, as requested by the memories
////////////////////////////////////////////////////////////////////////////////////
int Arch::query_addressing(void)
{
  for (int loop=0;loop<mem_count;loop++)
    {
      Port* mem_port=get_memory_handle(loop);
      if ((mem_port->port_active()) && (mem_port->query_addressing()))
	return(1);
    }
  
  // No memory port is requesting automatic addressing
  return(0);
}


///////////////////////////////////////////////////////////////////////////////
// For each used fpga in the architecture for this design, generate a sequencer
///////////////////////////////////////////////////////////////////////////////
void Arch::generate_sequencer(void)
{
  if (DEBUG_ARCH)
    printf("arch::generate_sequencer\n");
  for (int fpga_no=0;fpga_no<fpga_count;fpga_no++)
    {
      if (DEBUG_ARCH)
	printf("arch::generate_sequencer checking fpga %d\n",fpga_no);
      Fpga* fpga_elem=get_fpga_handle(fpga_no);
      if (fpga_elem->usage_state==FPGA_USED)
	{
	  if (DEBUG_ARCH)
	    printf("arch::generate_sequencer fpga %d is used\n",fpga_no);
	  fpga_elem->generate_sequencer();
	}
    }
}

void Arch::print_arch(const char* title)
{
  printf("Architecture %s: has %d Fpgas\n",title,fpga_count);
  for (int loop=0;loop<fpga_count;loop++)
    {
      printf("Fpga %d:\n",loop);
      fpgas[loop].print_fpga();
      fpgas[loop].print_children();
    }
}
