//	LUXSimWaterPMTBank.cc

//	This is the code to define the PMT bank in the water tank.
//	M.Sweany (sweany1@llnl.gov)


//	04.07.09 - Initial submission (Melinda)
//	08.27.09 - Changed the overlap checking to false, added explicit overlap
//			   check (Kareem)
//	10.01.09 - Code cleanup (Melinda)
//	03.29.10 - Changed dimensions to match with 
//			http://teacher.pas.rochester.edu:8080/wiki/pub/LuxHomestake/WaterTank/SFI_Quote_KM0813271.pdf
//	09 Mar 2012 - Removed pyramid; now created as separate class (Dave)
//	09 Mar 2012 - Changed height to match new water tank height (Dave)
//	28 May 2012 - Changed the internal water tank height to match the specs
//				  from the LZ detector diagrams (Kareem)

//
//	C/C++ includes
//
#include <sstream>

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

//
//	LUXSim includes
//
#include "LUXSimWaterPMTBank.hh"
#include "LUXSimExample8778PMT.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimWaterPMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimWaterPMTBank::LUXSimWaterPMTBank() {

	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	The PMT bank is nothing more than the water of the Muon Veto 
	//	System. The water: check with Simon on dimensions
	G4double radius = 381.*cm;
	G4double height = (15.*12.+2. + 26.5 + 26.5)*2.54*cm;
	
	G4Tubs *waterBlock_solid = new G4Tubs( "waterBlock_solid", 0*cm,radius, 
			height/2., 0.*deg, 360.*deg );
	waterBlock_log = new G4LogicalVolume( waterBlock_solid,
			luxMaterials->Water(), "waterBlock_log" );
	waterBlock_log->SetVisAttributes( luxMaterials->WaterVis() );
	
	
	//	place 20 PMTs in water for LUX
	G4double xOff[20], yOff[20], zOff[20];
	G4double sideRadius = radius - 30.48*cm;	//	placemement of side pmts
	G4double bottomRadius = radius - 60.96*cm;	//	placement of bottom pmts
	G4int pmtCounter = 0;
	for(G4int i=0; i<4; i++){
		G4double theta = (M_PI/2.)*i;
		xOff[pmtCounter] = bottomRadius*cos(theta);
		yOff[pmtCounter] = bottomRadius*sin(theta);
		zOff[pmtCounter] = (13.67/2.)*cm-height/2.;	//pmtRadius/2-waterHeight/2
		pmtCounter++;
		for(G4int j=0; j<4; j++){
			xOff[pmtCounter] = sideRadius*cos(theta);	
	 		yOff[pmtCounter] = sideRadius*sin(theta);
			zOff[pmtCounter] = (height/2.-1219.2*((double)(4-j)));	
			//	pmts have 4ft (=122cm) vertical spacing
			pmtCounter++;
		}
	}
	for(G4int i=0; i<20; i++){
		G4RotationMatrix * rotation = new G4RotationMatrix();
		if (i<5) rotation -> rotateY(M_PI/2.0);
		else if (i<10) rotation -> rotateX((3.0*M_PI)/2.0);
		else if (i<15) rotation -> rotateY((3.0*M_PI)/2.0);
		else if (i<20) rotation -> rotateX(M_PI/2.0);
		PMT7081[i] = new LUXSim7081PMT();
		stringstream name;
		name << "PMT7081_" << i+1;
		if(i%5==0)
			PMTs[i] = new LUXSimDetectorComponent( 0,
					G4ThreeVector( xOff[i], yOff[i], zOff[i] ),
					PMT7081[i]->GetLogicalVolume(), name.str(), waterBlock_log,
					0, 0, false );
		else
			PMTs[i] = new LUXSimDetectorComponent( rotation,
					G4ThreeVector( xOff[i], yOff[i], zOff[i] ),
					PMT7081[i]->GetLogicalVolume(), name.str(), waterBlock_log,
					0, 0, false );
	}
	
	
	
	
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimWaterPMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimWaterPMTBank::~LUXSimWaterPMTBank() {}
