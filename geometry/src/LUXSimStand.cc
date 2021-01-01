//	LUXSimStand.cc

//	This is the code to define the cryostat stand.  
//	M.Woods  (mwoods@student.physics.ucdavis.edu)

//	09.28.09 - Initial submission (Melinda)
//	10.01.09 - Code cleanup (Melinda)
//  01.07.10 - Added pieces (Michael)
//   3.08.10 - Removed cout statements and fixed detX[YZ] usage.
//   4.04.10 - Fixed position of support rods and position of
//              stand relative to cryostat. Individual rods are now
//              LUXSimDetectorComponents.
//  04.07.10 - Adding source tubes. Geometry reference: 
//              http://teacher.pas.rochester.edu:8080/wiki/pub/Lux/
//						LuxCalibrations/
//              170212_LUX_calibration_DM.pdf . (Dave)
//   4.08.10 - Holes in leveling ring to make room for support rods
//              are now properly subtracted. No more overlaps.
//	2010-04-16 - Added Set* functions for collimators (Dave)
//	2010-07-12 - Fixed source tube overlap bug
//				 Removed top source tube portion to fix water tank overlap
//				 warning
//  2010-09-29 - Minor changes to collimator hole depth / centering. (Dave)
//	2012-01-12 - Changed the subtraction solid offsets and sizes for the
//				 collimator hole so that the shapes involved have a 1-mm overlap
//				 (Kareem)
//	2012-01-16 - Modified the collimators to more accurately reflect reality.
//				 The collimators are now tungsten blocks inside steel housings,
//				 and there are source volumes contained within the collimators
//				 onto which radioactive loads can be placed (Kareem)
//	2012-02-06 - Made the collimator sources have separate logical as well as
//				 physical volumes to make the volume sampling for the event
//				 generation correct (Kareem)
//	2012-04-15 - Changed z-offset of collimator height to match new z of r412+
//				 (Matthew)
//	2012-04-17 - Corrected tube_ext_z and collimator_z and tested with source
//				 tubes on: eliminated over-correction from last change (Matthew)
//	2012-07-18 - Changed the source collimator geometry to be 0.2" deeper
//				 (parallel to the axis of the collimator hole), and to have a
//				 back plate of lead instead of being tungsten all the way
//				 through (Kareem)
//	2012-07-19 - Quick bug fix to get rid of small overlap error in the
//				 collimator geometry (Kareem)
//	2012-08-15 - Fixed the source geometry--now an approximate point source 
//				 inside the source buttons instead of a thin foil with 3/4"
//				 diameter (Kareem)
//	2013-03-11 - Modified the source tube collimator based on the underground
//				 design. This code checks to see if the surface flag is set or
//				 not, and sets the collimator appropriately. (Kareem)
//	2013-03-11 - Minor geometry tweaks to get the underground and surface
//				 geometries correct (Kareem)
//	2013-03-18 - Added the SetCollimatorSourceDiameter method, and set the
//				 distance between the vaccum vessel and the extender tubes to
//				 be 1.5 cm, regardless of whether the surface or underground
//				 geometry is called. (Kareem)
//	2013-03-28 - Renamed a couple collimator components to reduce confusion
//				 (Kareem)
//	2013-04-09 - The collimator source diameter is now properly initialized in
//				 the surface geometry setting (Kareem)
//  2013-07-08 - Source tube distance adjustment necessitated by geometry
//				 overhaul. (Matthew)
//  2013-07-18 - Added LUX neutron source tube (Kevin)
//  2013-07-30 - Fixed overlaps in neutron collimator geometry (Kevin)
//	2013-08-05 - Changed the volume name "neutronCollimatorInside" to
//				 "neutronInsideCollimator" to avoid degeneracy with volume names
//				 with respect to sourcing and record levels (Kareem)
//	2015-02-02 - Changed LUX DD neutron collimator to better reflect reality
//
// The documents used to build this file can be found at 
//
// http://teacher.pas.rochester.edu:8080/wiki/bin/view/LuxHomestake/
//				CryostatCryostatInstallation
//
// under the names:
//      lux_he_detectorstand-fabrication-contractdocs_09-08-28.pdf
//      lux_he_levelingringandliftingbracket-fabrication-contractdocs_
//				09-08-28.pdf
//      lux_he_towersegments-fabrication-contractdocs_09-08-28.pdf
//
// If these links break, contact Michael Woods for the documents he used.



/*  ========================================== */
/*  ||  SECTIONS                               */
/*  ||                                         */
/*  ||  1. Detector Stand                      */
/*  ||     a. Vertical Legs (3x)               */
/*  ||     b. Horizontal Triangle X-Beams (2x) */
/*  ||     c. Vertical Triangle Supports  (3x) */
/*  ||                                         */
/*  ||  2. Tower                               */
/*  ||     a. Vertical Legs (3x)               */
/*  ||     b. Horizontal Triangle X-Beams (3x) */
/*  ||                                         */
/*  ||  3. Leveling Ring and Support Rods      */
/*  ||     a. Leveling Ring                    */
/*  ||     b. Support Rods (18x)               */
/*  ||                                         */
/*  ||  4. Source Tubes                        */
/*  ||     a. All Tubes + Extenders            */
/*  ||     b. Collimator					   */
/*  ||                                         */
/*  ||                                         */
/*  ||                                         */
/*  ========================================== */

// 
//
//	C/C++ includes
//
#include <sstream>

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Para.hh"
#include "G4Trap.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4Sphere.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"

