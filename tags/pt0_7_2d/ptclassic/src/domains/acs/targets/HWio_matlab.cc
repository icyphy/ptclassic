static const char file_id[] = "HWio_matlab.cc";

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
 Date of creation: 10/25/99
 Version: @(#)HWio_matlab.cc      1.0     10/25/99
***********************************************************************/
#include "HWio_matlab.h"

#ifdef __GNUG__
#pragma implementation
#endif

/////////////////////////////////////////////////////////////////////////////
// Generate a netlist and supporting files for the matlab wordlength analysis
// tool.
/////////////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWnetlist_export(void)
{
  // Establish filenames and open file stream
  char* root_filename=new char[MAX_STR];
  strcpy(root_filename,galaxy()->name());

  ostrstream of_filename, types_filename, pnames_filename, del_filename;
  of_filename << design_directory->retrieve_extended() << root_filename 
	      << "_netlist.txt" << ends;
  types_filename << design_directory->retrieve_extended() << root_filename 
		 << "_types.txt" << ends;
  pnames_filename << design_directory->retrieve_extended() << root_filename
		  << "_pnames.txt" << ends;
  del_filename << design_directory->retrieve_extended() << "delchain_cost.m" << ends;

  ofstream net_file(of_filename.str());
  ofstream type_file(types_filename.str());
  ofstream pname_file(pnames_filename.str());
  ofstream del_file(del_filename.str());

  // Export execution script
  HWexport_wgscript();

  // Export version information
  net_file << HWexport_netversion();

  int total_sgs=smart_generators->size();

  // Export all algorithmic netlist information for sources and sinks first
  for (int sloop=0;sloop<total_sgs;sloop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(sloop);
      
      // Generate type entry for each smart generator
      type_file << fpga_core->name() << endl;
      
      // Trap for Sources and Sinks
      if (fpga_core->acs_type==SOURCE)
	{
	  // Generate netlist entry for this smart generator
	  net_file << HWexport_net(fpga_core,0,fpga_core->acs_type);
	}
    }
  for (int sloop=0;sloop<total_sgs;sloop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(sloop);
      
      // Generate type entry for each smart generator
      type_file << fpga_core->name() << endl;
      
      // Trap for Sources and Sinks
      if (fpga_core->acs_type==SINK)
	{
	  // Generate netlist entry for this smart generator
	  net_file << HWexport_net(fpga_core,0,fpga_core->acs_type);
	}
    }


  // Export all algorithmic netlist information for algorithmic smart generators
  int id=1;
  for (int sloop=0;sloop<total_sgs;sloop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(sloop);
      
      // Tell each smart generator to create matlab scripts
      // dont need to do this for Sources or Sinks
      if ((fpga_core->acs_type==BOTH) || (fpga_core->acs_type==SOURCE_LUT))
	{
	  fpga_core->sg_costfunc(id);
	  
	  // Generate type entry for each smart generator
//	  type_file << fpga_core->name() << endl;
      
	  // Generate netlist entry for this smart generator
	  net_file << HWexport_net(fpga_core,
				   id,
				   SINK);
	  net_file << HWexport_net(fpga_core,
				   id,
				   SOURCE);
	  id++;
	}
    }
  
  // Generate pin names for all (unique) netlist nodes
  pname_file << HWexport_pnames();

  // Generate a matlab-cost file for adding delay chains
  del_file << HWexport_delaychain();

  // Wrapup
  net_file.close();
  type_file.close();
  pname_file.close();
  del_file.close();

  // Return happy condition
  return(1);
}


/////////////////////////////////////////////////////////////////
// Export a matlab-compatible script for kickoff execution of the
// wordlength analysis tool
/////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWexport_wgscript(void)
{
  // Establish filenames
  char* root_filename=new char[MAX_STR];
  strcpy(root_filename,galaxy()->name());

  // Determine where Ptolemy is located
  char* environ_path=getenv("PTOLEMY");

  // Generate output file structures
  ostrstream script_filename;
  script_filename << design_directory->retrieve_extended() 
		  << "wlstart.m" << ends;
  ofstream script_file(script_filename.str());

  // Add path to the wordgui tool
  script_file << "% Path to word length analysis tool" << endl;
  script_file << "addpath " << environ_path
	      << "/src/domains/acs/utils/wordlength" << endl << endl;

  // Add path to the design and design name
  script_file << "% Path to the generated design and the name of the design" 
	      << endl;
  script_file << "pathname='" << design_directory->retrieve_extended() 
	      << "';" << endl;
  script_file << "dsnname='" << root_filename << "';" << endl << endl;

  // Add automatic execution information
  script_file << "% Automatic execution script" << endl;
  script_file << "cd(pathname);" << endl;
  script_file << "wordgui" << endl;

  // Add instructions message (more'd)
  script_file << "% List instructions for running the tool" << endl;
  script_file << "fprintf('\\n\\nWordgui instructions can be found in\\n');" 
	      << endl;
  script_file << "fprintf('" << environ_path 
	      << "/src/domains/acs/utils/wordlength/README.TXT\\n');" << endl;

  // Wrapup
  script_file.close();
}



///////////////////////////////////////////////
// Version control export for the word gui tool
///////////////////////////////////////////////
char* ACSCGFPGATarget::HWexport_netversion(void)
{
  static const char netlist_vname[] = "% Netlist V";
  ostrstream expression;
  
  expression << netlist_vname << netlist_major_version << "." 
	     << netlist_minor_version << endl;

  // Make sure the version number entry below matches the char version above!!!
  expression << netlist_major_version << "." << netlist_minor_version 
	     << " 0 0 0 0 0 0 0 0 0 0 0 0 0 0" << endl;
  expression << ends;
  // Return happy condition
  return(expression.str());
}

////////////////////////////////
// Export a single netlist entry
////////////////////////////////
char* ACSCGFPGATarget::HWexport_net(ACSCGFPGACore* fpga_core,
				    int sg_id,
				    const int node_type)
{
  Pin* localp=fpga_core->pins;
  ACSIntArray* the_nodes=NULL;
  if (node_type==SOURCE)
    the_nodes=localp->query_onodes();
  else
    the_nodes=localp->query_inodes();

  
  // FIX: Core use of YES/NO implies 1/0 interpretation.
  int device_lock=fpga_core->intparam_query("Device_Lock");

  ostrstream expression;

  int node_count=the_nodes->population();

  for (int loop=0;loop<node_count;loop++)
    {
      int pin_no=(int) the_nodes->query(loop);
      if (localp->query_pinassigned(pin_no)!=UNASSIGNED)
	{
	  expression << sg_id << " "
		     << fpga_core->acs_id << " ";
	  if ((fpga_core->acs_type==BOTH) || (fpga_core->acs_type==SOURCE_LUT))
	    expression << fpga_core->acs_device << " " << "-1 ";
	  else
	    expression << "-1 " << fpga_core->acs_device << " ";
	  
	  // Standard netlist information
	  expression << device_lock << " "
		     << node_type << " " 
		     << localp->query_netlistid(pin_no) << " " 
		     << "0 " 
		     << localp->query_majorbit(pin_no) << " "
		     << localp->query_bitlen(pin_no) << " "
		     << localp->query_wordlock(pin_no) << " "
		     << localp->query_wordcount(pin_no) << " ";
//		 << vector_length << " ";
	  
	  // Stubs for scheduling information (ignored by wordgui)
	  // FIX: One zero gets dropped out for some stupid reason, an extra has been
	  //      added to pad this thing:(
	  expression << "0 0 0" << endl;
	}
    }

  expression << ends;
  
  // Return happy condition
  return(expression.str());
}

////////////////////////////////////////////////////////////////////////////
// This function will generate a readable format of information for 
// visually translating wordgui information to elements in the Ptolemy graph
////////////////////////////////////////////////////////////////////////////
char* ACSCGFPGATarget::HWexport_pnames()
{
  ostrstream expression;
  ACSIntArray* unique_netids=new ACSIntArray;

  int total_sgs=smart_generators->size();
  for (int sloop=0;sloop<total_sgs;sloop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(sloop);
      Pin* localp=fpga_core->pins;

      ACSIntArray* the_nodes=localp->query_onodes();
      int node_count=the_nodes->population();
      for (int loop=0;loop<node_count;loop++)
	{
	  int pin_no=(int) the_nodes->query(loop);
	  int netlistid=localp->query_netlistid(pin_no);
	  if (!unique_netids->find(netlistid))
	    {
	      unique_netids->add(netlistid);
	      char* corona_name=(char*) localp->query_cpinname(pin_no);
	      expression << "Node:" << netlistid << " comes from smart generator " 
			 << fpga_core->comment_name() << " via Ptolemy port "
			 << corona_name << endl;
	    }
	}
    }

  expression << ends;

  // Cleanup
  
  // Return happy condition
  return(expression.str());
}


////////////////////////////////////////////////////////////////////////////////
// This function will generate a matlab cost file relating to the cost of adding
// delays to an algorithm
// FIX: Should request information from the Arch instance for more info on costs
//      rather than this hard-coded for XC4000
////////////////////////////////////////////////////////////////////////////////
char* ACSCGFPGATarget::HWexport_delaychain(void)
{
  ostrstream expression;

  // Function preamble
  expression << "function [cost,sd,so]=delchain_cost(outsizes,delamts,numforms)" << endl
	     << "% Delay chain cost for xc4000" << endl
	     << "% sd = incremental delays" << endl
	     << "% so = incremental width" << endl;

  // Meaty bits
  expression << "len=size(delamts,2);" << endl
	     << "if (numforms(1)==0)" << endl
	     << "\t% parallel fixed" << endl
	     << "\t[sd,inds]=sort(delamts); % sorted" << endl
	     << "\tfor k=1:len" << endl
	     << "\t\tso(:,k)=outsizes(inds(:,k),k); % sorted outsizes" << endl
	     << "\tend" << endl << endl
	     << "\t% reverse cumulative max" << endl
	     << "\tfor k=size(so,1)-1:-1:1" << endl
	     << "\t\tso(k,:)=max(so(k+1,:),so(k,:));" << endl
	     << "\tend" << endl << endl
	     << "\tsd=[sd(1,:);diff(sd)]; %ds has amt of dels, so has bit width" << endl
	     << "\tbd1=sd.*so/2; %clb's req'd building discretely" << endl
	     << "\tt=find(sd==0);" << endl
	     << "\tsd(t)=1;" << endl
	     << "\tbd2=so.*ceil(sd/16)/2+ceil(log2(sd))/2;" << endl
	     << "\tbd2(t)=0; % the neg inf problem" << endl
	     << "\tcost=sum(min(bd1,bd2)); % total clbs" << endl
	     << "else" << endl
	     << "\tdisp('****Error: delchain_cost****');" << endl
	     << "end" << endl
	     << "return;" << endl;
  
  expression << ends;

  // Cleanup
  
  // Return happy condition
  return(expression.str());
}


/////////////////////////////////////////////////////////////////////
// Halt the target until wordlength analysis is completed by the user
/////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWpause_for_wordgui(void)
{
  int pid;
  int wait_stat=512;
  int* wait_ptr=&wait_stat;

  if ((pid=fork())==0)
    {
      char* wordgui_command=new char[MAX_STR];
      char* environ_path=getenv("PTOLEMY");
      sprintf(wordgui_command,"%s/src/domains/acs/utils/wordgui_pause.tk",
	      environ_path);

      chdir(design_directory->retrieve());
      if (execlp("itkwish","itkwish",wordgui_command,(char*) 0)==-1)
	printf("HWpause_for_wordgui:Unable to spawn a wish interpreter\n");

      // Cleanup
      delete []wordgui_command;
    }
  else
    {
      int done=0;
      while (!done)
	{
	  int w_pid=wait (wait_ptr);
	  if (DEBUG_WAIT)
	    printf("Wait triggered for pid %d, received %d. waiting for pid %d\n"
		   ,w_pid,wait_stat,pid);
	  if (w_pid==pid)
	    if ((wait_stat==0) || (wait_stat==256))
	      done=1;
	}
    }
  return(wait_stat);
}

/////////////////////////////////////////////////////////////////////////////
// Read in the resultant netlist from the wordlength analysis tool.  Prohibit
// updates if no new netlist was generated
/////////////////////////////////////////////////////////////////////////////
void ACSCGFPGATarget::HWrevise(CoreList* sg_list)
{
  if (HWnetlist_import())
    {
      HWrevise_const(sg_list);
      HWrevise_fixed(sg_list);
      HWrevise_pipes(sg_list);
    }
  else
    printf("WARNING:No netlist generated by wordgui, bit widths not revised\n");
}

////////////////////////////////////////////////
// Retrieve the netlist from wordlength analysis
////////////////////////////////////////////////
int ACSCGFPGATarget::HWnetlist_import(void)
{
  // Establish filenames and open file stream
  char* root_filename=new char[MAX_STR];
  strcpy(root_filename,galaxy()->name());

  ostrstream net_filename;
  net_filename << design_directory->retrieve_extended() << root_filename 
	       << "_outnetlist.txt" << ends;
  delete []root_filename;
  
  ifstream net_file(net_filename.str());
  if (net_file.bad())
    return(0);

  //
  // Storage for modified netlist info 
  //
  char* netlist_buf=new char[MAX_STR];
  ACSIntArray* star_ids=new ACSIntArray;
  ACSIntArray* acs_ids=new ACSIntArray;
  ACSIntArray* fpga_nos=new ACSIntArray;
  ACSIntArray* ram_nos=new ACSIntArray;
  ACSIntArray* dev_locks=new ACSIntArray;
  ACSIntArray* node_types=new ACSIntArray;
  ACSIntArray* node_nums=new ACSIntArray;
  ACSIntArray* fixedfloats=new ACSIntArray;
  ACSIntArray* major_bits=new ACSIntArray;
  ACSIntArray* bitlens=new ACSIntArray;
  ACSIntArray* locks=new ACSIntArray;
  ACSIntArray* veclens=new ACSIntArray;

  //
  // Storage for modified scheduling info 
  //
  ACSIntArray* start_times=new ACSIntArray;
  ACSIntArray* step_times=new ACSIntArray;
  ACSIntArray* end_times=new ACSIntArray;

  // Extract header info from the netlist file
  net_file.getline(netlist_buf,MAX_STR);  // Strip text header
  net_file.getline(netlist_buf,MAX_STR);  // Fetch version number
  istrstream vers_buf(netlist_buf,MAX_STR);

  float current_version=netlist_major_version+(.1*netlist_minor_version);
  float retrieved_version;
  vers_buf >> retrieved_version;
  if (current_version!=retrieved_version)
    {
      sprintf(err_message,
	      "Error:Imported netlist version %f is incompatible (req:V%f)\n",
	      retrieved_version,
	      current_version);
      Error::error(*this,err_message);
      return(0);
    }

  while (net_file.getline(netlist_buf,MAX_STR))
    {
      // Temporary int handlers
      int star_id, acs_id, fpga_no, ram_no, dev_lock;
      int node_type, node_num, fixedfloat;
      int major_bit, bitlen, lock, veclen;
      int start_time, step_time, end_time;
      
      // Retrieve a netlist
      istrstream istr_buf(netlist_buf,MAX_STR);
      istr_buf >> star_id >> acs_id >> fpga_no >> ram_no >> dev_lock
	       >> node_type >> node_num >> fixedfloat 
	       >> major_bit >> bitlen >> lock >> veclen 
	       >> start_time >> step_time >> end_time;
      if (DEBUG_REVISE)
	printf("net:%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
	       star_id,acs_id,
	       fpga_no,ram_no,dev_lock,node_type,node_num,fixedfloat,
	       major_bit,bitlen,lock,veclen,
	       start_time,step_time,end_time);

      // Store it for later perusal
      star_ids->add(star_id);
      acs_ids->add(acs_id);
      fpga_nos->add(fpga_no);
      ram_nos->add(ram_no);
      dev_locks->add(dev_lock);     // Not used
      node_types->add(node_type);   // Not used
      node_nums->add(node_num);
      fixedfloats->add(fixedfloat); // Not used
      major_bits->add(major_bit);
      bitlens->add(bitlen);
      locks->add(lock);
      veclens->add(veclen);         // Not used

      // Retrieve the netlist's scheduling information
      start_times->add(start_time);
      step_times->add(step_time);
      end_times->add(end_time);
    }
  int total_nets=star_ids->population();

  // Update the smart generators
  int prev_node=-1;
  int prev_acsid=-1;
  int pin_index=1;
  for (int net_loop=0;net_loop<total_nets;net_loop++)
    {
      ACSCGFPGACore* fpga_core=HWfind_star(acs_ids->query(net_loop));
      if (DEBUG_REVISE)
	printf("Examining netrow %d, for star %s\n",
	       net_loop,fpga_core->comment_name());

      // Reset core parameters
      int read_skew=0;
      int write_skew=0;
      if (fpga_core->acs_type==BOTH)
	fpga_core->acs_device=fpga_nos->query(net_loop);
      else
	{
	  // FIX:Mapping of fpga and RAM bad mojo!
	  int ram_no=-1;
	  if (fpga_core->acs_type==SOURCE_LUT)
	    ram_no=fpga_nos->query(net_loop);
	  else
	    ram_no=ram_nos->query(net_loop);
	  fpga_core->acs_device=ram_no;
	  Port* mem_port=arch->get_memory_handle(ram_no);
	  read_skew=mem_port->read_skew;
	  write_skew=mem_port->write_skew;
	}
	  
      // Reset pin parameters
      Pin* fpga_pins=fpga_core->pins;
      int node=node_nums->query(net_loop);

//      int pin_count=fpga_pins->pins_withnetlist(node);
      // If multiple inputs with the same node, bitwidths may be different, need to trap
      // for dissimilar precisions on the same node!
      if ((prev_node==node) && (prev_acsid==acs_ids->query(net_loop)))
	pin_index++;
      else
	{
	  prev_node=node;
	  prev_acsid=acs_ids->query(net_loop);
	  pin_index=1;
	}

      int pin_no=fpga_pins->pin_withnetlist(node,pin_index);
      if (DEBUG_REVISE)
	printf("Node #%d corresponds to sg's %s pin #%d\n",
	       node,fpga_core->comment_name(),pin_no);
      
      // Update precision if necessary
      int old_mbit=fpga_pins->query_majorbit(pin_no);
      int old_blen=fpga_pins->query_bitlen(pin_no);
      int new_mbit=major_bits->query(net_loop);
      int new_blen=bitlens->query(net_loop);
      if ((old_mbit != new_mbit) || (old_blen != new_blen))
	{
	  if (DEBUG_REVISE)
	    printf("SG %s, precision changed from (%d,%d) to (%d,%d)\n",
		   fpga_core->comment_name(),
		   old_mbit,old_blen,
		   new_mbit,new_blen);
	  fpga_pins->set_precision(pin_no,new_mbit,new_blen,LOCKED);

	  // Recalculate pipeline size
	  fpga_core->sg_delays();
	}
      else
	fpga_pins->set_preclock(pin_no,LOCKED);

      // Reset scheduling info
      // FIX:If memories, first trap for, but do skews need to be
      //     accounted for here?
      if (fpga_pins->pin_classtype(pin_no)==OUTPUT_PIN)
	{
	  fpga_core->act_output=start_times->query(net_loop);
	  fpga_core->act_launch=step_times->query(net_loop);
	  fpga_core->act_complete=end_times->query(net_loop);
	  fpga_core->word_count=veclens->query(net_loop);
	  if ((fpga_core->acs_type==BOTH) || (fpga_core->acs_type==SOURCE_LUT))
	    fpga_core->act_input=fpga_core->act_output-fpga_core->acs_delay;
	  else
	    {
	      fpga_core->act_input=fpga_core->act_output;
		
	      // Adjust source/sink firing time since not returned from wordgui tool
	      if (fpga_core->acs_type==SOURCE)
		fpga_core->act_output+=read_skew;
	      else
		fpga_core->act_output+=write_skew;
	      
	    }
	  if (DEBUG_REVISE)
	    printf("Output node, core times set to [%d %d %d], input time is %d\n",
		   fpga_core->act_output,
		   fpga_core->act_launch,
		   fpga_core->act_complete,
		   fpga_core->act_input);
	}
      else
	{
	  if (fpga_core->acs_type==SINK)
	    {
	      fpga_core->act_input=start_times->query(net_loop)+write_skew;
	      fpga_core->act_output=fpga_core->act_input;
	      fpga_core->act_launch=step_times->query(net_loop);
	      fpga_core->act_complete=end_times->query(net_loop)+write_skew;
	      fpga_core->word_count=veclens->query(net_loop);

	      if (DEBUG_REVISE)
		printf("Sink node, core times set to [%d %d %d], input time is %d\n",
		       fpga_core->act_output,
		       fpga_core->act_launch,
		       fpga_core->act_complete,
		       fpga_core->act_input);
	    }
	  
	}
      
    }

  // Have each smart generator re-examine their precisions
  int sg_count=smart_generators->size();
  for (int loop=0;loop<sg_count;loop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) smart_generators->elem(loop);
      fpga_core->sg_bitwidths(LOCKED);
    }

  // Cleanup
  net_file.close();

  delete []netlist_buf;
  delete star_ids;
  delete acs_ids;
  delete fpga_nos;
  delete ram_nos;
  delete dev_locks;
  delete node_types;
  delete node_nums;
  delete fixedfloats;
  delete major_bits;
  delete bitlens;
  delete locks;

  delete start_times;
  delete step_times;
  delete end_times;

  // Return happy condition
  return(1);
}


