//	LUXSimR5912PMT.hh

//	This is the header file to define the R5912 model PMT by Hamamatsu.

//	04.07.09 - Initial submission (Melinda)

#ifndef LUXSimR5912PMT_HH
#define LUXSimR5912PMT_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimR5912PMT
{
		
	public:
		LUXSimR5912PMT();
		~LUXSimR5912PMT();
		
		inline G4LogicalVolume *GetLogicalVolume() { return PMTR5912_log; };
		inline LUXSimDetectorComponent *GetPMTWindow() { return PMTR5912window; };
		inline LUXSimDetectorComponent *GetPMTPhotocathode() { return PMTR5912photocathode; };
		
	private:
		
		G4LogicalVolume *PMTR5912_log;
		LUXSimDetectorComponent *PMTR5912window;
		LUXSimDetectorComponent *PMTR5912photocathode;
};

#endif
