//	LUXSimLLNL_SinglePhase.cc
//
//	This is the code file for the LLNL single-phase detector geometry. It is
//	in the moveable-source configuration, with six rings defining the primary
//	volume, and a 4-cm-high secondary volume defined by wires. The collimator is
//	also fully simulated.

//	24 Nov 2010 - Initial submission (Kareem)
//	3 Dec 2010  - Minor changes to the geometry to better match the actual,
//				  real-life detector (Kareem)
//	18 Dec 2010 - Create optical surfaces between the argon/nitrogen gas and
//				  the aluminum or gold parts that touch it. Also, move the gas
//				  Ar/N2 material definition to the LUXSimMaterials class.
//				  (Kareem)
//	4 Jan 2011  - Changed the call to G4SubtractionSolid to have an explicit
//				  G4ThreeVector, rather than just "0", to reflect the
//				  requirements of GEANT4.9.4 (Kareem)
//	31 Mar 2011 - Changed the 406-Torr Ar/N2 mixture to 750 Torr of pure argon.
//				  (Kareem)

//
//	C/C++ includes
//
#include <sstream>

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Material.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"

//
//	LUXSim includes
//
#include "LUXSimLLNL_SinglePhase.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimGrid.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimLLNL_SinglePhase
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLLNL_SinglePhase::LUXSimLLNL_SinglePhase()
{

	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
	
	//	First set up some rotation matrices for later use
	G4RotationMatrix *rotX90 = new G4RotationMatrix();
	rotX90->rotateX(90.*deg);
	G4RotationMatrix *rotX180 = new G4RotationMatrix();
	rotX180->rotateX(180.*deg);

	//
	//	Relevant dimensions
	//
	G4double chamberHalfHeight = 20.65*cm;
	G4double halfCavityHeight = 17.75*cm;
	G4double portDepth = 3.9*cm;
	G4double viewportHeight = 0.635*cm;
	G4double pmtWindowHeight = 1.*mm;
	G4double portPMTGap = 10.*um;
	G4double ringZPos[] = { 0.5*cm, 3.7*cm, 8.5735*cm, 10.5735*cm, 12.5735*cm,
			14.5735*cm, 16.5735*cm, 18.5735*cm, 22.60372*cm };
	G4double collimatorDistance = ringZPos[7] + 0.5*cm + 0.3429*cm;
	
	G4double cavityPadding = viewportHeight + pmtWindowHeight + portPMTGap;
	
	//
	//	Steel chamber
	//
	G4double sweepAngle = 360.*deg;
	G4double startAngle = 0*deg;
	
	G4Tubs *chamber1 = new G4Tubs( "chamber1", 0*cm, 18.4*cm, chamberHalfHeight,
			startAngle, sweepAngle );
	G4Tubs *chamber2 = new G4Tubs( "chamber2", 15.5*cm, 18.5*cm, 14.85*cm,
			startAngle, sweepAngle );
	G4SubtractionSolid *chamber3 = new G4SubtractionSolid( "chamber3", chamber1,
			chamber2, 0, G4ThreeVector() );
	
	G4Tubs *chamber4 = new G4Tubs( "chamber4", 0*cm, 3.8*cm,
			chamberHalfHeight - halfCavityHeight, startAngle, sweepAngle );
	G4SubtractionSolid *chamber5 = new G4SubtractionSolid( "chamber5", chamber3,
			chamber4, 0, G4ThreeVector( 0, 0, chamberHalfHeight ) );
	
	logicalVolume = new G4LogicalVolume( chamber5, luxMaterials->Steel(),
			"steelChamber_log" );
	logicalVolume->SetVisAttributes( luxMaterials->SteelVis() );

	//
	//	Chamber cavity
	//
	G4Tubs *cavity1 = new G4Tubs( "cavity1", 0*cm, 15.2*cm, halfCavityHeight,
			startAngle, sweepAngle );
	G4Tubs *cavity2 = new G4Tubs( "cavity2", 0*cm, 4.1*cm,
			portDepth - cavityPadding, startAngle, sweepAngle );
	G4SubtractionSolid *cavity3 = new G4SubtractionSolid( "cavity3", cavity1,
			cavity2, 0, G4ThreeVector( 0, 0, halfCavityHeight ) );
	
	G4LogicalVolume *cavity_log = new G4LogicalVolume( cavity3,
			luxMaterials->GasAr(), "cavity_log" );
	cavity_log->SetVisAttributes( luxMaterials->AirVis() );
	gasVolume = new LUXSimDetectorComponent( 0, G4ThreeVector(), cavity_log,
			"cavity_phys", logicalVolume, false, 0 );
	
	//
	//	Viewport
	//
	G4Tubs *viewport_phys = new G4Tubs( "viewport_phys", 0*cm, 3.8*cm,
			viewportHeight/2, startAngle, sweepAngle );
	G4LogicalVolume *viewport_log = new G4LogicalVolume( viewport_phys,
			luxMaterials->Quartz(), "viewport_log" );
	viewport_log->SetVisAttributes( luxMaterials->QuartzVis() );
	viewport = new LUXSimDetectorComponent( 0,
			G4ThreeVector(0,0, halfCavityHeight - portDepth + viewportHeight/2),
			viewport_log, "viewport", cavity_log, false, 0 );
	
	//
	//	PMT window
	//
	G4Tubs *pmtWindow_phys = new G4Tubs( "pmtWindow_phys", 0.*cm, 2.3*cm,
			pmtWindowHeight/2, startAngle, sweepAngle );
	G4LogicalVolume *pmtWindow_log = new G4LogicalVolume( pmtWindow_phys,
			luxMaterials->Quartz(), "pmtWindow_log" );
	pmtWindow_log->SetVisAttributes( luxMaterials->QuartzVis() );
	pmtWindow = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, halfCavityHeight - portDepth + cavityPadding
					 - pmtWindowHeight/2 ),
			pmtWindow_log, "pmtWindow", cavity_log, false, 0 );

	//
	//	Active volume
	//
	G4double activeVolumeHeight = (0.3 + 3.8735 + 11 + 0.3429)*cm;
	G4Tubs *activeVolume_phys = new G4Tubs( "activeVolume_phys", 0*cm, 5.09*cm,
			activeVolumeHeight/2, startAngle, sweepAngle );
	G4LogicalVolume *activeVolume_log = new G4LogicalVolume( activeVolume_phys,
			luxMaterials->GasAr(), "activeVolume_log" );
	G4VisAttributes *activeVolumeVis = new G4VisAttributes();
	activeVolumeVis->SetColor( G4Color::Red() );
	activeVolumeVis->SetForceSolid( true );
	activeVolumeVis->SetVisibility( true );
	activeVolume_log->SetVisAttributes( activeVolumeVis );
	activeVolume = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0,
					halfCavityHeight - activeVolumeHeight/2 - portDepth ),
			activeVolume_log, "activeVolume", cavity_log, false, 0 );
	
	//
	//	Field rings
	//
	G4Tubs *fieldRing_phys = new G4Tubs( "fieldRing_phys", 5.1*cm, 7.6*cm,
			0.5*cm, startAngle, sweepAngle );
	G4LogicalVolume *fieldRing_log = new G4LogicalVolume( fieldRing_phys,
			luxMaterials->Aluminum(), "fieldRing_log" );
	fieldRing_log->SetVisAttributes( luxMaterials->AluminumVis() );
	G4LogicalBorderSurface *aluminumGArSurface;
	for( G4int i=0; i<8; i++ ) {
		stringstream name;
		name << "fieldRing" << i;
		fieldRings[i] = new LUXSimDetectorComponent( 0,
				G4ThreeVector( 0, 0, halfCavityHeight - ringZPos[i] ),
				fieldRing_log, name.str().c_str(), cavity_log, false, 0 );
		aluminumGArSurface = new G4LogicalBorderSurface(
				"aluminumGArSurface", gasVolume, fieldRings[i],
				luxMaterials->GArAluminumSurface() );
	}
	
	//
	//	Field shapers
	//
	G4Torus *shaper1 = new G4Torus( "shaper1", 2*cm, 3*cm, 7.6*cm, startAngle,
			sweepAngle );
	G4Box *shaper2 = new G4Box( "shaper2", 11*cm, 11*cm, 4*cm );
	G4SubtractionSolid *shaper3 = new G4SubtractionSolid( "shaper3", shaper1,
			shaper2, 0, G4ThreeVector( 0, 0, 4*cm ) );
	G4Tubs *shaper4 = new G4Tubs( "shaper4", 0.*cm, 7.6*cm, 4.*cm, startAngle,
			sweepAngle );
	G4SubtractionSolid *fieldShaper_phys = new G4SubtractionSolid(
			"fieldShaper_phys", shaper3, shaper4, 0, G4ThreeVector() );
	
	G4LogicalVolume *fieldShaper_log = new G4LogicalVolume( fieldShaper_phys,
			luxMaterials->Aluminum(), "fieldShaper_log" );
	fieldShaper_log->SetVisAttributes( luxMaterials->AluminumVis() );
	shapingRings[0] = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, halfCavityHeight - ringZPos[1] + 2.5*cm  ),
			fieldShaper_log, "shaperRing0", cavity_log, false, 0 );
	shapingRings[1] = new LUXSimDetectorComponent( rotX180,
			G4ThreeVector( 0, 0, halfCavityHeight - ringZPos[2] - 2.5*cm ),
			fieldShaper_log, "shaperRing1", cavity_log, false, 1 );
	
	aluminumGArSurface = new G4LogicalBorderSurface(
			"aluminumGArSurface", activeVolume, shapingRings[0],
			luxMaterials->GArAluminumSurface() );
	aluminumGArSurface = new G4LogicalBorderSurface(
			"aluminumGArSurface", activeVolume, shapingRings[1],
			luxMaterials->GArAluminumSurface() );

	//
	//	Hemisphere
	//
	G4Sphere *hemisphere1 = new G4Sphere( "hemisphere1", 7.5*cm, 7.6*cm,
			startAngle, sweepAngle, 90*deg, 90*deg );
	G4Tubs *hemisphere2 = new G4Tubs( "hemisphere2", 5.1*cm, 7.5*cm, 0.5*cm,
			startAngle, sweepAngle );
	G4UnionSolid *hemisphere_phys = new G4UnionSolid( "hemisphere_phys",
			hemisphere1, hemisphere2, 0, G4ThreeVector( 0, 0, -0.5*cm ) );
	
	G4LogicalVolume *hemisphere_log = new G4LogicalVolume( hemisphere_phys,
			luxMaterials->Aluminum(), "hemisphere_log" );
	hemisphere_log->SetVisAttributes( luxMaterials->AluminumVis() );
	hemisphere = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, halfCavityHeight - ringZPos[8] + 0.5*cm ),
			hemisphere_log, "hemisphere", cavity_log, false, 0 );
	
	aluminumGArSurface = new G4LogicalBorderSurface(
			"aluminumGArSurface", activeVolume, hemisphere,
			luxMaterials->GArAluminumSurface() );

	//
	//	Collimator
	//
	G4Box *collimator1 = new G4Box( "collimator1", 7.5*mm, 7.5*mm, 25./2*mm );
	G4Tubs *collimator2 = new G4Tubs( "collimator2", 0.*mm, 5.*mm, 21.7/2*mm,
			startAngle, sweepAngle );
	G4Tubs *collimator3 = new G4Tubs( "collimator3", 0.*mm, 0.5*mm, 5*mm,
			startAngle, sweepAngle );
	G4UnionSolid *collimator4 = new G4UnionSolid( "collimator4", collimator2,
			collimator3, 0, G4ThreeVector( 0, 0, 21.7/2*mm ) );
	G4SubtractionSolid *collimator5 = new G4SubtractionSolid( "collimator5",
			collimator1, collimator4, 0,
			G4ThreeVector( 0, 0, ( (25.-21.7)/2 - 1.12 )*mm ) );
	G4LogicalVolume *collimator_log = new G4LogicalVolume( collimator5,
			luxMaterials->Aluminum(), "collimator5" );
	collimator_log->SetVisAttributes( luxMaterials->AluminumVis() );
	collimator = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, halfCavityHeight-25./2*mm-collimatorDistance ),
			collimator_log, "collimator", cavity_log, false, 0 );
	
	aluminumGArSurface = new G4LogicalBorderSurface(
			"aluminumGArSurface", activeVolume, collimator,
			luxMaterials->GArAluminumSurface() );

	//
	//	Source
	//
	G4Tubs *source_phys = new G4Tubs( "source_phys", 0.*mm, 2.5*mm, 0.05*mm,
			startAngle, sweepAngle );
	G4LogicalVolume *source_log = new G4LogicalVolume( source_phys,
			luxMaterials->Air(), "source_log" );
	G4VisAttributes *sourceVis = new G4VisAttributes();
	sourceVis->SetColor( G4Color::Red() );
	sourceVis->SetForceSolid( true );
	sourceVis->SetVisibility( true );
	source_log->SetVisAttributes( sourceVis );
	source = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, halfCavityHeight + 0.05*mm -
					collimatorDistance - 25.*mm + (25.-21.7)/2*mm +
					((25.-21.7)/2 - 1.12 )*mm ),
			source_log, "source", cavity_log, false, 0 );
	
	//
	//	Grids
	//
	
	//	Use a volume that holds the grids to make the simulation run a little
	//	faster.
	
	G4Tubs *gridHolder_phys = new G4Tubs( "gridHolder_phys", 0.*cm, 5.05*cm,
			20.*um, startAngle, sweepAngle );
	G4LogicalVolume *gridHolder_log = new G4LogicalVolume( gridHolder_phys,
			luxMaterials->GasAr(), "gridHolder_log" );
	gridHolder_log->SetVisAttributes( activeVolumeVis );
	gridHolder[0] = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, activeVolumeHeight/2 - 3.*mm ), gridHolder_log,
			"gridTop", activeVolume_log, false, 0 );
	gridHolder[1] = new LUXSimDetectorComponent( 0,
			G4ThreeVector( 0, 0, activeVolumeHeight/2 - 3.*mm - 3.8735*cm ),
			gridHolder_log, "gridBottom", activeVolume_log, false, 0 );

	//	The grids have an even number or wires, just to make the loops
	//	symmetric.
	G4double wireSpacing = 1.*mm;
	G4double wireRadius = 15.*um;
	G4int halfNumWires = G4int(5.*cm / 1.*mm);
	G4cout << "halfNumWires = " << halfNumWires << G4endl;
	for( G4int i=0; i<halfNumWires; i++ ) {
		G4double xPos = i*wireSpacing + wireSpacing/2;
		G4double wireHalfLength = sqrt( pow(5.0*cm,2) - pow(xPos,2) );
		
		G4Tubs *wire_phys = new G4Tubs( "wire_phys", 0.*cm, wireRadius,
				wireHalfLength, 0.*deg, 360.*deg );
		G4LogicalVolume *wire_log = new G4LogicalVolume( wire_phys,
				luxMaterials->Gold(), "wire_log" );

		stringstream name;
		name << "topWire" << i << "a";
		LUXSimDetectorComponent *wire = new LUXSimDetectorComponent( rotX90,
				G4ThreeVector( xPos, 0, 0 ), name.str().c_str(), wire_log,
				gridHolder[0], 0, 0, false );
		G4LogicalBorderSurface *goldGArSurface;
		goldGArSurface = new G4LogicalBorderSurface( "goldGArSurface",
				gridHolder[0], wire, luxMaterials->LXeTiSurface() );

		name.str("");
		name << "topWire" << i << "b";
		wire = new LUXSimDetectorComponent( rotX90,
				G4ThreeVector( -xPos, 0, 0 ), name.str().c_str(), wire_log,
				gridHolder[0], 0, 0, false );
		goldGArSurface =
				new G4LogicalBorderSurface( "goldGArSurface",
				gridHolder[0], wire, luxMaterials->GArGoldSurface() );

		name.str("");
		name << "bottomWire" << i << "a";
		wire = new LUXSimDetectorComponent( rotX90,
				G4ThreeVector( xPos, 0, 0 ), name.str().c_str(), wire_log,
				gridHolder[1], 0, 0, false );
		goldGArSurface =
				new G4LogicalBorderSurface( "goldGArSurface",
				gridHolder[1], wire, luxMaterials->GArGoldSurface() );

		name.str("");
		name << "bottomWire" << i << "b";
		wire = new LUXSimDetectorComponent( rotX90,
				G4ThreeVector( -xPos, 0, 0 ), name.str().c_str(), wire_log,
				gridHolder[1], 0, 0, false );
		goldGArSurface =
				new G4LogicalBorderSurface( "goldGArSurface",
				gridHolder[1], wire, luxMaterials->GArGoldSurface() );
	}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimLLNL_SinglePhase
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLLNL_SinglePhase::~LUXSimLLNL_SinglePhase() {}