// FIX: Unnecessary, trap moved to HWsynth_pipealigns
int ACSCGFPGATarget::HWrevise_const(CoreList* sg_list)
{
  // Return happy condition
  return(1);
}


////////////////////////////////////////////////////////////////////
// Translate new precision types back to Ptolemy precisions for each
// smart generator/star
////////////////////////////////////////////////////////////////////
int ACSCGFPGATarget::HWrevise_fixed(CoreList* sg_list)
{
  // Establish filenames and open file stream
  char* root_filename=new char[MAX_STR];
  strcpy(root_filename,galaxy()->name());

  ostrstream recipe_filename;
  recipe_filename << design_directory->retrieve_extended() << root_filename
		  << "_recipe.txt" << ends;
  ofstream recipe_file(recipe_filename.str());

  delete []root_filename;

  int total_sgs=sg_list->size();
  for (int sloop=0;sloop<total_sgs;sloop++)
    {
      // Allocate local storage
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(sloop);
      ACSCorona& fpga_corona=fpga_core->getCorona();
      Pin* fpga_pins=fpga_core->pins;
      ACSIntArray* inodes=fpga_pins->query_inodes();
      ACSIntArray* onodes=fpga_pins->query_onodes();
      StringArray* input_list=new StringArray;
      StringArray* output_list=new StringArray;

      // Start a recipe entry for this smart generator
      recipe_file << "Smart generator " << fpga_core->comment_name()
		  << " updated precisions should be: " << endl;

      // Obtain parameter name info from each smart generator
      fpga_core->sg_param_query(input_list,output_list);

      if (inodes->population() > 0)
	HWrevise_pins(fpga_corona,fpga_core,inodes,input_list,0,SINK,
		      recipe_file);
      if (onodes->population() > 0)
	HWrevise_pins(fpga_corona,fpga_core,onodes,output_list,
		      input_list->population(),SOURCE,recipe_file);

      // Cleanup
      delete input_list;
      delete output_list;
    }
  
  // Return happy condition
  return(1);
}

