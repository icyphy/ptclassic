static const char file_id[] = "DESiPStar.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1997-%Q% The Regents of the University of California.
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

 Programmer:  Shang-Pin Chang
 Date of creation: 11/2/97 

 Definition of DESiPStar (interface between SPIN and Ptolemy DE domain).

*******************************************************************/
#include <ctype.h>
#include <unistd.h>
#include "DESiPStar.h"

extern RunList	*X;
extern Ordered	*allglobal;
extern double	sched(int);
extern void     spin_main(char *);

PXPStarList	*allPXPStars = (PXPStarList *)0;
varList		*allProc = (varList *)0, *allVar = (varList *)0;
varList		*pbf = (varList *)0,     *pvf = (varList *)0;
char		spinfname[255];
int		SIP_ID = 0, currentMaxID = 0;

double DESiPStar :: RunSpin(RunList *runProc, double nowTime)
{
	Symbol *r;
	double suspendedTime = 0.0;
        double t,elapsedTime;
	int i;

	// Assign selected procedure
	X = runProc;
	elapsedTime = nowTime - X->sysTime;
	X->sysTime = nowTime;
	// 1. Clear all updated flags of local variables.
	// 2. Update all timers.
	for (r = X->symtab; r; r = r->next) {
	 r->updated = 0;
	 if (r->timer)
          {
	   for(i=0;i<r->nel;i++) {
            t = ((double *)(r->val))[i] + elapsedTime; 
            ((double *)(r->val))[i] = t;
	    if (r->togo[i] <= elapsedTime) r->togo[i] = 0.0;
	    else r->togo[i] -= elapsedTime;
	   }
          }
	}
	if (X->sysTime >= X->idlTime)
	{
	 // Goto SPIN !
	 X->suspend = 0;
	 suspendedTime = sched(0);
	}
	return suspendedTime;
}

void DESiPStar :: GlobalUpdate(void)
{
 static	char tmps[1024];
	PXPStarList *Xp,*tmp;
	Ordered *Op;
	int i;

 	for(Op=allglobal; Op && !Op->entry->updated; Op=Op->next);
	if (Op) {	
 	 for(Op=allglobal; Op; Op=Op->next) Op->entry->updated=0;
	 Xp = allPXPStars;
	 while(Xp) {
	  tmp = Xp->next;
	  if (Xp->entry != this) {
	   /* Check un-destroyed old star caused by FSM domain. */ 
	   if (Xp->entry->sipID > currentMaxID) delete Xp->entry; 
	   else {
	    if (strlen(Xp->entry->getScope()) > 0) {
	     if (strcmp(Xp->entry->getScope(),getScope())==0) { 
	      /* Fire PROMELA star (Xp->entry) to check global vars. */
	      Xp->entry->refireAtTime(arrivalTime);
	      Xp->entry->feedbackOut->sendData();
//	      Xp->entry->arrivalTime=arrivalTime;
//	      Xp->entry->run();
	     }  
	    } else {
	     strcpy(tmps,Xp->entry->fullName());
	     i=(int)(strrchr(tmps,'.')-tmps);
	     if (strncmp(tmps,fullName(),i)==0) {
	      /* Fire PROMELA star (Xp->entry) to check global vars. */
	      Xp->entry->refireAtTime(arrivalTime);
	      Xp->entry->feedbackOut->sendData();
//	      Xp->entry->arrivalTime=arrivalTime;
//	      Xp->entry->run();
	     }
	    }
	   }
	  } 
	  Xp = tmp;
	 }
	}
}

