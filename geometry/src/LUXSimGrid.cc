///	LUXSimGrid.cc

//	This is a generic algorithm for placing a grid with either a circular 
//	or dodecagon-shaped frame.  The grid can have wires in one direction or 
//	two directions (mesh).  The grid is also placed in a wafer (called 
//	holder in the code) of liquid or gaseous xenon for faster tracking.

//	The grid is made by placing a series of wires, and creating an 
//	intersection solid with a filled-in G4Tubs or G4Polyhedra in order to 
//	cut off the ends of the wires.  A union solid is then created with this
//	shape and the frame shape.  Finally, the grid is placed inside the 
//	liquid or gaseous holder; the holder is what should be placed inside 
//	LUXSim1_0Detector.cc or LUXSim0_1Detector.cc
//	M.Sweany (sweany1@llnl.gov) and A.Shei(ashei@ucdavis.edu)

//	09.25.09 - Initial submission (Melinda and Alex)

//	09.28.09 - Changed grid_log to take any grid material.  Default is steel
//		if the material isn't recognized. (Melinda)

//	09.29.09 - Changed to accept bool parameter to place wires or thin sheet
//		 (Melinda)

//	10.01.09 - Code cleanup (Melinda)

//	12.02.09 - Grid wire planes flush against frame (Alex)

//	12.03.09 - Changed grid overlap check to false (Melinda)

//	03.05.10 - Added PlaceWiresAndFrame and PlaceMeshWires.  Now, the wire
//			placement is done here, as well as the optical surface/boundary
//			declarations.  The logical volume of the frame and holder are
//			in the constructor, and are built in PlaceWiresAndFrame.  
//			PlaceWiresAndFrame MUST be called before PlaceMeshWires, otherwise
//			there will be no frame/holder.  (Melinda)

//	03.09.10 - Rotated wires so that PMTs are in correct orientation with PTFE
//			sheets and grid wires.  (Melinda)

//	03.27.10 - Changed wire overlap checks to false, added cout statements 
//			to indicate wire placement has begin. (Melinda)

//	03.30.10 - Made frame daughter of PTFE, holder and wires daughter of 
//			liquid/gas xenon (Melinda)

//	08.13.10 - Fixed a bug where the optical surface of the grid wires was only
//			being placed for half the grid wires (Kareem)

//	26 Mar 2012 - Grids now using appropriate materials tables from
//				  LUXSimMaterialsso that their reflectivities are correctly set
//				  (Dave)

//	15 Sep 2012 - Fixed wire length specification to eliminate volume overlap
//				  errors (Kareem)
//
//  06 Feb 2013 - Changed all G4Polyhedra definitions to be consistent with those in 
//                LUXSim1_0Detector.cc (Vic)

// 10.04.2013 - Grid holder volumes now have dimensions of +/- 3 times the wire radius rather 
//              than a constant number.  (Vic)

//  2013-04-23 - Changed ptfeSheetsRInner to be 24.313461097 cm (was 23.495 cm).  This means that
//               radial distance to the corner of the PTFE sheets is now right: 23.485 cm (NOT 
//               23.495.  Oops...) is the radial distance to the flat face of the PTFE sheets.
//               (Vic)

//  2013-05-04 - Shaved 0.1 um off of the grid holder volumes and 3.0 um off of the length of the 
//               grid wires to fix some overlap errors.  (Vic)

//  2013-05-27 - Shortened up the wire length by another micron, for a total of four (two off each
//               end) microns. (Vic)

//  2013-06-03 - Added another wire radius in thickness to the top and bottom of the holder volumes.

//  2013-06-07 - More wire/holder jiggery pokery, undoing some past changes. (Matthew and Vic)

//  2013-12-04 - Fixed a mistake where the first wire in a grid gets created with half the length it
//               should have.  This is no longer the case.  I also changed to name "PTFEInnerDia_cm"
//               (which caused this problem in the first place!) to be "PTFEInnerR_cm."

//  2013-12-08 - Got rid of PTFEInnerR_cm all together and made the radius of the wire grid a 
//               parameter that gets passed to these functions.

//  2014-01-04 - Added an if statement to aviod occasional problems with wires having negative 
//               length.  (Vic)
//
//	C/C++ includes
//

#define Cos15deg 9.65925826289068312e-1

// Chop this much off each end of the wires to avoid overlap errors.
#define WireCheat_mm 0.0

#include <sstream>
//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4Box.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Material.hh"
//
//	LUXSim includes
// 
#include "LUXSimGrid.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"


