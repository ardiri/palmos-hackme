#
# @(#)Makefile
#
# Copyright 2000, Aaron Ardiri (mailto:aaron@ardiri.com)
# All rights reserved.
#
# This  file was generated  as part of the "|HaCkMe|" system extension
# that has been specifically designed for the Palm Computing Platform.
#
#   http://www.palm.com/
#
# The contents of this file are confidential and proprietary in nature
# ("Confidential Information"). Redistribution or modification without
# prior consent of the original author(s) is prohibited.
#

# compiler commands
PREFIX   =
CC       = $(PREFIX)m68k-palmos-gcc
PILRC    = $(PREFIX)pilrc
BUILDPRC = $(PREFIX)build-prc

# source / outputs
EXEC     = hackme
OBJS     = palm.o

# compiler flags
CCFLAGS = -O2 -Wall -palmos3.5

# compile requirements
all:
	make $(EXEC).prc

$(EXEC).prc: $(EXEC) $(EXEC).def bin.stamp
	$(BUILDPRC) $(EXEC).def $(EXEC) *.bin
	make clean
	cp $(EXEC).prc ..

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(CCFLAGS) -o $(EXEC)

bin.stamp: $(EXEC).rcp
	$(PILRC) -q $(EXEC).rcp

# compile rules
.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CCFLAGS) $<

# clean-up funtions
clean:
	rm -f *.[oa] *.bin *~ $(EXEC)

distclean:
	rm -f *.[oa] *.bin *.[pg]rc *~ $(EXEC)
