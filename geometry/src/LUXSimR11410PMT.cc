////////////////////////////////////////////////////////////////////////////////
/*	LUXSimR11410PMT.cc
*
* This is the code file to define the R11410 model PMT by Hamamatsu.
*
********************************************************************************
* Change log
*	
*	2012-05-28 - Initial submission (Dave and Kareem)
*       2013-09-06 - The PMT body is changed to Teflon to account for sleeve 
*                    to be put over the PMT for reflectivity purposes (Monica)
*       2014-03-07 - Setting up the constructor so that G4bool buildGeometry
*                    is initialized in the .hh file not the .cc.
*	
*/
////////////////////////////////////////////////////////////////////////////////

//
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
#include "LUXSimR11410PMT.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimR11410PMT
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimR11410PMT::LUXSimR11410PMT( G4bool buildGeometry )
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	This PMT is very simply modeled. It is a hollow stainless steel 
	//	can with a quartz window. The steel is 1 mm thick, as is the 
	//	quartz.
	
	//	Notes:
	//	- Face body volume encompasses window and photocathode
	
	//	Set all the dimensions
	pmtHeight = 12.3*cm;
	G4double faceHeight = 3.*cm;

	faceOuterDiameter = 7.6*cm;	//	This is also the PMT diameter
	photocathodeDiameter = 6.4*cm;
	G4double bodyOuterDiameter = 5.33*cm;
	G4double bodyThickness = 1.*mm;
	G4double windowDiameter = faceOuterDiameter - 2.*bodyThickness;
	
	G4double windowThickness = 2.*mm;
	G4double photocathodeThickness = windowThickness/2;
	G4double flashingThickness = 0.1*mm;	//	Value based on the R8778 PMTs
	
	G4double vacuumHeight = pmtHeight - bodyThickness;
	G4double faceVacuumDiameter = windowDiameter;
	G4double bodyVacuumDiameter = bodyOuterDiameter - 2.*bodyThickness;
	G4double faceVacuumHeight = faceHeight - bodyThickness;

	G4double zOffset;
	
	if( buildGeometry ) {
		//	First make the body of the PMT. This is a large cylinder with a tube
		//	subtracted to have one wide part and one narrower part.
		G4Tubs *pmtBody_solid1 = new G4Tubs( "pmtBody_solid1",
				0*mm, faceOuterDiameter/2., pmtHeight/2.,
				0.*deg, 360.*deg );
		//	The extra 1 mm in this next definition is to ensure overlap with the
		//	PMT face outer diameter
		G4Tubs *pmtBody_solid2 = new G4Tubs( "pmtBody_solid2",
				bodyOuterDiameter/2, faceOuterDiameter/2. + 1.*mm, pmtHeight/2.,
				0.*deg, 360.*deg );

		zOffset = -faceHeight;	
		G4SubtractionSolid *pmtBody_solid3 = new G4SubtractionSolid(
				"pmtBody_solid3", pmtBody_solid1, pmtBody_solid2,
				0, G4ThreeVector(0,0,zOffset) );

		logicalVolume = new G4LogicalVolume( pmtBody_solid3,
				luxMaterials->Teflon(), "pmtBody_log" );
		logicalVolume->SetVisAttributes( luxMaterials->TeflonVis() );

		//	Now put a vacuum inside the PMT
		G4Tubs *pmtVacuum_solid1 = new G4Tubs( "pmtVacuum_solid1",
				0*mm, faceVacuumDiameter/2., vacuumHeight/2.,
				0.*deg, 360.*deg );
		G4Tubs *pmtVacuum_solid2 = new G4Tubs( "pmtVacuum_solid2",
				bodyVacuumDiameter/2., faceVacuumDiameter/2. + 1.*mm,
				vacuumHeight/2.,
				0.*deg, 360.*deg );
		
		zOffset = -faceVacuumHeight;
		G4SubtractionSolid *pmtVacuum_solid3 = new G4SubtractionSolid(
				"pmtVacuum_solid3", pmtVacuum_solid1, pmtVacuum_solid2,
				0, G4ThreeVector(0,0,zOffset) );
		
		G4LogicalVolume *pmtVacuum_log = new G4LogicalVolume( pmtVacuum_solid3,
				luxMaterials->Vacuum(), "pmtVacuum_log" );
		pmtVacuum_log->SetVisAttributes( luxMaterials->VacuumVis() );
		
		zOffset = bodyThickness/2;
		PMTVacuum = new LUXSimDetectorComponent( 0, G4ThreeVector(0,0,zOffset), 
				pmtVacuum_log, "PMTVacuum",
				logicalVolume, 0, 0, false );
		
		// Put a quartz window at the top of the PMT vacuum
		G4Tubs *pmtWindow_solid = new G4Tubs( "pmtWindow_solid", 0.0*mm, 
				windowDiameter/2., windowThickness/2., 0.0*deg, 360.0*deg );
		G4LogicalVolume *pmtWindow_log = new G4LogicalVolume( pmtWindow_solid, 
				luxMaterials->Quartz(), "pmtWindow_log" );
		pmtWindow_log->SetVisAttributes( luxMaterials->QuartzVis() );
		
		zOffset = vacuumHeight/2 - windowThickness/2;
		PMTWindow = new LUXSimDetectorComponent( 0,  G4ThreeVector(0,0,zOffset),
				"PMTWindow", pmtWindow_log, PMTVacuum, 0, 0, false );
		
		//	Put flashing in the PMT window, right next to the photocathode
		G4Tubs *pmtFlashing_solid = new G4Tubs( "pmtFlashing_sol", 
				photocathodeDiameter/2., windowDiameter/2., flashingThickness/2,
				0.*deg, 360.*deg );
		G4LogicalVolume *pmtFlashing_log = new G4LogicalVolume(
				pmtFlashing_solid, luxMaterials->Aluminum(), "pmtFlashing_log");
		pmtFlashing_log->SetVisAttributes( luxMaterials->AluminumVis() );
		
		zOffset = photocathodeThickness/2 + flashingThickness/2;
		PMTFlashing = new LUXSimDetectorComponent( 0,
				G4ThreeVector(0,0,zOffset), "PMTAlFlashing", 
				pmtFlashing_log, PMTWindow, 0, 0, false );
		
		//	Optical boundary between the Al flashing and window
		G4LogicalBorderSurface *flashingWindowSurface;
		flashingWindowSurface = new G4LogicalBorderSurface(
				"flashingWindowSurface", PMTWindow, PMTFlashing, 
				luxMaterials->AlQuartzSurface() );

		//	Make the "photocathode" nothing more than a volume of quartz wholly
		//	inside the PMT window, located behind the aluminum flashing.
		G4Tubs *pmtPhotocathode_sol = new G4Tubs( "pmtPhotocathode_sol",
				0.*cm, photocathodeDiameter/2., photocathodeThickness/2.,
				0.*deg, 360.*deg );
		G4LogicalVolume *pmtPhotocathode_log = new G4LogicalVolume(
				pmtPhotocathode_sol, luxMaterials->Quartz(),
				"pmtPhotocathode_log" );
		G4VisAttributes *photocathodeVis = new G4VisAttributes(
				G4Color::Magenta());
		pmtPhotocathode_log->SetVisAttributes( photocathodeVis );
		PMTPhotocathode = new LUXSimDetectorComponent( 0, G4ThreeVector(),
				"PMTPhotocathode", pmtPhotocathode_log, PMTWindow, 0, 0, false);
	}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimR11410PMT
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimR11410PMT::~LUXSimR11410PMT() {}
