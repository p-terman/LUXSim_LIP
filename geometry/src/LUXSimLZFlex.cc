////////////////////////////////////////////////////////////////////////////////
//	
//	LUXSimLZFlex.cc
//
//	This is the code file to define an LZ-like detector that sits inside the
//	current LUX water tank. It has a total liquid mass set by a messenger
//	command, but with a default mass of 10 tonnes. The liquid xenon volume is a
//	right cylinder with diameter = height. All encompassing volume IDs and ODs
//	are set according to the requested volume mass, although the wall
//	thicknesses are always the same.
//
////////////////////////////////////////////////////////////////////////////////
//
//	Change log
//
//	24 Feb 2012 - Initial submission. As of this submission, there are optical
//				  boundary layers between the active xenon target and the
//				  bottom PMT window and Teflon walls. There's also an optical
//				  boundary layer between the active gaseous xenon and the top
//				  PMT window and the Teflon walls. This means light propagation
//				  studies are only useful in the active region. Note, however,
//				  that the "PMTs" are just a large, single disk of quartz that
//				  covers the entire active volume, rather than individual PMTs.
//				  While there is a scintillator and a xenon skin, studies
//				  studies involving those volumes are best limited to energy
//				  deposition studies, until and unless someone creates volumes
//				  sensitive to optical photons that border those volumes. Also,
//				  as of this date, the scintillator material, isohexane, only
//				  has an index of refraction defined, and not any scintillation
//				  properties whatsoever. (Kareem)
//	26 Feb 2012 - Added an outer vacuum cryostat and a vacuum space. Added
//				  optical boundaries between the outer Teflon liner and the
//				  liquid / gaseous xenon skin volumes. Added top and bottom
//				  "PMTs" to the xenon skin volumes. (Kareem)
//	28 Feb 2012 - The outer and middle steel cryostats are now 4 cm. The inner
//				  titanium cryostat is 1 cm unless the liquid xenon target
//				  volume is less than 5 tonnes, in which case the titanium
//				  thickness is 7 mm. (Kareem)
//	01 Mar 2012 - Changed the names of the skin PMTs so that they're not
//				  conflated with the inner-volume PMT volumes when setting
//				  record levels and source loads. (Kareem)
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

//
//      LUXSim includes
//
#include "LUXSimLZFlex.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

