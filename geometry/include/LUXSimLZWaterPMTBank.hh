//	LUXSimLZWaterPMTBank.hh

//	This is the header file to define the LZ Veto Bank of R5912 PMTs.

//	05.10.14 - Initial submission (Scott Haselschwardt)

#ifndef LUXSimLZWaterPMTBank_HH
#define LUXSimLZWaterPMTBank_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimR5912PMT.hh"

//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimLZWaterPMTBank
{
		
	public:
		LUXSimLZWaterPMTBank();
		~LUXSimLZWaterPMTBank();
		
		inline G4LogicalVolume *GetLogicalVolume() { return waterBlock_log; };
		inline LUXSimDetectorComponent *GetPMT( G4int num )
				{ return PMTs[num]; };
		inline LUXSimR5912PMT *GetPMTClass( G4int num )
				{ return PMTR5912[num]; };
		
	private:
		
		G4LogicalVolume *waterBlock_log;
		LUXSimR5912PMT *PMTR5912[120];
		LUXSimDetectorComponent *PMTs[120];
		
};

#endif
