//	LUXSimLZ20PMTBank.hh

//	This is the header file to define the LZ20 bank of 8778-model PMTs.

//	01.19.10 - Initial submission (Melinda)

#ifndef LUXSimLZ20PMTBank_HH
#define LUXSimLZ20PMTBank_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimExample8778PMT.hh"

//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimLZ20PMTBank
{
		
	public:
		LUXSimLZ20PMTBank();
		~LUXSimLZ20PMTBank();
		inline G4LogicalVolume *GetCopperLogical() { return copperBlock_log; };
		inline G4LogicalVolume *GetReflectorLogical() {return reflector_log; };
		inline G4double GetRadiusCheck() { return radiusCheck; };
		inline G4double GetPMTSpace() { return pmtSpace; };

	private:		
		G4LogicalVolume *copperBlock_log;
		G4LogicalVolume *reflector_log;
		G4double radiusCheck;
		G4double pmtSpace;
};

#endif