int sgetWord(char *ss, char *w, int *p, int *l, int *f)
{
 char s1[255];
 int ch,ch2,ok,slen=strlen(ss);
 int gotOne=0,chlen=0,spos=0,isnum=0,isdbl=0,isvar=0,isdef=0;
 *p=0;
 while(spos<slen) {
  ch=ss[spos];
  ch2=(spos<slen-1)? ss[spos+1]:0;
  ok=0;
  if ((!isdef)&&(!isnum)&&(!isvar)&&(isalpha(ch)||(ch=='_'))) { /*var*/
   ok=1; isvar=1; } 
  if ((!isvar)&&(!isnum)&&(!isdef)&&(ch=='#')) {          /* #define */
   ok=1; isdef=1; }
  if ((isvar||isdef)&&(isalnum(ch)||(ch=='_'))) ok=1;	
  if ((!isvar)&&(!isdef)&&(!isnum)&&isdigit(ch)) {        /* num */
   ok=1; isnum=1; }
  if (isnum&&(!isdbl)&&(ch=='.')) {			  /* double */
   ok=1; isdbl=1; }  
  if ((isnum||isdbl)&&isdigit(ch)) ok=1;
  if ((!isvar)&&(!isdef)&&(!isnum)&&(!isdbl)&&(ch=='.')&&(isdigit(ch2)))
  { ok=1; isnum=1; isdbl=1; }
  if (ok) {
   if (!gotOne) { gotOne=1; *p=spos; }
   s1[chlen++]=ch;
  }
  else if (gotOne) break;
  spos++;
 }
 s1[chlen]='\0';
 strcpy(w,s1);
 *l=chlen;
 *f=isdbl;
 return gotOne;
}
  
int inList(varList *vlist, char *s1, char *s2)
{
 varList *vl;
 char ss[255];
 int i;
 sprintf(ss,"%s%s",s1,s2);
 for(vl=vlist,i=1; vl; vl=vl->next,i++)
  if (strcmp(vl->entry,ss)==0) return(i);
 return 0;
}

void addList(int idx, char *s1, char *s2)
{
 varList *vl;
 char *ss;
 ss=(char *)malloc(strlen(s1)+strlen(s2)+1);
 sprintf(ss,"%s%s",s1,s2);
 vl=(varList *)malloc(sizeof(varList));
 vl->entry=ss;
 switch (idx) {
  case 1: vl->next=allProc; allProc=vl; break;
  case 2: vl->next=allVar;  allVar=vl;  break;
  case 3: vl->next=pbf;     pbf=vl;     break;
  case 4: vl->next=pvf;     pvf=vl;     break;
 }
}

void freeVList(void)
{
 varList *vl;
 while(allProc) {
  free(allProc->entry);
  vl=allProc->next;
  free(allProc);
  allProc=vl;
 }
 while(allVar) {
  free(allVar->entry);
  vl=allVar->next;
  free(allVar);
  allVar=vl;
 }
 while(pbf) {
  free(pbf->entry);
  vl=pbf->next;
  free(pbf);
  pbf=vl;
 }
 while(pvf) {
  free(pvf->entry);
  vl=pvf->next;
  free(pvf);
  pvf=vl;
 }
 /* I also initialize some DESiPStar.cc global vars here. */
 currentMaxID = SIP_ID;
 SIP_ID = 0;
}