using namespace std;
//------++++++------++++++------+++++------++++++------++++++------++++++------
//								 LUXSimGrid
//------++++++------++++++------+++++------++++++------++++++------++++++------
LUXSimGrid::LUXSimGrid( G4double wireRadius, 
						G4String frameShapeStr,
						G4double frameInnerRadius, 
						G4double frameOuterRadius, 
						G4double frameHeight,			
						G4double wireGridRadius,
						G4String gridMaterialStr,
						G4String gridMotherMaterialStr)
{
	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

  G4double GridFrameRCorners[4] = {frameInnerRadius / Cos15deg,   frameOuterRadius / Cos15deg,   frameOuterRadius / Cos15deg,  frameInnerRadius / Cos15deg};
  G4double GridFrameZCorners[4] = {-0.5 * frameHeight,            -0.5 * frameHeight,            0.5 * frameHeight,            0.5 * frameHeight};
	
	G4Material * gridMaterial = G4Material::GetMaterial(gridMaterialStr);

	//	The frame
	G4VSolid * frame_solid;
	if(frameShapeStr.compare("Dodecagon") == 0){
		frame_solid = new G4Polyhedra("frame_solid", 
		                              0.*deg, 360.*deg, 12, 
		                              4, GridFrameRCorners, GridFrameZCorners);
	}else{
		frame_solid = new G4Tubs("frame_solid", frameInnerRadius, frameOuterRadius, wireRadius * 2., 0. * deg, 360. * deg);
	}
	if(!gridMaterial){
		frame_log = new G4LogicalVolume(frame_solid,luxMaterials->Steel(),
				"frame_log");
	}else{
		frame_log = new G4LogicalVolume(frame_solid,gridMaterial,"frame_log");
	  frame_log->SetVisAttributes( luxMaterials->SteelVis() );
  }
	//	Make gas/liquid xenon grid holder for faster tracking
  G4double holderRCorners[4] = {0.,               wireGridRadius,   wireGridRadius,  0.};
  G4double holderZCorners[4] = {-2. * wireRadius, -2. * wireRadius, 4. * wireRadius, 4. * wireRadius};
	G4VSolid * holder_solid;
	if(frameShapeStr.compare("Dodecagon") == 0 ){
		holder_solid = new G4Polyhedra("holder_solid", 0. * deg, 360. * deg, 12, 
		                               4, holderRCorners, holderZCorners);
	}else{
		holder_solid = new G4Tubs("holder_solid", 0. * cm, frameOuterRadius, frameHeight / 2., 0. * deg, 360. * deg);
  }
	if(gridMotherMaterialStr.compare("LiquidXe") == 0){
		holder_log = new G4LogicalVolume(holder_solid, luxMaterials->LiquidXe(), "holder_log");
		holder_log->SetVisAttributes(luxMaterials->LiquidXeVis());
	}
	if(gridMotherMaterialStr.compare("GasXe") == 0){
		holder_log = new G4LogicalVolume(holder_solid, luxMaterials->GasXe(), "holder_log");
		holder_log->SetVisAttributes(luxMaterials->GasXeVis());
	}
}

