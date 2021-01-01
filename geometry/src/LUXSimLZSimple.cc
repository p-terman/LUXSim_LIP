////////////////////////////////////////////////////////////////////////////////
//	
//	LUXSimLZSimple.cc
//
//	This is the code file to define a simple LZ geometry. This is nothing more
//	than a bunch of nested cylinders. The radial dimensions are decently
//	accurate. The verticle dimensions are accurate modulo the shape changes
//	(e.g., The height of the titanium vessels are the height of the vessels in
//	the high-fidelity geometry, though in the simple case there tops and bottoms
//	are not rounded. Thus the volume inside the vessels in the simple geometry
//	is larger.) That said, I did my best to make the active xenon volume close
//	to 7 tonnes.
//
////////////////////////////////////////////////////////////////////////////////
//
//	Change log
//
//	14 Apr 13 - Initial submission. Note that the scintillator material,
//				isohexane, only has an index of refraction defined, and not
//				any scintillation properties whatsoever. (Kareem)
//	23 Apr 13 - Fine-tuning various dimensions (Kareem)
//	23 Apr 13 - Added an optional neutron calibration tube (Kareem)
//	26 Apr 13 - Added source calibration tubes and a source volume with variable
//				Z height (Kareem)
//	20 May 13 - Calibration tube now includes an extension within the
//				scintillator volume (Kareem)
//  23 Jun 13 - Make the PTFE wall inner radius a class-level variable to be
//              able to pick it up in the manager (Kareem)
//      21 Mar 14 - Change scintillator to dopedLABGd (linear akyl benzene doped with Gd) (Kevin)
//      14-May-14 - Added option in LZSimple geometry for the tungsten pig for a YBe
//	        source. (Kevin)
//      16-May-14 - Updated neutron calibration tube (Kevin)
//
////////////////////////////////////////////////////////////////////////////////

//
//      C/C++ includes
//
#include <sstream>

//
//      GEANT4 includes
//
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4SubtractionSolid.hh"

//
//      LUXSim includes
//
#include "LUXSimLZSimple.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

