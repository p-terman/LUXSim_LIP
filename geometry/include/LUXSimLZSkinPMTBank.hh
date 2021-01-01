/*	LUXSimLZSkinPMTBank.hh
 *
 *	This is the code to define the LZDetector skin bank of R8520-model PMTs.
 *
 *
 ********************************************************************************
 *
 *	Change log
 *
 *  2015-06-12 Initial Sumbission (Jeremy M.)
 *
 ********************************************************************************
 */

#ifndef ____LUXSimLZSkinPMTBank_HH
#define ____LUXSimLZSkinPMTBank_HH 1

#endif /* defined(____LUXSimLZSkinPMTBank__) */

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimR8520PMT.hh"

//
//	Class forwarding
//
class G4Material;
class G4LogicalVolume;
class LUXSimDetectorComponent;
class LUXSimMaterials;

class LUXSimLZSkinPMTBank{
public:
    LUXSimLZSkinPMTBank(const char *prefix, G4double innerRadius, G4double teflonThickness, G4double outerRadius);
    ~LUXSimLZSkinPMTBank();
    inline G4LogicalVolume *GetLogicalVolume(){return logicalVolume;}
    inline LUXSimR8520PMT *GetSkinPMT(G4int index){return PMT[index];}
    inline LUXSimDetectorComponent *GetSkinPMTPhys(G4int index){return PMTs[index];}
    inline G4int GetNumSkinPMTs(){return numPMTs;}
    inline G4double GeSkinPMTHeight(){return fullVolumeHeight;}
    inline LUXSimDetectorComponent *GetPTFEReflector(){return ptfeReflector;}
private:
    G4LogicalVolume *logicalVolume;
    LUXSimMaterials *luxMaterials;
    G4int numPMTs;
    LUXSimR8520PMT *PMT[60];
    LUXSimDetectorComponent *PMTs[60];
    G4double fullVolumeHeight;
    LUXSimDetectorComponent *ptfeReflector;
};