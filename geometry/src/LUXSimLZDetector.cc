////////////////////////////////////////////////////////////////////////////////
//	
//	LUXSimLZDetector.cc
//
//	This is the header file to define the LZ detector as defined in the DOE and
//	NSF proposals circa May 2012. It sits inside the current LUX water tank. Its
//	dimensions are all mostly specified in the documents
//
//		http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LZMtg20120518/
//		LZ7_proposed_radial_dimensions.pdf
//
//	and
//
//		http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LZMtg20120518/
//		Acrylic_Tank_FEA.pptx
//
//	Where the dimensions are not available in these documents, best guesses and
//	suppositions are used. Those guesses are documented in the code itself.
//
//	The variables passed to the class define the liquid height, and whether or
//	not the grids are on. The liquid height is measured with respect to the
//	distance above the top surface of the bottom PMT windows.
//
////////////////////////////////////////////////////////////////////////////////
//
//	Change log
//
//	26 May 2012 - Initial submission (Kareem)
//	 5 Jun 2012 - Added the annual skin PMTs (Kareem)
//   7 Aug 2012 - Added useGrids=false (Matthew)
//	 4 Dec 2012 - Created a separate volume for the bottom liquid xenon skin
//				  (Kareem)
//   9 Sep 2013 - Remade the geometry based on the updates from Aug. collab
//                meeting.  See
//                http://teacher.pas.rochester.edu:8080/wiki/pub/Lz/LZSolidModels/
//                            LZ_August_2013_CAD_Model_Tour_20130817.pdf
//                The annular rings are still not included but are commented out
//                for later use.  The conduits and HV are not yet included and
//                the steel frame for the bottom scintillator portion  (Monica)
//   3 Mar 2014 - Added the grids, and code formatting (Kareem)
//	21 Mar 2014 - Change scintillator to dopedLABGd (linear akyl benzene doped
//                with Gd) (Kevin)
//  29 Apr 2-14 - Changed the name "InnerGaseousXenonAnodeGridHolder" for just
//                that physical volume so that attempts to place sources inside
//                "InnerGaseousXenon" skip that volume (because that volume is
//                filled with sub-volumes) (Kareem)
//
//	01 May 14 - Update geometry to reflect new dimensions and vertical shift. Also added
//		    side reflectors, foam displacer and center reflectors.  To do this the largest
//		    volume (that gets called in LUXSimDetectorConstruction) is now a thin layer of 
//		    water around the scintillator tanks. Added optical surfaces for reflectors. (Scott Ha.)
//
//	06 Feb 15 - Added thermosyphon, PMT cabling conduits and neutron tube. Water tank PMTs are now
//		    created here as well. (Scott Ha.)
//  12 Jun 15 - Added Side skin PMT banks (Jeremy M.)
//      03 Sep 15 - Added photoneutron pig. (Kevin)
//					
////////////////////////////////////////////////////////////////////////////////

//
//      C/C++ includes
//
#include <sstream>
#include <ctime>

//
//      GEANT4 includes
//
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4RotationMatrix.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Torus.hh"
//#include "G4CutTubs.hh"
#include "G4Ellipsoid.hh"
#include "G4Polyhedra.hh"
#include "G4Cons.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

