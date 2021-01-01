//	LUXSimWaterPMTBank.hh

//	This is the header file to define the Muon Veto Bank of 7081 PMTs.

//	04.07.09 - Initial submission (Melinda)

#ifndef LUXSimWaterPMTBank_HH
#define LUXSimWaterPMTBank_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSim7081PMT.hh"
//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimWaterPMTBank
{
		
	public:
		LUXSimWaterPMTBank(/*G4String*/);
		~LUXSimWaterPMTBank();
		
		inline G4LogicalVolume *GetLogicalVolume() { return waterBlock_log; };
		inline LUXSimDetectorComponent *GetPMT( G4int num )
				{ return PMTs[num]; };
		inline LUXSim7081PMT *GetPMTClass( G4int num )
				{ return PMT7081[num]; };
		
	private:
		
		G4LogicalVolume *waterBlock_log;
		LUXSim7081PMT *PMT7081[20];
		LUXSimDetectorComponent *PMTs[20];
		
};

#endif