void LUXSimGrid::Place1_0WiresAndFrame(G4double wireSpacing,
									                     G4double wireRadius,
									                     G4double wireGridRadius,
									                     G4double zOff,
									                     G4double hdpeHolderZ,
									                     LUXSimDetectorComponent * frameMother,
									                     LUXSimDetectorComponent * gridMother){
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
	G4double xOff = 0. * cm, yOff = 0. * cm;
	G4RotationMatrix* rotX90Y15 = new G4RotationMatrix();
	rotX90Y15->rotateX(90. * deg);
	rotX90Y15->rotateY(15. * deg);
	G4int numWires = (int)(2. * wireGridRadius / wireSpacing) + 2;
	G4Tubs * wire_solid;
	G4LogicalVolume * wire_log;
	LUXSimDetectorComponent * wire;
	if (gridMother->GetName().compare("LiquidXenon") == 0)
	  holder = new LUXSimDetectorComponent(0, G4ThreeVector(xOff,yOff,zOff), "LiquidHolderXenon",
	                                       holder_log, gridMother, 0, 0, false);
	else if(gridMother->GetName().compare("GasXenon") == 0){
	  holder = new LUXSimDetectorComponent(0, G4ThreeVector(xOff,yOff,zOff), "GasHolderXenon",
	                                       holder_log, gridMother, 0, 0, false);
	}else{
	  std::cout << "I'm not sure what the mother volume of this grid should be...  Help!" << std::endl;
	}
	zOff -= hdpeHolderZ;
	frame = new LUXSimDetectorComponent(0, G4ThreeVector(xOff,yOff,zOff), "GridFrame", frame_log,
	                                    frameMother, 0, 0, false);
	zOff = 0. * cm;
	cout << "Starting placement of " << numWires << " wires into " << gridMother->GetName() << endl;
	wire_solid = new G4Tubs("wire_solid", 0. * cm, wireRadius, wireGridRadius - (WireCheat_mm * mm), 0. * deg, 360. * deg);
	wire_log = new G4LogicalVolume(wire_solid, luxMaterials->Steel(), "wire_log");
	wire = new LUXSimDetectorComponent(rotX90Y15, G4ThreeVector(xOff,yOff,zOff),
	                                   "Wire", wire_log, holder, 0, 0, false);
	G4double side = wireGridRadius / (1. + (sqrt(3.) / 2.));
	//	Add reflective surface to grid wharrs
	G4LogicalBorderSurface *gridXeSurface;
	if(gridMother->GetName().compare("LiquidXenon") == 0){
    gridXeSurface = new G4LogicalBorderSurface("gridXeSurface", holder, wire, luxMaterials->LXeSteelSurface());
	}else{
	  gridXeSurface = new G4LogicalBorderSurface("gridXeSurface", holder, wire, luxMaterials->GXeSteelSurface());
  }
	for(int n = 1; n < numWires / 2; n++){
		xOff = n * wireSpacing;
		yOff = 0. * cm;
		G4double wireLength = 0. * cm;
		if(xOff < side / 2.){
			wireLength = 2. * wireGridRadius;
		}else if(xOff < side / 2. * (1. + sqrt(3.))){
			wireLength = 2. * (wireGridRadius - (xOff - side / 2.)/sqrt(3.));
		}else if(xOff < side / 2. * (2. + sqrt(3.))){
			wireLength = 2. * wireGridRadius - side - 3. * (2. * xOff - side - sqrt(3.) * side) / sqrt(3.);
		}
		wireLength -= 3.4 * wireRadius;
		wireLength -= (10. * um); //Subtract 10 um off every wire, not just the first one...
		if (wireLength > 0.){
		  wire_solid = new G4Tubs("wire_solid", 0. * cm, wireRadius, (0.5 * wireLength) - (WireCheat_mm * mm), 0. * deg, 360. * deg);
		  wire_log = new G4LogicalVolume(wire_solid, luxMaterials->Steel(), "wire_log");

		  G4double xOff_prime =  xOff * cos(-15. * deg) + yOff * sin(-15. * deg);
		  G4double yOff_prime = -xOff * sin(-15. * deg) + yOff * cos(-15. * deg);
		  wire = new LUXSimDetectorComponent(rotX90Y15, G4ThreeVector(xOff_prime,yOff_prime,zOff),
		                                     "Wire", wire_log, holder, 0, 0, false);
		  // Add reflective surface to grid wharrs
		  if(gridMother->GetName().compare("LiquidXenon") == 0 ){
		    gridXeSurface = new G4LogicalBorderSurface("gridXeSurface", holder, wire, luxMaterials->LXeSteelSurface());
		  }else{
            gridXeSurface = new G4LogicalBorderSurface("gridXeSurface", holder, wire, luxMaterials->GXeSteelSurface());
          }
		  xOff_prime =  xOff * cos(15. * deg) + yOff * sin(15. * deg);
		  yOff_prime = -xOff * sin(15. * deg) + yOff * cos(15. * deg);
		  wire = new LUXSimDetectorComponent(rotX90Y15, G4ThreeVector(-xOff_prime, yOff_prime, zOff),
		                                     "Wire", wire_log, holder, 0, 0, false);
		  // Add reflective surface to grid wharrs
		  if(gridMother->GetName().compare("LiquidXenon") == 0){
		    gridXeSurface = new G4LogicalBorderSurface("gridXeSurface", holder, wire, luxMaterials->LXeSteelSurface());
		  }else{
		    gridXeSurface = new G4LogicalBorderSurface("gridXeSurface", holder, wire, luxMaterials->GXeSteelSurface());
		  }
		}
		if(n % 100 == 0) cout << "Placing wire number " << n + 1 << endl;
	}
}

