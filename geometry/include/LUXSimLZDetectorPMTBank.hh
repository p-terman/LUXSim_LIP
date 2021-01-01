/*	LUXSimLZDetectorPMTBank.hh
*	
*	This is the header file to define the LZDetector bank of R11410-model PMTs.
*	
*	2012-05-28 - Initial submission (Dave and Kareem)
*       2013-09-06 - This is now adds all the changes needed for design with 
*                    1 Ti plate and 241 PMTS 
*                    each with 3 rods and a base added at the bottom of the PMT.  The truss
*                    is also implemented.  Again see 
*                    http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LZSolidModels/LZ_August_2013_CAD_Model_Tour_20130817.pdf (Monica)
*
*	
*/

#ifndef LUXSimLZDetectorPMTBank_HH
#define LUXSimLZDetectorPMTBank_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimR11410PMT.hh"

//
//	Class forwarding
//
class G4Material;
class G4LogicalVolume;
class LUXSimDetectorComponent;
class LUXSimMaterials;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimLZDetectorPMTBank
{
		
	public:
		LUXSimLZDetectorPMTBank( G4Material *, const char *, G4double );
		~LUXSimLZDetectorPMTBank();
		inline G4LogicalVolume *GetLogicalVolume() { return logicalVolume; };

		inline LUXSimR11410PMT *GetPMT( G4int index ) { return PMT[index]; };
		inline G4int GetNumPMTs() { return numPMTs; };
		inline LUXSimDetectorComponent *GetPMTPhys( G4int index )
				{ return PMTs[index]; };
		inline LUXSimDetectorComponent *GetTopPlate() { return TopPlate; };
		inline LUXSimDetectorComponent *GetBottomPlate() { return BottomPlate;};
  inline LUXSimDetectorComponent *GetTruss() {return Truss;};
  inline LUXSimDetectorComponent *GetXeSkinLiner() { return XeSkinLiner;};
  inline LUXSimDetectorComponent *GetPMTBase( G4int index) { return PMTsBase[index];};
  inline LUXSimDetectorComponent *GetPMTRod1( G4int index) { return PMTsRod1[index];};
  inline LUXSimDetectorComponent *GetPMTRod2( G4int index) { return PMTsRod2[index];};
  inline LUXSimDetectorComponent *GetPMTRod3( G4int index) { return PMTsRod3[index];};

		inline G4LogicalVolume *GetTeflonLinerLogical()
				{ return teflonLinerLogical;};
		
		inline G4double GetHeight() { return fullVolumeHeight; };
                inline G4double GetPMTHeight() { return pmtHeight; };
		inline G4double GetBankLinerThickness() { return ptfeLinerThickness; };

	private:
		
		G4LogicalVolume *logicalVolume;
		LUXSimMaterials *luxMaterials;
		G4int numPMTs;
		LUXSimR11410PMT *PMT[241];
		LUXSimDetectorComponent *PMTs[241];
		LUXSimDetectorComponent *TopPlate;
		LUXSimDetectorComponent *BottomPlate;
  LUXSimDetectorComponent *PMTsBase[241];
  LUXSimDetectorComponent *PMTsRod1[241];
  LUXSimDetectorComponent *PMTsRod2[241];
  LUXSimDetectorComponent *PMTsRod3[241];
  LUXSimDetectorComponent *XeSkinLiner;
  LUXSimDetectorComponent *Truss;
		G4LogicalVolume *teflonLinerLogical;
  
  G4double fullVolumeHeight;
		G4double pmtHeight;
		G4double ptfeLinerThickness;
};

#endif
