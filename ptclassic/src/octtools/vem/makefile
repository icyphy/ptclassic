# Version Identification:
# @(#)makefile	1.44 05/25/98
# Copyright (c) 1990-1998 The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
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
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY

# Makefile for Ptolemy vem

# Important!  Do not run this makefile while in ~ptolemy/src/octtools/vem.
# Instead run in from ~ptolemy/obj.$PTARCH (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

ROOT	      = ../../..
# Path (absolute or relative) to the object directory root
#OBJDIR=..
# VPATH points to the "real" source directory
VPATH         = $(ROOT)/src/octtools/vem

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# Oct Tools libraries.
OCTLIBS = -loh -lrpc -llist -ltr -lutility -lst -lerrtrap -luprintf -lport

# We use "frozen" Oct libraries so we do not depend on them.  Mistake?
LIBFILES = 

VERSION=0.5-development

# Rule for compiling with g++
.SUFFIXES: .cc
.cc.o:
	$(CPLUSPLUS) $(GPPFLAGS) $(INCL) -c $<

DIRS=bags buffers dialogs dispatch doc drawing drs lib main \
	options patterns physical properties rpc schematic select strings \
	symbolic utility windows

# "leaf" directories with a make.template
MTDIRS = bags buffers dialogs dispatch doc drawing drs lib main \
	options patterns physical properties rpc schematic select strings \
	symbolic utility windows

#skip bitmaps, doc, lib
LIBS=bags/libbags.$(LIBSUFFIX)  buffers/libbuffers.$(LIBSUFFIX) \
	dialogs/libdialogs.$(LIBSUFFIX) dispatch/libdispatch.$(LIBSUFFIX) \
	drawing/libdrawing.$(LIBSUFFIX) drs/libdrs.$(LIBSUFFIX) \
	main/libmain.$(LIBSUFFIX) \
	options/liboptions.$(LIBSUFFIX) patterns/libpatterns.$(LIBSUFFIX) \
	physical/libphysical.$(LIBSUFFIX) \
	properties/libproperties.$(LIBSUFFIX) rpc/librpc.$(LIBSUFFIX) \
	rpc/librpcserver.$(LIBSUFFIX) \
	schematic/libschematic.$(LIBSUFFIX) select/libselect.$(LIBSUFFIX) \
	strings/libstrings.$(LIBSUFFIX) symbolic/libsymbolic.$(LIBSUFFIX) \
	utility/libutility.$(LIBSUFFIX) windows/libwindows.$(LIBSUFFIX) 

# All the .o files in the LIBS above.
OBJECTS=bags/bags.o buffers/bufinternal.o \
	buffers/change.o buffers/general.o buffers/layers.o \
	buffers/open.o buffers/save.o dialogs/vemDMhelp.o \
	dispatch/commands.o dispatch/bindings.o \
	drawing/attributes.o drawing/display.o drawing/polymod.o \
	drs/init_change.o drs/undo.o main/browser.o \
	main/vem.o main/cursors.o main/errors.o main/interface.o \
	main/basic.o main/defaults.o main/message.o main/xvals.o \
	options/opts.o options/layers.o options/util.o patterns/tech.o \
	patterns/fill_pat.o properties/property.o \
	physical/createmod.o physical/selmod.o physical/terminal.o \
	physical/label.o schematic/schemCreate.o \
	schematic/schemPath.o schematic/schemMvCp.o \
	select/selection.o select/hilite.o select/selset.o \
	strings/str.o utility/openwin.o utility/other.o \
	utility/parse.o utility/octassist.o utility/measure.o \
	utility/print.o utility/color.o symbolic/se_create.o \
	symbolic/se_cseg.o symbolic/se_del.o symbolic/se_fterm.o \
	symbolic/se_init.o symbolic/se_inst.o symbolic/se_movecopy.o \
	symbolic/se_path.o symbolic/se_repl.o symbolic/se_reconn.o \
	symbolic/se_select.o symbolic/se_util.o windows/argsel.o \
	windows/create_modify.o windows/event_draw.o \
	windows/layers.o windows/objsel.o windows/options.o \
	windows/queue.o windows/table.o windows/titlebar.o \
	windows/trans.o windows/wininternal.o

# Objects (*.o and *.$(LIBSUFFIX) files) that are locally produced
VEMOBJECTS =	$(OBJECTS) dialogs/libdialogs.$(LIBSUFFIX) \
	rpc/librpcserver.$(LIBSUFFIX)

VEMLIBS = -Lrpc -lrpcserver -Ldialogs -ldialogs \
	-L$(OCTLIBDIR) -lts -llel -lfc -lsymbolic -lvulcan \
	-lharpoon -lfang -lregion -lkd -lgu -lda -llist -loh -lvov -ltap \
	-loptions -loct -ltr -lst -lxpa -lfb -ldds -lMaxport -lTable \
	-lTgl -lutility -lport -lerrtrap -luprintf -lst \
	$(X11_LIBSPEC) -lXaw -lXmu -lXt $(X11EXT_LIBSPEC) -lX11 $(CSYSLIBS)

