#ptmakemenu.awk -- Makes simple menu for HTML files, based on each file's <title>,
#             and can make simple table of contents based on <h1>-<h6> headings.
#
# Based on /usr/sww/share/lib/htmlchek-4.1/makemenu.awk
#
#
#Typical use:
#
# awk -f makemenu.awk [options] infiles.html > menu.html
#
#    Where command-line options have the form "option=value".  The possible
# options are title="...", toc=1, and dirprefix="..." and should appear on the
# command line _before_ the names of files to be processed.
#
#    The menu contains a list of <A HREF="file.html">....</A> elements: the
# content of each of these menu items is taken from that of the
# <TITLE>...</TITLE> element of the corresponding file.
#
#    The title="..." option specifies the title of the menu itself:
#
# awk -f makemenu.awk title="Menu for HTML files" *.html > menu.html
#
#    To make a menu of all the files in one's personal hierarchy under Unix:
#
# cd $HOME/public_html
# awk -f makemenu.awk title="My Files" `find . -name \*.html -print` > menu.html
#
#    The toc=1 command-line option attempts to construct a table of contents
# for each file as part of the menu, based on the <H1>-<H6> headings in the
# file.  If there are links inside headings, then makemenu.awk will attempt to
# preserve the validity of <A HREF="..."> references, and transform an
# <A NAME="..."> into an <A HREF="..."> link to the heading from the menu file;
# however, makemenu.awk is limited by the fact that it does not examine each
# <A> tag in a heading individually, but only does global search-and-replace
# operations on the whole <Hn>...</Hn> element (for this reason, the values
# of <A HREF=> and <A NAME=> are only operated on if they are quoted).
#
#    A dirprefix="..."  option can also be specified on the command line; this
# specifies a string which is prefixed to filenames, and which can be used to
# convert local filesystem references (relative URL's) to absolute URL's.
# An example:
#
# awk -f makemenu.awk dirprefix=http://myhost.edu/~myself/ *.html > menu.html
#
#    This program is rather simple-minded; if an HTML file does not have a
# <TITLE>...</TITLE> element, it will not appear in the menu.  If the closing
# </TITLE> tag is not present, it will try to stuff the whole remaining text of
# the file into the menu.  The closing `>' character of the <TITLE>, </TITLE>,
# <H1>-<H6> and </H1>-</H6> tags should not be on a different line from the
# rest of the tag.  Also, multiple headings should not be contained on a single
# line.  This is not an error-checking program, and illegal HTML input may
# result in incorrect HTML output.
#
#    On some systems, non-archaic awk may actually be named ``nawk''.  The
# ``gawk'' interpreter freely-available from the FSF GNU project is more robust
# than some vendor-supplied awk/nawk interpreters.
#
# Copyright 1995 by H. Churchyard, churchh@uts.cc.utexas.edu -- freely
# redistributable.
#
#  Version 1.0 12/94?? -- Was for my personal use only.
#  Version 1.1 1/8/95 -- Made more general, added documentation comments.
#  Version 1.2 1/12/95 -- Added heading-to-Table-of-Contents stuff.  Included
# in htmlchek 4.0 release.
#
BEGIN{accum="";haccum=""}
#
{if (FNR==1)
   {
    if (NR==1)
      {if (!title) {title="Menu for HTML files"};
	      print "<!--TOC below generated by ~ptdesign/doc/bin/addtoc -->";
	      #print "<ul>";
      # print "<html><head><title>" title "</title></head>";
      # print "<body><h1>" title "</h1><hr><ul>"}
      }
    #else {if (toc) {liout()}};

    # Set skipfirstheader=1 when we call gawk if we want to skip the
    # first header in a file
    if (skipfirstheader==1)
	{firstheader=1;}
	# print "<--",FILENAME,FNR,NR,firstheader,"-->";
    if (toc) {liout()};
    hlevel=1}}
