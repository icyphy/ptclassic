#ifndef _VoiceData_h
#define _VoiceData_h 1
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

Programmer:  Paul Haskell
Date of creation: 4/5/91
Based on:  FloatVecData.h by Joe Buck
Modified for VoiceData by:  John Loh
Date of Modification:  6/5/91
Modified: 7/9/91 for new methods
          7/16/91 for new methods

This is a simple Voice type message body.  It stores an array of
unsigned chars.
*************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

//////////// Defines
#define MESSAGEWIDTH 53

//////////// Include file
#include "Message.h"
#include <string.h>

//////////// VoiceData class definition

class VoiceData : public Message {

private:
    int width;
    float stamp;   // may mark message's origination time etc.
    int connectionID;    // to mark packet's connection tag    
    unsigned char *data;

public:

////// Constructor: make an uninitialized array.
    VoiceData(int a = MESSAGEWIDTH) : width(a)
        { LOG_NEW; data = new unsigned char[width]; 
	  connectionID = 0;
          clearMessage();
        }

////// Copy constructor:
    VoiceData (const VoiceData& id):
            width(id.retWidth()), stamp(id.retStamp()),
	    connectionID(id.retConnID())
        {
            LOG_NEW; data = new unsigned char[width];
            unsigned char* ptr = id.asVoice();
            for(int i = 0; i < width; i++) {
                data[i] = ptr[i];
            }
        }

    int temp;
    unsigned char dummy2;
    VoiceData(int l,int *srcData):width(MESSAGEWIDTH){
            LOG_NEW; data = new unsigned char[width];
	    connectionID = 0;
            clearMessage(); 
            for (int i = 0;i < l; i++) {
                temp = int(*srcData++);
                temp = temp%255;
                dummy2 = char(temp&0377);
                data[i] = dummy2;
            }
    }         

    VoiceData(char *name):width(MESSAGEWIDTH){
            LOG_NEW; data = new unsigned char[width];
	    connectionID = 0;
            clearMessage(); 
            if (strcmp(name,"ACK")==0) ackMessage();
            if (strcmp(name,"NAK")==0) nakMessage();
            if (strcmp(name,"STP")==0) stopMessage();
            if (strcmp(name,"SSDP")==0) ssdpMessage();
            if (strcmp(name,"STRP")==0) strpMessage();
            }

    VoiceData(char *name, int *srcData1):width(MESSAGEWIDTH){
            LOG_NEW; data = new unsigned char[width];
	    connectionID = 0;
            clearMessage(); 
            if (strcmp(name,"CDP")==0) callDurMessage(srcData1); // duration message
            if (strcmp(name,"CTP")==0) calltermMessage(srcData1);
            if (strcmp(name,"RP")==0) receiveMessage(srcData1);
    }         

    VoiceData(char *name, int *srcData1, int *srcData2):width(MESSAGEWIDTH){
            LOG_NEW; data = new unsigned char[width];
	    connectionID = 0;
            clearMessage(); 
            if (strcmp(name,"REQP")==0) requestMessage(srcData1,srcData2);
            if (strcmp(name,"SDP")==0) sendDataMessage(srcData1,srcData2);
            if (strcmp(name,"INQP")==0) inquireMessage(srcData1,srcData2);
            if (strcmp(name,"UPDP")==0) upDateMessage(srcData1,srcData2);
            if (strcmp(name,"DNDP")==0) downDateMessage(srcData1,srcData2);
            if (strcmp(name,"SOT")==0) sotMessage(srcData1,srcData2);
            if (strcmp(name,"EOT")==0) eotMessage(srcData1,srcData2);
    }         

    VoiceData(char *name, int *srcData1, int *srcData2, int *srcData3) :
            width(MESSAGEWIDTH)  {
            LOG_NEW; data = new unsigned char[width];
	    connectionID = 0;
            clearMessage();
            if (strcmp(name,"TRP")==0) transmitMessage(srcData1, srcData2,
                                                      srcData3);
    }

