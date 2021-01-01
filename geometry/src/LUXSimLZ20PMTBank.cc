//	LUXSimLZ20PMTBank.cc

//	This is the placement code for the PMT bank for LZ20.  
//	Author: Melinda Sweany

//	01.19.10 - Initial submission copied from LUXSim1_0PMTBank.
//			   Changed PMT space placement routine so that all
//			   that needs to be changed for a generic placement
//			   is the number of PMTs to be placed.  Removed 
//			   reflector space code. (Melinda)

#include <sstream>

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Polyhedra.hh"
//
//	LUXSim includes
//
#include "LUXSimLZ20PMTBank.hh"
#include "LUXSimExample8778PMT.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimLZ20PMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZ20PMTBank::LUXSimLZ20PMTBank() {

	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	The PMT bank is the copper block with holes for PMTs

	//	The copper block
	G4double copperRadius = 30.164*cm;
	G4double copperHeight = 7.62*cm;
	G4double copperRInner[2] = {0.*cm,0.*cm};
	G4double copperROuter[2] = {27.25*cm,27.25*cm};
	G4double copperZPlane[2] = {-1.27*cm,1.27*cm};

	G4double pmtRadius = 2.54*cm;
	G4double blockSpace = 0.889*cm;
	G4double totalBlockHeight = 11.049*cm;

	G4VSolid * copperBlock1_solid = new G4Tubs( "copperBlock1_solid",0.*cm,
			copperRadius,copperHeight/2.,0.*deg,360.*deg);
	G4VSolid * copperBlock2_solid = new G4Polyhedra( "copperBlock2_solid",
			0.*deg,360.*deg,12,2,copperZPlane,copperRInner,copperROuter);
	G4VSolid *copperBlockTemp_solid = new G4UnionSolid(
			"copperBlockTemp_solid",copperBlock1_solid,copperBlock2_solid,
			0,G4ThreeVector(0.*cm,0.*cm,blockSpace+copperHeight/2.
			+copperZPlane[1]));
	G4VSolid *copperBlock_solid;
	G4Tubs *pmtSpace_solid = new G4Tubs( "pmtSpace_solid",0*mm, pmtRadius,
			totalBlockHeight, 0.*deg, 360.*deg );

	//	place 61 PMTs spacers in block
	G4double xOff =0., yOff =0., zOff = 0.;
	pmtSpace = 6.0*cm;
	radiusCheck = 28.*cm;	

	G4int numPmtsPlaced = 0;
	G4int row = 1;
	while(numPmtsPlaced < 61){
		G4double pmtPlacementRadius = sqrt(pow(pmtRadius+yOff,2)+pow(pmtRadius+xOff,2));
		if(pmtPlacementRadius < radiusCheck){	
			copperBlock_solid = new G4SubtractionSolid("copperBlock_solid",
					copperBlockTemp_solid, pmtSpace_solid, 0, 
					G4ThreeVector(xOff,yOff,zOff));
			copperBlockTemp_solid = copperBlock_solid;
			numPmtsPlaced++;
			if(yOff!=0){
				copperBlock_solid = new G4SubtractionSolid("copperBlock_solid",
						copperBlockTemp_solid, pmtSpace_solid, 0, 
						G4ThreeVector(xOff,-yOff,zOff));
				copperBlockTemp_solid = copperBlock_solid;
				numPmtsPlaced++;
			}
			if(xOff!=0){
				copperBlock_solid = new G4SubtractionSolid("copperBlock_solid",
						copperBlockTemp_solid, pmtSpace_solid, 0, 
						G4ThreeVector(-xOff,yOff,zOff));
				copperBlockTemp_solid = copperBlock_solid;
				numPmtsPlaced++;
				if(yOff!=0){
					copperBlock_solid = new G4SubtractionSolid("copperBlock_solid",
							copperBlockTemp_solid, pmtSpace_solid, 0, 
							G4ThreeVector(-xOff,-yOff,zOff));
					copperBlockTemp_solid = copperBlock_solid;
					numPmtsPlaced++;
				}
			}
			yOff += pmtSpace;
		}
		else {
			if(row%2) yOff = pmtSpace/2.;
			else yOff = 0.;
			xOff += sqrt(3.)/2 * pmtSpace;
			row++;
		}
	}

	copperBlock_log = new G4LogicalVolume( copperBlockTemp_solid,
			luxMaterials->Copper(), "copperBlock_log" );
	copperBlock_log->SetVisAttributes( luxMaterials->CopperVis() );

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimLZ20PMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZ20PMTBank::~LUXSimLZ20PMTBank() {}