//
//      LUXSim includes
//
#include "LUXSimLZDetector.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimLZDetectorPMTBank.hh"
#include "LUXSimLZWaterPMTBank.hh"
#include "LUXSimLZSkinPMTBank.hh"
#include "LUXSimManager.hh"
//
//		Definitions
//
#define PI 3.14159265358979312
#define PRINTDIMENSIONS 0

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                              LUXSimLZDetector
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimLZDetector::LUXSimLZDetector( G4double liquidHeight, G4String useGrids )
{
    luxMaterials = LUXSimMaterials::GetMaterials();
    LUXSimManager *luxManager = LUXSimManager::GetManager();
	LUXSimLZDetectorPMTBank *tempBank = new LUXSimLZDetectorPMTBank(
			luxMaterials->LiquidXe(), "TEMP", 72.4*cm );
	
	if( liquidHeight < tempBank->GetBankLinerThickness() ) {
	
		G4cout << G4endl << G4endl << G4endl;
		G4cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << G4endl;
		G4cout << G4endl;
		G4cout << "The liquid height must be at least "
			   << tempBank->GetBankLinerThickness()/mm << " mm." << G4endl;
		G4cout << "Resetting the height to "
			   << tempBank->GetBankLinerThickness()/mm << " mm." << G4endl;
		G4cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << G4endl;
		G4cout << G4endl << G4endl << G4endl;
		
		liquidHeight = tempBank->GetBankLinerThickness();
	}
	
	//	We're trying something different here. All variables are going to be
	//	defined and/or calculated at the top of this file. This will hopefully
	//	make all the subsequent code easier to read. 
   
   //water tank dimensions
	G4double waterTankHeight = (15.*12.+2. + 26.5 + 26.5)*2.54*cm;
	G4double waterTankRadius = 381.*cm;
	G4double waterTankThickness = 0.476*cm;
	
	G4double reflectorThickness = 0.6*mm;
	G4double tyvekRadius = 2817.*mm;
	
	//HV umbilical dimensions
	G4double hvConduitAngle = 60.*(PI/180.); //from vertical
	G4double hvConduitRadius = 101.6/2. *mm;
	G4double hvConduitOuterTitaniumVesselJointVerticalHeight = 1881.3*mm; //from bottom of water tank
	G4double hvConduitJointRadius = 540/2.*mm;
	//Notes: the ConeHeight is the desired length, then the ConeRadius is chosen so that the piece left after
	//outer cryo shape subtraction has the right diameter at the joint of the conduit and outer cryo
	//these work!
	G4double hvConduitConeRadius = 425.*mm;
	G4double hvConduitConeHeight = 852.*mm;
	
	//pmt conduit dimensions
	G4double pmtConduitBendRadius = 375.*mm;
	//G4double pmtConduitRadius = 266.7/2.*mm - 2.54*cm;
	G4double pmtConduitRadius = 109.6*mm; //was 108.*mm
	G4double pmtConduitHoleRadius = 127.05*mm; //size of hole in LS tank != conduit radius
	G4double pmtConduitLSTankPosition = 500.126*mm; //position of center of hole
	//G4double pmtConduitVerticalPieceHeight = 759.*mm + 109.74*mm + 40*mm; //this will change, last # is a fudge factor
	G4double pmtConduitFudge = 200.*mm; //factor used for subtracting cryostat shape from conduit
	G4double pmtConduitVerticalPieceHeight = 932.*mm + pmtConduitFudge; //will want to use this eventually
	G4double pmtConduitHorizontalPieceLength = 686.*mm;
	G4double pmtConduitHorizontalPieceVerticalPosition = 5573.*mm; //relative to water tank floor
	G4double pmtConduitSmallVerticalPieceHeight = 30.*mm;
	
	//thermo siphon conduit dimensions
	G4double thermoConduitBendRadius = 250.*mm;
	//G4double thermoConduitRadius = 215.9/2.*mm - 2.54*cm;
	G4double thermoConduitRadius = 80.*mm;
	G4double thermoConduitHoleRadius = 99.6*mm;
	G4double thermoConduitLSTankPosition = 571.5*mm; //position of center of hole
	//G4double thermoConduitVerticalPieceHeight = 759.*mm + 109.74*mm + 40*mm; //this will change, last # is a fudge factor
	G4double thermoConduitFudge = 200.*mm;
	G4double thermoConduitVerticalPieceHeight = 955.*mm + thermoConduitFudge; //will want to use this eventually
	G4double thermoConduitHorizontalPieceLength = 1176.86*mm;
	G4double thermoConduitHorizontalPieceVerticalPosition = 5304.*mm; //relative to water tank floor,
	G4double thermoConduitLowerAnglePieceLength = 84.4*mm;
	G4double thermoConduitUpperAnglePieceLength = 101.6*mm;
	G4double thermoConduitSmallVerticalPieceHeight = 287.5*mm;
	
	//YBe Source
	G4double yBeHoleRadius = 368.3/2.*mm;
	
	//bottom LS tank center hole
	G4double bottomLSTankCenterHoleRadius = 362./2.*mm;
	
	//top LS tank source tube penetrations-three total
	G4double sourceTubePenetrationRadius = 106.426*mm;
	G4double sourceTubePenetrationPosition = 854.456*mm;
	
	//LS tanks
	G4double acrylicOutsideRadius =  3289.3/2.*mm;	// was 3454./2.*mm - 9.2*cm
	G4double acrylicWallThickness = 2.54*cm;	//	1" thick
	G4double acrylicWallOutsideHeight =  4330.7*mm;	//	was 4557.6*mm
	G4double acrylicInsideRadius = acrylicOutsideRadius - acrylicWallThickness;
	G4double acrylicInsideHeight = acrylicWallOutsideHeight - acrylicWallThickness*2.;
	G4double acrylicRibThickness = 2.*2.54*cm;
	G4double acrylicBottomRibHeight = 404.*mm; //was 539.4*mm
	
	//adjust top and bottom tank wall thicknesses on next line: currently 1"
	G4double acrylicTopBottomWallThickness = 12.7*mm + reflectorThickness; // 1/2" thick + reflector that sits in acrylic mother volume
	G4double acrylicInsideHeightCenter = acrylicWallOutsideHeight - (acrylicWallThickness + reflectorThickness)*2.;
	
	//Cryostat vessel dimensions
	G4double titaniumOuterVesselFlangeOutsideRadius = 1947.9/2.*mm;
	G4double titaniumOuterVesselFlangeThickness = 52.*mm; //was 52.325*mm
	G4double titaniumOuterVesselFlangeHeight = 75.*mm; // was 2*2.54 cm
	G4double titaniumOuterVesselWallThickness = 0.8*cm;
	G4double titaniumOuterVesselTopOutsideSemiMajorZHeight = 489.*mm; //was 536.6*mm
	G4double titaniumOuterVesselBottomOutsideSemiMajorZHeight = 476.*mm;	 //was 476.73*mm
	G4double titaniumOuterVesselStraightWallHeight = 2050.*mm; //was 2239.8*mm 
	G4double titaniumOuterVesselOverallHeight = 3015.*mm; //was 3234*mm;
	
	
	G4double titaniumOuterVesselWallOutsideRadius =
			titaniumOuterVesselFlangeOutsideRadius
			- titaniumOuterVesselFlangeThickness;
	G4double titaniumOuterVesselInsideRadius =
			titaniumOuterVesselWallOutsideRadius - titaniumOuterVesselWallThickness;
	
	G4double titaniumOuterVesselTopInsideSemiMajorZHeight =
			titaniumOuterVesselTopOutsideSemiMajorZHeight -
			titaniumOuterVesselWallThickness;

	G4double titaniumOuterVesselBottomInsideSemiMajorZHeight =
			titaniumOuterVesselBottomOutsideSemiMajorZHeight -
			titaniumOuterVesselWallThickness;
	
	G4double titaniumInnerVesselFlangeOutsideRadius = 850.*mm;  //was 839*mm
	G4double titaniumInnerVesselFlangeThickness = 53.*mm; //was 5*cm
	G4double titaniumInnerVesselFlangeHeight = 75.*mm; //was 2.*2.54*cm
	G4double titaniumInnerVesselWallThickness = 0.8*cm;  
	G4double titaniumInnerVesselTopOutsideSemiMajorZHeight = 477.6*mm;  //was 437.6*mm
	G4double titaniumInnerVesselBottomOutsideSemiMajorZHeight = 287.9*mm;  //was 427.61*mm

	
	G4double titaniumInnerVesselWallTopOutsideRadius =
			titaniumInnerVesselFlangeOutsideRadius
			- titaniumInnerVesselFlangeThickness;
	G4double titaniumInnerVesselWallTopInsideRadius =
			titaniumInnerVesselWallTopOutsideRadius -
            titaniumInnerVesselWallThickness;
	
	G4double titaniumInnerVesselWallBottomOutsideRadius = 1678./2.*mm; 
	G4double titaniumInnerVesselWallBottomInsideRadius =
            titaniumInnerVesselWallBottomOutsideRadius -
            titaniumInnerVesselWallThickness;
	
	G4double titaniumInnerVesselStraightWallHeightTop = 671.*mm;  //was 1043.22*mm
	G4double titaniumInnerVesselBulgeWallHeight = 686.*mm;  //was 350*mm
	G4double titaniumInnerVesselStraightWallHeightBottom = 549.*mm;  //was 484.522

	G4double titaniumInnerVesselTopInsideSemiMajorZHeight =
            titaniumInnerVesselTopOutsideSemiMajorZHeight -
			titaniumInnerVesselWallThickness;
	
	G4double titaniumInnerVesselBottomInsideSemiMajorZHeight =
			titaniumInnerVesselBottomOutsideSemiMajorZHeight -
			titaniumInnerVesselWallThickness;
	
	//  This next line assume that center of outer cryostat is the 0 pt
	//dimensions are relative to center of outer cryostat
	G4double titaniumOuterVesselBottomFlangeLocation = 184.5*mm;  //was 314.9*mm
	
	//This next line is the offset that moves the center of the outer cyrostat 
	//to the correct z position within the LS tank.
	G4double OuterTitaniumVesselVerticalOffset = 23.45*mm; //was 45.*mm
	
	G4double activeLXeTargetHeight = 1481.*mm;  //was 146.2*cm was 1481
	G4double cathodeToBottomGrid = 140.*mm;  //need update here? was 14.6*cm
	G4double bottomGridToBottomPMTBank = 1.0*cm;  //need update here?
	G4double pmtHeight = tempBank->GetPMTHeight();
	G4double liquidSurfaceToAnode = 0.5*cm;  //need update here?
	G4double anodeToTopGrid = 3.8*cm;  //need update here?
	G4double topGridToTopPMTBank = 1.0*cm;  //need update here?
	
	
	G4double ptfeHeight = activeLXeTargetHeight + cathodeToBottomGrid +
            bottomGridToBottomPMTBank;
	G4double ptfeGaseousHeight = liquidSurfaceToAnode + anodeToTopGrid +
            topGridToTopPMTBank;
	G4double gaseousXenonStraightWallHeight = ptfeGaseousHeight + pmtHeight ;
	
	G4double innerPTFEFlatRadius = 739.0*mm; //?= activeXeradius? was 72.8*cm
	G4double outerPTFEFlatRadius = innerPTFEFlatRadius + 2*cm;
	
	G4double ptfeLinerThickness = 2.*mm;

	G4double ptfeLinerOuterBottomRadius =
            titaniumInnerVesselWallBottomInsideRadius;
	G4double ptfeLinerInnerBottomRadius = ptfeLinerOuterBottomRadius -
            ptfeLinerThickness;
	G4double ptfeLinerOuterTopRadius = titaniumInnerVesselWallTopInsideRadius;
	G4double ptfeLinerInnerTopRadius = ptfeLinerOuterTopRadius -
            ptfeLinerThickness;
	
	G4double ptfeLinerTopStraightHeight =
            titaniumInnerVesselStraightWallHeightTop -
            gaseousXenonStraightWallHeight;
	
	G4double ptfeLinerBottomOuterSemiMajorZHeight =
			titaniumInnerVesselBottomInsideSemiMajorZHeight;
	G4double ptfeLinerBottomInnerSemiMajorZHeight =
			ptfeLinerBottomOuterSemiMajorZHeight - ptfeLinerThickness;

   //straight neutron tube dimensions
   G4double outerCryostatToWaterTankDistance = waterTankRadius - titaniumOuterVesselWallOutsideRadius;
	G4double neutronTubeOuterRadius = 3.*cm; //6*cm OD
	G4double neutronTubeVerticalPosition =  3168.5*mm; //relative to bottom of water tank, was 3153.51*mm
	G4double neutronTubeLength = outerCryostatToWaterTankDistance - 2.*cm; //how big is the water gap for real?
	G4double neutronTubeThickness = 0.55*cm;  //for 4.9*cm ID
   
   //angled neutron tube dimensions
	G4double angledNeutronTubeOuterRadius = neutronTubeOuterRadius;
	//to adjust angle of tube, change the next two numbers
	//they are the height of the top and bottom of the tube relative to the bottom of the water tank
	G4double angledNeutronTubeTopVerticalPosition =  2964.2*mm; //relative to bottom of water tank
	G4double angledNeutronTubeBottomVerticalPosition =  1296.8*mm; //relative to bottom of water tank
	//////
	G4double angledNeutronTubeAngle = atan( (angledNeutronTubeTopVerticalPosition-angledNeutronTubeBottomVerticalPosition)/outerCryostatToWaterTankDistance );
	//needed to take a little off the length until we make the ends bevelled
	G4double angledNeutronTubeLength = (angledNeutronTubeTopVerticalPosition-angledNeutronTubeBottomVerticalPosition)/sin(angledNeutronTubeAngle) - 3.5*cm;
   
   
   G4double weirHeight = 3.05*cm;

	/*To be added for pmt annular window
	G4double pmtAnnularSkinWindowThickness = 2.*mm;
	G4double pmtAnnularSkinPhotocathodeThickness = 
			pmtAnnularSkinWindowThickness/2;
	*/

	delete tempBank;
	G4double zOffset;
	G4double xOffset;
	G4double yOffset;

    //  Remember that the outermost vessel is defined as a LUXSimDetector
	//	"logicalVolume".

	////////////////////
	//Steel Water Tank -- this becomes a LUXSimDetectorComponent in LUXSimDetectorConstruction.cc
	////////////////////
   G4Tubs *waterTank_solid = new G4Tubs( "waterTank_solid", 0*cm,
	   		   waterTankRadius+waterTankThickness,
			   waterTankHeight/2.+waterTankThickness, 0.*deg, 360.*deg );
	logicalVolume = new G4LogicalVolume( waterTank_solid,
			luxMaterials->Steel(), "waterTank_log" );
	logicalVolume->SetVisAttributes( luxMaterials->SteelVis() );
	
	//Get the water with PMTs in it
	LUXSimLZWaterPMTBank *LZWaterBank = new LUXSimLZWaterPMTBank();
	
	LUXSimDetectorComponent *waterBank = new LUXSimDetectorComponent( 0, G4ThreeVector(), 
		    	LZWaterBank->GetLogicalVolume(), "WaterAndPMTs",logicalVolume,
		    	0, 0, true );
	
	//Give the PMTs names
	stringstream name;
		for( G4int i=0; i<120; i++ ) {
			name.str("");
			name << "Water_PMT_Can_" << i+1;
			LZWaterBank->GetPMT(i)->SetName( name.str() );
			name.str("");
			name << "Water_PMT_Window_" << i+1;
			LZWaterBank->GetPMTClass(i)->GetPMTWindow()->SetName( name.str() );
			name.str("");
			name << "Water_PMT_PhotoCathode_" << i+1;
			LZWaterBank->GetPMTClass(i)->GetPMTPhotocathode()->SetName( name.str() );
		}
   ////////////////////////////////
	//The tyvek curtain in the water
	///////////////////////////////
	//wall
   G4Tubs *tyvek_solid = new G4Tubs( "tyvek_solid", tyvekRadius,
			tyvekRadius + reflectorThickness, 
			( acrylicWallOutsideHeight + acrylicBottomRibHeight )/2. , 0.*deg, 360.*deg );
	//bottom
	G4Tubs *tyvek_solid2b = new G4Tubs( "tyvek_solid2b", acrylicOutsideRadius,
			tyvekRadius + reflectorThickness, 
			reflectorThickness/2., 0.*deg, 360.*deg );
	//top
	G4Tubs *tyvek_solid2t = new G4Tubs( "tyvek_solid2t", acrylicOutsideRadius,
			tyvekRadius + reflectorThickness, 
			reflectorThickness/2., 0.*deg, 360.*deg );
	//small vertical piece connects to bottom
	G4Tubs *tyvek_solid2v = new G4Tubs( "tyvek_solid2v", acrylicOutsideRadius,
			acrylicOutsideRadius + reflectorThickness, 
			acrylicBottomRibHeight/2., 0.*deg, 360.*deg );
	
	//top piece to wall
	zOffset = ( acrylicWallOutsideHeight + acrylicBottomRibHeight )/2. + reflectorThickness/2.;
	G4UnionSolid *tyvek_solid3 = new G4UnionSolid(
			"tyvek_solid3", tyvek_solid, tyvek_solid2t,
			0, G4ThreeVector(0,0, zOffset) ); //top of LS tanks
	
	//then add the bottom
	zOffset = - ( acrylicWallOutsideHeight + acrylicBottomRibHeight )/2. - reflectorThickness/2.;
	G4UnionSolid *tyvek_solid4 = new G4UnionSolid(
			"tyvek_solid4", tyvek_solid3, tyvek_solid2b,
			0, G4ThreeVector(0,0, zOffset) );	
	
	//then add the small vertical piece
	zOffset = - acrylicWallOutsideHeight/2.;
	G4UnionSolid *tyvek_solid5 = new G4UnionSolid(
			"tyvek_solid4", tyvek_solid4, tyvek_solid2v,
			0, G4ThreeVector(0,0, zOffset) );	
	
	//subtract hole for neutron tube
	G4Tubs *neutronTubeTyvekHole_solid = new G4Tubs(
	   "neutronTubeTyvekHole_solid",
	   0.*cm, neutronTubeOuterRadius + 1.*mm,
	   neutronTubeLength/2.,
	   0.*deg, 360.*deg );
	
	G4RotationMatrix *rotation = new G4RotationMatrix();
	rotation->rotateX( 90.*deg );
	
   zOffset = - ( acrylicWallOutsideHeight + acrylicBottomRibHeight )/2. + neutronTubeVerticalPosition;
   xOffset = 0*cm;
   yOffset = - ( neutronTubeLength/2. + titaniumOuterVesselWallOutsideRadius );
   G4SubtractionSolid *tyvek_solid6 = new G4SubtractionSolid(
      "tyvek_solid6",
	   tyvek_solid5,neutronTubeTyvekHole_solid,
	   rotation,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	//and a hole for angled tube
	G4RotationMatrix *angledTubeRotation = new G4RotationMatrix();
	angledTubeRotation->rotateY( PI/2. - angledNeutronTubeAngle );
	zOffset = - acrylicBottomRibHeight - acrylicWallOutsideHeight/2. + (angledNeutronTubeBottomVerticalPosition + angledNeutronTubeTopVerticalPosition)/2.;
   xOffset = outerCryostatToWaterTankDistance/2. + titaniumOuterVesselWallOutsideRadius;
   yOffset = 0*mm;
	G4SubtractionSolid *tyvek_solid7 = new G4SubtractionSolid(
      "tyvek_solid7",
	   tyvek_solid6,neutronTubeTyvekHole_solid,
	   angledTubeRotation,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	//and a hole for the HV umbilical
	G4Tubs *hvConduitTyvek_solid = new G4Tubs(
			"hvConduitTyvek_solid",
			0.*cm, hvConduitRadius+1*mm,
			(waterTankRadius-acrylicOutsideRadius)/(2.*sin(hvConduitAngle)),
			0.*deg, 360.*deg );
	
	G4RotationMatrix *hvRot = new G4RotationMatrix();
	hvRot->rotateY( hvConduitAngle );
	
	zOffset = - acrylicBottomRibHeight/2. - acrylicWallOutsideHeight/2. + hvConduitOuterTitaniumVesselJointVerticalHeight + 
	( (waterTankRadius-acrylicOutsideRadius)/(2.*sin(hvConduitAngle)) + hvConduitConeHeight )*cos(hvConduitAngle);
   xOffset = - titaniumOuterVesselWallOutsideRadius - ( (waterTankRadius-acrylicOutsideRadius)/(2.*sin(hvConduitAngle)) + hvConduitConeHeight )*sin(hvConduitAngle);
   yOffset = 0*mm;
	G4SubtractionSolid *tyvek_solid8 = new G4SubtractionSolid(
      "tyvek_solid8",
	   tyvek_solid7,hvConduitTyvek_solid,
	   hvRot,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	G4LogicalVolume *tyvek_log = new G4LogicalVolume( tyvek_solid8,
			luxMaterials->Tyvek600(), "tyvek_log" );
	tyvek_log->SetVisAttributes( luxMaterials->TeflonVis() );
	
	//zOffset = -acrylicBottomRibHeight;
	zOffset = -waterTankHeight/2. + ( acrylicWallOutsideHeight + acrylicBottomRibHeight )/2. + reflectorThickness;
	LUXSimDetectorComponent *Tyvek = new LUXSimDetectorComponent(0, G4ThreeVector(0,0,zOffset), tyvek_log, "Tyvek",
				LZWaterBank->GetLogicalVolume(), 0, 0, true);
	
	// Tyvek-Water Optical Surface
	G4OpticalSurface * tyvekWaterOpSurface = new G4OpticalSurface(
			"tyvekWaterOpSurface", unified, groundfrontpainted, 
			dielectric_dielectric);
	//Set Properties
	tyvekWaterOpSurface->SetMaterialPropertiesTable( luxMaterials->Tyvek600Table() );
	//Tyvek-Water boundary surface
	G4LogicalBorderSurface *tyvekWaterSurface;
	tyvekWaterSurface = new G4LogicalBorderSurface( "tyvekWaterSurface",
			waterBank, Tyvek, tyvekWaterOpSurface);
	
	////////////////////////////////
	//The acrylic LS tanks
	////////////////////////////////
	G4Tubs *acrylicVessel_solid1 = new G4Tubs(
			"acrylicVessel_solid1",
			0, acrylicOutsideRadius,
			acrylicWallOutsideHeight/2,
			0.*deg, 360.*deg );
	
	//	Add in the bottom ribs where every other has 2 superimposed into 1 ->
    //  solid3 every 45 degrees
	G4Box *acrylicVessel_solid2 = new G4Box( "acrylicVessel_solid2",
            (acrylicOutsideRadius - titaniumOuterVesselWallOutsideRadius)/2 - 1.*mm, //to avoid overlaps
            acrylicRibThickness, acrylicBottomRibHeight/2 );
	
	
	G4Box *acrylicVessel_solid3 = new G4Box( "acrylicVessel_solid3",
            (acrylicOutsideRadius - titaniumOuterVesselWallOutsideRadius)/2 - 1.*mm, //to avoid overlaps
            acrylicRibThickness/2, acrylicBottomRibHeight/2 );
	
	G4double stand_offset = titaniumOuterVesselWallOutsideRadius +
            (acrylicOutsideRadius -titaniumOuterVesselWallOutsideRadius)/2;
	G4double stand_offset_45deg = stand_offset*sqrt(2)/2;

	xOffset = stand_offset;
	yOffset = 0;
	zOffset = -acrylicWallOutsideHeight/2 -acrylicBottomRibHeight/2;
	
	G4UnionSolid *acrylicVessel_solid4 = new G4UnionSolid(
			"acrylicVessel_solid4", acrylicVessel_solid1, acrylicVessel_solid2,
			0, G4ThreeVector(xOffset,yOffset,zOffset ) );

	xOffset = stand_offset_45deg;
	yOffset = stand_offset_45deg;
	
	G4RotationMatrix *rotnZ45 = new G4RotationMatrix();
	rotnZ45->rotateZ( -45.*deg );

	G4UnionSolid *acrylicVessel_solid5 = new G4UnionSolid(
			"acrylicVessel_solid5", acrylicVessel_solid4, acrylicVessel_solid3,
			rotnZ45, G4ThreeVector(xOffset,yOffset,zOffset) );
	xOffset = 0;
	yOffset = stand_offset;
	
	G4RotationMatrix *rotZ90 = new G4RotationMatrix();
	rotZ90->rotateZ( 90.*deg );
	G4UnionSolid *acrylicVessel_solid6 = new G4UnionSolid(
			"acrylicVessel_solid6", acrylicVessel_solid5, acrylicVessel_solid2,
			rotZ90, G4ThreeVector(xOffset,yOffset,zOffset) );

	xOffset = -stand_offset_45deg;
	yOffset = stand_offset_45deg;
	
	G4RotationMatrix *rotZ45 = new G4RotationMatrix();
	rotZ45->rotateZ( 45.*deg );
	G4UnionSolid *acrylicVessel_solid7 = new G4UnionSolid(
			"acrylicVessel_solid7", acrylicVessel_solid6, acrylicVessel_solid3,
			rotZ45, G4ThreeVector(xOffset,yOffset,zOffset) );

	xOffset = -stand_offset;
	yOffset = 0;
	G4UnionSolid *acrylicVessel_solid8 = new G4UnionSolid(
			"acrylicVessel_solid8", acrylicVessel_solid7, acrylicVessel_solid2,
			0, G4ThreeVector(xOffset,yOffset,zOffset) );

	xOffset = -stand_offset_45deg;
	yOffset = -stand_offset_45deg;
	
	G4UnionSolid *acrylicVessel_solid9 = new G4UnionSolid(
			"acrylicVessel_solid9", acrylicVessel_solid8, acrylicVessel_solid3,
			rotnZ45, G4ThreeVector(xOffset,yOffset,zOffset) );
	
	xOffset = 0;
	yOffset = -stand_offset;
	G4UnionSolid *acrylicVessel_solid10 = new G4UnionSolid(
			"acrylicVessel_solid10", acrylicVessel_solid9, acrylicVessel_solid2,
			rotZ90, G4ThreeVector(xOffset,yOffset,zOffset) );

	xOffset = stand_offset_45deg;
	yOffset = -stand_offset_45deg;
	
	G4UnionSolid *acrylicVessel_solid11 = new G4UnionSolid(
			"acrylicVessel_solid11", acrylicVessel_solid10,acrylicVessel_solid3,
			rotZ45, G4ThreeVector(xOffset,yOffset,zOffset) );

	///////////////////////////	
	//
	//	The scintillator
	//
	//  This is where we will divide this into parts where the outer ring has 
	//  4 portions and the inner ring will be divided into top and bottom.
	//  The top will be divided into 2 and the bottom into 3 with the "stand" 
	//  in the bottom tank accounted for only in the scintillator vessel.  Next
	//  iteration can properly put this in if needed
	//
	
	G4Tubs *scintillator_solid1 = new G4Tubs(
			"scintillator_solid1",
			titaniumOuterVesselFlangeOutsideRadius+acrylicWallThickness, acrylicInsideRadius,
			acrylicInsideHeight/2,
			0.*deg, 360.*deg);
	
	//	Subtract out slices of scintillator every 90 degrees for acrylic "ribs"
	G4Box *scintillator_solid2 = new G4Box( "scintillator_solid2",
            (acrylicInsideRadius - titaniumOuterVesselFlangeOutsideRadius - acrylicWallThickness)/2 + 10.*mm,
            acrylicWallThickness, acrylicWallOutsideHeight/2 );
	
        G4double scint_offset =
            (acrylicInsideRadius - titaniumOuterVesselFlangeOutsideRadius - acrylicWallThickness)/2 +
            titaniumOuterVesselFlangeOutsideRadius + acrylicWallThickness;
        
	xOffset = scint_offset;
	yOffset = 0;
	zOffset = 0;
	G4SubtractionSolid *scintillator_solid3 = new G4SubtractionSolid(
			"scintillator_solid3",
			scintillator_solid1,scintillator_solid2,
			0,G4ThreeVector(xOffset,yOffset,zOffset));	
	
	xOffset = 0;
	yOffset = scint_offset;
	G4SubtractionSolid *scintillator_solid4 = new G4SubtractionSolid(
			"scintillator_solid4",
			scintillator_solid3,scintillator_solid2,
			rotZ90,G4ThreeVector(xOffset,yOffset,zOffset));

	
	xOffset = -scint_offset;
	yOffset = 0;
	G4SubtractionSolid *scintillator_solid5 = new G4SubtractionSolid(
			"scintillator_solid5",
			scintillator_solid4,scintillator_solid2,
			0,G4ThreeVector(xOffset,yOffset,zOffset));

	xOffset = 0;
	yOffset = -scint_offset;
	G4SubtractionSolid *scintillator_solid6 = new G4SubtractionSolid(
			"scintillator_solid6",
			scintillator_solid5,scintillator_solid2,
			rotZ90,G4ThreeVector(xOffset,yOffset,zOffset));

   //scintillator_solid6 is now side tanks with no penetrations
   //add penetration of the straight neutron tube
   G4Tubs *neutronTubeHole_solid = new G4Tubs(
	   "neutronTubeHole_solid",
	   0.*cm, neutronTubeOuterRadius + acrylicWallThickness,
	   neutronTubeLength/2.,
	   0.*deg, 360.*deg );
	   
	zOffset = - acrylicWallOutsideHeight/2. + neutronTubeVerticalPosition - acrylicBottomRibHeight;
   xOffset = 0*cm;
   yOffset = - ( neutronTubeLength/2. + titaniumOuterVesselWallOutsideRadius );
   G4SubtractionSolid *scintillator_solid6b = new G4SubtractionSolid(
      "scintillator_solid6b",
	   scintillator_solid6,neutronTubeHole_solid,
	   rotation,G4ThreeVector(xOffset,yOffset,zOffset) );
   
   //now add pentration of the angled neutron tube...use the same tube solid above for subtraction
   G4Tubs *angledNeutronTubeHole_solid = new G4Tubs(
	   "angledNeutronTubeHole_solid",
	   0.*cm, neutronTubeOuterRadius + acrylicWallThickness,
	   (angledNeutronTubeLength + 5*cm)/2.,
	   0.*deg, 360.*deg );
   
   zOffset = - acrylicWallOutsideHeight/2. - acrylicBottomRibHeight + (angledNeutronTubeBottomVerticalPosition + angledNeutronTubeTopVerticalPosition)/2.;
   xOffset = outerCryostatToWaterTankDistance/2. + titaniumOuterVesselWallOutsideRadius;
   yOffset = 0*mm;
   G4SubtractionSolid *scintillator_solid6c = new G4SubtractionSolid(
      "scintillator_solid6c",
	   scintillator_solid6b,angledNeutronTubeHole_solid,
	   angledTubeRotation,G4ThreeVector(xOffset,yOffset,zOffset) );
   
   //Now add penetrations for HV Umbilical conduit
	//long skinny piece
	G4Tubs *hvConduitHole_scint_solid = new G4Tubs(
			"hvConduitHole_scint_solid",
			0.*cm, hvConduitRadius+acrylicWallThickness,
			hvConduitConeHeight + 20.*mm,
			0.*deg, 360.*deg );
   //and the cone
   G4Cons *hvConduitCone_scint_solid = new G4Cons(
	   "hvConduitCone_scint_solid",
	   0.*cm, hvConduitConeRadius + acrylicWallThickness ,
	   0.*cm, hvConduitRadius + acrylicWallThickness,
	   hvConduitConeHeight, 0.*deg, 360.*deg );
   
   xOffset = - titaniumOuterVesselWallOutsideRadius + 0.95*mm;
   yOffset = 0*mm;
   zOffset = - acrylicWallOutsideHeight/2. - acrylicBottomRibHeight + hvConduitOuterTitaniumVesselJointVerticalHeight ;
   G4SubtractionSolid *scintillator_solid6d = new G4SubtractionSolid(
			"scintillator_solid6d",
			scintillator_solid6c,hvConduitHole_scint_solid,
			hvRot,G4ThreeVector(xOffset,yOffset,zOffset) );
   
   G4SubtractionSolid *scintillator_solid6e = new G4SubtractionSolid(
			"scintillator_solid6e",
			scintillator_solid6d,hvConduitCone_scint_solid,
			hvRot,G4ThreeVector(xOffset,yOffset,zOffset) );

   
   /*
	G4LogicalVolume *scintillatorSide_log = new G4LogicalVolume(
			scintillator_solid6c, luxMaterials->DopedLABGd(),
			"scintillatorSide_log" );
	scintillatorSide_log->SetVisAttributes( luxMaterials->LABVis() );
	
	LUXSimDetectorComponent *scintillatorSide;
	scintillatorSide = new LUXSimDetectorComponent(
			0, G4ThreeVector(), scintillatorSide_log, "ScintillatorSide",
			acrylic_log, 0, 0, true );
	*/
	
	//KEEP THIS CODE HERE! -Scott Haselschwardt
	//Inserting the reflectors in the "ribs" that go between side scint. tanks
	//Done by inserting a thin layer in each rib
	//Currently we don't use these but they're here if you need/want them
	
	/*
	G4Box *side_reflector_solid = new G4Box( "side_reflector_solid",
	    (acrylicOutsideRadius - titaniumOuterVesselFlangeOutsideRadius - reflectorThickness)/2. - 1.*mm, //subtracting a bit to avoid overlaps
            reflectorThickness/2., acrylicWallOutsideHeight/2 - reflectorThickness);
	
        G4double reflec_offset =
            (acrylicOutsideRadius - titaniumOuterVesselFlangeOutsideRadius - reflectorThickness)/2 + 
            titaniumOuterVesselFlangeOutsideRadius + reflectorThickness - 1.*mm;
	xOffset = reflec_offset;
	yOffset = 0;
	zOffset = 0;
	
	G4LogicalVolume *side_reflector_log = new G4LogicalVolume(
			side_reflector_solid, luxMaterials->Tyvek600(), 
			"side_reflector_log" );
	side_reflector_log->SetVisAttributes( luxMaterials->QuartzVis() );
	
	LUXSimDetectorComponent *SideReflector1;
	SideReflector1 = new LUXSimDetectorComponent(
			0, G4ThreeVector(xOffset,yOffset,zOffset), side_reflector_log, "SideReflector1",
			acrylic_log, 0, 0, true );
		
	
	xOffset = 0;
	yOffset = reflec_offset;
	
	LUXSimDetectorComponent *SideReflector2;
	SideReflector2 = new LUXSimDetectorComponent(
			rotZ90, G4ThreeVector(xOffset,yOffset,zOffset), side_reflector_log, "SideReflector2",
			acrylic_log, 0, 0, true );
	
	
	xOffset = -reflec_offset;
	yOffset = 0;
	
	LUXSimDetectorComponent *SideReflector3;
	SideReflector3 = new LUXSimDetectorComponent(
			0, G4ThreeVector(xOffset,yOffset,zOffset), side_reflector_log, "SideReflector3",
			acrylic_log, 0, 0, true );
	
	xOffset = 0;
	yOffset = -reflec_offset;
	
	LUXSimDetectorComponent *SideReflector4;
	SideReflector4 = new LUXSimDetectorComponent(
			rotZ90, G4ThreeVector(xOffset,yOffset,zOffset), side_reflector_log, "SideReflector4",
			acrylic_log, 0, 0, true );
	
	*/
	
	//Making the center portion of the scintillator region
	G4Tubs *scintillator_solid7 = new G4Tubs(
			"scintillator_solid7",
			0,titaniumOuterVesselFlangeOutsideRadius - ( acrylicTopBottomWallThickness - reflectorThickness),
			acrylicInsideHeightCenter/2 ,
			0.*deg, 360.*deg);	
	
	G4Box *scintillator_solid8 = new G4Box( "scintillator_solid8",
            (0.5*(titaniumOuterVesselFlangeOutsideRadius + acrylicWallThickness)),
            acrylicTopBottomWallThickness - reflectorThickness, 0.5*(acrylicInsideHeightCenter/2) ); 

	G4double bot_scint_offset = 0.5*(titaniumOuterVesselFlangeOutsideRadius +
            acrylicWallThickness);

	G4SubtractionSolid *scintillator_solid9 = new G4SubtractionSolid(
			"scintillator_solid9",
			scintillator_solid7,scintillator_solid8,
			0,G4ThreeVector(-bot_scint_offset,0,-0.5*(acrylicInsideHeightCenter/2)));
	
	xOffset = 0.5*bot_scint_offset;
	yOffset = sqrt(3)*bot_scint_offset/2;

	G4RotationMatrix *rotZ120 = new G4RotationMatrix();
	rotZ120->rotateZ( 120.*deg );
	
	G4SubtractionSolid *scintillator_solid10 = new G4SubtractionSolid(
            "scintillator_solid10", scintillator_solid9, scintillator_solid8,
			rotZ120,
            G4ThreeVector(xOffset,yOffset,-0.5*(acrylicInsideHeightCenter/2)));

	xOffset = 0.5*bot_scint_offset;
	yOffset = -sqrt(3)*bot_scint_offset/2;

	G4RotationMatrix *rotZ240 = new G4RotationMatrix();
	rotZ240->rotateZ( 240.*deg );

	G4SubtractionSolid *scintillator_solid11 = new G4SubtractionSolid(
			"scintillator_solid11", scintillator_solid10,scintillator_solid8,
			rotZ240,
            G4ThreeVector(xOffset,yOffset,-0.5*(acrylicInsideHeightCenter/2)));

	G4Box *scintillator_solid12 = new G4Box( "scintillator_solid12",
            titaniumOuterVesselFlangeOutsideRadius + acrylicWallThickness,
            acrylicTopBottomWallThickness - reflectorThickness, (acrylicInsideHeightCenter/2) );
	
	G4SubtractionSolid *scintillator_solid13 = new G4SubtractionSolid(
			"scintillator_solid13",
			scintillator_solid11,scintillator_solid12,
			0,G4ThreeVector(0,0,acrylicInsideHeightCenter/2)); 
	
   
	// This is to make the outside boundary of the Titanium cryostat
	//	Combine the main vessel section with the flange
	G4Tubs *scintTitaniumBoundary_solid3 = new G4Tubs(
			"scintTitaniumBoundary_solid3",
			0.*cm, titaniumOuterVesselFlangeOutsideRadius + acrylicTopBottomWallThickness,
			titaniumOuterVesselStraightWallHeight/2 + /* titaniumOuterVesselFlangeHeight/2 + */ acrylicTopBottomWallThickness,
			0.*deg, 360.*deg );

	
	G4Ellipsoid *scintTitaniumBoundary_solid4 = new G4Ellipsoid(
			"scintTitaniumBoundary_solid4",
			titaniumOuterVesselWallOutsideRadius + acrylicTopBottomWallThickness,
			titaniumOuterVesselWallOutsideRadius + acrylicTopBottomWallThickness,
			titaniumOuterVesselBottomOutsideSemiMajorZHeight + acrylicTopBottomWallThickness/2.,
			-titaniumOuterVesselBottomOutsideSemiMajorZHeight - acrylicTopBottomWallThickness/2,
			0.*cm );
			
	zOffset = -titaniumOuterVesselStraightWallHeight/2 /*- titaniumOuterVesselFlangeHeight/2*/ - acrylicTopBottomWallThickness/2;
            
	G4UnionSolid *scintTitaniumBoundary_solid5 = new G4UnionSolid(
			"scintTitaniumBoundary_solid5",
			scintTitaniumBoundary_solid3, scintTitaniumBoundary_solid4,
			0, G4ThreeVector( 0, 0, zOffset ) );
	
	//	Combine in the top dome shape
	G4Ellipsoid *scintTitaniumBoundary_solid6 = new G4Ellipsoid(
			"scintTitaniumBoundary_solid6",
			titaniumOuterVesselWallOutsideRadius + acrylicTopBottomWallThickness,
			titaniumOuterVesselWallOutsideRadius + acrylicTopBottomWallThickness,
			titaniumOuterVesselTopOutsideSemiMajorZHeight + acrylicTopBottomWallThickness/2,
			0.*cm,
			titaniumOuterVesselTopOutsideSemiMajorZHeight + acrylicTopBottomWallThickness/2);
			
	zOffset = titaniumOuterVesselStraightWallHeight/2 /*+ titaniumOuterVesselFlangeHeight/2*/ +  acrylicTopBottomWallThickness/2;
	
	G4UnionSolid *scintTitaniumBoundary_solid7 = new G4UnionSolid(
			"scintTitaniumBoundary_solid7",
			scintTitaniumBoundary_solid5, scintTitaniumBoundary_solid6,
			0, G4ThreeVector(0,0,zOffset) );
	
	G4SubtractionSolid *scintillator_solid14 = new G4SubtractionSolid(
			"scintillator_solid14",
			scintillator_solid13,scintTitaniumBoundary_solid7, 
			0,G4ThreeVector(0,0,-OuterTitaniumVesselVerticalOffset  )); //was (0,0,0)
   
   //scintillator_solid14 is now the top and bottom tanks with no penetrations
   //now we'll add the three holes in the top tank for the two conduits and the YBe source
   //and the one hole in the center of the bottom tank for the circ conduits
   //also the neutron tube
   
   //The five solids for subtraction:
   G4Tubs *pmtConduitHole_solid = new G4Tubs(
	   "pmtConduitHole_solid",
	   0.*cm, pmtConduitHoleRadius + (acrylicTopBottomWallThickness-reflectorThickness),
	   548.*mm, //this number is arbitary since this is just a solid used for subtraction
	   0.*deg, 360.*deg );
			
   G4Tubs *thermoConduitHole_solid = new G4Tubs(
	   "thermoConduitHold_solid",
	   0.*cm, thermoConduitHoleRadius + (acrylicTopBottomWallThickness-reflectorThickness),
	   548.*mm, //this number is arbitary since this is just a solid used for subtraction
	   0.*deg, 360.*deg );
       
   G4Tubs *yBeHole_solid = new G4Tubs(
	   "yBeHole_solid",
	   0.*cm, yBeHoleRadius + (acrylicTopBottomWallThickness-reflectorThickness),
	   548.*mm, //this number is arbitary since this is just a solid used for subtraction
	   0.*deg, 360.*deg );
       
   G4Tubs *bottomCenterHole_solid = new G4Tubs(
	   "bottomCenterHole_solid",
	   0.*cm, bottomLSTankCenterHoleRadius + (acrylicTopBottomWallThickness-reflectorThickness),
	   380.*mm, //this number is arbitary since this is just a solid used for subtraction
	   0.*deg, 360.*deg );
	
	//tubs + box for the source tube holes
	G4Tubs *sourceTubeHole_solid = new G4Tubs(
	   "sourceTubeHole_solid",
	   0.*cm, sourceTubePenetrationRadius + acrylicWallThickness,
	   548.*mm, //this number is arbitary since this is just a solid used for subtraction
	   180.*deg, 180.*deg );
	
	G4Box *sourceTubeHole_solid2 = new G4Box(
	   "sourceTubeHole_solid2",
      sourceTubePenetrationRadius + acrylicWallThickness, sourceTubePenetrationRadius/2.  + acrylicWallThickness/2., 548.*mm );
	
	yOffset = sourceTubePenetrationRadius/2. + acrylicWallThickness/2. - 2.*mm; //proper overlap
	G4UnionSolid *sourceTubeHole_solid3 = new G4UnionSolid(
			"sourceTubeHole_solid3",
			sourceTubeHole_solid, sourceTubeHole_solid2,
			0, G4ThreeVector(0,yOffset,0) );
   
   //First the bottom subtraction:
   zOffset = -acrylicWallOutsideHeight/2. + 380.*mm;
   G4SubtractionSolid *scintillator_solid15 = new G4SubtractionSolid(
	   "scintillator_solid15",
	   scintillator_solid14,bottomCenterHole_solid,
	   0,G4ThreeVector(0,0,zOffset) );
       
   //Next the YBe source Hole
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   G4SubtractionSolid *scintillator_solid16 = new G4SubtractionSolid(
	   "scintillator_solid16",
	   scintillator_solid15,yBeHole_solid,
	   0,G4ThreeVector(0,0,zOffset) );
			
   //Then the two conduits
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = pmtConduitLSTankPosition;
   G4SubtractionSolid *scintillator_solid17 = new G4SubtractionSolid(
	   "scintillator_solid17",
	   scintillator_solid16,pmtConduitHole_solid,
	   0,G4ThreeVector(xOffset,0,zOffset) );
       
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = -thermoConduitLSTankPosition;
   G4SubtractionSolid *scintillator_solid18 = new G4SubtractionSolid(
      "scintillator_solid18",
	   scintillator_solid17,thermoConduitHole_solid,
	   0,G4ThreeVector(xOffset,0,zOffset) );
	
	//then source tube holes 120*deg apart
	
	zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = 0.*mm;
   yOffset = sourceTubePenetrationPosition;
   G4SubtractionSolid *scintillator_solid19 = new G4SubtractionSolid(
      "scintillator_solid19",
	   scintillator_solid18,sourceTubeHole_solid3,
	   0,G4ThreeVector(xOffset,yOffset,zOffset) );
	   
	zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = (sqrt(3)/2.)*sourceTubePenetrationPosition;
   yOffset = - sourceTubePenetrationPosition/2.;
   G4SubtractionSolid *scintillator_solid20 = new G4SubtractionSolid(
      "scintillator_solid20",
	   scintillator_solid19,sourceTubeHole_solid3,
	   rotZ120,G4ThreeVector(xOffset,yOffset,zOffset) );
	
   
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = - (sqrt(3)/2.)*sourceTubePenetrationPosition;
   yOffset = - sourceTubePenetrationPosition/2.;
   G4SubtractionSolid *scintillator_solid21 = new G4SubtractionSolid(
      "scintillator_solid21",
	   scintillator_solid20,sourceTubeHole_solid3,
	   rotZ240,G4ThreeVector(xOffset,yOffset,zOffset) );
   
   //Combine (or don't) the side LS with top and bottom LS --------------------------------------
   
   G4UnionSolid *big_scintillator_solid = new G4UnionSolid(
	   "big_scintillator_solid",
		scintillator_solid6e, scintillator_solid21, //combines the side tanks w/ center tanks
		0, G4ThreeVector(0,0,0) );
      
   G4LogicalVolume *scintillatorCenter_log = new G4LogicalVolume(
		big_scintillator_solid, luxMaterials->DopedLABGd(), //DopedLABGd()
        "scintillatorCenter_log" );
   scintillatorCenter_log->SetVisAttributes( luxMaterials->LABVis() );
   //------------------------------------------------------------------------------------------
     /*  
	G4LogicalVolume *scintillatorCenter_log = new G4LogicalVolume( scintillator_solid21, luxMaterials->DopedLABGd(), "scintillatorCenter_log" );
	scintillatorCenter_log->SetVisAttributes( luxMaterials->LABVis() );
	
	LUXSimDetectorComponent *scintillatorCenter;
	scintillatorCenter = new LUXSimDetectorComponent(
			0, G4ThreeVector(), scintillatorCenter_log, "ScintillatorCenter",
			acrylic_log, 0, 0, true );
   */
	////////////////////////////////////
	////////////////////////////////////
	//	The Outer Titanium Vessel
	//////////////////////////////////
	////////////////////////////////////
	
	//	Combine the main vessel section with the flange
	G4Tubs *outerTitaniumVessel_solid1 = new G4Tubs(
			"outerTitaniumVessel_solid1",
			0.*cm, titaniumOuterVesselWallOutsideRadius,
			titaniumOuterVesselStraightWallHeight/2,
			0.*deg, 360.*deg );
	G4Tubs *outerTitaniumVessel_solid2 = new G4Tubs(
			"outerTitaniumVessel_solid2",
			0.*cm, titaniumOuterVesselFlangeOutsideRadius,
			titaniumOuterVesselFlangeHeight/2,
			0.*deg, 360.*deg );
	zOffset = titaniumOuterVesselStraightWallHeight/2
			- titaniumOuterVesselFlangeHeight/2;
	G4UnionSolid *outerTitaniumVessel_solid3a = new G4UnionSolid(
			"outerTitaniumVessel_solid3a",
			outerTitaniumVessel_solid1, outerTitaniumVessel_solid2,
			0, G4ThreeVector(0,0,zOffset) );
	
	//adding bottom flange area
	zOffset = -titaniumOuterVesselBottomFlangeLocation;
	
	G4UnionSolid *outerTitaniumVessel_solid3 = new G4UnionSolid(
			"outerTitaniumVessel_solid3",
			outerTitaniumVessel_solid3a, outerTitaniumVessel_solid2,
			0, G4ThreeVector(0,0,zOffset) );
	

	//	Combine in the bottom dome shape
	G4Ellipsoid *outerTitaniumVessel_solid4 = new G4Ellipsoid(
			"outerTitaniumVessel_solid4",
			titaniumOuterVesselWallOutsideRadius,
			titaniumOuterVesselWallOutsideRadius,
			titaniumOuterVesselBottomOutsideSemiMajorZHeight,
			-titaniumOuterVesselBottomOutsideSemiMajorZHeight,
			0.*cm );
	zOffset = -titaniumOuterVesselStraightWallHeight/2;
	G4UnionSolid *outerTitaniumVessel_solid5 = new G4UnionSolid(
			"outerTitaniumVessel_solid5",
			outerTitaniumVessel_solid3, outerTitaniumVessel_solid4,
			0, G4ThreeVector( 0, 0, zOffset ) );
	
	//	Combine in the top dome shape
	G4Ellipsoid *outerTitaniumVessel_solid6 = new G4Ellipsoid(
			"outerTitaniumVessel_solid6",
			titaniumOuterVesselWallOutsideRadius,
			titaniumOuterVesselWallOutsideRadius,
			titaniumOuterVesselTopOutsideSemiMajorZHeight,
			0.*cm,
			titaniumOuterVesselTopOutsideSemiMajorZHeight );
	zOffset = titaniumOuterVesselStraightWallHeight/2;
	G4UnionSolid *outerTitaniumVessel_solid7 = new G4UnionSolid(
			"outerTitaniumVessel_solid7",
			outerTitaniumVessel_solid5, outerTitaniumVessel_solid6,
			0, G4ThreeVector(0,0,zOffset) );
	
	G4LogicalVolume *outerTitaniumVessel_log = new G4LogicalVolume(
			outerTitaniumVessel_solid7, luxMaterials->Titanium(),
			"outerTitaniumVessel_log" );
	outerTitaniumVessel_log->SetVisAttributes( luxMaterials->TitaniumVis() );
   
   //before placing cryostat solid, subtract its shape from the acrylic and subtract
   //out the conduit holes in the acrylic
   
   zOffset = -OuterTitaniumVesselVerticalOffset;
   G4SubtractionSolid *acrylicVessel_solid12 = new G4SubtractionSolid(
	   "acrylicVessel_solid12",
	   acrylicVessel_solid11, outerTitaniumVessel_solid7,
	   0,G4ThreeVector(0,0,zOffset) );
   
   //Now the conduit holes
   //The solids for subtraction:
   G4Tubs *pmtConduitHole_solid2 = new G4Tubs(
	   "pmtConduitHole_solid2",
	   0.*cm, pmtConduitHoleRadius, 
	   548.*mm, //this number is arbitary since this is just a solid used for subtraction
	   0.*deg, 360.*deg );
			
   G4Tubs *thermoConduitHole_solid2 = new G4Tubs(
	   "thermoConduitHold_solid2",
	   0.*cm, thermoConduitHoleRadius, 
	   548.*mm, //this number is arbitary since this is just a solid used for subtraction
	   0.*deg, 360.*deg );
       
   G4Tubs *yBeHole_solid2 = new G4Tubs(
	   "yBeHole_solid2",
	   0.*cm, yBeHoleRadius,
	   548.*mm, //this number is arbitary since this is just a solid used for subtraction
	   0.*deg, 360.*deg );
       
   G4Tubs *bottomCenterHole_solid2 = new G4Tubs(
	   "bottomCenterHole_solid2",
	   0.*cm, bottomLSTankCenterHoleRadius,
	   385.*mm, //this number is arbitary since this is just a solid used for subtraction
	   0.*deg, 360.*deg );
	   
	G4Tubs *neutronTubeHole_solid2 = new G4Tubs(
	   "neutronTubeHole_solid2",
	   0.*cm, neutronTubeOuterRadius,
	   neutronTubeLength/2.,
	   0.*deg, 360.*deg );
  
  G4Tubs *angledNeutronTubeHole_solid2 = new G4Tubs(
	   "angledNeutronTubeHole_solid2",
	   0.*cm, neutronTubeOuterRadius,
	   (angledNeutronTubeLength)/2. + 20.*cm,
	   0.*deg, 360.*deg );
	   
  //tubs + box for the source tube holes
	G4Tubs *sourceTubeHole2_solid = new G4Tubs(
	   "sourceTubeHole2_solid",
	   0.*cm, sourceTubePenetrationRadius,
	   548.*mm, //this number is arbitary since this is just a solid used for subtraction
	   180.*deg, 180.*deg );
	
	G4Box *sourceTubeHole2_solid2 = new G4Box(
	   "sourceTubeHole2_solid2",
      sourceTubePenetrationRadius, sourceTubePenetrationRadius/2., 548.*mm );
	
	yOffset = sourceTubePenetrationRadius/2.; //proper overlap
	G4UnionSolid *sourceTubeHole2_solid3 = new G4UnionSolid(
			"sourceTubeHole2_solid3",
			sourceTubeHole2_solid, sourceTubeHole2_solid2,
			0, G4ThreeVector(0,yOffset,0) );
	
	//Cone and long skinny piece for HV umbilical
	 //Make a big cylinder to subtract cryo shape from cone:
	G4Tubs *cryo_solid = new G4Tubs(
			"cryo_solid",0.*cm, titaniumOuterVesselWallOutsideRadius,
			titaniumOuterVesselStraightWallHeight*10, //point is to make it long, this is arbitrary
			0.*deg, 360.*deg );
	//The cone
	G4Cons *cone_solid = new G4Cons(
	   "cone_solid",
	   0.*cm, hvConduitConeRadius,
	   0.*cm, hvConduitRadius,
	   hvConduitConeHeight, 0.*deg, 360.*deg );
	
	G4RotationMatrix *hvNegRot = new G4RotationMatrix();
	hvNegRot->rotateY( -hvConduitAngle );	
	xOffset = 1842*mm + 100*mm; //really hacking it here... this gets the length of the cone right
	yOffset = 0*mm;
	zOffset = 0*mm;
	G4SubtractionSolid *hvConduitHole_solid2 = new G4SubtractionSolid(
	   "hvConduitHole_solid2",
	   cone_solid,cryo_solid,
	   hvNegRot,G4ThreeVector(xOffset,yOffset,zOffset) );
	 
	//and a skinny piece to be used later
	G4Tubs *hvConduitHole_tank_solid = new G4Tubs(
			"hvConduitHole_tank_solid",
			0.*cm, hvConduitRadius,
			hvConduitConeHeight + 20.*mm,
			0.*deg, 360.*deg );
	 
	//Now do all the subtractions--------------------
	
   //First the bottom subtraction:
   zOffset = - acrylicWallOutsideHeight/2. + 385.*mm;
   G4SubtractionSolid *acrylicVessel_solid13 = new G4SubtractionSolid(
	   "acrylicVessel_solid13",
	   acrylicVessel_solid12,bottomCenterHole_solid2,
	   0,G4ThreeVector(0,0,zOffset) );
       
   //Next the YBe source Hole
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   G4SubtractionSolid *acrylicVessel_solid14 = new G4SubtractionSolid(
	   "acrylicVessel_solid14",
	   acrylicVessel_solid13,yBeHole_solid2,
	   0,G4ThreeVector(0,0,zOffset) );
			
   //Then the two conduits
   zOffset = acrylicWallOutsideHeight/2. - 540.*mm;
   xOffset = pmtConduitLSTankPosition;
   G4SubtractionSolid *acrylicVessel_solid15 = new G4SubtractionSolid(
	   "acrylicVessel_solid15",
	   acrylicVessel_solid14,pmtConduitHole_solid2,
	   0,G4ThreeVector(xOffset,0,zOffset) );
       
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = -thermoConduitLSTankPosition;
   G4SubtractionSolid *acrylicVessel_solid16 = new G4SubtractionSolid(
      "acrylicVessel_solid16",
	   acrylicVessel_solid15,thermoConduitHole_solid2,
	   0,G4ThreeVector(xOffset,0,zOffset) );
   
   //Then the straight neutron tube
   zOffset = - acrylicBottomRibHeight - acrylicWallOutsideHeight/2. + neutronTubeVerticalPosition;
   xOffset = 0*cm;
   yOffset = - (  neutronTubeLength/2. + titaniumOuterVesselWallOutsideRadius - 10.*mm );
   G4SubtractionSolid *acrylicVessel_solid17 = new G4SubtractionSolid(
      "acrylicVessel_solid17",
	   acrylicVessel_solid16,neutronTubeHole_solid2,
	   rotation,G4ThreeVector(xOffset,yOffset,zOffset) );
   
   //Then the angled neutron tube
   zOffset = - acrylicWallOutsideHeight/2. - acrylicBottomRibHeight + (angledNeutronTubeBottomVerticalPosition + angledNeutronTubeTopVerticalPosition)/2.;
   xOffset = outerCryostatToWaterTankDistance/2. + titaniumOuterVesselWallOutsideRadius;
   yOffset = 0*mm;
   G4SubtractionSolid *acrylicVessel_solid18 = new G4SubtractionSolid(
      "acrylicVessel_solid18",
	   acrylicVessel_solid17,angledNeutronTubeHole_solid2,
	   angledTubeRotation,G4ThreeVector(xOffset,yOffset,zOffset) );
   
   //then the source tube holes
	zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = 0.*mm;
   yOffset = sourceTubePenetrationPosition;
   G4SubtractionSolid *acrylicVessel_solid19 = new G4SubtractionSolid(
      "acrylicVessel_solid19",
	   acrylicVessel_solid18,sourceTubeHole2_solid3,
	   0,G4ThreeVector(xOffset,yOffset,zOffset) );
	   
	zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = (sqrt(3)/2)*sourceTubePenetrationPosition;
   yOffset = - sourceTubePenetrationPosition/2;
   G4SubtractionSolid *acrylicVessel_solid20 = new G4SubtractionSolid(
      "acrylicVessel_solid20",
	   acrylicVessel_solid19,sourceTubeHole2_solid3,
	   rotZ120,G4ThreeVector(xOffset,yOffset,zOffset) );
	
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = - (sqrt(3)/2)*sourceTubePenetrationPosition;
   yOffset = - sourceTubePenetrationPosition/2;
   G4SubtractionSolid *acrylicVessel_solid21 = new G4SubtractionSolid(
      "acrylicVessel_solid21",
	   acrylicVessel_solid20,sourceTubeHole2_solid3,
	   rotZ240,G4ThreeVector(xOffset,yOffset,zOffset) );
   
   //Now the HV umbilical shape
   xOffset = - titaniumOuterVesselWallOutsideRadius  + 0.95*mm;
   yOffset = 0*mm;
   zOffset = - acrylicWallOutsideHeight/2. - acrylicBottomRibHeight + hvConduitOuterTitaniumVesselJointVerticalHeight;
   G4SubtractionSolid *acrylicVessel_solid22 = new G4SubtractionSolid(
      "acrylicVessel_solid22",
	   acrylicVessel_solid21,hvConduitHole_tank_solid,
	   hvRot,G4ThreeVector(xOffset,yOffset,zOffset) );
   
   G4SubtractionSolid *acrylicVessel_solid22b = new G4SubtractionSolid(
      "acrylicVessel_solid22b",
	   acrylicVessel_solid22,hvConduitHole_solid2 , 
	   hvRot,G4ThreeVector(xOffset,yOffset,zOffset) );
   
   //Add back the titanium outline so that acrylic_log is still the mother
   //volume for the outer cyrostat
   zOffset = -OuterTitaniumVesselVerticalOffset;
   G4UnionSolid *acrylicVessel_solid23 = new G4UnionSolid(
	   "acrylicVessel_solid23",
	   acrylicVessel_solid22b, outerTitaniumVessel_solid7, 
	   0,G4ThreeVector(0,0,zOffset) );
   
   //make the logical volume for acrylic
   G4LogicalVolume *acrylic_log = new G4LogicalVolume(  
			acrylicVessel_solid23, luxMaterials->Acrylic(), 
            		"acrylic_log" );
	acrylic_log->SetVisAttributes( luxMaterials->AcrylicVis() );
   
   //Now make the scintillator volume
   LUXSimDetectorComponent *scintillatorCenter;
	scintillatorCenter = new LUXSimDetectorComponent(
			0, G4ThreeVector(), scintillatorCenter_log, "ScintillatorCenter",
			acrylic_log, 0, 0, true );
   
   
   //Now place the outer cryostat
	//zOffset = -waterTankHeight/2. + (acrylicWallOutsideHeight+acrylicBottomRibHeight)/2. - OuterTitaniumVesselVerticalOffset + 155.*mm;
	//zOffset = -waterTankHeight/2. + acrylicWallOutsideHeight + acrylicBottomRibHeight - OuterTitaniumVesselVerticalOffset;
	
	zOffset = -OuterTitaniumVesselVerticalOffset;
	LUXSimDetectorComponent *outerTitaniumVessel;
	outerTitaniumVessel = new LUXSimDetectorComponent(
            0, G4ThreeVector(0,0,zOffset), outerTitaniumVessel_log,
            "OuterTitaniumVessel", acrylic_log, 0, 0, true );
            
            
	///////////////////
	//Foam Displacer
	////////////////
	
	//Make a shell for the foam displacer and subtract out slices where the flanges of 
	//the cyrostat fit in
	G4Tubs *displacer_solid = new G4Tubs(
			"displacer_solid",
			titaniumOuterVesselWallOutsideRadius,
			titaniumOuterVesselFlangeOutsideRadius,
			titaniumOuterVesselStraightWallHeight/2,
			0.*deg, 360.*deg);
			
	G4Tubs *flangesub_solid = new G4Tubs(
			"flangesub_solid",
			0.*cm, titaniumOuterVesselFlangeOutsideRadius + 5.*mm,
			titaniumOuterVesselFlangeHeight/2,
			0.*deg, 360.*deg );
	//substract the top flange
	zOffset = titaniumOuterVesselStraightWallHeight/2
			- titaniumOuterVesselFlangeHeight/2;
	G4SubtractionSolid *displacer_solid2 = new G4SubtractionSolid(
			"displacer_solid2",
			displacer_solid,flangesub_solid,
			0,G4ThreeVector(0,0,zOffset));
	
	//substract the bottom flange
	zOffset = -titaniumOuterVesselBottomFlangeLocation;
	G4SubtractionSolid *displacer_solid3 = new G4SubtractionSolid(
			"displacer_solid3",
			displacer_solid2,flangesub_solid,
			0,G4ThreeVector(0,0,zOffset));
	
	//subtract hole for straight neutron tube
	zOffset = - acrylicBottomRibHeight - acrylicWallOutsideHeight/2. + neutronTubeVerticalPosition + OuterTitaniumVesselVerticalOffset;
	xOffset = 0.*mm;
	yOffset = - ( neutronTubeLength/2. + titaniumOuterVesselWallOutsideRadius/2. );
	G4SubtractionSolid *displacer_solid4 = new G4SubtractionSolid(
			"displacer_solid4",
			displacer_solid3,neutronTubeHole_solid2,
			rotation,G4ThreeVector(xOffset,yOffset,zOffset));
	
	//and for the angled neutron tube
	zOffset = - acrylicBottomRibHeight - acrylicWallOutsideHeight/2. + OuterTitaniumVesselVerticalOffset + 
	(angledNeutronTubeBottomVerticalPosition + angledNeutronTubeTopVerticalPosition)/2.;
   xOffset = outerCryostatToWaterTankDistance/2. + titaniumOuterVesselWallOutsideRadius;
   yOffset = 0*mm;;
	G4SubtractionSolid *displacer_solid5 = new G4SubtractionSolid(
			"displacer_solid5",
			displacer_solid4,angledNeutronTubeHole_solid2,
			angledTubeRotation,G4ThreeVector(xOffset,yOffset,zOffset));
	
	//subtract a hole for the HV Umbilical
	//Cone
	G4Cons *hvConduitHole_solid3 = new G4Cons(
	   "hvConduitHole_solid3",
	   0.*cm, hvConduitConeRadius ,
	   0.*cm, hvConduitRadius ,
	   hvConduitConeHeight, 0.*deg, 360.*deg );
	
	xOffset = - titaniumOuterVesselWallOutsideRadius + 0.95*mm;
   yOffset = 0*mm;
   zOffset = - acrylicWallOutsideHeight/2. - acrylicBottomRibHeight + hvConduitOuterTitaniumVesselJointVerticalHeight + OuterTitaniumVesselVerticalOffset;
	G4SubtractionSolid *displacer_solid6 = new G4SubtractionSolid(
			"displacer_solid6",
			displacer_solid5,hvConduitHole_solid3,
			hvRot,G4ThreeVector(xOffset,yOffset,zOffset));
	
	G4LogicalVolume *displacer_log = new G4LogicalVolume(
			displacer_solid6, luxMaterials->PolyFoam(), 
            		"displacer_log" );
	displacer_log->SetVisAttributes( luxMaterials->WaterVis() ); 
	
	zOffset = -OuterTitaniumVesselVerticalOffset; //was 0.*mm;
	
	LUXSimDetectorComponent *FoamDisplacer;
	FoamDisplacer = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), displacer_log, "FoamDisplacer",
			acrylic_log, 0, 0, true );
   	
			
	///////////////////
	//Reflectors
	///////////////////
	
	//Add in a reflector between scintillator and foam displacer
	G4Tubs *centerReflector_solid = new G4Tubs(
			"centerReflector_solid",
			titaniumOuterVesselFlangeOutsideRadius,
			titaniumOuterVesselFlangeOutsideRadius + reflectorThickness,
			titaniumOuterVesselStraightWallHeight/2,
			0.*deg, 360.*deg);
	
	//subtract hole for straight neutron tube
	zOffset = - acrylicBottomRibHeight - acrylicWallOutsideHeight/2. + neutronTubeVerticalPosition + OuterTitaniumVesselVerticalOffset;
	xOffset = 0.*mm;
	yOffset = - (  neutronTubeLength/2. + titaniumOuterVesselWallOutsideRadius );
	G4SubtractionSolid *centerReflector_solid2 = new G4SubtractionSolid(
			"centerReflector_solid2",
			centerReflector_solid,neutronTubeHole_solid2,
			rotation,G4ThreeVector(xOffset,yOffset,zOffset));
	
	//and for angled neutron tube
	zOffset = - acrylicBottomRibHeight - acrylicWallOutsideHeight/2. + OuterTitaniumVesselVerticalOffset + 
	(angledNeutronTubeBottomVerticalPosition + angledNeutronTubeTopVerticalPosition)/2.;
   xOffset = outerCryostatToWaterTankDistance/2. + titaniumOuterVesselWallOutsideRadius;
   yOffset = 0*mm;
	G4SubtractionSolid *centerReflector_solid3 = new G4SubtractionSolid(
			"centerReflector_solid3",
			centerReflector_solid2,angledNeutronTubeHole_solid2,
			angledTubeRotation,G4ThreeVector(xOffset,yOffset,zOffset));
	
	//and then for the hv umbilical
	//Cone
	
	G4Cons *hvConduitHole_solid4 = new G4Cons(
	   "hvConduitHole_solid4",
	   0.*cm, hvConduitConeRadius,
	   0.*cm, hvConduitRadius,
	   hvConduitConeHeight, 0.*deg, 360.*deg );
	
	xOffset = - titaniumOuterVesselWallOutsideRadius + 0.95*mm;
   yOffset = 0*mm;
   zOffset = - acrylicWallOutsideHeight/2. - acrylicBottomRibHeight + hvConduitOuterTitaniumVesselJointVerticalHeight + OuterTitaniumVesselVerticalOffset;
	G4SubtractionSolid *centerReflector_solid4 = new G4SubtractionSolid(
			"centerReflector_solid4",
			centerReflector_solid3,hvConduitHole_solid4,
			hvRot,G4ThreeVector(xOffset,yOffset,zOffset));
	
	
	G4LogicalVolume *centerReflector_log = new G4LogicalVolume(
			centerReflector_solid4, luxMaterials->LZSpecRef(),
            		"centerReflector_log" );
	centerReflector_log->SetVisAttributes( luxMaterials->QuartzVis() ); //Needs to be changed
	
	zOffset = -OuterTitaniumVesselVerticalOffset;
	
	LUXSimDetectorComponent *CenterReflector;
	CenterReflector = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), centerReflector_log, "CenterReflector",
			acrylic_log, 0, 0, true );
	
	
	//Add reflector over the top and bottom domes
	//make ellipsoid and subtract out the shape of top and bottom domes leaving a thin shell
	//top first
	G4Ellipsoid *topReflector_solid = new G4Ellipsoid(
			"topReflector_solid",
			titaniumOuterVesselWallOutsideRadius + reflectorThickness/2,
			titaniumOuterVesselWallOutsideRadius + reflectorThickness/2,
			titaniumOuterVesselTopOutsideSemiMajorZHeight + reflectorThickness,
			0.*cm,
			titaniumOuterVesselTopOutsideSemiMajorZHeight + reflectorThickness );
	
	zOffset = 0*cm; //where to do the subtraction
	G4SubtractionSolid *topReflector_solid2 = new G4SubtractionSolid(
			"topReflector_solid2",
			topReflector_solid,outerTitaniumVessel_solid6,
			0,G4ThreeVector(0,0,zOffset));
			
	//add in piece to cover the top of the flange
	G4Tubs *topReflector_solid3 = new G4Tubs(
			"topReflector_solid3",
			titaniumOuterVesselWallOutsideRadius + reflectorThickness/2,
			titaniumOuterVesselFlangeOutsideRadius + reflectorThickness,
			reflectorThickness/2,
			0.*deg, 360.*deg);
	
	zOffset = reflectorThickness/2;
	G4UnionSolid *topReflector_solid4 = new G4UnionSolid(
			"topReflector_solid4",
			topReflector_solid2, topReflector_solid3,
			0, G4ThreeVector(0,0,zOffset) );
	
	//Now subtract out the holes for the conduits    
   //The YBe source Hole
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   G4SubtractionSolid *topReflector_solid5 = new G4SubtractionSolid(
	   "topReflector_solid5",
	   topReflector_solid4,yBeHole_solid2,
	   0,G4ThreeVector(0,0,0) ); //was zOffset
			
   //Then the two conduits
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = pmtConduitLSTankPosition;
   G4SubtractionSolid *topReflector_solid6 = new G4SubtractionSolid(
	   "topReflector_solid6",
	   topReflector_solid5,pmtConduitHole_solid2,
	   0,G4ThreeVector(xOffset,0,0) ); //was zOffset
       
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = -thermoConduitLSTankPosition;
   G4SubtractionSolid *topReflector_solid7 = new G4SubtractionSolid(
      "topReflector_solid7",
	   topReflector_solid6,thermoConduitHole_solid2,
	   0,G4ThreeVector(xOffset,0,0) ); //was zOffset
	
	//subtract source tube holes: this is to avoid overlaps for the time being
	//then the source tube holes
	zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = 0.*mm;
   yOffset = sourceTubePenetrationPosition;
   G4SubtractionSolid *topReflector_solid8 = new G4SubtractionSolid(
      "topReflector_solid8",
	   topReflector_solid7,sourceTubeHole2_solid3,
	   0,G4ThreeVector(xOffset,yOffset,0) );
	   
	zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = (sqrt(3)/2)*sourceTubePenetrationPosition;
   yOffset = - sourceTubePenetrationPosition/2;
   G4SubtractionSolid *topReflector_solid9 = new G4SubtractionSolid(
      "topReflector_solid9",
	   topReflector_solid8,sourceTubeHole2_solid3,
	   rotZ120,G4ThreeVector(xOffset,yOffset,0) );
	
   
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = - (sqrt(3)/2)*sourceTubePenetrationPosition;
   yOffset = - sourceTubePenetrationPosition/2;
   G4SubtractionSolid *topReflector_solid10 = new G4SubtractionSolid(
      "topReflector_solid10",
	   topReflector_solid9,sourceTubeHole2_solid3,
	   rotZ240,G4ThreeVector(xOffset,yOffset,0) );
	
	
	G4LogicalVolume *topReflector_log = new G4LogicalVolume(
			topReflector_solid10, luxMaterials->LZSpecRef(),
            		"topReflector_log" );
	topReflector_log->SetVisAttributes( luxMaterials->QuartzVis() );
	
	zOffset = titaniumOuterVesselStraightWallHeight/2 - OuterTitaniumVesselVerticalOffset; 
	
	LUXSimDetectorComponent *TopReflector;
	TopReflector = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), topReflector_log, "TopReflector",
			acrylic_log, 0, 0, true );
	
	//bottom next
	G4Ellipsoid *bottomReflector_solid = new G4Ellipsoid(
			"bottomReflector_solid",
			titaniumOuterVesselWallOutsideRadius + reflectorThickness/2,
			titaniumOuterVesselWallOutsideRadius + reflectorThickness/2,
			titaniumOuterVesselBottomOutsideSemiMajorZHeight + reflectorThickness,
			-titaniumOuterVesselBottomOutsideSemiMajorZHeight - reflectorThickness,
			0.*cm );
	
	zOffset = 0*cm; //where to do the subtraction
	G4SubtractionSolid *bottomReflector_solid2 = new G4SubtractionSolid(
			"bottomReflector_solid2",
			bottomReflector_solid,outerTitaniumVessel_solid4,
			0,G4ThreeVector(0,0,zOffset));
	
	//add in piece to cover the bottom of the displacer
	G4Tubs *bottomReflector_solid3 = new G4Tubs(
			"bottomReflector_solid3",
			titaniumOuterVesselWallOutsideRadius + reflectorThickness/2,
			titaniumOuterVesselFlangeOutsideRadius + reflectorThickness,
			reflectorThickness/2,
			0.*deg, 360.*deg);
	
	zOffset = -reflectorThickness/2;
	G4UnionSolid *bottomReflector_solid4 = new G4UnionSolid(
			"bottomReflector_solid4",
			bottomReflector_solid2, bottomReflector_solid3,
			0, G4ThreeVector(0,0,zOffset) );
	
	//Subtract hole for conduit
	//First the bottom subtraction:
   //zOffset = - acrylicWallOutsideHeight/2. + 380.*mm;
   zOffset = - titaniumOuterVesselBottomOutsideSemiMajorZHeight/2.;
   G4SubtractionSolid *bottomReflector_solid5 = new G4SubtractionSolid(
	   "bottomReflector_solid5",
	   bottomReflector_solid4,bottomCenterHole_solid2,
	   0,G4ThreeVector(0,0,zOffset) ); //was zOffset
	   
	G4LogicalVolume *bottomReflector_log = new G4LogicalVolume(
			bottomReflector_solid5, luxMaterials->LZSpecRef(), 
            		"bottomReflector_log" );
	bottomReflector_log->SetVisAttributes( luxMaterials->QuartzVis() );
	
	zOffset = -titaniumOuterVesselStraightWallHeight/2 - OuterTitaniumVesselVerticalOffset; 
	
	LUXSimDetectorComponent *BottomReflector;
	BottomReflector = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), bottomReflector_log, "BottomReflector",
			acrylic_log, 0, 0, true );
	
	
	//////////////////////////////////
	//Add reflectors on the top and bottom of the LS tanks
	////////////////////////////////////////////
	//top first
	
	G4Tubs *topTankReflector_solid = new G4Tubs(
			"topTankReflector_solid",
			0.*cm,
			acrylicOutsideRadius, 
			reflectorThickness/2,
			0.*deg, 360.*deg);
	
	//subtract the conduit holes
	//YBe
   G4SubtractionSolid *topTankReflector_solid2 = new G4SubtractionSolid(
	   "topReflector_solid2",
	   topTankReflector_solid,yBeHole_solid2,
	   0,G4ThreeVector(0,0,0) );
			
   //Then the two conduits
   xOffset = pmtConduitLSTankPosition;
   G4SubtractionSolid *topTankReflector_solid3 = new G4SubtractionSolid(
	   "topTankReflector_solid3",
	   topTankReflector_solid2,pmtConduitHole_solid2,
	   0,G4ThreeVector(xOffset,0,0) );
       
   xOffset = -thermoConduitLSTankPosition;
   G4SubtractionSolid *topTankReflector_solid4 = new G4SubtractionSolid(
      "topTankReflector_solid4",
	   topTankReflector_solid3,thermoConduitHole_solid2,
	   0,G4ThreeVector(xOffset,0,0) ); //was zOffset
	
	//then the source tube holes
	zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = 0.*mm;
   yOffset = sourceTubePenetrationPosition;
   G4SubtractionSolid *topTankReflector_solid5 = new G4SubtractionSolid(
      "topTankReflector_solid5",
	   topTankReflector_solid4,sourceTubeHole2_solid3,
	   0,G4ThreeVector(xOffset,yOffset,0) );
	   
	zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = (sqrt(3)/2.)*sourceTubePenetrationPosition;
   yOffset = - sourceTubePenetrationPosition/2.;
   G4SubtractionSolid *topTankReflector_solid6 = new G4SubtractionSolid(
      "topTankReflector_solid6",
	   topTankReflector_solid5,sourceTubeHole2_solid3,
	   rotZ120,G4ThreeVector(xOffset,yOffset,0) );
	
   
   zOffset = acrylicWallOutsideHeight/2. - 548.*mm;
   xOffset = - (sqrt(3)/2)*sourceTubePenetrationPosition;
   yOffset = - sourceTubePenetrationPosition/2.;
   G4SubtractionSolid *topTankReflector_solid7 = new G4SubtractionSolid(
      "topTankReflector_solid7",
	   topTankReflector_solid6,sourceTubeHole2_solid3,
	   rotZ240,G4ThreeVector(xOffset,yOffset,0) );
	
	G4LogicalVolume *topTankReflector_log = new G4LogicalVolume(
			topTankReflector_solid7, luxMaterials->Tyvek600(),
            		"topTankReflector_log" );
	topTankReflector_log->SetVisAttributes( luxMaterials->QuartzVis() );
	
	zOffset = acrylicWallOutsideHeight/2 - reflectorThickness/2; 
	
	LUXSimDetectorComponent *TopTankReflector;
	TopTankReflector = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), topTankReflector_log, "TopTankReflector",
			acrylic_log, 0, 0, true );
	
	//then the bottom tank reflector
	G4Tubs *bottomTankReflector_solid = new G4Tubs(
			"bottomTankReflector_solid",
			0.*cm,
			acrylicOutsideRadius,
			reflectorThickness/2,
			0.*deg, 360.*deg);
	
	//subtract conduit hole
	G4SubtractionSolid *bottomTankReflector_solid2 = new G4SubtractionSolid(
	   "bottomTankReflector_solid2",
	   bottomTankReflector_solid,bottomCenterHole_solid2,
	   0,G4ThreeVector(0,0,0) );
	
	G4LogicalVolume *bottomTankReflector_log = new G4LogicalVolume(
			bottomTankReflector_solid2, luxMaterials->Tyvek600(),
            		"bottomTankReflector_log" );
	bottomTankReflector_log->SetVisAttributes( luxMaterials->QuartzVis() );
	 
	zOffset = -acrylicWallOutsideHeight/2 + reflectorThickness/2; //also needs fixing
	
	LUXSimDetectorComponent *BottomTankReflector;
	BottomTankReflector = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), bottomTankReflector_log, "BottomTankReflector",
			acrylic_log, 0, 0, true );
	
	
	//Place the scintillator tank inside the water PMT bank
	zOffset = - waterTankHeight/2. + acrylicBottomRibHeight + acrylicWallOutsideHeight/2.;
	LUXSimDetectorComponent *ScintillatorTank;
	ScintillatorTank = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), acrylic_log, "ScintillatorTank",
			LZWaterBank->GetLogicalVolume(), 0, 0, true );
	
	
	
	/////////////////////////////////////////
	//The reflector-acrylic optical surfaces:
	/////////////////////////////////////////
	
	//Diffuse surface:
	G4OpticalSurface* diffuseReflectorOpSurface = new G4OpticalSurface(
					"diffuseReflectorOpSurface", unified, groundfrontpainted,
					dielectric_dielectric);
	diffuseReflectorOpSurface->SetMaterialPropertiesTable( luxMaterials->Tyvek600Table() );
	
	//Specular surface:
	G4OpticalSurface* specularReflectorOpSurface = new G4OpticalSurface(
					"specularReflectorOpSurface", unified, polishedfrontpainted, 
					dielectric_dielectric);
	specularReflectorOpSurface->SetMaterialPropertiesTable( luxMaterials->LZSpecRefTable() );
	
	//Each reflector has a logical border surface:
	
	//Side Reflectors 1-4, TopTank and BottomTank are diffuse:
	/*
	G4LogicalBorderSurface* diffuseSideReflector1Surface;
	diffuseSideReflector1Surface = new G4LogicalBorderSurface("diffuseSideReflector1Surface", ScintillatorTank, SideReflector1,
					diffuseReflectorOpSurface);
	
	G4LogicalBorderSurface* diffuseSideReflector2Surface;
	diffuseSideReflector2Surface = new G4LogicalBorderSurface("diffuseSideReflector2Surface", ScintillatorTank, SideReflector2,
					diffuseReflectorOpSurface);
	
	G4LogicalBorderSurface* diffuseSideReflector3Surface;
	diffuseSideReflector3Surface = new G4LogicalBorderSurface("diffuseSideReflector3Surface", ScintillatorTank, SideReflector3,
					diffuseReflectorOpSurface);
	
	G4LogicalBorderSurface* diffuseSideReflector4Surface;
	diffuseSideReflector4Surface = new G4LogicalBorderSurface("diffuseSideReflector4Surface", ScintillatorTank, SideReflector4,
					diffuseReflectorOpSurface);
	*/
	G4LogicalBorderSurface* diffuseTopTankReflectorSurface;
	diffuseTopTankReflectorSurface = new G4LogicalBorderSurface("diffuseTopTankReflectorSurface", ScintillatorTank, TopTankReflector,
					diffuseReflectorOpSurface);
					
	G4LogicalBorderSurface* diffuseBottomTankReflectorSurface;
	diffuseBottomTankReflectorSurface = new G4LogicalBorderSurface("diffuseBottomTankReflectorSurface", ScintillatorTank, BottomTankReflector,
					diffuseReflectorOpSurface);				
	
	//Center, Top, Bottom are specular: changed to diffuse!
	
	G4LogicalBorderSurface* specularCenterReflectorSurface;
	specularCenterReflectorSurface = new G4LogicalBorderSurface("specularCenterReflectorSurface", ScintillatorTank, CenterReflector,
					diffuseReflectorOpSurface);
	
	G4LogicalBorderSurface* specularTopReflectorSurface;
	specularTopReflectorSurface = new G4LogicalBorderSurface("specularTopReflectorSurface", ScintillatorTank, TopReflector,
					diffuseReflectorOpSurface);
	
	G4LogicalBorderSurface* specularBottomReflectorSurface;
	specularBottomReflectorSurface = new G4LogicalBorderSurface("specularBottomReflectorSurface", ScintillatorTank, BottomReflector,
					diffuseReflectorOpSurface);
	
	////////////////////////////////
	// The HV Umbilical Conduit
	///////////////////////////////
	//Currently just a straight tube + cone
	//First have to subtract shape of cryostat off of cone
			
	//The cone
	G4Cons *hvConduitCone_solid = new G4Cons(
	   "hvConduitCone_solid",
	   0.*cm, hvConduitConeRadius,
	   0.*cm, hvConduitRadius,
	   hvConduitConeHeight, 0.*deg, 360.*deg );
	
	//xOffset = acrylicOutsideRadius - titaniumOuterVesselFlangeOutsideRadius + titaniumOuterVesselWallOutsideRadius;
	//xOffset = /*titaniumOuterVesselWallOutsideRadius +*/ (hvConduitConeHeight/2. - hvConduitConeRadius/tan(hvConduitAngle))*sin(hvConduitAngle);
	//xOffset = titaniumOuterVesselWallOutsideRadius + hvConduitConeHeight*sin(hvConduitAngle);
	
	xOffset = 1842*mm; //really hacking it here... this gets the length of the cone right
	yOffset = 0*mm;
	zOffset = 0*mm;
	G4SubtractionSolid *hvConduit_solid1 = new G4SubtractionSolid(
	   "hvConduit_solid1",
	   hvConduitCone_solid,cryo_solid,
	   hvNegRot,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	//Now add in the long skinny piece that goes to the water tank edge
	G4Tubs *hvConduit_solid2 = new G4Tubs(
			"hvConduit_solid2",
			0.*cm, hvConduitRadius,
			(waterTankRadius-acrylicOutsideRadius)/(2.*sin(hvConduitAngle)),
			0.*deg, 360.*deg );
	
	zOffset = hvConduitConeHeight/2. + (waterTankRadius-acrylicOutsideRadius)/(2.*sin(hvConduitAngle));
	xOffset = 0*mm;
	yOffset = 0*mm;
	G4UnionSolid *hvConduit_solid3 = new G4UnionSolid(
	   "hvConduit_solid3",
	   hvConduit_solid1,hvConduit_solid2, //want hvConduit_solid1b
	   0,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	G4LogicalVolume *hvConduit_log = new G4LogicalVolume(
			hvConduit_solid3, luxMaterials->LZPMTCablingConduit(), "hvConduit_log" );
	hvConduit_log->SetVisAttributes( luxMaterials->SteelVis() );
	
	zOffset = - waterTankHeight/2. + hvConduitOuterTitaniumVesselJointVerticalHeight;
	xOffset = - titaniumOuterVesselWallOutsideRadius + 0.95*mm;
	LUXSimDetectorComponent *hvUmbilicalConduit;
	hvUmbilicalConduit = new LUXSimDetectorComponent(
			hvRot, G4ThreeVector(xOffset,0,zOffset), hvConduit_log, "HVConduit",
			LZWaterBank->GetLogicalVolume(), 0, 0, true );
	
	
	/////////////////////////////////
	// The PMT cabling conduit
	/////////////////////////////////
	
	//First thing is to make the straight vertical piece that connects to the cryostat
	//it's a funny shape so we make a straight piece and subtract off the shape of the cryostat
	
	G4Tubs *pmtConduitVertical_solid = new G4Tubs(
			"pmtConduitVertical_solid",
			0.*cm, pmtConduitRadius,
			pmtConduitVerticalPieceHeight/2.,
			0.*deg, 360.*deg );
	
	//now subtract off the outer cryo shape
	zOffset = - titaniumOuterVesselOverallHeight/2. - pmtConduitVerticalPieceHeight/2. + pmtConduitFudge + 46*mm + 87.2*mm; // no overlaps
	xOffset = - pmtConduitLSTankPosition;
	
	G4SubtractionSolid *pmtConduitVertical_solid2 = new G4SubtractionSolid(
	   "pmtConduitVertical_solid2",
	   pmtConduitVertical_solid,outerTitaniumVessel_solid7,
	   0,G4ThreeVector(xOffset,0,zOffset) );
	
	//Then make the bent part
	G4Torus *pmtConduitBend_solid = new G4Torus(
	         "pmtConduitBend_solid",
            0.*cm,
            pmtConduitRadius,
            pmtConduitBendRadius,
            90.*degree,
            90.*degree );
	
	//Now the straight horizontal piece
	G4Tubs *pmtConduitHorizontal_solid = new G4Tubs(
			"pmtConduitHorizontal_solid",
			0.*cm, pmtConduitRadius,
			pmtConduitHorizontalPieceLength/2.,
			0.*deg, 360.*deg );
			
	G4RotationMatrix *rotY90 = new G4RotationMatrix();
	rotY90->rotateY( 90.*deg );
	
	zOffset = 0*cm;
	xOffset = pmtConduitHorizontalPieceLength/2.;
	yOffset = pmtConduitBendRadius;
	G4UnionSolid *pmtConduit_solid = new G4UnionSolid(
	   "pmtConduit_solid",
	   pmtConduitBend_solid,pmtConduitHorizontal_solid,
	   rotY90,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	//Add another bent part
	G4RotationMatrix *rotZ180 = new G4RotationMatrix();
	rotZ180->rotateZ( 180.*deg );
	
	zOffset = 0*cm;
	xOffset = pmtConduitHorizontalPieceLength;
	yOffset = pmtConduitBendRadius*2.;
	G4UnionSolid *pmtConduit_solid2 = new G4UnionSolid(
	   "pmtConduit_solid2",
	   pmtConduit_solid,pmtConduitBend_solid,
	   rotZ180,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	//Then add a small vertical piece that touches the top of the water tank
	G4Tubs *pmtConduitSmallVertical_solid = new G4Tubs(
			"pmtConduitSmallVertical_solid",
			0.*cm, pmtConduitRadius,
			pmtConduitSmallVerticalPieceHeight/2.,
			0.*deg, 360.*deg );
	
	G4RotationMatrix *rotXp90 = new G4RotationMatrix();
	rotXp90->rotateX( 90.*deg );
	
	zOffset = 0*cm;
	xOffset = pmtConduitHorizontalPieceLength + pmtConduitBendRadius;
	yOffset = pmtConduitBendRadius*2. + pmtConduitSmallVerticalPieceHeight/2.;
	G4UnionSolid *pmtConduit_solid3 = new G4UnionSolid(
	   "pmtConduit_solid3",
	   pmtConduit_solid2,pmtConduitSmallVertical_solid,
	   rotXp90,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	//Then add the vertical piece that touches the cryostat
	G4RotationMatrix *rotX90 = new G4RotationMatrix();
	rotX90->rotateX( -90.*deg );
	
	zOffset = pmtConduitVerticalPieceHeight/2.;
	xOffset = pmtConduitBendRadius;
	G4UnionSolid *pmtConduit_solid4 = new G4UnionSolid(
	   "pmtConduit_solid4",
	   pmtConduitVertical_solid2,pmtConduit_solid3,
	   rotX90,G4ThreeVector(xOffset,0,zOffset) );
	
	
	G4LogicalVolume *pmtConduit_log = new G4LogicalVolume(
			pmtConduit_solid4, luxMaterials->LZPMTCablingConduit(), 
			"pmtConduit_log" );
	pmtConduit_log->SetVisAttributes( luxMaterials->SteelVis() );
	
	
	zOffset = - waterTankHeight/2. + pmtConduitHorizontalPieceVerticalPosition - pmtConduitFudge - 2.*pmtConduitBendRadius - 336.8*mm; //hardcoded until updated dimension
	xOffset = pmtConduitLSTankPosition;
	LUXSimDetectorComponent *PMTCablingConduit;
	PMTCablingConduit = new LUXSimDetectorComponent(
			0, G4ThreeVector(xOffset,0,zOffset), pmtConduit_log, "PMTCablingConduit",
			LZWaterBank->GetLogicalVolume(), 0, 0, true );
	
	
	
	
	
	////////////////////////////
	//The thermosyphon conduit
	////////////////////////////
	
	//Make the straight vertical piece that connect to the cryostat
	//Make a straight piece and subtract off the shape of the cryostat
	
	G4Tubs *thermoConduitVertical_solid = new G4Tubs(
			"thermoConduitVertical_solid",
			0.*cm, thermoConduitRadius,
			thermoConduitVerticalPieceHeight/2.,
			0.*deg, 360.*deg );
	
	//now subtract off the outer cryo shape
	//zOffset = - titaniumOuterVesselOverallHeight/2. - thermoConduitVerticalPieceHeight/2. + 115.*mm; //hardcoded, needs work, old
	zOffset = - titaniumOuterVesselOverallHeight/2. - thermoConduitVerticalPieceHeight/2. + thermoConduitFudge + 76*mm + 87.7*mm; //no overlaps
	xOffset = thermoConduitLSTankPosition;
	
	G4SubtractionSolid *thermoConduitVertical_solid2 = new G4SubtractionSolid(
	   "thermoConduitVertical_solid2",
	   thermoConduitVertical_solid,outerTitaniumVessel_solid7,
	   0,G4ThreeVector(xOffset,0,zOffset) );
	
	//Then make the bent part
	G4Torus *thermoConduitBend_solid = new G4Torus(
	         "thermoConduitBend_solid",
            0.*cm,
            thermoConduitRadius,
            thermoConduitBendRadius,
            0.*degree,
            90.*degree );
	
	//and a piece that has the angle
	//small angle = 12.3*degree
	//larger angle = 77.7*degree
	/*
	G4CutTubs *thermoConduitLowerAdapter = new G4CutTubs( 
	                "thermoConduitLowerAdapter",
                   0*cm,
                   thermoConduitRadius,
                   thermoConduitLowerAnglePieceLength/2.,
                   0.*degree,
                   360.*degree,
                   G4ThreeVector(0,0,-1), //lower surface normal vector
                   G4ThreeVector(0.98,0,.21) ); //upper surface normal vector
	*/
	
	//Now the straight horizontal piece
	G4Tubs *thermoConduitHorizontal_solid = new G4Tubs(
			"thermoConduitHorizontal_solid",
			0.*cm, thermoConduitRadius,
			thermoConduitHorizontalPieceLength/2.,
			0.*deg, 360.*deg );
			
	//G4RotationMatrix *rotY90 = new G4RotationMatrix();
	//rotY90->rotateY( 90.*deg );
	
	G4RotationMatrix *rot = new G4RotationMatrix();
	rot->rotateY( 90.*deg );
	//rot->rotateZ( 12.3*deg ); //not working...
	//rot->rotateZ( .785 );
	
	zOffset = 0*cm;
	xOffset = - thermoConduitHorizontalPieceLength/2.;
	yOffset = thermoConduitBendRadius;
	G4UnionSolid *thermoConduit_solid = new G4UnionSolid(
	   "thermoConduit_solid",
	   thermoConduitBend_solid,thermoConduitHorizontal_solid,
	   rot,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	//Add another bent part
	//G4RotationMatrix *rotZ180 = new G4RotationMatrix();
	//rotZ180->rotateZ( 180.*deg );
	//rotZ180 already declared above
	zOffset = 0*cm;
	xOffset = - thermoConduitHorizontalPieceLength;
	yOffset = thermoConduitBendRadius*2.;
	G4UnionSolid *thermoConduit_solid2 = new G4UnionSolid(
	   "thermoConduit_solid2",
	   thermoConduit_solid,thermoConduitBend_solid,
	   rotZ180,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	//Then add a small vertical piece that touches the top of the water tank
	G4Tubs *thermoConduitSmallVertical_solid = new G4Tubs(
			"thermoConduitSmallVertical_solid",
			0.*cm, thermoConduitRadius,
			thermoConduitSmallVerticalPieceHeight/2.,
			0.*deg, 360.*deg );
	
	//G4RotationMatrix *rotXp90 = new G4RotationMatrix();
	//rotXp90->rotateX( 90.*deg );
	
	zOffset = 0*cm;
	xOffset = - thermoConduitHorizontalPieceLength - thermoConduitBendRadius;
	yOffset = thermoConduitBendRadius*2. + thermoConduitSmallVerticalPieceHeight/2.;
	G4UnionSolid *thermoConduit_solid3 = new G4UnionSolid(
	   "thermoConduit_solid3",
	   thermoConduit_solid2,thermoConduitSmallVertical_solid,
	   rotXp90,G4ThreeVector(xOffset,yOffset,zOffset) );
	
	//Then add the vertical piece that touches the cryostat
	//G4RotationMatrix *rotX90 = new G4RotationMatrix();
	//rotX90->rotateX( -90.*deg );
	
	//zOffset = (840./2.)*mm + thermoConduitBendRadius - 216*mm; //hardcoded, needs work
	zOffset = thermoConduitVerticalPieceHeight/2.;
	xOffset = - thermoConduitBendRadius;
	G4UnionSolid *thermoConduit_solid4 = new G4UnionSolid(
	   "thermoConduit_solid4",
	   thermoConduitVertical_solid2,thermoConduit_solid3,
	   rotX90,G4ThreeVector(xOffset,0,zOffset) );
	
	
	G4LogicalVolume *thermoConduit_log = new G4LogicalVolume(
			thermoConduit_solid4, luxMaterials->LZThermosyphonConduit(),
			"thermoConduit_log" );
	thermoConduit_log->SetVisAttributes( luxMaterials->SteelVis() );
	
	//zOffset = - waterTankHeight/2. + acrylicBottomRibHeight + acrylicWallOutsideHeight - 460.*mm; //hardcoded, needs work, old
	zOffset = - waterTankHeight/2. + thermoConduitHorizontalPieceVerticalPosition - thermoConduitFudge - 2.*thermoConduitBendRadius - 336.8*mm;
	xOffset = - thermoConduitLSTankPosition;
	LUXSimDetectorComponent *ThermosyphonConduit;
	ThermosyphonConduit = new LUXSimDetectorComponent(
			0, G4ThreeVector(xOffset,0,zOffset), thermoConduit_log, "ThermosyphonConduit",
			LZWaterBank->GetLogicalVolume(), 0, 0, true );
	
	
	////////////////////////////
	// The straight neutron tube
	///////////////////////////
	
	//Already made the solid above: neutronTubeHole_solid2
	
	G4LogicalVolume *neutronTubeAcrylic_log = new G4LogicalVolume(
			neutronTubeHole_solid2, luxMaterials->Acrylic(),
			"neutronTubeAcrylic_log" );
	neutronTubeAcrylic_log->SetVisAttributes( luxMaterials->AcrylicVis() );
	
	zOffset = -waterTankHeight/2. + neutronTubeVerticalPosition;
	yOffset = - ( neutronTubeLength/2. + titaniumOuterVesselWallOutsideRadius );// + 1.*cm; //gap here
	xOffset = 0.*mm;
	LUXSimDetectorComponent *AcrylicNeutronTube;
	AcrylicNeutronTube = new LUXSimDetectorComponent(
			rotation, G4ThreeVector(xOffset,yOffset,zOffset), neutronTubeAcrylic_log, "AcrylicNeutronTube",
			LZWaterBank->GetLogicalVolume(), 0, 0, true );
	
	//Now make the air filled part of the tube
	G4Tubs *neutronTubeHole_solid3 = new G4Tubs(
	   "neutronTubeHole_solid3",
	   0.*cm, neutronTubeOuterRadius - neutronTubeThickness,
	   neutronTubeLength/2. - 1.*cm, //endcap of tube is 1*cm thick
	   0.*deg, 360.*deg );
	
	G4LogicalVolume *neutronTubeAir_log = new G4LogicalVolume(
			neutronTubeHole_solid3, luxMaterials->Air(),
			"neutronTubeAcrylic_log" );
	neutronTubeAir_log->SetVisAttributes( luxMaterials->AirVis() );
	
	LUXSimDetectorComponent *AirNeutronTube;
	AirNeutronTube = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,0), neutronTubeAir_log, "AirNeutronTube",
			neutronTubeAcrylic_log, 0, 0, true );
	
	////////////////////////////
	// The angled neutron tube
	///////////////////////////
	
	//still need to clip ends off to make them angled
	
	G4Tubs *angledNeutronTubeHole_solid3 = new G4Tubs(
	   "angledNeutronTubeHole_solid3",
	   0.*cm, neutronTubeOuterRadius,
	   (angledNeutronTubeLength)/2.,
	   0.*deg, 360.*deg );
	
	G4LogicalVolume *angledNeutronTubeAcrylic_log = new G4LogicalVolume(
			angledNeutronTubeHole_solid3, luxMaterials->Acrylic(),
			"angledNeutronTubeAcrylic_log" );
	angledNeutronTubeAcrylic_log->SetVisAttributes( luxMaterials->AcrylicVis() );
	
	//zOffset = -waterTankHeight/2. + neutronTubeVerticalPosition;
	//yOffset = neutronTubeLength/2. + titaniumOuterVesselWallOutsideRadius + 1.*cm;
	//xOffset = 0.*mm;
	
	zOffset = -waterTankHeight/2. + (angledNeutronTubeBottomVerticalPosition + angledNeutronTubeTopVerticalPosition)/2.;
   xOffset = outerCryostatToWaterTankDistance/2. + titaniumOuterVesselWallOutsideRadius;
   yOffset = 0*mm;
	//yOffset = neutronTubeLength/2. + titaniumOuterVesselWallOutsideRadius/2.;
	
	LUXSimDetectorComponent *AngledAcrylicNeutronTube;
	AngledAcrylicNeutronTube = new LUXSimDetectorComponent(
			angledTubeRotation, G4ThreeVector(xOffset,yOffset,zOffset), angledNeutronTubeAcrylic_log, "AngledAcrylicNeutronTube",
			LZWaterBank->GetLogicalVolume(), 0, 0, true );
	
	//Now make the air filled part of the tube
	G4Tubs *angledNeutronTubeHole_solid4 = new G4Tubs(
	   "angledNeutronTubeHole_solid4",
	   0.*cm, neutronTubeOuterRadius - neutronTubeThickness,
	   angledNeutronTubeLength/2. - 1.*cm, //endcap of tube is 1*cm thick
	   0.*deg, 360.*deg );
	
	G4LogicalVolume *angledNeutronTubeAir_log = new G4LogicalVolume(
			angledNeutronTubeHole_solid4, luxMaterials->Air(),
			"angledNeutronTubeAcrylic_log" );
	angledNeutronTubeAir_log->SetVisAttributes( luxMaterials->AirVis() );
	
	LUXSimDetectorComponent *AngledAirNeutronTube;
	AngledAirNeutronTube = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,0), angledNeutronTubeAir_log, "AngledAirNeutronTube",
			angledNeutronTubeAcrylic_log, 0, 0, true );
	
	
	
	
	///////////////////////
	//	The vacuum space
	////////////////////////
	
	//	Combine the main cylindrical section with the bottom dome space
	G4Tubs *vacuumSpace_solid1 = new G4Tubs(
			"vacuumSpace_solid1",
			0.*cm, titaniumOuterVesselInsideRadius,
			titaniumOuterVesselStraightWallHeight/2,
			0.*deg, 360.*deg );
	G4Ellipsoid *vacuumSpace_solid2 = new G4Ellipsoid(
			"vacuumSpace_solid2",
			titaniumOuterVesselInsideRadius,
			titaniumOuterVesselInsideRadius,
			titaniumOuterVesselBottomInsideSemiMajorZHeight,
			-titaniumOuterVesselBottomInsideSemiMajorZHeight,
			0.*cm );
	zOffset = -titaniumOuterVesselStraightWallHeight/2;
	G4UnionSolid *vacuumSpace_solid3 = new G4UnionSolid(
			"vacuumSpace_solid3",
			vacuumSpace_solid1, vacuumSpace_solid2,
			0, G4ThreeVector(0,0,zOffset) );
	
	//	Combine in the top dome space
	G4Ellipsoid *vacuumSpace_solid4 = new G4Ellipsoid(
			"vacuumSpace_solid4",
			titaniumOuterVesselInsideRadius,
			titaniumOuterVesselInsideRadius,
			titaniumOuterVesselTopInsideSemiMajorZHeight,
			0.*cm,
			titaniumOuterVesselTopInsideSemiMajorZHeight );
	zOffset = titaniumOuterVesselStraightWallHeight/2;
	G4UnionSolid *vacuumSpace_solid5 = new G4UnionSolid(
			"vacuumSpace_solid5",
			vacuumSpace_solid3, vacuumSpace_solid4,
			0, G4ThreeVector(0,0,zOffset) );

	G4LogicalVolume *vacuumSpace_log = new G4LogicalVolume(
			vacuumSpace_solid5, luxMaterials->Vacuum(),
			"vacuumSpace_log" );
	vacuumSpace_log->SetVisAttributes( luxMaterials->VacuumVis() );
	
		LUXSimDetectorComponent *vacuumSpace;
	vacuumSpace = new LUXSimDetectorComponent(
			0, G4ThreeVector(), "VacuumSpace",
			vacuumSpace_log, outerTitaniumVessel, 0, 0, true );
	
	//
	//	The inner titanium vessel
	//
	
    //      The Ti outer boundary
	
	//	Combine the main vessel section with the flange
	G4Tubs *innerTitaniumVessel_solid1 = new G4Tubs(
			"innerTitaniumVessel_solid1",
			0.*cm, titaniumInnerVesselWallTopOutsideRadius,
			titaniumInnerVesselStraightWallHeightTop/2,
			0.*deg, 360.*deg );


	G4Cons *innerTitaniumVessel_solid2  = new G4Cons(
            "innerTitaniumVessel_solid2", 0,
            titaniumInnerVesselWallBottomOutsideRadius, 0,
            titaniumInnerVesselWallTopOutsideRadius,
            titaniumInnerVesselBulgeWallHeight/2, 0.*deg,360.*deg);
	
	zOffset = titaniumInnerVesselBulgeWallHeight/2 +
            titaniumInnerVesselStraightWallHeightTop/2;
	G4UnionSolid *innerTitaniumVessel_solid3 = new G4UnionSolid(
            "innerTitaniumVessel_solid3", innerTitaniumVessel_solid2,
            innerTitaniumVessel_solid1, 0, G4ThreeVector(0,0,zOffset) );

	G4Tubs *innerTitaniumVessel_solid4 = new G4Tubs(
			"innerTitaniumVessel_solid4",
			0.*cm, titaniumInnerVesselWallBottomOutsideRadius,
			titaniumInnerVesselStraightWallHeightBottom/2,
			0.*deg, 360.*deg );

	zOffset =  titaniumInnerVesselStraightWallHeightBottom/2 +
            titaniumInnerVesselBulgeWallHeight/2;

	G4UnionSolid *innerTitaniumVessel_solid5 = new G4UnionSolid(
            "innerTitaniumVessel_solid5", innerTitaniumVessel_solid4,
            innerTitaniumVessel_solid3, 0, G4ThreeVector(0,0,zOffset) );

	G4Tubs *innerTitaniumVessel_solid6 = new G4Tubs(
			"innerTitaniumVessel_solid6",
			0.*cm, titaniumInnerVesselFlangeOutsideRadius,
			titaniumInnerVesselFlangeHeight/2,
			0.*deg, 360.*deg );

	zOffset = titaniumInnerVesselBulgeWallHeight +
            titaniumInnerVesselStraightWallHeightTop +
            titaniumInnerVesselStraightWallHeightBottom/2 +
            titaniumInnerVesselFlangeHeight/2 -
            titaniumInnerVesselFlangeHeight/2;

	G4UnionSolid *innerTitaniumVessel_solid7 = new G4UnionSolid(
			"innerTitaniumVessel_solid7",
			innerTitaniumVessel_solid5, innerTitaniumVessel_solid6,
			0, G4ThreeVector(0,0,zOffset) );
	

	//	Combine in the bottom dome shape
	G4Ellipsoid *innerTitaniumVessel_solid8 = new G4Ellipsoid(
			"innerTitaniumVessel_solid8",
			titaniumInnerVesselWallBottomOutsideRadius,
			titaniumInnerVesselWallBottomOutsideRadius,
			titaniumInnerVesselBottomOutsideSemiMajorZHeight,
			-titaniumInnerVesselBottomOutsideSemiMajorZHeight,
			0.*cm );
	zOffset = -titaniumInnerVesselStraightWallHeightBottom/2;

	G4UnionSolid *innerTitaniumVessel_solid9 = new G4UnionSolid(
			"innerTitaniumVessel_solid9",
			innerTitaniumVessel_solid7, innerTitaniumVessel_solid8,
			0, G4ThreeVector( 0, 0, zOffset ) );
	
	//	Combine in the top dome shape
	G4Ellipsoid *innerTitaniumVessel_solid10 = new G4Ellipsoid(
			"innerTitaniumVessel_solid10",
			titaniumInnerVesselWallTopOutsideRadius,
			titaniumInnerVesselWallTopOutsideRadius,
			titaniumInnerVesselTopOutsideSemiMajorZHeight,
	 		0.*cm,
			titaniumInnerVesselTopOutsideSemiMajorZHeight );
	zOffset = titaniumInnerVesselBulgeWallHeight +
            titaniumInnerVesselStraightWallHeightTop +
            titaniumInnerVesselStraightWallHeightBottom/2 + 1.5*mm;
	//kludge factor of 1.5mm added to get dimensions to match with drawing

	G4UnionSolid *innerTitaniumVessel_solid11 = new G4UnionSolid(
			"innerTitaniumVessel_solid11",
			innerTitaniumVessel_solid9, innerTitaniumVessel_solid10,
			0, G4ThreeVector(0,0,zOffset) );
	
	G4LogicalVolume *innerTitaniumVessel_log = new G4LogicalVolume(
			innerTitaniumVessel_solid11, luxMaterials->Titanium(),
			"innerTitaniumVessel_log" );
	innerTitaniumVessel_log->SetVisAttributes( luxMaterials->TitaniumVis() );
	
	zOffset = 166.79*mm - (titaniumInnerVesselStraightWallHeightBottom/2 +
            titaniumInnerVesselBottomOutsideSemiMajorZHeight) - 238.21*mm;

	LUXSimDetectorComponent *innerTitaniumVessel;
	innerTitaniumVessel = new LUXSimDetectorComponent(
            0, G4ThreeVector(0,0,zOffset), "InnerTitaniumVessel",
			innerTitaniumVessel_log, vacuumSpace, 0, 0, true );

        // The code below adds a tungsten pig which will hold the YBe source
        if(luxManager->GetLZYBePig()) {
            G4double yBePigInnerRadius = 2.54 * cm;
            G4double yBePigOuterRadius = luxManager->GetLZYBePigDiameter() / 2.;
            G4double yBePigBottomCurvature = 2.54 * cm;
            G4double yBePigOuterHeight = luxManager->GetLZYBePigHeight() - yBePigBottomCurvature;
            G4double yBeXOffset = 0. * cm;
            G4double yBeZOffset = 153. * cm;
            G4Tubs *yBePig_outer = new G4Tubs( "yBePig_outer", 0,
            yBePigOuterRadius, yBePigOuterHeight/2, 0, 360.*deg );
            G4Tubs *yBePig_inner = new G4Tubs("yBePig_inner", 0, yBePigInnerRadius, yBePigOuterHeight/4 - .5 * cm, 0, 360.*deg);
            G4VSolid *yBePigMain_solid = new G4SubtractionSolid("yBePigMain_solid", yBePig_outer, yBePig_inner, 0, G4ThreeVector(0., 0., yBePigOuterHeight/4. - .5 * cm));
            G4double yBePigBottomTorusRadius = yBePigOuterRadius - yBePigBottomCurvature;
            G4Torus *yBePigBottomTor_solid = new G4Torus("yBePigBottomTor_solid", 0., yBePigBottomCurvature, yBePigBottomTorusRadius, 0., 360. * deg);
            G4Tubs *yBePigBottomCyl_solid = new G4Tubs("yBePigBottomCyl_solid", 0., yBePigBottomTorusRadius, yBePigBottomCurvature, 0, 360. * deg);
            G4UnionSolid *yBePigBottom_solid = new G4UnionSolid ("yBePigBottom_solid", yBePigBottomTor_solid, yBePigBottomCyl_solid);
            G4double yBePigUnionOffset = -1. * yBePigOuterHeight / 2.;
            G4UnionSolid *yBePig_solid = new G4UnionSolid ("yBePig_solid", yBePigMain_solid, yBePigBottom_solid, 0, G4ThreeVector(0, 0, yBePigUnionOffset));
            G4LogicalVolume *yBePig_log = new G4LogicalVolume(yBePig_solid, luxMaterials->HeavyMet(), "yBePig_log");
                                                              yBePig_log->SetVisAttributes(luxMaterials->TungstenVis());
            LUXSimDetectorComponent *yBePig;
            yBePig = new LUXSimDetectorComponent(0, G4ThreeVector(yBeXOffset, 0., yBeZOffset), "YBePig", yBePig_log, vacuumSpace, 0, 0, false );
            G4Tubs *yBeSource_solid = new G4Tubs( "yBeSource_solid", 0, yBePigInnerRadius, yBePigOuterHeight/4, 0, 360.*deg );
            G4LogicalVolume *yBeSource_log = new G4LogicalVolume(yBeSource_solid, luxMaterials->Air(), "yBeSource_log");
            yBeSource_log->SetVisAttributes(luxMaterials->AirVis());
            LUXSimDetectorComponent *yBeSource;
            yBeSource = new LUXSimDetectorComponent(0, G4ThreeVector(yBeXOffset, 0., yBeZOffset), "YBeSource", yBeSource_log, vacuumSpace, 0, 0, false );
        }
	
	//
	//	The xenon space. This is actually filled with liquid xenon, and is
	//	referred to as the liquid skin.
	//

	//	Combine the main vessel section with the flange
	G4Tubs *liquidSkinXenon_solid1 = new G4Tubs(
			"liquidSkinXenon_solid1",
			0.*cm, titaniumInnerVesselWallTopInsideRadius,
			titaniumInnerVesselStraightWallHeightTop/2,
			0.*deg, 360.*deg );


	G4Cons *liquidSkinXenon_solid2  = new G4Cons("liquidSkinXenon_solid2",
					0,titaniumInnerVesselWallBottomInsideRadius,
					0,titaniumInnerVesselWallTopInsideRadius,
					titaniumInnerVesselBulgeWallHeight/2,
					0.*deg,360.*deg);

	
	zOffset = titaniumInnerVesselBulgeWallHeight/2 +
            titaniumInnerVesselStraightWallHeightTop/2;
	G4UnionSolid *liquidSkinXenon_solid3 = new G4UnionSolid(
						   "liquidSkinXenon_solid3",
						   liquidSkinXenon_solid2, liquidSkinXenon_solid1,
						   0, G4ThreeVector(0,0,zOffset) );

	G4Tubs *liquidSkinXenon_solid4 = new G4Tubs(
			"liquidSkinXenon_solid4",
			0.*cm, titaniumInnerVesselWallBottomInsideRadius,
			titaniumInnerVesselStraightWallHeightBottom/2,
			0.*deg, 360.*deg );

	zOffset =  titaniumInnerVesselStraightWallHeightBottom/2 +
            titaniumInnerVesselBulgeWallHeight/2;

	G4UnionSolid *liquidSkinXenon_solid5 = new G4UnionSolid(
						   "liquidSkinXenon_solid5",
						   liquidSkinXenon_solid4, liquidSkinXenon_solid3,
						   0, G4ThreeVector(0,0,zOffset) );

	//	Combine in the bottom dome shape
	G4Ellipsoid *liquidSkinXenon_solid6 = new G4Ellipsoid(
			"liquidSkinXenon_solid6",
			titaniumInnerVesselWallBottomInsideRadius,
			titaniumInnerVesselWallBottomInsideRadius,
			titaniumInnerVesselBottomInsideSemiMajorZHeight,
			-titaniumInnerVesselBottomInsideSemiMajorZHeight,
			0.*cm );
	zOffset = -titaniumInnerVesselStraightWallHeightBottom/2;

	G4UnionSolid *liquidSkinXenon_solid7 = new G4UnionSolid(
			"liquidSkinXenon_solid7",
			liquidSkinXenon_solid5, liquidSkinXenon_solid6,
			0, G4ThreeVector( 0, 0, zOffset ) );
	
	//	Combine in the top dome shape
	G4Ellipsoid *liquidSkinXenon_solid8 = new G4Ellipsoid(
			"liquidSkinXenon_solid8",
			titaniumInnerVesselWallTopInsideRadius,
			titaniumInnerVesselWallTopInsideRadius,
			titaniumInnerVesselTopInsideSemiMajorZHeight+1.5*mm, //removed +1.5*mm
	 		0.*cm,
			titaniumInnerVesselTopInsideSemiMajorZHeight +1.5*mm); //removed +1.5*mm
	zOffset = titaniumInnerVesselBulgeWallHeight +
            titaniumInnerVesselStraightWallHeightTop +
            titaniumInnerVesselStraightWallHeightBottom/2;
	//kludge factor of 1.5mm added to get dimensions to match with drawing

	G4UnionSolid *liquidSkinXenon_solid9 = new G4UnionSolid(
			"liquidSkinXenon_solid9",
			liquidSkinXenon_solid7, liquidSkinXenon_solid8,
			0, G4ThreeVector(0,0,zOffset) );
	
	G4LogicalVolume *liquidSkinXenon_log = new G4LogicalVolume(
			liquidSkinXenon_solid9, luxMaterials->LiquidXe(),
			"liquidSkinXenon_log" );
	liquidSkinXenon_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
	
	zOffset = 166.79*mm - (titaniumInnerVesselStraightWallHeightBottom/2 +
            titaniumInnerVesselBottomInsideSemiMajorZHeight) - 238.21*mm;
	zOffset = 0;

	LUXSimDetectorComponent *liquidSkinXenon;
	liquidSkinXenon = new LUXSimDetectorComponent(
            0, G4ThreeVector(0,0,zOffset), "LiquidSkinXenon",
            liquidSkinXenon_log, innerTitaniumVessel, 0, 0, true );
	
	

	//	The liquid xenon skin / titanium optical boundary
	G4LogicalBorderSurface *titaniumLiquidSkinXeSurface;
	titaniumLiquidSkinXeSurface = new G4LogicalBorderSurface(
			"titaniumLiquidSkinXeSurface",
			liquidSkinXenon, innerTitaniumVessel,
			luxMaterials->LXeTiSurface() );



	//
	//	The gaseous xenon space. This volume is referred to as the gaseous skin.
	//
       
	//	Combine the main cylindrical section with the top dome space

	G4Tubs *gaseousSkinXenon_solid1 = new G4Tubs(
			"gaseousSkinXenon_solid1",
			0.*cm, titaniumInnerVesselWallTopInsideRadius,
			gaseousXenonStraightWallHeight/2,
			0.*deg, 360.*deg );

	G4Ellipsoid *gaseousSkinXenon_solid2 = new G4Ellipsoid(
			"gaseousSkinXenon_solid2",
			titaniumInnerVesselWallTopInsideRadius,
			titaniumInnerVesselWallTopInsideRadius,
			titaniumInnerVesselTopInsideSemiMajorZHeight + 1.5*mm,
	 		0.*cm,
			titaniumInnerVesselTopInsideSemiMajorZHeight +1.5*mm );

	zOffset = gaseousXenonStraightWallHeight/2;
	G4UnionSolid *gaseousSkinXenon_solid3 = new G4UnionSolid(
			"gaseousSkinXenon_solid3",
			gaseousSkinXenon_solid1, gaseousSkinXenon_solid2,
			0, G4ThreeVector(0,0,zOffset) );
	
	G4LogicalVolume *gaseousSkinXenon_log = new G4LogicalVolume(
			gaseousSkinXenon_solid3, luxMaterials->GasXe(),
			"gaseousSkinXenon_log" );
	gaseousSkinXenon_log->SetVisAttributes( luxMaterials->GasXeVis() );

	zOffset = titaniumInnerVesselBulgeWallHeight +
            titaniumInnerVesselStraightWallHeightTop +
            titaniumInnerVesselStraightWallHeightBottom/2 -
            gaseousXenonStraightWallHeight/2;
	
	LUXSimDetectorComponent *gaseousSkinXenon;
	gaseousSkinXenon = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), "GaseousSkinXenon",
			gaseousSkinXenon_log, liquidSkinXenon, 0, 0, true );
	
	//	The gaseous xenon skin / titanium optical boundary
	G4LogicalBorderSurface *titaniumGasSkinXeSurface;
	titaniumGasSkinXeSurface = new G4LogicalBorderSurface(
			"titaniumGasSkinXeSurface",
			gaseousSkinXenon, innerTitaniumVessel,
			luxMaterials->GXeTiSurface() );
	
	
	//
	//	The PTFE liner. This is used to increase light collection efficiency in
	//	the skin volumes. It is a daughter of the xenon space because there is
	//	no covering in the top dome, only the straight walls and the bottom
	//	dome (i.e., wherever there is liquid xenon).
	//
	
	//	Combine the main cylindrical section with the bottom dome space. Note
	//	that the straight wall section can extend into the gaseous volume,
	//	depending on the liquid height.

	//	Combine the main vessel section with the flange
	
	G4Tubs *ptfeLinerLiquid_solid1 = new G4Tubs(
			"ptfeLinerLiquid_solid1",
			ptfeLinerInnerTopRadius, ptfeLinerOuterTopRadius,
			ptfeLinerTopStraightHeight/2,
			0.*deg, 360.*deg );


	G4Cons *ptfeLinerLiquid_solid2  = new G4Cons("ptfeLinerLiquid_solid2",
            ptfeLinerInnerBottomRadius, ptfeLinerOuterBottomRadius,
            ptfeLinerInnerTopRadius, ptfeLinerOuterTopRadius,
			titaniumInnerVesselBulgeWallHeight/2, 0.*deg,360.*deg);

	
	zOffset = titaniumInnerVesselBulgeWallHeight/2 +
            ptfeLinerTopStraightHeight/2;

	G4UnionSolid *ptfeLinerLiquid_solid3 = new G4UnionSolid(
						   "ptfeLinerLiquid_solid3",
						   ptfeLinerLiquid_solid2, ptfeLinerLiquid_solid1,
						   0, G4ThreeVector(0,0,zOffset) );

	G4Tubs *ptfeLinerLiquid_solid4 = new G4Tubs(
			"ptfeLinerLiquid_solid4",
			ptfeLinerInnerBottomRadius, ptfeLinerOuterBottomRadius,
			titaniumInnerVesselStraightWallHeightBottom/2,
			0.*deg, 360.*deg );

	zOffset =  titaniumInnerVesselStraightWallHeightBottom/2 +
            titaniumInnerVesselBulgeWallHeight/2;

	G4UnionSolid *ptfeLinerLiquid_solid5 = new G4UnionSolid(
						   "ptfeLinerLiquid_solid5",
						   ptfeLinerLiquid_solid4, ptfeLinerLiquid_solid3,
						   0, G4ThreeVector(0,0,zOffset) );
	
 	
	//	Combine in the bottom dome shape
	G4Ellipsoid *ptfeLinerLiquid_solid6 = new G4Ellipsoid(
			"ptfeLinerLiquid_solid6",
			ptfeLinerInnerBottomRadius,
			ptfeLinerInnerBottomRadius,
			ptfeLinerBottomInnerSemiMajorZHeight,
			-ptfeLinerBottomInnerSemiMajorZHeight,
			0.*cm );

	G4Ellipsoid *ptfeLinerLiquid_solid7 = new G4Ellipsoid(
			"ptfeLinerLiquid_solid7",
			ptfeLinerOuterBottomRadius,
			ptfeLinerOuterBottomRadius,
			ptfeLinerBottomOuterSemiMajorZHeight,
			-ptfeLinerBottomOuterSemiMajorZHeight,
			0.*cm );

	G4SubtractionSolid *ptfeLinerLiquid_solid8 = new G4SubtractionSolid(
			"ptfeLinerLiquid_solid8",
			ptfeLinerLiquid_solid7, ptfeLinerLiquid_solid6,
			0, G4ThreeVector() );
	
	zOffset = -titaniumInnerVesselStraightWallHeightBottom/2;

	G4UnionSolid *ptfeLinerLiquid_solid9 = new G4UnionSolid(
			"ptfeLinerLiquid_solid9",
			ptfeLinerLiquid_solid5, ptfeLinerLiquid_solid8,
			0, G4ThreeVector(0,0,zOffset) );
	
	
	G4LogicalVolume *ptfeLinerLiquid_log = new G4LogicalVolume(
			ptfeLinerLiquid_solid9, luxMaterials->Teflon(),
			"ptfeLinerLiquid_log" );
	ptfeLinerLiquid_log->SetVisAttributes( luxMaterials->TeflonVis() );
	
	zOffset = 0;
	
	LUXSimDetectorComponent *ptfeLinerLiquid;
	ptfeLinerLiquid = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), "PTFELinerLiquid",
			ptfeLinerLiquid_log, liquidSkinXenon, 0, 0, true );
	
	//	The liquid xenon skin / PTFE liner optical boundary
	G4LogicalBorderSurface *liquidSkinXePTFELinerSurface;
	liquidSkinXePTFELinerSurface = new G4LogicalBorderSurface(
			"liquidSkinXePTFELinerSurface",
			liquidSkinXenon, ptfeLinerLiquid,
			luxMaterials->LXeTeflonSurface() );
	

	//	Now do the straight-wall part in the gaseous volume
	G4Tubs *ptfeLinerGas_solid1 = new G4Tubs(
			"gaseousSkinXenon_solid1",
			ptfeLinerInnerTopRadius, ptfeLinerOuterTopRadius,
			 gaseousXenonStraightWallHeight/2,
			0.*deg, 360.*deg );
	
	G4LogicalVolume *ptfeLinerGas_log = new G4LogicalVolume(
			ptfeLinerGas_solid1, luxMaterials->Teflon(),
			"ptfeLinerGas_log" );
	ptfeLinerGas_log->SetVisAttributes( luxMaterials->TeflonVis() );
	
	zOffset = 0;

	LUXSimDetectorComponent *ptfeLinerGas;
	ptfeLinerGas = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), "PTFELinerGas",
			ptfeLinerGas_log, gaseousSkinXenon, 0, 0, true );
	
	//	The gaseous xenon skin / PTFE liner optical boundary
	G4LogicalBorderSurface *gaseousSkinXePTFELinerSurface;
	gaseousSkinXePTFELinerSurface = new G4LogicalBorderSurface(
			"gaseousSkinXePTFELinerSurface",
			gaseousSkinXenon, ptfeLinerGas,
			luxMaterials->GXeTeflonSurface() );
	
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    //Skin PMTs
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    LUXSimLZSkinPMTBank *bottomSkinBank = new LUXSimLZSkinPMTBank("Bottom",75.9*cm,2.*mm, 78.7*cm);
    
    zOffset = titaniumInnerVesselBulgeWallHeight + titaniumInnerVesselStraightWallHeightTop +titaniumInnerVesselStraightWallHeightBottom/2 - gaseousXenonStraightWallHeight - ptfeHeight -
    bottomSkinBank->GeSkinPMTHeight()/2;
    LUXSimDetectorComponent *bottomSkinPMTBank_phys;
    bottomSkinPMTBank_phys = new LUXSimDetectorComponent(0, G4ThreeVector(0,0,zOffset), "BottomSkinBank",
                                                         bottomSkinBank->GetLogicalVolume(), liquidSkinXenon,
                                                         0, 0, true );
    
    G4RotationMatrix * rot180 = new G4RotationMatrix();
    rot180->rotateX(180.*deg);
    
    LUXSimLZSkinPMTBank *topSkinBank = new LUXSimLZSkinPMTBank("Top",75.9*cm,2.*mm, 78.7*cm);
    zOffset = titaniumInnerVesselBulgeWallHeight + titaniumInnerVesselStraightWallHeightTop + titaniumInnerVesselStraightWallHeightBottom/2 - gaseousXenonStraightWallHeight - weirHeight - topSkinBank->GeSkinPMTHeight()/2.;
    LUXSimDetectorComponent *topSkinPMTBank_phys;
    topSkinPMTBank_phys = new LUXSimDetectorComponent(rot180,G4ThreeVector(0,0,zOffset),"TopSkinBank",topSkinBank->GetLogicalVolume(),liquidSkinXenon,0,0,true);
    
    //optical surfaces
    G4int numSkinPMTs = bottomSkinBank->GetNumSkinPMTs();
    G4LogicalBorderSurface *topSkinPTFESurface = new G4LogicalBorderSurface("topSkinPTFESurface",topSkinPMTBank_phys,topSkinBank->GetPTFEReflector(),luxMaterials->LXeTeflonSurface());
    G4LogicalBorderSurface *bottomSkinPTFESurface = new G4LogicalBorderSurface("bottomSkinPTFESurface",bottomSkinPMTBank_phys,bottomSkinBank->GetPTFEReflector(),luxMaterials->LXeTeflonSurface());
    
    G4LogicalBorderSurface *topSkinPTFESurfaceLS = new G4LogicalBorderSurface("topSkinPTFESurfaceLS",liquidSkinXenon,topSkinBank->GetPTFEReflector(),luxMaterials->LXeTeflonSurface());
    G4LogicalBorderSurface *bottomSkinPTFESurfaceLS = new G4LogicalBorderSurface("bottomSkinPTFESurfaceLS",liquidSkinXenon,bottomSkinBank->GetPTFEReflector(),luxMaterials->LXeTeflonSurface());
    
    G4LogicalBorderSurface *skinPMTLXeSurfaceTop, *skinPMTLXeSurfaceBottom, *skinPMTLSurfaceTop, *skinPMTLSurfaceBottom;
    for (G4int i = 0; i < numSkinPMTs; i++) {
        stringstream name;
        name << "skinPMTLXeSurfaceBottom_" << i+1;
        skinPMTLXeSurfaceBottom = new G4LogicalBorderSurface(name.str(),bottomSkinPMTBank_phys,bottomSkinBank->GetSkinPMTPhys(i),luxMaterials->LXeSteelSurface());
        name.str("");
        name << "skinPMTLSurfaceBottom_" << i+1;
        skinPMTLSurfaceBottom = new G4LogicalBorderSurface(name.str(),liquidSkinXenon,bottomSkinBank->GetSkinPMTPhys(i),luxMaterials->LXeSteelSurface());
        name.str("");
        name << "skinPMTLXeSurfaceTop_" << i+1;
        skinPMTLXeSurfaceTop = new G4LogicalBorderSurface(name.str(),topSkinPMTBank_phys,topSkinBank->GetSkinPMTPhys(i),luxMaterials->LXeSteelSurface());
        name.str("");
        name << "skinPMTLSurfaceTop_" << i+1;
        skinPMTLSurfaceTop = new G4LogicalBorderSurface(name.str(),liquidSkinXenon,topSkinBank->GetSkinPMTPhys(i),luxMaterials->LXeSteelSurface());
        
    }
    
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////

    
   	
	//
	//	The PTFE walls in the liquid xenon skin
	//
	G4Tubs *liquidPTFE_solid = new G4Tubs(
			"liquidPTFE_solid",
			innerPTFEFlatRadius, outerPTFEFlatRadius,
			ptfeHeight/2,
			0.*deg, 360.*deg );
	
	G4LogicalVolume *liquidPTFE_log = new G4LogicalVolume(
			liquidPTFE_solid, luxMaterials->Teflon(), "liquidPTFE_log" );
	liquidPTFE_log->SetVisAttributes( luxMaterials->TeflonVis() );

	zOffset = titaniumInnerVesselBulgeWallHeight +
            titaniumInnerVesselStraightWallHeightTop +
            titaniumInnerVesselStraightWallHeightBottom/2 -
            gaseousXenonStraightWallHeight - ptfeHeight/2;
 
	LUXSimDetectorComponent *ptfeWallsInLiquid;
	ptfeWallsInLiquid = new LUXSimDetectorComponent(
			0, G4ThreeVector( 0, 0, zOffset ), "PTFEWallsInLiquid",
			liquidPTFE_log, liquidSkinXenon, 0, 0, true );

	//	The liquid xenon skin / PTFE walls optical boundary
	G4LogicalBorderSurface *liquidSkinXePTFEWallsSurface;
	liquidSkinXePTFEWallsSurface = new G4LogicalBorderSurface(
			"liquidSkinXePTFEWallsSurface",
			liquidSkinXenon, ptfeWallsInLiquid,
			luxMaterials->LXeTeflonSurface() );
	
	//
	//	The PTFE walls in the gaseous xenon skin
	//

	G4Tubs *gaseousPTFE_solid1 = new G4Tubs(
			"gaseousPTFE_solid",
			innerPTFEFlatRadius, outerPTFEFlatRadius,
			ptfeGaseousHeight/2,
			0.*deg, 360.*deg );

	G4LogicalVolume *gaseousPTFE_log = new G4LogicalVolume(
			gaseousPTFE_solid1, luxMaterials->Teflon(), "gaseousPTFE_log" );
	gaseousPTFE_log->SetVisAttributes( luxMaterials->TeflonVis() );

	zOffset = -gaseousXenonStraightWallHeight/2
			+ ptfeGaseousHeight/2;
	
	
	LUXSimDetectorComponent *ptfeWallsInGas;
	ptfeWallsInGas = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), "PTFEWallsInGas",
			gaseousPTFE_log, gaseousSkinXenon, 0, 0, true );

	//	The gaseous xenon skin / PTFE walls optical boundary
	G4LogicalBorderSurface *gaseousSkinXePTFEWallsSurface;
	gaseousSkinXePTFEWallsSurface = new G4LogicalBorderSurface(
			"gaseousSkinXePTFEWallsSurface",
			gaseousSkinXenon, ptfeWallsInGas,
			luxMaterials->GXeTeflonSurface() );
       
	
	
	//
	//	The bottom PMT bank
	//
	
	LUXSimLZDetectorPMTBank *bottomBank = new LUXSimLZDetectorPMTBank(
			luxMaterials->LiquidXe(), "Bottom",
			outerPTFEFlatRadius );
	
	zOffset = titaniumInnerVesselBulgeWallHeight +
            titaniumInnerVesselStraightWallHeightTop +
            titaniumInnerVesselStraightWallHeightBottom/2 -
            gaseousXenonStraightWallHeight - ptfeHeight -
            bottomBank->GetHeight()/2;
	LUXSimDetectorComponent *bottomPMTBank_phys;
	bottomPMTBank_phys = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), "LiquidSkinXenonBank",
			bottomBank->GetLogicalVolume(), liquidSkinXenon, 
			0, 0, true );
    
    
	//	The optical boundary between the encapsulating volume and the titanium
	//	plates and PMT casings. Also need to create an optical boundary between
	//	the PMT casings and the lower volume of liquid xenon skin
    
    G4LogicalBorderSurface *frontTitaniumPlateLXeSurface;
    frontTitaniumPlateLXeSurface = new G4LogicalBorderSurface(
            "frontTitaniumPlateLXeSurface", bottomPMTBank_phys,
            bottomBank->GetTopPlate(), luxMaterials->LXeTiSurface() );
     
    G4LogicalBorderSurface *backPTFELinerLXeSurface;
    backPTFELinerLXeSurface = new G4LogicalBorderSurface(
            "backPTFELinerLXeSurface", bottomPMTBank_phys,
            bottomBank->GetXeSkinLiner(), luxMaterials->LXeTeflonSurface() );
	G4LogicalBorderSurface *bottomTrussLXeSurface;
    bottomTrussLXeSurface = new G4LogicalBorderSurface( "bottomTrussLXeSurface",
            bottomPMTBank_phys, bottomBank->GetTruss(),
            luxMaterials->LXeTiSurface() );

    G4int numPMTs = bottomBank->GetNumPMTs();

    G4LogicalBorderSurface *pmtLXeSurface, *pmtLSkinXeSurface,
            *pmtBaseLSkinXeSurface, *pmtRod1LSkinXeSurface,
            *pmtRod2LSkinXeSurface, *pmtRod3LSkinXeSurface;
	G4LogicalBorderSurface *pmtBaseLSkinXeBankSurface,
            *pmtRod1LSkinXeBankSurface, *pmtRod2LSkinXeBankSurface,
            *pmtRod3LSkinXeBankSurface;

    for( G4int i=0; i<numPMTs; i++ ) {
        stringstream name;
        name << "pmtLXeSurface_" << i+1;
        pmtLXeSurface = new G4LogicalBorderSurface(
                             name.str(), bottomPMTBank_phys,
                             bottomBank->GetPMTPhys(i),
                             luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "pmtLSkinXeSurface_" << i+1;
        pmtLSkinXeSurface = new G4LogicalBorderSurface(
                             name.str(), liquidSkinXenon,
                             bottomBank->GetPMTPhys(i),
                             luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTBaseLSkinXeSurface_" << i +1;
        pmtBaseLSkinXeSurface = new G4LogicalBorderSurface(
                                 name.str(), liquidSkinXenon,
                                 bottomBank->GetPMTBase(i),
                                 luxMaterials->LXeTeflonSurface() );
        name.str("");
        name << "PMTBaseLSkinXeBankSurface_" << i +1;
        pmtBaseLSkinXeBankSurface = new G4LogicalBorderSurface(
                                 name.str(), bottomPMTBank_phys,
                                 bottomBank->GetPMTBase(i),
                                 luxMaterials->LXeTeflonSurface() );


        name.str("");
        name << "PMTRod1LSkinXeSurface_" << i +1;
        pmtRod1LSkinXeSurface = new G4LogicalBorderSurface(
                                 name.str(), liquidSkinXenon,
                                 bottomBank->GetPMTRod1(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod1LSkinXeBankSurface_" << i +1;
        pmtRod1LSkinXeBankSurface = new G4LogicalBorderSurface(
                                 name.str(), bottomPMTBank_phys,
                                 bottomBank->GetPMTRod1(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod2LSkinXeSurface_" << i +1;
        pmtRod2LSkinXeSurface = new G4LogicalBorderSurface(
                                 name.str(), liquidSkinXenon,
                                 bottomBank->GetPMTRod2(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod2LSkinXeBankSurface_" << i +1;
        pmtRod2LSkinXeBankSurface = new G4LogicalBorderSurface(
                                 name.str(), bottomPMTBank_phys,
                                 bottomBank->GetPMTRod2(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod3LSkinXeSurface_" << i +1;
        pmtRod3LSkinXeSurface = new G4LogicalBorderSurface(
                                 name.str(), liquidSkinXenon,
                                 bottomBank->GetPMTRod3(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod3LSkinXeBankSurface_" << i +1;
        pmtRod3LSkinXeBankSurface = new G4LogicalBorderSurface(
                                 name.str(), bottomPMTBank_phys,
                                 bottomBank->GetPMTRod3(i),
                                 luxMaterials->LXeTeflonSurface() );

    }


	//
	//	The inner liquid xenon
	//
	
	G4Tubs *innerLiquidXenon_solid = new G4Tubs(
			"innerLiquidXenon_solid",
			0, innerPTFEFlatRadius,
			ptfeHeight/2,
			0.*deg, 360.*deg );

	
	G4LogicalVolume *innerLiquidXenon_log = new G4LogicalVolume(
			innerLiquidXenon_solid, luxMaterials->LiquidXe(),
			"innerLiquidXenon_log" );
	innerLiquidXenon_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
	
	zOffset = titaniumInnerVesselBulgeWallHeight +
            titaniumInnerVesselStraightWallHeightTop +
            titaniumInnerVesselStraightWallHeightBottom/2 -
            gaseousXenonStraightWallHeight -  ptfeHeight/2;

	LUXSimDetectorComponent *innerLiquidXenon;
	innerLiquidXenon = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), "InnerLiquidXenon",
			innerLiquidXenon_log, liquidSkinXenon, 0, 0, true );
	
	//	The inner liquid xenon / PTFE wall optical boundary
	G4LogicalBorderSurface *innerLiquidXePTFEWallSurface;
	innerLiquidXePTFEWallSurface = new G4LogicalBorderSurface(
			"innerLiquidXePTFEWallSurface",
			innerLiquidXenon, ptfeWallsInLiquid,
			luxMaterials->LXeTeflonSurface() );
	
	
	//	Now place the PMT bank liner at the bottom of this same volume and
	//	create the associated optical boundaries
	zOffset = -ptfeHeight/2 + bottomBank->GetBankLinerThickness()/2;
	LUXSimDetectorComponent *bottomBankPTFELiner;
	
	bottomBankPTFELiner = new LUXSimDetectorComponent(
				0, G4ThreeVector(0,0,zOffset), "BottomPTFELiner",
				bottomBank->GetTeflonLinerLogical(), innerLiquidXenon, 
				0, 0, true );
	
	G4LogicalBorderSurface *ptfeLinerLXeSurface;
	ptfeLinerLXeSurface = new G4LogicalBorderSurface(
			"ptfeLinerLXeSurface", innerLiquidXenon, bottomBankPTFELiner,
			luxMaterials->LXeTeflonSurface() );
	
	G4LogicalBorderSurface *bottomPMT_PTFESurface;
	for( G4int i=0; i<numPMTs; i++ ) {
		stringstream name;
		name << "bottomPMT_PTFESurface_" << i+1;
		bottomPMT_PTFESurface = new G4LogicalBorderSurface(
			name.str(),
			bottomBank->GetPMT(i)->GetPMTWindow(), bottomBankPTFELiner,
			luxMaterials->QuartzTeflonSurface() );
	}
	
	
	//
	//	Active liquid xenon target (the one that's actually 7 tonnes)
	//
	
    LUXSimDetectorComponent *liquidXenonTarget;
	if( activeLXeTargetHeight ) {

	  G4Tubs *liquidXenonTarget_solid = new G4Tubs(
	  		"liquidXenonTarget_solid",
	  		0, innerPTFEFlatRadius,
	  		activeLXeTargetHeight/2,
			0.*deg, 360.*deg );

		G4LogicalVolume *liquidXenonTarget_log = new G4LogicalVolume(
				liquidXenonTarget_solid, luxMaterials->LiquidXe(),
				"liquidXenonTarget_log" );
		liquidXenonTarget_log->SetVisAttributes( luxMaterials->LiquidXeVis() );
		
		zOffset = (cathodeToBottomGrid + bottomGridToBottomPMTBank)/2;
		
		liquidXenonTarget = new LUXSimDetectorComponent(
				0, G4ThreeVector(0,0,zOffset), "LiquidXenonTarget",
				liquidXenonTarget_log, innerLiquidXenon, 0, 0, true );
		
		//	The optical boundary between the liquid xenon target and the PTFE
		//	walls.
		
		G4LogicalBorderSurface *liquidXeTargetPTFEWallsSurface;
		liquidXeTargetPTFEWallsSurface = new G4LogicalBorderSurface(
				"liquidXeTargetPTFEWallsSurface",
				liquidXenonTarget, ptfeWallsInLiquid,
				luxMaterials->LXeTeflonSurface() );
	    
		
	} else {
	
		G4cout << G4endl << G4endl << G4endl;
		G4cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << G4endl;
		G4cout << "Because the liquid height was set to "
			   << liquidHeight/mm << " mm, there is no" << G4endl;
		G4cout << "active liquid xenon target volume." << G4endl;
		G4cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << G4endl;
		G4cout << G4endl << G4endl << G4endl;
	
	}
	
	
	//
	//	The top PMT bank
	//
	
	LUXSimLZDetectorPMTBank *topBank = new LUXSimLZDetectorPMTBank(
			luxMaterials->GasXe(), "Top",
			outerPTFEFlatRadius );
	
	
	zOffset = -gaseousXenonStraightWallHeight/2 + ptfeGaseousHeight +
            bottomBank->GetHeight()/2;;

	LUXSimDetectorComponent *topPMTBank_phys;
	topPMTBank_phys = new LUXSimDetectorComponent( rot180, 
			G4ThreeVector(0,0,zOffset), 
			"GaseousSkinXenonBank", topBank->GetLogicalVolume(),
			gaseousSkinXenon, 0, 0, true );

	
	//	The optical boundary between the encapsulating volume and the titanium
	//	plates and PMT casings. Also need to create an optical boundary between
	//	the PMT casings and the upper volume of gaseous xenon skin

	G4LogicalBorderSurface *frontTitaniumPlateGXeSurface;
        frontTitaniumPlateGXeSurface = new G4LogicalBorderSurface(
								  "frontTitaniumPlateGXeSurface", topPMTBank_phys,
								  topBank->GetTopPlate(),
								  luxMaterials->GXeTiSurface() );

    G4LogicalBorderSurface *backPTFELinerGXeSurface;
    backPTFELinerGXeSurface = new G4LogicalBorderSurface(
            "backPTFELinerGXeSurface", topPMTBank_phys,
            topBank->GetXeSkinLiner(), luxMaterials->GXeTeflonSurface() );
	G4LogicalBorderSurface *topTrussGXeSurface;
    topTrussGXeSurface = new G4LogicalBorderSurface( "topTrussGXeSurface",
            topPMTBank_phys, topBank->GetTruss(), luxMaterials->GXeTiSurface());

    numPMTs = topBank->GetNumPMTs();

    G4LogicalBorderSurface *pmtGXeSurface, *pmtGSkinXeSurface,
            *pmtBaseGSkinXeSurface, *pmtRod1GSkinXeSurface,
            *pmtRod2GSkinXeSurface,  *pmtRod3GSkinXeSurface;
	G4LogicalBorderSurface *pmtBaseGSkinXeBankSurface,
            *pmtRod1GSkinXeBankSurface, *pmtRod2GSkinXeBankSurface,
            *pmtRod3GSkinXeBankSurface;

    for( G4int i=0; i<numPMTs; i++ ) {
        stringstream name;
        name << "pmtGXeSurface_" << i+1;
        pmtGXeSurface = new G4LogicalBorderSurface(
                             name.str(), topPMTBank_phys,
                             topBank->GetPMTPhys(i),
                             luxMaterials->GXeTeflonSurface() );

        name.str("");
        name << "pmtGSkinXeSurface_" << i+1;
        pmtGSkinXeSurface = new G4LogicalBorderSurface(
                             name.str(), gaseousSkinXenon,
                             topBank->GetPMTPhys(i),
                             luxMaterials->GXeTeflonSurface() );

        name.str("");
        name << "PMTBaseGSkinXeSurface_" << i +1;
        pmtBaseGSkinXeSurface = new G4LogicalBorderSurface(
                                 name.str(), gaseousSkinXenon,
                                 topBank->GetPMTBase(i),
                                 luxMaterials->LXeTeflonSurface() );
        name.str("");
        name << "PMTBaseGSkinXeBankSurface_" << i +1;
        pmtBaseGSkinXeBankSurface = new G4LogicalBorderSurface(
                                 name.str(), topPMTBank_phys,
                                 topBank->GetPMTBase(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod1GSkinXeSurface_" << i +1;
        pmtRod1GSkinXeSurface = new G4LogicalBorderSurface(
                                 name.str(), gaseousSkinXenon,
                                 topBank->GetPMTRod1(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod1GSkinXeBankSurface_" << i +1;
        pmtRod1GSkinXeBankSurface = new G4LogicalBorderSurface(
                                 name.str(), topPMTBank_phys,
                                 topBank->GetPMTRod1(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod2GSkinXeSurface_" << i +1;
        pmtRod2GSkinXeSurface = new G4LogicalBorderSurface(
                                 name.str(), gaseousSkinXenon,
                                 topBank->GetPMTRod2(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod2GSkinXeBankSurface_" << i +1;
        pmtRod2GSkinXeBankSurface = new G4LogicalBorderSurface(
                                 name.str(), topPMTBank_phys,
                                 topBank->GetPMTRod2(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod3GSkinXeSurface_" << i +1;
        pmtRod3GSkinXeSurface = new G4LogicalBorderSurface(
                                 name.str(), gaseousSkinXenon,
                                 topBank->GetPMTRod3(i),
                                 luxMaterials->LXeTeflonSurface() );

        name.str("");
        name << "PMTRod3GSkinXeBankSurface_" << i +1;
        pmtRod3GSkinXeBankSurface = new G4LogicalBorderSurface(
                                 name.str(), topPMTBank_phys,
                                 topBank->GetPMTRod3(i),
                                 luxMaterials->LXeTeflonSurface() );

    }
	
	//
	//	The inner gaseous xenon
	//
	G4Tubs *innerGaseousXenon_solid = new G4Tubs(
			"innerGaseousXenon_solid",
			0, innerPTFEFlatRadius,
			ptfeGaseousHeight/2,
			0.*deg, 360.*deg );
	
	G4LogicalVolume *innerGaseousXenon_log = new G4LogicalVolume(
			innerGaseousXenon_solid, luxMaterials->GasXe(),
			"innerGaseousXenon_log" );
	innerGaseousXenon_log->SetVisAttributes( luxMaterials->GasXeVis() );

	zOffset = -gaseousXenonStraightWallHeight/2 + ptfeGaseousHeight/2;
	
	LUXSimDetectorComponent *innerGaseousXenon;
	innerGaseousXenon = new LUXSimDetectorComponent(
			0, G4ThreeVector(0,0,zOffset), "InnerGaseousXenon",
			innerGaseousXenon_log, gaseousSkinXenon, 0, 0, true );

	//	The main gaseous xenon target / PTFE wall optical boundary
	G4LogicalBorderSurface *innerGaseousXePTFEWallSurface;
	innerGaseousXePTFEWallSurface = new G4LogicalBorderSurface(
			"innerGaseousXePTFEWallSurface",
			innerGaseousXenon, ptfeWallsInGas,
			luxMaterials->GXeTeflonSurface() );
	
	//	Now place the PMT bank liner at the top of this same volume and
	//	create the associated optical boundaries
	zOffset = ptfeGaseousHeight/2 - bottomBank->GetBankLinerThickness()/2;
	LUXSimDetectorComponent *topBankPTFELiner;
	topBankPTFELiner = new LUXSimDetectorComponent(
				0, G4ThreeVector(0,0,zOffset), "TopPTFELiner",
				topBank->GetTeflonLinerLogical(), innerGaseousXenon, 
				0, 0, true );
	
	G4LogicalBorderSurface *ptfeLinerGXeSurface;
	ptfeLinerGXeSurface = new G4LogicalBorderSurface(
			"ptfeLinerGXeSurface", innerGaseousXenon, topBankPTFELiner,
			luxMaterials->GXeTeflonSurface() );
	G4LogicalBorderSurface *topPMT_PTFESurface;
	for( G4int i=0; i<numPMTs; i++ ) {
		stringstream name;
		name << "topPMT_PTFESurface_" << i+1;
		topPMT_PTFESurface = new G4LogicalBorderSurface(
			name.str(),
			topBank->GetPMT(i)->GetPMTWindow(), topBankPTFELiner,
			luxMaterials->QuartzTeflonSurface() );
	}

    //
    //  The grids
    //
    //  Normally I'd just re-use the LUXSimGrid class that already exists, but
    //  unfortunately that class has hard-coded volume name dependencies that
    //  really only allow it to work with the LUX detector. I'm loathe to change
    //  that code since it's working, so I'm just going to roll my own grids
    //  here.
    //
    if( useGrids == "on" ) {
    
        time_t startTime, endTime;
        time( &startTime );
    
        G4cout << "Creating the grid wires:" << endl;
    
        //  Set wire specs
        G4double wireSpacing[5] = { 0.5*cm, 0.0254*cm, 0.5*cm, 0.5*cm, 0.5*cm};
        G4double wireDiameter[5] = { 50.8*um, 30.5*um, 101.6*um, 206.*um,
                206.*um };
        G4String holderName[5] = {
                "InnerGaseousXenonTopGridHolder",
                "InnerGaseousXenonAnodeGridHolder",
                "LXeTargetGateGridHolder",
                "InnerLiquidXenonCathodeGridHolder",
                "InnerLiquidXenonBottomGridHolder" };
        G4Material *holderMaterial[5] = {
                luxMaterials->GasXe(),
                luxMaterials->GasXe(),
                luxMaterials->LiquidXe(),
                luxMaterials->LiquidXe(),
                luxMaterials->LiquidXe() };
        LUXSimDetectorComponent *motherVolume[5] = {
                innerGaseousXenon,
                innerGaseousXenon,
                liquidXenonTarget,
                innerLiquidXenon,
                innerLiquidXenon };
        G4VisAttributes *holderVisAttributes[5] = {
                luxMaterials->GasXeVis(),
                luxMaterials->GasXeVis(),
                luxMaterials->LiquidXeVis(),
                luxMaterials->LiquidXeVis(),
                luxMaterials->LiquidXeVis() };
        G4double gridZOffsets[5] = {
                -ptfeGaseousHeight/2 + 0.5*cm + 3.8*cm,
                -ptfeGaseousHeight/2 + 0.5*cm,
                activeLXeTargetHeight/2 - 0.5*cm,
                -ptfeHeight/2 + 14.6*cm,
                -ptfeHeight/2 + 1.0*cm };
        LUXSimDetectorComponent *surroundingVolume[5] = {
                ptfeWallsInGas,
                ptfeWallsInGas,
                ptfeWallsInLiquid,
                ptfeWallsInLiquid,
                ptfeWallsInLiquid };
        G4OpticalSurface *holderOpticalSurface[5] = {
                luxMaterials->GXeTeflonSurface(),
                luxMaterials->GXeTeflonSurface(),
                luxMaterials->LXeTeflonSurface(),
                luxMaterials->LXeTeflonSurface(),
                luxMaterials->LXeTeflonSurface() };
        G4OpticalSurface *gridOpticalSurface[5] = {
                luxMaterials->GXeSteelSurface(),
                luxMaterials->GXeSteelSurface(),
                luxMaterials->LXeSteelSurface(),
                luxMaterials->LXeSteelSurface(),
                luxMaterials->LXeSteelSurface() };
        G4String wireName[5] = { "Top", "Anode", "Gate", "Cathode", "Bottom" };
        
        //  Set up all the loop variables. We need six(?) large arrays for the
        //  grid wires because the anode is actually a mesh (i.e., two crossed
        //  grids).
        G4Tubs **gridWire_solid[5], *gridHolder_solid[5];
        G4LogicalVolume **gridWire_log[5], *gridHolder_log[5];
        LUXSimDetectorComponent **gridWire[5], *gridHolder[5];
        G4LogicalBorderSurface **gridWire_surface[5], *gridHolder_surface[5];
        
        G4Tubs *anodeSubHolder_solid[4];
        G4LogicalVolume *anodeSubHolder_log[4];
        LUXSimDetectorComponent *anodeSubHolder[4];

        G4Tubs *cathodeSubHolder_solid[4];
        G4LogicalVolume *cathodeSubHolder_log[4];
        LUXSimDetectorComponent *cathodeSubHolder[4];
        
        G4RotationMatrix *rotX90 = new G4RotationMatrix();
        rotX90->rotateX(90.*deg);
        
        stringstream name;
        for( G4int i=0; i<5; i++ ) {
            //  For the anode and the cathode, we have a mesh instead of a grid, so the holder
            //  needs to accommodate both grids plus 1 micron space on either
            //  side plus 1 micron space in the middle.
            G4double halfHolderThickness = (wireDiameter[i] + 2.*um)/2.;
            if( i == 1 || i == 3)    //  Anode or cathode mesh
                halfHolderThickness = (2.*wireDiameter[i] + 3.*um)/2.;

            //  First create the holders
            name.str("");
            name << holderName[i] << "_solid";
            gridHolder_solid[i] = new G4Tubs( name.str(), 0,
                    innerPTFEFlatRadius, halfHolderThickness, 0.*deg, 360.*deg);

            name.str("");
            name << holderName[i] << "_log";
            gridHolder_log[i] = new G4LogicalVolume( gridHolder_solid[i],
                    holderMaterial[i], name.str() );
            gridHolder_log[i]->SetVisAttributes( holderVisAttributes[i] );
            
            if( i == 1 )
                gridHolder[i] = new LUXSimDetectorComponent( 0,
                        G4ThreeVector(0,0,gridZOffsets[i]),
                        "Inner_Gaseous_Xenon_Anode_Grid_Holder",
                        gridHolder_log[i], motherVolume[i], 0, 0, false );
            else if( i == 3 )
                gridHolder[i] = new LUXSimDetectorComponent( 0,
                        G4ThreeVector(0,0,gridZOffsets[i]),
                        "Inner_Liquid_Xenon_Cathode_Grid_Holder",
                        gridHolder_log[i], motherVolume[i], 0, 0, false );
            else
                gridHolder[i] = new LUXSimDetectorComponent( 0,
                        G4ThreeVector(0,0,gridZOffsets[i]), holderName[i],
                        gridHolder_log[i], motherVolume[i], 0, 0, false );
            
            name.str("");
            name << holderName[i] << "Surface";
            gridHolder_surface[i] = new G4LogicalBorderSurface( name.str(),
                    gridHolder[i], surroundingVolume[i],
                    holderOpticalSurface[i] );
            
            xOffset = yOffset = zOffset = 0;
            
            //  Handle the case of the anode / cathode mesh. Because the mesh is comprised
            //  of over 10,000 wires, it takes an exceedingly long time just to
            //  build the geometry. To speed up the construction, the mesh is
            //  sub-divided into four volumes, each with ~2600 wires. Each sub-
            //  volume is 1 nm smaller in radius so that the optical boundary
            //  between the sub-holder and the PTFE walls remains intact.
            if( i == 1 || i == 3) {

                zOffset = halfHolderThickness/2;
                
                G4RotationMatrix *rot[4];
                rot[0] = 0;

                rot[1] = new G4RotationMatrix();
                rot[1]->rotateZ(180.*deg);

                rot[2] = new G4RotationMatrix();
                rot[2]->rotateZ(90.*deg);
                rot[2]->rotateY(180.*deg);

                rot[3] = new G4RotationMatrix();
                rot[3]->rotateZ(-90.*deg);
                rot[3]->rotateY(180.*deg);
                
                //  Create the anode / cathode mesh sub-holders
                for( G4int j=0; j<4; j++ ) {
                    name.str("");
                    name << holderName[i] << "_Subvolume_solid_" << j+1;
                    if (i == 1)
                        anodeSubHolder_solid[j] = new G4Tubs( name.str(), 0,
                                innerPTFEFlatRadius - 1.*nm, halfHolderThickness/2,
                                90.*deg, 180.*deg );
                    else if (i == 3)
                        cathodeSubHolder_solid[j] = new G4Tubs( name.str(), 0,
                                innerPTFEFlatRadius - 1.*nm, halfHolderThickness/2,
                                90.*deg, 180.*deg );
                    else
                    {
                        cout << "We shouldn't be here. Exit!" << endl;
                        exit(1);
                    }
                    
                    name.str("");
                    name << holderName[i] << "_Subvolume_log_" << j+1;
                    if (i == 1)
                    {
                        anodeSubHolder_log[j] = new G4LogicalVolume(
                                anodeSubHolder_solid[j], holderMaterial[i],
                                name.str() );
                        anodeSubHolder_log[j]->SetVisAttributes(
                                holderVisAttributes[i] );
                    }
                    else if (i == 3)
                    {
                        cathodeSubHolder_log[j] = new G4LogicalVolume(
                                cathodeSubHolder_solid[j], holderMaterial[i],
                                name.str() );
                        cathodeSubHolder_log[j]->SetVisAttributes(
                                holderVisAttributes[i] );
                    }
                    else
                    {
                        cout << "We shouldn't be here. Exit!" << endl;
                        exit(1);
                    }
                    
                    if( j == 2 || j == 3 )
                        zOffset = -halfHolderThickness/2;
                    name.str("");
                    name << holderName[i] << "_Subvolume_" << j+1;
                    if (i == 1)
                        anodeSubHolder[j] = new LUXSimDetectorComponent( rot[j],
                                G4ThreeVector(0,0,zOffset), name.str(),
                                anodeSubHolder_log[j], gridHolder[i], 0, 0, false );
                    else if (i == 3)
                        cathodeSubHolder[j] = new LUXSimDetectorComponent( rot[j],
                                G4ThreeVector(0,0,zOffset), name.str(),
                                cathodeSubHolder_log[j], gridHolder[i], 0, 0, false );
                    else
                    {
                        cout << "We shouldn't be here. Exit!" << endl;
                        exit(1);
                    }
                }
            }
            
            //
            //  Now create the wires and place them in the holders
            //

            G4int numWires = innerPTFEFlatRadius*2 / wireSpacing[i];
            //  If we're dealing with the mesh, then we need to allocate enough
            //  space for twice as many wires.
            if( i == 1 || i == 3)
                numWires *= 2;
            G4cout << "\t" << numWires << " wires for " << holderName[i]
                   << "..." << endl;

            gridWire_solid[i] = new G4Tubs*[numWires];
            gridWire_log[i] = new G4LogicalVolume*[numWires];
            gridWire[i] = new LUXSimDetectorComponent*[numWires];
            gridWire_surface[i] = new G4LogicalBorderSurface*[numWires];
            
            //  If we're dealing with the mesh, reduce the number of wires by
            //  a factor of 2 to have the proper calculation of the X offset for
            //  each layer in the mesh.
            if( i == 1 || i == 3)
                numWires /= 2;
            if( numWires%2 == 0 )
                xOffset = wireSpacing[i]*(0.5 - numWires/2);
            else
                xOffset = -(G4int)(numWires/2) * wireSpacing[i];
            //  And again, if we're dealing with the mesh, reduce the number
            //  of wires by another factor of 2 because we're sub-dividing each
            //  pancake in half.
            if( i == 1 || i == 3)
                numWires /= 2;
            
            //  Reset the vertical offset for the wires within the holder (or
            //  sub-holder) volumes
            zOffset = 0;
            if( i == 1 || i == 3 )
                zOffset = -halfHolderThickness/2 + wireDiameter[i]/2 + 0.5*um;
            
            for( G4int j=0; j<numWires; j++ ) {
                
                G4double wireLength = sqrt( pow(innerPTFEFlatRadius,2) -
                        pow(fabs(xOffset)+wireDiameter[i]/2,2) ) - 1.*um;
                
                //  If we're dealing with the anode mesh, then we need to loop
                //  over all four anode sub-holders.
                G4int numLoops = 1;
                if( i == 1 || i == 3)
                    numLoops = 4;
                
                for( G4int k=0; k<numLoops; k++ ) {
                    G4int wireIndex = j;
                    if( i == 1 || i == 3)
                        wireIndex = j*4 + k;
                
                    if( !(wireIndex%100) ) {
                        time( &endTime );
                        G4cout << "\t\tPlaced " << wireIndex << " grid wires ("
                               << difftime( endTime, startTime) << " seconds)"
                               << G4endl;
                    }

                    name.str("");
                    name << wireName[i] << "GridWire_" << wireIndex+1
                         << "_solid";
                    gridWire_solid[i][wireIndex] = new G4Tubs( name.str(), 0,
                            wireDiameter[i]/2, wireLength, 0.*deg, 360.*deg );
                    
                    name.str("");
                    name << wireName[i] << "GridWire_" << wireIndex+1 << "_log";
                    gridWire_log[i][wireIndex] = new G4LogicalVolume(
                            gridWire_solid[i][wireIndex], luxMaterials->Steel(),
                            name.str() );
                    gridWire_log[i][wireIndex]->SetVisAttributes(
                            luxMaterials->SteelVis());
                    
                    name.str("");
                    name << wireName[i] << "GridWire_" << wireIndex+1;
                    if( i == 1 )
                        gridWire[i][wireIndex] = new LUXSimDetectorComponent(
                                rotX90, G4ThreeVector(xOffset,0,zOffset),
                                name.str(), gridWire_log[i][wireIndex],
                                anodeSubHolder[k], 0, 0, false );
                    else if (i == 3)
                        gridWire[i][wireIndex] = new LUXSimDetectorComponent(
                                rotX90, G4ThreeVector(xOffset,0,zOffset),
                                name.str(), gridWire_log[i][wireIndex],
                                cathodeSubHolder[k], 0, 0, false );
                    else
                        gridWire[i][wireIndex] = new LUXSimDetectorComponent(
                                rotX90, G4ThreeVector(xOffset,0,zOffset),
                                name.str(), gridWire_log[i][wireIndex],
                                gridHolder[i], 0, 0, false );
                    
                    name.str("");
                    name << wireName[i] << "GridWireSurface_" << wireIndex+1;
                    if( i == 1 )
                        gridWire_surface[i][wireIndex] = new
                                G4LogicalBorderSurface( name.str(),
                                anodeSubHolder[k], gridWire[i][wireIndex],
                                gridOpticalSurface[i] );
                    else if (i == 3)
                        gridWire_surface[i][wireIndex] = new
                                G4LogicalBorderSurface( name.str(),
                                cathodeSubHolder[k], gridWire[i][wireIndex],
                                gridOpticalSurface[i] );
                    else
                        gridWire_surface[i][wireIndex] = new
                                G4LogicalBorderSurface( name.str(),
                                gridHolder[i], gridWire[i][wireIndex],
                                gridOpticalSurface[i] );
                }
                
                xOffset += wireSpacing[i];
            }
        }
        
        time( &endTime );
        G4cout << "Time to place all wires: " << difftime( endTime, startTime )
               << " seconds" << endl;
    }
}