BINARY = 	vem$(BINARY_EXT)
BINARY_DEBUG = 	vem.debug$(BINARY_EXT)
all: $(BINARY) $(BINARY_DEBUG)
$(BINARY): include $(VEMOBJECTS)
	rm -f main/version.o
	(cd main; $(MAKE) $(MFLAGS) $(MAKEVARS) \
		VPATH=../../../../src/octtools/vem/main version.o)
	$(CLINKER) $(LINKFLAGS) -o $@ main/version.o $(OBJECTS) $(VEMLIBS)

$(BINARY_DEBUG): include $(VEMOBJECTS)
	rm -f main/version.o
	(cd main; $(MAKE) $(MFLAGS) $(MAKEVARS) \
		VPATH=../../../../src/octtools/vem/main version.o)
	$(CLINKER) $(LINKFLAGS_D) -o $@ main/version.o \
		$(OBJECTS) $(VEMLIBS)

vem.debug.purify: include $(VEMOBJECTS)
	rm -f main/version.o
	(cd main; $(MAKE) $(MFLAGS) $(MAKEVARS) \
		VPATH=../../../../src/octtools/vem/main version.o)
	$(PURIFY) $(CLINKER) $(LINKFLAGS_D) -o $@ main/version.o \
		$(OBJECTS) $(VEMLIBS)

# This target makes sure all the makefiles are current
makefiles:
	@for x in $(MTDIRS); do \
		cd $$x ; \
		echo making makefile in $$x ; \
		$(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/octtools/vem/$$x makefile ;\
		cd .. ; \
	done



# This rule uses GNU make specific directives $(@D) $(@F) 
$(VEMOBJECTS):
	(cd $(@D); \
		$(MAKE) $(MFLAGS) $(MAKEVARS) \
		VPATH=../../../../src/octtools/vem/$(@D) $(@F)  ;\
	)

#rpc/librpcserver.$(LIBSUFFIX) dialogs/libdialogs.$(LIBSUFFIX) \

INC_FILES = \
	../bags/bags.h \
	../buffers/buffer.h \
	../dialogs/vemDM.h \
	../dispatch/bindings.h ../dispatch/commands.h \
	../drawing/attributes.h ../drawing/display.h \
	../drs/drs.h \
	../main/ansi.h ../main/defaults.h ../main/errors.h ../main/general.h \
	../main/message.h ../main/cursors.h ../main/vemMain.h ../main/xvals.h \
	../options/opts.h \
	../patterns/fill_pat.h ../patterns/tech.h \
	../select/hilite.h ../select/selset.h ../select/selection.h \
	../strings/str.h \
	../symbolic/se.h \
	../utility/vemUtil.h \
	../windows/argsel.h ../windows/objsel.h ../windows/windows.h

include:
	if [ ! -h include ]; then \
		rm -rf include; \
		ln -s $(VPATH)/include .; \
	fi
	-mkdir $(VPATH)/include
	@echo -n Linking header files in ./include to actual header files...
	@for file in $(INC_FILES); do\
	    ( cd $(VPATH)/include;  ln -s $$file .; );\
	done
	@echo done

_vem: 
	@for x in $(DIRS); do \
		cd $$x ; \
		echo making all in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/octtools/vem/$$x all ;\
		cd .. ; \
	done

OCTBINDIR =	$(OCTTOOLS)/bin.$(PTARCH)
DESTBIN =	$(OCTBINDIR)/$(BINARY)
DESTBIN_D =	$(OCTBINDIR)/$(BINARY_DEBUG)

$(OCTBINDIR):
	mkdir $@

$(DESTBIN):	$(BINARY) $(OCTBINDIR)
		@echo Installing $(BINARY)
		rm -f $(DESTBIN)
		ln $(BINARY) $(DESTBIN)

$(DESTBIN_D):	$(BINARY_DEBUG) $(OCTBINDIR)
		@echo Installing $(BINARY)
		rm -f $(DESTBIN_D)
		ln $(BINARY_DEBUG) $(DESTBIN_D)

install: $(DESTBIN) $(DESTBIN_D)  $(BINDIR)/$(BINARY) \
		$(BINDIR)/../octtools/bin.$(PTARCH)/debug/vem
	@for x in $(DIRS); do \
		cd $$x ; \
		echo making install in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../../src/octtools/vem/$$x install ;\
		cd .. ; \
	done

# These should rely on OCTBINDIR etc, but the relative paths make life
# difficult.
$(BINDIR)/$(BINARY):
	(cd $(BINDIR); rm -f $(BINARY); ln -s ../octtools/bin.$(PTARCH)/$(BINARY) .)

$(BINDIR)/../octtools/bin.$(PTARCH)/debug/vem:
	if [ ! -d $(BINDIR)/../octtools/bin.$(PTARCH)/debug ]; then \
		mkdir $(BINDIR)/../octtools/bin.$(PTARCH)/debug; fi
	(cd $(BINDIR)/../octtools/bin.$(PTARCH)/debug; ln -s ../$(BINARY_DEBUG) vem)

clean::
	rm -f $(BINARY) $(BINARY_DEBUG)

# cleaner than clean
realclean:	clean


SRCS =		$(OBJECTS:.o=.c) 
lint: $(SRCS) 
	lint -u -Iinclude -Ibitmaps -I../include -I$(OCTTOOLS)/include $^ > lint

sources depend checkjunk sccsinfo clean ::
	@for x in $(DIRS); do \
		cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../../src/octtools/vem/$$x $@ ;\
		cd .. ; \
	done
