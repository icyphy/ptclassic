/* 
 * Convert SiP .ppl file to Ptolemy .pl file
 * 
 * Version:
 * $Id$
 * 
 * Copyright (c) 1997-%Q% The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in all
 * copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 * PT_COPYRIGHT_VERSION_2
 * COPYRIGHTENDKEY
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#include "compat.h"

typedef struct varList {
  char *entry;
  char *value;
  int type;
  struct varList *next;
} varList;

varList *in_other = NULL, *in_chan = NULL, *out_other = NULL, *out_chan = NULL;
varList *constvar = NULL, *pv_fun = NULL, *pb_fun = NULL;
char currLine[255];
int pv_fnum = 0, pb_fnum = 0;

int 
sgetWord(char *ss, char *w, int *p, int *l)
{
  char s1[255];
  int ch, gotOne = 0, chlen = 0, spos = 0, slen = strlen(ss);
  *p = 0;
  while (spos < slen) {
    ch = ss[spos];
    if (isalnum(ch) || ch == '_' || ch == '#' || ch == '.') {
      if (!gotOne) {
	gotOne = 1;
	*p = spos;
      }
      s1[chlen++] = ch;
    }
    else if (gotOne)
      break;
    spos++;
  }
  s1[chlen] = '\0';
  strcpy(w, s1);
  *l = chlen;
  return gotOne;
}

void 
addList(int idx, char *s1, char *s2)
{
  char *ss;
  varList *vl;
  ss = (char *) malloc(strlen(s1) + 1);
  strcpy(ss, s1);
  vl = (varList *) malloc(sizeof(varList));
  vl->entry = ss;
  if (s2[0] != '\0') {
    ss = (char *) malloc(strlen(s2) + 1);
    strcpy(ss, s2);
    vl->value = ss;
  }
  vl->type = idx / 8;		/* 1 if persistent port; 2 if multi port */
  switch (idx) {
  case 0:
  case 8:
  case 16:
  case 24:
  case 32:
  case 40:
  case 48:
  case 56:
    vl->next = in_other;
    in_other = vl;
    break;
  case 1:
  case 17:
    vl->next = in_chan;
    in_chan = vl;
    break;
  case 2:
  case 18:
  case 34:
    vl->next = out_other;
    out_other = vl;
    break;
  case 3:
  case 19:
    vl->next = out_chan;
    out_chan = vl;
    break;
  case 4:
  case 36:
    vl->next = constvar;
    constvar = vl;
    break;
  case 128:
    vl->next = pv_fun;
    pv_fun = vl;
    pv_fnum++;
    break;
  case 129:
    vl->next = pb_fun;
    pb_fun = vl;
    pb_fnum++;
    break;
  }
}

void 
freeVList(varList * vlToFree)
{
  varList *vl;
  while (vlToFree) {
    free(vlToFree->entry);
    vl = vlToFree->next;
    free(vlToFree);
    vlToFree = vl;
  }
}

int 
gotoNext(FILE * ff)
{
  int ch;
  while (!feof(ff)) {
    ch = fgetc(ff);
    if (isalnum(ch) || ch == '_' || ch == '#') {
      ungetc(ch, ff);
      break;
    }
  }
  return (feof(ff) ? 0 : 1);
}