    VoiceData(char *name,int *srcData1,int *srcData2,int *srcData3,int *srcData4):width(MESSAGEWIDTH){
            LOG_NEW; data = new unsigned char[width];
            clearMessage(); 
	    connectionID = 0;
            if (strcmp(name,"SRTUP")==0) srtuMessage(srcData1,srcData2,srcData3,srcData4);
    }         

    void loadField(int position, unsigned char info) {
      data[position -1] = info; 
    }

    void setStamp (float num) {
      stamp = num;
    }

    void setConnID (int num) {
      connectionID = num;
    }

    void clearMessage() {
      for (int i=1; i<=width; i++)
        loadField(i,0);
    }

    ///Additional methods, control methods with no data to put in

    void ackMessage() {
          data[4] = 011;
          data[5] = 011;
    }

    void nakMessage() {
          data[4] = 021;
          data[5] = 021;
    }

    void receiveMessage(int *VCI) {
          data[4] = 031;
          data[5] = 031;
          data[6] = (char)((*VCI)&0377);	
    }

    void calltermMessage(int *VCI) {
          data[4] = 041;
          data[5] = 041;
          data[6] = (char)((*VCI)&0377);	
    }

    void stopMessage() {
          data[4] = 051;
          data[5] = 051;
    }

    void ssdpMessage() {
          data[4] = 061;
          data[5] = 061;
    }

    void strpMessage() {
          data[4] = 071;
          data[5] = 071;
    }

    // control messages with one piece data to put in

    void callDurMessage(int *duration) {
          data[4] = 013;
          data[5] = 013;
          data[6] = (char)((*duration)&0377);	
    }

    // control messages with two pieces of data to put in

    void requestMessage(int *VCI1,int *VCI2) {
          data[4] = 023;
          data[5] = 023;
          data[6] = (char)((*VCI1)&0377);
          data[7] = (char)((*VCI2)&0377);
    }

    void sendDataMessage(int *duration,int *VCI) {
          data[4] = 033;
          data[5] = 033;
          data[6] = (char)((*duration)&0377);
          data[7] = (char)((*VCI)&0377);
    }

    void inquireMessage(int *switchnum,int *VPI) {
          data[4] = 017;
          data[5] = 017;
          data[6] = (char)((*switchnum)&0377);
          data[7] = (char)((*VPI)&0377);
    }

    void upDateMessage(int *switchnum,int *VPI) {
          data[4] = 027;
          data[5] = 027;
          data[6] = (char)((*switchnum)&0377);
          data[7] = (char)((*VPI)&0377);
    }

    void downDateMessage(int *switchnum,int *VPI) {
          data[4] = 037;
          data[5] = 037;
          data[6] = (char)((*switchnum)&0377);
          data[7] = (char)((*VPI)&0377);
    }

    void sotMessage(int* IVCI, int* OVCI) {
          data[4] = 057;
          data[5] = 057;
          data[2] = (char)((*IVCI)&0377);
          data[3] = (char)((*OVCI)&0377);
    }

    void eotMessage(int* IVCI, int* OVCI) {
          data[4] = 022;
          data[5] = 022;
          data[2] = (char)((*IVCI)&0377);
          data[3] = (char)((*OVCI)&0377);
    }

    void transmitMessage(int *IVCI, int *OVCI, int *VPI) {
          data[4] = 047;
          data[5] = 047;
          data[6] = (char)((*IVCI)&0377);
          data[7] = (char)((*OVCI)&0377);
          data[8] = (char)((*VPI)&0377);
    }

    void srtuMessage(int *IVCI,int *OVCI,int *SCPE,int *DCPE) {
          data[0] = (char)((*IVCI)&0377);
          data[1] = (char)((*OVCI)&0377);
          data[2] = (char)((*SCPE)&0377);
          data[3] = (char)((*DCPE)&0377);
    }