//
//	LUXSim includes
//
#include "LUXSimStand.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimStand
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimStand::LUXSimStand( LUXSimDetectorComponent *standMother, 
				G4double detX, G4double detY, G4double detZ ) {

	//	Get the LUXSimManager pointer
	luxManager = LUXSimManager::GetManager();
	underground = !(luxManager->GetLUXSurfaceGeometry());
	useNeutronCollimator = luxManager->GetLUXNeutronCollimator();
	neutronCollimatorHeight = luxManager->GetLUXNeutronCollimatorHeight();
	sourceTubeGap = luxManager->GetLUXSourceTubeGap();

	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

    // Introduce an offset to the whole stand.
    detZ = detZ + 20*cm;

	G4double triangleSide = 277.1775*cm;
    G4double buffer = 1*cm; // For hollowing beams

    //
    //  SECTION 1
    //  Detector Stand:
    //
	//	The base legs of the stand
    //  Three vertical legs of stand.

	G4double standColumnZ = 508.00*cm;
	G4double standColumnX = 15.24*cm;
	G4double standColumnY = standColumnX;
	G4double standColumnThickness = 0.9525*cm;

    // Build Vertical Leg
	G4Box *outerColumn_solid = new G4Box( "outerColumn_solid",
            standColumnX/2.,
			standColumnY/2.,
            standColumnZ/2.);
	G4Box *innerColumn_solid = new G4Box( "innerColumn_solid",
			standColumnX/2.-standColumnThickness,
            standColumnY/2.-standColumnThickness,
			standColumnZ/2.+buffer);

    // First Stand Leg
	G4SubtractionSolid *standColumn_solid = new G4SubtractionSolid(
			"standColumn_solid", outerColumn_solid, innerColumn_solid);

    // Second Stand Leg
	G4double xOff, yOff, zOff;
	xOff = triangleSide/2.;
	yOff = -triangleSide*(sqrt(3)/2.);
    zOff = 0;
    G4RotationMatrix* col_rotation1 = new G4RotationMatrix;
    G4RotationMatrix* col_rotation2 = new G4RotationMatrix;
    col_rotation1->rotateZ(30.*deg);
    col_rotation2->rotateZ(60.*deg);
	G4UnionSolid *standColumnTemp_solid = new G4UnionSolid(
			"standColumnTemp_solid", standColumn_solid, standColumn_solid,
            col_rotation1, G4ThreeVector(xOff,yOff,zOff));

    // Third Stand Leg
	xOff = -triangleSide/2.;
	yOff = -triangleSide*(sqrt(3)/2.);
	zOff = 0;
	G4UnionSolid *standColumns_solid = new G4UnionSolid("standColumns_solid",
            standColumnTemp_solid, standColumn_solid,
            col_rotation2, G4ThreeVector(xOff,yOff,zOff));

	G4LogicalVolume *standColumn_log = new G4LogicalVolume( standColumns_solid,
			luxMaterials->Steel(), "standColumn_log" );
	standColumn_log -> SetVisAttributes( luxMaterials->SteelVis() );
    xOff = 0;
    yOff = triangleSide*(sqrt(3)/3.);
    zOff = 0;
    standColumn = new LUXSimDetectorComponent(0,
            G4ThreeVector(xOff+detX,yOff+detY,zOff+detZ),
           "standColumn", standColumn_log, standMother, 0, 0, false);


	//	Horizontal beams between the three vertical legs
    //  forming two triangles. 
    //  The effective triangleSide for these beams is slightly
    //  larger than the distance between the vertical columns.
    //  It is changed here and changed back when needed.

    // First Beam
	G4double standBeamX = 253.84125*cm;
	G4double standBeamY = 10.16*cm;
	G4double standBeamZ = 10.16*cm;
	G4double standBeamThickness = .635*cm; 
	triangleSide = 277.1775*cm + 20.32*cm;

	G4Box *outerBeam_solid = new G4Box( "outerBeam_solid",standBeamX/2.,
			standBeamY/2., standBeamZ/2.);
	G4Box *innerBeam_solid = new G4Box( "innerBeam_solid",standBeamX/2.+buffer,
			standBeamY/2.-standBeamThickness,standBeamZ/2.-standBeamThickness);

	G4SubtractionSolid* standBeam_solid = new G4SubtractionSolid(
			"standBeam_solid", outerBeam_solid, innerBeam_solid);

    // Second Beam
	G4RotationMatrix* beam_rotation1 = new G4RotationMatrix;
	G4RotationMatrix* beam_rotation2 = new G4RotationMatrix;
	beam_rotation1->rotateZ(60.*deg);
	beam_rotation2->rotateZ(120.*deg);
	xOff = triangleSide/4;
	yOff = triangleSide*(sqrt(3)/4);
	zOff = 0;
	G4UnionSolid* standBeamTemp_solid = new G4UnionSolid("standBeamTemp_solid",
			standBeam_solid, standBeam_solid, beam_rotation1,
            G4ThreeVector(xOff,yOff,zOff));

    // Third Beam
	xOff = -triangleSide/4;
	yOff = triangleSide*(sqrt(3)/4);
	zOff = 0;
	G4UnionSolid* oneStandBeam_solid = new G4UnionSolid("oneStandBeam_solid",
            standBeamTemp_solid,standBeam_solid,beam_rotation2,
            G4ThreeVector(xOff,yOff,zOff));

    // Duplicate Horizontal Triangle
    G4UnionSolid* standBeams_solid = new G4UnionSolid("standBeams_solid",
            oneStandBeam_solid, oneStandBeam_solid,
            0, G4ThreeVector(0, 0, 85.24875*cm));

	G4LogicalVolume *standBeams_log = new G4LogicalVolume( standBeams_solid,
			luxMaterials->Steel(), "standBeams_log");
	standBeams_log -> SetVisAttributes( luxMaterials->SteelVis() );
	xOff = 0;
	yOff = -triangleSide*(sqrt(3)/6);
	zOff = standColumnZ/2. - 7.77875*cm - 5.08*cm - 85.24875*cm;
	standBeams = new LUXSimDetectorComponent(0,
            G4ThreeVector(xOff+detX,yOff+detY,zOff+detZ),
            "standBeams",standBeams_log, standMother, 0, 0, false);


	triangleSide = 277.1775*cm;

    // Vertical Triangles
    // and radial beams:
    // Radial beam and angled Kickers.

    G4double standRadialY = 97.63125*cm;
    G4double standRadialZ = 10.16*cm;
    G4double standRadialX = 10.16*cm;
    G4double standRadialOff = 11.43*cm; // Distance away from col center

    G4double standKickerY = 105.41*cm;
    G4double standKickerX = 10.16*cm;
    G4double standKickerZ = 10.16*cm;
    G4double standKickerThickness = 0.635*cm;
    G4double standKickerAng = -42.03*deg;
    
    G4Box* standRadialBeam_solid = new G4Box("standRadialBeam_solid",
            standRadialX/2, standRadialY/2, standRadialZ/2);
   
    G4Box *standOuterKicker = new G4Box("standOuterKicker_solid",
            standKickerX/2., standKickerY/2., standKickerZ/2.);
    G4Box *standInnerKicker = new G4Box("standInnerKicker_solid",
            standKickerX/2.-standKickerThickness,
            standKickerY/2.+buffer,
            standKickerZ/2.-standKickerThickness);

    G4SubtractionSolid *standKickerFlat =
            new G4SubtractionSolid("standKickerFlat_solid",
            standOuterKicker, standInnerKicker);

    G4RotationMatrix* kick_rotation = new G4RotationMatrix;
    kick_rotation -> rotateX(standKickerAng);
    G4UnionSolid* standRadialBeam = new G4UnionSolid("standRadialBeam",
            standRadialBeam_solid, standKickerFlat, kick_rotation, 
            G4ThreeVector(0, -0.*cm, -.5 * standKickerY * sin(standKickerAng))); 

    // 2nd Beam
    G4RotationMatrix* rad_rotation1 = new G4RotationMatrix;
    G4RotationMatrix* rad_rotation2 = new G4RotationMatrix;
    rad_rotation1->rotateZ(-120*deg);
    rad_rotation2->rotateZ(-240*deg);
	xOff = -triangleSide/2.+ standRadialY*(sqrt(3)/4)
            + standRadialOff * (sqrt(3)/2);
	yOff = -triangleSide*(sqrt(3)/2.) + standRadialY*3/4
            + standRadialOff*3/2.;
    zOff = 0;

    G4UnionSolid* standRadialTemp_solid = 
            new G4UnionSolid("standRadialTemp_solid", standRadialBeam,
            standRadialBeam, rad_rotation1, G4ThreeVector(xOff, yOff, zOff));

    // 3rd Beam
    xOff = -1 * xOff;
	yOff = yOff;
    zOff = 0;
    G4UnionSolid* standRadial_solid = 
            new G4UnionSolid("standRadial_solid", standRadialTemp_solid,
            standRadialBeam, rad_rotation2, G4ThreeVector(xOff, yOff, zOff));

    G4LogicalVolume *standRadial_log = new G4LogicalVolume( standRadial_solid,
            luxMaterials->Steel(), "standRadial_log");
    standRadial_log -> SetVisAttributes( luxMaterials->SteelVis() );
	xOff = 0;
	yOff = triangleSide*(sqrt(3)/3) - standRadialY/2 - standRadialOff;
	zOff = standColumnZ/2. - 7.77875*cm - 5.08*cm - 85.24875*cm;
    standRadial = new LUXSimDetectorComponent(0,
            G4ThreeVector(xOff+detX,yOff+detY,zOff+detZ),
            "standRadial", standRadial_log, standMother, 0,0, false);
    //  END DETECTOR STAND SECTION



    //
    //  SECTION 2
    //  Tower:
    //
	//	The three columns and beams of the tower up from
    //  the detector. In the simulation a column is a single
    //  piece of material versus the actuality which is two
    //  seperate vertical sections.



    // Single Tower Column
    G4double towerColumnX = 10.16*cm;
    G4double towerColumnY = 10.16*cm;
    G4double towerColumnZ = 289.56*cm;
    G4double towerColumnThickness = 0.9525*cm;
    
    G4double triangleSideCol = 75.2475*cm;

    G4Box *towerColumnOuter = new G4Box("towerColumnOuter",
            towerColumnX/2., towerColumnY/2., towerColumnZ/2.);
    G4Box *towerColumnInner = new G4Box("towerColumnInner",
            towerColumnX/2.-towerColumnThickness,
            towerColumnY/2.-towerColumnThickness,
            towerColumnZ/2.+buffer);

    G4SubtractionSolid *towerColumn_solid = 
            new G4SubtractionSolid("towerColumn_solid",
            towerColumnOuter, towerColumnInner);

    // The horizontal tower beams are attached
    G4double towerBeamX = 60.6425*cm;
    G4double towerBeamY = 5.08*cm;
    G4double towerBeamZ = 7.62*cm;
    G4double towerBeamThickness = 0.635*cm;

    G4double triangleSideBeam = towerBeamX + 50.8*cm;//52.07*cm;

    G4Box* towerBeamOuter = new G4Box("towerBeamOuter",
            towerBeamX/2., towerBeamY/2., towerBeamZ/2.);
    G4Box* towerBeamInner = new G4Box("towerBeamInner",
            towerBeamX/2.+buffer,
            towerBeamY/2.-towerBeamThickness,
            towerBeamZ/2.-towerBeamThickness);
    G4SubtractionSolid* towerBeam_solid =
            new G4SubtractionSolid("towerBeam_solid", 
            towerBeamOuter, towerBeamInner);

    xOff = 0;
    yOff = -1* (triangleSideCol * (sqrt(3)/3.) +
            triangleSideBeam * (sqrt(3)/6.));
    zOff = -5.08*cm;

    G4UnionSolid* towerSectionTemp1 = new G4UnionSolid("towerSectionTemp1",
            towerColumn_solid, towerBeam_solid, 0,
            G4ThreeVector(xOff, yOff, zOff));
    zOff = 139.065*cm;
    G4UnionSolid* towerSectionTemp2 = new G4UnionSolid("towerSectionTemp2",
            towerSectionTemp1, towerBeam_solid, 0,
            G4ThreeVector(xOff, yOff, zOff));
    zOff = -139.7*cm;
    G4UnionSolid* towerSection_solid = new G4UnionSolid("towerSection_solid",
            towerSectionTemp2, towerBeam_solid, 0,
            G4ThreeVector(xOff, yOff, zOff));

    // Second Tower Column

    G4RotationMatrix *towCol_rotation1 = new G4RotationMatrix;
    G4RotationMatrix *towCol_rotation2 = new G4RotationMatrix;
    towCol_rotation1 -> rotateZ(120.*deg);
    towCol_rotation2 -> rotateZ(-120.*deg);

    xOff = triangleSideCol/2.;
    yOff = -triangleSideCol*(sqrt(3)/2.);
    zOff = 0;
    G4UnionSolid *towerColumnTemp = new G4UnionSolid("towerColumnTemp",
            towerSection_solid, towerSection_solid, towCol_rotation1,
            G4ThreeVector(xOff, yOff, zOff));

    // Third Tower Column
    xOff = -xOff;
    yOff = yOff;
    zOff = zOff;

    //G4double towerOffset = 139.7*cm;
    G4double towerOffset = 150.0*cm; // A bit of an arbitrary offset.

    G4UnionSolid *towerColumns_solid = new G4UnionSolid("towerColumns_solid",
            towerColumnTemp, towerSection_solid, towCol_rotation2,
            G4ThreeVector(xOff, yOff, zOff));

    G4LogicalVolume *tower_log = new G4LogicalVolume( towerColumns_solid,
            luxMaterials->Steel(), "tower_log");
    tower_log ->SetVisAttributes( luxMaterials->SteelVis() );

    xOff = 0;
    yOff = triangleSideCol*(sqrt(3)/3.);
	//	155.892 is the height of the standBeams
    zOff = 155.892*cm + towerOffset;
    standTower = new LUXSimDetectorComponent( 0,
            G4ThreeVector(xOff+detX,yOff+detY,zOff+detZ),
            "standTower", tower_log, standMother, 0, 0, false);
    //  END TOWER SECTION



	//
    //  SECTION 3
    //  Leveling Ring and Support Rods:
    //
    //  The 18 support rods, leveling ring

    G4double levelingRingDiamInner = 78.74 * cm;
    G4double levelingRingDiamMid   = 86.36 * cm;
    G4double levelingRingDiamOuter = 93.98 * cm;
    G4double levelingRingHeight    = 2.540 * cm;

    G4Tubs* levelingRing_solid = new G4Tubs("levelingRing_solid",
            levelingRingDiamInner/2., levelingRingDiamOuter/2.,
            levelingRingHeight/2., 0, 360*deg);

    const G4int nrods = 18;

    LUXSimDetectorComponent * rods[nrods];

    G4double supportRodLenShort = 100.0125*cm;
    G4double supportRodLenLong = 113.3475*cm;
    G4double supportRodDiamShort = 2.2225*cm;
    G4double supportRodDiamLong = 1.5875*cm;
    G4double supportRodSubDiam = supportRodDiamLong + 2*mm;

    G4Tubs* supportRodLong = new G4Tubs("supportRodLong",
            0, supportRodDiamLong/2., supportRodLenLong/2., 0, 360*deg);

    G4Tubs* supportRodShort = new G4Tubs("supportRodShort",
            0, supportRodDiamShort/2., supportRodLenShort/2., 0, 360*deg);

    G4Tubs* supportRodSubtractor = new G4Tubs("supportRodSubtractor",
            0, supportRodSubDiam/2., supportRodLenLong/2., 0, 360*deg);

    // The thin sections on this rod are asymmetric. I could not tell from the
    // schematics which end was "up", so it is symmetric right now.
    G4UnionSolid* supportRod_solid = new G4UnionSolid("supportRod_solid",
            supportRodLong, supportRodShort, 0, G4ThreeVector());

    G4LogicalVolume* supportRod_log = new G4LogicalVolume( supportRod_solid,
            luxMaterials->Steel(), "supportRod_log");
    supportRod_log -> SetVisAttributes( luxMaterials->SteelVis() );

	// For type purposes, I build a temporary leveling ring of subtraction type.
    // It subtracts from the leveling a leveling ring 1 meter away that doesn't
    // overlap, thus just creating an identical leveling ring of a new type.
    G4SubtractionSolid* levelingRingSub_solid;
    G4SubtractionSolid* levelingRingTemp_solid;
    levelingRingTemp_solid = new G4SubtractionSolid(
        "levelingRingTemp_solid", levelingRing_solid, levelingRing_solid, 0,
        G4ThreeVector(1*m, 0, 0));

    G4double rodx, rody, rodz;
    G4double rodAngle;
    stringstream rodName;

    G4double standRodsOffset = 1.0*m;
    xOff = 0;
    yOff = 0;
    zOff = standRodsOffset;

	for(G4int i=1;i<=nrods;i++) {
        rodAngle = (i*1.0/(1.*nrods)) * 360.*deg;

        rodx = xOff + levelingRingDiamMid/2. * cos(rodAngle);
        rody = yOff + levelingRingDiamMid/2. * sin(rodAngle);
        rodz = zOff;

        // Empty holes in the leveling ring for the support rods using a special
        // G4Tubs that is just thicker than the support rods.
        levelingRingSub_solid = new G4SubtractionSolid(
            "levelingRingSub_solid", levelingRingTemp_solid,
			supportRodSubtractor, 0, G4ThreeVector(rodx, rody, 0));

        levelingRingTemp_solid = levelingRingSub_solid;

		// Create the support rods.
		rodName.str("");
		rodName << "supportRod" << i;
		rods[i] = new LUXSimDetectorComponent(0,
				G4ThreeVector(rodx+detX, rody+detY, rodz+detZ),
				rodName.str(), supportRod_log, standMother, 0, 0, false);

    }

	G4LogicalVolume * levelingRing_log = new G4LogicalVolume(
			levelingRingSub_solid, luxMaterials->Steel(), "levelingRing_log");
    levelingRing_log -> SetVisAttributes( luxMaterials->SteelVis() );

    xOff = 0;
    yOff = 0;
    zOff = supportRodLenLong/2. + standRodsOffset - 2.*cm;

    LUXSimDetectorComponent* levelingRing;
    levelingRing = new LUXSimDetectorComponent(0,
        G4ThreeVector(xOff+detX, yOff+detY, zOff+detZ),
        "levelingRing", levelingRing_log, standMother, 0, 0, false);



	// END LEVELING RING AND SUPPORT ROD SECTION
	
	
	// ***** SECTION 4 ***** //
	
	// PART A
	// Source tubes
	// 
	// Recreating bottom acrylic section next to cryostat with extender modules
	// Steel sections combined into one (only useful as a visual aid)
	
	// Source tube radial distance. The 1.5 cm is the gap between the vacuum
	//	vessel and the source extender tubes.
	G4double tube_r_dist = 54.23025*cm + sourceTubeGap;
        // radius is 39.53 cm (for the OV) + (6.0325)/2 cm (half the source tube side length) + 11.684 cm (for the source tube extenders)
	
	// Source tube dimensions
	//G4double tube_top_h = 2 * 198.755*cm;
	//G4double tube_top_s = 5.3975*cm; // OUTER side length (including wall
									 // thicknesses)
	//G4double tube_top_thick = 0.1651*cm; // wall thickness 0.065"
	
        // source tube mechanical drawings can be found here: http://teacher.pas.rochester.edu:8080/wiki/pub/Lux/DrawingsDiagrams/LUX-YALE-CS-0101.PDF
	G4double tube_bot_h = 232.392*cm;
	G4double tube_bot_s = 6.0325*cm; // OUTER side length (including wall
									 // thicknesses)
	G4double tube_bot_thick = 0.3175*cm; // wall thickness 1/8"
	
	G4double tube_ext_h = 78.74*cm;
	G4double tube_ext_w = 3.0*cm;
	G4double tube_ext_l = 11.684*cm;
	
	G4double tube_ext_thick = tube_bot_thick;
	
	// Source tube placement
	G4double tube_ext_r_dist = tube_r_dist - tube_bot_s/2 - tube_ext_l/2;
	G4double tube_ext_z = -14.6*cm; // offset (approximately right -- 
									// gets extender centered on drift region)
	
	G4double tube_bot_z = tube_ext_z - tube_ext_h/2 + tube_bot_h/2;
	
	G4double tube_vac_z_off = tube_bot_thick/2;
	
	//G4double tube_top_z = tube_bot_z + tube_bot_h/2 + tube_top_h/2;
	
	// Construct steel source tube inner vacuum solid
	//G4double tube_top_vac_z_off = -tube_top_thick/2;
	//G4Box * tube_top_vac_sol = new G4Box("tube_top_vac_sol", 
	//	(tube_top_s - 2*tube_top_thick)/2, 
	//	(tube_top_s - 2*tube_top_thick)/2, 
	//	(tube_top_h - tube_top_thick)/2); // vacuum hangs over top side of tube
	
	// Construct steel source tube walls solid & logical
	//G4Box * tube_top_wall_sol = new G4Box("tube_top_wall_sol", 
	//	tube_top_s/2, tube_top_s/2, tube_top_h/2);
	//G4LogicalVolume * tube_top_wall_log = 
	//	new G4LogicalVolume(tube_top_wall_sol, luxMaterials->Steel(),
	//	"tube_top_wall_log");
	//tube_top_wall_log->SetVisAttributes(luxMaterials->SteelVis());
	
	// Place volumes
	G4double tube_x, tube_y, ext_x, ext_y;
	
	int tube_ctr = 1;
	
	//LUXSimDetectorComponent * tube_top_vac_phys[6];
	LUXSimDetectorComponent * tube_bot_vac_phys[6];
	
	for (G4double angle = 0.0*deg; angle <= 301.0*deg; angle += 60.0*deg)
	{
		// Construct acrylic source tube inner vacuum solid
		char tube_bot_vac_sol_n[32];
		sprintf(tube_bot_vac_sol_n, "tube_bot_vac_sol_%i", tube_ctr);
		G4Box * tube_bot_vac_sol = new G4Box(tube_bot_vac_sol_n, 
			(tube_bot_s - 2*tube_bot_thick)/2, 
			(tube_bot_s - 2*tube_bot_thick)/2, 
			(tube_bot_h - tube_bot_thick)/2); // vacuum hangs over top side of
											  // tube
			
		// Construct acrylic source tube walls solid & logical
		char tube_bot_wall_sol_n[32];
		sprintf(tube_bot_wall_sol_n, "tube_bot_wall_sol_%i", tube_ctr);
		G4Box * tube_bot_wall_sol = new G4Box(tube_bot_wall_sol_n, 
			tube_bot_s/2, tube_bot_s/2, tube_bot_h/2);
		char tube_bot_wall_log_n[32];
		sprintf(tube_bot_wall_log_n, "tube_bot_wall_log_%i", tube_ctr);
		G4LogicalVolume * tube_bot_wall_log = 
			new G4LogicalVolume(tube_bot_wall_sol, luxMaterials->Acrylic(),
			tube_bot_wall_log_n);
		
		tube_bot_wall_log->SetVisAttributes(luxMaterials->AcrylicVis());
		
		// Construct source tube extender region vacuum solid & logical
		char tube_ext_vac_sol_n[32];
		sprintf(tube_ext_vac_sol_n, "tube_ext_vac_sol_%i", tube_ctr);
		G4Box * tube_ext_vac_sol = new G4Box(tube_ext_vac_sol_n, 
			(tube_ext_l - 2*tube_ext_thick)/2, 
			(tube_ext_w - 2*tube_ext_thick)/2, 
			(tube_ext_h - 2*tube_ext_thick)/2);
		char tube_ext_vac_log_n[32];
		sprintf(tube_ext_vac_log_n, "tube_ext_vac_log_%i", tube_ctr);
		G4LogicalVolume *tube_ext_vac_log;
		if( underground ) {
			tube_ext_vac_log = new G4LogicalVolume(tube_ext_vac_sol,
					luxMaterials->Water(), tube_ext_vac_log_n);
			tube_ext_vac_log->SetVisAttributes(luxMaterials->WaterVis());
		} else {
			tube_ext_vac_log = new G4LogicalVolume(tube_ext_vac_sol,
					luxMaterials->Air(), tube_ext_vac_log_n);
			tube_ext_vac_log->SetVisAttributes(luxMaterials->AirVis());
		}
		
		// Construct source tube extender region walls solid & logical
		char tube_ext_wall_sol_n[32];
		sprintf(tube_ext_wall_sol_n, "tube_ext_wall_sol_%i", tube_ctr);
		G4Box * tube_ext_wall_sol = new G4Box(tube_ext_wall_sol_n, 
			tube_ext_l/2, tube_ext_w/2, tube_ext_h/2);
		char tube_ext_wall_log_n[32];
		sprintf(tube_ext_wall_log_n, "tube_ext_wall_log_%i", tube_ctr);
		G4LogicalVolume * tube_ext_wall_log = 
			new G4LogicalVolume(tube_ext_wall_sol, luxMaterials->Acrylic(),
			tube_ext_wall_log_n);
		
		tube_ext_wall_log->SetVisAttributes(luxMaterials->AcrylicVis());
	
		// Set x,y source tube coordinates from angle
		tube_x = tube_r_dist * cos(angle);
		tube_y = tube_r_dist * sin(angle);
		
		// Set x,y extender coordinates from angle
		ext_x = tube_ext_r_dist * cos(angle);
		ext_y = tube_ext_r_dist * sin(angle);
		
		// Set names
		//char tube_top_vac_n[32];
		//sprintf(tube_top_vac_n, "Source_Tube_Top_Vacuum_%i", tube_ctr);
		
		//char tube_top_wall_n[32];
		//sprintf(tube_top_wall_n, "Source_Tube_Wall_%i", tube_ctr);
		
		char tube_bot_vac_n[32];
		sprintf(tube_bot_vac_n, "Source_Tube_Internal_%i", tube_ctr);
		
		char tube_bot_wall_n[32];
		sprintf(tube_bot_wall_n, "Source_Tube_Wall_%i", tube_ctr);
		
		char tube_ext_vac_n[32];
		sprintf(tube_ext_vac_n, "Source_Tube_Extender_Internal_%i", tube_ctr);
		
		char tube_ext_wall_n[32];
		sprintf(tube_ext_wall_n, "Source_Tube_Extender_Wall_%i", tube_ctr);
		
		// Place steel source tube walls
		//LUXSimDetectorComponent * stl_tube_phys = new LUXSimDetectorComponent(
		//	new G4RotationMatrix(G4ThreeVector(0,0,1), -angle), 
		//	G4ThreeVector(tube_x, tube_y, tube_top_z),
		//	tube_top_wall_n,
		//	tube_top_wall_log,
		//	standMother,
		//	0, 0, false);
		
		// Create new top vacuum logical volume
		// (must have unique logical volume for each tube for collimator
		// placement)
		//G4LogicalVolume * tube_top_vac_log = 
		//	new G4LogicalVolume(tube_top_vac_sol, luxMaterials->Vacuum(),
		//	"tube_top_vac_log");
		//tube_top_vac_log->SetVisAttributes(luxMaterials->VacuumVis());
		
		// Place top source tube vacuum inside walls
		//tube_top_vac_phys[tube_ctr-1] = new LUXSimDetectorComponent(
		//	0,
		//	G4ThreeVector(0.0, 0.0, tube_top_vac_z_off),
		//	tube_top_vac_n,
		//	tube_top_vac_log,
		//	stl_tube_phys,
		//	0, 0, false);
		
		// Place acrylic source tube walls
		LUXSimDetectorComponent * tube_phys = new LUXSimDetectorComponent(
			new G4RotationMatrix(G4ThreeVector(0,0,1), -angle), 
			G4ThreeVector(tube_x, tube_y, tube_bot_z),
			tube_bot_wall_n,
			tube_bot_wall_log,
			standMother,
			0, 0, false);
		
		// Create new vacuum logical volume
		// (must have unique logical volume for each tube for collimator
		// placement)
		G4LogicalVolume *tube_bot_vac_log;
		if( underground ) {
			tube_bot_vac_log = new G4LogicalVolume(tube_bot_vac_sol,
					luxMaterials->Water(), "tube_bot_vac_log");
			tube_bot_vac_log->SetVisAttributes( luxMaterials->WaterVis() );
		} else {
			tube_bot_vac_log = new G4LogicalVolume(tube_bot_vac_sol,
					luxMaterials->Air(), "tube_bot_vac_log");
			tube_bot_vac_log->SetVisAttributes( luxMaterials->AirVis() );
		}
		
		// Place source tube vacuum inside walls
		tube_bot_vac_phys[tube_ctr-1] = new LUXSimDetectorComponent(0, 
			G4ThreeVector(0.0, 0.0, tube_vac_z_off),
			tube_bot_vac_n,
			tube_bot_vac_log,
			tube_phys,
			0, 0, false);
			
		// Place extender walls
		LUXSimDetectorComponent * ext_phys = new LUXSimDetectorComponent(
			new G4RotationMatrix(G4ThreeVector(0,0,1), -angle), 
			G4ThreeVector(ext_x, ext_y, tube_ext_z),
			tube_ext_wall_n,
			tube_ext_wall_log,
			standMother,
			0, 0, false);
		
		// Place extender vacuum inside walls
		new LUXSimDetectorComponent(0, 
			G4ThreeVector(),
			tube_ext_vac_n,
			tube_ext_vac_log,
			ext_phys,
			0, 0, false);
		
		
		++tube_ctr;
	}
	
	// ***** END PART A ***** //
	
	// ***** PART B ***** //

	//	First build the collimator for the underground geometry (the collimator
	//	that is mostly tungsten, rather than a mix of tungsten, lead, and steel)
	if( underground ) {

		// Collimator volume dimensions
		G4double collimatorSteelThickness = 0.06 * 2.54 * cm;
		G4double collimatorHeight = 5.709*2.54*cm + collimatorSteelThickness;
		G4double collimatorWidth = (1.303 + 2.*0.254) * 2.54 * cm;
		G4double collimatorDepth = 1.680*2.54*cm + 2.*collimatorSteelThickness;
		
		//	Collimator steel housing dimensions
		G4double collimatorPlugHoleDiameter = 0.580 * 2.54 * cm;
		G4double plugHoleXOffset = -collimatorDepth/2
				+ collimatorSteelThickness/2;
		G4double plugHoleZOffset = -collimatorHeight/2
				+ collimatorSteelThickness + 2.631*2.54*cm;
		
		//	Collimator (i.e., tungsten proper) dimensions
		G4double mainblockHeight = collimatorHeight - collimatorSteelThickness;
		G4double mainblockWidth = collimatorWidth;
		G4double mainblockDepth = collimatorDepth - 2*collimatorSteelThickness;
		
		G4double overlapKludge = 1*mm;
		mainblockBoreDepth = mainblockDepth - 0.92*2.54*cm
				+ overlapKludge;
		boreHoleXOffset = -mainblockDepth/2 + mainblockBoreDepth/2
				- overlapKludge/2;
		boreHoleZOffset = plugHoleZOffset - collimatorSteelThickness/2;

		//	Acrylic source housing dimensions
		G4double acrylicDepth = 0.48 * 2.54 * cm;
		G4double acrylicWidth = 1.303 * 2.54 * cm;
		G4double acrylicHeight = 1.68 * 2.54 * cm;
		G4double acrylicXOffset = -mainblockDepth/2 + acrylicDepth/2
				+ mainblockBoreDepth - overlapKludge/2;
		G4double acrylicZOffset = -collimatorHeight/2 +
				+ acrylicHeight/2 + collimatorSteelThickness + 1.96*2.54*cm;
		
		//	Source dimensions (basically, the Z offset within (i.e., relative to
		//	only) the center of the acrylic block)
		G4double sourceZOffset = -5.*mm;
		
		//	Dimensions are set, now actually build the thing
		
		//	Create the outer box of water that will contain all the tungsten,
		//	steel, and acrylic parts.
		G4Box *collimatorVolume_solid = new G4Box( "collimatorVolume_solid",
				collimatorDepth/2, collimatorWidth/2, collimatorHeight/2 );
		for( G4int i=0; i<6; i++ ) {
			stringstream name;
			name << "collimatorVolume_log_" << i+1;
			collimatorVolume_log[i] = new G4LogicalVolume(
					collimatorVolume_solid, luxMaterials->Water(),
					name.str().c_str() );
			collimatorVolume_log[i]->SetVisAttributes(luxMaterials->WaterVis());
		}
		
		//	For the housing, create the front, back, and bottom plates, and put
		//	an aperture in the front plate. Note that these plates are not
		//	individual plates, but are instead an appropriately hollowed-out
		//	box.
		G4Box *collimatorHousing_solid1 = new G4Box( "collimatorHousing_solid1",
				collimatorDepth/2, collimatorWidth/2, collimatorHeight/2 );
		G4Box *collimatorHousing_solid2 = new G4Box( "collimatorHousing_solid2",
				collimatorDepth/2 - collimatorSteelThickness,
				collimatorWidth/2 + collimatorSteelThickness,
				collimatorHeight/2 );
		G4SubtractionSolid *collimatorHousing_solid3 = new G4SubtractionSolid(
				"collimatorHousing_solid3",
				collimatorHousing_solid1, collimatorHousing_solid2,
				0, G4ThreeVector( 0, 0, collimatorSteelThickness ) );
		
		G4Tubs *collimatorHousing_solid4 = new G4Tubs(
				"collimatorHousing_solid4", 0, collimatorPlugHoleDiameter/2,
				collimatorSteelThickness, 0, 360.*deg );
		G4SubtractionSolid *collimatorHousing_solid = new G4SubtractionSolid(
				"collimatorHousing_solid", collimatorHousing_solid3,
				collimatorHousing_solid4,
				new G4RotationMatrix(G4ThreeVector(0,1,0), 90.*deg),
				G4ThreeVector(plugHoleXOffset,0,plugHoleZOffset) );
		for( G4int i=0; i<6; i++ ) {
			stringstream name;
			name << "collimatorHousing_log_" << i+1;
			collimatorHousing_log[i] = new G4LogicalVolume(
					collimatorHousing_solid, luxMaterials->Steel(),
					name.str().c_str() );
			collimatorHousing_log[i]->SetVisAttributes(
					luxMaterials->SteelVis() );
		}
		
		//	Next, do the tungsten block. As with the surface geometry, the
		//	default collimation aperture is 1 mm.
		mainblock_solid1 = new G4Box( "mainblock_solid1",
				mainblockDepth/2, mainblockWidth/2, mainblockHeight/2 );
		mainblock_solid2 = new G4Tubs( "mainblock_solid2",
				0, 1.*mm/2, mainblockBoreDepth/2, 0, 360.*deg );
		mainblock_solid = new G4SubtractionSolid(
				"mainblock_solid", mainblock_solid1, mainblock_solid2,
				new G4RotationMatrix( G4ThreeVector(0,1,0), 90.*deg ),
				G4ThreeVector(boreHoleXOffset,0,boreHoleZOffset) );
		for( G4int i=0; i<6; i++ ) {
			stringstream name;
			name << "collimator_log_" << i+1;
			collimator_log[i] = new G4LogicalVolume(
					mainblock_solid, luxMaterials->Tungsten(),
					name.str().c_str() );
			collimator_log[i]->SetVisAttributes(
					luxMaterials->TungstenVis() );
		}
		
		//	Now the acrylic block
		G4Box *acrylicBlock_solid = new G4Box( "acrylicBlock_solid",
				acrylicDepth/2, acrylicWidth/2, acrylicHeight/2 );
		
		for( G4int i=0; i<6; i++ ) {
			stringstream name;
			name << "sourceHousing_log_" << i+1;
			sourceHousing_log[i] = new G4LogicalVolume(
					acrylicBlock_solid, luxMaterials->Acrylic(),
					name.str().c_str() );
			sourceHousing_log[i]->SetVisAttributes( luxMaterials->AcrylicVis());
		}
		
		//	Finally the source itself. The active volume is modeled with a 5-mm
		//	diameter and a 100-micron thickness
		activeSourceDiameter = 5*mm;
		source_solid = new G4Tubs( "source_solid", 0,
				activeSourceDiameter/2, 0.1*mm/2, 0, 360.*deg );
		
		G4VisAttributes *collimatorSourceVis = new G4VisAttributes();
		collimatorSourceVis->SetColor( G4Color::Red() );
		for( G4int i=0; i<6; i++ ) {
			stringstream name;
			name << "collimatorSource_log_" << i+1;
			collimatorSource_log[i] = new G4LogicalVolume(
					source_solid, luxMaterials->Aluminum(),
					name.str().c_str() );
			collimatorSource_log[i]->SetVisAttributes( collimatorSourceVis );
		}
		
		//	Create all the physical volumes
		collimator_z = -tube_bot_h/2 + tube_ext_h/2;
		collimatorHousing_z = collimator_z - collimatorCavityOffset;
		for( G4int i=0; i<6; i++ ) {
			stringstream name;
			name << "SurroundingColVolume_" << i+1;
			collimator_Volume[i] = new LUXSimDetectorComponent( 0,
					G4ThreeVector(0,0,collimatorHousing_z),
					name.str().c_str(), collimatorVolume_log[i],
					tube_bot_vac_phys[i], 0, 0, false );
			
			name.str("");
			name << "CollimatorHousing_" << i+1;
			collimatorHousing[i] = new LUXSimDetectorComponent( 0,
					G4ThreeVector(), name.str().c_str(),
					collimatorHousing_log[i], collimator_Volume[i],
					0, 0, false );

			name.str("");
			name << "Collimator_" << i+1;
			collimator[i] = new LUXSimDetectorComponent( 0,
					G4ThreeVector(0,0,collimatorSteelThickness/2),
					name.str().c_str(), collimator_log[i], collimator_Volume[i],
					0, 0, false );

			name.str("");
			name << "CollimatorHousingSource_" << i+1;
			sourceHousing[i] = new LUXSimDetectorComponent( 0,
					G4ThreeVector(acrylicXOffset,0,acrylicZOffset),
					name.str().c_str(), sourceHousing_log[i],
					collimator[i], 0, 0, false );

			name.str("");
			name << "CollimatorSource_" << i+1;
			collimator[i] = new LUXSimDetectorComponent(
					new G4RotationMatrix( G4ThreeVector(0,1,0), 90.*deg),
					G4ThreeVector(0,0,sourceZOffset),
					name.str().c_str(), collimatorSource_log[i],
					sourceHousing[i], 0, 0, false );
		}
		
		
	} else {
	//	Now handle the case of the surface collimator geometry (the
	//	collimator with steel and lead parts instead of mostly tungsten).
		// Collimator housing
		
		// Collimator dimensions
		G4double collimatorHeight = (5.709 + 0.063) * 2.54 * cm;
		G4double collimatorWidth = (1.303 + 2.*0.254) * 2.54 * cm;
		G4double collimatorDepth = (1.680 + 2.*0.063) * 2.54 * cm;
		G4double collimatorPlugHoleDiameter = 0.580 * 2.54 * cm;
		G4double collimatorBottomThickness = 0.063 * 2.54 * cm;
		G4double collimatorTopThickness = 0.447 * 2.54 * cm;
		G4double collimatorSidesThickness = 0.254 * 2.54 * cm;
		
		G4double cavityDepth = collimatorDepth - 2*collimatorBottomThickness;
		G4double cavityWidth = collimatorWidth - 2*collimatorSidesThickness;
		G4double cavityHeight = collimatorHeight - collimatorBottomThickness -
				collimatorTopThickness;
		
		//	Create collimator solid and logical. Do this by creating the outer
		//	box, and then subtract out the central volume including the bore
		//	hole for the removable tungsten plug.
		G4Box *collimator1 = new G4Box( "collimator1", collimatorDepth/2,
				collimatorWidth/2, collimatorHeight/2 );
		
		collimatorCavity1 = new G4Box( "collimatorCavity1",
				cavityDepth/2, cavityWidth/2, cavityHeight/2 );
		G4Tubs *collimatorCavity2 = new G4Tubs( "collimatorCavity2", 0,
				collimatorPlugHoleDiameter/2, 1.*cm, 0, 360.*deg );
		G4UnionSolid *collimatorCavity = new G4UnionSolid( "collimatorCavity",
				collimatorCavity1, collimatorCavity2,
				new G4RotationMatrix(G4ThreeVector(0,1,0), 90.0*deg),
				G4ThreeVector( -collimatorDepth/2, 0, 0 ) );
				
		collimatorCavityOffset = -collimatorHeight/2 + cavityHeight/2 +
				collimatorBottomThickness;
		G4SubtractionSolid *collimatorHousing_solid = new G4SubtractionSolid(
				"collimatorHousing_solid", collimator1, collimatorCavity, 0,
				G4ThreeVector( 0, 0, collimatorCavityOffset ) );
		
		for( G4int i=0; i<6; i++ ) {
			stringstream name;
			name << "collimatorHousing_log_" << i+1;
			collimatorHousing_log[i] = new G4LogicalVolume(
					collimatorHousing_solid, luxMaterials->Steel(),
					name.str().c_str() );
			collimatorHousing_log[i]->SetVisAttributes(
					luxMaterials->SteelVis() );
		}
		
		//	The collimator itself. This is a block of tungsten with a cavity for
		//	the source, and a bore hole to provide collimation.
		G4double sourceCavityDiameter = 1.*2.54*cm;
		G4double sourceCavityHeight = 0.25*2.54*cm;
		collimatorBoreDepth = (1.680-0.258-0.25)*2.54*cm;
		
		//	Create a default collimator diameter of 1 mm. Also, the source cavity
		//	has a clearance of 0.01 inches all around.
		sourceCavity1 = new G4Tubs( "sourceCavity1", 0, 1.*mm/2,
				collimatorBoreDepth/2 + 1.*mm, 0, 360.*deg );
		sourceCavity2 = new G4Tubs( "sourceCavity2", 0,
				sourceCavityDiameter/2 + 0.01*25.4*mm,
				sourceCavityHeight/2 + 0.01*25.4*mm, 0,
				360.*deg );
		
		collimatorBoreOffset =
				-(collimatorBoreDepth/2 + sourceCavityHeight/2 - 0.01*25.4*mm);
		sourceCavity_solid = new G4UnionSolid( "sourceCavity_solid",
				sourceCavity1, sourceCavity2, 0,
				G4ThreeVector( 0, 0, collimatorBoreOffset ) );
		
		sourceCavityOffset = -cavityDepth/2 + collimatorBoreDepth/2;
		collimator_solid = new G4SubtractionSolid( "collimator_solid", 
				collimatorCavity1, sourceCavity_solid,
				new G4RotationMatrix(G4ThreeVector(0,1,0), 90.0*deg),
				G4ThreeVector( sourceCavityOffset, 0, 0 ) );
		
		//	Create the shape of the lead plate to place inside each collimator
		//	logical.
		G4double leadPlateDepth = cavityDepth - collimatorBoreDepth -
				sourceCavityHeight;
		G4Box *leadPlate_solid = new G4Box( "leadPlate_solid", leadPlateDepth/2,
				cavityWidth/2, cavityHeight/2 );

		G4double leadPlateOffset = cavityDepth/2 + -leadPlateDepth/2;


		for( G4int i=0; i<6; i++ ) {
			stringstream name;
			name << "collimator_log_" << i+1;
			collimator_log[i] = new G4LogicalVolume( collimator_solid,
					luxMaterials->Tungsten(), name.str().c_str() );
			collimator_log[i]->SetVisAttributes( luxMaterials->TungstenVis() );
			
			name.str("");
			name << "leadPlate_log_" << i+1;
			leadPlate_log[i] = new G4LogicalVolume( leadPlate_solid,
					luxMaterials->Lead(), name.str().c_str() );
			leadPlate_log[i]->SetVisAttributes( luxMaterials->LeadVis() );
			
			name.str("");
			name << "leadPlate_" << i+1;
			leadPlate[i] = new LUXSimDetectorComponent( 0,
					G4ThreeVector(leadPlateOffset,0,0), leadPlate_log[i],
					name.str().c_str(), collimator_log[i], 0, 0, false );
		}
		
		//	Now put the source inside a plastic disk, and put that disk inside the
		//	source cavity. Conveniently, the source cavity is vertically centered
		//	in the collimator block.
		G4Tubs *sourceHousing_solid = new G4Tubs( "sourceHousing_solid", 0,
				sourceCavityDiameter/2, sourceCavityHeight/2, 0, 360.*deg );
		for( G4int i=0; i<6; i++ ) {
			stringstream name;
			name.str("");
			name << "sourceHousing_log_" << i+1;
			sourceHousing_log[i] = new G4LogicalVolume(
				sourceHousing_solid, luxMaterials->Polyethylene(),
				name.str().c_str() );
			sourceHousing_log[i]->SetVisAttributes(luxMaterials->PolyethyleneVis());
		}
		
		activeSourceDiameter = 5*mm;
		source_solid = new G4Tubs( "source_solid", 0,
				activeSourceDiameter/2, 0.1*mm/2, 0, 360.*deg );
		G4VisAttributes *collimatorSourceVis = new G4VisAttributes();
		collimatorSourceVis->SetColor( G4Color::Red() );
		for( G4int i=0; i<6; i++ ) {
			stringstream name;
			name << "collimatorSource_log_" << i+1;
			collimatorSource_log[i] = new G4LogicalVolume(
					source_solid, luxMaterials->Aluminum(),
					name.str().c_str() );
			collimatorSource_log[i]->SetVisAttributes( collimatorSourceVis );
		}

		// Place collimator housings, collimators, and sources in all source
		// tubes. The initial placement is in the middle of the drift region, 
		// although the vertical placement is user-settable.
		collimator_z = -tube_bot_h/2 + tube_ext_h/2;
		collimatorHousing_z = collimator_z - collimatorCavityOffset;
		sourceHousingOffset = -cavityDepth/2 + collimatorBoreDepth +
				sourceCavityHeight/2;
		sourceHousing_z = collimator_z;
		
		for( G4int i=0; i<6; i++ ) {

			stringstream name;
			
			name << "CollimatorHousing_" << i+1;
			collimatorHousing[i] = new LUXSimDetectorComponent(
				0, 
				G4ThreeVector(0, 0, collimatorHousing_z),
				name.str().c_str(),
				collimatorHousing_log[i],
				tube_bot_vac_phys[i],
				0, 0, false);
			
			name.str("");
			name << "Collimator_" << i+1;
			collimator[i] = new LUXSimDetectorComponent(
				0,
				G4ThreeVector(0, 0, collimator_z),
				name.str().c_str(),
				collimator_log[i],
				tube_bot_vac_phys[i],
				0, 0, false );
			
			name.str("");
			name << "CollimatorSourceHousing_" << i+1;
			sourceHousing[i] = new LUXSimDetectorComponent(
				new G4RotationMatrix(G4ThreeVector(0,1,0), 90.0*deg),
				G4ThreeVector(sourceHousingOffset, 0, sourceHousing_z),
				name.str().c_str(),
				sourceHousing_log[i],
				tube_bot_vac_phys[i],
				0, 0, false );
			
			name.str("");
			name << "CollimatorSource_" << i+1;
			collimatorSource[i] = new LUXSimDetectorComponent(
				0, G4ThreeVector(), name.str().c_str(), collimatorSource_log[i],
				sourceHousing[i], 0, 0, false );
		}
	}
	// now add neutron collimator if needed
	if( useNeutronCollimator ) {
		const G4double vacuumVesselRadius = 39.53 * cm;
		const G4double neutronCollimatorGap = 2.5 * cm; // this number comes from Dave it's the
								// approximate gap between collimator and Ti
		// dimension of the pipe taken from the TWiki
		G4double neutronCollimatorDiamInner = 4.9 * cm;
		G4double neutronCollimatorDiamOuter = 6.0 * cm;
		G4double neutronCollimatorLength = 339.5 * cm;
		G4RotationMatrix *rotX90 = new G4RotationMatrix();
		rotX90->rotateZ(15.*deg - 6.0725*deg); 
                // 15 degrees because it makes the rotation work - 15 degress for the detector stand
                // 6.07 degrees is the rotation for the neutron source
		rotX90->rotateX(90.*deg);
                G4double xOffset = -27.3516 * cm; //54.38 * cm;
                G4double yOffset = -209.6988 * cm; //neutronCollimatorLength/2.+vacuumVesselRadius+neutronCollimatorGap - 6. * cm;
         	G4double zOffset = neutronCollimatorHeight + neutronCollimatorDiamOuter/2. - 42.62 * cm - 0.15 * cm;
                // this is the same 42.62cm hardcoded in
 	        // line 180 of LUXSimDetectorConstruction.cc
                // plus 1.5mm offset that we saw by plotting the primary postion of particles emmited by the tube itself
		G4Tubs* neutronCollimator_solid = new G4Tubs("neutronCollimator_solid",
			neutronCollimatorDiamInner/2., neutronCollimatorDiamOuter/2.,
			neutronCollimatorLength/2., 0, 360*deg);
		G4LogicalVolume* neutronCollimator_log = new G4LogicalVolume( neutronCollimator_solid,
			luxMaterials->PVC(), "neutronCollimator_log");
		neutronCollimator_log -> SetVisAttributes( luxMaterials->PVCVis() );
                // here we place the neutron collimator along the y-axis
		neutronCollimator = new LUXSimDetectorComponent(
			rotX90,
			G4ThreeVector(xOffset, yOffset, zOffset),
			"neutronCollimator",
			neutronCollimator_log,
			standMother, 0, 0, false);
                // now add the hollow inside
		G4Tubs* neutronCollimatorInside_solid = new G4Tubs("neutronCollimator_solid",
			0, neutronCollimatorDiamInner/2.,
			neutronCollimatorLength/2., 0, 360*deg);
		G4LogicalVolume* neutronCollimatorInside_log = new G4LogicalVolume( neutronCollimatorInside_solid,
			luxMaterials->Air(), "neutronCollimatorInside_log");
		neutronCollimatorInside_log -> SetVisAttributes( luxMaterials->AirVis() );
                // here we place the neutron collimator along the y-axis
		neutronCollimatorInside = new LUXSimDetectorComponent(
			rotX90,
			G4ThreeVector(xOffset, yOffset, zOffset),
			"neutronInsideCollimator",
			neutronCollimatorInside_log,
			standMother, 0, 0, false);
	}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimStand
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimStand::~LUXSimStand() {}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				SetCollimatorHeight
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimStand::SetCollimatorHeight(G4double h)
{
	G4cout << "LUXSimStand::SetCollimatorHeight : h : " << h << " mm" << G4endl;
	
	//	First handle the underground geometry
	if( underground ) {
	
		collimator_z = -62.381*cm - 42.62*cm + 4.8774*mm + h;
		
		for( G4int i=0; i<6; i++ )
			collimator_Volume[i]->SetTranslation( 
					G4ThreeVector(0, 0, collimator_z) );
	
	//	Now handle the surface geometry
	} else {

		collimator_z = -62.381*cm - 42.62*cm + h; // in line with global z coord.
		collimatorHousing_z = collimator_z - collimatorCavityOffset;
		sourceHousing_z = collimator_z;
		
		for (int ii_col = 0; ii_col < 6; ii_col++)
		{
			collimatorHousing[ii_col]->SetTranslation(
					G4ThreeVector(0, 0, collimatorHousing_z) );
			collimator[ii_col]->SetTranslation(
					G4ThreeVector(0, 0, collimator_z) );
			sourceHousing[ii_col]->SetTranslation(
					G4ThreeVector(sourceHousingOffset, 0, sourceHousing_z) );
		}

	}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				SetCollimatorHoleDiameter
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimStand::SetCollimatorHoleDiameter(G4double d)
{	
	G4cout << "LUXSimStand::SetCollimatorHole : d : " << d << " mm" << G4endl;
	
	//	First handle the underground geometry
	if( underground ) {
	
		if( d ) {
		
			mainblock_solid2 = new G4Tubs( "mainblock_solid2", 0, d/2,
					mainblockBoreDepth/2, 0, 360.*deg );
			mainblock_solid = new G4SubtractionSolid( "mainblock_solid",
					mainblock_solid1, mainblock_solid2,
					new G4RotationMatrix( G4ThreeVector(0,1,0), 90.*deg ),
					G4ThreeVector(boreHoleXOffset,0,boreHoleZOffset) );
		
		} else {
		
			mainblock_solid = mainblock_solid1;
		
		}
		
		for( G4int i=0; i<6; i++ )
			collimator_log[i]->SetSolid( mainblock_solid );
	
	//	Now handle the surface geometry
	} else {
		if( d ) {
		
			sourceCavity1 = new G4Tubs( "sourceCavity1", 0, d/2,
					collimatorBoreDepth/2 + 1.*mm, 0, 360.*deg );

			sourceCavity_solid = new G4UnionSolid( "sourceCavity_solid",
				sourceCavity1, sourceCavity2, 0,
				G4ThreeVector( 0, 0, collimatorBoreOffset ) );

			collimator_solid = new G4SubtractionSolid( "collimator_solid", 
					collimatorCavity1, sourceCavity_solid,
					new G4RotationMatrix(G4ThreeVector(0,1,0), 90.0*deg),
					G4ThreeVector( sourceCavityOffset, 0, 0 ) );

		} else {
		
			collimator_solid = new G4SubtractionSolid( "collimator_solid",
					collimatorCavity1, sourceCavity2,
					new G4RotationMatrix(G4ThreeVector(0,1,0), 90.0*deg),
					G4ThreeVector( sourceHousingOffset, 0, 0 ) );

		}

		for (int ii_col = 0; ii_col < 6; ii_col++)
			collimator_log[ii_col]->SetSolid( collimator_solid );
	}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				SetCollimatorSourceDiameter
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimStand::SetCollimatorSourceDiameter(G4double d)
{	

	if( d == 0 ) {
		d = 1.*nm;
		G4cout << "Cannot set the collimator source diameter to 0, so setting "
			   << "to 1 nm." << G4endl;
	}
	
	G4cout << "LUXSimStand::SetCollimatorSourceDiameter : d : " << d << " mm"
		   << G4endl;
	
	activeSourceDiameter = d;
	
	source_solid = new G4Tubs( "source_solid", 0, d/2, 0.1*mm/2, 0, 360.*deg );
	
	for( G4int i=0; i<6; i++ )
		collimatorSource_log[i]->SetSolid( source_solid );
}