//
//		Definitions
//
#define PI 3.14159265358979312
#define PRINTDIMENSIONS 0

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                              LUXSimLZSimple
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZSimple::LUXSimLZSimple()
{
	luxMaterials = LUXSimMaterials::GetMaterials();
	
    //
	//
	//	Set the relevent dimensions of the entire simplified detector.
    //
	//
	
	//	Start with the acrylic vessel
	outerAcrylicRadius = (11.*12+2.)*2.54*cm/2;	//	11'2" OD
	G4double acrylicThickness =	2.54*cm;					//	1" thick
	innerAcrylicRadius = outerAcrylicRadius - acrylicThickness;
	G4double outerAcrylicHeight = (15.*12.+2.)*2.54*cm;		//	15'2" height
	G4double innerAcrylicHeight = outerAcrylicHeight - 2.*acrylicThickness;
	
	//	Next the large outer titanium vessel, also known as the vacuum vessel.
	//	It has no flange, reinforcement, or rounded top/bottom. It's just a
	//	cylinder. Round with flat ends. Yup. Cylinder. 
	
	//	The dimensions for the outer and inner titanium vessels came from the
	//	diagrams up at
	//	http://teacher.pas.rochester.edu:8080/wiki/bin/view/Lz/LzDrawingsDiagrams
	//	I'll try to call out specific files as I find them.
	
	//	http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LzDrawingsDiagrams/LZ-LBNL-CV-0002_weldment_outer_cryostat_vessel.PDF
	//	http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LzDrawingsDiagrams/LZ-LBNL-CV-0003_weldment_outer_lid_cryostat_vessel.PDF
	G4double outerLargeTiVesselRadius = (75.75-2*3.5)*2.54*cm/2;
	G4double largeTiVesselThickness = 5./16*2.54*cm;
	G4double innerLargeTiVesselRadius = outerLargeTiVesselRadius -
			largeTiVesselThickness;
	G4double outerLargeTiVesselHeight = (102.53+12.62)*2.54*cm;
	G4double innerLargeTiVesselHeight = outerLargeTiVesselHeight -
			2.*largeTiVesselThickness;
	
	//	Small titanium vessel
	//	http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LzDrawingsDiagrams/LZ-LBNL-CV-0004_weldment_inner_lid_cryostat_vessel.PDF
	//	http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LzDrawingsDiagrams/LZ-LBNL-CV-0005_weldment_inner_cryostat_vessel.PDF
    //  This next file is outdated, but provides some of the vertical spacings
    //  between various parts of the detector
    //  http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LzDrawingsDiagrams/LZ_Vertical_Heights.pdf
	G4double outerSmallTiVesselRadius = 61.88*2.54*cm/2;
	G4double smallTiVesselThickness = 5./16*2.54*cm;
	G4double innerSmallTiVesselRadius = outerSmallTiVesselRadius -
			smallTiVesselThickness;
	G4double outerSmallTiVesselHeight = (89.22+11.45)*2.54*cm;
	G4double innerSmallTiVesselHeight = outerSmallTiVesselHeight -
			2.*smallTiVesselThickness;
    G4double smallTiVesselHeightAboveBottom = (60.24-55.8)*2.54*cm;
	
	//	Source guide tubes. These are simply hollow tubes of titanium, 4 cm in
	//	diameter and 1 mm thick. Dimensions come from Kevin O'Sullivan.
	G4double sourceTubeOD = 4.*cm;
	G4double sourceTubeThicknes = 1.*mm;
	G4double sourceTubeID = sourceTubeOD - sourceTubeThicknes;
	G4double sourceTubeHeight = outerSmallTiVesselHeight;
	
	//	This PTFE liner. This is the PTFE layer on the inside of the titanium to
    //  reflect scintillation in the skin.
	G4double ptfeLinerThickness = 1.*mm;
    G4double innerPTFELinerRadius = innerSmallTiVesselRadius -
            ptfeLinerThickness;
    G4double outerPTFELinerHeight = innerSmallTiVesselHeight;
    G4double innerPTFELinerHeight = outerPTFELinerHeight -2.*ptfeLinerThickness;

    //	PTFE wall. Joe Saba confirms that the PTFE wall is 2 cm thick, and that
	//	the xenon skin is 6.27 cm thick at the flats of the icosagon. After a
	//	little geometry, the skin thickness at the corners is 5.38 cm. Because
	//	we are dealing with a cylinder and not an icosagon, we need the average
	//	depth, which I'm taking to simply the after of these two lengths (not
	//	precise, but close enough). A quick-and-dirty average is 5.82 cm.
	
	//	The skin thickness of 5.82 cm should be compared to the previous default
	//	value within LZFlex of 4 cm. The wall thickness of 2 cm should be
	//	compared to the previous LZFlex default value of 3 cm.
	G4double xenonSkinThickness = 5.82*cm;
    G4double ptfeWallOuterRadius = innerPTFELinerRadius - xenonSkinThickness;
    G4double ptfeWallThickness = 2.*cm;
    ptfeWallInnerRadius = ptfeWallOuterRadius - ptfeWallThickness;
    G4double ptfeWallHeight = (138.056 - 75.725)*2.54*cm;
    
    //  PMT slab. This dimension defines the top and bottom pancake PMTs in the
    //  active xenon volume.
    G4double pmtWindowThickness = 1.*mm;
    G4double pmtHeight = 12.3*cm - pmtWindowThickness;
    G4double pmtHeightAboveBottom = (75.725 - 60.24)*2.54*cm - pmtHeight -
            pmtWindowThickness;
    G4double bottomSkinHeight = pmtHeightAboveBottom - pmtWindowThickness;
	
    //  Liquid xenon height. This the *total* liquid height, as measured from
    //  the inner surface of the PTFE liner to the liquid level. It must take
    //  into account two concerns: the height of the PMT, and the height of the
    //  gamma-X volume.
    
    //  I had to make a guess about this next value. The distance between the
    //  PMT banks is the ptfeWallHeight defined about a dozen lines up. But
    //  part of that distance is taken up by the S2 volume, which I am
    //  estimating to be 2 inches.
    G4double s2VolumeHeight = 2.*2.54*cm;
    G4double gammaXVolumeHeight = 13.*cm;
    G4double ptfeWallHeightLiquid = ptfeWallHeight - s2VolumeHeight;
    G4double activeLiquidXenonHeight = ptfeWallHeightLiquid -gammaXVolumeHeight;

    G4double xOffset, yOffset, zOffset;
    xOffset = yOffset = zOffset = 0;
    
    //  Anyway, with that defined I can now set the total liquid height
    G4double totalLiquidHeight = ptfeWallHeight + pmtHeight +
            pmtHeightAboveBottom - s2VolumeHeight;
    G4double totalGasHeight = innerPTFELinerHeight - totalLiquidHeight;
    G4double topSkinHeight = totalGasHeight - s2VolumeHeight - pmtHeight -
            pmtWindowThickness;
	
	//	This is a kludge part. It's for a neutron calibration tube.
	neutronCalibrationTubeOD = 7.6*cm;
	neutronCalibrationTubeThickness = 0.5*cm;
	G4double neutronCalibrationTubeThicknessInScintillator = 0.5*cm;
	G4double neutronCalibrationTubeID = neutronCalibrationTubeOD -
			neutronCalibrationTubeThicknessInScintillator;
	//	The extra 1 cm in these next lines are to prevent overlap errors with
	//	the acrylic vessel.
        yOffset = luxManager->GetLZSimpleNeutronCalTubeYPosition();
        G4double outerYOffset = neutronCalibrationTubeOD / 2.;
        if(yOffset > 0) {
                outerYOffset = outerYOffset * -1.;
        }
	outerYOffset = yOffset + outerYOffset;
        G4double neutronCalibrationTubeScintillatorXOut = sqrt(innerAcrylicRadius * innerAcrylicRadius - outerYOffset * outerYOffset);
        outerYOffset = neutronCalibrationTubeOD / 2.;
        if(yOffset < 0) {
                outerYOffset = outerYOffset * -1.;
        }
	outerYOffset = yOffset + outerYOffset;
        G4double neutronCalibrationTubeScintillatorXIn = sqrt(outerLargeTiVesselRadius * outerLargeTiVesselRadius - outerYOffset * outerYOffset);
        G4double neutronCalibrationTubeScintillatorHeight = neutronCalibrationTubeScintillatorXOut -
                        neutronCalibrationTubeScintillatorXIn;    
        G4double neutronCalibrationTubeScintilltorHeightVertical =
            (innerAcrylicHeight - outerLargeTiVesselHeight)/2 - 1.*cm;
    //
    //
    //  Construct the detector
    //
    //
    
    //  The acrylic vessel. This is the logical detector volume, and everything
    //  else is a daughter (or sub-daughter) of this volume.
    G4Tubs *acrylicVessel_solid = new G4Tubs( "acrylicVessel_solid", 0,
            outerAcrylicRadius, outerAcrylicHeight/2, 0, 360.*deg );
    logicalVolume = new G4LogicalVolume( acrylicVessel_solid,
            luxMaterials->Acrylic(), "acrylicVessel_log" );
    logicalVolume->SetVisAttributes( luxMaterials->AcrylicVis() );
    
    //  The LAB scintillator
    G4Tubs *scintillator_solid = new G4Tubs( "scintillator_solid", 0,
            innerAcrylicRadius, innerAcrylicHeight/2, 0, 360.*deg );
    G4LogicalVolume *scintillator_log = new G4LogicalVolume( scintillator_solid,
            luxMaterials->DopedLABGd(), "scintillator_log" );
    scintillator_log->SetVisAttributes( luxMaterials->LABVis() );
    LUXSimDetectorComponent *scintillator = new LUXSimDetectorComponent(
            0, G4ThreeVector(), scintillator_log, "Scintillator", logicalVolume,
            0, 0, false );
	
                neutronCalibrationTubeZOffset = -innerPTFELinerHeight/2 + pmtHeightAboveBottom +
                                                pmtHeight + luxManager->GetLZSimpleNeutronCalTubeHeight() +
                                                pmtWindowThickness + 5 * cm;
	//	The air tube within the scintillator for external neutron calibration
	if( luxManager->GetLZSimpleNeutronCalTube() ) {
		G4cout << "Creating the scintillator tube" << G4endl;
		G4RotationMatrix *rotY90 = new G4RotationMatrix();
		rotY90->rotateY(90.*deg);
                xOffset = -1. * (neutronCalibrationTubeScintillatorXOut - neutronCalibrationTubeScintillatorHeight/2.);
                yOffset = luxManager->GetLZSimpleNeutronCalTubeYPosition();
                zOffset = neutronCalibrationTubeZOffset;
                G4Tubs *scintillatorNeutronTube_cyl = new G4Tubs("scintillatorNeutronTube_solid",
                                0, neutronCalibrationTubeID/2, neutronCalibrationTubeScintillatorHeight/2, 0, 360.*deg );
                G4Tubs *scintillatorNeutronTube_acrylic = new G4Tubs("scintillatorNeutronTube_acrylic",
                                innerAcrylicRadius, outerAcrylicRadius + 100. * cm, outerAcrylicHeight/2, 0., 360.*deg );
                G4Tubs *scintillatorNeutronTube_titanium = new G4Tubs("scintillatorNeutronTube_titanium",
                                0, outerLargeTiVesselRadius, outerLargeTiVesselHeight/2, 0., 360.*deg );
                G4SubtractionSolid *scintillatorNeutronTube_intermed = new G4SubtractionSolid("scintillatorNeutronTube_intermed",
                                scintillatorNeutronTube_cyl, scintillatorNeutronTube_acrylic, rotY90, G4ThreeVector(0., -1 * yOffset, xOffset));
                G4SubtractionSolid *scintillatorNeutronTube_solid = new G4SubtractionSolid("scintillatorNeutronTube_solid",
                                scintillatorNeutronTube_intermed, scintillatorNeutronTube_titanium, rotY90, G4ThreeVector(0., -1.*yOffset, xOffset));
		G4LogicalVolume *scintillatorNeutronTube_log = new G4LogicalVolume(
				scintillatorNeutronTube_solid, luxMaterials->Air(),
				"scintillatorNeutronTube_log" );
		scintillatorNeutronTube_log->SetVisAttributes(luxMaterials->AluminumVis() );
		LUXSimDetectorComponent *scintillatorNeutronTube;
		scintillatorNeutronTube = new LUXSimDetectorComponent( rotY90,
				G4ThreeVector(xOffset,yOffset,zOffset), "ScintillatorNeutronTube",
				scintillatorNeutronTube_log, scintillator, 0, 0, false );
	} else if( luxManager->GetLZSimpleNeutronCalTopTube() ) {
    
        G4cout << "Creating the vertical scintillator tube" << G4endl;
        G4Tubs *scintillatorNeutronTubeVertical_solid = new G4Tubs(
                "scintillatorNeutronTubeVertical_solid", 0,
                neutronCalibrationTubeID/2,
                neutronCalibrationTubeScintilltorHeightVertical/2, 0, 360.*deg);
        G4LogicalVolume *scintillatorNeutronTubeVertical_log =
                new G4LogicalVolume( scintillatorNeutronTubeVertical_solid,
                luxMaterials->Air(), "scintillatorNeutronTubeVertical_log" );
        scintillatorNeutronTubeVertical_log->SetVisAttributes(
                luxMaterials->AluminumVis() );
        xOffset = -ptfeWallInnerRadius/2;
        zOffset = innerAcrylicHeight/2 -
                neutronCalibrationTubeScintilltorHeightVertical/2 - 0.5*cm;
        LUXSimDetectorComponent *scintillatorNeutronTubeVertical;
        scintillatorNeutronTubeVertical = new LUXSimDetectorComponent(
                0, G4ThreeVector(xOffset,0,zOffset),
                "ScintillatorNeutronTubeVertical",
                scintillatorNeutronTubeVertical_log, scintillator, 0, 0, false);
    }
	
    //  The outer titanium vessel
    G4Tubs *outerTitanium_solid = new G4Tubs( "outerTitanium_solid", 0,
            outerLargeTiVesselRadius, outerLargeTiVesselHeight/2, 0, 360.*deg );
    G4LogicalVolume *outerTitanium_log = new G4LogicalVolume(
            outerTitanium_solid, luxMaterials->Titanium(), "outerTitanium_log");
    outerTitanium_log->SetVisAttributes( luxMaterials->TitaniumVis() );
    LUXSimDetectorComponent *outerTitanium = new LUXSimDetectorComponent(
            0, G4ThreeVector(), "OuterTitanium", outerTitanium_log,
            scintillator, 0, 0, false );
    
    //  The cryostat vacuum
    G4Tubs *cryostatVacuum_solid = new G4Tubs( "cryostatVacuum_solid", 0,
            innerLargeTiVesselRadius, innerLargeTiVesselHeight/2, 0, 360.*deg );
    G4LogicalVolume *cryostatVacuum_log = new G4LogicalVolume(
            cryostatVacuum_solid, luxMaterials->Vacuum(), "cryostatVacuum_log");
    cryostatVacuum_log->SetVisAttributes( luxMaterials->VacuumVis() );
    LUXSimDetectorComponent *cryostatVacuum = new LUXSimDetectorComponent(
            0, G4ThreeVector(), "CryostatVacuum", cryostatVacuum_log,
            outerTitanium, 0, 0, false );

    // The code below adds a tungsten pig which will hold the YBe source
    if(luxManager->GetLZYBePig()) {
    	G4double yBePigInnerRadius = 1. * cm;
    	G4double yBePigInnerHeight = 2. * cm;
    	G4double yBePigOuterRadius = luxManager->GetLZYBePigDiameter() / 2.;
    	G4double yBePigOuterHeight = luxManager->GetLZYBePigHeight();
    	G4double yBeXOffset = 0. * cm;
    	G4double yBeZOffset = 133. * cm;
    	G4Tubs *yBePig_outer = new G4Tubs( "yBePig_outer", 0,
    	        yBePigOuterRadius, yBePigOuterHeight/2, 0, 360.*deg );
    	G4Tubs *yBePig_inner = new G4Tubs( "yBePig_inner", 0,
    	        yBePigInnerRadius, yBePigInnerHeight/2, 0, 360.*deg );
    	G4VSolid *yBePig_solid = new G4SubtractionSolid("yBePig_solid", yBePig_outer, yBePig_inner, 0, G4ThreeVector());
    	G4LogicalVolume *yBePig_log = new G4LogicalVolume(
    	        yBePig_solid, luxMaterials->Tungsten(), "yBePig_log");
    	yBePig_log->SetVisAttributes( luxMaterials->TungstenVis() );
    	LUXSimDetectorComponent *yBePig;
    	yBePig = new LUXSimDetectorComponent(0, G4ThreeVector(yBeXOffset, 0., yBeZOffset), "YBePig", yBePig_log, cryostatVacuum, 0, 0, false );

    	G4Tubs *yBeSource_solid = new G4Tubs( "yBeSource_solid", 0,
    	        yBePigInnerRadius, yBePigInnerHeight/2, 0, 360.*deg );
    	G4LogicalVolume *yBeSource_log = new G4LogicalVolume(
    	        yBeSource_solid, luxMaterials->Air(), "yBeSource_log");
    	yBeSource_log->SetVisAttributes( luxMaterials->AirVis() );
    	LUXSimDetectorComponent *yBeSource;
    	yBeSource = new LUXSimDetectorComponent(0, G4ThreeVector(yBeXOffset, 0., yBeZOffset), "YBeSource", yBeSource_log, cryostatVacuum, 0, 0, false );
    }

	//	The source tubes and sources. The sources are very simple, 1mm x 1mm
	//	right cylinders, just to have a volume to hold a radioactive load.
	//	Again, this is the simple geometry.
	G4Tubs *sourceGuideTube_solid = new G4Tubs( "sourceGuideTube_solid",
			sourceTubeID/2, sourceTubeOD/2, sourceTubeHeight/2, 0, 360.*deg );
	G4LogicalVolume *sourceGuideTube_log[4];
	LUXSimDetectorComponent *sourceGuideTube[4];

	G4Tubs *sourceVolume_solid = new G4Tubs( "sourceVolume_solid", 0, 0.5*mm,
			0.5*mm, 0, 360.*deg );
	G4LogicalVolume *sourceVolume_log[4];
	LUXSimDetectorComponent *sourceVolume[4];
	
	stringstream name;
	for( G4int i=0; i<4; i++ ) {
		name.str("");
		name << "sourceGuideTube_log" << i+1;
		sourceGuideTube_log[i] = new G4LogicalVolume( sourceGuideTube_solid,
				luxMaterials->Titanium(), name.str().c_str() );
		sourceGuideTube_log[i]->SetVisAttributes( luxMaterials->TitaniumVis() );
		G4double angle = PI*i/2.;
		G4double radialPlacement = outerSmallTiVesselRadius + sourceTubeOD/2
				+ 0.1*mm;	//	The extra 0.1 mm is just to guarantee no overlap
		xOffset = radialPlacement * cos( angle );
		yOffset = radialPlacement * sin( angle );
		zOffset = -outerLargeTiVesselHeight/2 +
				outerSmallTiVesselHeight/2 + smallTiVesselHeightAboveBottom;
		name.str("");
		name << "SourceGuideTube_" << i+1;
		sourceGuideTube[i] = new LUXSimDetectorComponent( 0,
				G4ThreeVector( xOffset, yOffset, zOffset ), name.str().c_str(),
				sourceGuideTube_log[i], cryostatVacuum, 0, 0, false );
				
		name.str("");
		name << "sourceVolume_log" << i+1;
		sourceVolume_log[i] = new G4LogicalVolume( sourceVolume_solid,
				luxMaterials->Aluminum(), name.str().c_str() );
		G4VisAttributes *tempSourceAttributes = new G4VisAttributes();
		tempSourceAttributes->SetColor( G4Color::Red() );
		sourceVolume_log[i]->SetVisAttributes( tempSourceAttributes );
		zOffset = zOffset - outerSmallTiVesselHeight/2 +
				smallTiVesselThickness + ptfeLinerThickness +
				pmtHeightAboveBottom + pmtHeight + pmtWindowThickness +
				
luxManager->GetLZSimpleCalSourceHeight() -
				8.9375*mm;	//	I apologize for having this hard-coded offset
							//	when I should be able to calculate it from
							//	the available variables. But whatever. It works.
		name.str("");
		name << "SourceVolume_" << i+1;
		sourceVolume[i] = new LUXSimDetectorComponent( 0,
				G4ThreeVector( xOffset, yOffset, zOffset ), name.str().c_str(),
				sourceVolume_log[i], cryostatVacuum, 0, 0, false );
		
	}
	
    //  The inner titanium vessel
    G4Tubs *innerTitanium_solid = new G4Tubs( "innerTitanium_solid", 0,
            outerSmallTiVesselRadius, outerSmallTiVesselHeight/2, 0, 360.*deg );
    G4LogicalVolume *innerTitanium_log = new G4LogicalVolume(
            innerTitanium_solid, luxMaterials->Titanium(), "innerTitanium_log");
    innerTitanium_log->SetVisAttributes( luxMaterials->TitaniumVis() );
	zOffset = -outerLargeTiVesselHeight/2 +
            innerSmallTiVesselHeight/2 + smallTiVesselHeightAboveBottom;
    LUXSimDetectorComponent *innerTitanium = new LUXSimDetectorComponent(
            0, G4ThreeVector(0,0,zOffset), "InnerTitanium", innerTitanium_log,
            cryostatVacuum, 0, 0, false );
    
    //  PTFE liner. This is the thin layer of PTFE used to increase reflectivity
    //  in the xenon skin volume.
    G4Tubs *ptfeLiner_solid = new G4Tubs( "ptfeLiner_solid", 0,
            innerSmallTiVesselRadius, outerPTFELinerHeight/2, 0, 360.*deg );
    G4LogicalVolume *ptfeLiner_log = new G4LogicalVolume(
            ptfeLiner_solid, luxMaterials->Teflon(), "ptfeLiner_log" );
    ptfeLiner_log->SetVisAttributes( luxMaterials->TeflonVis() );
    LUXSimDetectorComponent *ptfeLiner = new LUXSimDetectorComponent(
            0, G4ThreeVector(), "PTFELiner", ptfeLiner_log, innerTitanium,
            0, 0, false );
    
    //  The xenon volume. This volume is the liquid "side skin", and everything
    //  else (gas xenon, PTFE walls in the liquid, bottom skin xenon) are all
    //  daughter volumes.
    G4Tubs *sideLiquidSkinXenon_solid = new G4Tubs( "sideLiquidSkinXenon_solid",
            0, innerPTFELinerRadius, innerPTFELinerHeight/2, 0,
            360.*deg );
    G4LogicalVolume *sideLiquidSkinXenon_log = new G4LogicalVolume(
            sideLiquidSkinXenon_solid, luxMaterials->LiquidXe(),
            "sideLiquidSkinXenon_log");
    sideLiquidSkinXenon_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
    LUXSimDetectorComponent *sideLiquidSkinXenon = new LUXSimDetectorComponent(
            0, G4ThreeVector(), "SideLiquidSkinXenon", sideLiquidSkinXenon_log,
            ptfeLiner, 0, 0, false );
    
    //  Bottom PMT window
    G4Tubs *bottomPMTWindow_solid = new G4Tubs( "bottomPMTWindow_solid",
            0, ptfeWallInnerRadius, pmtWindowThickness/2, 0, 360.*deg );
    G4LogicalVolume *bottomPMTWindow_log = new G4LogicalVolume(
            bottomPMTWindow_solid, luxMaterials->Quartz(),
            "bottomPMTWindow_log" );
    bottomPMTWindow_log->SetVisAttributes( luxMaterials->QuartzVis() );
    zOffset = -innerPTFELinerHeight/2 - pmtWindowThickness/2 +
            pmtHeightAboveBottom + pmtHeight;
    LUXSimDetectorComponent *bottomPMTWindow;
    bottomPMTWindow = new LUXSimDetectorComponent(0, G4ThreeVector(0,0,zOffset),
            "BottomPMTWindow", bottomPMTWindow_log, sideLiquidSkinXenon,
            0, 0, false );
    
    //  Bottom PMT
    G4Tubs *bottomPMT_solid = new G4Tubs( "bottomPMT_solid", 0,
            ptfeWallInnerRadius, pmtHeight/2, 0, 360.*deg );
    G4LogicalVolume *bottomPMT_log = new G4LogicalVolume( bottomPMT_solid,
            luxMaterials->Aluminum(), "bottomPMT_log" );
    bottomPMT_log->SetVisAttributes( luxMaterials->AluminumVis() );
    zOffset = -innerPTFELinerHeight/2 + pmtHeightAboveBottom + pmtHeight/2 -
            pmtWindowThickness;
    LUXSimDetectorComponent *bottomPMT;
    bottomPMT = new LUXSimDetectorComponent( 0, G4ThreeVector(0,0,zOffset),
            "BottomPMT", bottomPMT_log, sideLiquidSkinXenon, 0, 0, false );
    
    //  Bottom liquid xenon skin
    G4Tubs *bottomLiquidSkinXenon_solid = new G4Tubs(
            "bottomLiquidSkinXenon_solid", 0, innerPTFELinerRadius,
            bottomSkinHeight/2, 0, 360.*deg );
    G4LogicalVolume *bottomLiquidSkinXenon_log = new G4LogicalVolume(
            bottomLiquidSkinXenon_solid, luxMaterials->LiquidXe(),
            "bottomLiquidSkinXenon_log" );
    bottomLiquidSkinXenon_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
    zOffset = -innerPTFELinerHeight/2 + bottomSkinHeight/2;
    LUXSimDetectorComponent *bottomLiquidSkinXenon;
    bottomLiquidSkinXenon = new LUXSimDetectorComponent(
            0, G4ThreeVector(0,0,zOffset), "BottomLiquidSkinXenon",
            bottomLiquidSkinXenon_log, sideLiquidSkinXenon, 0, 0, false );
    
    //  PTFE wall inside the liquid side skin. This volume will house the
    //  active liquid xenon and the gamma-X volume. Note that there is also a
    //  separate volume of the PTFE wall inside the gas volume.
    G4Tubs *ptfeLiquidWall_solid = new G4Tubs( "ptfeLiquidWall_solid",
            0, ptfeWallOuterRadius, ptfeWallHeightLiquid/2, 0, 360.*deg );
    G4LogicalVolume *ptfeLiquidWall_log = new G4LogicalVolume(
            ptfeLiquidWall_solid, luxMaterials->Teflon(),
            "ptfeLiquidWall_log" );
    ptfeLiquidWall_log->SetVisAttributes( luxMaterials->TeflonVis() );
    zOffset = innerPTFELinerHeight/2 - ptfeWallHeightLiquid/2 - totalGasHeight;
    LUXSimDetectorComponent *ptfeLiquidWall = new LUXSimDetectorComponent(
            0, G4ThreeVector(0,0,zOffset), "PTFELiquidWall",
            ptfeLiquidWall_log, sideLiquidSkinXenon, 0, 0, false );
    
    //  Central liquid xenon volume. This is placed inside the PTFE liquid wall.
    G4Tubs *gammaXLiquidXenon_solid = new G4Tubs( "gammaXLiquidXenon_solid",
            0, ptfeWallInnerRadius, ptfeWallHeightLiquid/2, 0, 360.*deg );
    G4LogicalVolume *gammaXLiquidXenon_log = new G4LogicalVolume(
            gammaXLiquidXenon_solid, luxMaterials->LiquidXe(),
            "gammaXLiquidXenon_log" );
    gammaXLiquidXenon_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
    LUXSimDetectorComponent *gammaXLiquidXenon = new LUXSimDetectorComponent(
            0, G4ThreeVector(), "LiquidGammaXXenon", gammaXLiquidXenon_log,
            ptfeLiquidWall, 0, 0, false );
    
    //  Active liquid xenon volume. This is the beast that should weight in at
    //  seven tonnes.
    G4Tubs *liquidXenon_solid = new G4Tubs( "liquidXenon_solid",
            0, ptfeWallInnerRadius, activeLiquidXenonHeight/2, 0, 360.*deg );
    G4LogicalVolume *liquidXenon_log = new G4LogicalVolume(
            liquidXenon_solid, luxMaterials->LiquidXe(), "liquidXenon_log" );
    liquidXenon_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
    zOffset = ptfeWallHeightLiquid/2 - activeLiquidXenonHeight/2;
    LUXSimDetectorComponent *liquidXenon;
    liquidXenon = new LUXSimDetectorComponent( 0, G4ThreeVector(0,0,zOffset),
            "ActiveLiquidXenon", liquidXenon_log, gammaXLiquidXenon,
            0, 0, false );
    
    //  The gaseous volume. In parallel with the liquid xenon, the highest-level
    //  gaseous xenon is the side skin, and other volumes (top skin, top PMT,
    //  S2 volume, and the part of the PTFE wall in the gas) are daughter
    //  volumes
    G4Tubs *sideGasSkinXenon_solid = new G4Tubs( "sideGasSkinXenon_solid",
            0, innerPTFELinerRadius, totalGasHeight/2, 0, 360.*deg );
    G4LogicalVolume *sideGasSkinXenon_log = new G4LogicalVolume(
            sideGasSkinXenon_solid, luxMaterials->GasXe(),
            "sideGasSkinXenon_log" );
    sideGasSkinXenon_log->SetVisAttributes( luxMaterials->GasXeVis() );
    zOffset = innerPTFELinerHeight/2 - totalGasHeight/2;
    LUXSimDetectorComponent *sideGasSkinXenon = new LUXSimDetectorComponent(
            0, G4ThreeVector(0,0,zOffset), "SideGasSkinXenon",
            sideGasSkinXenon_log, sideLiquidSkinXenon, 0, 0, false );
    
    //  PTFE Walls inside the gas side skin. This volume will house the S2
    //  volume.
    G4Tubs *ptfeGasWall_solid = new G4Tubs( "ptfeGasWall_solid",
            0, ptfeWallOuterRadius, s2VolumeHeight/2, 0, 360.*deg );
    G4LogicalVolume *ptfeGasWall_log = new G4LogicalVolume(
            ptfeGasWall_solid, luxMaterials->Teflon(), "ptfeGasWall_log" );
    ptfeGasWall_log->SetVisAttributes( luxMaterials->TeflonVis() );
    zOffset = -totalGasHeight/2 + s2VolumeHeight/2;
    LUXSimDetectorComponent *ptfeGasWall = new LUXSimDetectorComponent(
            0, G4ThreeVector(0,0,zOffset), "PTFEGasWall", ptfeGasWall_log,
            sideGasSkinXenon, 0, 0, false );
    
    //  S2 volume. This sits inside the PTFE gas wall. The PMT window, PMT,
    //  and top gas skin volume are placed directly in the side gas volume.
    G4Tubs *s2Volume_solid = new G4Tubs( "s2Volume_solid",
            0, ptfeWallInnerRadius, s2VolumeHeight/2, 0, 360.*deg );
    G4LogicalVolume *s2Volume_log = new G4LogicalVolume(
            s2Volume_solid, luxMaterials->GasXe(), "s2Volume_log" );
    s2Volume_log->SetVisAttributes( luxMaterials->GasXeVis() );
    LUXSimDetectorComponent *s2Volume;
    s2Volume = new LUXSimDetectorComponent( 0, G4ThreeVector(), "S2Volume",
            s2Volume_log, ptfeGasWall, 0, 0, false );
    
    //  Top PMT window.
    G4Tubs *topPMTWindow_solid = new G4Tubs( "topPMTWindow_solid",
            0, ptfeWallInnerRadius, pmtWindowThickness/2, 0, 360.*deg );
    G4LogicalVolume *topPMTWindow_log = new G4LogicalVolume(
            topPMTWindow_solid, luxMaterials->Quartz(),
            "topPMTWindow_log" );
    topPMTWindow_log->SetVisAttributes( luxMaterials->QuartzVis() );
    zOffset = -totalGasHeight/2 + pmtWindowThickness/2 + s2VolumeHeight;
    LUXSimDetectorComponent *topPMTWindow;
    topPMTWindow = new LUXSimDetectorComponent( 0, G4ThreeVector(0,0,zOffset),
            "TopPMTWindow", topPMTWindow_log, sideGasSkinXenon, 0, 0, false );
    
    //  Top PMT
    G4Tubs *topPMT_solid = new G4Tubs( "topPMT_solid", 0,
            ptfeWallInnerRadius, pmtHeight/2, 0, 360.*deg );
    G4LogicalVolume *topPMT_log = new G4LogicalVolume( topPMT_solid,
            luxMaterials->Aluminum(), "topPMT_log" );
    topPMT_log->SetVisAttributes( luxMaterials->AluminumVis() );
    zOffset = -totalGasHeight/2 + pmtHeight/2 + s2VolumeHeight +
            pmtWindowThickness;
    LUXSimDetectorComponent *topPMT;
    topPMT = new LUXSimDetectorComponent( 0, G4ThreeVector(0,0,zOffset),
            "TopPMT", topPMT_log, sideGasSkinXenon, 0, 0, false );
    
    //  Top gas xenon skin
    G4Tubs *topGasSkinXenon_solid = new G4Tubs(
            "topGasSkinXenon_solid", 0, innerPTFELinerRadius,
            topSkinHeight/2, 0, 360.*deg );
    G4LogicalVolume *topGasSkinXenon_log = new G4LogicalVolume(
            topGasSkinXenon_solid, luxMaterials->LiquidXe(),
            "topGasSkinXenon_log" );
    topGasSkinXenon_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
    zOffset = totalGasHeight/2 - topSkinHeight/2;
    LUXSimDetectorComponent *topGasSkinXenon;
    topGasSkinXenon = new LUXSimDetectorComponent(0, G4ThreeVector(0,0,zOffset),
            "TopGasSkinXenon", topGasSkinXenon_log, sideGasSkinXenon,
            0, 0, false );
    
    //  With the detector fully specified, print out some variables of interest
    G4cout << "Active liquid xenon radius: " << ptfeWallInnerRadius/cm
           << " cm" << G4endl;
    G4cout << "Active liquid xenon height: " << activeLiquidXenonHeight/cm
           << " cm" << G4endl;
    G4cout << "Active liquid xenon mass: "
           << liquidXenon_log->GetMass(true,false)/kg << " kg" << G4endl;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                              ~LUXSimLZSimple
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZSimple::~LUXSimLZSimple() {}
