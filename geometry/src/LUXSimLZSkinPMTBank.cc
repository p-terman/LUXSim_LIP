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


//
//	C/C++ includes
//
#include <sstream>
#include <string>
#include <cmath>

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Polyhedra.hh"
#include "G4Tubs.hh"
#include "G4Material.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Box.hh"

//
//	LUXSim includes
//
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimLZSkinPMTBank.hh"

using namespace std;

#define PI 3.14159265358979312

LUXSimLZSkinPMTBank::LUXSimLZSkinPMTBank(const char *prefix, G4double innerRadius, G4double teflonThickness, G4double outerRadius){
    luxMaterials = LUXSimMaterials::GetMaterials();
    numPMTs = 60;
    
    LUXSimR8520PMT *tempPMT = new LUXSimR8520PMT(false);
    G4double pmtRadius = tempPMT->GetOuterRadius();
    G4double pmtHeight = tempPMT->GetSkinPMTHeight();
    fullVolumeHeight = pmtHeight;
    G4double ptfeHoleRadius = pmtRadius;
    G4double centerRadius = innerRadius+pmtRadius;
    
    delete tempPMT;
    
    G4Tubs *fullVolume_solid = new G4Tubs("fullVolume_solid",innerRadius,outerRadius,fullVolumeHeight/2.,0.*deg,360.*deg);
    stringstream name;
    name << prefix << "SkinPMTVolume_log";
    logicalVolume = new G4LogicalVolume(fullVolume_solid,luxMaterials->LiquidXe(),name.str());
    logicalVolume->SetVisAttributes(luxMaterials->LiquidXeVis());
    
    G4double xOffset;
    G4double yOffset;
    G4double zOffset;
    
    //PTFE reflectors
    G4Tubs *ptfeReflector_solid1 = new G4Tubs("ptfeReflector_solid1",innerRadius,outerRadius,teflonThickness/2.,0.*deg,360.*deg);
    G4Tubs *ptfeReflector_solid2 = new G4Tubs("ptfeReflector_solid2",0.*cm,ptfeHoleRadius,teflonThickness,0.*deg,360.*deg);
    G4VSolid *ptfeReflector_solid3 = ptfeReflector_solid1;
    G4double SkinAngleSpacing = 2.*PI/(double)numPMTs;
    G4double SkinAngle = 0.;
    for (int i = 0; i < numPMTs; i++) {
        xOffset = centerRadius * cos(SkinAngle);
        yOffset = centerRadius * sin(SkinAngle);
        ptfeReflector_solid3 = new G4SubtractionSolid("ptfeReflector_solid3",ptfeReflector_solid3,ptfeReflector_solid2,0,G4ThreeVector(xOffset,yOffset,0.));
        SkinAngle += SkinAngleSpacing;
    }
    G4LogicalVolume *ptfeReflector_log = new G4LogicalVolume(ptfeReflector_solid3,luxMaterials->Teflon(),"ptfeReflector_log");
    ptfeReflector_log->SetVisAttributes(luxMaterials->TeflonVis());
    name.str("");
    name << prefix << "_Skin_PMT_Reflector";
    zOffset = fullVolumeHeight/2. - teflonThickness/2.;
    ptfeReflector = new LUXSimDetectorComponent(0,G4ThreeVector(0,0,zOffset),ptfeReflector_log,name.str(),logicalVolume,0,0,false);
    
    SkinAngle = 0.;
    //place PMTs now
        for (G4int i = 0; i < numPMTs; i++) {
        PMT[i] = new LUXSimR8520PMT(true);
        stringstream number;
        number.width(3);
        number.fill('0');
        number << i+1;
        name.str("");
        name << prefix << "_Skin_PMT_Body_" << number.str();
        xOffset = centerRadius * cos(SkinAngle);
        yOffset = centerRadius * sin(SkinAngle);
        zOffset = fullVolumeHeight/2. - pmtHeight/2.;
        PMTs[i] = new LUXSimDetectorComponent(0,G4ThreeVector(xOffset,yOffset,zOffset),PMT[i]->GetLogicalVolume(),name.str(),logicalVolume,0,0,false);
        SkinAngle += SkinAngleSpacing;
        
        //Rename individual components
        name.str("");
        name << prefix << "_Skin_PMT_Vacuum_" << number.str();
        PMT[i]->GetSkinPMTVacuum()->SetName(name.str());
        name.str("");
        name << prefix << "_Skin_PMT_Window_" << number.str();
        PMT[i]->GetSkinPMTWindow()->SetName(name.str());
        name.str("");
        name << prefix << "_Skin_PMT_Photocathode_" << number.str();
        PMT[i]->GetSkinPMTPhotocathode()->SetName(name.str());
    }
}
