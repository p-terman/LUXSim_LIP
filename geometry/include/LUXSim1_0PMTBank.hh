//	LUXSim1_0PMTBank.hh

//	This is the header file to define the Lux1.0 bank of 8778-model PMTs.

//	04.07.09 - Initial submission (Melinda)

//	03.05.10 - Added PlaceXenonCones and PlacePMTs.  Moved some variable 
//			   declarations here. (Melinda)

//	07.21.11 - Added GetPMT function (Dave)

//      08.16.12 - PlacePMTs now has optional third argument to adjust PMT
//                 numbering to real numbering (Rich)

#ifndef LUXSim1_0PMTBank_HH
#define LUXSim1_0PMTBank_HH 1

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
class LUXSimMaterials;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSim1_0PMTBank
{
		
	public:
		LUXSim1_0PMTBank();
		~LUXSim1_0PMTBank();
		inline G4LogicalVolume *GetCopperLogical() { return copperBlock_log; };
		inline G4LogicalVolume *GetReflectorLogical() {return reflector_log; };
		inline G4LogicalVolume *GetSpaceLogical() {return space_log; };
		inline G4double GetReflectorHeight() {return reflectorHeight; };
		inline LUXSimExample8778PMT *GetPMT(int index) { return PMT8778[index]; };

		void PlaceXenonCones( G4String spaceMaterialStr, 
							  LUXSimDetectorComponent * reflector ); 
		void PlacePMTs( G4LogicalVolume * mother_log,
				G4double zOff, G4bool useRealNumbering=false);

	private:		
		LUXSimMaterials *luxMaterials;

		G4LogicalVolume *copperBlock_log;
		G4LogicalVolume *reflector_log;
		G4LogicalVolume *space_log;
		LUXSimDetectorComponent *reflectorSpace[61];
		G4LogicalBorderSurface * reflectorSpaceSurface[61];
		G4double reflectorHeight;
		LUXSimExample8778PMT *PMT8778[61];
		LUXSimDetectorComponent *PMTs[61];
		G4double pmtSpace;
};

#endif
