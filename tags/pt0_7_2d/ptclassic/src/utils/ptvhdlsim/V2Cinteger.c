/*
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
/*
   V2Cinteger.c
   Integer communication from VHDL to C
   Version:  @(#)V2Cinteger.c	1.3 3/15/96
*/

#include "CLISocket.h"

/*********************
   Elaboration Routine
*********************/
void v2cinteger_open(did,iid)
     cliDID did;
     cliIID iid;
{
  cliVALUE gvPairid;
  cliVALUE gvNumxfer;
  cliAFTER waitTime;
  INSTANCE_DATA *idata;

  cli_warnings = 0;
  CHECK_ERRNO;
  idata = (INSTANCE_DATA *)
    cliAllocMem(sizeof(INSTANCE_DATA));
  CHECK_ERRNO;
  idata->gidPairid = cliGenericName2GID(did,"PAIRID");
  CHECK_ERRNO;
  idata->gidNumxfer = cliGenericName2GID(did,"NUMXFER");
  CHECK_ERRNO;
  idata->pidGo = cliPortName2PID(did,"GO");
  CHECK_ERRNO;
  idata->pidData = cliPortName2PID(did,"DATA");
  CHECK_ERRNO;
  idata->pidDone = cliPortName2PID(did,"DONE");
  CHECK_ERRNO;
  idata->dummy = "  v2cinteger Error";
  idata->nbytes = 12;
  idata->connection_established = 0;
  idata->data_requested = 0;

  /* Configure according to the generic parameter values */
  cliGetGenericValue(did,iid,idata->gidPairid,&gvPairid);
  CHECK_ERRNO;
  cliGetGenericValue(did,iid,idata->gidNumxfer,&gvNumxfer);
  CHECK_ERRNO;
  idata->pairid = gvPairid.value.integer;
  idata->numxfer = gvNumxfer.value.integer;

  (void) sprintf(idata->nearstring, "%s%s%d", SOCK_BASE_NAME,
		 "snd", idata->pairid);
  (void) sprintf(idata->farstring, "%s%s%d", SOCK_BASE_NAME,
		 "rcv", idata->pairid);
  (void) sprintf(idata->format, "%%d\0");

  idata->nearnamelen = strlen(idata->nearstring);
  idata->farnamelen = strlen(idata->farstring);
  idata->faraddrlen = sizeof(idata->faraddr);

  if (cli_fatals != 0) {
    /* messages, cannot continue simulation */
    printf("Cli Fatals == %d !!\n", cli_fatals);
  }
  if (cli_warnings != 0) {
    /* messages, notify user */
    CHECK_ERRNO;
    printf("Cli Warnings == %d !!\n", cli_warnings);
    CHECK_ERRNO;
  }

  /* Only set up data structures for sockets here.
     Worry about connection establishment in evaluation routine.
     */
  create_socket(idata);

  /* Do the following only once, and only during elaboration */
  cliSaveInstanceData(did,iid, (void *) idata);
  CHECK_ERRNO;

  /* Schedule to wake up immediately to establish socket connection */
  waitTime.delay.low = 0;
  waitTime.delay.high = 0;
  waitTime.delay.timebase = NS;
  waitTime.delaytype = TRANSPORT;
  cliScheduleWakeup(did,iid,&waitTime);
  CHECK_ERRNO;
  return;
}

/********************
   Evaluation Routine
********************/
void v2cinteger_eval(did,iid,ppid,np)
     cliDID did;
     cliIID iid;
     cliPID *ppid;
     int np;
{
  cliVALUE svGo;
  cliVALUE svData;
  cliVALUE svDone;
  cliAFTER waitTime;
  INSTANCE_DATA *idata;

  idata = (INSTANCE_DATA *) cliRestoreInstanceData(did,iid);
  CHECK_ERRNO;

  /* Save data request if one is being made */
  /* if(np > 0) { -- Need different test than this.
     Problem could be another port being active?
     Please explicitly check for go port and for
     an actual event on that port */
  if(np > 0) {
    cliAssert(np == 1);
    CHECK_ERRNO;
    cliAssert(ppid[0] == idata->pidGo);
    CHECK_ERRNO;
    cliGetPortValue(did,iid,idata->pidGo,&svGo);
    CHECK_ERRNO;
    cliAssert(svGo.typemark == CLI_TYPE_STD_LOGIC);
    CHECK_ERRNO;
    if(svGo.value.std_logic != STD_LOGIC_U) {
      idata->data_requested++;
      cliAssert(idata->data_requested == 1);
      CHECK_ERRNO;
    }
  }
/* If np = 1 but connection_established = 0,
   need to remember to reactivate once connection is
   established and do that read, else event is lost!

   New synch:  don't process "go" event until
   connection is established - but how to do this
   when don't know it's a go wakeup until begin this eval?
   Save flag to do a write.
   What if do more than one data request back to back?
   Shouldn't happen since VHDL proc synchs before asking
   for more data.
*/
  waitTime.delay.low = 0;
  waitTime.delay.high = 0;
  waitTime.delay.timebase = NS;
  waitTime.delaytype = TRANSPORT;

  /* Check for connection established.
     Do not handle any data until then.
     */
  if (!idata->connection_established) {
    /* If connection not established, try again */
    if (connect_socket(idata) == 0) {
      /* Successful, suspend */
/*
      perror("v2cinteger Connection Established");
*/
      idata->connection_established = 1;
      return;
    }
    else {
      /* Unsuccessful, schedule zero-delay wait time to try again */
      cliScheduleWakeup(did,iid,&waitTime);
      CHECK_ERRNO;
      return;
    }
  }
  else { /* Connection established, must be time to transfer data */
    if(idata->data_requested) {
      /* Get the data from the port and put it on the socket */
      cliGetPortValue(did,iid,idata->pidData,&svData);
      CHECK_ERRNO;
      cliAssert(svData.typemark == CLI_TYPE_INTEGER);
      CHECK_ERRNO;
      cliAssert(svData.length == 0);
      CHECK_ERRNO;
      cliAssert(svData.direction == NOT_USED);
      CHECK_ERRNO;
      idata->intptr = svData.value.integer;

  (void) sprintf(idata->buffer, idata->format, idata->intptr);

      put_data(idata);

      /* Put a transaction on the done port to signal completion */
      svDone.value.std_logic = STD_LOGIC_1;
      svDone.typemark = CLI_TYPE_STD_LOGIC;
      svDone.length = 0;
      svDone.direction = NOT_USED;
      cliScheduleOutput(did,iid,idata->pidDone,&svDone,&waitTime);
      CHECK_ERRNO;

      idata->data_requested--;
    }
  }
  return;
}

/*****************
   Closing Routine
*****************/
void v2cinteger_close(did,iid)
     cliDID did;
     cliIID iid;
{
  INSTANCE_DATA *idata;

  idata = (INSTANCE_DATA *) cliRestoreInstanceData(did,iid);
  CHECK_ERRNO;

  close_socket(idata);

  if (idata != (INSTANCE_DATA *) 0) {
    cliFreeMem((void *) idata);
    CHECK_ERRNO;
  }
  return;
}

/************************
   Error Handling Routine
************************/
void v2cinteger_error(did,iid,level)
     cliDID did;
     cliIID iid;
     int level;
{
  char *s;
  if (level == 0)
    s = "warning";
  else
    s = "fatal error";

  (void) printf("%s encountered with (%d,%d)\n",s,did,iid);
  CHECK_ERRNO;

  return;
}
