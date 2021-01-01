//	LUXSimLZ20Detector.cc

//	This is the code to define the geometry of the LZ20 detector.
//	Author: Melinda Sweany

//	01.19.10 - Initial Submission copied from LUXSim1_0Detector.  Changed
//			   PMT placement so that it can be scaled to any number, PMT
//			   number scheme needs to be fixed and optical properties added 
//			   back.  Removed reflectors and grids,	but left the bool 
//			   arguments of the class for later. (Melinda)

//	03.05.10 - Got rid of WIRES (Melinda)

//
//	C/C++ includes
//
#include <sstream>
#include <iomanip>
//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4Ellipsoid.hh"
#include "G4Polyhedra.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Material.hh"
//
//	LUXSim includes
//
#include "LUXSimLZ20Detector.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimLZ20PMTBank.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimExample8778PMT.hh"
#include "LUXSimGrid.hh"

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimLZ20Detector
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZ20Detector::LUXSimLZ20Detector( G4bool GRIDS )
{
	GRIDS = false;

	//	Get the LUXSimMaterials pointer
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
		
	//	Vacuum Vessel (or outer cryostat)
	G4double vacuumVesselRadius = 39.53*cm;		
		//outer Radius, inner radius will be outer radius of vacuum
	G4double vacuumVesselHeight = 145.93*cm;
	G4double vacuumVesselThickness = 0.635*cm;
	G4Tubs *vacuumVessel_solid = new G4Tubs( "vacuumVessel_solid",0.*cm, 
			vacuumVesselRadius, vacuumVesselHeight/2.,0.*deg,360.*deg);
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
			"Vacuum", logicalVolume, 0, 0, false );


	//	Copper Thermal Shield 
	G4double thermalShieldRadius = 32.51*cm;
	G4double thermalShieldThickness = 0.48*cm;
	G4double thermalShieldHeight = 95.63*cm;
	G4Tubs *thermalShield_solid = new G4Tubs( "thermalShield_solid",
	thermalShieldRadius - thermalShieldThickness,
	thermalShieldRadius,thermalShieldHeight/2.,0.*deg,360.*deg);
	G4LogicalVolume *thermalShield_log = new G4LogicalVolume(
			thermalShield_solid,luxMaterials->Copper(),"thermalShield_log");
			thermalShield_log -> SetVisAttributes (luxMaterials->CopperVis());
	thermalShield = new LUXSimDetectorComponent( 0, G4ThreeVector(), 
			"ThermalShield",thermalShield_log, vacuum, 0, 0, false);


	// 	Xenon Vessel (or inner cryostat)
	G4double xenonVesselRadius = 30.80*cm;
	G4double xenonVesselThickness = 0.635*cm;
	G4double xenonVesselHeight = 113.03*cm;
	G4Tubs *xenonVessel_solid = new G4Tubs( "xenonVessel_solid",
			xenonVesselRadius - xenonVesselThickness,xenonVesselRadius,
			xenonVesselHeight/2.,0.*deg,360.*deg);
	G4LogicalVolume *xenonVessel_log = new G4LogicalVolume(	xenonVessel_solid,
			luxMaterials->Titanium(),"xenonVessel_log");
	xenonVessel_log -> SetVisAttributes (luxMaterials->TitaniumVis());
	xenonVessel = new LUXSimDetectorComponent( 0, G4ThreeVector(),"XenonVessel",
			xenonVessel_log, vacuum, 0, 0, false);
	
	//	Liquid Xenon
	G4double liquidXenonHeight = xenonVesselHeight;
	G4double liquidXenonRadius = xenonVesselRadius-xenonVesselThickness;
	G4Tubs * liquidXenon_solid = new G4Tubs( "liquidXenon_solid",0.*cm,
			liquidXenonRadius,liquidXenonHeight/2.,0.*deg,360.*deg);
	G4LogicalVolume *liquidXenon_log = new G4LogicalVolume(	liquidXenon_solid,
			luxMaterials->LiquidXe(),"liquidXenon_log");
	liquidXenon_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
	liquidXenon = new LUXSimDetectorComponent( 0, G4ThreeVector(),
			"LiquidXenon",liquidXenon_log,vacuum, 0, 0, false );

	//	Optical boundary between the inner cryostat and liquid xenon
	G4OpticalSurface *titaniumLXeOpSurface = new G4OpticalSurface(
			"titaniumLXeOpSurface", unified, polished, dielectric_metal );
	titaniumLXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->TitaniumTable() );
	G4LogicalBorderSurface *titaniumLXeSurface;
	titaniumLXeSurface = new G4LogicalBorderSurface( "titaniumLXeSurface",
			liquidXenon, xenonVessel, titaniumLXeOpSurface );
	
	//	Gas Xenon
	G4double copperBlockHeight=7.62*cm;
	G4double copperDomeSpace = 2.846*cm;
	G4double blockHDPESpace = 0.936*cm;
	G4double hdpeHolderHeight = 66.62*cm;

	G4double gasXenonHeight = liquidXenonHeight - copperBlockHeight - 
			hdpeHolderHeight - blockHDPESpace - copperDomeSpace;
	G4double gasXenonRadius = xenonVesselRadius-xenonVesselThickness;
	G4double gasZPlane[2] = {-4.36*cm,4.36*cm};
	G4double gasRInner[2] = {0.*cm, 0.*cm};
	G4double gasROuter[2] = {27.25*cm,27.25*cm};
	G4double gasXenonZ = liquidXenonHeight/2.-gasXenonHeight/2.;	
	G4VSolid * gasXenonTop_solid = new G4Tubs( "gasXenonTop_solid", 0.*cm,
			gasXenonRadius,gasXenonHeight/2.,0.*deg,360.*deg);
	G4VSolid * gasXenonBottom_solid = new G4Polyhedra(
			"gasXenonBottom_solid",0.*deg,360.*deg,12,2,gasZPlane,
		gasRInner,gasROuter);
	G4VSolid * gasXenon_solid = new G4UnionSolid( "gasXenon_solid",
			gasXenonTop_solid,gasXenonBottom_solid,0,
			G4ThreeVector(0,0,-gasXenonHeight/2.-gasZPlane[1]));
	G4LogicalVolume *gasXenon_log = new G4LogicalVolume(gasXenon_solid,
			luxMaterials->GasXe(),"gasXenon_log"); 
	gasXenon_log->SetVisAttributes( luxMaterials->GasXeVis() );
	gasXenon = new LUXSimDetectorComponent(0,
			G4ThreeVector(0.*cm,0.*cm,gasXenonZ),"GasXenon",gasXenon_log, 
			liquidXenon, 0, 0, false ); 

	//	Optical boundary between the inner cryostat and gaseous xenon
	G4OpticalSurface *titaniumGasXeOpSurface = new G4OpticalSurface(
			"titaniumGasXeOpSurface", unified, polished, dielectric_metal );
	titaniumGasXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->TitaniumTable() );
	G4LogicalBorderSurface *titaniumGasXeSurface;
	titaniumGasXeSurface = new G4LogicalBorderSurface( "titaniumGasXeSurface",
			gasXenon, xenonVessel, titaniumGasXeOpSurface );
	
	//	Copper Radiation Shield:
	//	There are two radiation shields, one cylinder above the active
	// 	region and one dome shaped (G4Ellipsoid) below the active 
	//	region
	G4double radShieldRadius = xenonVesselRadius-xenonVesselThickness;
	G4double radShieldHeight = 5.08*cm;
	G4double shieldBlockSpace = 15.241*cm;
	G4double radShieldZ = -gasXenonHeight/2. + radShieldHeight/2. + 
			shieldBlockSpace + copperBlockHeight + blockHDPESpace;
	G4double xSemiAxis = radShieldRadius;
	G4double ySemiAxis = xSemiAxis;
	G4double zSemiAxis = radShieldRadius/2.;
	G4double zBot = -radShieldRadius/2.;
	G4double zTop = 0.*cm;
	G4double radDomeZ = -xenonVesselHeight/2.;
			G4Tubs *radShield_solid = new G4Tubs( "radShield_solid",0.*cm,
			radShieldRadius,radShieldHeight/2.,0.*deg,360.*deg);
	G4LogicalVolume *radShield_log = new G4LogicalVolume(radShield_solid,
				luxMaterials->Copper(),"radShield_log");
	radShield_log -> SetVisAttributes (luxMaterials->CopperVis());
	radShield = new LUXSimDetectorComponent(0,
			G4ThreeVector(0.*cm,0.*cm,radShieldZ),"RadShield",radShield_log,
			gasXenon, 0, 0, false);
	G4Ellipsoid *radDome_solid = new G4Ellipsoid( "radDome_solid", 
			xSemiAxis, ySemiAxis, zSemiAxis,zBot, zTop);
	G4LogicalVolume *radDome_log = new G4LogicalVolume(radDome_solid, 
			luxMaterials->Copper(),	"radDome_log");
	radDome_log -> SetVisAttributes (luxMaterials->CopperVis());
	radDome = new LUXSimDetectorComponent(0,G4ThreeVector(0.*cm,0.*cm,radDomeZ),
			"RadDome",radDome_log,vacuum, 0, 0, false);

	//	Optical boundary between the radiation shield and gaseous xenon. No
	//	optical boundary is needed between the radiation dome and anything else
	G4OpticalSurface *radShieldGasXeOpSurface = new G4OpticalSurface(
			"radShieldGasXeOpSurface", unified, polished, dielectric_metal );
	radShieldGasXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->CopperTable() );
	G4LogicalBorderSurface *radShieldGasXeSurface;
	radShieldGasXeSurface = new G4LogicalBorderSurface( "radShieldGasXeSurface",
			gasXenon, radShield, radShieldGasXeOpSurface );

	//	HDPE Holder
	G4double hdpeHolderOuterRadius=xenonVesselRadius-xenonVesselThickness;
	G4double hdpeHolderZ = -liquidXenonHeight/2. + hdpeHolderHeight/2. + 
			copperBlockHeight + copperDomeSpace + blockHDPESpace;
	G4double hdpeHolderZPlane[2] = {-33.31*cm,33.31*cm};
	G4double hdpeHolderRInner[2] = {0.*cm,0.*cm};
	G4double hdpeHolderROuter[2] = {27.25*cm,27.25*cm};
	G4VSolid * hdpeHolderOuter_solid = new G4Tubs( "hdpeHolderOuter_solid",
			0.*cm,hdpeHolderOuterRadius,hdpeHolderHeight/2.,0.*deg,	360.*deg);
	G4VSolid * hdpeHolderCutout_solid = new G4Polyhedra(
			"hdpeHolderCutout_solid",0.*deg,360.*deg,12,2,hdpeHolderZPlane,
			hdpeHolderRInner,hdpeHolderROuter);
	G4VSolid * hdpeHolder_solid = new G4SubtractionSolid ("hdpeHolder_solid", 
			hdpeHolderOuter_solid,hdpeHolderCutout_solid,0,G4ThreeVector());
	G4LogicalVolume * hdpeHolder_log = new G4LogicalVolume(hdpeHolder_solid,
			luxMaterials->Polyethylene(),"hdpeHolder_log");
	hdpeHolder_log->SetVisAttributes( luxMaterials->PolyethyleneVis() );
	hdpeHolder = new LUXSimDetectorComponent( 0,
			G4ThreeVector(0.*cm,0.*cm,hdpeHolderZ),"HDPEHolder",hdpeHolder_log,
			liquidXenon, 0, 0, false);

	//	Optical boundary between the HDPE and liquid xenon
	G4OpticalSurface *hdpeLiquidXeOpSurface = new G4OpticalSurface(
			"hdpeLiquidXeOpSurface", unified, ground, dielectric_dielectric );
	hdpeLiquidXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->PolyTable() );
	G4LogicalBorderSurface *hdpeLiquidXeSurface;
	hdpeLiquidXeSurface = new G4LogicalBorderSurface( "hdpeLiquidXeSurface",
			liquidXenon, hdpeHolder, hdpeLiquidXeOpSurface );

	//	Optical boundary between the HDPE and gaseous xenon
	G4OpticalSurface *hdpeGasXeOpSurface = new G4OpticalSurface(
			"hdpeGasXeOpSurface", unified, ground, dielectric_dielectric );
	hdpeGasXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->PolyTable() );
	G4LogicalBorderSurface *hdpeGasXeSurface;
	hdpeGasXeSurface = new G4LogicalBorderSurface( "hdpeGasXeSurface", gasXenon,
			hdpeHolder, hdpeGasXeOpSurface );

	//	PTFE Sheets
	G4double ptfeSheetsRInner[2] = {27.25*cm,27.25*cm};
	G4double ptfeSheetsZPlane[2] = {-29.623*cm,29.623*cm};
	G4double ptfeSheetsROuter[2] = {28.25*cm,28.26*cm};
	G4Polyhedra * ptfeSheets_solid = new G4Polyhedra( "ptfeSheets_solid",0.*deg,
			360.*deg,12,2,ptfeSheetsZPlane,ptfeSheetsRInner,ptfeSheetsROuter);
	G4LogicalVolume *ptfeSheets_log = new G4LogicalVolume(ptfeSheets_solid,
			luxMaterials->Teflon(),"ptfeSheets_log");
	ptfeSheets_log->SetVisAttributes( luxMaterials->TeflonVis() );
	ptfeSheets = new LUXSimDetectorComponent( 0, G4ThreeVector(),"PTFESheets",
			ptfeSheets_log, hdpeHolder, 0, 0, false);

	//	Optical boundary between the Teflon (PTFE) and liquid xenon
	G4OpticalSurface *ptfeLiquidXeOpSurface = new G4OpticalSurface(
			"ptfeLiquidXeOpSurface", unified, ground, dielectric_dielectric );
	hdpeLiquidXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->TeflonTable() );
	G4LogicalBorderSurface *ptfeLiquidXeSurface;
	ptfeLiquidXeSurface = new G4LogicalBorderSurface( "ptfeLiquidXeSurface",
			liquidXenon, ptfeSheets, ptfeLiquidXeOpSurface );

	//	Optical boundary between the Teflon (PTFE) and gaseous xenon
	G4OpticalSurface *ptfeGasXeOpSurface = new G4OpticalSurface(
			"ptfeGasXeOpSurface", unified, ground, dielectric_dielectric );
	ptfeGasXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->TeflonTable() );
	G4LogicalBorderSurface *ptfeGasXeSurface;
	ptfeGasXeSurface = new G4LogicalBorderSurface( "ptfeGasXeSurface", gasXenon,
			ptfeSheets, ptfeGasXeOpSurface );

	//	PMT Banks: top and bottom copper banks, each with 61 PMTs
	banks[0] = new LUXSimLZ20PMTBank();
	banks[1] = new LUXSimLZ20PMTBank();

	G4double xOff = 0, yOff = 0, zOff = 0;
	G4RotationMatrix *rotY180 = new G4RotationMatrix();
	rotY180->rotateY(180.*deg);

	zOff = -liquidXenonHeight/2.+copperBlockHeight/2.+copperDomeSpace;
	bottomBank = new LUXSimDetectorComponent(0,
			G4ThreeVector(xOff,yOff,zOff), "BottomPMTHolder",
			banks[0]->GetCopperLogical(), liquidXenon, 0, 0, false );

	//	Optical boundary between the bottom PMT bank and liquid xenon
	G4OpticalSurface *bottomBankLiquidXeOpSurface = new G4OpticalSurface(
			"bottomBankLiquidXeOpSurface", unified, polished, dielectric_metal );
	bottomBankLiquidXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->CopperTable() );
	G4LogicalBorderSurface *bottomBankLiquidXeSurface;
	bottomBankLiquidXeSurface = new G4LogicalBorderSurface(
			"bottomBankLiquidXeSurface", liquidXenon, bottomBank,
			bottomBankLiquidXeOpSurface );
	
	G4double totalBlockHeight = 11.049*cm;

	//	Now the top bank
	zOff = -gasXenonHeight/2. + copperBlockHeight/2. + blockHDPESpace;
	topBank = new LUXSimDetectorComponent( rotY180,	
			G4ThreeVector( xOff, yOff, zOff ), "TopPMTHolder",
			banks[1]->GetCopperLogical(), gasXenon, 0, 0, false );

	//	Optical boundary between the top PMT bank and gaseous xenon
	G4OpticalSurface *topBankGasXeOpSurface = new G4OpticalSurface(
			"topBankGasXeOpSurface", unified, polished, dielectric_metal );
	topBankGasXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->CopperTable() );
	G4LogicalBorderSurface *topBankGasXeSurface;
	topBankGasXeSurface = new G4LogicalBorderSurface( "topBankGasXeSurface",
			gasXenon, topBank, topBankGasXeOpSurface );
	
	G4double smallBlockHeight = 2.54*cm;
	
	//	Optical boundary between the bottom PMT windows and the liquid xenon
	G4OpticalSurface *windowLiquidXeOpSurface = new G4OpticalSurface(
			"windowLiquidXeOpSurface", unified, polished,
			dielectric_dielectric );
	windowLiquidXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->QuartzTable() );
	
	//	Optical boundary between the top PMT windows and the gaseous xenon
	G4OpticalSurface *windowGasXeOpSurface = new G4OpticalSurface(
			"windowGasXeOpSurface", unified, polished,
			dielectric_dielectric );
	windowGasXeOpSurface->SetMaterialPropertiesTable(
			luxMaterials->QuartzTable() );
		
	//	PMT Placement and numbering
	G4double pmtSpace = banks[0]->GetPMTSpace();
	G4double pmtRadius = 2.54*cm;
	//	this is the center-to-center spacing between PMTs
	G4double pmtHeight = 12.*cm;
	G4int numPmtsPlaced = 0;
	G4int row = 1;
	while(numPmtsPlaced < 61){
		G4double pmtPlacementRadius = sqrt(pow(pmtRadius+yOff,2)+pow(pmtRadius+xOff,2));
		if(pmtPlacementRadius < banks[0]->GetRadiusCheck()){
			PMT8778[numPmtsPlaced] = new LUXSimExample8778PMT();
			stringstream name;
			stringstream number;
			number << setw(2) << setfill('0') << numPmtsPlaced+1;
			name << "Bottom_PMT_Window_" << number.str();
			PMT8778[numPmtsPlaced]->GetPMTWindow()->SetName( name.str() );
			name.str("");
			name << "Bottom_PMT_Vacuum_" << number.str();
			PMT8778[numPmtsPlaced]->GetPMTVacuum()->SetName( name.str() );
			name.str("");
			name << "Bottom_PMT_Can_" << number.str();
			zOff = -liquidXenonHeight/2. + totalBlockHeight + copperDomeSpace - 
					pmtHeight/2.;
			PMTs[numPmtsPlaced] = new LUXSimDetectorComponent(0,G4ThreeVector(xOff, yOff, zOff),
					PMT8778[numPmtsPlaced]->GetLogicalVolume(), name.str(),liquidXenon_log, 
					0, 0, false );
			zOff = -gasXenonHeight/2. - smallBlockHeight + pmtHeight/2.;
			PMT8778[numPmtsPlaced+61] = new LUXSimExample8778PMT();
			name.str("");
			name << "Top_PMT_Window_" << number.str();
			PMT8778[numPmtsPlaced+61]->GetPMTWindow()->SetName( name.str() );
			name.str("");
			name << "Top_PMT_Vacuum_" << number.str();
			PMT8778[numPmtsPlaced+61]->GetPMTVacuum()->SetName( name.str() );
			name.str("");
			name << "Top_PMT_Can_" << number.str();
			PMTs[numPmtsPlaced+61] = new LUXSimDetectorComponent( rotY180,
					G4ThreeVector( xOff, yOff, zOff ),
					PMT8778[numPmtsPlaced+61]->GetLogicalVolume(), name.str(), 
					gasXenon_log, 0, 0, false );
			numPmtsPlaced++;
			if(yOff!=0){
				PMT8778[numPmtsPlaced] = new LUXSimExample8778PMT();
				name.str("");
				number.str("");
				number << setw(2) << setfill('0') << numPmtsPlaced+1;
				name << "Bottom_PMT_Window_" << number.str();
				PMT8778[numPmtsPlaced]->GetPMTWindow()->SetName( name.str() );
				name.str("");
				name << "Bottom_PMT_Vacuum_" << number.str();
				PMT8778[numPmtsPlaced]->GetPMTVacuum()->SetName( name.str() );
				name.str("");
				name << "Bottom_PMT_Can_" << number.str();
				zOff = -liquidXenonHeight/2. + totalBlockHeight + copperDomeSpace - 
						pmtHeight/2.;
				PMTs[numPmtsPlaced] = new LUXSimDetectorComponent(0,
						G4ThreeVector(xOff, -yOff, zOff),
						PMT8778[numPmtsPlaced]->GetLogicalVolume(), name.str(),
						liquidXenon_log, 0, 0, false );
				zOff = -gasXenonHeight/2. - smallBlockHeight + pmtHeight/2.;
				PMT8778[numPmtsPlaced+61] = new LUXSimExample8778PMT();
				name.str("");
				name << "Top_PMT_Window_" << number.str();
				PMT8778[numPmtsPlaced+61]->GetPMTWindow()->SetName( name.str() );
				name.str("");
				name << "Top_PMT_Vacuum_" << number.str();
				PMT8778[numPmtsPlaced+61]->GetPMTVacuum()->SetName( name.str() );
				name.str("");
				name << "Top_PMT_Can_" << number.str();
				PMTs[numPmtsPlaced+61] = new LUXSimDetectorComponent( rotY180,
						G4ThreeVector( xOff, -yOff, zOff ),
						PMT8778[numPmtsPlaced+61]->GetLogicalVolume(), name.str(), 
						gasXenon_log, 0, 0, false );
				numPmtsPlaced++;
			}
			if(xOff!=0){
				PMT8778[numPmtsPlaced] = new LUXSimExample8778PMT();
				name.str("");
				number.str("");
				number << setw(2) << setfill('0') << numPmtsPlaced+1;
				name << "Bottom_PMT_Window_" << number.str();
				PMT8778[numPmtsPlaced]->GetPMTWindow()->SetName( name.str() );
				name.str("");
				name << "Bottom_PMT_Vacuum_" << number.str();
				PMT8778[numPmtsPlaced]->GetPMTVacuum()->SetName( name.str() );
				name.str("");
				name << "Bottom_PMT_Can_" << number.str();
				zOff = -liquidXenonHeight/2. + totalBlockHeight + copperDomeSpace - 
						pmtHeight/2.;
				PMTs[numPmtsPlaced] = new LUXSimDetectorComponent(0,
						G4ThreeVector( -xOff, yOff, zOff),
						PMT8778[numPmtsPlaced]->GetLogicalVolume(), name.str(),
						liquidXenon_log, 0, 0, false );
				zOff = -gasXenonHeight/2. - smallBlockHeight + pmtHeight/2.;
				PMT8778[numPmtsPlaced+61] = new LUXSimExample8778PMT();
				name.str("");
				name << "Top_PMT_Window_" << number.str();
				PMT8778[numPmtsPlaced+61]->GetPMTWindow()->SetName( name.str() );
				name.str("");
				name << "Top_PMT_Vacuum_" << number.str();
				PMT8778[numPmtsPlaced+61]->GetPMTVacuum()->SetName( name.str() );
				name.str("");
				name << "Top_PMT_Can_" << number.str();
				PMTs[numPmtsPlaced+61] = new LUXSimDetectorComponent( rotY180,
						G4ThreeVector( -xOff, yOff, zOff ),
						PMT8778[numPmtsPlaced+61]->GetLogicalVolume(), name.str(), 
						gasXenon_log, 0, 0, false );
				numPmtsPlaced++;
				if(yOff!=0) {
					PMT8778[numPmtsPlaced] = new LUXSimExample8778PMT();
					name.str("");
					number.str("");
					number << setw(2) << setfill('0') << numPmtsPlaced+1;
					name << "Bottom_PMT_Window_" << number.str();
					PMT8778[numPmtsPlaced]->GetPMTWindow()->SetName( name.str() );
					name.str("");
					name << "Bottom_PMT_Vacuum_" << number.str();
					PMT8778[numPmtsPlaced]->GetPMTVacuum()->SetName( name.str() );
					name.str("");
					name << "Bottom_PMT_Can_" << number.str();
					zOff = -liquidXenonHeight/2. + totalBlockHeight + copperDomeSpace - 
							pmtHeight/2.;
					PMTs[numPmtsPlaced] = new LUXSimDetectorComponent(0,
							G4ThreeVector( -xOff, -yOff, zOff),
							PMT8778[numPmtsPlaced]->GetLogicalVolume(), name.str(),
							liquidXenon_log, 0, 0, false );
					zOff = -gasXenonHeight/2. - smallBlockHeight + pmtHeight/2.;
					PMT8778[numPmtsPlaced+61] = new LUXSimExample8778PMT();
					name.str("");
					name << "Top_PMT_Window_" << number.str();
					PMT8778[numPmtsPlaced+61]->GetPMTWindow()->SetName( name.str() );
					name.str("");
					name << "Top_PMT_Vacuum_" << number.str();
					PMT8778[numPmtsPlaced+61]->GetPMTVacuum()->SetName( name.str() );
					name.str("");
					name << "Top_PMT_Can_" << number.str();
					PMTs[numPmtsPlaced+61] = new LUXSimDetectorComponent( rotY180,
							G4ThreeVector( -xOff, -yOff, zOff ),
							PMT8778[numPmtsPlaced+61]->GetLogicalVolume(), name.str(), 
							gasXenon_log, 0, 0, false );
					numPmtsPlaced++;
				}
			}
			yOff += pmtSpace;
		}
		else {
			if(row%2) yOff = pmtSpace/2.;
			else yOff = 0.;
			xOff += sqrt(3.)/2 * pmtSpace;
			row++;
		}
	}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimLZ20Detector
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZ20Detector::~LUXSimLZ20Detector()
{}
