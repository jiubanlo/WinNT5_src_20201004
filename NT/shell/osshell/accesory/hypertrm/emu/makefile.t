#  File: D:\WACKER\emu\makefile.t (Created: 08-Dec-1993)
#
#  Copyright 1993 by Hilgraeve Inc. -- Monroe, MI
#  All rights reserved
#
#  $Revision: 4 $
#  $Date: 12/27/01 11:33a $
#

MKMF_SRCS	=	emudll.c		emu.c		emu_std.c		emustate.c	\
			emu_ansi.c		emu_scr.c	emudisp.c		vt52.c		\
			ansi.c			ansiinit.c	vt100.c 		vt_xtra.c	\
			emuhdl.c		vt100ini.c	vt_chars.c		vt52init.c	\
			viewdini.c		viewdata.c	emudlgs.c		vtutf8ini.c

HDRS		=

EXTHDRS         =

SRCS            =

OBJS		=

#-------------------#

RCSFILES = makefile.t $(SRCS) $(HDRS) emudll.def emudlgs.rc

NOTUSED =  emu_load.c vid2.c vidstate.c vid.h vid.hh trm.c vt_print.c

#-------------------#

%include ..\common.mki

#-------------------#

TARGETS : emudll.dll

#-------------------#

LFLAGS += -DLL -entry:EmuEntry $(**,M\.exp) $(**,M\.lib)

emudll.dll : $(OBJS) emudll.def emudll.exp tdll.lib
	@link $(LFLAGS) $(OBJS:X) -out:$@

#-------------------#
