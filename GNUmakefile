################################################################################
# GNUmakefile for the overall LUXSim package
#
# Change log:
# 18 March 2009 - Initial submission (Kareem)
# 17 March 2010 - Added make commands for the tools directory (Kareem)
#
################################################################################

SUBDIRS = generator geometry io management physicslist processing
G4LIB_BUILD_SHARED =

.PHONY: makeall clean

makeall:
	@echo "********************************************************************************"
	@echo " Building LUXSim"
	@echo "********************************************************************************"
	@for dir in $(SUBDIRS); do (echo; echo; echo Building $$dir...; cd $$dir; $(MAKE) "SUBDIRS=$(SUBDIRS)"); done
	@echo; echo; echo Building tools...; cd tools; $(MAKE); cd ..
	@echo
	@echo
	@echo Building LUXSim...
	@cd LUXSim; $(MAKE) "SUBDIRS=$(SUBDIRS)"
	@cd ..
	@rm -f LUXSimExecutable
	@ln -s LUXSim/bin/$(G4SYSTEM)/LUXSim ./LUXSimExecutable

clean:
	@for dir in $(SUBDIRS); do (echo; echo; echo Cleaning $$dir...; cd $$dir; $(MAKE) clean); done
	@echo; echo; echo Cleaning tools...; cd tools; $(MAKE) cleanup; cd ..
	@echo
	@echo
	@echo Cleaning LUXSim...
	rm -rf LUXSim/bin LUXSim/tmp
	@echo Cleaning libraries and executable...
	rm -rf LUXSimLibraries
	rm -f LUXSimExecutable
	@echo Removing extraneous, system-dependent files
	@rm -rf $(shell find . -name .DS_Store)
	@rm -rf $(shell find . -name ._\*)	
	@rm -rf $(shell find . -name g4_\*.wrl)

archive: clean
	@cd ..; tar zcvf LUXSim.tgz LUXSim