    int nulltestMessage() const {
          int flag = 1;
          unsigned char blank = 0;
          for (int k = 0; k < width; k++) 
            if (data[k] != blank) flag = 0;
          return flag;
    } 

//  Return 1 if the message is an ACK, 0 if not
    int isitACKMessage() const {
      int ackflag = 0;
      if ((data[4]==011)&&(data[5]==011)) ackflag  = 1;
      return ackflag;
    }

//  Return 1 if the message is a Nack, 0 if not
    int isitNAKMessage() const {
      int nakflag = 0;
      if ((data[4]==021)&&(data[5]==021)) nakflag  = 1;
      return nakflag;
    }

//  Return 1 if the message is a Receive Message, 0 if not
    int isitRMessage() const {
      int rpflag = 0;
      if ((data[4]==031)&&(data[5]==031)) rpflag  = 1;
      return rpflag;
    }

//  Return 1 if the message is a Call Termination Message, 0 if not
    int isitCTMessage() const {
      int ctpflag = 0;
      if ((data[4]==041)&&(data[5]==041)) ctpflag  = 1;
      return ctpflag;
    }

//  Return 1 if the message is a Stop  Message, 0 if not
    int isitSTMessage() const {
      int stpflag = 0;
      if ((data[4]==051)&&(data[5]==051)) stpflag  = 1;
      return stpflag;
    }

//  Return 1 if the message is a Stop Send Data Message, 0 if not
    int isitSSDMessage() const {
      int stsdflag = 0;
      if ((data[4]==061)&&(data[5]==061)) stsdflag  = 1;
      return stsdflag;
    }

//  Return 1 if the message is a Stop Transmit Message, 0 if not
    int isitSTRMessage() const {
      int strpflag = 0;
      if ((data[4]==071)&&(data[5]==071)) strpflag  = 1;
      return strpflag;
    }

//  Return 1 if the message is a Start of Transmission Message, 0 if not
    int isitSOTMessage() const {
      int sotpflag = 0;
      if ((data[4]==057)&&(data[5]==057)) sotpflag  = 1;
      return sotpflag;
    }

//  Return 1 if the message is an End of Transmission Message, 0 if not
    int isitEOTMessage() const {
      int eotpflag = 0;
      if ((data[4]==022)&&(data[5]==022)) eotpflag  = 1;
      return eotpflag;
    }

//  Return 1 if the message is a Call Duration Message, 0 if not
    int isitCDMessage() const {
      int cdflag = 0;
      if ((data[4]==013)&&(data[5]==013)) cdflag  = 1;
      return cdflag;
    }

//  Return 1 if the message is a Request Duration Message, 0 if not
    int isitREQMessage() const {
      int reqflag = 0;
      if ((data[4]==023)&&(data[5]==023)) reqflag  = 1;
      return reqflag;
    }

//  Return 1 if the message is a Send Data Message, 0 if not
    int isitSDMessage() const {
      int sdflag = 0;
      if ((data[4]==033)&&(data[5]==033)) sdflag  = 1;
      return sdflag;
    }

//  Return 1 if the message is an Inquire Message, 0 if not
    int isitINQMessage() const {
      int inqflag = 0;
      if ((data[4]==017)&&(data[5]==017)) inqflag  = 1;
      return inqflag;
    }

//  Return 1 if the message is an UPDate Message, 0 if not
    int isitUPDMessage() const {
      int udflag = 0;
      if ((data[4]==027)&&(data[5]==027)) udflag  = 1;
      return udflag;
    }

//  Return 1 if the message is a DownDate Message, 0 if not
    int isitDNDMessage() const {
      int ddflag = 0;
      if ((data[4]==037)&&(data[5]==037)) ddflag  = 1;
      return ddflag;
    }

//  Return 1 if the message is a TransmitMessage, 0 if not
    int isitTRMessage() const {
      int trflag = 0;
      if ((data[4]==047)&&(data[5]==047)) trflag  = 1;
      return trflag;
    }


////// Clone
    Message* clone() const { LOG_NEW; return new VoiceData(*this);}

////// Destructor.
    ~VoiceData() { LOG_DEL; delete [] data; }

////// Generic MessageData functions.
    const char* dataType() const    { return "VoiceData";}

    int isA (const char* name) const {
      if (strcmp(name,"VoiceData")==0) return 1;
      else return Message::isA(name);
    }

    inline unsigned char* asVoice() const { return data; }
    inline int retWidth()           const { return width; }
    inline float retStamp()         const { return stamp; }
    inline int retConnID()          const { return connectionID; }

}; // end class VoiceData


#endif 
