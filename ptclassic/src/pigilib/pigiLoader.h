/* 
Copyright (c) 1990-%Q% The Regents of the University of California.
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
   $Id$
*/

#ifdef __cplusplus
extern "C" {
extern void KcLoadInit (const char* argv0);
extern void KcDoStartupLinking();
extern int FindStarSourceFile(const char* dir,const char* dom,const char* base, char* buf);
extern int KcCompileAndLink (const char* name, const char* idomain, const char* srcDir, int permB, const char* linkArgs);
extern int KcLoad (const char* iconName, int permB, const char* linkArgs);
}
#else /* __cplusplus */
extern void KcLoadInit (/* const char* argv0 */);
void KcDoStartupLinking();
extern int FindStarSourceFile(/* const char* dir,const char* dom,const
				 char* base, char* buf */); 
extern int KcCompileAndLink (/* const char* name, const char* idomain,
				const char* srcDir, int permB,
				const char* linkArgs */);
extern int KcLoad ( /*const char* iconName, int permB, const char*
		      linkArgs */);
#endif /* __cplusplus */