int 
createVLists(FILE * f1)
{
  static char *chan = "chan", *vartype = " inport outport const persist multi ",
   *dbl = "double";
  int npos, spos, slen, isConst, isOut, tokIdx, isChan, isPst, isMul, isDbl,
    isWlss, notDone = 1, i;
  char s2[255], s3[255], *nn, *mm;
  gotoNext(f1);
  while (notDone) {
    fgets(currLine, 255, f1);
    npos = 0;
    isOut = -1;
    tokIdx = 0;
    isChan = 0;
    isConst = 0;
    isPst = 0;
    isMul = 0;
    isDbl = 0;
    isWlss = 0;
    s3[0] = '\0';
    while (sgetWord(currLine + npos, s2, &spos, &slen)) {
      if ((npos > 0) || (strcmp(s2, "wireless"))) {
	tokIdx++;
	if (!isConst && (isOut == -1)) {
	  nn = strstr(vartype, s2);
	  if (nn == NULL) {
	    notDone = 0;
	    break;
	  }
	  else {
	    mm = strchr(nn, ' ');
	    while (*nn != ' ')
	      nn--;
	    nn++;
	    if (strncmp(nn, s2, (int) (mm - nn))) {
	      notDone = 0;
	      break;
	    }
	    i = (int) (nn - vartype);
	    switch (i) {
	    case 1:
	      isOut = 0;
	      break;
	    case 8:
	      isOut = 1;
	      break;
	    case 16:
	      isConst = 1;
	      npos += (spos + slen);
	      if (sgetWord(currLine + npos, s2, &spos, &slen)) {
		tokIdx++;
		isDbl = (strcmp(s2, dbl)) ? 0 : 1;
		npos += (spos + slen);
		if (sgetWord(currLine + npos, s2, &spos, &slen)) {
		  tokIdx++;
		  npos += (spos + slen);
		  if (sgetWord(currLine + npos, s3, &spos, &slen)) {
		    tokIdx++;
		  }
		}
	      }
	      break;
	    case 22:
	      isPst = 1;
	      tokIdx--;
	      break;
	    case 30:
	      isMul = 1;
	      tokIdx--;
	      break;
	    }
	  }
	}
	else if (!isConst && (tokIdx == 2)) {
	  isChan = (strcmp(s2, chan)) ? 0 : 1;
	  isDbl = (strcmp(s2, dbl)) ? 0 : 1;
	}
	if (tokIdx >= 3)
	  break;
	npos += (spos + slen);
      }
      else {
	isWlss = 1;
	break;
      }
    }
    if (notDone && (!isWlss)) {
      if (tokIdx < 3 + isConst)
	return 3;
      addList(isDbl * 32 + isMul * 16 + isPst * 8 + isConst * 4 + (1 - isConst) * (isOut * 2 + isChan), s2, s3);
    }
  }
  return 0;
}

int 
getWord(FILE * ff, char *w)
{
  char s1[255];
  int ch = -1, gotOne = 0, chlen = 0;
  while (!feof(ff)) {
    ch = fgetc(ff);
    if (isalnum(ch) || ch == '_' || ch == '#') {
      gotOne = 1;
      s1[chlen++] = ch;
    }
    else if (gotOne)
      break;
  }
  if (gotOne)
    ungetc(ch, ff);
  s1[chlen] = '\0';
  strcpy(w, s1);
  return gotOne;
}

int 
gotoWord(FILE * ff, const char *w)
{
  char s1[255];
  while (!feof(ff))
    if (getWord(ff, s1) && (strcmp(w, s1) == 0))
      break;
  return (feof(ff) ? 0 : 1);
}

int 
getWordBeforeChar(FILE * ff, char *w, char z)
{
  char s1[255];
  int ch = -1, gotOne = 0, chlen = 0;
  while (!feof(ff)) {
    ch = fgetc(ff);
    if (isalnum(ch) || ch == '_' || ch == '#') {
      gotOne = 1;
      s1[chlen++] = ch;
    }
    else if (gotOne || (ch == z))
      break;
  }
  if (gotOne)
    ungetc(ch, ff);
  s1[chlen] = '\0';
  strcpy(w, s1);
  return gotOne;
}

/*
 * int gotoChar(FILE *ff, char w)
 * {
 * char s1[255];
 * int ch,gotOne=0;
 * while(!feof(ff)) {
 * ch=fgetc(ff);
 * if (ch==w) gotOne=1; break;
 * }
 * return gotOne;
 * }
 */

