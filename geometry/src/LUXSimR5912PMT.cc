//	LUXSimR5912PMT.cc

//      This is the code file to define the R5912 model PMT by Hamamatsu.
//	It's just like the 7081 PMT definition but the "can" is now made of
//	PVC so that it does not allow photons to enter through the back.
//	It also has a photocathode defined.
//	(Scott Haselschwardt)
//
//	12 May 2014 - Initial submission. (Scott Haselschwardt)
//	17 Oct 2014 - Updated radius and angle to get correct photocathode area
//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Sphere.hh"

//
//	LUXSim includes
//
#include "LUXSimR5912PMT.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimR5912PMT
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimR5912PMT::LUXSimR5912PMT()
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	This PMT is extremely simply modeled: it is a spherical vacuum 
	//	with a quartz semi-sphere window

	//	First make the glass window of the PMT
	G4double radius = 131.*mm; //was 101.*mm
	G4double thickness = 1.*mm;

	G4Sphere *PMTR5912_solid = new G4Sphere( "PMTR5912_solid", 0.*cm,radius,
			0.*deg, 360.*deg, 0.*deg, 90.*deg );
	PMTR5912_log = new G4LogicalVolume( PMTR5912_solid,
			luxMaterials->PVC(), "PMTR5912_log" );
	PMTR5912_log->SetVisAttributes( luxMaterials->PVCVis() );


	//	Now put a quartz window inside the vacuum
	G4Sphere *window_solid = new G4Sphere( "window_solid",radius-thickness,
			radius,0.*deg, 360.*deg, 0.*deg, 90.*deg );
	G4LogicalVolume *window_log = new G4LogicalVolume( window_solid,
			luxMaterials->Quartz(), "PMTR5912Window_log" );
	window_log->SetVisAttributes( luxMaterials->QuartzVis() );
	PMTR5912window = new LUXSimDetectorComponent( 0,G4ThreeVector(), 
			window_log,"PMTR5912Window", PMTR5912_log, 0, 0, false );
			
	//	Now put a quartz photocathode behind the window
	G4Sphere *photocathode_solid = new G4Sphere( "photocathode_solid",radius - 2.*thickness,
			radius-thickness,0.*deg, 360.*deg, 0.*deg, 47.*deg ); //was 90.*deg
	G4LogicalVolume *photocathode_log = new G4LogicalVolume( photocathode_solid,
			luxMaterials->Quartz(), "PMT7081Photocathode_log" );
	photocathode_log->SetVisAttributes( luxMaterials->TitaniumVis() );
	PMTR5912photocathode = new LUXSimDetectorComponent( 0,G4ThreeVector(), 
			photocathode_log,"PMTR5912Photocathode", PMTR5912_log, 0, 0, false );
			
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimR5912PMT
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimR5912PMT::~LUXSimR5912PMT() {}
