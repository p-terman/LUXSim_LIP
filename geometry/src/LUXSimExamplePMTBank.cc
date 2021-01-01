////////////////////////////////////////////////////////////////////////////////
/*	LUXSimExamplePMTBank.hh
*
* This is the header file to define an example bank of 8778-model PMTs.
*
********************************************************************************
* Change log
*	16 March 2009 - Initial submission (Kareem)
*	26-Aug-09 - Fixed the PMT placement so there is no overlap with the wider
*				PMTs (Kareem)
*	27-Aug-09 - Changed the overlap checking to false, added explicit overlap
*				check (Kareem)
*/
////////////////////////////////////////////////////////////////////////////////

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
#include "LUXSimExamplePMTBank.hh"
#include "LUXSimExample8778PMT.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimExamplePMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimExamplePMTBank::LUXSimExamplePMTBank()
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	The PMT bank is nothing more than a copper block with PMTs in it.

	//	The copper block
	G4double outerDiameter = 15.*cm;
	G4double height = 12.*cm;
	G4Tubs *copperBlock_solid = new G4Tubs( "copperBlock_solid", 0*mm,
			outerDiameter/2, height/2, 0.*deg, 360.*deg );
	logicalVolume = new G4LogicalVolume( copperBlock_solid,
			luxMaterials->Copper(), "copperBlock_log" );
	logicalVolume->SetVisAttributes( luxMaterials->CopperVis() );

	//	Now put three PMTs in the block
	G4double xOff[3], yOff[3], zOff[3];
	xOff[0] = 0;       yOff[0] = 2.6*(sqrt(3)-1./sqrt(3))*cm; zOff[0] = 0.*mm;
	xOff[1] = 2.6*cm;  yOff[1] = -2.6/sqrt(3)*cm;             zOff[1] = 0.*mm;
	xOff[2] = -2.6*cm; yOff[2] = -2.6/sqrt(3)*cm;             zOff[2] = 0.*mm;
	
	for( G4int i=0; i<3; i++ ) {
		PMT8778[i] = new LUXSimExample8778PMT();
		stringstream name;
		name << "PMT8778_" << i+1;
		PMTs[i] = new LUXSimDetectorComponent( 0,
				G4ThreeVector( xOff[i], yOff[i], zOff[i] ),
				PMT8778[i]->GetLogicalVolume(), name.str(), 
				logicalVolume, 0, 0, false );
	}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimExamplePMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimExamplePMTBank::~LUXSimExamplePMTBank() {}
