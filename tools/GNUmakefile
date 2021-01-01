################################################################################
# GNUmakefile for the LUXSim tools
#
# Change log:
# 17 March 2010 - Initial submission (Melinda)
# 20 April 2010 - Added LUXExampleAnalysis (Melinda)
# 22 April 2010 - Added LUXRoot2Dat and a -gstabs for macosx to
#                 get rid of .dSYM bundles created on OS X (Michael Woods)
# 27 Sep 2010 - Added LUXExampleAnalysis and NMDAnalysis to the cleanup section
#		(Kareem)
# 14 Feb 2012 - Removed funny ^M newline characters, and added LUXAsciiReader
#		(Matthew)
# 15 Aug 2012 - Changed order of arguments and corrected LIBDIRS to match
#               needs of latest g++ (Rich)
# 11 May 2015 - Removed the -g flag from the compilation, as well as the -gstabs
#               flag from the linking (Kareem)
# 09 Mar 2016 - Added the BaccRootConverter code (Kareem)
################################################################################

CC			 = g++
CCFLAGS		 = -O2
INCLUDE		 = -I$(ROOTSYS)/include

ifeq ($(OSTYPE),darwin)
OSFLAGS	= -bind_at_load
endif

ifdef ROOTSYS
COMPILEJOBS	= LUXAsciiReader LUXRootReader BaccRootConverter libBaccRootConverterEvent.so LUXExampleAnalysis NMDAnalysis LUXSim2evt
RC     := root-config
ROOTLIBS	= $(shell $(RC) --libs)
LIBDIRS		= -L$(shell $(RC) --libdir)
INCLUDE		= -I$(shell $(RC) --incdir)
LDFLAGS		= $(shell $(RC) --ldflags)
PLATFORM	= $(shell $(RC) --platform)
OBJLIST		= LUXAsciiReader.o LUXRootReader.o BaccRootConverter.o LUXExampleAnalysis.o NMDAnalysis.o
ifeq ($(shell which $(RC) 2>&1 | sed -ne "s@.*/$(RC)@$(RC)@p"),$(RC))
MKARCH := $(wildcard $(shell $(RC) --etcdir)/Makefile.arch)
RCONFIG := $(wildcard $(shell $(RC) --incdir)/RConfigure.h)
endif
ifneq ($(MKARCH),)
include $(MKARCH)
include $(ROOTSYS)/etc/Makefile.arch
endif
endif

ALLFLAGS	= $(CCFLAGS) $(LDFLAGS) $(OSFLAGS) $(INCLUDE)
ALLLIBS		= $(LIBDIRS) $(ROOTLIBS)

All:		$(COMPILEJOBS)

LUXAsciiReader:		LUXAsciiReader.cc
			@echo
			$(CXX) LUXAsciiReader.cc $(ALLFLAGS) $(ALLLIBS) -o LUXAsciiReader

LUXRootReader:          LUXRootReader.cc
			@echo
			$(CXX) LUXRootReader.cc $(ALLFLAGS) $(ALLLIBS) -o LUXRootReader

BaccRootConverter:  BaccRootConverter.cc BaccRootConverterEvent.cc BaccRootConverterEvent.hh BaccRootConverterEvent_dict.cc BaccRootConverterEvent_dict.h BaccRootConverterEvent_LinkDef.h
			@echo
			$(CXX) $(ALLFLAGS) $(ALLLIBS) BaccRootConverter.cc BaccRootConverterEvent.cc BaccRootConverterEvent_dict.cc -o BaccRootConverter

libBaccRootConverterEvent.so: BaccRootConverterEvent.cc BaccRootConverterEvent.hh BaccRootConverterEvent_dict.cc BaccRootConverterEvent_dict.h BaccRootConverterEvent_LinkDef.h
			@echo
			$(LD) $(SOFLAGS) $(LDFLAGS) $(INCLUDE) $(ALLLIBS) -fPIC BaccRootConverterEvent.cc BaccRootConverterEvent_dict.cc -o libBaccRootConverterEvent.so

LUXExampleAnalysis:	LUXExampleAnalysis.cc
			@echo
			$(CXX)  LUXExampleAnalysis.cc $(ALLFLAGS) $(ALLLIBS) -o LUXExampleAnalysis

NMDAnalysis:		NMDAnalysis.cc
			@echo
			$(CXX) NMDAnalysis.cc $(ALLFLAGS) $(ALLLIBS) -o NMDAnalysis

.PHONY: LUXSim2evt
LUXSim2evt:
			@echo
			@cd LUXSim2evt && make #-C LUXSim2evt
			@#cd LUXSim2evt && make -B

neat:
		rm -rf *.o

cleanup:
		rm -rf *.o LUXAsciiReader LUXRootReader BaccRootConverter libBaccRootConverterEvent.so LUXExampleAnalysis NMDAnalysis LUXSim2evt/LUXSim2evt