void copyRest(char *scp, FILE *f1, FILE *f2, FILE *f3)
{
 static char *chan="chan",*iport="inport",*oport="outport";
 int i,j,pp,fidx,copyThis,npos,spos,slen;
 int isDbl=0,isOut=0,isWless=0,outChan=0,justChan=0;
 char s1[255],s2[255],s3[255],s4[255],s5[255],s6[255];
 while(!feof(f1)) {
  fgets(s1,255,f1);
  if (!feof(f1)) {
   npos=0; isDbl=0; isOut=0; isWless=0; outChan=0; justChan=0;
   while (sgetWord(s1+npos,s2,&spos,&slen,&isDbl)) {
    copyThis=1;
    if ((spos>0)&&(!((npos==0)&&(strcmp(s2,"wireless")==0)))) {
     strncpy(s3,s1+npos,spos);
     s3[spos]='\0';
     fprintf(f2,"%s",s3);
    }
    npos+=(spos+slen);
    if (inList(allVar,scp,s2)) fprintf(f2,"%s",scp);
    if ((fidx=inList(pbf,"",s2))) {
     i=(int)(strchr(s1+npos,'(')-s1)-npos;
     npos+=(i+1);
     fprintf(f2,"_func(%d,",(fidx-1));
     copyThis=0;
    }
    if ((fidx=inList(pvf,"",s2))) {
     i=(int)(strchr(s1+npos,'(')-s1)-npos;
     npos+=(i+1);
     fprintf(f2,"_func(%d,",(fidx-1)+1000);
     copyThis=0;
    }
    if (outChan) strcpy(s5,s2);
    if (justChan) { strcpy(s4,s2); justChan=0; }
    if (isOut && strcmp(chan,s2)==0) { outChan=1; justChan=1; }
    isOut=0;
    if (strcmp(s2,iport)&&strcmp(s2,oport)) {
     if (copyThis) {
      if (isWless) {
       strcpy(s5,s2);
       npos++;
       sgetWord(s1+npos,s2,&spos,&slen,&isDbl);
       npos+=(spos+slen);
       if (!inList(allVar,scp,s2)) {
        fprintf(f3,"%s %s%s",s5,scp,s2);
        addList(2,scp,s2);
        strncpy(s2,s1+npos,strlen(s1)-npos);
	s2[strlen(s1)-npos]='\0';
        fputs(s2,f3);
       } 
       npos=strlen(s1);
       isWless=0;
      } else {
       if (isDbl) {
	pp=(int)(strchr(s2,'.')-s2);
	if (pp>0) {
         strncpy(s5,s2,pp);
	 s5[pp]='\0';
	} else { s5[0]='0'; s5[1]='\0'; }
	j=(int)strlen(s2)-pp-1;
	if (j>0) {
	 i=1;
	 while((i<j)&&(s2[pp+i]=='0')) i++;
	 strncpy(s6,s2+pp+i,j-i+1);
	 s6[j-i+1]='\0';
	} else { j=1; s6[0]='0'; s6[1]='\0'; }
	fprintf(f2,"(_packf(%s,%s,%d))",s5,s6,j);
	isDbl=0;
       } else {
       if (strcmp(s2,"const")&&strcmp(s2,"persist")&&strcmp(s2,"multi"))
       {
        if (strcmp(s2,"wireless")) fprintf(f2,"%s",s2); 
	else { isWless=1; npos++; }
       }
       else npos++;
       }
      }
     }
    } else {
     if (strcmp(s2,iport)) { isOut=1; strcpy(s6,s3); }
     npos++;
    }
   }
   if (npos < (int)strlen(s1)) {
    strncpy(s3,s1+npos,strlen(s1)-npos);
    s3[strlen(s1)-npos]='\0';
    fprintf(f2,"%s",s3);
   }
   if (outChan) {
    fprintf(f2,"%s%s _%s_tmp;\n",s6,s5,s4);
    outChan=0;
   }
  }
 }
}

int gotoNext(FILE *ff)
{
 int ch;
 while(!feof(ff)) {
  ch=fgetc(ff);
  if (isalnum(ch) || ch=='_' || ch=='#') { ungetc(ch,ff); break; }
 }
 return(feof(ff)? 0:1);
}

int getWord(FILE *ff, char *w)
{
 char s1[255];
 int ch=0,gotOne=0,chlen=0;
 while(!feof(ff)) {
  ch=fgetc(ff);
  if (isalnum(ch) || ch=='_' || ch=='#') { gotOne=1; s1[chlen++]=ch; }
  else if (gotOne) break;
 }
 if (gotOne) ungetc(ch,ff);
 s1[chlen]='\0';
 strcpy(w,s1);
 return gotOne;
}

int copyToBrace(FILE *f1, FILE *f2)
{
 int ch;
 while(!feof(f1)) {
  ch=fgetc(f1);
  fputc(ch,f2);
  if (ch=='}') { 
   fprintf(f2,";\n");
   return(1);
  }
 }
 return(0);
}

