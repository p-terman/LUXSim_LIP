//	LUXSimLZWaterPMTBank.cc

//	This is the code to define the LZ PMT bank in the water tank.

//	05.12.14 - Initial submission (Scott Haselschwardt)

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
#include "LUXSimLZWaterPMTBank.hh"
#include "LUXSimExample8778PMT.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimLZWaterPMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZWaterPMTBank::LUXSimLZWaterPMTBank() {

	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	The PMT bank is the water with 120 PMTs
	G4double radius = 381.*cm;
	G4double height = (15.*12.+2. + 26.5 + 26.5)*2.54*cm;
	G4double LZpmtRadius = 2425.5*mm;
	G4double LZlsTankHeight = 5102.*mm;
	G4double minTubeDistance = 231.*mm;
	
	G4Tubs *waterBlock_solid = new G4Tubs( "waterBlock_solid", 0*cm,radius, 
			height/2., 0.*deg, 360.*deg );
	waterBlock_log = new G4LogicalVolume( waterBlock_solid,
			luxMaterials->Water(), "waterBlock_log" );
	waterBlock_log->SetVisAttributes( luxMaterials->WaterVis() );
	
	
	//	place 20 strings of 6 PMTs in water
	G4double xOff[120], yOff[120], zOff[120];
	G4double sideRadius = LZpmtRadius;
	G4double theta;
	G4int pmtCounter = 0;
	for(G4int i=0; i<20; i++){
		theta = (M_PI/10.)*i + M_PI/20.;
		G4RotationMatrix * rotation = new G4RotationMatrix();
		rotation -> rotateY(M_PI/2.0);
		rotation -> rotateX( -theta );
		
		for(G4int j=0; j<6; j++){
			xOff[pmtCounter] = sideRadius*cos(theta);	
	 		yOff[pmtCounter] = sideRadius*sin(theta);
	 		
	 		if( j==0 ) zOff[pmtCounter] = -height/2. + minTubeDistance; //bottom PMTs
	 		else if( j==5 ) zOff[pmtCounter] = -height/2. + LZlsTankHeight - minTubeDistance; //top PMTs
			else zOff[pmtCounter] = -height/2. + minTubeDistance + (LZlsTankHeight-2.*minTubeDistance)/5.*( (double)(j) );
			
			PMTR5912[pmtCounter] = new LUXSimR5912PMT();
			stringstream name;
			name << "PMTR5912_" << pmtCounter+1;	
			
			PMTs[pmtCounter] = new LUXSimDetectorComponent( rotation,
					G4ThreeVector( xOff[pmtCounter], yOff[pmtCounter], zOff[pmtCounter] ),
					PMTR5912[pmtCounter]->GetLogicalVolume(), name.str(), waterBlock_log,
					0, 0, false );
			
			pmtCounter++;
		}
		
	}
	
	
	
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimLZWaterPMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZWaterPMTBank::~LUXSimLZWaterPMTBank() {}
