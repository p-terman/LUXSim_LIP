//	LUXSim1_0PMTBank.cc

//	04.07.09 - Initial submission (Melinda)

//	This is the code to define the PMT bank in LUX1.0.  The PMT placement is 
//	from Kaixuan's GEANT4 code 
//	M.Sweany (sweany1@llnl.gov)

//	04.23.09: Changed from 60 to 61 PMTs (Melinda)

//	05.26.09: Due to the PMT length overshooting the holder's length, the PMTs 
//		are now daughters of the liquid and gas xenon and the copper block 
//		contains a series of cylidrical holes for the PMTs. (Melinda)

//	06.05.09: Added honeycomb-shaped teflon reflectors that sit on pmt holders. 
//		(Melinda)

//	08.31.09: Fixed reflector edges, reflector can no longer exceed bounds of 
//		copper block.  Fixed bug in reflector's cone placement. (Melinda)

//	09.25.09: Changed reflector polyhedra to 12 sides from 6. Added small 
//		dodecagon-shaped block. (Melinda)

//	10.01.09: Code cleanup (Melinda)

//	12.17.09: Changed cone spacers to be two times as long as reflector 
//		height (Melinda)

//	02.26.10: Reflector is now just dodecagon, cone "subtraction" is done
//			  by adding xenon cones in LUXSim1_0DetectorConstruction (Melinda)

//	03.05.10: Added PlaceXenonCones and PlacePMTs, so that the detector
//			  construction is less messy.  Now all placement related to 
//			  the copper blocks is done here, including the optical properties
//			  of the reflector/xenon space boundary. (Melinda)

//	03.09.10 - Changes due to PMT orientation and numbering scheme.  PMT 
//			numbering scheme now conforms to the document at
//			http://teacher.pas.rochester.edu:8080/wiki/pub/Lux/PMT/PMT_map.pdf 
//			(Melinda)

//	04.01.10 - Changed PMT space radius in copper block(Melinda)

//	07.22.10 - Updated the PMT component names to split the window into a dead
//			region and an active region (Kareem)

//	08.06.10 - Changed the geometry definition of the cone-shaped gaps in the
//			reflectors to solve the issue of rounding errors and clipping of
//			the PMT windows (Kareem)

//	09.13.10 - Added PMT photocathode volume and placed underscores into all 
//			PMT component names (Mike W. and Matthew)

//	07.15.11 - Added optical boundary between the PMT can and the PMT window,
//			for photons that travel inside the window and hit the inner wall
//			of the PMT can (yes, this actually happens enough to warrant an
//			update to the geometry) (Kareem)

//	11.29.11 - Replaced call to G4Cons with G4EllipticalCone because G4Cons
//			   causes lost photons. Also, fixed radii of cones, which were too
//			   big and were creating a volume overlap not caught. (Matthew)

//	02.09.12 - Put the photocathode name specifications back in (Kareem)

//      03.26.12 - Allowed for different LXe/GXe reflectivity in cones (Matthew)

//      08.16.12 - Added option to number PMTs to match real numbering (Rich)

//  2013-05-01 - Updated PMT reflector geometry.  (Vic)

//	C/C++ includes
//
#include <sstream>

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4EllipticalCone.hh"
#include "G4Polyhedra.hh"
#include "G4Material.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
//
//	LUXSim includes
//
#include "LUXSim1_0PMTBank.hh"
#include "LUXSimExample8778PMT.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"
#include "LUXSim1_0PMTRenumbering.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"


