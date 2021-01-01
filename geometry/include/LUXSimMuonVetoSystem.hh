//	LUXSimMuonVetoSystem.hh

//	This is the header file to define the Muon Veto System.

//	04.07.09 - Initial submission (Melinda)
//	08 Mar 2012 - Added variable and method for recording height (Dave)
//	23 Apr 2013 - Added a hook for the water tank radius (Kareem)
//      03 Apr 2014 - Added tyvek volume as a detector component (Curt)
//	28 May 2014 - Added LZ veto system option (Scott Ha.)

#ifndef LUXSimMuonVetoSystem_HH
#define LUXSimMuonVetoSystem_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4ThreeVector.hh"

//
//	LUXSim includes
//
#include "LUXSimDetector.hh"

//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;
class LUXSimWaterPMTBank;
class LUXSimLZWaterPMTBank;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimMuonVetoSystem : public LUXSimDetector
{
		
	public:
		LUXSimMuonVetoSystem(  );
		~LUXSimMuonVetoSystem();
		
		inline G4double GetHeight() { return height; };
		inline LUXSimDetectorComponent *GetWaterBank() { return waterBank; };
		
		G4double GetWaterTankRadius() { return waterTankRadius; };
		
	private:
		G4double waterTankRadius;
	
		LUXSimLZWaterPMTBank *LZbank;
		LUXSimWaterPMTBank *bank;
		G4double height;
		LUXSimDetectorComponent *waterBank;
		LUXSimDetectorComponent *tyvek;
		LUXSimDetectorComponent *megaPMTwindow;
                LUXSimDetectorComponent *megaPMTphotocathode;
};

#endif