#
# This section deals with the title, but we ignore it.
#
#/<[Tt][Ii][Tt][Ll][Ee][^<>]*>/,/<\/[Tt][Ii][Tt][Ll][Ee][^<>]*>/{
# line=$0;
# sub(/^.*<[Tt][Ii][Tt][Ll][Ee][^<>]*>/,"",line);
# x=sub(/<\/[Tt][Ii][Tt][Ll][Ee][^<>]*>.*$/,"",line);
# accum=(accum " " line);
# if (x) {
#   if (toc) {liout()};
#   fn=FILENAME;sub(/^\.\//,"",fn);
#   sub(/^  */,"",accum);sub(/  *$/,"",accum);
#   #print " <LI><A HREF=\042" dirprefix fn "\042>" accum "</A> <tt>(" fn ")</tt>";
#   accum="";}}
#
/<[Hh][2-6][^<>]*>/,/<\/[Hh][2-6][^<>]*>/{
 if (toc)
   {if (match($0,/<[Hh][2-6]/)!=0)
      {newhlevel=substr($0,(RSTART+2),1);
       #print "<!-- toc:",newhlevel,"-",hlevel,"-->";
       if (newhlevel>hlevel)
         {printf "%" ((newhlevel*2)+1) "s","";
          for (i=(newhlevel-hlevel);i>=1;--i) {printf "<UL>"};
          printf "\n"}
        else {if (newhlevel<hlevel)
		{
		 printf "%" ((hlevel*2)+1) "s","";
                 for (i=(hlevel-newhlevel);i>=1;--i) {printf "</UL>"};
                 printf "\n"}};
       hlevel=newhlevel};
    line=$0;
    sub(/^.*<[Hh][2-6][^<>]*>/,"",line);
    x=sub(/<\/[Hh][2-6][^<>]*>.*$/,"",line);
    haccum=(haccum " " line);
    if (x)
      {fn=FILENAME;sub(/^\.\//,"",fn);
       sub(/^  */,"",haccum);sub(/  *$/,"",haccum);
       # The following code attempts to preserve the validity of HREF's,
       # and transform <A NAME>'s into HREF's where possible, but it's kind
       # of lame because it doesn't examine each <A> tag individually.
       if (haccum~/<[Aa]/) {
       gsub(/[ \t]*=[ \t]*\042/,"=\042",haccum);
       z=gsub(/[Hh][Rr][Ee][Ff]=\042\043/,("HREF=\042" dirprefix fn "\043"),haccum);
       if ((!z)&&(haccum!~/[Hh][Rr][Ee][Ff]=\042[^\042]*[:\057][^\042]*\042/))
         {gsub(/[Hh][Rr][Ee][Ff]=\042/,("HREF=\042" dirprefix),haccum)};
       xxx=0;if (haccum!~/[Hh][Rr][Ee][Ff]=\042/) {xxx=1}
else {if (haccum!~/<[Aa][^<>]*[Hh][Rr][Ee][Ff][ \t]*=[^<>]*[Nn][Aa][Mm][Ee][ \t]*=[^<>]*>/)
{if (haccum!~/<[Aa][^<>]*[Nn][Aa][Mm][Ee][ \t]*=[^<>]*[Hh][Rr][Ee][Ff][ \t]*=[^<>]*>/)
                 {xxx=1}}};
       if (xxx)
         {gsub(/[Nn][Aa][Mm][Ee]=\042/,("HREF=\042" dirprefix fn "\043"),haccum)}};
       # </lame>
	# remove any trailing space from the tag
	gsub(/ \042>/,"\042>",haccum);
       if (firstheader == 1) 
	{firstheader=0}
       else 
        {printf "%" ((hlevel*2)+1) "s<LI>%s\n","",haccum;}
       haccum=""}}}
#
END{if (NR>0)
      {if (toc) {liout()};
	print "<hr>";
	print "<!-- Above TOC generated by ~ptdesign/doc/bin/addtoc -->";
       #print "</ul>";
       #print "<!-- Replace this comment with your signature stuff -->";
       #print "</body></html>"
	}}
#
function liout() {
 if (hlevel>1)
   {printf "%" ((hlevel*2)+1) "s","";
	#print "<!-- liout hlevel=",hlevel,"-->";
    for (i=hlevel;i>=2;--i) {printf "</UL>"};
    printf "\n"};
 hlevel=1}
##EOF
