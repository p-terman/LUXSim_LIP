//	LUXSimMuonVetoSystem.cc

//	This is the code defining the Muon Veto System.
//	It consists of a steel tank and one PMT bank make of water.
//	The PMT placement is done inside LUXSimWaterPMTBank.
//	M.Sweany (sweany1@llnl.gov)


//	04.07.09 - Initial submission (Melinda)
//	08.27.09 - Changed the overlap checking to false, added explicit overlap
//			check (Kareem)
//  03.29.10 - Changed dimensions to match with 
//			http://teacher.pas.rochester.edu:8080/wiki/pub/LuxHomestake/WaterTank/SFI_Quote_KM0813271.pdf


//	10.01.09 - Code cleanup (Melinda)
//	09 Mar 2012 - height variable set in constructor (Dave)
//	09 Mar 2012 - Corrected water tank dimensions to match final CAD (Dave)
//	28 May 2012 - Changed the internal water tank height to match the specs
//				  from the LZ detector diagrams (Kareem)
//	23 Apr 2013 - Changed waterTankRadius to a class variable (Kareem)
//      03 Apr 2014 - Added 160 micron layer of tyvek volume, and added reflection
//	              to water/tyvek boundary. (Curt)
//	12 May 2014 - Added LZ veto system via big "if" statement. (Scott Ha.)
//      16 Dec 2014 - Reverted back to the old file.  This now only gets used to build the 
//                    LUX veto system.  LZ veto is built in LUXSimLZDetector.cc.
//
//    10 Feb 2015 - Updated PMT Component to match naming convention of LZ veto PMTs (QE works again!) ( Doug T)
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
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4UnionSolid.hh"

//
//	LUXSim includes
//
#include "LUXSimDetector.hh"
#include "LUXSimMuonVetoSystem.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimWaterPMTBank.hh"
#include "LUXSimLZWaterPMTBank.hh"
#include "LUXSimMaterials.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimMuonVetoSystem
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimMuonVetoSystem::LUXSimMuonVetoSystem(  )
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	Water Tank: get current dimensions from Simon
	waterTankRadius = 381.*cm;
	//	The following tank height is based on LZ calculations, where the
	//	acrylic tank is 15'2" high, and has 26.5" clearance on the top and
	//	bottom.
//	G4double waterTankHeight = 592*cm;
	G4double waterTankHeight = (15.*12.+2. + 26.5 + 26.5)*2.54*cm;
	G4double waterTankThickness = 0.476*cm;
	G4Tubs *waterTank_solid = new G4Tubs( "waterTank_solid", 0*cm,
			waterTankRadius+waterTankThickness,
			waterTankHeight/2.+waterTankThickness, 0.*deg, 360.*deg );
	logicalVolume = new G4LogicalVolume( waterTank_solid,
			luxMaterials->Steel(), "waterTank_log" );
	logicalVolume->SetVisAttributes( luxMaterials->SteelVis() );
	
	//	Tyvek layer: 160 microns
        G4double tyvekThickness = .016*cm; 
	G4Tubs *tyvek_solid = new G4Tubs( "tyvek_solid", 0*cm,
			waterTankRadius+tyvekThickness, 
			waterTankHeight/2. + tyvekThickness, 0.*deg, 360.*deg );
	G4LogicalVolume *tyvek_log = new G4LogicalVolume( tyvek_solid,
			luxMaterials->Tyvek(), "tyvek_log" );
	tyvek_log->SetVisAttributes( luxMaterials->TeflonVis() );
	tyvek = new LUXSimDetectorComponent(0, G4ThreeVector(), tyvek_log, "Tyvek",
			logicalVolume, 0, 0, true);

	//	Add water bank with PMTs
	bank = new LUXSimWaterPMTBank();
	waterBank = new LUXSimDetectorComponent( 0, G4ThreeVector(), 
			bank->GetLogicalVolume(), "WaterPMTHolder",tyvek_log, 
			0, 0, false );
	stringstream name;
	for( G4int i=0; i<20; i++ ) {
		name.str("");
		name << "Water_PMT_Can_" << i+1;
		bank->GetPMT(i)->SetName( name.str() );
		name.str("");
		name << "Water_PMT_Window_" << i+1;
		bank->GetPMTClass(i)->GetPMTWindow()->SetName( name.str() );
        name.str("");
        name << "Water_PMT_PhotoCathode_" << i+1;
        bank->GetPMTClass(i)->GetPMTPhotocathode()->SetName( name.str() );
	}

	//	The tyvek/water optical boundary:
	G4OpticalSurface * tyvekWaterOpSurface = new G4OpticalSurface(
			"tyvekWaterOpSurface", unified, groundfrontpainted, 
			dielectric_dielectric);
	tyvekWaterOpSurface->SetMaterialPropertiesTable(luxMaterials->TyvekTable());
	G4LogicalBorderSurface *tyvekWaterSurface;
	tyvekWaterSurface = new G4LogicalBorderSurface( "tyvekWaterSurface",
			waterBank, tyvek, tyvekWaterOpSurface);
	
	//	Set height for eternal recordkeeping
	height = waterTankHeight+2*waterTankThickness;
	
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimMuonVetoSystem
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimMuonVetoSystem::~LUXSimMuonVetoSystem()
{}