///////////////////////////////////////////////////////
// Revise the internal precision on a pin-by-pin basis.
// Attempt to revise the ptolemy-based parameter field.
// FIX: Revise the ptolemy-based parameter field.
///////////////////////////////////////////////////////
int ACSCGFPGATarget::HWrevise_pins(ACSCorona& fpga_corona,
				   ACSCGFPGACore* fpga_core,
				   ACSIntArray* nodes,
				   StringArray* state_names,
				   int state_offset,
				   char mode,
				   ofstream& recipe_file)
{
  // Allocate space for state name queries
  char* fixed_statename=new char[MAX_STR];
  char* new_precision=new char[MAX_STR];
  if (mode==SINK)
    strcpy(fixed_statename,"InputPrecision");
  else
    strcpy(fixed_statename,"OutputPrecision");

  Pin* fpga_pins=fpga_core->pins;
  int node_count=nodes->population();

  // Accumlator for single ptolemy precision representation
  int major_bit=-INT_MAX;
  int bitlen=-INT_MAX;

  // Update "new" precision fields and determine a conversion to
  // the Ptolemy fixed precision
  for (int node_loop=0;node_loop<node_count;node_loop++)
    {
      int pin_no=nodes->query(node_loop);
      int q_majorbit=fpga_pins->query_majorbit(pin_no);
      int q_bitlen=fpga_pins->query_bitlen(pin_no);

      int state_index=(pin_no*2)-state_offset;
      char* mbit_statename=(char*) state_names->get(state_index);
      if (DEBUG_REVISE)
	printf("sg %s retrieved mbit state name of %s (%d)\n",
	       fpga_core->comment_name(),
	       mbit_statename,
	       state_index);
      State* majorbit_state=fpga_core->stateWithName(mbit_statename);
      if (majorbit_state != NULL)
	{
	  sprintf(new_precision,"%d",q_majorbit);
	  majorbit_state->setCurrentValue(new_precision);
	}
      else
	if (DEBUG_REVISE)
	  printf("HWrevise_pins:Warning, state %s froms star %s not found\n",
		 mbit_statename,
		 fpga_core->comment_name());
      recipe_file << "\t" << mbit_statename << "=" << q_majorbit << endl;

      state_index=(pin_no*2)+1-state_offset;
      char* bitlen_statename=(char*) state_names->get(state_index);
      if (DEBUG_REVISE)
	printf("sg %s retrieved bitlen state name of %s (%d)\n",
	       fpga_core->comment_name(),
	       bitlen_statename,
	       state_index);
      State* bitlen_state=fpga_core->stateWithName(bitlen_statename);
      if (bitlen_state != NULL)
	{
	  sprintf(new_precision,"%d",q_bitlen);
	  bitlen_state->setCurrentValue(new_precision);
	}
      else
	if (DEBUG_REVISE)
	  printf("HWrevise_pins:Warning, state %s froms star %s not found\n",
		 bitlen_statename,
		 fpga_core->comment_name());
      recipe_file << "\t" << bitlen_statename << "=" << q_bitlen << endl;

      // Test for mapping problem between precisions
      if (q_majorbit > major_bit)
	major_bit=q_majorbit;
      if (q_bitlen > bitlen)
	bitlen=q_bitlen;
    }

  // Bypass Ptolemy precisions for SINK stars (not needed)
  if (fpga_core->acs_type==SINK)
    return(1);

  // Trap for major_bit underflow
  if (major_bit < -1)
    {
      printf("Warning:Majorbit value %d cannot be represented in ",major_bit);
      printf("Ptolemy format, will pad to match\n");
      int delta=1-major_bit;
      major_bit=1;
      bitlen+=delta;
    }

  // Update fixed point precision fields
  State* precision_state=fpga_core->stateWithName(fixed_statename);
  if (precision_state!=NULL)
    {
      sprintf(new_precision,"%d.%d",major_bit,bitlen);
      precision_state->setInitValue(new_precision);
    }
  else
    if (DEBUG_REVISE)
      printf("HWrevise_pins:Warning, state %s from star %s not found\n",
	     fixed_statename,
	     fpga_core->comment_name());
  recipe_file << "\t" << fixed_statename << "=" << new_precision << endl;


  // Cleanup
  delete []fixed_statename;
  delete []new_precision;
	     
  // Return happy condition
  return(1);
}


///////////////////////////////////////////////////////
// Once bitwidths have been altered, revise pipe delays
///////////////////////////////////////////////////////
int ACSCGFPGATarget::HWrevise_pipes(CoreList* sg_list)
{
  int total_sgs=sg_list->size();
  for (int sloop=0;sloop<total_sgs;sloop++)
    {
      ACSCGFPGACore* fpga_core=(ACSCGFPGACore*) sg_list->elem(sloop);
//      fpga_core->sg_resources(LOCKED);
//      fpga_core->sg_delay_query();
      fpga_core->update_sg(UNLOCKED,UNLOCKED);
    }

  // Return happy condition
  return(1);
}
