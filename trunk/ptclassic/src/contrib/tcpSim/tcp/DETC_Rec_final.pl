//***********************************************************************
//                              DETCP_Rec.pl
//***********************************************************************
 
defstar{
 	name { TCP_Rec }
	domain { DE }
	author { Dorgahm Sisalem }

	desc {
        This star simulates a TCP receiver that fulfills the following
        requirements:
        Sliding window: Using  acknowledgment packets, the receiver informs
                the source of the allowed number of bytes that can be sent
	        before the sender buffer gets full.
        Update packets: After sending an acknowledgment with the window size 
                smaller than the maximum segment size (MSS) an update packet 
                is sent when there is new buffer space available.
        silly window: If the window size gets too small -smaller than MSS-
	        an update packet is only sent after freeing enough buffer space
	        - here, it was set to half the original window size.
        Duplicate acks: If a packet is lost the receiver sends for each new 
                received packet an Ack for the last correctly received packet.
                All out of order packets get buffered until the missing packet is
	        received. Only then can these packets get also acknowledged.
        Delayed acks: Ack packets are sent after an ack timer goes off or every
                other packet.

        In this model it is assumed that all packets have the same length
        -MSS-. Based on this the receiver only informs the application when a
        packet is received. If there is any interest in using variable packet
        sizes, the actual received packet must be sent to the application.
	}

//***********************************************************************
//			Define Ports
//***********************************************************************
	input { name { input }
                type { message}
	        desc { new data}
	}
	input { name { Ack_Fin}
                type { int}
	        desc { ack timer went off}
	}
        input { name { Size}
                type { int}
	        desc { size of available buffer }
	}

	output { name { control}
                 type { message}
	         desc { send Ack}
        }
        output { name { application}
                 type { int}
	         desc { send a packet to the application}
	}
	output { name { Ack_Start}
                 type { int}
	         desc { start the ack timer }
	}

//***********************************************************************
//			Define States
//***********************************************************************
	defstate {
		name { Window_Size}
		type { int  }
		default { 10 }
                desc {maximum transmission window. }
	}
	defstate {
               name { fileName }
               type { string }
               default {"" }
	}

	defstate {
		name { MSS}
		type { int  }
		default { 1 }
		desc { Maximum Segment Size}
	}
	defstate {
		name { Ack_Time}
		type { int  }
		default { 1 }
		desc { Start value for the acknowledgment timer.}
	}
	defstate {
		name { debug}
		type { int  }
		default { 0 }
	}

//***********************************************************************
//			Protected variables
//***********************************************************************
	hinclude { "TCPPacket.h" ,<memory.h>,<fstream.h>}
	protected {
		    Envelope theEnvp,Envp;
		    TCPPacket* last_Packet;
		    int Win_Size,Win_update;
		    int Last_Rec,Ack_Flag;
		    float last_TS;
		    int *buffer,Retransmit;
	            int seq,num,size,buf_size,Last_Num,Last_Win;
		    int counter,pointer,VC;
		  }
        hinclude { "pt_fstream.h" }
        protected {
                pt_ofstream *p_out;
        }

//*****************************************************************************/
// Initialization and buffer allocation
//*****************************************************************************/

	setup
	{
	  Ack_Flag=-1;
	  Last_Rec=-1;
	  Last_Num=0;
	  buf_size=0;
	  delayType= FALSE;
	  Win_update=0;
	  Win_Size=Window_Size;
	  Window_Size=(int)Window_Size/(int)MSS;
	  Retransmit=0;
	  counter=pointer=0;
 
	}
        begin{
          if(buffer) {LOG_DEL; delete [Window_Size] buffer; buffer=NULL;}
	  LOG_NEW;
	  buffer=new int[Window_Size];
	  for(int i=0;i<Window_Size;i++)
                 buffer[i]=0;
          if(p_out){LOG_DEL; delete p_out; p_out=NULL;}
          LOG_NEW; p_out = new pt_ofstream(fileName);
	  }
 
        wrapup{ 
               if(buffer) {LOG_DEL; delete [Window_Size] buffer; buffer=NULL;}
	       if(p_out){LOG_DEL; delete p_out;
	       p_out=NULL;} 
	   }

	constructor
	{
          Size.before(Ack_Fin);
  	  Size.before(input);
	  Size.triggers(control);
	  Ack_Fin.before(input);
	  Ack_Fin.triggers(control);
          p_out = NULL;
	  buffer=NULL;
          if(buffer) {LOG_DEL; delete [Window_Size] buffer; buffer=NULL;}
          if(p_out){LOG_DEL; delete p_out; p_out=NULL;}
	}
        destructor { if(p_out){LOG_DEL; delete p_out; p_out=NULL;}
               if(buffer) {LOG_DEL; delete [Window_Size] buffer;buffer=NULL;}
	}
//**********************************************************************
//                GO
//**********************************************************************
    
	go {
	  completionTime= arrivalTime;
                pt_ofstream& OUTPUT = *p_out;

//***************************************************************************
//                   update the window size
//***************************************************************************

	  if(Size.dataNew)
          {
            int actual_win=int(Size.get());
	    if(!counter)                   // update only if nothing is 
	      Win_Size=(int)(Window_Size*MSS)-actual_win;//saved in the local buffer

	    if(Win_update)  //handle silly window and send an update packet
	      if(float(actual_win)/float(Window_Size)<=0.5)
	      {

                const TCPPacket* Packet=(const TCPPacket *)
	                      (theEnvp).myData();
		int seq=Packet->readSeq();
		int num=Packet->readNum();
                last_Packet->setDestination(Packet->readDestination());
                last_Packet->setSource(Packet->readSource());
                last_Packet->setVC(Packet->readVC());
	    	last_Packet->setSize(Packet->readSize());
                last_Packet->setWin(Win_Size);
	        last_Packet->setSeq(Last_Rec);
	        last_Packet->setAck(Last_Num);
		last_Packet->setTimestamp(Last_Rec);
	        Envelope outEnvp( *last_Packet);
		Win_update=0;
if(debug)
OUTPUT<<" PROBE "<<last_Packet->readSeq()<<"\n";
		control.put(completionTime)<<outEnvp;
       	      }
	  }

//***************************************************************************
//                      new data received
//***************************************************************************

	  if(input.dataNew)
	  {
            //read the new packet
            input.get().getMessage(theEnvp);
	    const TCPPacket* PacketPtr = (const TCPPacket*) 
	                               (theEnvp).myData();
	    last_Packet=new TCPPacket(*PacketPtr);
	    seq = PacketPtr->readSeq();
	    num=PacketPtr->readNum();
	    size=PacketPtr->readSize();
	    VC=PacketPtr->readVC();
if(debug)
OUTPUT<<" RECEIVED "<<seq<<" VC "<<VC<<" AT "<<completionTime<<"\n";
            if(Last_Rec>=seq)
	    {
                last_Packet->setNum(num);
                last_Packet->setSeq(seq);
                last_Packet->setVC(VC);
                last_Packet->setWin(Win_Size);
if(debug)
OUTPUT<<" ACK out of Order "<<last_Packet->readSeq()<<"\n";
                Envelope outEnvp( *last_Packet);
                control.put(completionTime)<<outEnvp;
	 	return;
	
	    }
		
            int STOP=0;
	    if(Last_Rec+1<seq)             //missing packets ??
	    {
	      for(int i=0;(i<counter)&&!STOP;i++)
	      {
	      	if(buffer[i%(int)Window_Size]==seq) 
	           return;              
		if(i+1<counter)
		{
		    if((buffer[i%(int)Window_Size]<seq)&&
			(buffer[(i+1)%(int)Window_Size]>seq))
		    {
			for(int tmp=counter-1;tmp>i;tmp--)
	        	  buffer[(int)(tmp)%(int)Window_Size]=//reorder  packets
				buffer[((int)(tmp)+1)%(int)Window_Size]; 
			STOP=1;
if(debug)
OUTPUT<<" ADD "<<seq<<"\n";
                        buffer[(int)(i+1)%(int)Window_Size]=seq;
		    }
		}
		else
		{
if(debug)
OUTPUT<<" ADD "<<seq<<"\n";
                        buffer[(int)(counter)%(int)Window_Size]=seq;
		}
	      }
	      if(!counter)
	         buffer[(int)(counter)%(int)Window_Size]=seq;


	      if(counter+1>=Window_Size)
	      {
		Error::abortRun(*this,"receiver Buffer overflow","");
		return;
	      }
		counter++;
	        Win_Size-=size;

	        //send a duplicate ack
	        Retransmit++;   //count the number of out of sequence
			      //packets
		last_Packet->setAck(Last_Num);
		last_Packet->setSeq(Last_Rec);
		last_Packet->setVC(VC);
		last_Packet->setWin(Win_Size);
		last_Packet->setTimestamp(Last_Rec);
if(debug)
OUTPUT<<" ACK RET "<<last_Packet->readSeq()<<"\n";
		Envelope outEnvp( *last_Packet);
		control.put(completionTime)<<outEnvp;
	      //}
	      return;
	    }
	    else  // a correct packet was received
	    {
	      Win_Size-=size;
	      if(Retransmit)
	      {
                //send for each out of sequence packet a notification to the
	        //application
	        application.put(completionTime)<<seq;
		buf_size=0;
		last_TS=Last_Rec+1;

//***************************************************************************/
// Control if the buffered packets are correctly received 
//***************************************************************************/

	        while((buffer[(int)pointer%(int)Window_Size]==seq+(++pointer))&&(counter!=0))
	        {

	            buf_size+=MSS;
	            application.put(completionTime)<<buffer[(int)(pointer-1)%(int)Window_Size];
		    counter--;
                    Last_Rec=buffer[(int)(pointer-1)%(int)Window_Size];
		}
		pointer--;
		if(!counter) // no packet loss was detected
		  Retransmit=0;

		if(!pointer) // the first packet in the buffer was already 
		  Last_Rec=seq; // out of order, so set the received packet
	                        // as the last correctly received packet

		Ack_Flag=Last_Rec;
		Last_Num=num+buf_size;
		last_Packet->setSeq(Last_Rec);
		last_Packet->setAck(Last_Num);
		last_Packet->setWin(Win_Size);
		
		int i=0;
		while((buffer[(int)pointer%Window_Size]!=0)&&(pointer!=0)) //reorder the packets
		{                                      // that were left in the
		  buffer[(int)(i++)%Window_Size]=buffer[(int)pointer%Window_Size];         // buffer
		  buffer[(int)(pointer++)%Window_Size]=0;
		}
		pointer=0;

	    }
            else
	    {
	      if(seq<=Last_Rec) //a packet has been received that has already
	         Ack_Flag=-2;  // been acknowledged

	      else
	      {
	           application.put(completionTime)<<seq;
		   Last_Rec=seq;
		   Last_Num=num;
		   Retransmit=counter=buf_size=pointer=0;
	      }
	    }

	      if(Ack_Flag!=-1) // a packet has already been received
	      {
		last_TS=seq;
		Last_Win=Win_Size;
	        if(Win_Size<=MSS) Win_update=1;   // silly window ?
	        if(Win_update)
	        {
	            last_Packet->setWin(0);
		    Last_Win=0;
		}
	        else
	     	    last_Packet->setWin(Win_Size);
	     	last_Packet->setTimestamp(last_TS);
		last_Packet->setVC(VC);
		Envelope outEnvp( *last_Packet);
if(debug)
OUTPUT<<" Ack $$ "<<last_Packet->readSeq()<<"\n";
		control.put(completionTime)<<outEnvp; // send ack
		Ack_Flag=-1;
	      }
	      else  //start ack timer
	      {
	        Ack_Flag=seq;
		last_TS=seq;
		Ack_Start.put(completionTime)<<int(seq);
	      }
	    }
	  }

//*************************************************************************
//                            ack timer went off
//************************************************************************

	  if(Ack_Fin.dataNew)
	  {
	     Ack_Fin.dataNew=FALSE;

	     //if the timer went off for a packet that has not been
	     //acknowledged yet, send an ack

	     if((Ack_Flag==Ack_Fin%0)&&!Retransmit)
	     {
	        const TCPPacket* Packet=(const TCPPacket *)
	                     (theEnvp).myData();  
	        int seq=Packet->readSeq();
		int num=Packet->readNum();
		Last_Win=Win_Size;
	        if(Win_Size<=MSS) Win_update=1;   // silly window ?
	        if(Win_update)
	        {
	            last_Packet->setWin(0);
		    Last_Win=0;
		}
	        else
	     	    last_Packet->setWin(Win_Size);
                last_Packet->setDestination(Packet->readDestination());
                last_Packet->setSource(Packet->readSource());
                last_Packet->setVC(Packet->readVC());
                last_Packet->setSize(Packet->readSize());
		last_Packet->setVC(VC);
	        last_Packet->setSeq(seq);
	        last_Packet->setAck(num);
	     	last_Packet->setTimestamp(seq);
	        Envelope outEnvp( *last_Packet);
		control.put(completionTime)<<outEnvp;  // send ack
if(debug)
OUTPUT<<" ACK "<<last_Packet->readSeq()<<" WIN "<<last_Packet->readWin()<<"\n";
	        Ack_Flag=-1;
	     }
          }
	} // end go
} // end defstar



