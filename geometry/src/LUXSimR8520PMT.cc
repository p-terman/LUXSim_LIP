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
//	GEANT4 includes
//
#include "globals.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"

//
//	LUXSim includes
//
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimR8520PMT.hh"

LUXSimR8520PMT::LUXSimR8520PMT(G4bool buildGeometry){
    LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
    //At the moment, this is just a PMT photocathode
    G4double pmtAnnularSkinWindowThickness = 2.*mm;
    pmtHeight = 2.825*cm;
    G4double pmtAnnularSkinPhotocathodeThickness = 1.*mm;
    G4double photocathodeRadius = 1.18*cm;
    bodyRadius = 1.35*cm;
    G4double bodyThickness = bodyRadius - photocathodeRadius;
    G4double vacuumRadius = bodyRadius - bodyThickness;
    G4double vacuumHeight = pmtHeight - bodyThickness;
    G4double windowRadius = vacuumRadius;
    G4double xOffset,yOffset,zOffset;
    
    if (buildGeometry) {
        //Make the PMT Body, for now out of steel
        G4Tubs *skinPMT_solid = new G4Tubs("skinPMT_solid",0.*cm,bodyRadius,pmtHeight/2.,0.*deg,360.*deg);
        logicalVolume = new G4LogicalVolume(skinPMT_solid,luxMaterials->Steel(),"skinPMT_log");
        logicalVolume->SetVisAttributes(luxMaterials->SteelVis());
        //put vacuum inside
        G4Tubs *skinPMTvacuum_solid = new G4Tubs("skinPMTvacuum_solid",0.*cm,vacuumRadius,vacuumHeight/2.,0.*deg,360.*deg);
        G4LogicalVolume *skinPMTvacuum_log = new G4LogicalVolume(skinPMTvacuum_solid,luxMaterials->Vacuum(),"skinPMTvacuum_log");
        skinPMTvacuum_log->SetVisAttributes(luxMaterials->VacuumVis());
        xOffset = 0.;
        yOffset = 0.;
        zOffset = 0.5*bodyThickness;
        skinPMTvacuum = new LUXSimDetectorComponent(0,G4ThreeVector(xOffset,yOffset,zOffset),skinPMTvacuum_log,"skinPMTvacuum",logicalVolume,0,0,false);
        //do a quartz window at the front
        zOffset = 0.5*(vacuumHeight-pmtAnnularSkinWindowThickness);
        G4Tubs *skinPMTwindow_solid = new G4Tubs("skinPMTwindow_solid",0.*cm,windowRadius,pmtAnnularSkinWindowThickness/2.,0.*deg,360.*deg);
        G4LogicalVolume *skinPMTwindow_log = new G4LogicalVolume(skinPMTwindow_solid,luxMaterials->Quartz(),"skinPMTwindow_log");
        skinPMTwindow_log->SetVisAttributes(luxMaterials->QuartzVis());
        skinPMTwindow = new LUXSimDetectorComponent(0,G4ThreeVector(xOffset,yOffset,zOffset),"skinPMTwindow",skinPMTwindow_log,skinPMTvacuum,0,0,false);
        //photocathode
        zOffset = (-0.5*pmtAnnularSkinPhotocathodeThickness);
        G4Tubs *skinPMTphotocathode_solid = new G4Tubs("skinPMTphotocathode_solid",0.*cm,photocathodeRadius,pmtAnnularSkinPhotocathodeThickness/2.,0.*deg,360.*deg);
        G4LogicalVolume *skinPMTphotocathode_log = new G4LogicalVolume(skinPMTphotocathode_solid,luxMaterials->Quartz(),"skinPMTphotocathode_log");
        G4VisAttributes *photocathodeVis = new G4VisAttributes(G4Color::Magenta());
        skinPMTphotocathode_log->SetVisAttributes(photocathodeVis);
        skinPMTphotocathode = new LUXSimDetectorComponent(0,G4ThreeVector(xOffset,yOffset,zOffset),"skinPMTphotocathode",skinPMTphotocathode_log,skinPMTwindow,0,0,false);
    }
}
LUXSimR8520PMT::~LUXSimR8520PMT(){}