//
//		Definitions
//
#define PI 3.14159265358979312
#define PRINTDIMENSIONS 0

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                              LUXSimLZFlex
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZFlex::LUXSimLZFlex(
		G4double liquidXenonActiveMass,
		G4double xenonSkinThickness,
		G4double teflonWallThickness,
		G4double scintillatorThickness )
{
	luxMaterials = LUXSimMaterials::GetMaterials();
	
	//	Set the dimensions of the active xenon target volume based on the mass
	//	of the liquid xenon. Note that the liquid xenon volume is assumed to be
	//	a right cylinder with 1-to-1 aspect ratio.
	G4double liquidXenonDensity = luxMaterials->LiquidXe()->GetDensity();
	G4double liquidXenonActiveVolume = liquidXenonActiveMass/liquidXenonDensity;
	G4double liquidXenonActiveHeight =
			pow( liquidXenonActiveVolume/PI*4, 1./3 );
	G4double liquidXenonActiveRadius = liquidXenonActiveHeight/2;
	
	if( PRINTDIMENSIONS ) {
		G4cout << G4endl << G4endl << G4endl;
		G4cout << "For a " << liquidXenonActiveMass/kg
			   << "-kg active liquid xenon " << "target, height = diameter = "
			   << liquidXenonActiveHeight/cm << " cm" << G4endl;
	}
	
	//	Set the static dimensions - radial parameters
	G4double teflonLinerThickness = 1.*mm;
	G4double titaniumCryostatWallThickness = 1.*cm;
	if( liquidXenonActiveMass < 5000.*kg )
		titaniumCryostatWallThickness = 7.*mm;
	G4double steelCryostatWallThickness = 4.*cm;
	G4double vacuumThickness = 5.*cm;
	
	//	Set the static dimensions - height parameters
	G4double gasXenonActiveHeight = 1*cm;
	G4double pmtHolderHeight = 10.*cm;
	G4double spaceAboveAndBelowPMTHolders = 5.*cm;
	G4double pmtWindowThickness = 2.*mm;
	
	//	Now build the detector. Start with the outer vacuum cryostat, which
	//	will be this detector's logicalVolume, and work in from there.
	G4double outerCryostatHeight = liquidXenonActiveHeight +
			gasXenonActiveHeight + 2*pmtWindowThickness + 2*pmtHolderHeight +
			2*spaceAboveAndBelowPMTHolders + 2*titaniumCryostatWallThickness +
			2*scintillatorThickness + 2*steelCryostatWallThickness +
			2*vacuumThickness + 2*steelCryostatWallThickness;
	G4double outerCryostatOuterRadius = liquidXenonActiveRadius +
			teflonWallThickness + xenonSkinThickness + teflonLinerThickness +
			titaniumCryostatWallThickness + scintillatorThickness +
			steelCryostatWallThickness + vacuumThickness +
			steelCryostatWallThickness;
	if( PRINTDIMENSIONS )
		G4cout << "Outer cryostat: " << outerCryostatHeight/cm << " cm high by "
			   << outerCryostatOuterRadius/cm*2 << " cm diameter" << G4endl;
	
	G4Tubs *outerCryostat_solid = new G4Tubs( "outerCryostat_solid", 0,
			outerCryostatOuterRadius, outerCryostatHeight/2, 0, 360.*deg );
	logicalVolume = new G4LogicalVolume( outerCryostat_solid,
			luxMaterials->Steel(), "outerCryostat_log" );
	logicalVolume->SetVisAttributes( luxMaterials->SteelVis() );
	
	//	Next, the vacuum space
	G4double vacuumSpaceHeight = outerCryostatHeight - 2*steelCryostatWallThickness;
	G4double vacuumSpaceOuterRadius = outerCryostatOuterRadius -
			steelCryostatWallThickness;
	if( PRINTDIMENSIONS )
		G4cout << "Vacuum space: " << vacuumSpaceHeight/cm << " cm high by "
			   << vacuumSpaceOuterRadius/cm*2 << " cm diameter" << G4endl;
	
	G4Tubs *vacuumSpace_solid = new G4Tubs( "vacuumSpace_solid", 0,
			vacuumSpaceOuterRadius, vacuumSpaceHeight/2, 0, 360.*deg );
	G4LogicalVolume *vacuumSpace_log = new G4LogicalVolume(
			vacuumSpace_solid, luxMaterials->Vacuum(), "vacuumSpace_log");
	vacuumSpace_log->SetVisAttributes( luxMaterials->VacuumVis() );
	LUXSimDetectorComponent *vacuumSpace = new LUXSimDetectorComponent(
			0, G4ThreeVector(), vacuumSpace_log, "VacuumSpace",
			logicalVolume, 0, 0, false );
		
	//	Next, the middle cryostat
	G4double middleCryostatHeight = vacuumSpaceHeight - 2*vacuumThickness;
	G4double middleCryostatOuterRadius = vacuumSpaceOuterRadius -
			vacuumThickness;
	if( PRINTDIMENSIONS )
		G4cout << "Middle cryostat: " << middleCryostatHeight/cm
			   << " cm high by " << middleCryostatOuterRadius/cm*2
			   << " cm diameter" << G4endl;
	
	G4Tubs *middleCryostat_solid = new G4Tubs( "middleCryostat_solid", 0,
			middleCryostatOuterRadius, middleCryostatHeight/2, 0, 360.*deg );
	G4LogicalVolume *middleCryostat_log = new G4LogicalVolume(
			middleCryostat_solid, luxMaterials->Steel(), "middleCryostat_log");
	middleCryostat_log->SetVisAttributes( luxMaterials->SteelVis() );
	LUXSimDetectorComponent *middleCryostat = new LUXSimDetectorComponent(
			0, G4ThreeVector(), "MiddleCryostat", middleCryostat_log,
			vacuumSpace, 0, 0, false );

	//	Next, the scintillator veto
	G4double scintillatorHeight = middleCryostatHeight -
			2*steelCryostatWallThickness;
	G4double scintillatorOuterRadius = middleCryostatOuterRadius -
			steelCryostatWallThickness;
	if( PRINTDIMENSIONS )
		G4cout << "Scintillator: " << scintillatorHeight/cm << " cm high by "
			   << scintillatorOuterRadius/cm*2 << " cm diameter" << G4endl;
	
	G4Tubs *scintillator_solid = new G4Tubs( "scintillator_solid", 0,
			scintillatorOuterRadius, scintillatorHeight/2, 0, 360.*deg );
	G4LogicalVolume *scintillator_log = new G4LogicalVolume(
			scintillator_solid, luxMaterials->Isohexane(), "scintillator_log");
	scintillator_log->SetVisAttributes( luxMaterials->IsohexaneVis() );
	LUXSimDetectorComponent *scintillator = new LUXSimDetectorComponent(
			0, G4ThreeVector(), "Scintillator", scintillator_log,
			middleCryostat, 0, 0, false );
	
	//	Next, the inner cryostat
	G4double innerCryostatHeight = scintillatorHeight - 2*scintillatorThickness;
	G4double innerCryostatOuterRadius = scintillatorOuterRadius -
			scintillatorThickness;
	if( PRINTDIMENSIONS )
		G4cout << "Inner cryostat: " << innerCryostatHeight/cm << " cm high by "
			   << innerCryostatOuterRadius/cm*2 << " cm diameter" << G4endl;

	G4Tubs *innerCryostat_solid = new G4Tubs( "innerCryostat_solid", 0,
			innerCryostatOuterRadius, innerCryostatHeight/2, 0, 360.*deg );
	G4LogicalVolume *innerCryostat_log = new G4LogicalVolume(
			innerCryostat_solid, luxMaterials->Titanium(),
			"innerCryostat_log" );
	innerCryostat_log->SetVisAttributes( luxMaterials->TitaniumVis() );
	LUXSimDetectorComponent *innerCryostat = new LUXSimDetectorComponent(
			0, G4ThreeVector(), "InnerCryostat", innerCryostat_log,
			scintillator, 0, 0, false );
	
	//	Next, the outer teflon liner for the xenon skin
	G4double xenonSkinHeight = innerCryostatHeight - 2*titaniumCryostatWallThickness;
	G4double teflonLinerOuterRadius = innerCryostatOuterRadius -
			titaniumCryostatWallThickness;
	if( PRINTDIMENSIONS )
		G4cout << "Outer Teflon liner: " << xenonSkinHeight/cm << " cm high by "
			   << teflonLinerOuterRadius/cm*2 << " cm diameter" << G4endl;
	
	G4Tubs *outerTeflonLiner_solid = new G4Tubs( "outerTeflonLiner_solid", 0,
			teflonLinerOuterRadius, xenonSkinHeight/2, 0, 360.*deg );
	G4LogicalVolume *outerTeflonLiner_log = new G4LogicalVolume(
			outerTeflonLiner_solid, luxMaterials->Teflon(),
			"outerTeflonLiner_log" );
	outerTeflonLiner_log->SetVisAttributes( luxMaterials->TeflonVis() );
	LUXSimDetectorComponent *outerTeflonLiner = new LUXSimDetectorComponent(
			0, G4ThreeVector(), "OuterTeflonLiner", outerTeflonLiner_log,
			innerCryostat, 0, 0, false );

	//	Next, the liquid xenon skin. Note that the active liquid xenon volume
	//	is going to be a sub-volume, so at this radius, calling it the xenon
	//	skin is good enough.
	G4double xenonSkinOuterRadius = teflonLinerOuterRadius -
			teflonLinerThickness;
	if( PRINTDIMENSIONS )
		G4cout << "Xenon skin: " << xenonSkinHeight/cm << " cm high by "
			   << xenonSkinOuterRadius/cm*2 << " cm diameter" << G4endl;

	G4Tubs *xeSkin_solid = new G4Tubs( "xeSkin_solid", 0,
			xenonSkinOuterRadius, xenonSkinHeight/2, 0, 360.*deg );
	G4LogicalVolume *xeSkin_log = new G4LogicalVolume( xeSkin_solid, 
			luxMaterials->LiquidXe(), "xeSkin_log" );
	xeSkin_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
	LUXSimDetectorComponent *xeSkin = new LUXSimDetectorComponent(
			0, G4ThreeVector(), "LiquidXeSkin", xeSkin_log,
			outerTeflonLiner, 0, 0, false );
	
	//	Next, the bottom PMT window for instrumenting the xenon skin volume.
	//	This is just an annulus of quartz to take the place of many discrete
	//	PMTs. You have been warned.
	G4double bottomSkinPMTWindowOffset = -xenonSkinHeight/2 +
			pmtWindowThickness/2;
	G4double xenonSkinInnerRadius = xenonSkinOuterRadius - xenonSkinThickness;
	if( PRINTDIMENSIONS ) {
		G4cout << "Bottom skin PMT window offset: "
			   << bottomSkinPMTWindowOffset/cm << " cm" << G4endl;
		G4cout << "                    Inner diameter = "
			   << xenonSkinInnerRadius/cm*2 << " cm " << G4endl;
		G4cout << "                    Outer diameter = "
			   << xenonSkinOuterRadius/cm*2 << " cm " << G4endl;
	}
	
	G4Tubs *bottomSkinPMTWindow_solid = new G4Tubs( "bottomSkinPMTWindow_solid",
			xenonSkinInnerRadius, xenonSkinOuterRadius, pmtWindowThickness/2, 0,
			360.*deg );
	G4LogicalVolume *bottomSkinPMTWindow_log = new G4LogicalVolume(
			bottomSkinPMTWindow_solid, luxMaterials->Quartz(),
			"bottomSkinPMTWindow_log" );
	bottomSkinPMTWindow_log->SetVisAttributes( luxMaterials->QuartzVis() );
	LUXSimDetectorComponent *bottomSkinPMTWindow;
	bottomSkinPMTWindow = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, bottomSkinPMTWindowOffset ),
			"BottomSkinPMTAnnularWindow", bottomSkinPMTWindow_log,
			xeSkin, 0, 0, false );
	
	//	Next, a "photocathode" inside the bottom skin PMT window. This is just a
	//	wholly embedded volume of quartz that can be used to trap the optical
	//	photons only after they've actually entered the PMT window volume (and
	//	have NOT been reflected).
	G4Tubs *bottomSkinPMTPhotocathode_solid = new G4Tubs(
			"bottomSkinPMTPhotocathode_solid", xenonSkinInnerRadius,
			xenonSkinOuterRadius, pmtWindowThickness/4, 0, 360.*deg );
	G4LogicalVolume *bottomSkinPMTPhotocathode_log = new G4LogicalVolume(
			bottomSkinPMTPhotocathode_solid, luxMaterials->Quartz(),
			"bottomSkinPMTPhotocathode_log" );
	bottomSkinPMTPhotocathode_log->SetVisAttributes( luxMaterials->QuartzVis());
	LUXSimDetectorComponent *bottomSkinPMTPhotocathode;
	bottomSkinPMTPhotocathode = new LUXSimDetectorComponent( 0, G4ThreeVector(),
			"BottomSkinPMTAnnularPhotocathode", bottomSkinPMTPhotocathode_log,
			bottomSkinPMTWindow, 0, 0, false );
	
	//	Next, a gaseous layer of xenon in the upper part of the liquid xenon
	//	skin. Note that this volume does NOT have an inner radius of 0, so that
	//	until more parts are placed in the upper part of the liquid xenon, there
	//	is still liquid xenon up where it shouldn't be.
	G4double gaseousSkinHeight = gasXenonActiveHeight + pmtWindowThickness +
			pmtHolderHeight + spaceAboveAndBelowPMTHolders;
	G4double gaseousSkinVerticalOffset = xenonSkinHeight/2 -
			gaseousSkinHeight/2;
	if( PRINTDIMENSIONS )
		G4cout << "Gaseous xenon skin: " << gaseousSkinHeight/cm
			   << " cm high with " << gaseousSkinVerticalOffset/cm
			   << "-cm vertical offset" << G4endl;
	
	G4Tubs *gaseousSkin_solid = new G4Tubs( "gaseousSkin_solid",
			xenonSkinInnerRadius, xenonSkinOuterRadius, gaseousSkinHeight/2,
			0, 360.*deg );
	G4LogicalVolume *gaseousSkin_log = new G4LogicalVolume( gaseousSkin_solid,
			luxMaterials->GasXe(), "gaseousSkin_log" );
	gaseousSkin_log->SetVisAttributes( luxMaterials->GasXeVis() );
	LUXSimDetectorComponent *gaseousSkin;
	gaseousSkin = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, gaseousSkinVerticalOffset ), "GaseousXeSkin",
			gaseousSkin_log, xeSkin, 0, 0, false );

	//	Next, the top PMT window for instrumenting the xenon skin volume.
	G4double topSkinPMTWindowOffset = gaseousSkinHeight/2 -
			pmtWindowThickness/2;
	if( PRINTDIMENSIONS )
		G4cout << "Top skin PMT window offset: " << topSkinPMTWindowOffset/cm
			   << " cm" << G4endl;
	
	G4Tubs *topSkinPMTWindow_solid = new G4Tubs( "topSkinPMTWindow_solid",
			xenonSkinInnerRadius, xenonSkinOuterRadius, pmtWindowThickness/2, 0,
			360.*deg );
	G4LogicalVolume *topSkinPMTWindow_log = new G4LogicalVolume(
			topSkinPMTWindow_solid, luxMaterials->Quartz(),
			"topSkinPMTWindow_log" );
	topSkinPMTWindow_log->SetVisAttributes( luxMaterials->QuartzVis() );
	LUXSimDetectorComponent *topSkinPMTWindow;
	topSkinPMTWindow = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, topSkinPMTWindowOffset ),
			"TopSkinPMTAnnularWindow", topSkinPMTWindow_log, gaseousSkin, 0, 0,
			false );
	
	//	Next, a "photocathode" inside the top skin PMT window.
	G4Tubs *topSkinPMTPhotocathode_solid = new G4Tubs(
			"topSkinPMTPhotocathode_solid", xenonSkinInnerRadius,
			xenonSkinOuterRadius, pmtWindowThickness/4, 0, 360.*deg );
	G4LogicalVolume *topSkinPMTPhotocathode_log = new G4LogicalVolume(
			topSkinPMTPhotocathode_solid, luxMaterials->Quartz(),
			"topSkinPMTPhotocathode_log" );
	topSkinPMTPhotocathode_log->SetVisAttributes( luxMaterials->QuartzVis());
	LUXSimDetectorComponent *topSkinPMTPhotocathode;
	topSkinPMTPhotocathode = new LUXSimDetectorComponent( 0, G4ThreeVector(),
			"TopSkinPMTAnnularPhotocathode", topSkinPMTPhotocathode_log,
			topSkinPMTWindow, 0, 0, false );
	
	//	Next, the teflon reflector. The inner wall of this reflector is in
	//	contact with the active liquid and gaseous xenon target volumes, while
	//	the outer wall is in contact with the xenon skin.
	G4double teflonReflectorOuterRadius = xenonSkinInnerRadius;
	if( PRINTDIMENSIONS )
		G4cout << "Teflon reflector: " << xenonSkinHeight/cm << " cm high by "
			   << teflonReflectorOuterRadius/cm*2 << " cm diameter" << G4endl;
	
	G4Tubs *teflonReflector_solid = new G4Tubs( "teflonReflector_solid", 0,
			teflonReflectorOuterRadius, xenonSkinHeight/2, 0, 360.*deg );
	G4LogicalVolume *teflonReflector_log = new G4LogicalVolume(
			teflonReflector_solid, luxMaterials->Teflon(),
			"teflonReflector_log" );
	teflonReflector_log->SetVisAttributes( luxMaterials->TeflonVis() );
	LUXSimDetectorComponent *teflonReflector = new LUXSimDetectorComponent(
			0, G4ThreeVector(), "TeflonReflector", teflonReflector_log,
			xeSkin, 0, 0, false );

	//	Next, the full inner liquid xenon volume. This volume encompasses not
	//	only the active xenon target, but the xenon below the PMT holder as
	//	well.
	G4double innerLiquidXenonVolumeHeight = liquidXenonActiveHeight +
			pmtHolderHeight + spaceAboveAndBelowPMTHolders + pmtWindowThickness;
	G4double innerLiquidXenonVolumeOffset = -xenonSkinHeight/2 +
			innerLiquidXenonVolumeHeight/2;
	if( PRINTDIMENSIONS )
		G4cout << "Inner liquid xenon volume: "
			   << innerLiquidXenonVolumeHeight/	cm << " cm high with "
			   << innerLiquidXenonVolumeOffset/cm << "-cm vertical offset"
			   << G4endl;
	
	G4Tubs *innerLiquidXenonVolume_solid = new G4Tubs(
			"innerLiquidXenonVolume_solid", 0, liquidXenonActiveRadius,
			innerLiquidXenonVolumeHeight/2, 0, 360.*deg );
	G4LogicalVolume *innerLiquidXenonVolume_log = new G4LogicalVolume(
			innerLiquidXenonVolume_solid, luxMaterials->LiquidXe(),
			"innerLiquidXenonVolume_log" );
	innerLiquidXenonVolume_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
	LUXSimDetectorComponent *innerLiquidXenonVolume =
			new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, innerLiquidXenonVolumeOffset ),
			"InnerLiquidXenonVolume", innerLiquidXenonVolume_log,
			teflonReflector, 0, 0, false );

	//	Next, the bottom PMT holder. 
	G4double bottomPMTHolderOffset = -innerLiquidXenonVolumeHeight/2 +
			pmtHolderHeight/2 + spaceAboveAndBelowPMTHolders;
	if( PRINTDIMENSIONS )
		G4cout << "Bottom PMT holder offset: " << bottomPMTHolderOffset/cm
			   << " cm" << G4endl;
	
	G4Tubs *bottomPMTHolder_solid = new G4Tubs( "bottomPMTHolder_solid", 0,
			liquidXenonActiveRadius, pmtHolderHeight/2, 0, 360.*deg );
	G4LogicalVolume *bottomPMTHolder_log = new G4LogicalVolume(
			bottomPMTHolder_solid, luxMaterials->Polyethylene(),
			"bottomPMTHolder_log" );
	bottomPMTHolder_log->SetVisAttributes( luxMaterials->PolyethyleneVis() );
	LUXSimDetectorComponent *bottomPMTHolder;
	bottomPMTHolder = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, bottomPMTHolderOffset ), "BottomPMTHolder",
			bottomPMTHolder_log, innerLiquidXenonVolume, 0, 0, false );

	//	Next, the bottom PMT window. This is just a large sheet of quartz to
	//	take the place of many discrete PMTs. You have been warned.
	G4double bottomPMTWindowOffset = bottomPMTHolderOffset + pmtHolderHeight/2 +
			pmtWindowThickness/2;
	if( PRINTDIMENSIONS )
		G4cout << "Bottom PMT window offset: " << bottomPMTWindowOffset/cm
			   << " cm" << G4endl;
	
	G4Tubs *bottomPMTWindow_solid = new G4Tubs( "bottomPMTWindow_solid", 0,
			liquidXenonActiveRadius, pmtWindowThickness/2, 0, 360.*deg );
	G4LogicalVolume *bottomPMTWindow_log = new G4LogicalVolume(
			bottomPMTWindow_solid, luxMaterials->Quartz(),
			"bottomPMTWindow_log" );
	bottomPMTWindow_log->SetVisAttributes( luxMaterials->QuartzVis() );
	LUXSimDetectorComponent *bottomPMTWindow;
	bottomPMTWindow = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, bottomPMTWindowOffset ), "BottomPMTWindow",
			bottomPMTWindow_log, innerLiquidXenonVolume, 0, 0, false );
	
	//	Next, a "photocathode" inside the PMT window.
	G4Tubs *bottomPMTPhotocathode_solid = new G4Tubs(
			"bottomPMTPhotocathode_solid", 0, liquidXenonActiveRadius,
			pmtWindowThickness/4, 0, 360.*deg );
	G4LogicalVolume *bottomPMTPhotocathode_log = new G4LogicalVolume(
			bottomPMTPhotocathode_solid, luxMaterials->Quartz(),
			"bottomPMTPhotocathode_log" );
	bottomPMTPhotocathode_log->SetVisAttributes( luxMaterials->QuartzVis() );
	LUXSimDetectorComponent *bottomPMTPhotocathode;
	bottomPMTPhotocathode = new LUXSimDetectorComponent( 0, G4ThreeVector(),
			"BottomPMTPhotocathode", bottomPMTPhotocathode_log,
			bottomPMTWindow, 0, 0, false );
			
	//	Next, the active liquid xenon target.
	G4double activeLiquidXenonOffset = bottomPMTWindowOffset +
			liquidXenonActiveHeight/2 + pmtWindowThickness/2;
	if( PRINTDIMENSIONS )
		G4cout << "Active liquid xenon target offset: "
			   << activeLiquidXenonOffset/cm << " cm" << G4endl;
	
	G4Tubs *activeXenonLiquidTarget_solid = new G4Tubs(
			"activeXenonLiquidTarget_solid", 0, liquidXenonActiveRadius,
			liquidXenonActiveHeight/2, 0, 360.*deg );
	G4LogicalVolume *activeXenonLiquidTarget_log = new G4LogicalVolume(
			activeXenonLiquidTarget_solid, luxMaterials->LiquidXe(),
			"activeXenonLiquidTarget_log" );
	activeXenonLiquidTarget_log->SetVisAttributes( luxMaterials->LiquidXeVis());
	LUXSimDetectorComponent *activeXenonLiquidTarget;
	activeXenonLiquidTarget = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, activeLiquidXenonOffset ), "LiquidXeTarget",
			activeXenonLiquidTarget_log, innerLiquidXenonVolume, 0, 0, false );
			
	//	Next, the full inner gaseous xenon volume. This volume encompasses not
	//	only the active xenon gas, but the gaseous xenon above the PMT holder as
	//	well.
	G4double innerGaseousXenonVolumeHeight = xenonSkinHeight -
			innerLiquidXenonVolumeHeight;
	G4double innerGaseousXenonVolumeOffset = xenonSkinHeight/2 -
			innerGaseousXenonVolumeHeight/2;
	if( PRINTDIMENSIONS )
		G4cout << "Inner gaseous xenon volume: "
			   << innerGaseousXenonVolumeHeight/cm << " cm high with "
			   << innerGaseousXenonVolumeOffset/cm << "-cm vertical offset"
			   << G4endl;
	
	G4Tubs *innerGaseousXenonVolume_solid = new G4Tubs(
			"innerGaseousXenonVolume_solid", 0, liquidXenonActiveRadius,
			innerGaseousXenonVolumeHeight/2, 0, 360.*deg );
	G4LogicalVolume *innerGaseousXenonVolume_log = new G4LogicalVolume(
			innerGaseousXenonVolume_solid, luxMaterials->GasXe(),
			"innerGaseousXenonVolume_log" );
	innerGaseousXenonVolume_log->SetVisAttributes( luxMaterials->GasXeVis());
	LUXSimDetectorComponent *innerGaseousXenonVolume =
			new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, innerGaseousXenonVolumeOffset ),
			"InnerGaseousXenonVolume", innerGaseousXenonVolume_log,
			teflonReflector, 0, 0, false );

	//	Next, the top PMT holder. 
	G4double topPMTHolderOffset = innerGaseousXenonVolumeHeight/2 -
			pmtHolderHeight/2 - spaceAboveAndBelowPMTHolders;
	if( PRINTDIMENSIONS )
		G4cout << "Top PMT holder offset: " << topPMTHolderOffset/cm << " cm"
			   << G4endl;
	
	G4Tubs *topPMTHolder_solid = new G4Tubs( "topPMTHolder_solid", 0,
			liquidXenonActiveRadius, pmtHolderHeight/2, 0, 360.*deg );
	G4LogicalVolume *topPMTHolder_log = new G4LogicalVolume(
			topPMTHolder_solid, luxMaterials->Polyethylene(),
			"topPMTHolder_log" );
	topPMTHolder_log->SetVisAttributes( luxMaterials->PolyethyleneVis());
	LUXSimDetectorComponent *topPMTHolder;
	topPMTHolder = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, topPMTHolderOffset ), "TopPMTHolder",
			topPMTHolder_log, innerGaseousXenonVolume, 0, 0, false );

	//	Next, the top PMT window. This is just a large sheet of quartz to
	//	take the place of many discrete PMTs. You have been warned.
	G4double topPMTWindowOffset = topPMTHolderOffset - pmtHolderHeight/2 -
			pmtWindowThickness/2;
	if( PRINTDIMENSIONS )
		G4cout << "Top PMT window offset: " << topPMTWindowOffset/cm << " cm"
			   << G4endl;
	
	G4Tubs *topPMTWindow_solid = new G4Tubs( "topPMTWindow_solid", 0,
			liquidXenonActiveRadius, pmtWindowThickness/2, 0, 360.*deg );
	G4LogicalVolume *topPMTWindow_log = new G4LogicalVolume(
			topPMTWindow_solid, luxMaterials->Quartz(),
			"topPMTWindow_log" );
	topPMTWindow_log->SetVisAttributes( luxMaterials->QuartzVis());
	LUXSimDetectorComponent *topPMTWindow;
	topPMTWindow = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, topPMTWindowOffset ), "TopPMTWindow",
			topPMTWindow_log, innerGaseousXenonVolume, 0, 0, false );
			
	//	Next, the top "photocathode". See the comments on the bottom
	//	"photocathode".
	G4Tubs *topPMTPhotocathode_solid = new G4Tubs(
			"topPMTPhotocathode_solid", 0, liquidXenonActiveRadius,
			pmtWindowThickness/4, 0, 360.*deg );
	G4LogicalVolume *topPMTPhotocathode_log = new G4LogicalVolume(
			topPMTPhotocathode_solid, luxMaterials->Quartz(),
			"topPMTPhotocathode_log" );
	topPMTPhotocathode_log->SetVisAttributes( luxMaterials->QuartzVis() );
	LUXSimDetectorComponent *topPMTPhotocathode;
	topPMTPhotocathode = new LUXSimDetectorComponent( 0, G4ThreeVector(),
			"TopPMTPhotocathode", topPMTPhotocathode_log,
			topPMTWindow, 0, 0, false );
			
	//	Finally, the active gaseous xenon target.
	G4double activeGaseousXenonOffset = topPMTWindowOffset -
			pmtWindowThickness/2 - gasXenonActiveHeight/2;
	if( PRINTDIMENSIONS )
		G4cout << "Active gas xenon target offset: "
			   << activeGaseousXenonOffset/cm << " cm" << G4endl;
	
	G4Tubs *activeXenonGaseousTarget_solid = new G4Tubs(
			"activeXenonGaseousTarget_solid", 0, liquidXenonActiveRadius,
			gasXenonActiveHeight/2, 0, 360.*deg );
	G4LogicalVolume *activeXenonGaseousTarget_log = new G4LogicalVolume(
			activeXenonGaseousTarget_solid, luxMaterials->GasXe(),
			"activeXenonGaseousTarget_log" );
	activeXenonGaseousTarget_log->SetVisAttributes( luxMaterials->GasXeVis());
	LUXSimDetectorComponent *activeXenonGaseousTarget;
	activeXenonGaseousTarget = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, activeGaseousXenonOffset ),
			"ActiveXenonGaseousTarget", activeXenonGaseousTarget_log,
			innerGaseousXenonVolume, 0, 0, false );
	
	//	At this point, we need to create a few optical boundaries between
	//	various volumes. Those volumes are...
	//
	//	activeXenonLiquidTarget and teflonReflector
	//	activeXenonGaseousTarget and teflonReflector
	//	xeSkin and ( teflonReflector and outerTeflonLiner )
	//	gaseousSkin and ( teflonReflector and outerTeflonLiner )
	
	G4LogicalBorderSurface *teflonReflectorLXeSurface;
	teflonReflectorLXeSurface = new G4LogicalBorderSurface(
			"teflonReflectorLXeSurface", activeXenonLiquidTarget,
			teflonReflector, luxMaterials->LXeTeflonSurface() );

	G4LogicalBorderSurface *teflonReflectorGXeSurface;
	teflonReflectorGXeSurface = new G4LogicalBorderSurface(
			"teflonReflectorGXeSurface", activeXenonGaseousTarget,
			teflonReflector, luxMaterials->GXeTeflonSurface() );

	G4LogicalBorderSurface *teflonReflectorLXeSkinSurface;
	teflonReflectorLXeSkinSurface = new G4LogicalBorderSurface(
			"teflonReflectorLXeSkinSurface", xeSkin,
			teflonReflector, luxMaterials->LXeTeflonSurface() );

	G4LogicalBorderSurface *teflonLinerLXeSkinSurface;
	teflonLinerLXeSkinSurface = new G4LogicalBorderSurface(
			"teflonLinerLXeSkinSurface", xeSkin,
			outerTeflonLiner, luxMaterials->LXeTeflonSurface() );

	G4LogicalBorderSurface *teflonReflectorGXeSkinSurface;
	teflonReflectorGXeSkinSurface = new G4LogicalBorderSurface(
			"teflonReflectorGXeSkinSurface", gaseousSkin,
			teflonReflector, luxMaterials->GXeTeflonSurface() );

	G4LogicalBorderSurface *teflonLinerGXeSkinSurface;
	teflonLinerGXeSkinSurface = new G4LogicalBorderSurface(
			"teflonLinerGXeSkinSurface", gaseousSkin,
			outerTeflonLiner, luxMaterials->GXeTeflonSurface() );

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                              ~LUXSimLZFlex
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZFlex::~LUXSimLZFlex() {}