void 
createPL(char *pname)
{
  int i;
  varList *vl;
  FILE *fpl, *finfo;
  char plfn[255], myinfo[5][255], tmpfn[255], cmd[255];

  if (system("cd ptolemy")) {
    system("mkdir ptolemy");
    printf("ptolemy subdirectory created.\n");
  }
  else
    system("cd ..");

  /* Get user's info */
  tmpnam(tmpfn);
  sprintf(cmd, "pwd >%s", tmpfn);
  system((const char *) cmd);
  sprintf(cmd, "whoami >>%s", tmpfn);
  system((const char *) cmd);
  sprintf(cmd, "hostname >>%s", tmpfn);
  system((const char *) cmd);
  sprintf(cmd, "date >>%s", tmpfn);
  system((const char *) cmd);
  sprintf(cmd, "finger `whoami` | grep Login >>%s", tmpfn);
  system((const char *) cmd);
  finfo = fopen(tmpfn, "r");
  fgets(myinfo[0], 255, finfo);
  *strrchr(myinfo[0], 0x0A) = '\0';	/* remove carry return */
  fgets(myinfo[1], 255, finfo);
  *strrchr(myinfo[1], 0x0A) = '\0';	/* remove carry return */
  fgets(myinfo[3], 255, finfo);
  *strrchr(myinfo[3], 0x0A) = '\0';	/* remove carry return */
  fgets(myinfo[4], 255, finfo);
  *strrchr(myinfo[4], 0x0A) = '\0';	/* remove carry return */
  fgets(cmd, 255, finfo);
  fclose(finfo);
  *strrchr(cmd, 0x0A) = '\0';	/* remove carry return */
  i = (int) (strrchr(cmd, ':') - cmd);
  strncpy(myinfo[2], cmd + i + 2, strlen(cmd) - i - 2);
  myinfo[2][strlen(cmd) - i - 2] = '\0';
  (void) unlink((const char *) tmpfn);

  /* Create *.pl here  */
  sprintf(tmpfn, "ptolemy/DE%s.cc", pname);
  if ((fpl = fopen(tmpfn, "r")) != NULL) {
    fclose(fpl);
    (void) unlink((const char *) tmpfn);
  }
  sprintf(plfn, "ptolemy/DE%s.pl", pname);
  fpl = fopen(plfn, "w");
  fprintf(fpl, "defstar\n");
  fprintf(fpl, "{\n");
  fprintf(fpl, "    name { %s }\n", pname);
  fprintf(fpl, "    domain { DE }\n");
  fprintf(fpl, "    derivedfrom { SiPStar }\n");
  fprintf(fpl, "    author { %s }\n", myinfo[2]);
  fprintf(fpl, "    descriptor { DE%s.pl is an interface DE star associated with PROMELA file %s.ppl. It was generated by %s@%s on %s. }\n", pname, pname, myinfo[1], myinfo[3], myinfo[4]);
  fprintf(fpl, "    copyright {\n");
  fprintf(fpl, "Copyright (c) 1990-1998 The Regents of the University of California.\n");
  fprintf(fpl, "All rights reserved.\n");
  fprintf(fpl, "See the file $PTOLEMY/copyright for copyright notice,\n");
  fprintf(fpl, "limitation of liability, and disclaimer of warranty provisions.\n");
  fprintf(fpl, "    }\n");
  for (vl = out_other; vl; vl = vl->next) {
    if ((vl->type / 2) % 2)
      fprintf(fpl, "    outmulti {\n");
    else
      fprintf(fpl, "    output {\n");
    fprintf(fpl, "        name { %s }\n", vl->entry);
    if ((vl->type / 4) % 2)
      fprintf(fpl, "        type { double }\n");
    else
      fprintf(fpl, "        type { int }\n");
    fprintf(fpl, "    }\n");
  }
  for (vl = out_chan; vl; vl = vl->next) {
    if ((vl->type / 2) % 2)
      fprintf(fpl, "    outmulti {\n");
    else
      fprintf(fpl, "    output {\n");
    fprintf(fpl, "        name { %s }\n", vl->entry);
    fprintf(fpl, "        type { int }\n");
    fprintf(fpl, "    }\n");
  }
  for (vl = in_other; vl; vl = vl->next) {
    if ((vl->type / 2) % 2)
      fprintf(fpl, "    inmulti {\n");
    else
      fprintf(fpl, "    input {\n");
    fprintf(fpl, "        name { %s }\n", vl->entry);
    if ((vl->type / 4) % 2)
      fprintf(fpl, "        type { double }\n");
    else
      fprintf(fpl, "        type { int }\n");
    fprintf(fpl, "    }\n");
  }
  for (vl = in_chan; vl; vl = vl->next) {
    if ((vl->type / 2) % 2)
      fprintf(fpl, "    inmulti {\n");
    else
      fprintf(fpl, "    input {\n");
    fprintf(fpl, "        name { %s }\n", vl->entry);
    fprintf(fpl, "        type { int }\n");
    fprintf(fpl, "    }\n");
  }
  fprintf(fpl, "    defstate {\n");
  fprintf(fpl, "        name { scope }\n");
  fprintf(fpl, "        type { string }\n");
  fprintf(fpl, "        default { \"\" }\n");
  fprintf(fpl, "        desc { Scope of the global variables used in file %s.ppl }\n", pname);
  fprintf(fpl, "    }\n");
  fprintf(fpl, "    defstate {\n");
  fprintf(fpl, "        name { instOp }\n");
  fprintf(fpl, "        type { int }\n");
  fprintf(fpl, "        default { 0 }\n");
  fprintf(fpl, "        desc { Set to 1 for an instant operation module, otherwise 0. }\n");
  fprintf(fpl, "    }\n");
  for (vl = constvar; vl; vl = vl->next) {
    fprintf(fpl, "    defstate {\n");
    fprintf(fpl, "        name { %s }\n", vl->entry);
    fprintf(fpl, "        type { int }\n");
    fprintf(fpl, "        default { \"%s\" }\n", vl->value);
    fprintf(fpl, "        desc { Initial value of %s used in file %s.ppl }\n", vl->entry, pname);
    fprintf(fpl, "    }\n");
  }
  if (out_other || out_chan || in_other || in_chan || pb_fun || pv_fun) {
    fprintf(fpl, "    protected {\n");
    if (out_chan)
      fprintf(fpl, "        int    notEmpty;\n");
    for (vl = out_other; vl; vl = vl->next)
      fprintf(fpl, "        Symbol *sym_%s;\n", vl->entry);
    for (vl = out_chan; vl; vl = vl->next) {
      fprintf(fpl, "        Symbol *sym_%s;\n", vl->entry);
      fprintf(fpl, "        Symbol *sym_%s_tmp;\n", vl->entry);
    }
    for (vl = in_other; vl; vl = vl->next) {
      fprintf(fpl, "        Symbol *sym_%s;\n", vl->entry);
      if ((vl->type % 2) == 0)
	fprintf(fpl, "        double lastP_%s;\n", vl->entry);
    }
    for (vl = in_chan; vl; vl = vl->next)
      fprintf(fpl, "        Symbol *sym_%s;\n", vl->entry);
    if (out_chan)
      fprintf(fpl, "        Lextok *tok_Qout,*tok_Qoutlft,*tok_Qoutrgt,*tok_Qoutrgtlft,*tok_Qoutrgtlftlft;\n");
    if (in_chan)
      fprintf(fpl, "        Lextok *tok_Qin,*tok_Qinlft,*tok_Qinrgt,*tok_Qinrgtlft;\n");
    if (pv_fun)
      fprintf(fpl, "        int (DE%s::*myfp[%d])(int*);\n", pname, pv_fnum);
    if (pb_fun)
      fprintf(fpl, "        int (*pubfp[%d])(int*);\n", pb_fnum);
    fprintf(fpl, "    }\n");
  }
  fprintf(fpl, "    constructor {\n");
  if (out_chan) {
    fprintf(fpl, "        tok_Qout = ZN;\n");
    fprintf(fpl, "        tok_Qoutlft = ZN;\n");
    fprintf(fpl, "        tok_Qoutrgt = ZN;\n");
    fprintf(fpl, "        tok_Qoutrgtlft = ZN;\n");
    fprintf(fpl, "        tok_Qoutrgtlftlft = ZN;\n");
  }
  if (in_chan) {
    fprintf(fpl, "        tok_Qin = ZN;\n");
    fprintf(fpl, "        tok_Qinlft = ZN;\n");
    fprintf(fpl, "        tok_Qinrgt = ZN;\n");
    fprintf(fpl, "        tok_Qinrgtlft = ZN;\n");
  }
  for (i = 0, vl = pv_fun; (i++ < pv_fnum); vl = vl->next)
    fprintf(fpl, "        myfp[%d]=&%s;\n", i - 1, vl->entry);
  for (i = 0, vl = pb_fun; (i++ < pb_fnum); vl = vl->next)
    fprintf(fpl, "        pubfp[%d]=&%s;\n", i - 1, vl->entry);
  fprintf(fpl, "    }\n");
  fprintf(fpl, "    destructor {\n");
  fprintf(fpl, "        unregisterPXPStar();\n");
  if (out_chan) {
    fprintf(fpl, "        if (tok_Qout) free(tok_Qout);\n");
    fprintf(fpl, "        if (tok_Qoutlft) free(tok_Qoutlft);\n");
    fprintf(fpl, "        if (tok_Qoutrgt) free(tok_Qoutrgt);\n");
    fprintf(fpl, "        if (tok_Qoutrgtlft) free(tok_Qoutrgtlft);\n");
    fprintf(fpl, "        if (tok_Qoutrgtlftlft) free(tok_Qoutrgtlftlft);\n");
  }
  if (in_chan) {
    fprintf(fpl, "        if (tok_Qin) free(tok_Qin);\n");
    fprintf(fpl, "        if (tok_Qinlft) free(tok_Qinlft);\n");
    fprintf(fpl, "        if (tok_Qinrgt) free(tok_Qinrgt);\n");
    fprintf(fpl, "        if (tok_Qinrgtlft) free(tok_Qinrgtlft);\n");
  }
  fprintf(fpl, "    }\n");
  fprintf(fpl, "    method {\n");
  fprintf(fpl, "        name { getScope }\n");
  fprintf(fpl, "        access { protected }\n");
  fprintf(fpl, "        arglist { \"(void)\" }\n");
  fprintf(fpl, "        type { \"const char*\" }\n");
  fprintf(fpl, "        code { return (const char*)scope; }\n");
  fprintf(fpl, "    }\n");
  for (vl = pv_fun; vl; vl = vl->next) {
    fprintf(fpl, "    method {\n");
    fprintf(fpl, "        name { %s }\n", vl->entry);
    fprintf(fpl, "        access { protected }\n");
    fprintf(fpl, "        arglist { \"(int* args)\" }\n");
    fprintf(fpl, "        type { int }\n");
    fprintf(fpl, "        code { #include \"%s/utility/%s/%s.cc\" }\n", myinfo[0], pname, vl->entry);
    fprintf(fpl, "    }\n");
  }
  if (pv_fun || pb_fun) {
    fprintf(fpl, "    method {\n");
    fprintf(fpl, "        name { func }\n");
    fprintf(fpl, "        access { protected }\n");
    fprintf(fpl, "        arglist { \"(int prv, int idx)\" }\n");
    fprintf(fpl, "        type { int }\n");
    if (pb_fun && pv_fun)
      fprintf(fpl, "        code { return ((prv)? (*this.*myfp[idx])(arglist):(*pubfp[idx])(arglist)); }\n");
    else if (pb_fun)
      fprintf(fpl, "        code { return ((*pubfp[idx])(arglist)); }\n");
    else if (pv_fun)
      fprintf(fpl, "        code { return ((*this.*myfp[idx])(arglist)); }\n");
    fprintf(fpl, "    }\n");
  }
  fprintf(fpl, "    code {\n");
  fprintf(fpl, "        extern RunList* findproc3(int ID);\n");
  fprintf(fpl, "        extern Symbol* findloc2(RunList *x, const char *s);\n");
  fprintf(fpl, "        extern Lextok* pt_nn(Lextok *s, int t, Lextok *ll, Lextok *rl);\n");
  if (out_chan)
    fprintf(fpl, "        extern int qrecv(Lextok *n, int full);\n");
  if (in_chan)
    fprintf(fpl, "        extern int qsend(Lextok *n);\n");
  for (vl = pb_fun; vl; vl = vl->next) {
    fprintf(fpl, "        #ifndef _SPIN_CFUNC_%s_\n", vl->entry);
    fprintf(fpl, "        #define _SPIN_CFUNC_%s_\n", vl->entry);
    fprintf(fpl, "        int %s(int* args)\n", vl->entry);
    fprintf(fpl, "        {\n");
    fprintf(fpl, "        #include \"%s/utility/%s.cc\"\n", myinfo[0], vl->entry);
    fprintf(fpl, "        }\n");
    fprintf(fpl, "        #else\n");
    fprintf(fpl, "        extern int %s(int* args);\n", vl->entry);
    fprintf(fpl, "        #endif\n");
  }
  fprintf(fpl, "    }\n");
  fprintf(fpl, "    setup {\n");
  fprintf(fpl, "        delayType = instOp? FALSE:TRUE;\n");
  fprintf(fpl, "    }\n");
  fprintf(fpl, "    begin {\n");
  fprintf(fpl, "        SPINReady = FALSE;\n");
  fprintf(fpl, "        DERepeatStar::begin();\n");
  fprintf(fpl, "        outputPML(((strlen(scope)>0)? 1:0),\"%s/\",fullName(),name(),className(),(const char*)scope);\n", myinfo[0]);
  fprintf(fpl, "        registerPXPStar();\n");
  fprintf(fpl, "    }\n");
  fprintf(fpl, "    go {\n");
  fprintf(fpl, "     if (!SPINReady) {\n");
  if (out_chan) {
    fprintf(fpl, "      if (tok_Qout) free(tok_Qout);\n");
    fprintf(fpl, "      if (tok_Qoutlft) free(tok_Qoutlft);\n");
    fprintf(fpl, "      if (tok_Qoutrgt) free(tok_Qoutrgt);\n");
    fprintf(fpl, "      if (tok_Qoutrgtlft) free(tok_Qoutrgtlft);\n");
    fprintf(fpl, "      if (tok_Qoutrgtlftlft) free(tok_Qoutrgtlftlft);\n");
  }
  if (in_chan) {
    fprintf(fpl, "      if (tok_Qin) free(tok_Qin);\n");
    fprintf(fpl, "      if (tok_Qinlft) free(tok_Qinlft);\n");
    fprintf(fpl, "      if (tok_Qinrgt) free(tok_Qinrgt);\n");
    fprintf(fpl, "      if (tok_Qinrgtlft) free(tok_Qinrgtlft);\n");
  }
  if (out_chan) {
    fprintf(fpl, "      tok_Qoutrgtlftlft = pt_nn(ZN,CONST,ZN,ZN);\n");
    fprintf(fpl, "      tok_Qoutrgtlft = pt_nn(ZN,TYPE,tok_Qoutrgtlftlft,ZN);\n");
    fprintf(fpl, "      tok_Qoutrgt = pt_nn(ZN,NAME,tok_Qoutrgtlft,ZN);\n");
    fprintf(fpl, "      tok_Qoutlft = pt_nn(ZN,CONST,ZN,ZN);\n");
    fprintf(fpl, "      tok_Qout = pt_nn(ZN,'r',tok_Qoutlft,tok_Qoutrgt);\n");
    fprintf(fpl, "      tok_Qout->rgt->lft->lft->val = 0;\n");
  }
  if (in_chan) {
    fprintf(fpl, "      tok_Qinlft = pt_nn(ZN,CONST,ZN,ZN);\n");
    fprintf(fpl, "      tok_Qinrgtlft = pt_nn(ZN,CONST,ZN,ZN);\n");
    fprintf(fpl, "      tok_Qinrgt = pt_nn(ZN,NAME,tok_Qinrgtlft,ZN);\n");
    fprintf(fpl, "      tok_Qin = pt_nn(ZN,'s',tok_Qinlft,tok_Qinrgt);\n");
  }
  fprintf(fpl, "      run_Proc = findproc3(sipID);\n");
  fprintf(fpl, "      run_Proc->host = this;\n");
  for (vl = out_other; vl; vl = vl->next)
    fprintf(fpl, "      sym_%s = findloc2(run_Proc,\"%s\");\n", vl->entry, vl->entry);
  for (vl = out_chan; vl; vl = vl->next) {
    fprintf(fpl, "      sym_%s = findloc2(run_Proc,\"%s\");\n", vl->entry, vl->entry);
    fprintf(fpl, "      sym_%s_tmp = findloc2(run_Proc,\"_%s_tmp\");\n", vl->entry, vl->entry);
  }
  for (vl = in_other; vl; vl = vl->next) {
    fprintf(fpl, "      sym_%s = findloc2(run_Proc,\"%s\");\n", vl->entry, vl->entry);
    if ((vl->type % 2) == 0)
      fprintf(fpl, "      lastP_%s = 0.0;\n", vl->entry);
  }
  for (vl = in_chan; vl; vl = vl->next)
    fprintf(fpl, "      sym_%s = findloc2(run_Proc,\"%s\");\n", vl->entry, vl->entry);
  for (vl = constvar; vl; vl = vl->next) {
    if ((vl->type / 4) % 2)
      fprintf(fpl, "      ((double *)(findloc2(run_Proc,\"%s\")->val))[0] = (double)(%s);\n", vl->entry, vl->entry);
    else
      fprintf(fpl, "      ((int *)(findloc2(run_Proc,\"%s\")->val))[0] = (int)(%s);\n", vl->entry, vl->entry);
  }
  fprintf(fpl, "      SPINReady = TRUE;\n");
  fprintf(fpl, "     }\n");
  for (vl = in_other; vl; vl = vl->next) {
    if ((vl->type % 2) == 0)
      fprintf(fpl, "     if (lastP_%s<arrivalTime) sym_%s->present = 0;\n", vl->entry, vl->entry);
    fprintf(fpl, "     while (%s.dataNew) {\n", vl->entry);
    if ((vl->type / 4) % 2)
      fprintf(fpl, "      ((double *)(sym_%s->val))[0] = (double)(%s.get());\n", vl->entry, vl->entry);
    else
      fprintf(fpl, "      ((int *)(sym_%s->val))[0] = (int)(%s.get());\n", vl->entry, vl->entry);
    fprintf(fpl, "      sym_%s->present = 1;\n", vl->entry);
    if ((vl->type % 2) == 0)
      fprintf(fpl, "      lastP_%s = arrivalTime;\n", vl->entry);
    fprintf(fpl, "     }\n");
  }
  for (vl = in_chan; vl; vl = vl->next) {
    fprintf(fpl, "     if (%s.dataNew) {\n", vl->entry);
    fprintf(fpl, "      tok_Qin->lft->val = ((int *)(sym_%s->val))[0];\n", vl->entry);
    fprintf(fpl, "      while (%s.dataNew) {\n", vl->entry);
    fprintf(fpl, "       tok_Qin->rgt->lft->val = (int)(%s.get());\n", vl->entry);
    fprintf(fpl, "       qsend(tok_Qin);\n");
    fprintf(fpl, "      }\n");
    fprintf(fpl, "      sym_%s->present = 1;\n", vl->entry);
    fprintf(fpl, "     }\n");
  }
  fprintf(fpl, "     if (arrivalTime >= completionTime) {\n");
  fprintf(fpl, "      NxtFireDur = RunSpin(run_Proc,arrivalTime);\n");
  for (vl = out_other; vl; vl = vl->next) {
    fprintf(fpl, "      if (sym_%s->updated) {\n", vl->entry);
    if ((vl->type / 4) % 2)
      fprintf(fpl, "       %s.put(arrivalTime) << ((double *)(sym_%s->val))[0];\n", vl->entry, vl->entry);
    else
      fprintf(fpl, "       %s.put(arrivalTime) << ((int *)(sym_%s->val))[0];\n", vl->entry, vl->entry);
    fprintf(fpl, "      }\n");
  }
  for (vl = out_chan; vl; vl = vl->next) {
    fprintf(fpl, "      if (sym_%s->updated) {\n", vl->entry);
    fprintf(fpl, "       tok_Qout->rgt->lft->sym = sym_%s_tmp;\n", vl->entry);
    fprintf(fpl, "       tok_Qout->lft->val = ((int *)(sym_%s->val))[0];\n", vl->entry);
    fprintf(fpl, "       do {\n");
    fprintf(fpl, "        if ((notEmpty=qrecv(tok_Qout,1)))\n");
    fprintf(fpl, "         %s.put(arrivalTime) << ((int *)(tok_Qout->rgt->lft->sym->val))[0];\n", vl->entry);
    fprintf(fpl, "       } while(notEmpty);\n");
    fprintf(fpl, "      }\n");
  }
  fprintf(fpl, "      setNextFiring();\n");
  fprintf(fpl, "      GlobalUpdate();\n");
  fprintf(fpl, "     }\n");
  fprintf(fpl, "    }\n");
  fprintf(fpl, "}\n");
  fclose(fpl);
  freeVList(out_other);
  freeVList(out_chan);
  freeVList(in_other);
  freeVList(in_chan);
}

