////////////////////////////////////////////////////////////////////////////////
/*	LUXSimR11410PMT.hh
 *
 * This is the header file to define the R8520 model PMT by Hamamatsu.
 *
 ********************************************************************************
 * Change log
 *
 *	2012-05-28 - Initial submission (Jeremy M)
 *
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef ____LUXSimSkinPMT_HH
#define ____LUXSimSkinPMT_HH 1

#include <stdio.h>

#endif /* defined(____LUXSimSkinPMT__) */

#include "globals.hh"
//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;

class LUXSimR8520PMT{
public:
    LUXSimR8520PMT(G4bool buildGeometry="true");
    ~LUXSimR8520PMT();
    inline G4LogicalVolume *GetLogicalVolume(){return logicalVolume;};
    inline LUXSimDetectorComponent *GetSkinPMTPhotocathode(){return skinPMTphotocathode;}
    inline LUXSimDetectorComponent *GetSkinPMTWindow(){return skinPMTwindow;}
    inline LUXSimDetectorComponent *GetSkinPMTVacuum(){return skinPMTvacuum;}
    inline G4double GetOuterRadius(){return bodyRadius;}
    inline G4double GetSkinPMTHeight(){return pmtHeight;}
private:
    G4LogicalVolume *logicalVolume;
    LUXSimDetectorComponent *skinPMTphotocathode;
    LUXSimDetectorComponent *skinPMTwindow;
    LUXSimDetectorComponent *skinPMTvacuum;
    G4double bodyRadius;
    G4double pmtHeight;
};