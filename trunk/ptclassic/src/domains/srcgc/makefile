# Makefile for SRCGC domain support
#
#  Copyright (c) 1998 SupÅÈlec & Thomson-CSF Optronique.
#  All rights reserved.
#  
#  Permission is hereby granted, without written agreement and without
#  license or royalty fees, to use, copy, modify, and distribute this
#  software and its documentation for any purpose, provided that the
#  above copyright notice and the following two paragraphs appear in all
#  copies of this software.
#  
#  IN NO EVENT SHALL SUPELEC OR THOMSON-CSF OPTRONIQUE BE LIABLE TO ANY PARTY
#  FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
#  ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
#  SUPELEC OR THOMSON-CSF OPTRONIQUE HAS BEEN ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.
#  
#  SUPELEC AND THOMSON-CSF OPTRONIQUE SPECIFICALLY DISCLAIMS ANY WARRANTIES,
#  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
#  PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND SUPELEC OR THOMSON-CSF
#  OPTRONIQUE HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
#  ENHANCEMENTS, OR MODIFICATIONS.
#
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
#   Version: @(#)makefile	1.1 05/26/98
#
#   Authors:  Vincent Legrand, Mathilde Roger, FrÅÈdÅÈric Boulanger
#   E-mail: Frederic.Boulanger@supelec.fr
#   mail: SupÅÈlec - Service Informatique
#   Plateau de Moulon, F-91192 Gif-sur-Yvette cedex
#   www: http://wwwsi.supelec.fr/
#   
#   Thomson: Xavier Warzee <XAVIER.W.X.WARZEE@tco.thomson.fr>
#   

# 
# Important!  Do not run this makefile while in ~ptolemy/src/domains/srcgc.
# Instead run in from ~ptolemy/obj.$PTARCH/domains/srcgc (through the symbolic
# link) -- that way a separate directory containing, say, Sun-3 and Sun-4
# objects can be maintained.

ME = srcgc
ROOT = ../../..
VPATH = $(ROOT)/src/domains/$(ME)

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

include $(ROOT)/mk/topdomain.mk

# Subdirectories with star directories in them
SUBDOMAINDIRS = 

DIRS = kernel stars $(SUBDOMAINDIRS)

include $(ROOT)/mk/domain.mk