int getWordBeforeChar(FILE *ff, char *w, char z)
{
 char s1[255];
 int ch=0,gotOne=0,chlen=0;
 while(!feof(ff)) {
  ch=fgetc(ff);
  if (isalnum(ch) || ch=='_' || ch=='#') { gotOne=1; s1[chlen++]=ch; }
  else if (gotOne || (ch==z)) break;
 }
 if (gotOne) ungetc(ch,ff);
 s1[chlen]='\0';
 strcpy(w,s1);
 return gotOne;
}
 
/*
int skipToBrace(FILE *f1)
{
 int ch;
 while(!feof(f1)) {
  ch=fgetc(f1);
  if (ch=='}') return(1);
 }
 return(0);
}

int gotoWord(FILE *ff, const char *w)
{
 char s1[255];
 while(!feof(ff)) 
  if (getWord(ff,s1) && (strcmp(w,s1)==0)) break;
 return(feof(ff)? 0:1);
}
*/

int copyGlobal(char *scp, FILE *f1, FILE *f2)
{
 static char *procidf="proctype", *mtypeidf="mtype", *typedefidf="typedef", *utilidf="utility";
 int gotProc=0,alreadyIn=0,err=0;
 char s1[255],s2[255],tmp[255];
 while(getWord(f1,s1)) {
  if (strcmp(s1,procidf)==0) { gotProc=1; break; }
  if (strcmp(s1,utilidf)==0) { 
   do {
    err=1-getWordBeforeChar(f1,tmp,'}');
    if (!err && (strcmp(tmp,"private")==0)) err=1;
    if (!err && (isalpha(tmp[0]))) addList(3,tmp,"");
   } while (!err);
   if (strcmp(tmp,"private")==0) {
    do {
     err=1-getWordBeforeChar(f1,tmp,'}');
     if (!err && (isalpha(tmp[0]))) addList(4,tmp,"");
    } while (!err);
   }
   err=0;
   continue;
  }
  if ((strcmp(s1,mtypeidf)==0) || (strcmp(s1,typedefidf)==0)) {
   fputs(s1,f2);
   copyToBrace(f1,f2);
   continue;
  }
  strcpy(s2,s1);
  if (getWord(f1,s1)) {
   if (!inList(allVar,scp,s1)) {
    fprintf(f2,"%s %s%s",s2,scp,s1);
    addList(2,scp,s1);
   } else alreadyIn = 1;
   fgets(s1,255,f1);
   if (!alreadyIn) fputs(s1,f2); else alreadyIn = 0; 
  } else break;
 }
 return gotProc;
}

void DESiPStar :: outputPML(int assignScope, const char *pplpath, const char *s1, const char *s2, const char *s3, const char *s4)
{
 char ss[255],starname[255],starnamei[255],starfull[255];
 char longscope[255],starscope[255];
 FILE *fppl,*fpml,*fvar,*fini;
 int i,scplen,scpstr;

 sipID = ++SIP_ID;
 if (SIP_ID==1) tmpnam(spinfname);
 strcpy(starfull,s1);
 strcpy(starnamei,s2);
 strcpy(starname,s3+2);
 sprintf(ss,"%s%s.ppl",pplpath,starname);
 fppl=fopen(ss,"r");
 sprintf(ss,"%s_pml",spinfname);
 if ((fpml=fopen(ss,"r"))==NULL) {
  fpml=fopen(ss,"w");
 } else {
  fclose(fpml);
  fpml=fopen(ss,"a");
 } 
 sprintf(ss,"%s_ini",spinfname);
 if ((fini=fopen(ss,"r"))==NULL) {
  fini=fopen(ss,"w");
  fprintf(fini,"init\n");
  fprintf(fini,"{\n");
  fprintf(fini," atomic {\n");
 } else {
  fclose(fini);
  fini=fopen(ss,"a");
 } 
 if ((fvar=fopen(spinfname,"r"))==NULL) {
  fvar=fopen(spinfname,"w");
 } else {
  fclose(fvar);
  fvar=fopen(spinfname,"a");
 } 
 scpstr=(int)(strchr(starfull,'.')-starfull)+1;
 scplen=(int)(strstr(starfull,starnamei)-starfull)-scpstr;
 strncpy(longscope,starfull+scpstr,scplen);
 longscope[scplen]='\0';
 for(i=0;i<scplen;i++) if (longscope[i]=='.') longscope[i]='_';
 if (assignScope) strcpy(starscope,s4); else strcpy(starscope,longscope);
 if (!inList(allProc,starscope,starname)) {
  addList(1,starscope,starname);
  if (gotoNext(fppl)) {
   copyGlobal(starscope,fppl,fvar);
   fprintf(fpml,"\nproctype %s%s(int _SIP_ID_)\n",starscope,starname);
   fgets(ss,255,fppl);
   copyRest(starscope,fppl,fpml,fvar);
  }
 }
 fprintf(fini,"  run %s%s(%d);\n",starscope,starname,sipID);
 fclose(fppl);
 fclose(fpml);
 fclose(fvar);
 fclose(fini);
}

