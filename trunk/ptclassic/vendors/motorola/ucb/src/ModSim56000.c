
/*	$Id$ 						     */
/* MODULE NAME: SIM56000.C                                           */
/*                                                                   */
/* MODULE DESCRIPTION:                                               */
/*   This module contains the main entry point for the DSP56000      */
/* simulator SIM56000, REV 2.02.  It simulates a single device       */
/* and displays the output to the terminal.                          */
/*                                                                   */
/* The lines indicated between "FRIGG" represent the changes from    */
/* the original Motorola simulator				     */
/* FUNCTIONS:                                                        */
/* main(argc,argv) - SIM56000, rev 2.02, single dsp, with window     */
/*                                                                   */

#include "config.h"
#include "brackets.h"
#include "dspexec.h"
#include "dspsdef.h"
#include "dspsdcl.h"

#define NUM_DSPS 1 	/* Number of simultaneously simulated devices */

/****FRIGG****/
#define NUM_PINS 74 	/* Number of device pins defined in the simulator */

/****FRIGG:3****/
int 	my_cycle_count = 0;	/* cycle counter */
char 	my_pin_val[NUM_PINS];	/* array of pin values */
int 	pin_map[NUM_PINS];  	/* pin number translation table */



/* main simulator entry point */
main (argc,argv)
int argc;
char **argv;
{

/****FRIGG:7****/
char	my_string[199]; /* holds string for output to simulator screen */
int  	i;              /* counter for pin array packing/unpacking loops */
int	fipc;           /* file descriptor for reading/writing on IPC socket */
int	id;             /* unique id for this DSP; used for ipc addresses */
char host[99];		/* name of host */
int  port;		/* port number for IPC connection */
char protocol[19];	/* protocol for IPC connection */

   char cmdstr[81]; /* Will hold command strings input from terminal */
   struct dsp_var *dspn[NUM_DSPS]; /* dsp device pointers */
   dsp_const.num_dsps=NUM_DSPS;
   dsp_const.dsp_list= dspn;
   dsp_const.viewdev=dsp_const.breakdev=0;


    /****FRIGG:16****/
    if (argc < 5) {		/* check for right # of args from Frigg */
	    fprintf(stderr, "ModSim56000: ERROR: main: bad arg count; A .cmd file needs to be specified to load the ModSim56000.\n");
	    /*system("sleep 10"); */
	    exit(-1);
    }

    id = atoi(argv[1]);		/* get parameters for IPC connection */
    strcpy(host,argv[3]);
    strcpy(protocol,"inet");
    port = atoi(argv[4]);

    if ((fipc = RPCConnectToServer(host,port,protocol)) == -1) {  
	fprintf(stderr,"ModSim56000: ERROR: main: open_ipc_connection faild.\n");
	system("sleep 5");
	exit(-1);
    }

   dsp_new(0,DSP56001);
   dsplwinit();      /* Initialize window and keyboard parameters */
   dsp_revision(0);  /* Display the simulator version */

    /****FRIGG:2****/
   /* sprintf(my_string, "ModSim56000: id = %d; host = '%s'; port = %d; protocol = '%s'", id, host, port, protocol); */

    dsplwscr(my_string, 0);
   				/* if unable to open connection... */
   /* Begin with execution of macros specified in command line */

   /****FRIGG****/
   if (argc >= 3){
      if (!strcmp(argv[2],"resume")){
         /* Reload all simulator state files from DSP_SIMn.mem */
         /* This is used in PC version following SYSTEM simulator command.*/
         (void) dsp_ldall("DSP_SIM"); 
         }
      else dsp_docmd(0,argv[2]); /* Execution of other macros <name>.cmd */
      }


    /****FRIGG:8****/
    mappins(); 		/* set up pin number tranlation table */

   /* Now set up IPC connection with Thor; */
   /* This is the client side; we assume that Thor (the server) has */
   /* already opened the socket on its end */

/*    sprintf(my_string, "  #### ModSim56000: main: open_ipc_connection returned: %d\n", fipc); */
sprintf(my_string, "RPC connection with Thor established \n");
    dsplwscr(my_string, 0);
 

   /* This is the main execution loop.  If the device is at a breakpoint, */
   /* the next command is requested from a macro file or from the terminal.*/
   /* Commands that cause the device to begin executing will set the */
   /* "executing" flag in the dsp structure. */


    for(;;){
	if (dspn[0]->executing) {

    /****FRIGG:26****/
/*
	    sprintf(my_string, "  #### ModSim56000: main: about to execute cycle: %d\n", my_cycle_count);
	    dsplwscr(my_string, 0);
*/

            /* get all pin values from Thor, read into my_pin_val[] */
            receive_pin_vals(fipc, my_pin_val);

            /* unpack array, writing to pins and translating order */
            for (i = 0; i < NUM_PINS; i++)
                 dsp_wpin(0, pin_map[i], (int) my_pin_val[i]);
/* write DSP pin state 
 writes the pin number given by pin_map for  that i as this is
the order in which thor willsend the values, with the value my_pin_val
as is recd. 
so that pin gets that value 
*/

            dsp_exec(0);    /* execute cycles until breakpoint is reached */
/*executes a single dsp cycle and updates the device sturcture */

            /* put all pin values into an array to return to Thor */

            for (i = 0; i < NUM_PINS; i++) {
                my_pin_val[i]  = (char) dsp_rpin(0, pin_map[i]);
/* Read DSP Pin-state */
/*fprintf(stderr," computed values are my_pin_values[%d]:\t%d\n",i,my_pin_val[i]); */
						  }


            /* return pin value array to Thor via IPC */
            send_pin_vals(fipc, my_pin_val);

            my_cycle_count++;
	
	}	

        else {
           do {
            if (dspn[0]->in_macro) dsp_gmcmd(0,cmdstr); /* get macro command */
            if (!dspn[0]->in_macro) dsp_gtcmd(0,cmdstr); /* get terminal command*/
            dsp_docmd(0,cmdstr);
            } while (!dspn[0]->executing);
         }
/* fprintf(stderr," end of one round of the ModSim56000 \n"); */
      }   
}   /* end of main */


