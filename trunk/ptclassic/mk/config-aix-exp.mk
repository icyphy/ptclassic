# Copyright (c) 1990-1995 The Regents of the University of California.
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
# $Id$

#	Programmer: Xavier Warzee (Thomson CSF)
#
# Makefile to make symbols externally visible under AIX

.SUFFIXES:    .o .cc .h .pl .chdl .is .exp

.a.exp:
	${PTOLEMY}/mk/mkexps $< > $*.exp
	rm -f $(LIBDIR)/$*.exp
	ln $*.exp $(LIBDIR)

EXP=\
libDC.exp\
libDL.exp\
libHu.exp\
libMSH.exp\
libImage.exp\
libLS.exp\
libPar.exp\
libatm.exp\
libbdf.exp\
libbdfstars.exp\
libcg.exp\
libcg56.exp\
libcg56dspstars.exp\
libcg56stars.exp\
libcg96.exp\
libcg96dspstars.exp\
libcg96stars.exp\
libcgc.exp\
libcgcm5.exp\
libcgcstars.exp\
libcgctcltk.exp\
libcgstars.exp\
libcompileSDF.exp\
libddf.exp\
libddfstars.exp\
libde.exp\
libdeatmstars.exp\
libdestars.exp\
libdetclstars.exp\
libhof.exp\
libhofstars.exp\
libgslider.exp\
libmdsdf.exp\
libmdsdfstars.exp\
libmdsdftclstars.exp\
libmq.exp\
libmqstars.exp\
libptk.exp\
libptolemy.exp\
libsdf.exp\
libsdfatmstars.exp\
libsdfdspstars.exp\
libsdfimagestars.exp\
libsdfcontrib.exp\
libsdfmatrixstars.exp\
libsdfstars.exp\
libsdftclstars.exp\
libsilage.exp\
libsilagestars.exp\
libsproc.exp\
libsprocstars.exp\
libthor.exp\
libthorstars.exp\
libvhdlb.exp\
libvhdlbstars.exp\
libvhdlf.exp\
libvhdlfstars.exp\
libgantt.exp\
libpigi.exp\
libX11.exp

$(EXP): $(LIB)

$(LIBDIR)/$(LIB): $(LIB) $(EXP)
	rm -f $(LIBDIR)/$(LIB)
	ln $(LIB) $(LIBDIR)
	ln $(EXP) $(LIBDIR)
