//	LUXSimEmptyCryostat.cc

//	This is an empty LUX1_0Detector, so that any water tank
//	simulations will run faster (M.Sweany)
 
//	04.28.09 - Initial submission (Melinda)
//	08.27.09 - Changed the overlap checking to false, added explicit overlap
//			   check (Kareem)
//	10.01.09 - Code cleanup (Melinda)
 
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
//
//	LUXSim includes
//
#include "LUXSimEmptyCryostat.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimEmptyCryostat
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimEmptyCryostat::LUXSimEmptyCryostat()
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	Vacuum Vessel (or outer cryostat)
	G4double vacuumVesselRadius = 39.53*cm;	
	G4double vacuumVesselHeight = 145.93*cm;
	G4double vacuumVesselThickness = 0.635*cm;
	G4Tubs *vacuumVessel_solid = new G4Tubs( "vacuumVessel_solid",0.*cm, 
			vacuumVesselRadius,vacuumVesselHeight/2.,0.*deg,360.*deg);
	logicalVolume = new G4LogicalVolume (vacuumVessel_solid,
			luxMaterials->Titanium(), "vacuumVessel_log" );
	logicalVolume->SetVisAttributes( luxMaterials->TitaniumVis() );
 

	//	now put vacuum inside the vessel
	G4double vacuumRadius = vacuumVesselRadius - vacuumVesselThickness;
	G4double vacuumHeight = vacuumVesselHeight - vacuumVesselThickness;
	G4Tubs * vacuum_solid = new G4Tubs( "vacuum_solid",0.*cm, vacuumRadius, 
			vacuumHeight/2.,0.*deg,360.*deg);
	G4LogicalVolume *vacuum_log = new G4LogicalVolume( vacuum_solid, 
			luxMaterials->Vacuum(),"vacuum_log" );
	vacuum_log->SetVisAttributes( luxMaterials->VacuumVis() );
			vacuum = new LUXSimDetectorComponent( 0, G4ThreeVector(),vacuum_log,
			"vacuum", logicalVolume, 0, 0, false );


	//	Copper Thermal Shield 
	G4double thermalShieldRadius = 32.51*cm;
	G4double thermalShieldThickness = 0.48*cm;
	G4double thermalShieldHeight = 95.63*cm;
	G4Tubs *thermalShield_solid = new G4Tubs( "thermalShield_solid",
			thermalShieldRadius-thermalShieldThickness,thermalShieldRadius,
			thermalShieldHeight/2.,0.*deg,360.*deg);
	G4LogicalVolume *thermalShield_log = new G4LogicalVolume(
			thermalShield_solid,luxMaterials->Copper(),"thermalShield_log");
	thermalShield_log -> SetVisAttributes (luxMaterials->CopperVis());
	thermalShield = new LUXSimDetectorComponent( 0, G4ThreeVector(), 
			"thermalShield",thermalShield_log, vacuum, 0, 0, false);


	// 	Xenon Vessel (or inner cryostat)
	G4double xenonVesselRadius = 30.80*cm;
	G4double xenonVesselThickness = 0.635*cm;
	G4double xenonVesselHeight = 113.03*cm;
	G4Tubs *xenonVessel_solid = new G4Tubs( "xenonVessel_solid",
			xenonVesselRadius - xenonVesselThickness,xenonVesselRadius,
			xenonVesselHeight/2.,0.*deg,360.*deg);
	G4LogicalVolume *xenonVessel_log = new G4LogicalVolume(xenonVessel_solid, 
			luxMaterials->Titanium(),"xenonVessel_log");
	xenonVessel_log -> SetVisAttributes (luxMaterials->TitaniumVis());
	xenonVessel = new LUXSimDetectorComponent( 0, G4ThreeVector(),"xenonVessel",
			xenonVessel_log, vacuum, 0, 0, false);

			
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimEmptyCryostat
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimEmptyCryostat::~LUXSimEmptyCryostat()
{}

