//	LUXSimPyramid.cc

//	This is the code defining the inverted steel pyramid.
//	It consists of 6 layered steel plates. CAD posted at
//	http://teacher.pas.rochester.edu:8080/wiki/pub/Lux/
//		DesignSpecificationsPyramid/Pyramid_drawing.pdf
//	D. Malling (David_Malling@brown.edu)


//	8 Mar 2012 - Created (Dave)
//
//	C/C++ includes
//
#include <sstream>

//
//	GEANT4 includes
//
#include "G4Box.hh"
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

//
//	LUXSim includes
//
#include "LUXSimDetector.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimPyramid.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimPyramid
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPyramid::LUXSimPyramid()
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	Modeling pyramid as cylinders rather than octagons
	//	because I really don't trust Geant4 special volume
	//	shapes right now. Will make proper adjustments to 
	//	preserve total surface area.
	G4double pyramidBlockThickness = 5.08*cm;	// 2" steel plates
	G4double pyramidBlock1Radius = 20.4*cm;	//	Will actually model this as a square 
											//	as shown in CAD
	G4double pyramidBlock2Radius = 72.0*cm;	//	Will model as disk
	G4double pyramidBlock3Radius = 121.*cm;	// 	Will model as disk
	G4double pyramidBlock4Radius = 162.*cm;	//	Will model as disk
	G4double pyramidBlock5Radius = 203.*cm;	//	Will model as disk
	G4double pyramidBlock6Radius = 257.*cm;	//	Will model as disk
	
	height = 6*pyramidBlockThickness;
	
	// Create little universe for pyramid to live in
	G4Tubs *pyr_universe = new G4Tubs( "pyr_universe", 
		0.*cm, pyramidBlock6Radius, height/2, 
		0.*deg, 360.*deg);
	logicalVolume = new G4LogicalVolume( pyr_universe, 
		luxMaterials->Vacuum(), "pyr_logical" );
	logicalVolume->SetVisAttributes( luxMaterials->VacuumVis() );
	
	// Create block 1 (as a cube, as in reality)
	G4Box *block1Solid = new G4Box( "block1_solid", 
		pyramidBlock1Radius, pyramidBlock1Radius, pyramidBlockThickness/2 );
	G4LogicalVolume *block1Logical = new G4LogicalVolume( block1Solid, 
		luxMaterials->Water(), "block1_logical" );
	block1Logical->SetVisAttributes( luxMaterials->SteelVis() );
	new LUXSimDetectorComponent(0,
		G4ThreeVector(0.*cm,0.*cm,(-height+pyramidBlockThickness)/2), 
		block1Logical, "Pyramid Block 1", logicalVolume, 
		0, 0, false);
	
	// Create block 2 (as a disk)
	G4Tubs *block2Solid = new G4Tubs( "block2_solid", 
		0.*cm, pyramidBlock2Radius, pyramidBlockThickness/2, 
		0.*deg, 360.*deg );
	G4LogicalVolume *block2Logical = new G4LogicalVolume( block2Solid, 
		luxMaterials->Water(), "block2_logical" );
	block2Logical->SetVisAttributes( luxMaterials->SteelVis() );
	new LUXSimDetectorComponent(0,
		G4ThreeVector(0.*cm,0.*cm,(-height+pyramidBlockThickness)/2+pyramidBlockThickness), 
		block2Logical, "Pyramid Block 2", logicalVolume, 
		0, 0, false);
	
	// Create block 3 (as a disk)
	G4Tubs *block3Solid = new G4Tubs( "block3_solid", 
		0.*cm, pyramidBlock3Radius, pyramidBlockThickness/2, 
		0.*deg, 360.*deg );
	G4LogicalVolume *block3Logical = new G4LogicalVolume( block3Solid, 
		luxMaterials->Water(), "block3_logical" );
	block3Logical->SetVisAttributes( luxMaterials->SteelVis() );
	new LUXSimDetectorComponent(0,
		G4ThreeVector(0.*cm,0.*cm,(-height+pyramidBlockThickness)/2+2*pyramidBlockThickness), 
		block3Logical, "Pyramid Block 3", logicalVolume, 
		0, 0, false);
	
	// Create block 4 (as a disk)
	G4Tubs *block4Solid = new G4Tubs( "block4_solid", 
		0.*cm, pyramidBlock4Radius, pyramidBlockThickness/2, 
		0.*deg, 360.*deg );
	G4LogicalVolume *block4Logical = new G4LogicalVolume( block4Solid, 
		luxMaterials->Water(), "block4_logical" );
	block4Logical->SetVisAttributes( luxMaterials->SteelVis() );
	new LUXSimDetectorComponent(0,
		G4ThreeVector(0.*cm,0.*cm,(height-pyramidBlockThickness)/2-2*pyramidBlockThickness), 
		block4Logical, "Pyramid Block 4", logicalVolume, 
		0, 0, false);
	
	// Create block 5 (as a disk)
	G4Tubs *block5Solid = new G4Tubs( "block5_solid", 
		0.*cm, pyramidBlock5Radius, pyramidBlockThickness/2, 
		0.*deg, 360.*deg );
	G4LogicalVolume *block5Logical = new G4LogicalVolume( block5Solid, 
		luxMaterials->Water(), "block5_logical" );
	block5Logical->SetVisAttributes( luxMaterials->SteelVis() );
	new LUXSimDetectorComponent(0,
		G4ThreeVector(0.*cm,0.*cm,(height-pyramidBlockThickness)/2-pyramidBlockThickness), 
		block5Logical, "Pyramid Block 5", logicalVolume, 
		0, 0, false);
	
	// Create block 6 (as a disk)
	G4Tubs *block6Solid = new G4Tubs( "block6_solid", 
		0.*cm, pyramidBlock6Radius, pyramidBlockThickness/2, 
		0.*deg, 360.*deg );
	G4LogicalVolume *block6Logical = new G4LogicalVolume( block6Solid, 
		luxMaterials->Water(), "block6_logical" );
	block6Logical->SetVisAttributes( luxMaterials->SteelVis() );
	new LUXSimDetectorComponent(0,
		G4ThreeVector(0.*cm,0.*cm,(height-pyramidBlockThickness)/2), 
		block6Logical, "Pyramid Block 6", logicalVolume, 
		0, 0, false);
			
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimMuonVetoSystem
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPyramid::~LUXSimPyramid()
{}

