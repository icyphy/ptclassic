# Makefile for the modified sim56000 (ModSim56000)
# $Id$
GABRIEL = /usr/users/gabriel
MOTODIR = $(GABRIEL)/vendors/motorola/sun3/sim56000

CFLAGS = -DUNIX -O -DFULLSIM=1 -I$(MOTODIR)

OBJS = ModSim56000.o smem.o ModSim56000_ipc_client.o scrmgr.o mapPins.o

ModSim56000: $(OBJS)
	$(CC) -o ModSim56000 $(OBJS) -lm -lcurses -ltermlib dspwwin.a
