################################################################################
# GNUmakefile for the LUXSim2evt
#
# Change log:
# 11 May 2015 - Removed the -g flag from the compilation, as well as the -gstabs
#               flag from the linking (Kareem)
# ## Month 2010 - Initial submission (Michael Woods)
################################################################################

CC			 = g++
CCFLAGS		 = -O3 -Wall

COMPILEJOBS	= LUXSim2evt

SOURCES     = LUXSim2evt.cc LUXSim2evtMethods.cc LUXSim2evtPulse.cc LUXSim2evtTrigger.cc LUXSim2evtReader.cc XMLtoVector.cc
HEADERS     = LUXSim2evt.hh LUXSim2evtMethods.hh LUXSim2evtPulse.hh LUXSim2evtTrigger.hh LUXSim2evtReader.hh XMLtoVector.hh

ALLFLAGS	= $(CCFLAGS) $(LDFLAGS) $(OSFLAGS) $(INCLUDE)
ALLLIBS		= $(LIBDIRS)

All:		$(COMPILEJOBS)

LUXSim2evtBaseline.hh:		LUXSim2evtLibraryGen.cc
		g++ LUXSim2evtLibraryGen.cc -o libGen
		./libGen

LUXSim2evt: 		$(SOURCES) $(HEADERS) LUXSim2evtBaseline.hh
			$(CC) $(ALLFLAGS) $(ALLLIBS) $(SOURCES) -o LUXSim2evt

debug: 		$(SOURCES) $(HEADERS)
			$(CC) -save-temps $(ALLFLAGS) $(ALLLIBS) $(SOURCES) -O0 -o LUXSim2evt

neat:
		rm -rf *.o

clean:
		rm -rf *.o LUXSim2evt libGen LUXSim2evtBaseline.hh

run:
		@#./LUXSim2evt LUXSimNest/Nest_LUXSim2evt/LUXOut50.bin
		./LUXSim2evt S1S2_ForPaper1110482182.bin
