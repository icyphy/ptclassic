# Version id: $Id$
# Programmer: E. A. Lee,
# inspired by previous programs by David A. Wheeler and HorngDar Lin
#
# nr2mml.awk - convert a ptolemy troff documentation file into a mml file.
#
# The PTOLEMY environment variable must be set to point to the home
# directory of the Ptolemy installation.
#
# Usage:
#         Cat sect.* files to xxxx.mm.
#         Run nr2mml xxxx.mm.
#         Create a new maker file (e.g. using ~FRAME/TEMPLATES/printpaper)
#         Import xxxx.mml.
#
# Copyright (c) 1993 The Regents of the University of California.
# All rights reserved.
#
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
#
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
#                                                        COPYRIGHTENDKEY

BEGIN	{ FS = " "; OFS = " "; bodynext = 1; eachline = 0;
       # Modify these for local formats
       defaultparagraph="<Body>";
       printf("<MML -- Framemaker mml translated from troff Ptolemy doc>\n");
       printf("<Comment nr2mml filter (C) 1993 University of California>\n");
# The following is printed directly, rather than through an include file,
# to avoid having to search the file system for the include file.
       printf("<!DefineFont Class\n <Family Courier>\n <pts 11>\n <Bold> >\n");
       printf("<!DefineFont Norm\n <Family Times>\n <pts 12> >\n");
       printf("<!DefineTag 1Heading>\n");
       printf("<!DefineTag 2Heading>\n");
       printf("<!DefineTag 3Heading>\n");
       printf("<!DefineTag Affiliation>\n");
       printf("<!DefineTag Author>\n");
       printf("<!DefineTag Body>\n");
       printf("<!DefineTag Bullet>\n");
       printf("<!DefineTag Commands>\n");
       printf("<!DefineTag Definition>\n");
       printf("<!DefineTag Display>\n");
       printf("<!DefineTag Explanation>\n");
       printf("<!DefineTag Footnote>\n");
       printf("<!DefineTag Heading>\n");
       printf("<!DefineTag LBody>\n");
       printf("<!DefineTag ListElem>\n");
       printf("<!DefineTag OtherContrib>\n");
       printf("<!DefineTag Section>\n");
       printf("<!DefineTag Title>\n");
       }
# Commands to ignore
/^\.eh/ { next }
/^\.oh/ { next }
/^\.VR/ { next }
/^\.TP/ { next }
#
# Commands to convert headers and titles
# The translation given below offsets the levels by one, as done
# in the Ptolemy troff macros when the document is printed together
# with other parts of the document.
/^\.H1/	{print ""; printf("<2Heading>\n") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("\n") ; bodynext = 1 ; next}
/^\.H2/	{print ""; printf("<3Heading>\n") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("\n") ; bodynext = 1 ; next}
/^\.H3/	{print ""; printf("<4Heading>\n") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("\n") ; bodynext = 1 ; next}
/^\.H4/	{print ""; printf("<Heading>\n") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("\n") ; bodynext = 1 ; next}
/^\.UH/	{print ""; printf("<Heading>\n") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("\n") ; bodynext = 1 ; next}
/^\.TI/	{print ""; printf("<1Heading>\n") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("\n") ; bodynext = 1 ; next}
/^\.ip/	{print ""; printf("<Definition>\n") ;
         for (i=2; i <= NF-1; i++) { printf("%s ", $i); };
         printf("\t") ; bodynext = 0 ; next}
#
# Commands to convert displays
/^\.(c/	{print ""; printf("<Commands>\n") ;
         eachline++ ; next}
/^\.)c/	{print ""; printf("<LBody>\n") ;
         eachline-- ; next}
/^\.(d/	{print ""; printf("<Commands>\n") ;
         eachline++ ; next}
/^\.)d/	{print ""; printf("<LBody>\n") ;
         eachline-- ; next}
#
# Commands to convert index entries
/^\.IE/	{printf("<Marker <MType 10> <MText `") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("'>>") ; bodynext = 0 ; next}
/^\.Ir/	{printf("<Marker <MType 10> <MText `") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("'>>") ; bodynext = 0 ; next}
/^\.Ie/	{printf("<Marker <MType 11> <MText `") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("'>>") ; bodynext = 0 ; next}
/^\.Id/	{printf("<Marker <MType 12> <MText `") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("'>>") ; bodynext = 0 ; next}
/^\.Sr/	{printf("<Marker <MType 13> <MText `") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("'>>") ; bodynext = 0 ; next}
/^\.Se/	{printf("<Marker <MType 14> <MText `") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("'>>") ; bodynext = 0 ; next}
/^\.Sd/	{printf("<Marker <MType 15> <MText `") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("'>>") ; bodynext = 0 ; next}
#
# Commands to convert fonts
/^\.c/	{printf("<Class>") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("<Norm>\n") ; bodynext = 0 ; next}
/^\.I/	{printf("<Italic>") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("<Plain>\n") ; bodynext = 0 ; next}
/^\.i/	{printf("<Italic>") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("<Plain>\n") ; bodynext = 0 ; next}
/^\.BO/	{printf("<Bold>") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("<Plain>\n") ; bodynext = 0 ; next}
/^\.b/	{printf("<Bold>") ;
         for (i=2; i <= NF; i++) { printf("%s ", $i); };
         printf("<Plain>\n") ; bodynext = 0 ; next}
#
# Miscellaneous conversions
/^\.SP/	{bodynext = 1 ; next} # make space same as paragraph
/^\.sp/	{bodynext = 1 ; next} # make space same as paragraph
/^\.nf/	{eachline++ ; next }  # no fill
/^\.fi/	{eachline-- ; next }  # fill again
/^\.ce/	{printf("<Display>\n") ; bodynext = 0; next}
/^\.\\\\"/	{bodynext = 1 ; next}  # scratch comment. Extra \\ from sed
/^\./	{printf("\n"); print ; bodynext = 1; next} # other cmds stay in
	{ if ( eachline > 0) printf("\n");
          if (bodynext > 0) {print defaultparagraph ;
                             bodynext-- ;} ;
          print ; next}