using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSim1_0PMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSim1_0PMTBank::LUXSim1_0PMTBank() {

	//	Get the LUXSimMaterials pointer
	luxMaterials = LUXSimMaterials::GetMaterials();

	//	The PMT bank is the copper block with holes for PMTs

	//	The copper block
	G4double copperRadius = 30.164*cm;//29.634*cm;
	G4double copperHeight = 7.62*cm;
	G4double copperRInner[2] = {0.*cm,0.*cm};
	G4double copperROuter[2] = {27.25*cm,27.25*cm};
	G4double copperZPlane[2] = {-1.27*cm,1.27*cm};

	G4double pmtRadius = 2.95*cm;		//	the block has bigger space than
										//	pmt radius
	G4double blockSpace = 0.889*cm;
	G4double totalBlockHeight = 11.049*cm;

	G4VSolid * copperBlock1_solid = new G4Tubs( "copperBlock1_solid",0.*cm,
			copperRadius,copperHeight/2.,0.*deg,360.*deg);
	G4VSolid * copperBlock2_solid = new G4Polyhedra( "copperBlock2_solid",
			0.*deg,360.*deg,12,2,copperZPlane,copperRInner,copperROuter);
	G4VSolid *copperBlock_solid = new G4UnionSolid(
			"copperBlock_solid",copperBlock1_solid,copperBlock2_solid,
			0,G4ThreeVector(0.*cm,0.*cm,blockSpace+copperHeight/2.
			+copperZPlane[1]));
	G4Tubs *pmtSpace_solid = new G4Tubs( "pmtSpace_solid",0*mm, pmtRadius,
			totalBlockHeight, 0.*deg, 360.*deg );

	//	place 61 PMTs spacers in block
	G4double xOff =0., yOff =0., zOff = 0.;
	pmtSpace = 6.0*cm;	
	//	this is the center-to-center spacing between PMTs

	for(G4int i = 0; i < 61; i++) {
		if(i < 5) {
			yOff = 4 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 2) * pmtSpace;
		}
		else if(i < 5+6) {
			yOff = 3 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 7.5) * pmtSpace;
		}
		else if(i < 5+6+7) {
			yOff = 2 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 14) * pmtSpace;
		}
		else if(i < 5+6+7+8) {
			yOff = 1 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 21.5) * pmtSpace;
		}
		else if(i < 5+6+7+8+9) {
			yOff = 0;  
			xOff = (i - 30) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8) {
			yOff = -1 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 38.5) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8+7) {
			yOff = -2 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 46) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8+7+6) {
			yOff = -3 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 52.5) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8+7+6+5) {
			yOff = -4 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 58) * pmtSpace;
		}
		copperBlock_solid = new G4SubtractionSolid("copperBlock_solid",
				copperBlock_solid, pmtSpace_solid, 0, 
				G4ThreeVector(xOff,yOff,zOff));
	}
	copperBlock_log = new G4LogicalVolume( copperBlock_solid,
			luxMaterials->Copper(), "copperBlock_log" );
	copperBlock_log->SetVisAttributes( luxMaterials->CopperVis() );

	//	The PTFE reflector is honey-comb shaped, and its atop the 
	//	copper block

	//	reflector_solid is a hexagon  block
	reflectorHeight = 0.66802 * cm;

	G4double zPlane[2] = {0,reflectorHeight};
	G4double rInner[2] = {0.,0.};
	G4double rOuter[2] = {27.25*cm,27.25*cm};

	G4VSolid *reflector_solid = new G4Polyhedra("reflector_solid",
			0.*deg, 360.*deg,12,2, zPlane, rInner, rOuter);
	reflector_log = new G4LogicalVolume( reflector_solid, 
			luxMaterials->Teflon(), "reflector_log");
	reflector_log->SetVisAttributes( luxMaterials->TeflonVis() );
}

