//	LUXSimNMDetector.cc

//	05.19.10 - Initial submission based on code provided by Christine Nielsen 
//			at UC Santa Barbara (Melinda):

//	Re-wrote the original code to create a detector class that will be called
//			twice to build the two detectors.  Both detectors and the lead will
//			be placed in LUXSimDetectorConstruction.  Depending on how 
//			complicated the lead becomes, it may require a class of its own. 
//			(Melinda)
	
//	05.27.10 - Added PlaceSource (Melinda)

//	06.15.10 - Moved NMLead placement to here, added side lead (Melinda)

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
#include "G4Cons.hh"
#include "G4Ellipsoid.hh"
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Material.hh"
//
//	LUXSim includes
//
#include "LUXSimNMDetector.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//							  LUXSimNMDetector
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimNMDetector::LUXSimNMDetector(bool NORTH)
{
	//	  Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	The Tank and Lid:
	G4double tankX = 243.84*cm;
	G4double tankY = 121.92*cm;
	G4double tankZ = 78.105*cm;
	G4double tankThickness = 1.27*cm;

	G4Box * tank_solid = new G4Box("tank_solid",tankX/2.,tankY/2.,tankZ/2.);
	logicalVolume = new G4LogicalVolume(tank_solid,
			luxMaterials->Acrylic(),"logicalVolume");
	logicalVolume->SetVisAttributes( luxMaterials->AcrylicVis() );

	//	The Halon:
	G4double halonX = tankX-tankThickness;
	G4double halonY = tankY-tankThickness;
	G4double halonZ = tankZ-tankThickness;

	G4double halonThickness = 0.635*cm;

	G4Box * halon_solid = new G4Box("halon_solid",halonX/2.,halonY/2.,halonZ/2.);
	G4LogicalVolume * halon_log = new G4LogicalVolume(halon_solid,
			luxMaterials->Halon(),"halon_log");
	halon_log->SetVisAttributes( luxMaterials->TeflonVis() );
	halon = new LUXSimDetectorComponent(0, G4ThreeVector(), halon_log, "Halon",
			logicalVolume, 0, 0, true);


	//	The Water:
	G4double waterX = halonX - halonThickness;
	G4double waterY = halonY - halonThickness;
	G4double waterZ = halonZ - halonThickness;

	G4Box * water_solid = new G4Box("water_solid", waterX/2., waterY/2., 
			waterZ/2.);

	G4LogicalVolume * water_log;
	if(NORTH)
		water_log = new G4LogicalVolume(water_solid,
				luxMaterials->DopedWater7(),"water_log");
	else
		water_log = new G4LogicalVolume(water_solid,
				luxMaterials->DopedWater3(),"water_log");

	water_log->SetVisAttributes( luxMaterials->WaterVis() );
	water = new LUXSimDetectorComponent(0, G4ThreeVector(), "Water",water_log, 
			halon, 0, 0, true );

	//	The halon/water boundary:
	G4OpticalSurface * halonWaterOpSurface = new G4OpticalSurface(
			"halonWaterOpSurface", unified, groundfrontpainted, 
			dielectric_dielectric);
	halonWaterOpSurface->SetMaterialPropertiesTable(luxMaterials->HalonTable());
	G4LogicalBorderSurface *halonWaterSurface;
	halonWaterSurface = new G4LogicalBorderSurface( "halonWaterSurface",
			water, halon, halonWaterOpSurface);
	

	//	The PMTs:
	G4double pmtSemiX = 10.16*cm;
	G4double pmtSemiY = 10.16*cm;
	G4double pmtSemiZ = 6.358*cm;
	G4double pmtZBot = -pmtSemiZ;
	G4double pmtZTop = 0.*cm;

	G4double pmtX[2] = {73.66*cm,-73.66*cm};
	G4double pmtY[2] = {0.*cm, 0.*cm};
	G4double pmtZ[2] = {waterZ/2.,waterZ/2.};

	G4Ellipsoid * pmt_solid = new G4Ellipsoid("pmt_solid",pmtSemiX,
			pmtSemiY,pmtSemiZ, pmtZBot,pmtZTop);
	G4LogicalVolume * pmt_log = new G4LogicalVolume(pmt_solid,
			luxMaterials->Quartz(),"pmt_log");
	pmt_log->SetVisAttributes( luxMaterials->QuartzVis() );

	for(int p=0; p<2; p++){
		stringstream name;
		stringstream number;
		number.width(2);
		number.fill('0');
		number << p+1;
		name.str("");
		if(NORTH) name << "North_PMT_" << number.str();
		else name << "South_PMT_" << number.str();
		pmt[p] = new LUXSimDetectorComponent(0, 
				G4ThreeVector(pmtX[p],pmtY[p],pmtZ[p]),name.str(),pmt_log,
				water, 0, 0, true);

	}

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//							  PlaceTopLead
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimNMDetector::PlaceTopLead(double offsetY, double offsetZ, LUXSimDetectorComponent *leadMother)
{

	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//  The top lead:
	G4double topX = 243.84*cm;;
	G4double topY = 121.92*cm;
	G4double topZ = 5.08*cm;

	G4double topPosZ = 48.9825*cm;
	G4double leadHoleR = 10.16*cm;
	G4double leadHoleX = 73.66*cm;

	G4VSolid * top_solid = new G4Box("top_solid",topX/2.,topY/2.,topZ/2.);
	G4VSolid * hole_solid = new G4Tubs("hole_solid",0.*cm,leadHoleR,topZ,0.*deg,360.*deg);
	
	top_solid = new G4SubtractionSolid("top_solid",top_solid, hole_solid,0, 
			G4ThreeVector(leadHoleX,0*cm,0*cm));
	top_solid = new G4SubtractionSolid("top_solid",top_solid, hole_solid,0,
			G4ThreeVector(-leadHoleX,0*cm,0*cm));

	G4LogicalVolume * top_log = new G4LogicalVolume(top_solid,luxMaterials->Lead(),
			"top_log");
	top_log->SetVisAttributes( luxMaterials->LeadVis() );
	topLead = new LUXSimDetectorComponent(0,
			G4ThreeVector(0,offsetY,topPosZ+offsetZ),"TopLead",top_log,
			leadMother, 0, 0, true);

}
//------++++++------++++++------++++++------++++++------++++++------++++++------
//							PlaceSideLead
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimNMDetector::PlaceSideLead(LUXSimDetectorComponent *leadMother, 
									 G4double offsetZ)
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	The side lead:
	G4double thickness = 5.08*cm;
	G4double sideX = 243.84*cm;
	G4double sideY = 248.92*cm;
	G4double sideZ = 83.185*cm;

	G4VSolid * side_solid = new G4Box("side_solid",sideX/2.+thickness,
			sideY/2.+thickness, sideZ/2.);
	G4VSolid * cutout_solid = new G4Box("cutout_solid",sideX/2., sideY/2.,sideZ/2.);
	side_solid = new G4SubtractionSolid("side_solid",side_solid, cutout_solid,
			0,G4ThreeVector());

	G4LogicalVolume * side_log = new G4LogicalVolume(side_solid,luxMaterials->Lead(),
			"side_log");
	side_log->SetVisAttributes( luxMaterials->LeadVis() );
	sideLead = new LUXSimDetectorComponent(0,G4ThreeVector(0,0,offsetZ),"SideLead",
			side_log, leadMother, 0, 0, true);


}
//------++++++------++++++------++++++------++++++------++++++------++++++------
//							 PlaceBottomLead 
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimNMDetector::PlaceBottomLead(LUXSimDetectorComponent *leadMother)
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	The bottom lead:
	G4double bottomX = 203.2*cm;
	G4double bottomY = 213.36*cm;
	G4double bottomZ = 40.64*cm;

	G4double xtraX = bottomX;
	G4double xtraY = 101.6*cm;
	G4double xtraZ = 5.08*cm;

	G4double xtraPosX = 0.*cm;
	G4double xtraPosY = -(bottomY-xtraY)/2.;
	G4double xtraPosZ = (bottomZ+xtraZ)/2.;

	G4double leadZPos = -59.3725*cm;

	G4VSolid * bottom_solid = new G4Box("bottom_solid",bottomX/2.,bottomY/2.,
			bottomZ/2.);
	G4VSolid * xtra_solid = new G4Box("xtra_solid",xtraX/2.,xtraY/2.,xtraZ/2.);
	bottom_solid = new G4UnionSolid("bottom_solid",bottom_solid,xtra_solid,0,
			G4ThreeVector(xtraPosX,xtraPosY,xtraPosZ));
	G4LogicalVolume * bottom_log = new G4LogicalVolume(bottom_solid,
			luxMaterials->Lead(),"bottom_log");
	bottom_log->SetVisAttributes( luxMaterials->LeadVis() );
	bottomLead = new LUXSimDetectorComponent(0,
			G4ThreeVector(0,0,leadZPos),"BottomLead",bottom_log, 
			leadMother, 0, 0, true);

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//							  PlaceSource
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimNMDetector::PlaceSource(G4double xPos, G4double yPos){ 

	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	//	The source:
	G4double sourceRadius = 0.5*cm;
	G4double sourceHeight = 0.25*cm;
	G4double zPos = 18.115*cm;

	G4Tubs * source_solid = new G4Tubs("source_solid",0.*cm,sourceRadius,
			sourceHeight/2.,0.*deg, 360.*deg);
	G4LogicalVolume * source_log = new G4LogicalVolume(source_solid,
			luxMaterials->Lead(),"source_log");
	source_log->SetVisAttributes( luxMaterials->QuartzVis() );
	source = new LUXSimDetectorComponent(0, G4ThreeVector(xPos,yPos,zPos),
			"Source",source_log,bottomLead, 0, 0, true);

}
//------++++++------++++++------++++++------++++++------++++++------++++++------
//							  ~LUXSimNMDetector
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimNMDetector::~LUXSimNMDetector()
{}

