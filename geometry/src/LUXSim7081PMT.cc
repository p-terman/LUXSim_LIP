//	LUXSim7081PMT.cc

//      This is the code file to define the R7081 model PMT by Hamamatsu.
//	M.Sweany (sweany1@llnl.gov)

//	04.07.09 - Initial submission (Melinda)

//	08.27.09 - Changed the overlap checking to false, added explicit 
//		overlap check (Kareem)

//	10.01.09 - Code cleanup (Melinda)

//      05.28.14 - PMT vacuum replaced with PVC so it is opaque. Also put in
//                 Photocathode (Scott Ha./Curt)

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Sphere.hh"

//
//	LUXSim includes
//
#include "LUXSim7081PMT.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSim7081PMT
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSim7081PMT::LUXSim7081PMT()
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	This PMT is extremely simply modeled: it is a hemisphere of PVC 
	//	with a quartz window and quartz photocathode

	//	First make the glass window of the PMT
	G4double radius = 13.67*cm;
	G4double thickness = 1.*mm;

	G4Sphere *PMT7081_solid = new G4Sphere( "PMT7081_solid", 0.*cm,radius,
			0.*deg, 360.*deg, 0.*deg, 90.*deg );
	PMT7081_log = new G4LogicalVolume( PMT7081_solid,
			luxMaterials->PVC(), "PMT7081_log" );
	PMT7081_log->SetVisAttributes( luxMaterials->PVCVis() );


	//	Now put a quartz window inside the vacuum
	G4Sphere *window_solid = new G4Sphere( "window_solid",radius-thickness,
			radius,0.*deg, 360.*deg, 0.*deg, 90.*deg );
	G4LogicalVolume *window_log = new G4LogicalVolume( window_solid,
			luxMaterials->Quartz(), "PMT7081Window_log" );
	window_log->SetVisAttributes( luxMaterials->QuartzVis() );
	PMT7081window = new LUXSimDetectorComponent( 0,G4ThreeVector(), 
			window_log,"PMT7081Window", PMT7081_log, 0, 0, false );
			
	//	Now put a quartz photocathode behind the window
	G4Sphere *photocathode_solid = new G4Sphere( "photocathode_solid",radius - 2.*thickness,
			radius-thickness,0.*deg, 360.*deg, 0.*deg, 90.*deg );
	G4LogicalVolume *photocathode_log = new G4LogicalVolume( photocathode_solid,
			luxMaterials->Quartz(), "PMT7081Photocathode_log" );
	photocathode_log->SetVisAttributes( luxMaterials->QuartzVis() );
	PMT7081photocathode = new LUXSimDetectorComponent( 0,G4ThreeVector(), 
			photocathode_log,"PMT7081Photocathode", PMT7081_log, 0, 0, true );
			
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSim7081PMT
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSim7081PMT::~LUXSim7081PMT() {}
