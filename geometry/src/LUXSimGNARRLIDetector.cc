////////////////////////////////////////////////////////////////////////////////
/*	LUXSimGNARRLIDetector.cc
 *
 * This is the code file to define the geometry of the GNARRLI Detector 
 * operating in either 1 or 4 PMT mode in an experimental hall.
 *
 ********************************************************************************
 * Change log
 *	24 Aug 2011 - Initial submission (Tenzing)
 *  25 Aug 2011 - Added an "activeLiquid" volume inside of the field cage
 *				  (Tenzing)
 *	25 Aug 2011 - Added a piece of Li to act as the neutron source (Mike)
 *	08 Sep 2011 - Changed the number of internal support steel rods from 4 to 3
 *				  (Tenzing)
 *  13 Sep 2011 - Rotated the internal steel support rods by 30 degrees 
 *				  (Tenzing)
 *  23 Sep 2011 - Adding a borated poly collimator for neutron collimation
 *				  (Tenzing)
 *
 */
////////////////////////////////////////////////////////////////////////////////

//
//	C/C++ includes
//
#include <sstream>
#include "math.h"

//
//	GEANT4 includes
//
#include "globals.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Torus.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4String.hh"

#include "G4ios.hh"

//
//	LUXSim includes
//
#include "LUXSimGNARRLIDetector.hh"

#include "LUXSimMaterials.hh"
#include "LUXSimDetectorComponent.hh"


using namespace std;

#define pi 3.14159265