int main(argc, argv)
     int argc;
     char *argv[];
{
  char tmp[255], tmp2[255], filen[255];
  FILE *fppl, *fclib;
  int i, err = 0, gotUtil = 0, gotProc = 0;
  if (argc != 2) {
    printf("Usage: ppl2pl filename.ppl\n");
    printf("       This will create a file ptolemy/DEfilename.pl. If the\n");
    printf("       directory ptolemy doesn't exist, it will be created.\n");
    exit(1);
  }

  if ((fppl = fopen(argv[1], "r")) == NULL) {
    printf("File %s not found!\n", argv[1]);
    exit(1);
  }

  while (getWord(fppl, tmp)) {
    if (strcmp(tmp, "utility") == 0)
      gotUtil = 1;
    else if (strcmp(tmp, "proctype") == 0)
      gotProc = 1;
    if (gotUtil || gotProc)
      break;
  }

  if (gotUtil) {
    do {
      err = 1 - getWordBeforeChar(fppl, tmp, '}');
      if (!err && (strcmp(tmp, "private") == 0))
	err = 1;
      if (!err && (isalpha((int)tmp[0]))) {
	addList(129, tmp, "");
	if (system("cd utility")) {
	  system("mkdir utility");
	  printf("Create /utility directory to store C-lib functions.\n");
	}
	else
	  system("cd ..");
	sprintf(filen, "utility/%s.cc", tmp);
	if ((fclib = fopen(filen, "r")) == NULL) {
	  printf("Create program template for utility/%s.cc\n", tmp);
	  fclib = fopen(filen, "w");
	  fprintf(fclib, "// Arguments stored in args[0], args[1], args[2], ...\n");
	  fprintf(fclib, "// Do not erase the remark symbol ahead function name.\n");
	  fprintf(fclib, "// int %s(int* args)\n", tmp);
	  fprintf(fclib, "{\n");
	  fprintf(fclib, "\n");
	  fprintf(fclib, "}\n");
	}
	fclose(fclib);
      }
    } while (!err);
    if (strcmp(tmp, "private") == 0) {
      do {
	err = 1 - getWordBeforeChar(fppl, tmp, '}');
	if (!err && (isalpha((int)tmp[0]))) {
	  addList(128, tmp, "");
	  if (system("cd utility")) {
	    system("mkdir utility");
	    printf("Create /utility directory to store public C-lib functions.\n");
	  }
	  else
	    system("cd ..");
	  i = (int) (strchr(argv[1], '.') - argv[1]);
	  strncpy(tmp2, argv[1], i);
	  tmp2[i] = '\0';
	  sprintf(filen, "cd utility/%s", tmp2);
	  if (system((const char *) filen)) {
	    sprintf(filen, "mkdir utility/%s", tmp2);
	    system((const char *) filen);
	    printf("Create /utility/%s directory to store private C-lib functions.\n", tmp2);
	  }
	  else
	    system("cd ../..");
	  sprintf(filen, "utility/%s/%s.cc", tmp2, tmp);
	  if ((fclib = fopen(filen, "r")) == NULL) {
	    printf("Create program template for utility/%s/%s.cc\n", tmp2, tmp);
	    fclib = fopen(filen, "w");
	    fprintf(fclib, "// Arguments stored in args[0], args[1], args[2], ...\n");
	    fprintf(fclib, "// Do not erase the remark symbol ahead function name.\n");
	    fprintf(fclib, "// int %s(int* args)\n", tmp);
	    fprintf(fclib, "{\n");
	    fprintf(fclib, "\n");
	    fprintf(fclib, "}\n");
	  }
	  fclose(fclib);
	}
      } while (!err);
    }
    err = 0;
  }
  if (!gotProc)
    err = 1 - gotoWord(fppl, "proctype");
  if (!err) {
    if (getWord(fppl, tmp))
      err = createVLists(fppl);
    else
      err = 2;
  }
  fclose(fppl);
  switch (err) {
  case 0:
    createPL(tmp);
    break;
  case 1:
    printf("Missing keyword 'proctype' in file %s.\n", argv[1]);
    break;
  case 2:
    printf("Missing procedure name in file %s.\n", argv[1]);
    break;
  case 3:
    printf("Declaration error: %s\n", currLine);
    break;
  }
  return 0;
}
