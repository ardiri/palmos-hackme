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
MSEG     = $(EXEC)-sections.o $(EXEC)-sections.ld
OBJS     = palm.o hackme.o $(MSEG)

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

$(MSEG): $(EXEC).def
	m68k-palmos-multigen $(EXEC).def
	$(CC) -c -o $(EXEC)-sections.o $(EXEC)-sections.s

bin.stamp: $(EXEC).rcp
	$(PILRC) -q $(EXEC).rcp

syncbin.stamp: $(EXEC)_sync.rcp
	$(PILRC) -q $(EXEC)_sync.rcp

# compile rules
.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CCFLAGS) $<

# clean-up funtions
clean:
	rm -f *.[oa] *sections.* *.bin *.grc *~ $(EXEC)

distclean:
	rm -f *.[oa] *section.s* *.bin *.pdb *.[pg]rc *~ $(EXEC)
