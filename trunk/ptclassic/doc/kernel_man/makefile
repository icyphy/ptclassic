# Version $Id$ $Revision$
ROOT= ../..
include $(ROOT)/mk/latexinfo.mk

SRC=	kernel.tex codeGen.tex

all:		ptk.ps ptk.info clean

ptk.info:	ptk.tex $(SRC)

ptk.dvi:	ptk.tex $(SRC) ptk.toc

clean:	ptk.clean
	rm -f ptk.dvi $(SRC:.tex=.aux)
###################
man_html: chapter.tex kernel.tex
	latex2html -external_images -t "Ptolemy 0.5.2 Kernel Manual" chapter.tex

###################
LA2MML=/users/cxh/src/la2mml/la2mml
#LA2MML=/users/tools/bin/la2mml

# To get the example code to work, make the following change to
# tables.c

# *** tables.c	Wed Nov  8 17:08:05 1995
# --- tables.c.new	Sun Jun  8 11:40:47 1997
# ***************
# *** 829,834 ****
# --- 829,837 ----
#   	{ "equation", "%T", "%n%L", "<Equation>", ' ', 'P', MATH },
#   	{ "eqnarray", "%T", "%n%L", "<Equation>", ' ', 'P', MATHARRAY },
#   	{ "eqnarray*", "%T", "%n%L", "<Equation>", ' ', 'P', MATHARRAY },
# + 	{ "example", "%T", "%n%L", "<Code>", 'P', 'P', VERBATIM },
# + 	{ "example*", "%T", "%n%L", "<Code>", 'P', 'P', VERBATIM },
# + 
#   	{ "figure", "%o%n%D+9<SpaceBefore 36pt>\\x11 %n",
#   			"%n%D0%L", "<FirstBody>", ' ', 'P', PARA },
#   	{ "figure*", "%o%n%D+9<SpaceBefore 36pt>\\x11 %n",

ptk.mml: $(LA2MML) kernel.tex codeGen.tex ptk.tex
	$(LA2MML) ptk.tex

ptk2.mml: ptk.mml #makefile
	echo "<MML -- created by la2mml-cxh on `date`" > $@
	echo '>' >> $@
	echo '<!DefineFont ProgramCode' >> $@
	echo '	<Family Courier>' >> $@
	echo '	<pts 11>' >> $@
	echo '>' >> $@
	echo '<!DefineFont ProgramVariable' >> $@
	echo '	<Family Courier>' >> $@
	echo '  <oblique>' >> $@
	echo '	<pts 11>' >> $@
	echo '>' >> $@
	sed \
		-e 's/\\\\alwaysrefill .*//g' \
		-e 's/\\\\code \([^ <]*\)/<ProgramCode>\1<rm>/g' \
		-e 's/\\\\code{\([^ }]*\)}/<ProgramCode>\1<rm>/g' \
		-e 's/\\\\comment .*//g' \
		-e 's/\\\\file \([^ <]*\)/<ProgramCode>\1<rm>/g' \
		-e 's/^iftex$$//g' \
		-e 's/\\\\node .*//g' \
		-e 's/\\\\setfilename .*//g' \
		-e 's/(\\\\pxref \([^)]*\))/<Marker <MType 2> <MText "\1">>/g' \
		-e 's/\\\\pxref \([^)]*\))/<Marker <MType 2> <MText "\1">>/g' \
		-e 's/.*\\\\comment .*//g' \
		-e 's/\\\\tableofcontents .*//g' \
		-e 's/\\\\var \([^ <]*\)/<ProgramVariable>\1<rm>/g' \
		-e 's/\\\\var{\([^ }]*\)}/<ProgramVariable>\1<rm>/g' \
		-e 's/xd3<rm>/xd3 <rm>/g' \
		-e 's/x12<rm>/xd12 <rm>/g' \
		ptk.mml >> $@ 

# Convert from mml to mif.
ptk2.tmp: ptk2.mml
	mmltomif ptk2.mml ptk2.tmp

# Get rid of \} \{ and <em> <rm> which could be in our markers.
ptk2.mif: ptk2.tmp
	(sed -e 's/\\}/}/g' \
		-e 's/\\{/{/g' \
		-e 's/<em>//g' \
		-e 's/<rm>//g' \
		ptk2.tmp > $@)

# Open up the mif we generated and resave it so that we are in the 
# canonical mif format.  This should not be necessary, but it is.
ptk3.mif: ptk2.mif
	echo "Open ptk2.mif" > /tmp/ptk.fmb
	echo "SaveAs m ptk2.mif ptk3.mif" >> /tmp/ptk.fmb
	echo "Quit" >> /tmp/ptk.fmb
	fmbatch -v /tmp/ptk.fmb
	rm -f /tmp/ptk.fmb

# Unfortunately, we need to convert Chapters to 1Headings, 1Headings to
# 2 Headings etc.
ptk4.mif: ptk3.mif #makefile
	(sed \
		-e 's/'\`'3Heading'\''/'\`'4Heading'\''/g' \
		-e 's/'\`'2Heading'\''/'\`'3Heading'\''/g' \
		-e 's/'\`'1Heading'\''/'\`'2Heading'\''/g' \
		-e 's/'\`'Chapter'\''/'\''1Heading'\''/g' \
		ptk3.mif > ptk4.mif)

# Open up sdf.doc and import the file formats from it.
ptk5.mif: ptk4.mif
	echo "Open ptk4.mif" > /tmp/ptk.fmb
	echo "Open ../users_man/sdf.doc" >> /tmp/ptk.fmb
	echo "ImportFormats ptk4.mif ../users_man/sdf.doc" >> /tmp/ptk.fmb
	echo "SaveAs m ptk4.mif ptk5.mif" >> /tmp/ptk.fmb
	echo "Quit" >> /tmp/ptk.fmb
	fmbatch -v /tmp/ptk.fmb
	rm -f /tmp/ptk.fmb

# Place old sed commands here.
foo:
	(sed \
		-e 's/<PgfNumFormat .C:CHAPTER +:\\t.>/<PgfNumFormat '\`'Chapter +:\\t'\''>/g' \
		-e 's/<PgfNumFormat .Chapter <+\\>:\\\\t.>/<PgfNumFormat '\`'Chapter <n+\\>\\\\t'\''>/g' \
		-e 's/<PgfNumFormat .H:<+\\>\\\\t.>/<PgfNumFormat '\`'<n\\>.<n+\\>  < =0 \\>< \\>< \\>< \\>\\\\t'\''>/g' \
		-e 's/<PgfNumFormat .H:<n\\>.<+\\>\\\\t.>/<PgfNumFormat '\`'<n\\>.<n\\>.<n+\\>  < \\>< \\>< \\>'\''>/g' \
		-e 's/<PgfNumFormat .H:<n\\>.<n\\>.<+\\>\\\\t.>/<PgfNumFormat '\`'<n\\>.<n\\>.<n\\>.<n+\\> '\''>/g' \

		-e 's/<PgfNumFormat .C:CHAPTER +:\\t.>/<PgfNumFormat '\`'Chapter <n+\\>\\t'\''>/g' \
		-e 's/<PgfNumFormat .H:+\\t.>/<PgfNumFormat '\`'<n\\>.<n+\\>  < =0 \\>< \\>< \\>< \\>\\t'\''>/g' )