void LUXSim1_0PMTBank::PlaceXenonCones( G4String spaceMaterialStr, 
										LUXSimDetectorComponent * reflector ) 
{
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	G4double xOff = 0.*cm, yOff = 0.*cm, zOff = 0.*cm;
	G4Material *spaceMaterial = G4Material::GetMaterial(spaceMaterialStr);

	//	space_solid is the gas/liquid xenon cone:
	//	space placed inside the reflector to make
	//	a honeycomb shape
	
	G4double coneRadiusTop = 2.794 * cm;
	G4double coneRadiusBot = 2.540 * cm;
        G4double semiAxis = (coneRadiusTop - coneRadiusBot) / reflectorHeight;
        G4double zHeight  = (coneRadiusTop + coneRadiusBot) / (2. * semiAxis);
	
	G4EllipticalCone *space_solid = new G4EllipticalCone("space_solid",
				 semiAxis,semiAxis,zHeight,reflectorHeight/2.);
        space_log = new G4LogicalVolume(space_solid,spaceMaterial,"space_log");
        G4RotationMatrix * rotation = new G4RotationMatrix();
        rotation->rotateX(180.*deg);
	
	G4OpticalSurface *reflectorSpaceOpSurface = NULL;
	
	//	place cone shaped holes in reflector
	zOff = reflectorHeight/2.;

	for(G4int i = 0; i < 61; i++) {
		if(i < 5) {
			yOff = 4 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 2) * pmtSpace;
		}
		else if(i < 5+6) {
			yOff = 3 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 7.5) * pmtSpace;
		}
		else if(i < 5+6+7) {
			yOff = 2 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 14) * pmtSpace;
		}
		else if(i < 5+6+7+8) {
			yOff = 1 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 21.5) * pmtSpace;
		}
		else if(i < 5+6+7+8+9) {
			yOff = 0;  
			xOff = (i - 30) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8) {
			yOff = -1 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 38.5) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8+7) {
			yOff = -2 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 46) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8+7+6) {
			yOff = -3 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 52.5) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8+7+6+5) {
			yOff = -4 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 58) * pmtSpace;
		}
		if(strcmp(spaceMaterialStr,"liquidXe")==0) {
		  reflectorSpace[i] = new LUXSimDetectorComponent(rotation,
				G4ThreeVector(xOff,yOff,zOff),space_log,
				"Bottom_XenonSpace",reflector_log,0,0,false);
		  reflectorSpaceOpSurface = new G4OpticalSurface(
				"reflectorSpaceOpSurface",unified, groundfrontpainted,
			      	dielectric_dielectric );
		  reflectorSpaceOpSurface->SetMaterialPropertiesTable(
				luxMaterials->LXeTeflonSurface()->GetMaterialPropertiesTable() );
		}
		if(strcmp(spaceMaterialStr,"gasXe")==0) {
                  reflectorSpace[i] = new LUXSimDetectorComponent(rotation,
				G4ThreeVector(xOff,yOff,zOff),space_log,
				"Top_XenonSpace",reflector_log,0,0,false);
		  reflectorSpaceOpSurface = new G4OpticalSurface(
				"reflectorSpaceOpSurface",unified, groundfrontpainted,
				dielectric_dielectric );
		  reflectorSpaceOpSurface->SetMaterialPropertiesTable(
				luxMaterials->GXeTeflonSurface()->GetMaterialPropertiesTable() );
		}
		reflectorSpaceSurface[i] = new G4LogicalBorderSurface(
			"reflectorSpaceSurface",reflectorSpace[i],reflector,
			reflectorSpaceOpSurface);
	}

}
void LUXSim1_0PMTBank::PlacePMTs(G4LogicalVolume * mother_log,
				 G4double zOff, G4bool useRealNumbering)
{
	G4double xOff = 0.*cm, yOff = 0.*cm;	
	for(G4int i = 0; i < 61; i++) {
		if(i < 5) {
			yOff = 4 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 2) * pmtSpace;
		}
		else if(i < 5+6) {
			yOff = 3 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 7.5) * pmtSpace;
		}
		else if(i < 5+6+7) {
			yOff = 2 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 14) * pmtSpace;
		}
		else if(i < 5+6+7+8) {
			yOff = 1 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 21.5) * pmtSpace;
		}
		else if(i < 5+6+7+8+9) {
			yOff = 0;  
			xOff = (i - 30) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8) {
			yOff = -1 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 38.5) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8+7) {
			yOff = -2 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 46) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8+7+6) {
			yOff = -3 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 52.5) * pmtSpace;
		}
		else if(i < 5+6+7+8+9+8+7+6+5) {
			yOff = -4 * sqrt(3.)/2 * pmtSpace;  
			xOff = (i - 58) * pmtSpace;
		}

		int arrayIndex = 0;
		int oldSimNumber = 0;
		int realPMTNumber = 0;
		if(useRealNumbering) {
		  //Convert spatial number (old simulation number) to new 
		  //array index
		  //which is more closely aligned with real PMT number
		  if(strcmp(mother_log->GetName(),"gasXenon_log")==0) {
		    //Top PMT
		    oldSimNumber = i+1;
		  } else {
		    //Bottom PMT
		    oldSimNumber = i + 62;
		  }
		  realPMTNumber = LUXSim1_0PMTRenumbering::GetRealFromOldSim(oldSimNumber);
		  //Get new array index
		  if(LUXSim1_0PMTRenumbering::IsTop(realPMTNumber))
		    arrayIndex = LUXSim1_0PMTRenumbering::GetTopArrayIndexFromReal(realPMTNumber);
		  if(LUXSim1_0PMTRenumbering::IsBottom(realPMTNumber))
		    arrayIndex = LUXSim1_0PMTRenumbering::GetBottomArrayIndexFromReal(realPMTNumber);
		} else {
		  arrayIndex = i;
		  realPMTNumber = i+1;
		}
		PMT8778[arrayIndex] = new LUXSimExample8778PMT();

		G4RotationMatrix * rotation = new G4RotationMatrix();
		stringstream name;
		stringstream number;
		number.width(2);
		number.fill('0');
		number << realPMTNumber;
		if(strcmp(mother_log->GetName(),"gasXenon_log")==0) {
			rotation->rotateY(180.*deg);
			name.str("");
			name << "Top_PMT_Window_" << number.str();
			PMT8778[arrayIndex]->GetPMTWindow()->SetName( name.str() );
			name.str("");
			name << "Top_PMT_Vacuum_" << number.str();
			PMT8778[arrayIndex]->GetPMTVacuum()->SetName( name.str() );
			name.str("");
			name << "Top_PMT_PhotoCathode_" << number.str();
			PMT8778[arrayIndex]->GetPMTPhotocathode()->SetName( name.str() );
			name.str("");
			name << "Top_PMT_Flashing_" << number.str();
			PMT8778[arrayIndex]->GetPMTFlashing()->SetName( name.str() );
			name.str("");
			name << "Top_PMT_Can_" << number.str();
		}
		else {
			rotation->rotateY(0.*deg);
			name.str("");
			name << "Bottom_PMT_Window_" << number.str();
			PMT8778[arrayIndex]->GetPMTWindow()->SetName( name.str() );
			name.str("");
			name << "Bottom_PMT_Vacuum_" << number.str();
			PMT8778[arrayIndex]->GetPMTVacuum()->SetName( name.str() );
			name.str("");
			name << "Bottom_PMT_PhotoCathode_" << number.str();
			PMT8778[arrayIndex]->GetPMTPhotocathode()->SetName( name.str() );
			name.str("");
			name << "Bottom_PMT_Flashing_" << number.str();
			PMT8778[arrayIndex]->GetPMTFlashing()->SetName( name.str() );
			name.str("");
			name << "Bottom_PMT_Can_" << number.str();
		}

		PMTs[arrayIndex] = new LUXSimDetectorComponent(rotation,
				G4ThreeVector(xOff, yOff, zOff),
				PMT8778[arrayIndex]->GetLogicalVolume(), name.str(),
				mother_log,0, 0, false );
		
		//	Put an optical boundary between the quartz and the PMT can. We have
		//	found evidence of photons travelling inside the quartz, and then
		//	hitting the can. We want those to reflect properly. For now, just
		//	use the aluminum / quartz surface definition in the materials
		//	database.
		G4LogicalBorderSurface *canWindowSurface;
		canWindowSurface = new G4LogicalBorderSurface(
				"canWindowSurface", PMT8778[arrayIndex]->GetPMTWindow(), PMTs[arrayIndex], 
				luxMaterials->AlQuartzSurface() );
	}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSim1_0PMTBank
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSim1_0PMTBank::~LUXSim1_0PMTBank() {}