void DESiPStar :: unregisterPXPStar(void)
{
 // Remove myself from PXPStarList
 PXPStarList *Xp,*tmp=NULL;

 Xp = allPXPStars;
 while(Xp && Xp->entry != this) { 
  tmp = Xp;
  Xp = Xp->next;
 }
 if (Xp) {
  if (tmp) tmp->next = Xp->next;
  else allPXPStars = allPXPStars->next;
  free(Xp);
 }
}

void DESiPStar :: registerPXPStar(void)
{
 //Register myself to PXPStarList
 PXPStarList *Xp;

 for (Xp=allPXPStars; Xp; Xp = Xp->next)
  if ((Xp->entry!=this)&&(Xp->entry->sipID==sipID)) delete Xp->entry;
 Xp = allPXPStars;
 while(Xp && Xp->entry != this) Xp = Xp->next;
 if (Xp==NULL) {
  Xp = (PXPStarList *)malloc(sizeof(PXPStarList));
  Xp->entry = this;
  Xp->next = allPXPStars;
  allPXPStars = Xp;
 }
}

void DESiPStar :: setNextFiring(void)
{
	 // Next Firing Preparation
	 if (NxtFireDur == 0.0)
	  completionTime = arrivalTime;
	 else if (NxtFireDur > 0.0)
	 {
	  completionTime = arrivalTime + NxtFireDur;
	  refireAtTime(completionTime);
//	  feedbackOut->sendData();
	 }
	 else if (NxtFireDur < 0.0)
	  {
	  completionTime = arrivalTime;
	  refireAtTime(arrivalTime-NxtFireDur);
//	  feedbackOut->sendData();
	  }
}

// DESiPStar constructor
DESiPStar :: DESiPStar() {
 setMode(PHASE);
}

// Destructor.
DESiPStar::~DESiPStar() {
}

// isA function
ISA_FUNC(DESiPStar,DERepeatStar);
 
/*
void DESiPStar :: unregisterAllPXPStar(void)
{
 PXPStarList *Xp,*tmp=NULL;

 Xp = allPXPStars;
 while (Xp) { 
  tmp = Xp;
  Xp = Xp->next;
  free(tmp);
 }
 allPXPStars = (PXPStarList *)0;
}
*/

void wrapUpPML(void)
{
   char ss[255];
   FILE *fini;
   if (SIP_ID > 0) {                 /* having SiP star on schematic */
    sprintf(ss,"%s_ini",spinfname);
    if ((fini=fopen(ss,"r"))!=NULL) {
     fclose(fini);
     fini=fopen(ss,"a");
     fprintf(fini," }\n");
     fprintf(fini,"}\n\n");
     fclose(fini);
     sprintf(ss,"cat %s_pml %s_ini >> %s",spinfname,spinfname,spinfname);
     system((const char *)ss);
     sprintf(ss,"%s_pml",spinfname);
     (void) unlink((const char *)ss);
     sprintf(ss,"%s_ini",spinfname);
     (void) unlink((const char *)ss);
     freeVList();
     spin_main(spinfname);
    }
   }
}