///	------++++++------++++++------++++++------++++++------
///					LUXSimGNARRLIDetector()
///	------++++++------++++++------++++++------++++++------ 
LUXSimGNARRLIDetector::LUXSimGNARRLIDetector() {

	
	// Find out whether to draw one or four PMTs
	G4bool onePMTflag = luxManager->GetNumGNARRLIPMTs();
	
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
	

//--------- Sizes of the principal geometrical components (solids)	---------
	
	
	fWorldLength= 5.0*m;	// world dimension
	
	liquidHeight = 98.*cm;	// height of liquid above floor
	gainGap = 2.0*cm;		
	// distance from bottom of field shaper to top of upper field cage ring

	
	// Geometry of the cryogenic dewar and flange
	G4double tDewar = 3.*mm;		// thickness of dewar wall (inner or outer)
	G4double tDewarVacuum = 4.*mm;
	G4double irDewarInner = 11.33/2.*cm;
	G4double orDewarInner = 11.4316/2.*cm;
	G4double irDewarOuter = 12.7784/2.*cm;
	G4double orDewarOuter = 12.88/2.*cm;
	G4double hDewarOuter = 45.72*cm;		
	// height of the dewar's exterior
	G4double tDewarTotal = orDewarOuter - irDewarInner;	
	// total thickness from outer wall to inner wall
	G4double hDewarInner = hDewarOuter - tDewarTotal;
	// height of the inside of the dewar
	G4double orDewarFlange = orDewarOuter + 3.*cm;
	G4double hDewarFlange = 3.*cm;
	G4double hCollarCrude = 25.*cm;
	G4double hCollarFlange = 4.*cm;
	
	G4double dDewarBottom = hDewarOuter/2. - tDewarTotal;
		// depth of the bottom of the dewar from the origin
	
	G4double orLargeFT = 4.0*cm;
	G4double irLargeFT = 3.0*cm;
	G4double lLargeFT = 8.*cm;
	G4double orLargeFTFlange = 5.0*cm;
	G4double lLargeFTFlange = 2.*cm;
	
	G4double orSmallFT = 3.0*cm;
	G4double irSmallFT = 2.0*cm;
	G4double lSmallFT = 6.*cm;
	G4double orSmallFTFlange = 4.0*cm;
	G4double lSmallFTFlange = 1.*cm;
	
	G4double orTFT = 2.5*cm;
	G4double irTFT = 1.5*cm;
	G4double lTFTbase = 8.*cm;
	G4double lTFTtee = 14.*cm;
	//	G4double orTFTFlange = 3.5*cm;
	//	G4double lTFTFlange = 1.*cm;
	

	// Field cage parameters
	G4double irFieldCage = 5.105/2.*cm;
	G4double orFieldCage = 6.16/2.*cm;
	G4double tFieldCage = 0.20574*cm;	
	// thickness of field rngs
	G4double numRings = 6;				
	G4double spacingRings = 1.22*cm;		
	// spacing of the field rings (center to center)
	G4double depthRing = 5.0*mm;		
	// depth of the top of the first ring below liquid surface
	
	
	
	// liquid and gas geometries
	G4double rLiquid = irDewarInner;          
	G4double hLiquid = 15.*cm;	
	// height of the liquid region (starts at bottom of dewar)
	//G4double rTarget = irFieldCage;
	G4double rTarget = 2.5*cm;
	
	//G4double hTarget = (numRings-1)*(spacingRings+tFieldCage);
	G4double hTarget = 7.*cm;
	
	G4double rGasDewar = rLiquid;				
	G4double hGasDewar = hDewarInner - hLiquid ;	
	// height of the gaseous region in the dewar
	G4double hGasCollar = hCollarCrude - hCollarFlange;
	
	
	// PEEK field cage holders (PCH)  (not pacific coast highway)
	G4double xPCH = 2.0*cm;
	G4double yPCH = 1.0*cm;
	G4double zPCH = depthRing + (numRings)*spacingRings;
	G4double cPCH = 5.*mm;
	
	
	// Steel rods that the detector hangs from inside the dewar
	G4double orSteelRod = 5.*mm;
	G4double irSteelRod = 3.*mm;
	G4double distSteelRod = irDewarInner - orSteelRod - 0.5*mm;
	
	// PEEK rods
	G4double rPeekRod = 5.*mm;
	G4double hPeekRod = 8.*cm;
	G4double distPeekRod = orFieldCage + rPeekRod + 1.*mm;
	
	//Cold Finger
	G4double rCF = 2.4*cm;
	G4double hCF = 9.*cm;
	
	// Field Shaper size and location
	// Made of a torus, cored by a cylinder
	// of rMajor with top taken off by a box
	// Placed in the gaseous volume just above the liquid surface
	G4double rMinorInner = 0.81*cm;
	G4double rMinorOuter = 1.22*cm;
	G4double rMajor = 3.08*cm; 
	G4double hFieldShaper = -hGasDewar/2. + rMinorOuter + gainGap - depthRing;

	// Cart geometry
	// two planes of steel, a hole in the top one that the dewar floats inside
	// four vertical rods that hold the dewar
	G4double xCart = 70.*cm;
	G4double yCart = 120.*cm;
	G4double zCart = 3.*mm;		
	// height of the top sheet of the cart from the floor
	G4double hCart = 90.*cm;		
	// height of top sheet of cart from the bottom sheet
	G4double gapCart = 67.*cm;
	G4double rCartHole = orDewarOuter * 1.1;
	G4double xCartHole = xCart/2. - 15.*cm;
	G4double yCartHole = yCart/2. - 15.*cm;
	G4double rSupportRod = 0.5*cm;
	G4double hSupportRod = 30*cm;
	G4double xSupportRod = rCartHole + 1.0*cm;
	G4double ySupportRod = xSupportRod;
	G4double xLegCart = 2.0*cm;
	G4double yLegCart = 1.0*cm;
	
	G4double rotdeg = 56;	// rotation angle of the 4 peek rods supporting
							// the field cage, pmts, field shaper etc...

	// PMT Dimensions 8520s
	G4double xPMT = 25.7 * mm;
	G4double zPMT = 28.25 * mm;
	G4double tPMT = 1.2 * mm;
	
	
	// PMT Dimensions 8778s
	G4double r8778 = 2.85*cm;
	G4double h8778 = 11.15*cm;
	G4double t8778 = 1.2 * mm;
	
	
	// PMT Holder dimensions 8778s
	G4double h8778holder = 6.*cm;
	G4double r8778holder = rMajor + rMinorInner*0.999;
	G4double t8778holder = 4.*mm;
	
	
	
	// Dimension of TPB glass
	G4double xTPBglass = 4.08*cm;
	G4double zTPBglass = 1.*mm;
	
	//Dimension of collimator
//	G4double rCollimator = 15.*cm;
//	G4double dCollimator = 15.*cm;
	
	// rotation matricies 
	G4RotationMatrix *rotX90 = new G4RotationMatrix();
	rotX90->rotateX(90.*deg);	
	
	G4RotationMatrix *rotX270 = new G4RotationMatrix();
	rotX270->rotateX(270.*deg);	
	
	G4RotationMatrix *rotY90 = new G4RotationMatrix;
	rotY90->rotateY(90.*deg);	
	
	G4RotationMatrix *rotY270 = new G4RotationMatrix;
	rotY270->rotateY(270.*deg);		
	
	
			
//--------- Definitions of Solids, Logical Volumes, Physical Volumes ---------

	//------------------------------ 
	// World
	//------------------------------ 

	G4double HalfWorldLength = 0.5*fWorldLength;
 
	G4Box* solidWorld= new 
		G4Box("world",HalfWorldLength,HalfWorldLength,HalfWorldLength);
	logicalVolume= new 
		G4LogicalVolume( solidWorld, luxMaterials->Air(), "World", 0, 0, 0);
	
	
	//------------------------------ 
	// Floor
	//------------------------------
	
	G4double floorDepth = HalfWorldLength - liquidHeight;
	
	G4Box* solidFloor = new 
		G4Box("solidFloor", HalfWorldLength, HalfWorldLength, floorDepth/2.);
	G4LogicalVolume* logicFloor = new 
		G4LogicalVolume( solidFloor, luxMaterials->Rock(), "logicFloor" );
	
	logicFloor->SetVisAttributes( luxMaterials->RockVis() );
	
	G4ThreeVector floorloc = 
		G4ThreeVector(0,0,-HalfWorldLength + floorDepth/2.);
	LUXSimDetectorComponent* physiFloor;
	physiFloor = new LUXSimDetectorComponent
		(0, floorloc, logicFloor, "physiFloor", logicalVolume, false, 0);
	
	
	
	//------------------------------ 
	// Vacuum Dewar
	//------------------------------ 
	// the dewar is composed of 5 solids
	// d1 is the aluminum wall of the dewar
	// d2 is the aluminum bottom of the dewar
	// d5 is the al flange outside of the dewar radius at the top of the dewar
	// d3 is the vacuum portion of the dewar in the wall
	// d4 is the vacuum portion of the dewar in the bottom

	
	// ---- Defining the aluminum portion of the dewar ----
	// wall of the dewar
	G4Tubs *d1 = new G4Tubs
		( "d1", 0, orDewarOuter, hDewarOuter/2., 0*deg, 360*deg );
	
	// round flange portion of the dewar
	G4Tubs *d5 = new G4Tubs
		( "d5", orDewarOuter, orDewarFlange, hDewarFlange/4., 0*deg, 360*deg );
	
	// square flange portion of the dewar
	G4Box *d6 = new G4Box
		( "d6", 1.1*orDewarFlange, 1.1*orDewarFlange, hDewarFlange/4. );
	
	// union of the flange to the walls
	G4UnionSolid *d15 = new G4UnionSolid
		( "d15", d1, d5, 0, G4ThreeVector
		(0,0,hDewarOuter/2.-3.*hDewarFlange/4.) );
	
	G4UnionSolid *solidAlDewar = new G4UnionSolid
		( "solidAlDewar", d15, d6, 0, G4ThreeVector
		(0,0,hDewarOuter/2.-hDewarFlange/4. ));	
	
	// ---- Defining the vacuum portion of the dewar ----
	// wall portion of the vacuum region of the dewar
	G4double hVacuum = hDewarOuter-2.*tDewar;
	
	G4Tubs *d3 = new G4Tubs
		( "d3", orDewarInner, irDewarOuter, hVacuum/2., 0*deg, 360*deg );
	
	G4Tubs *d4 = new G4Tubs
		( "d4", 0, irDewarOuter, tDewarVacuum/2., 0*deg, 360*deg );
	
	
	G4UnionSolid *solidVacuumDewar = new G4UnionSolid
		( "solidVacuumDewar", d3, d4, 0, G4ThreeVector
		(0,0, 0.5*(-hVacuum+tDewarVacuum) ) );
	
	
	// ---- Defining the logical volumes of the dewar (steel and vacuum) ----
	
	G4LogicalVolume* logicDewar = new G4LogicalVolume
		( solidAlDewar, luxMaterials->SS316(), "logicDewar");
	logicDewar->SetVisAttributes( luxMaterials->SteelVis() );
	
	
	G4LogicalVolume* logicVacuumDewar = new G4LogicalVolume
		( solidVacuumDewar, luxMaterials->Vacuum(), "logicVacuumDewar");
	logicVacuumDewar->SetVisAttributes( luxMaterials->VacuumVis() );

	LUXSimDetectorComponent* physiVacuumDewar;
	physiVacuumDewar = new LUXSimDetectorComponent
		( 0, G4ThreeVector(), logicVacuumDewar, "physiVacuumDewar", logicDewar, 
		false, 0 );
	
	G4double dewarloc = dDewarBottom - hLiquid;
	
	LUXSimDetectorComponent* physiDewar;
	physiDewar = new LUXSimDetectorComponent
		( 0, G4ThreeVector(0, 0, dewarloc), logicDewar, "physiDewar", 
		logicalVolume, false, 0);
	
	

	//------------------------------ 
	// Collar
	//------------------------------ 
	
	//
	// Crude collar place holder

	
	G4Tubs *cc1 = new G4Tubs
		( "cc1", 0, orDewarOuter, hCollarCrude/2., 0*deg, 360*deg );
	
	G4Tubs *cc2 = new G4Tubs
		( "cc2", 0, orDewarFlange, hCollarFlange/2., 0*deg, 360*deg );
	
	G4UnionSolid *cc12 = new G4UnionSolid
		( "cc12", cc1, cc2, 0, G4ThreeVector
		 ( 0, 0, -hCollarCrude/2. + hCollarFlange/2. ) );
	
	G4UnionSolid *solidCollarCrude = new G4UnionSolid
		( "solidCollarCrude", cc12, cc2, 0, G4ThreeVector
		 ( 0, 0, hCollarCrude/2. - hCollarFlange/2. ) );
	
	G4LogicalVolume* logicCollar = new G4LogicalVolume
		( solidCollarCrude, luxMaterials->SS316(), "logicCollar" );
		logicCollar->SetVisAttributes( luxMaterials->SteelVis() );

	LUXSimDetectorComponent* physiCollar;
	physiCollar = new LUXSimDetectorComponent
		( 0, G4ThreeVector(0, 0, hDewarOuter/2. + dewarloc +hCollarCrude/2. ),
		 logicCollar, "physiCollar", logicalVolume, false, 0);
	

	
	//------------------------------ 
	// FeedThrough
	//------------------------------ 
	
	//						//
	// Large FT				//
	//						//
	
	G4Tubs *lgft1 = new G4Tubs
		( "lgft1", 0, orLargeFT, lLargeFT/2. + orDewarOuter/2., 0*deg,360*deg );
	G4Tubs *lgftf = new G4Tubs
		( "lgftf",orLargeFT, orLargeFTFlange, lLargeFTFlange/2.,0*deg,360*deg );

	G4UnionSolid *lgft2 = new G4UnionSolid
		( "lgft2", lgft1, lgftf, 0, G4ThreeVector
		 (0, 0, lLargeFT/2. + orDewarOuter/2. - lLargeFTFlange/2.) );

	G4SubtractionSolid *solidLargeFT = new G4SubtractionSolid
		( "solidLargeFT", lgft2, solidCollarCrude, rotX90, G4ThreeVector
		 (0, 0, -lLargeFT/2. - orDewarOuter/2.) );
	
	
	G4LogicalVolume* logicLargeFT = new G4LogicalVolume( solidLargeFT, luxMaterials->SS316(), 
														"logicLargeFT" );
	logicLargeFT->SetVisAttributes( luxMaterials->SteelVis() );

	
	G4Tubs *lgftg1 = new G4Tubs
		( "lgftg1", 0, irLargeFT, lLargeFT/2. + orDewarOuter/2., 0*deg,360*deg);
	
	G4SubtractionSolid *solidLargeFTgas = new G4SubtractionSolid
		( "solidLargeFTgas", lgftg1, solidCollarCrude, rotX90, G4ThreeVector
		(0, 0, -lLargeFT/2. - orDewarOuter/2.) );
	
	G4LogicalVolume* logicLargeFTgas = new G4LogicalVolume
		( solidLargeFTgas, luxMaterials->GasAr(), "logicLargeFTgas" );
	logicLargeFTgas->SetVisAttributes( luxMaterials->GasArVis() );

	LUXSimDetectorComponent* physiLargeFTgas;
	physiLargeFTgas = new LUXSimDetectorComponent
		( 0, G4ThreeVector(), logicLargeFTgas, "physiLargeFTgas", 
		 logicLargeFT, false, 0);
	
	LUXSimDetectorComponent* physiLargeFT;
	physiLargeFT = new LUXSimDetectorComponent
	( rotX90, G4ThreeVector(0, lLargeFT/2. + orDewarOuter/2., 
	 hDewarOuter/2. + dewarloc + hCollarCrude/2. ), 
	  logicLargeFT, "physiLargeFT", logicalVolume, false, 0);

	
	//						//
	// Small FT				//
	//						//
	
	G4Tubs *smft1 = new G4Tubs
		( "smft1", 0, orSmallFT, lSmallFT/2. + orDewarOuter/2., 0*deg,360*deg );
	G4Tubs *smftf = new G4Tubs
		( "smftf", orSmallFT, orSmallFTFlange, lSmallFTFlange/2., 
		 0*deg, 360*deg );
	
	G4UnionSolid *smft2 = new G4UnionSolid
		( "smft2", smft1, smftf, 0, G4ThreeVector
		 (0, 0, lSmallFT/2. + orDewarOuter/2. - lSmallFTFlange/2.) );
	
	G4SubtractionSolid *solidSmallFT = new G4SubtractionSolid
	( "solidSmallFT", smft2, solidCollarCrude, rotX270, G4ThreeVector
	 (0, 0, -lSmallFT/2. - orDewarOuter/2.) );
	
	
	G4LogicalVolume* logicSmallFT = new G4LogicalVolume
		( solidSmallFT, luxMaterials->SS316(), "logicSmallFT" );
	logicSmallFT->SetVisAttributes( luxMaterials->SteelVis() );

	
	G4Tubs *smftg1 = new G4Tubs
		( "smftg1", 0, irSmallFT, lSmallFT/2. + orDewarOuter/2., 
		 0*deg, 360*deg );
	
	G4SubtractionSolid *solidSmallFTgas = new G4SubtractionSolid
	( "solidSmallFTgas", smftg1, solidCollarCrude, rotX270, G4ThreeVector
	 (0, 0, -lSmallFT/2. - orDewarOuter/2.) );
	
	G4LogicalVolume* logicSmallFTgas = new G4LogicalVolume
		( solidSmallFTgas, luxMaterials->GasAr(), "logicSmallFTgas" );
	logicSmallFTgas->SetVisAttributes( luxMaterials->GasArVis() );

	LUXSimDetectorComponent* physiSmallFTgas;
	physiSmallFTgas = new LUXSimDetectorComponent
		( 0, G4ThreeVector(), logicSmallFTgas, "physiLargeFTgas", 
		 logicSmallFT, false, 0);
	
	LUXSimDetectorComponent* physiSmallFT;
	physiSmallFT = new LUXSimDetectorComponent
	( rotX270, G4ThreeVector(0, -lSmallFT/2. - orDewarOuter/2., 
	 hDewarOuter/2. + dewarloc + hCollarCrude/2. ) , 
	  logicSmallFT, "physiSmallFT", logicalVolume, false, 0);
	
	
	//						//
	// Tee FTs				//  no flanges on the Tee'd part
	//						//
	
	G4Tubs *tft1 = new G4Tubs
		( "tft1", 0, orTFT, lTFTbase/2. + orDewarOuter/2., 0*deg, 360*deg );
	G4Tubs *tft2 = new G4Tubs
		( "tft2", 0, orTFT, lTFTtee/2., 0*deg, 360*deg );
	G4Tubs *tftg1 = new G4Tubs
		( "tftg1", 0, irTFT, lTFTbase/2. + orDewarOuter/2., 0*deg, 360*deg );
	G4Tubs *tftg2 = new G4Tubs
		( "tftg2", 0, irTFT, lTFTtee/2., 0*deg, 360*deg );
	
	G4SubtractionSolid *tft3 = new G4SubtractionSolid
		( "tft3", tft1, solidCollarCrude, rotY90, G4ThreeVector
		 (0, 0, -lTFTbase/2. - orDewarOuter/2.) );

	G4SubtractionSolid *tftg3 = new G4SubtractionSolid
		( "tftg3", tftg1, solidCollarCrude, rotY90, G4ThreeVector
		 (0, 0, -lTFTbase/2. - orDewarOuter/2.) );
	
	G4UnionSolid *solidTFT = new G4UnionSolid
		( "solidTFT", tft3, tft2, rotX90, G4ThreeVector
		 (0, 0, lTFTbase/2. + orDewarOuter/2.) );

	G4UnionSolid *solidTFTgas = new G4UnionSolid
		( "solidTFTgas", tftg3, tftg2, rotX90, G4ThreeVector
		(0, 0, lTFTbase/2. + orDewarOuter/2.) );
	
	G4LogicalVolume* logicTFT = new G4LogicalVolume
		( solidTFT, luxMaterials->SS316(), "logicTFT" );
	logicTFT->SetVisAttributes( luxMaterials->SteelVis() );

	G4LogicalVolume* logicTFTgas = new G4LogicalVolume
		( solidTFTgas, luxMaterials->GasAr(),  "logicTFTgas"); 
	logicTFTgas->SetVisAttributes( luxMaterials->GasArVis() );

	
	LUXSimDetectorComponent* physiTFTgas;
	physiTFTgas = new LUXSimDetectorComponent
		( 0, G4ThreeVector(), logicTFTgas, "physiTFTGas", logicTFT, false, 0); 
	
	LUXSimDetectorComponent* physiTFT1;
	physiTFT1 = new LUXSimDetectorComponent
		( rotY90, G4ThreeVector( -lTFTbase/2. - orDewarOuter/2., 0, 
		 hDewarOuter/2. + dewarloc + hCollarCrude/2. ), 
		 logicTFT, "physiTFT1", logicalVolume, false, 1);
	
	LUXSimDetectorComponent* physiTFT2;
	physiTFT2 = new LUXSimDetectorComponent
		( rotY270, G4ThreeVector( lTFTbase/2. + orDewarOuter/2., 0, 
		 hDewarOuter/2. + dewarloc + hCollarCrude/2. )
		  , logicTFT, "physiTFT2", logicalVolume, false, 2);	
	
	
	
	//------------------------------ 
	// Liquid region
	//------------------------------
	
	G4Tubs *solidSqLiquid = new G4Tubs
		( "solidSqLiquid", 0, rLiquid, hLiquid/2., 0*deg, 360*deg );

	G4Sphere *solidSph = new G4Sphere
		( "solidSph", rLiquid, 10*rLiquid, 0*deg, 360*deg, 90*deg, 90*deg );
	
	G4SubtractionSolid *solidLiquid = new G4SubtractionSolid
		( "solidLiquid", solidSqLiquid, solidSph, 0, G4ThreeVector
		 (0, 0, -(hLiquid/2. - rLiquid)) );
	
	
	G4LogicalVolume* logicLiquid = new G4LogicalVolume( solidLiquid, 
		luxMaterials->LiquidAr(), "logicLiquid");
	logicLiquid->SetVisAttributes( luxMaterials->LiquidArVis() );

	
	G4Tubs *solidActiveLiquid = new G4Tubs
	( "solidActiveLiquid", 0, rTarget, hTarget/2., 0*deg, 360*deg );
	
	G4LogicalVolume* logicActiveLiquid = new G4LogicalVolume
		( solidActiveLiquid, luxMaterials->LiquidAr(), "logicActiveLiquid");
	logicActiveLiquid->SetVisAttributes( luxMaterials->LiquidArVis() );
	
	LUXSimDetectorComponent* physiActiveLiquid;
	physiActiveLiquid = new LUXSimDetectorComponent
		( 0, G4ThreeVector( 0,0,(hLiquid-hTarget)/2.-depthRing ), 
		 logicActiveLiquid, "physiActiveLiquid", logicLiquid, false, 0 );
	
	LUXSimDetectorComponent* physiLiquid;
	physiLiquid = new LUXSimDetectorComponent
		( 0, G4ThreeVector(0,0,-dDewarBottom+hLiquid/2.), logicLiquid, 
		 "physiLiquid", logicDewar, false, 0 );
	
	//------------------------------ 
	// Teflon framed gaseous VolumeFiller or pure teflon
	//------------------------------
/*	G4Tubs *solidVolumeFiller = new G4Tubs
		( "solidVolumeFiller", orFieldCage*1.05, irDewarInner*0.97, 
		hLiquid/4., 160*deg, 40*deg );

	G4Tubs *solidVolumeFillerGas = new G4Tubs
		( "solidVolumeFillerGas", orFieldCage*1.05+2.*mm, irDewarInner*0.97-2.*mm, 
		 hLiquid/4.-1.5*mm, 165*deg, 30*deg );
	
	G4LogicalVolume* logicVolumeFiller = new G4LogicalVolume
		(solidVolumeFiller, luxMaterials->Teflon(), "logicVolumeFiller");
	logicVolumeFiller->SetVisAttributes( luxMaterials->CopperVis() );
	
	G4LogicalVolume* logicVolumeFillerGas = new G4LogicalVolume
	(solidVolumeFillerGas, luxMaterials->GasAr(), "logicVolumeFillerGas");
	logicVolumeFillerGas->SetVisAttributes( luxMaterials->SteelVis() );

	LUXSimDetectorComponent* physiVolumeFillerGas;
	physiVolumeFillerGas = new LUXSimDetectorComponent
	(0,G4ThreeVector(0,0,1.5*mm), logicVolumeFillerGas,
	 "physiVolumeFillerGas", logicVolumeFiller, false, 0 );
	
	LUXSimDetectorComponent* physiVolumeFiller;
	physiVolumeFiller = new LUXSimDetectorComponent
		(0,G4ThreeVector(0,0,hLiquid/2.-hLiquid/4.), logicVolumeFiller,
		 "physiVolumeFiller", logicLiquid, false, 0 );
*/	
	
	//------------------------------ 
	// Field Cage  
	//------------------------------
	
	G4Tubs *solidFieldRing = new G4Tubs
	( "solidFieldRing", irFieldCage, orFieldCage, tFieldCage/2.,0*deg, 360*deg);
	
	G4LogicalVolume* logicRing = new G4LogicalVolume( solidFieldRing, 
		luxMaterials->Copper(), "logicRing" );
	logicRing->SetVisAttributes( luxMaterials->CopperVis() );

	
	for ( G4int i = 0; i < numRings; i++) {
		G4ThreeVector rloc = G4ThreeVector
			(0, 0, hLiquid/2.-tFieldCage/2. - depthRing - i * spacingRings);
		LUXSimDetectorComponent* physiRing;
		stringstream name;
		stringstream number;
		number.width(1);
		number.fill('0');
		number << i;
		name.str("");
		name << "physiRing" << number.str();
		physiRing = new LUXSimDetectorComponent
			( 0, rloc, logicRing, name.str(), logicLiquid, false, i+1);
	}								  
	
	
	//------------------------------ 
	// PEEK Cage holders 
	//------------------------------

	G4Box *pch1 = new G4Box( "pch1", xPCH/2., yPCH/2. ,zPCH/2. );
	
	G4Tubs *solidFieldRingSubtraction = new G4Tubs
	( "solidFieldRing", irFieldCage*1.00001, orFieldCage*1.00001, 
	 tFieldCage*1.00001/2.,0*deg, 360*deg);
	
	G4SubtractionSolid *pch2 = 0;
	
	for (G4int i=0; i < numRings; i++ ) {
		G4ThreeVector ringloc = G4ThreeVector
			( orFieldCage + xPCH/2. - cPCH, 
			  0, 
			  zPCH/2. - tFieldCage/2. - depthRing - i * spacingRings );
		
		if (i == 0 ){
			G4SubtractionSolid *tmp1 = new G4SubtractionSolid
			( "tmp1", pch1, solidFieldRingSubtraction, 0, ringloc );
			pch2 = tmp1;
		}
		else {
			G4SubtractionSolid *tmp1 = new G4SubtractionSolid
			( "tmp1", pch2, solidFieldRingSubtraction, 0, ringloc );
			pch2 = tmp1;
		}
	}
	
	G4LogicalVolume* logicPCH = new G4LogicalVolume( pch2, luxMaterials->Peek(), 
													"logicPCH" );
	logicPCH->SetVisAttributes( luxMaterials->PeekVis() );

	for (G4int i=0; i<4; i++) {
		G4RotationMatrix *rotZ = new G4RotationMatrix();
		rotZ->rotateZ( (180 - rotdeg - i*90 )*pi/180.);

		G4double xx = (rotdeg+i*90)*pi/180.;
		stringstream name;
		stringstream number;
		number.width(1);
		number.fill('0');
		number << i;
		name.str("");
		name << "physiPCH" << number.str();
		LUXSimDetectorComponent* physiPCH;
		physiPCH = new LUXSimDetectorComponent
					( rotZ, 
					 G4ThreeVector( 
								   (orFieldCage + xPCH/2. - cPCH)*cos(xx), 
								   (orFieldCage + xPCH/2. - cPCH)*sin(xx), 
								   hLiquid/2. - zPCH/2. ), 
					logicPCH, name.str(), logicLiquid, false, i+1);
	}
	

	
	
	//------------------------------ 
	// Gaseous region
	//------------------------------
	
	G4Tubs *solidGasDewar = new G4Tubs
		( "solidGasDewar", 0, rGasDewar, hGasDewar/2., 0*deg, 360*deg );
	
	G4LogicalVolume* logicGasDewar = new G4LogicalVolume
		( solidGasDewar, luxMaterials->GasAr(), "logicGasDewar");
	logicGasDewar->SetVisAttributes( luxMaterials->GasArVis() );

	LUXSimDetectorComponent* physiGasDewar;
	physiGasDewar = new LUXSimDetectorComponent
		( 0, G4ThreeVector(0,0,-dDewarBottom+hLiquid+0.5*hGasDewar), 
		 logicGasDewar, "physiGasDewar", logicDewar, false, 0 );	
	
	
	G4Tubs *solidGasCollar = new G4Tubs
		( "solidGasCollar", 0, rGasDewar, hGasCollar/2., 0*deg, 360*deg );
	
	G4LogicalVolume* logicGasCollar = new G4LogicalVolume
		( solidGasCollar, luxMaterials->GasAr(),  "logicGasCollar");
	logicGasCollar->SetVisAttributes( luxMaterials->GasArVis() );

	LUXSimDetectorComponent* physiGasCollar;
	physiGasCollar = new LUXSimDetectorComponent
		( 0, G4ThreeVector(0,0,-hCollarFlange/2.), logicGasCollar, 
		 "physiGasCollar", logicCollar, false, 0 );	


	
	//------------------------------ 
	// Cold Finger
	//------------------------------
	
	G4Tubs *solidCF = new G4Tubs( "solidCF", 0, rCF, hCF/2., 0*deg, 360*deg );
	
	G4LogicalVolume* logicCF = new G4LogicalVolume
		( solidCF, luxMaterials->Copper(),"logicCF" );
	logicCF->SetVisAttributes ( luxMaterials->CopperVis() );

	LUXSimDetectorComponent* physiCF;
	physiCF = new LUXSimDetectorComponent
		( 0, G4ThreeVector(), logicCF, "physiCF",logicGasCollar, false, 0 );
	
	
	
	//------------------------------ 
	// Steel Rods 
	//------------------------------	
	
	G4Tubs *solidSteelRod = new G4Tubs
		("pr0", irSteelRod, orSteelRod, hGasDewar/2., 0*deg, 360*deg);
	
	G4LogicalVolume* logicSteelRod = new G4LogicalVolume
		( solidSteelRod, luxMaterials->SS316(), "logicSteelRod");
	logicSteelRod->SetVisAttributes ( luxMaterials->SteelVis() );

	for (G4int i=0; i<3; i++) {
		
		G4double xx = (rotdeg+i*120-30)*pi/180.;
		
		LUXSimDetectorComponent* physiSteelRod;
		physiSteelRod = new LUXSimDetectorComponent
			( 0, G4ThreeVector( distSteelRod*cos(xx), distSteelRod*sin(xx), 0 ), 
			 logicSteelRod, "physiSteelRod", logicGasDewar, false, i+1);
	}
	
	
	G4Tubs *solidLiquidSteelRod = new G4Tubs
		("pr0", irSteelRod, orSteelRod, hLiquid/2. - rLiquid/2., 0*deg,360*deg);
	
	G4LogicalVolume* logicLiquidSteelRod = new G4LogicalVolume
		( solidLiquidSteelRod, luxMaterials->SS316(), "logicSteelLiquidRod");
	logicLiquidSteelRod->SetVisAttributes ( luxMaterials->SteelVis() );

	for (G4int i=0; i<3; i++) {
		
		G4double xx = (rotdeg+i*120-30)*pi/180.;
		LUXSimDetectorComponent* physiLiquidSteelRod;
		physiLiquidSteelRod = new LUXSimDetectorComponent
		( 0, G4ThreeVector( 
						distSteelRod*cos(xx), 
						distSteelRod*sin(xx), 
						rLiquid/2. ), 
		  logicLiquidSteelRod, "physiSteelLiquidRod", logicLiquid, false, i+1);
	}

	
	//------------------------------ 
	// PEEK Rods 
	//------------------------------	
	
	G4Tubs *solidPeekRod = new G4Tubs
		("pr0", 0, rPeekRod, hPeekRod/2., 0*deg, 360*deg);
	
	G4LogicalVolume* logicPeekRod = new G4LogicalVolume
		( solidPeekRod, luxMaterials->Peek(), "logicPeekRod");
	logicPeekRod->SetVisAttributes ( luxMaterials->PeekVis() );

	for (G4int i=0; i<4; i++) {
		
		G4double xx = (rotdeg+i*90)*pi/180.;
		LUXSimDetectorComponent* physiPeekRod;
		physiPeekRod = new LUXSimDetectorComponent
			( 0, G4ThreeVector( distPeekRod*cos(xx), distPeekRod*sin(xx), 
			 -hGasDewar/2.+hPeekRod/2. ), logicPeekRod, "physiPeekRod", 
			  logicGasDewar, false, i+1);
	}
	
	
	
	//------------------------------ 
	// PMTs (8520)
	//------------------------------

	if ( onePMTflag == 1 ) {

		G4double tPMTactiveVacuum = 4.*mm;
		G4double tPMTwindow = 3.*mm;
		
		G4Box *solidPMT = new G4Box( "solidPMT", xPMT/2., xPMT/2., zPMT/2. );
		G4LogicalVolume* logicPMT = new G4LogicalVolume
			( solidPMT, luxMaterials->SS316(), "logicPMT" );
		logicPMT->SetVisAttributes ( luxMaterials->SteelVis() );

		G4Box *solidPMTVacuum = new G4Box
			( "solidPMTVacuum", xPMT/2. - tPMT, xPMT/2. - tPMT, (zPMT-tPMT)/2. );
		G4LogicalVolume *logicPMTVacuum = new G4LogicalVolume
			( solidPMTVacuum, luxMaterials->Vacuum(), "logicPMTVacuum" );
		logicPMTVacuum->SetVisAttributes ( luxMaterials->VacuumVis() );

		LUXSimDetectorComponent *physiPMTVacuum;
		physiPMTVacuum = new LUXSimDetectorComponent
			( 0, G4ThreeVector(0, 0, -tPMT/2.), 
			 logicPMTVacuum, "physiPMTVacuum", logicPMT, false, 0);
		
		
		G4Box *solidPMTactiveVacuum = new G4Box
			( "solidPMTactiveVacuum", xPMT/2. - tPMT, xPMT/2. - tPMT, 
			 tPMTactiveVacuum/2. );
		G4LogicalVolume *logicPMTactiveVacuum = new G4LogicalVolume
			( solidPMTactiveVacuum, luxMaterials->Vacuum(), "logicPMTactiveVacuum");
		logicPMTactiveVacuum->SetVisAttributes ( luxMaterials->VacuumVis() );

		LUXSimDetectorComponent *physiPMTactiveVacuum;
		physiPMTactiveVacuum = new LUXSimDetectorComponent
			( 0, G4ThreeVector(0,0,-(zPMT-tPMT)/2.+tPMTactiveVacuum/2.), 
			 logicPMTactiveVacuum, "physiPMTactiveVacuum", logicPMTVacuum,false, 0);
		

		G4Box *solidPMTwindow = new G4Box
			( "solidPMTwindow", xPMT/2. - tPMT, xPMT/2. - tPMT, tPMTwindow/2. );
		G4LogicalVolume *logicPMTwindow = new G4LogicalVolume
			( solidPMTwindow, luxMaterials->Quartz(), "logicPMTwindow");
		logicPMTwindow->SetVisAttributes ( luxMaterials->QuartzVis() );

		LUXSimDetectorComponent *physiPMTwindow;
		physiPMTwindow = new LUXSimDetectorComponent
			( 0, G4ThreeVector(0,0, (-tPMTactiveVacuum+tPMTwindow)/2.), 
			 logicPMTwindow, "physiPMTwindow", logicPMTactiveVacuum, false, 0);
	
	// Need to add the other 3 PMTs and their placement.
		LUXSimDetectorComponent* physiPMT;
		physiPMT = new LUXSimDetectorComponent
			( 0, G4ThreeVector(0,0,hFieldShaper + xPMT/2.), logicPMT, 
			 "physiPMT", logicGasDewar, false, 0);
	}
	
	
	
	//------------------------------ 
	// PMTs (8778)
	//------------------------------

	if (onePMTflag == 0){


		G4double t8778activeVacuum = 4.*mm;
		G4double t8778window = 3.*mm;
		
		G4Tubs *solid8778 = new G4Tubs
			( "solid8778", 0., r8778, h8778/2., 0*deg, 360*deg );
		G4LogicalVolume* logic8778 = new G4LogicalVolume
			( solid8778, luxMaterials->SS316(), "logic8778" );
		logic8778->SetVisAttributes ( luxMaterials->SteelVis() );

		G4Tubs *solid8778Vacuum = new G4Tubs
			( "solid8778Vacuum", 0, r8778-t8778, h8778/2. - t8778/2.,0*deg,360*deg);
		G4LogicalVolume *logic8778Vacuum = new G4LogicalVolume
			( solid8778Vacuum, luxMaterials->Vacuum(), "logic8778Vacuum" );
		logic8778Vacuum->SetVisAttributes ( luxMaterials->VacuumVis() );

		LUXSimDetectorComponent *physi8778Vacuum;
		physi8778Vacuum = new LUXSimDetectorComponent
			( 0, G4ThreeVector(0,0,-t8778/2.), logic8778Vacuum, "physi8778Vacuum", 
			 logic8778, false, 0 );
		
		G4Tubs *solid8778activeVacuum = new G4Tubs
			( "solid8778activeVacuum", 0, r8778-t8778, t8778activeVacuum/2., 
			 0*deg, 360*deg);
		G4LogicalVolume *logic8778activeVacuum = new G4LogicalVolume
			( solid8778activeVacuum, luxMaterials->Vacuum(), "logic8778activeVacuum" );
		logic8778activeVacuum->SetVisAttributes ( luxMaterials->VacuumVis() );

		LUXSimDetectorComponent *physi8778activeVacuum;
		physi8778activeVacuum = new LUXSimDetectorComponent
			( 0, G4ThreeVector(0,0,-(h8778-t8778)/2. + t8778activeVacuum/2.), 
			 logic8778activeVacuum, "physi8778activeVacuum", logic8778Vacuum, 
			  false, 0 );
		
		G4Tubs *solid8778window = new G4Tubs
			( "solid8778window", 0, r8778-t8778, t8778window/2., 0*deg, 360*deg);
		G4LogicalVolume *logic8778window = new G4LogicalVolume
			( solid8778window, luxMaterials->Quartz(), "logic8778window" );
		logic8778window->SetVisAttributes ( luxMaterials->QuartzVis() );

		LUXSimDetectorComponent *physi8778window;
		physi8778window = new LUXSimDetectorComponent
			( 0, G4ThreeVector(0,0,(-t8778activeVacuum+t8778window)/2.), 
			 logic8778window, "physi8778window", logic8778activeVacuum, false, 0 );
		
		LUXSimDetectorComponent* physi8778;
		physi8778 = new LUXSimDetectorComponent
			( 0, G4ThreeVector(0,0,hFieldShaper + h8778/2.), logic8778, 
			 "physi8778", logicGasDewar, false, 0);
	}
	
	
	//------------------------------ 
	// Field Shaping Ring
	//------------------------------
	
	// Check these dimensions
	
	G4Torus *s1 = new G4Torus( "s1", rMinorInner, rMinorOuter, rMajor, 0*deg,
		360*deg );
	G4Box *s2 = new G4Box( "s2", 11*cm, 11*cm, 4*cm );
	G4SubtractionSolid *s3 = new G4SubtractionSolid( "s3", s1, s2, 0, 
		G4ThreeVector( 0, 0, 4*cm ) );

	G4Tubs *s4 = new G4Tubs("s4", 0*cm, rMajor, 5*cm, 0*deg, 360*deg);
	G4SubtractionSolid *s5 = new G4SubtractionSolid
		( "s5", s3, s4, 0, G4ThreeVector() );
	
	G4SubtractionSolid *solidFieldShaper = 0;
	
	for (G4int i=0; i<4; i++) {
		G4double xx = (rotdeg+i*90)*pi/180.;
		
		if (i==0){
			G4SubtractionSolid *tmp1 = new G4SubtractionSolid
				("solidFieldShaper", s5, solidPeekRod, 0, 
				 G4ThreeVector(distPeekRod*cos(xx), 
							   distPeekRod*sin(xx), 
							   0 ) );
			
			solidFieldShaper = tmp1;
		}
		else {
			G4SubtractionSolid *tmp2 = new G4SubtractionSolid
				("solidFieldShaper", solidFieldShaper, solidPeekRod, 0, 
				 G4ThreeVector(distPeekRod*cos(xx), distPeekRod*sin(xx), 0 ) );
			solidFieldShaper = tmp2;
		}
	}

	G4LogicalVolume* logicFieldShaper = new G4LogicalVolume
		( solidFieldShaper, luxMaterials->SS316(),"logicFieldShaper" );
	logicFieldShaper->SetVisAttributes ( luxMaterials->SteelVis() );

	LUXSimDetectorComponent* physiFieldShaper;
	physiFieldShaper = new LUXSimDetectorComponent( 0, G4ThreeVector
		(0,0,hFieldShaper ), logicFieldShaper, "physiFieldShaper", 
		 logicGasDewar, false, 0 );
	
	
	
	G4Tubs *s6 = new G4Tubs
		("s6", 0*cm, rMajor, (rMinorOuter-rMinorInner)/2., 0*deg, 360*deg );
	G4Box *s7 = new G4Box
		("s7", xTPBglass/2., xTPBglass/2., (rMinorOuter-rMinorInner) );
	
	G4RotationMatrix *rotZ = new G4RotationMatrix();
	rotZ->rotateZ(-(rotdeg+45)*deg);
	
	G4SubtractionSolid *s8 = new G4SubtractionSolid
		("s8", s6, s7, rotZ, G4ThreeVector() );
	
	G4LogicalVolume* logicFieldShaper2 = new G4LogicalVolume
		( s8, luxMaterials->SS316(), "logicFieldShaper2" );
	logicFieldShaper2->SetVisAttributes ( luxMaterials->SteelVis() );

	LUXSimDetectorComponent* physiFieldShaper2;
	physiFieldShaper2 = new LUXSimDetectorComponent
		( 0, G4ThreeVector(0,0,hFieldShaper-(rMinorInner+rMinorOuter)/2.), 
		 logicFieldShaper2, "physiFieldShaper2", logicGasDewar, false, 0 );
	
	
	
	
	
	//------------------------------ 
	// PMT holder (for 8520s)
	//------------------------------
	
	// use the defined onePMTflag variable 
	// to determine whether to draw this or not
	// maybe just make it out of a cylindrical 
	// shell minus the field shaping ring
	// also have to subrtract the peek rods
	
	if (onePMTflag == 1) {
		
		
		
	}
	
	
	
	//------------------------------ 
	// PMT holder (for 8778s)
	//------------------------------
	

	G4Tubs *holder8778 = new G4Tubs
		("holder8778", r8778, r8778holder, h8778holder/2., 0.*deg, 360.*deg );
	G4SubtractionSolid *solidh1 = new G4SubtractionSolid
		("solidh1", holder8778, s5, 0, 
		 G4ThreeVector(0,0,-h8778holder/2. + rMinorInner) );
	
	G4SubtractionSolid *solid8778holdertemp = 0;
	for (G4int i=0; i<4; i++) {
		G4double xx = (rotdeg+i*90)*pi/180.;
		
		if (i==0){
			G4SubtractionSolid *tmp3 = new G4SubtractionSolid
				("solid8778holdertemp", solidh1, solidPeekRod, 0, 
				 G4ThreeVector(distPeekRod*cos(xx), distPeekRod*sin(xx), 0 ) );
			solid8778holdertemp = tmp3;
		}
		else {
			G4SubtractionSolid *tmp4 = new G4SubtractionSolid
				("solid8778holdertemp", solid8778holdertemp, solidPeekRod, 0, 
				 G4ThreeVector(distPeekRod*cos(xx), distPeekRod*sin(xx), 0 ) );
			solid8778holdertemp = tmp4;
		}
	}
	
	G4Tubs *solidh2 = new G4Tubs
		("solidh2", r8778+t8778holder, r8778holder*1.5, h8778holder/2., 
		 0.*deg, 360.*deg );
		
	G4SubtractionSolid *solid8778holder = new G4SubtractionSolid
		("solid8778holder", solid8778holdertemp, solidh2, 0, 
		 G4ThreeVector(0, 0, rMinorInner) );
	
	G4LogicalVolume *logic8778holder = new G4LogicalVolume
		( solid8778holder, luxMaterials->SS316(), "logic8778holder" );	
	logic8778holder->SetVisAttributes ( luxMaterials->SteelVis() );

	
	// use the defined onePMTflag variable 
	// to determine whether to draw this or not

	if (onePMTflag == 0) {

		LUXSimDetectorComponent *physi8778holder;
		physi8778holder = new LUXSimDetectorComponent
			( 0, G4ThreeVector( 0,0,hFieldShaper + h8778holder/2.-rMinorInner ), 
			 logic8778holder, "physi8778holder", logicGasDewar, false, 0 );
	}
	
	
	
	//------------------------------ 
	// TPB Coated Window
	//------------------------------
	
	G4Box *tpbg1 = new G4Box
		("tpbg1", xTPBglass/2., xTPBglass/2., zTPBglass/2. );
	
	G4LogicalVolume* logicTPBglass = new G4LogicalVolume
		( tpbg1, luxMaterials->Quartz(), "logicTPBglass" );
	logicTPBglass->SetVisAttributes ( luxMaterials->QuartzVis() );

	LUXSimDetectorComponent* physiTPBglass;
	physiTPBglass = new LUXSimDetectorComponent
		( rotZ, G4ThreeVector(0,0,hFieldShaper-rMinorInner-zTPBglass/2.), 
		 logicTPBglass, "physiTPBglass", logicGasDewar, false, 0 );
	
	
	
	
	//------------------------------ 
	// Cart
	//------------------------------
	
	G4Box *c1 = new G4Box( "c1", xCart/2., yCart/2., zCart/2. );
	G4Tubs *c2 = new G4Tubs( "c2", 0.*cm, rCartHole, zCart, 0*deg, 360*deg );
	
	G4SubtractionSolid *c12 = new G4SubtractionSolid( "c12", c1, c2,
		0, G4ThreeVector(-xCartHole, -yCartHole, 0));
	
	G4Tubs *solidSupportRod = new G4Tubs
		("solidSupportRod", 0, rSupportRod, hSupportRod/2., 0*deg, 360*deg);
	
	G4UnionSolid *c12r1 = new G4UnionSolid( "c12r1", c12, solidSupportRod, 0, 
			G4ThreeVector(-xCartHole+xSupportRod, -yCartHole+ySupportRod, 
			 hSupportRod/2.+zCart/2.));
	
	G4UnionSolid *c12r2 = new G4UnionSolid( "c12r2", c12r1, solidSupportRod, 0, 
			G4ThreeVector(-xCartHole+xSupportRod, -yCartHole-ySupportRod, 
			 hSupportRod/2.+zCart/2.));
	
	G4UnionSolid *c12r3 = new G4UnionSolid( "c12r3", c12r2, solidSupportRod, 0, 
			G4ThreeVector(-xCartHole-xSupportRod, -yCartHole+ySupportRod, 
			 hSupportRod/2.+zCart/2.));
	
	G4UnionSolid *c12r4 = new G4UnionSolid( "c12r4", c12r3, solidSupportRod, 0, 
			G4ThreeVector(-xCartHole-xSupportRod, -yCartHole-ySupportRod, 
			 hSupportRod/2.+zCart/2.));
	
	
	G4Box *c3 = new G4Box("c3", xLegCart/2., yLegCart/2., gapCart/2 - zCart/2. );
	
	G4UnionSolid *c12r4l1 = new G4UnionSolid( "c12r4l1", c12r4, c3, 0, 
			G4ThreeVector((xCart/2.-xLegCart/2.), (yCart/2.-yLegCart/2.), 
						  -gapCart/2.));
	
	G4UnionSolid *c12r4l2 = new G4UnionSolid( "c12r4l2", c12r4l1, c3, 0, 
			G4ThreeVector(-(xCart/2.-xLegCart/2.), (yCart/2.-yLegCart/2.), 
						  -gapCart/2.));
	
	G4UnionSolid *c12r4l3 = new G4UnionSolid( "c12r4l3", c12r4l2, c3, 0, 
			G4ThreeVector((xCart/2.-xLegCart/2.), -(yCart/2.-yLegCart/2.), 
						  -gapCart/2.));
	
	G4UnionSolid *c12r4l4 = new G4UnionSolid( "c12r4l4", c12r4l3, c3, 0, 
			G4ThreeVector(-(xCart/2.-xLegCart/2.), -(yCart/2.-yLegCart/2.), 
						  -gapCart/2.));
	
	
	G4UnionSolid *solidCart = new G4UnionSolid
		("solidCart", c12r4l4, c1, 0, G4ThreeVector(0,0,-gapCart));
	
	G4LogicalVolume* logicCart = new G4LogicalVolume
		( solidCart, luxMaterials->SS316(), "logicCart");
	logicCart->SetVisAttributes ( luxMaterials->SteelVis() );

	G4ThreeVector cartloc = G4ThreeVector
		(xCartHole, yCartHole, -(HalfWorldLength - floorDepth) + hCart);
	
	LUXSimDetectorComponent* physiCart;
	physiCart = new LUXSimDetectorComponent
		( 0, cartloc, logicCart,"physiCart", logicalVolume, false, 0);
	
	
	
	//------------------------------ 
	// NaI detectors
	//------------------------------
	// crystal, PMT, else?
	
	
	//------------------------------ 
	// Lithium neutron source
	//------------------------------
/*	
	G4Box *pLig1 = new G4Box
	("pLig1", .1*mm/2. , .1*mm/2. , .1*mm/2.);
	
	G4LogicalVolume* logicpLiglass = new G4LogicalVolume
	( pLig1, luxMaterials->Air(), "logicpLiglass" );
	logicpLiglass->SetVisAttributes ( luxMaterials->QuartzVis() );
	
	LUXSimDetectorComponent* physipLiglass;
	physipLiglass = new LUXSimDetectorComponent
			( 0, G4ThreeVector(-50*cm,0.*cm,-hTarget/2. - depthRing ), 
			logicpLiglass, "LithiumTarget", logicalVolume, false, 0 );
*/	
	
	//------------------------------ 
	// Collimator
	//------------------------------
/*	
	G4Tubs *solidCollimator1 = new G4Tubs
		( "solidCollimator1", 0., rCollimator, dCollimator/2., 
		 0.*deg, 360.*deg );
	G4Cons *solidCollimator2 = new G4Cons
		( "solidCollimator2", 0., 0.00001*cm, 0., 2.*cm, 25.*cm, 0.*deg, 360.*deg );
	
	G4SubtractionSolid *solidCollimator = new G4SubtractionSolid
		( "solidCollimator", solidCollimator1, solidCollimator2, 0, 
		 G4ThreeVector(0.,0., -2.5*cm) );
	
	G4LogicalVolume *logicCollimator = new G4LogicalVolume
		( solidCollimator, luxMaterials->PolyethyleneBorated(), 
		 "logicCollimator" );
	logicCollimator->SetVisAttributes( luxMaterials->PolyethyleneBoratedVis() );

	
	LUXSimDetectorComponent* physiCollimator;
	physiCollimator = new LUXSimDetectorComponent
		( rotY270, G4ThreeVector(-27.5*cm, 0, -3.5*cm - depthRing), 
		 logicCollimator, "physiCollimator", logicalVolume, false, 0 );
*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 


///	------++++++------++++++------++++++------++++++------
///					~LUXSimGNARRLIDetector()
///	------++++++------++++++------++++++------++++++------ 
LUXSimGNARRLIDetector::~LUXSimGNARRLIDetector()
{}