void LUXSimGrid::Place1_0MeshWires(G4double wireSpacing, G4double wireRadius, G4double wireGridRadius){

	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	G4double xOff = 0. * cm, yOff = 0. * cm, zOff = 2. * wireRadius;
	G4RotationMatrix* rotY90X15 = new G4RotationMatrix();
	rotY90X15->rotateY(90. * deg);
	rotY90X15->rotateX(15. * deg);
	G4int numWires = (int)(2. * wireGridRadius / wireSpacing) + 2;
	G4Tubs * wire_solid;
	G4LogicalVolume * wire_log;
	LUXSimDetectorComponent * wire;
	cout << "Starting mesh wire placement...\n";
	wire_solid = new G4Tubs("wire_solid", 0. * cm, wireRadius, wireGridRadius - (WireCheat_mm * mm), 0. * deg, 360. * deg);
	wire_log = new G4LogicalVolume(wire_solid, luxMaterials->Steel(), "wire_log");
	wire = new LUXSimDetectorComponent(rotY90X15, G4ThreeVector(xOff,yOff,zOff), "MeshWire",
	                                   wire_log,holder,	0, 0, false);
	G4double side = wireGridRadius / (1. + (sqrt(3.) / 2.));
	//	*** NOTE *** always assuming that the mesh grid (i.e. anode) is in gas...
	G4LogicalBorderSurface *gridXeSurface;
	gridXeSurface = new G4LogicalBorderSurface("gridXeSurface", holder, wire, 
	                                           luxMaterials->GXeSteelSurface());
	for(int n = 1; n < numWires / 2; n++){
		yOff = n * wireSpacing;
		xOff = 0. * cm;
		G4double wireLength = 0. * cm;
		if(yOff < side / 2.) {
			wireLength = 2. * wireGridRadius;
		}else if(yOff < side / 2. * (1. + sqrt(3.))){
			wireLength = 2. * (wireGridRadius - (yOff - side / 2.) / sqrt(3.));
		}else if(yOff < side / 2. * (2. + sqrt(3.))){
			wireLength = 2. * wireGridRadius - side - 3. * (2. * yOff - side - sqrt(3.) * side) / sqrt(3.);
		}
		wireLength -= 0.2 * mm;
		if (wireLength > 0.){
		  wire_solid = new G4Tubs("wire_solid", 0. * cm, wireRadius, (0.5 * wireLength) - (WireCheat_mm * mm), 0. * deg, 360. * deg);
		  wire_log = new G4LogicalVolume(wire_solid, luxMaterials->Steel(), "wire_log");
		  G4double xOff_prime =  xOff * cos(15. * deg) + yOff * sin(15. * deg);
		  G4double yOff_prime = -xOff * sin(15. * deg) + yOff * cos(15. * deg);
		  wire = new LUXSimDetectorComponent(rotY90X15, G4ThreeVector(xOff_prime,yOff_prime,zOff),
		                                     "MeshWire", wire_log, holder, 0, 0, false);
		  gridXeSurface = new G4LogicalBorderSurface("gridXeSurface", holder, wire, 
		                                             luxMaterials->GXeSteelSurface());
		  xOff_prime =  xOff * cos(-15. * deg) + yOff * sin(-15. * deg);
		  yOff_prime = -xOff * sin(-15. * deg) + yOff * cos(-15. * deg);
		  wire = new LUXSimDetectorComponent(rotY90X15, G4ThreeVector(xOff_prime,-yOff_prime,zOff),
		                                     "MeshWire", wire_log, holder, 0, 0, false);
		  gridXeSurface = new G4LogicalBorderSurface("gridXeSurface", holder, wire, 
		                                             luxMaterials->GXeSteelSurface());
		}
		if(n % 100 == 0) cout << "Placing wire number " << n + 1 << endl;			
	}
}

