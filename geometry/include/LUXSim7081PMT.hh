//	LUXSim7081PMT.hh

//	This is the header file to define the 7081 model PMT by Hamamatsu.

//	04.07.09 - Initial submission (Melinda)
//      05.28.14 - Added Photocathode volume (Scott Ha./Curt)

#ifndef LUXSim7081PMT_HH
#define LUXSim7081PMT_HH 1

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
class LUXSim7081PMT
{
		
	public:
		LUXSim7081PMT();
		~LUXSim7081PMT();
		
		inline G4LogicalVolume *GetLogicalVolume() { return PMT7081_log; };
		inline LUXSimDetectorComponent *GetPMTWindow() { return PMT7081window; };
		inline LUXSimDetectorComponent *GetPMTPhotocathode() { return PMT7081photocathode; };
		
	private:
		
		G4LogicalVolume *PMT7081_log;
		LUXSimDetectorComponent *PMT7081window;
		LUXSimDetectorComponent *PMT7081photocathode;
};

#endif