void LUXSimGrid::Place0_1WiresAndFrame(G4double wireSpacing, G4double wireRadius, 
                                       G4double frameInnerRadius, G4double zOff, 
                                       LUXSimDetectorComponent * mother){

	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
	G4double xOff=0.*cm, yOff=0.*cm;
	G4RotationMatrix* rotX90 = new G4RotationMatrix();
	rotX90->rotateX(90.*deg);

	G4int numWires = (int)(2.*frameInnerRadius/wireSpacing)+2;

	
	G4Tubs * wire_solid;
	G4LogicalVolume * wire_log;
	LUXSimDetectorComponent * wire;

	holder = new LUXSimDetectorComponent(0,G4ThreeVector(xOff,yOff,zOff),
			"Holder",holder_log,mother,0,0,false);
	frame = new LUXSimDetectorComponent(0,G4ThreeVector(xOff,yOff,zOff),
			"GridFrame",frame_log,holder,0,0,false);

	zOff = 0.*cm;
	cout << "Starting wire placement, this could take a while...\n";

	wire_solid = new G4Tubs("wire_solid",0.*cm,wireRadius,
			frameInnerRadius,0.*deg,360.*deg);
	wire_log = new G4LogicalVolume(wire_solid,
			luxMaterials->Steel(),"wire_log");
	wire = new LUXSimDetectorComponent(rotX90,
			G4ThreeVector(xOff,yOff,zOff),"Wire",wire_log,holder,	
			0,0,false);

	G4OpticalSurface *gridXeOpSurface = new G4OpticalSurface(
			"gridXeOpSurface", unified, polished, dielectric_metal );
	gridXeOpSurface->SetMaterialPropertiesTable( 
			luxMaterials->BeCu()->GetMaterialPropertiesTable() );
	G4LogicalBorderSurface *gridXeSurface;
	gridXeSurface = new G4LogicalBorderSurface("gridXeSurface",
			holder,wire,gridXeOpSurface);

	for(int n=1; n<numWires/2; n++){
		xOff = n*wireSpacing;
		yOff = 0.*cm;
		G4double wireLength = 0.*cm;
		wireLength = sqrt(pow(frameInnerRadius,2)-pow(xOff,2));
		wireLength -= 0.2*mm;

		wire_solid = new G4Tubs("wire_solid",0.*cm,wireRadius,
				wireLength,0.*deg,360.*deg);
		wire_log = new G4LogicalVolume(wire_solid,
				luxMaterials->Steel(),"wire_log");

		wire = new LUXSimDetectorComponent(rotX90,
				G4ThreeVector(xOff,yOff,zOff),"Wire",wire_log,holder,
				0,0,false);
		gridXeSurface = new G4LogicalBorderSurface("gridXeSurface",
				holder,wire,gridXeOpSurface);

		wire = new LUXSimDetectorComponent(rotX90,
				G4ThreeVector(-xOff,yOff,zOff),"Wire",wire_log,holder,
				0,0,false);
		gridXeSurface = new G4LogicalBorderSurface("gridXeSurface",
				holder,wire,gridXeOpSurface);
	}

}

void LUXSimGrid::Place0_1MeshWires(G4double wireSpacing,
								   G4double wireRadius,
								   G4double frameInnerRadius){

	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();

	G4double xOff=0.*cm, yOff=0.*cm,zOff=2.*wireRadius;
	G4RotationMatrix* rotY90 = new G4RotationMatrix();
	rotY90->rotateY(90.*deg);
	G4int numWires = (int)(2.*frameInnerRadius/wireSpacing)+2;

	G4Tubs * wire_solid;
	G4LogicalVolume * wire_log;
	LUXSimDetectorComponent * wire;

	cout << "Starting mesh wire placement, this could take a while\n";
	wire_solid = new G4Tubs("wire_solid",0.*cm,wireRadius,
			frameInnerRadius,0.*deg,360.*deg);
	wire_log = new G4LogicalVolume(wire_solid,
			luxMaterials->Steel(),"wire_log");
	wire = new LUXSimDetectorComponent(rotY90,
			G4ThreeVector(xOff,yOff,zOff),"MeshWire",wire_log,holder,	
			0,0,false);
	
	G4OpticalSurface *gridXeOpSurface = new G4OpticalSurface(
			"gridXeOpSurface", unified, polished, dielectric_metal );
	gridXeOpSurface->SetMaterialPropertiesTable( 
			luxMaterials->BeCu()->GetMaterialPropertiesTable() );
	G4LogicalBorderSurface *gridXeSurface;
	gridXeSurface = new G4LogicalBorderSurface("gridXeSurface",
			holder,wire,gridXeOpSurface);

	for(int n=1; n<numWires/2; n++){
		yOff = n*wireSpacing;
		xOff = 0.*cm;
		G4double wireLength = 0.*cm;
		wireLength = sqrt(pow(frameInnerRadius,2) - pow(yOff,2));
		wireLength -= 0.2*mm;
		wire_solid = new G4Tubs("wire_solid",0.*cm,wireRadius,
				wireLength,0.*deg,360.*deg);
		wire_log = new G4LogicalVolume(wire_solid,
				luxMaterials->Steel(),"wire_log");

		wire = new LUXSimDetectorComponent(rotY90,
				G4ThreeVector(xOff,yOff,zOff),"MeshWire",wire_log,holder,
				0,0,false);
		gridXeSurface = new G4LogicalBorderSurface("gridXeSurface",
				holder,wire,gridXeOpSurface);

		wire = new LUXSimDetectorComponent(rotY90,
				G4ThreeVector(xOff,-yOff,zOff),"MeshWire",wire_log,holder,
				0,0,false);
		gridXeSurface = new G4LogicalBorderSurface("gridXeSurface",
				holder,wire,gridXeOpSurface);
			
	}
}

//------++++++------++++++------+++++------++++++------++++++------++++++------
//								~LUXSimGrid
//------++++++------++++++------+++++------++++++------++++++------++++++------
LUXSimGrid::~LUXSimGrid() 
{}
