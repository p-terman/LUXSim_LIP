////////////////////////////////////////////////////////////////////////////////
/*	LUXSimDetectorConstruction.cc
*
* This is the code file to build the detector geometry within LUXSim.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	7 April 2009 - Added support for the muon veto and the LUX1.0 detector
*		   	(Melinda)
*	30 April 2009 - Made detetor placement independent of muonVeto placement
*			and added empty cryostat option (Melinda)
*	27-Aug-09 - Changed the overlap checking to false, added explicit 
*			overlap check (Kareem)
*	10-Sep-09 - Added LUX 0.1 detector option (Dave M.)
*	28-Sept-09 - Added cryostat stand placement (Melinda)
*	29-Sept-09 - Added grid options, passing bool to LUX1_0Detector 
*			(Melinda)
*	01-Oct-09 - Code cleanup (Melinda)
*	09-Nov-09 - Changed variable declaration to avoid compilation warnings about
*				unused variables (Kareem)
*	12-Jan-10 - Added LZ20Detector option (Melinda)
*	23-Feb-10 - Added case to handle no set geometry (Dave)
*	05-Mar-10 - Got rid of WIRES (Melinda)
*	19-May-10 - Added NMDetector (Melinda)
*	26-May-10 - Added LZ20 Simple Detector (Monica)
*	15-June-10 - Changes to NMD Placement (Melinda)
*	20-Sep-10 - Added a SetNewMaterials method to accommodate simple material
*				optical parameters for debugging purposes (Kareem)
*   24-Sep-10 - Deleted local variable declaration overwriting LUXSimStand 
*               (Dave)
*	27-Sep-10 - Removed the SetNewMaterials method, and reference the
*				LUXSimMaterials class properly as a singleton (Kareem)
*	24-Nov-10 - Added the LLNL single-phase detector to the detector list
*				(Kareem)
*	24-Aug-11 - Added the GNARRLI detector to the detector list (Tenzing)
*	18-Jan-12 - Added functions for changing material properties (Dave)
*	24-Feb-12 - Added the LZFlex detector to the detector list (Kareem)
*	26-Feb-12 - Changed the default scintillator thickness of the LZFlex
*				detector to 75 cm (Kareem)
*	01-Mar-12 - Changed the behavior of setting the mass or thicknesses to 0.
*				This is no longer allowed, and instead the minimum mass is 1
*				gram, while the minimum thicknesses is 1 micron. (Kareem)
*	06-Mar-12 - Added functions allowing for manual setting of detector Z
*				location (Dave)
*	09-Mar-12 - Added pyramid when muon veto is on (Dave)
*	09-Mar-12 - Added support for Rayleigh UI hooks (Dave)
*	13-Apr-12 - Changed the Z placement of the water tank so that the origin of
*				the LUX detector is at the center of the top surface of the
*				bottom PMTs. This setting was done both by setting the tankZ
*				variable as well as the detZ variable when the water tank isn't
*				present. (Kareem)
*	14-Apr-12 - Fixed bugs in the vertical offset of the LUX detector for when
*				the water tank (a.k.a. muon veto) is turned on or off (Kareem)
*	26-May-12 - Added support for the official LZ detector (Kareem)
*   16-Aug-12 - Added PMT numbering option (real v. old simulation) 
*               for 1.0 Detector
*	25-Sep-12 - Added toggle for LUX1_0Detector gas run (Dave)
*	01-Mar-13 - Changed the default value of detZ so that the top face of the
*				bottom PMT windows are once again at z = 0 with the re-coded
*				LUX1.0 geometry (Kareem)
*	09-Apr-13 - The 17-degree underground rotation now occurs in the water tank
*				rather than the detector internals, to keep the x- and y-axes
*				symmetric on the PMTs (Kareem)
*	14-Apr-13 - Added support for the LZSimple detector, including proper Z
*               offset (Kareem)
*	15-Apr-13 - Changed the detector rotation from 17 degrees to 15 degrees
*				(Kareem)
*	23-Apr-13 - Added a neutron calibration tube to the water tank, when
*				requested, and right now only for the LZSimple geometry (Kareem)
*   23-Jun-13 - Added a TOP neutron calibration tube to the water tank, when
*               requested, and right now only for the LZSimple geometry (Kareem)
*   10-Sep-13 - Fixed the offset for the LZDetector to zero in z (Monica)
*   18-Sep-13 - Put an encomapssing volume around the water tank to enable
*               studies of particles emitted from the water tank. (Kareem)
*   26-Sep-13 - Added an offset for the 1_0Detector, LZDetector, and LZSimple
*               geometries so that Z = 0 is always at the correct location. This
*               required making the experimentalHall volume a daughter of the
*               universe volume. (Kareem)
*   30-Sep-13 - Removed output concerning Z offsets (was used for debugging, no
*               longer necessary). (Kareem)
*   03-Mar-14 - Added support for turning grids on and off in LZ (Kareem)
*   07-Apr-14 - Force the muon veto "on" if the LZDetector is selected (Kareem)
*   30-Apr-14 - Changed vertical position of LZDetector so it rests on the bottom
*               of water tank. Currently the z position is hardcoded, sorry.
*               (Scott Ha.)
*   30-Apr-14 - Changed vertical position of LZDetector so it rests on the
*               bottom of water tank. Currently the z position is hardcoded,
*               sorry. (Scott Ha.)
*   15-May-14 - Removed the screen output of the detector midpoint, since really
*               it was just confusing the issue (and was incorrect to boot)
*               (Kareem)
*   15-May-14 - Updated LZ neutron calibration tube geometry. (Kevin)
*   23-May-14 - Added variable initialization to a few variables to eliminate
*               a compiler warning (Kareem)
*   28-May-14 - Added flag to turn on LZ veto system. It is set "on" when
*               "LZDetector" geometry is selected. (Scott Ha.)
*   21-Aug-14 - Fixed a bug where the Z offset as set by the macro command was
*               being completely ignored (Kareem)
*   16-Dec-14 - Changed LZDetector geometry so that mother volume in this file is 
*               the water tank.  No more /LZveto on/off command necessary. (Scott Ha.)
*   08-Jan-15 - Forced muonVeto to "off" when using LZDetector (Scott Ha.)
*   02-Fed-15 - Added the DD Neutron source (Kevin)
*   15-Jul-15 - Added cavern rock (David W)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4RunManager.hh"
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimDetectorConstruction.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimManager.hh"

#include "LUXSimDetector.hh"
#include "LUXSimExampleSimpleDetector.hh"
#include "LUXSimExampleComplexDetector.hh"
#include "LUXSim0_1Detector.hh"
#include "LUXSim1_0Detector.hh"
#include "LUXSimGNARRLIDetector.hh"
#include "LUXSimLZDetector.hh"
#include "LUXSimLZSimple.hh"
#include "LUXSimLZFlex.hh"
#include "LUXSimLZ20Detector.hh"
#include "LUXSimLZ20Simple.hh"
#include "LUXSimNMDetector.hh"
#include "LUXSimEmptyCryostat.hh"
#include "LUXSimMuonVetoSystem.hh"
#include "LUXSimPyramid.hh"
#include "LUXSimStand.hh"
#include "LUXSimLLNL_SinglePhase.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimDetectorConstruction()
//------++++++------++++++------++++++------++++++------++++++------++++++------

//	In the constructor, we just set all pointers to 0
LUXSimDetectorConstruction::LUXSimDetectorConstruction()
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
	materials = LUXSimMaterials::GetMaterials();
	
	detectorSpecified = false;
	
	detZ = -27.350 * mm;
    detZ = 0;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimDetectorConstruction()
//------++++++------++++++------++++++------++++++------++++++------++++++------

LUXSimDetectorConstruction::~LUXSimDetectorConstruction()
{
	delete materials;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					Construct()
//------++++++------++++++------++++++------++++++------++++++------++++++------
//	This method is required by the GEANT4 simulation
G4VPhysicalVolume *LUXSimDetectorConstruction::Construct()
{
	return ConstructDetector();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					ConstructDetector()
//------++++++------++++++------++++++------++++++------++++++------++++++------
//	This method is required by the GEANT4 simulation
G4VPhysicalVolume *LUXSimDetectorConstruction::ConstructDetector()
{
    //  Entire universe
    G4double universe_x = 1000.*m;
    G4double universe_y = 1000.*m;
    G4double universe_z = 1000.*m;
    
	G4Box *universe_box = new G4Box( "universe_box", universe_x,
			universe_y, universe_z );
	G4LogicalVolume *universe_log = new G4LogicalVolume(
			universe_box, materials->Vacuum(),"universe_log",
			0, 0, 0 );
	universe_log->SetVisAttributes( G4VisAttributes::Invisible );
	universe = new LUXSimDetectorComponent( 0, G4ThreeVector(),
			universe_log, "universe", 0, 0, 0, false );

	//	Experimental hall
	G4double expHall_x = 100.*m;
	G4double expHall_y = 100.*m;
	G4double expHall_z = 100.*m;

	G4Box *experimentalHall_box = new G4Box( "expHall_box", expHall_x,
			expHall_y, expHall_z );
	G4LogicalVolume *experimentalHall_log = new G4LogicalVolume(
			experimentalHall_box, materials->Vacuum(),"expHall_log",
			0, 0, 0 );
	experimentalHall_log->SetVisAttributes( G4VisAttributes::Invisible );
	LUXSimDetectorComponent *experimentalHall = new LUXSimDetectorComponent( 0,
            G4ThreeVector(), "expHall", experimentalHall_log, universe,
            0, 0, false );
    
    G4Tubs *encompassingVolume_solid1 = new G4Tubs( "encompassingVolume_solid1",
            0.*m, 5.001*m, 5.001*m, 0, 360.*deg );
    G4Tubs *encompassingVolume_solid2 = new G4Tubs( "encompassingVolume_solid2",
            0.*m, 5*m, 5*m, 0, 360.*deg );
    G4SubtractionSolid *encompassingVolume_solid = new G4SubtractionSolid(
            "encompassingVolume_solid", encompassingVolume_solid1,
            encompassingVolume_solid2, 0, G4ThreeVector() );
    G4LogicalVolume *encompassingVolume_log = new G4LogicalVolume(
            encompassingVolume_solid, materials->Vacuum(),
            "encompassingVolume_log", 0, 0, 0 );
    LUXSimDetectorComponent *encompassingVolume;
    encompassingVolume = new LUXSimDetectorComponent( 0, G4ThreeVector(),
            "EncompassingVolume", encompassingVolume_log, experimentalHall,
            0, 0, false );
	
	LUXSimDetectorComponent *detectorMother = experimentalHall;	
	//	default mother volume for detector
	//	can be waterTank, rock, etc.

	//    Cavern rock                                                                                                                                           
	//    The cavern is 30x24x24m in x, y, z. The inner cavern is 20x14x12m. The top face is 7m thick, the side faces are 5m thick and the bottom is 5m thick.    
	//    The cavern extends -14m to +16m in x, -12m to +12m in y and -11m to +13m in z (based on center of world being top of water tank).              
        if(luxManager->GetCavernRockSelection() == "on"){
          G4cout << "Building cavern rock ";
          G4double outCavBox_X = 15.*m;
          G4double outCavBox_Y = 12.*m;
          G4double outCavBox_Z = 12.*m;
          G4double inCavBox_X = 10.*m;
          G4double inCavBox_Y = 7.*m;
          G4double inCavBox_Z = 6*m;

          //    Rock rotation (cavern is rotated 28 degrees relative to LUXSim axes)                                                                  
          G4RotationMatrix *rotCav = new G4RotationMatrix();
          G4double rotAngCav = 28.*deg;
          rotCav->rotateZ(rotAngCav);

	  //    Visualisation attributes                                                                                                                            
	  G4VisAttributes * visRed;
          G4VisAttributes * visGreen;
          visRed = new G4VisAttributes(G4Colour(1., 0, 0, 1.)); // red                                                                                              
	  visGreen = new G4VisAttributes(G4Colour(0, 1., 0, 1.)); // green                                                                                           
	  
          //    The outer box                                                                                                                              
	  G4Box *outCavBox = new G4Box( "outCavBox", outCavBox_X,
					outCavBox_Y, outCavBox_Z );

          //    The inner box                                                                                                                           
	  G4Box *inCavBox = new G4Box( "inCavBox", inCavBox_X,
                                       inCavBox_Y, inCavBox_Z );

          //    The inner box cut out of the outer box                                                                                                                  
	  G4SubtractionSolid *cavBox = new G4SubtractionSolid("cavBox", outCavBox, inCavBox, 0, G4ThreeVector(0,0,-1.*m));

          //    The logical volume                                                                                                                          
	  G4LogicalVolume *cavRockLog = new G4LogicalVolume( cavBox,
                                                             materials->CavRock(),"cavRockLog",
                                                             0, 0, 0 );
          cavRockLog->SetVisAttributes( visGreen );
          LUXSimDetectorComponent *cavRock = new LUXSimDetectorComponent( rotCav,                                                                                
									  G4ThreeVector(1.0*m,0,1.0*m),//4.0*m),                                                   
									  "cavRock", cavRockLog,
									  experimentalHall, 0, 0, false );
	  G4cout << "... Done!" << G4endl;
	}


	//	The muon veto system
	
	G4double tankX = 0., tankY = 0., tankZ = detZ + (42.62 * cm);
    tankZ = 0;
	
	if( luxManager->GetDetectorSelection() == "LZSimple" &&
			luxManager->GetMuonVetoSelection() == "off" ) {
		G4cout << "The muon veto system must be turned on when using LZSimple"
			   << G4endl;
		G4cout << "Turning on the muon veto..." << G4endl;
		luxManager->SetMuonVetoSelection( "on" );
	}
    
    //no muonVeto for LZDetector
    if( luxManager->GetDetectorSelection() == "LZDetector" ) luxManager->SetMuonVetoSelection("off");  
    
	if( luxManager->GetMuonVetoSelection() == "on") {
		G4cout <<"Muon Veto System is on" << G4endl;
		muonDetector = new LUXSimMuonVetoSystem();
		
		tankZ -= detZ;
      tankZ = 0;
		
		G4RotationMatrix *rotZsurface = new G4RotationMatrix();
		if( !luxManager->GetLUXSurfaceGeometry() &&
                luxManager->GetDetectorSelection() == "1_0Detector" )
			rotZsurface->rotateZ(-15.*deg);
        
        //  This next specification is to maintain the origin of the coordinate
        //  system at the center of the top surface of the bottom PMT
        if( luxManager->GetDetectorSelection() == "LZSimple" )
            tankZ += 55.996*cm;
        tankZ = 0;
		
		if( luxManager->GetDetectorSelection() == "LZDetector" ) {
		//G4cout << "...Here..." << G4endl;
		/*
			detectorType = new LUXSimLZDetector( 72.7*2.*cm + 4.5*cm,
                    luxManager->GetGridWiresSelection() );
			tankZ = 0; //((LUXSimLZDetector*)(detectorType))->GetHeightOffset();
			waterTank = new LUXSimDetectorComponent( rotZsurface, 
					G4ThreeVector(tankX,tankY,tankZ),
					"WaterTank", muonDetector->GetLogicalVolume(), 
					experimentalHall, 0, 0, false);
			
			detectorMother = waterTank; //then below make the bank sit inside this
		*/
		} else{
			waterTank = new LUXSimDetectorComponent( rotZsurface, 
					G4ThreeVector(tankX,tankY,tankZ),"WaterTank",
					muonDetector->GetLogicalVolume(),experimentalHall, 
					0, 0, false);
		   detectorMother = muonDetector->GetWaterBank();
		}
		//	detector mother is now waterBank
		//	also throw in the pyramid
		LUXSimPyramid *pyramid_obj = new LUXSimPyramid();
		pyramid = new LUXSimDetectorComponent (0,
				G4ThreeVector(tankX,tankY,
					tankZ-(muonDetector->GetHeight()+pyramid_obj->GetHeight())/2),//this is the problem 
				"Pyramid", pyramid_obj->GetLogicalVolume(), experimentalHall,
				0, 0, false);
	}

	//	  The cryostat stand
	G4double detX=0., detY=0.;	// detZ is now controlled by the user
	
	if( luxManager->GetCryoStandSelection() == "on") {
		G4cout <<"Cryostat Stand is on" << G4endl;
		if( detectorMother == experimentalHall )
			cryoStand= new LUXSimStand( detectorMother, detX, detY, tankZ );
		else
			cryoStand= new LUXSimStand( detectorMother, detX, detY, detZ );
		
	}

	//	Deciding what to do with grids

	G4bool GRIDS = false;		//build grids 
	if( luxManager->GetGridWiresSelection() == "on") {
		GRIDS = true;
		G4cout << "Grid wires are on" << G4endl;
	}
	else  {
		G4cout << "Grid wires are off" << G4endl;
	}

	//      Select which PMT numbering scheme to use
	G4bool useRealNumbering = false;
	useRealNumbering = luxManager->GetPMTNumberingScheme();
	
	//		Select whether to do gas running
	G4bool gasRun = luxManager->GetGasRun();
	
	//	  The detector
	
	detectorSpecified = true;
    LUXSimDetectorComponent *offsetComponent;
    G4double centerZ = 0, zExtent = 0, offset = 0;
	if( luxManager->GetDetectorSelection() == "simpleExample" ) {
		detectorType = new LUXSimExampleSimpleDetector();
		G4cout << "Using simpleExample" << G4endl;
		detector = new LUXSimDetectorComponent( 0,G4ThreeVector(detX,detY,detZ),
				"Cryostat",	detectorType->GetLogicalVolume(),detectorMother, 
				0, 0, false );
	} else if( luxManager->GetDetectorSelection() == "complexExample" ) {
		detectorType = new LUXSimExampleComplexDetector();
		G4cout << "Using complexExample" << G4endl;
		detector = new LUXSimDetectorComponent( 0,G4ThreeVector(detX,detY,detZ),
				"OuterCryostat", detectorType->GetLogicalVolume(),
				detectorMother, 0, 0, false );
	}	else if( luxManager->GetDetectorSelection() == "0_1Detector" ) {
		detectorType = new LUXSim0_1Detector(GRIDS);
		G4cout <<"Using LUX0.1Detector" << G4endl;
		detector = new LUXSimDetectorComponent( 0,G4ThreeVector(detX,detY,detZ),
				"VacuumVessel", detectorType->GetLogicalVolume(),detectorMother,
				0, 0, false);
	} else if( luxManager->GetDetectorSelection() == "1_0Detector" ) {
        detectorType = new LUXSim1_0Detector(GRIDS, useRealNumbering, gasRun);
		G4cout <<"Using Lux1.0Detector" << G4endl;
		if( detectorMother == experimentalHall ) {
			detector = new LUXSimDetectorComponent(0,
                    G4ThreeVector(detX,detY,tankZ), "VacuumVessel",
                    detectorType->GetLogicalVolume(), detectorMother, 0, 0,
                    false);
                    
		} else {
			detector = new LUXSimDetectorComponent(0,
                    G4ThreeVector(detX,detY,detZ), "VacuumVessel", 
			        detectorType->GetLogicalVolume(), detectorMother, 0, 0,
                    false);
        }

	//DD Generator for LUX
        G4Sphere *ddNeutronSource_solid;
        if (luxManager->GetLUXDDPencilBeam()) {
            ddNeutronSource_solid = new G4Sphere( "ddNeutronSource_solid",
                    0, .5*mm, 0*deg, 360.*deg, 0*deg, 180.*deg );
        }
        else {
            ddNeutronSource_solid = new G4Sphere( "ddNeutronSource_solid",
                    0, 35.0*mm, 0*deg, 360.*deg, 0*deg, 180.*deg );
         }
         G4LogicalVolume *ddNeutronSource_log = new G4LogicalVolume(
                ddNeutronSource_solid, materials->Air(),
                "ddNeutronSource_log" );
        LUXSimDetectorComponent *ddNeutronSource;
        G4double neutronCollimatorDiamOuter = 6.0 * cm;
        G4double neutronCollimatorHeight = luxManager->GetLUXNeutronCollimatorHeight();
        G4double zOffset = neutronCollimatorHeight + neutronCollimatorDiamOuter/2. - 42.62 * cm - 0.15 * cm;
        ddNeutronSource = new LUXSimDetectorComponent(
                0, G4ThreeVector( 48.1 *cm, -399.9399 *cm, zOffset ), "DDNeutronSource",
                ddNeutronSource_log, experimentalHall, 0, 0, false );

        //  The PMT window at the middle is "Bottom_PMT_Window_122". Get the
        //  global center and Z extent of that detector component so that the
        //  vertical offset is set correctly.
        offsetComponent =
                luxManager->GetComponentByName( "Bottom_PMT_Window_122" );
        offsetComponent->DetermineCenterAndExtent( universe );
        centerZ = offsetComponent->GetGlobalCenter().z();
        zExtent = offsetComponent->GetMaxXYZ().z();
        offset = -(centerZ + zExtent);
	} else if( luxManager->GetDetectorSelection() == "emptyCryostat" ) {
		detectorType = new LUXSimEmptyCryostat();
		G4cout <<"Using Empty Cryostat" << G4endl;
		detector = new LUXSimDetectorComponent(0, G4ThreeVector(detX,detY,detZ),
                "VacuumVessel", detectorType->GetLogicalVolume(),detectorMother,
                0, 0, false);
	} else if( luxManager->GetDetectorSelection() == "LZDetector" ) {
		G4cout <<"Using LZDetector" << G4endl;
		/*
		detector = new LUXSimDetectorComponent( 0,
				G4ThreeVector(detX,detY,-432.9*mm), "ThinWaterLayer", //was -433.5*mm, sorry this is hardcoded for now
				detectorType->GetLogicalVolume(), detectorMother, 0, 0, true);
		*/
		detectorType = new LUXSimLZDetector( 72.7*2.*cm + 4.5*cm, luxManager->GetGridWiresSelection() );
		
		detector = new LUXSimDetectorComponent( 0,
				G4ThreeVector(0,0,0), "WaterTank",
				detectorType->GetLogicalVolume(), detectorMother, 0, 0, true); //logicalVolume should be water tank
		
        //  The volume that sits just above Z = 0 is "LiquidXenonTarget".
        offsetComponent =
                luxManager->GetComponentByName( "LiquidXenonTarget" );
        offsetComponent->DetermineCenterAndExtent( universe );
        centerZ = offsetComponent->GetGlobalCenter().z();
        zExtent = offsetComponent->GetMinXYZ().z();
        offset = -(centerZ + zExtent);

	// DD Generator for LZ, copied from LUX
	// Edit the coordinates to align with the tube
        G4Sphere *ddNeutronSource_solid;
        if (luxManager->GetLUXDDPencilBeam()) {
            ddNeutronSource_solid = new G4Sphere( "ddNeutronSource_solid",
                    0, .5*mm, 0*deg, 360.*deg, 0*deg, 180.*deg );
        }
        else {
            ddNeutronSource_solid = new G4Sphere( "ddNeutronSource_solid",
                    0, 35.0*mm, 0*deg, 360.*deg, 0*deg, 180.*deg );
         }
         G4LogicalVolume *ddNeutronSource_log = new G4LogicalVolume(
                ddNeutronSource_solid, materials->Air(),
                "ddNeutronSource_log" );
        LUXSimDetectorComponent *ddNeutronSource;
	//Note: LZDetector has a Z offset in this file of 870.62 mm. Need to subtract that from z value.
	//For top end at 2964.2 and low end at 1296.8, use: G4ThreeVector( 3915 *mm, 0 *mm, -1748.44 *mm )
        ddNeutronSource = new LUXSimDetectorComponent(
                0, G4ThreeVector( 3915 *mm, 0 *mm, -1748.44 *mm ), "DDNeutronSource",
                ddNeutronSource_log, experimentalHall, 0, 0, false );
        
        
	} else if( luxManager->GetDetectorSelection() == "LZ20Detector" ) {
		detectorType = new LUXSimLZ20Detector(GRIDS);
		G4cout <<"Using LZ20 Detector" << G4endl;
		detector = new LUXSimDetectorComponent( 0,G4ThreeVector(detX,detY,detZ),
				"VacuumVessel",detectorType->GetLogicalVolume(),detectorMother, 
				0, 0, false);
	} else if( luxManager->GetDetectorSelection() == "LZ20Simple" ) {
		detectorType = new LUXSimLZ20Simple();
		G4cout << "Using simple version of LZ20 Detector" << G4endl;
		detector = new LUXSimDetectorComponent( 0,G4ThreeVector(detX,detY,detZ),
				"VacuumVessel",detectorType->GetLogicalVolume(),detectorMother,
				0, 0, false );
	} else if( luxManager->GetDetectorSelection() == "LZSimple" ) {
		detectorType = new LUXSimLZSimple();
		G4cout << "Using simple version of LZ Detector" << G4endl;
		
		if( luxManager->GetMuonVetoSelection() == "off" ) {
		
			G4cout << G4endl << G4endl;
			G4cout << "The muon veto (i.e., water tank) must be turned on to "
				   << "use the LZSimple geometry." << G4endl;
			G4cout << G4endl << G4endl;
			exit(0);
		}
		
		detector = new LUXSimDetectorComponent( 0,G4ThreeVector(detX,detY,detZ),
				"AcrylicVessel",detectorType->GetLogicalVolume(),detectorMother,
				0, 0, false );
		
		if( luxManager->GetLZSimpleNeutronCalTube() ) {
		
			G4double neutronCalibrationTubeOD =
					((LUXSimLZSimple*)detectorType)->
					GetNeutronCalibrationTubeOD();
			G4double neutronCalibrationThickness =
					((LUXSimLZSimple*)detectorType)->
					GetNeutronCalibrationTubeThickness();
			G4double neutronCalibrationTubeID = neutronCalibrationTubeOD -
					neutronCalibrationThickness;
			G4double neutronCalibrationTubeLength =
					muonDetector->GetWaterTankRadius() -
					((LUXSimLZSimple*)detectorType)->GetOuterAcrylicRadius() -
					2.5*cm + 50.*cm;
			G4double neutronCalibrationTubeInnerLength =
					neutronCalibrationTubeLength - neutronCalibrationThickness;
			
			//	Neutron calibration tube. This is not always present.
                        G4double xOffset =
                                        -( muonDetector->GetWaterTankRadius() +
                                           ((LUXSimLZSimple*)detectorType)->
                                           GetInnerAcrylicRadius() )/2 + 1.*cm;
                        G4double yOffset = luxManager->GetLZSimpleNeutronCalTubeYPosition();
                        G4double zOffset = ((LUXSimLZSimple*)detectorType)->GetNeutronCalibrationTubeZOffset();
                        G4RotationMatrix *rotY90 = new G4RotationMatrix();
                       	rotY90->rotateY(90.*deg);
                        G4Tubs *neutronCalTube_cyl = new G4Tubs( "neutronCalTube_cyl",0,
                                        neutronCalibrationTubeOD/2, neutronCalibrationTubeLength/2,
                                        0, 360.*deg );
                        G4Tubs *neutronCalTube_tank_subtr = new G4Tubs( "neutronCalTube_tank_subtr", muonDetector->GetWaterTankRadius(),
                                        muonDetector->GetWaterTankRadius() + 100.*cm, 350.*cm,
                                        0, 360.*deg );
                        G4SubtractionSolid *neutronCalTube_intermed = new G4SubtractionSolid("neutronCalTube_intermed",
                                        neutronCalTube_cyl, neutronCalTube_tank_subtr, rotY90, G4ThreeVector(0., -1 * yOffset, xOffset));
                        G4Tubs *neutronCalTube_scint_subtr = new G4Tubs( "neutronCalTube_scint_subtr", 0.,
                                        ((LUXSimLZSimple*)detectorType)->GetOuterAcrylicRadius(), 350.*cm,
                                        0, 360.*deg );
                        G4SubtractionSolid *neutronCalTube_solid = new G4SubtractionSolid("neutronCalTube_solid",
                                        neutronCalTube_intermed, neutronCalTube_scint_subtr, rotY90, G4ThreeVector(0., -1 * yOffset, xOffset));
                        G4LogicalVolume *neutronCalTube_log = new G4LogicalVolume(
                                        neutronCalTube_solid, materials->Titanium(),
                                        "neutronCalTube_log" );
                        neutronCalTube_log->SetVisAttributes( materials->TitaniumVis() );
                        LUXSimDetectorComponent *neutronCalTube=new LUXSimDetectorComponent(
                                        rotY90, G4ThreeVector(xOffset,yOffset,zOffset),
                                        "NeutronCalibrationTube", neutronCalTube_log,
                                        detectorMother, 0, 0, false );
			
			//	Air-filled space inside the neutron calibration tube
                        G4Tubs *neutronCalTubeSpace_cyl = new G4Tubs(
                                        "neutronCalTubeSpace_cyl", 0, neutronCalibrationTubeID/2,
                                        neutronCalibrationTubeInnerLength/2, 0, 360.*deg );
                        G4SubtractionSolid *neutronCalTubeSpace_intermed = new G4SubtractionSolid("neutronCalTubeSpace_intermed",
                                        neutronCalTubeSpace_cyl, neutronCalTube_tank_subtr, rotY90, G4ThreeVector(0., -1 * yOffset, xOffset));
                        G4SubtractionSolid *neutronCalTubeSpace_solid = new G4SubtractionSolid("neutronCalTubeSpace_solid",
                                        neutronCalTubeSpace_intermed, neutronCalTube_scint_subtr, rotY90, G4ThreeVector(0., -1 * yOffset, xOffset));
                        G4LogicalVolume *neutronCalTubeSpace_log = new G4LogicalVolume(
                                        neutronCalTubeSpace_solid, materials->Air(),
                                        "neutronCalTubeSpace_log" );
                        neutronCalTubeSpace_log->SetVisAttributes( materials->AirVis() );
                        LUXSimDetectorComponent *neutronCalTubeSpace;
                        neutronCalTubeSpace = new LUXSimDetectorComponent( 0,
                                        G4ThreeVector(), "NeutronCalibrationTubeSpace",
                                        neutronCalTubeSpace_log, neutronCalTube, 0, 0, false );

		} else if( luxManager->GetLZSimpleNeutronCalTopTube() ) {
		
			G4double neutronCalibrationTubeOD =
					((LUXSimLZSimple*)detectorType)->
					GetNeutronCalibrationTubeOD();
			G4double neutronCalibrationThickness =
					((LUXSimLZSimple*)detectorType)->
					GetNeutronCalibrationTubeThickness();
			G4double neutronCalibrationTubeID = neutronCalibrationTubeOD -
					neutronCalibrationThickness;
			G4double neutronCalibrationTubeLength = (3970.66 - 3270.21)*mm
                    - 2.*cm;
			G4double neutronCalibrationTubeInnerLength =
					neutronCalibrationTubeLength - neutronCalibrationThickness;
			
			//	Neutron calibration tube. This is not always present.
			G4Tubs *neutronCalTopTube_solid = new G4Tubs(
                  "neutronCalTopTube_solid", 0, neutronCalibrationTubeOD/2,
                  neutronCalibrationTubeLength/2, 0, 360.*deg );
			G4LogicalVolume *neutronCalTopTube_log = new G4LogicalVolume(
					neutronCalTopTube_solid, materials->Titanium(),
					"neutronCalTopTube_log" );
			neutronCalTopTube_log->SetVisAttributes( materials->TitaniumVis() );
			G4double xOffset = -((LUXSimLZSimple*)detectorType)->
                  GetPTFEWallInnerRadius()/2;
            //  This height offset is, alas and alack, hardcoded. My gravest
            //  apologies. --Kareem
			G4double zOffset = 262.16*cm;
			LUXSimDetectorComponent *neutronCalTopTube =
                  new LUXSimDetectorComponent( 0,
                  G4ThreeVector(xOffset,0,zOffset),
                  "NeutronCalibrationTopTube", neutronCalTopTube_log,
                  detectorMother, 0, 0, false );
			
			//	Air-filled space inside the neutron calibration tube
			G4Tubs *neutronCalTopTubeSpace_solid = new G4Tubs(
					"neutronCalTopTubeSpace_solid", 0,
                  neutronCalibrationTubeID/2,
                  neutronCalibrationTubeInnerLength/2, 0, 360.*deg );
			G4LogicalVolume *neutronCalTopTubeSpace_log = new G4LogicalVolume(
					neutronCalTopTubeSpace_solid, materials->Air(),
					"neutronCalTopTubeSpace_log" );
			neutronCalTopTubeSpace_log->SetVisAttributes( materials->AirVis() );
			LUXSimDetectorComponent *neutronCalTopTubeSpace;
			neutronCalTopTubeSpace = new LUXSimDetectorComponent( 0,
					G4ThreeVector(), "NeutronCalibrationTopTubeSpace",
					neutronCalTopTubeSpace_log, neutronCalTopTube, 0, 0, false);
		}

        //  The volume that sits just above Z = 0 is "ActiveLiquidXenon".
        offsetComponent =
                luxManager->GetComponentByName( "ActiveLiquidXenon" );
        offsetComponent->DetermineCenterAndExtent( universe );
        centerZ = offsetComponent->GetGlobalCenter().z();
        zExtent = offsetComponent->GetMinXYZ().z();
        offset = -(centerZ + zExtent);

	} else if( luxManager->GetDetectorSelection() == "LZFlex" ) {
		
		G4cout << "Using flexible version of LZ Detector" << G4endl;

		G4double mass, xeSkinThickness, teflonThickness, scintThickness;
		
		if( luxManager->GetLZFlexMass() > 0 )
			mass = luxManager->GetLZFlexMass();
		else if( luxManager->GetLZFlexMass() == 0 ) {
			G4cout << "WARNING: Cannot set the LZFlex xenon mass to 0."
				   << G4endl;
			G4cout << "         Setting it to 1 gram instead." << G4endl;
			mass = 1.*g;
		} else
			mass = 1.e4*kg;
		
		if( luxManager->GetLZFlexXeSkinThickness() > 0 )
			xeSkinThickness = luxManager->GetLZFlexXeSkinThickness();
		else if( luxManager->GetLZFlexXeSkinThickness() == 0 ) {
			G4cout << "WARNING: Cannot set the LZFlex xenon skin thickness "
				   << "to 0." << G4endl;
			G4cout << "         Setting it to 1 micron instead." << G4endl;
			xeSkinThickness = 1.*um;
		} else
			xeSkinThickness = 4.*cm;
		
		if( luxManager->GetLZFlexTeflonThickness() > 0 )
			teflonThickness = luxManager->GetLZFlexTeflonThickness();
		else if( luxManager->GetLZFlexTeflonThickness() == 0 ) {
			G4cout << "WARNING: Cannot set the LZFlex teflon thickness "
				   << "to 0." << G4endl;
			G4cout << "         Setting it to 1 micron instead." << G4endl;
			teflonThickness = 1.*um;
		} else
			teflonThickness = 3.*cm;
		
		if( luxManager->GetLZFlexScintThickness() > 0 )
			scintThickness = luxManager->GetLZFlexScintThickness();
		else if( luxManager->GetLZFlexScintThickness() == 0 ) {
			G4cout << "WARNING: Cannot set the LZFlex scintillator "
				   << "thickness to 0." << G4endl;
			G4cout << "         Setting it to 1 micron instead." << G4endl;
			scintThickness = 1.*um;
		} else
			scintThickness = 75.*cm;
		
		G4cout << "LZFlex mass = " << mass/kg << " kg" << G4endl;
		G4cout << "Xe skin thickness = " << xeSkinThickness/cm << " cm"
			   << G4endl;
		G4cout << "Teflon thickness = " << teflonThickness/cm << " cm"
			   << G4endl;
		G4cout << "Scintillator thickness = " << scintThickness/cm << " cm"
			   << G4endl;
		
		detectorType = new LUXSimLZFlex( mass, xeSkinThickness, teflonThickness,
				scintThickness );
	
		detector = new LUXSimDetectorComponent( 0,G4ThreeVector(detX,detY,detZ),
				"OuterCryostat",detectorType->GetLogicalVolume(),detectorMother,
				0, 0, false );

	} else if( luxManager->GetDetectorSelection() == "NMDetector") {
		G4cout << "Using Neutron Multiplicity Detector" << G4endl;
		//	construct the rock:
		G4double rockX = 73.*m;
		G4double rockY = 14.*m;
		G4double rockZ = 14.*m;
		G4double rockThickness = 1.*cm;
		G4VSolid * rock_solid = new G4Box( "rock_solid", rockX/2., rockY/2.,
				rockZ/2. );
		G4VSolid * rock_cutout = new G4Box("rock_cutout",rockX/2.-rockThickness,
				rockY/2.-rockThickness,rockZ/2.-rockThickness);
		rock_solid = new G4SubtractionSolid("rock_solid",rock_solid,rock_cutout,
			0,G4ThreeVector());
		G4LogicalVolume * rock_log = new G4LogicalVolume(rock_solid,
				materials->Rock(),"rock_log");
		rock_log->SetVisAttributes( materials->RockVis() );
		LUXSimDetectorComponent * rock;
		rock  = new LUXSimDetectorComponent(0, G4ThreeVector(),"Rock",
				rock_log,detectorMother,0, 0, false);
	
		//	the two detectors:
		detX = 0.*cm; detY = 63.5*cm; detZ = 1.27*cm;
		LUXSimNMDetector * neutronDetector = new LUXSimNMDetector(true);
		LUXSimDetectorComponent * northTank;
		northTank = new LUXSimDetectorComponent( 0,
				G4ThreeVector(detX,detY,detZ), "NorthTank",
				neutronDetector->GetLogicalVolume(),detectorMother, 0, 0,
				false);
		neutronDetector->PlaceTopLead(detY, detZ, detectorMother);
		neutronDetector->PlaceSideLead(detectorMother,3.81*cm);

		neutronDetector = new LUXSimNMDetector(false);
		LUXSimDetectorComponent * southTank;
		detY = -63.5*cm; detZ = 6.35*cm;
		southTank = new LUXSimDetectorComponent(0,G4ThreeVector(detX,detY,detZ),
				"SouthTank",neutronDetector->GetLogicalVolume(),detectorMother,
				0, 0, false);
		neutronDetector->PlaceTopLead(detY, detZ, detectorMother);
		neutronDetector->PlaceBottomLead(detectorMother);
		G4double xPos = 66.04*cm;
		G4double yPos = 101.6*cm;
		neutronDetector->PlaceSource(xPos, yPos);

	} else if( luxManager->GetDetectorSelection() == "LLNLSinglePhase" ) {
		detectorType = new LUXSimLLNL_SinglePhase();
		G4cout << "Using the LLNL single-phase detector" << G4endl;
		detector = new LUXSimDetectorComponent( 0,
				G4ThreeVector(detX,detY,detZ), "VacuumVessel",
				detectorType->GetLogicalVolume(), detectorMother, 0, 0, false );

	} else if( luxManager->GetDetectorSelection() == "GNARRLIDetector" ) {
		detectorType = new LUXSimGNARRLIDetector();
		G4cout << "Using the GNARRLI Detector" << G4endl;
		G4cout << "detectorType->GetLogicalVolume() = "
			   << detectorType->GetLogicalVolume() << G4endl;
		detector = new LUXSimDetectorComponent( 0,
			G4ThreeVector(detX,detY,detZ), "GNARRLIChamber",
			detectorType->GetLogicalVolume(), detectorMother, 0, 0, false );

	} else {
		G4cout << "Geometry not set -- please specify geometry to use"
			   << G4endl;
		detectorType = NULL;
		detector = NULL;
		detectorSpecified = false;
	}
    
    //  Now set the vertical offset in the case of the 1_0Detector, LZDetector,
    //  or LZSimple
    if( luxManager->GetDetectorSelection() == "1_0Detector" ||
        luxManager->GetDetectorSelection() == "LZDetector" ||
        luxManager->GetDetectorSelection() == "LZSimple" ) {
        
        //  Be sure to incorporate the Z offset as specified by the macro
        //  command
        offset += luxManager->GetDetectorZ();
        
        //  Now that we have the required offset, we have to apply that offset
        //  to the experimentalHall.
        G4ThreeVector currentTranslation = experimentalHall->GetTranslation();
        currentTranslation.setZ( currentTranslation.z() + offset );
        experimentalHall->SetTranslation( currentTranslation );
    }

	//G4cout << offset << G4endl;

	//
	//	All done! Return the detector geometry
	//
	return universe;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					UpdateGeometry()
//------++++++------++++++------++++++------++++++------++++++------++++++------
//	This next method will be most useful for future use. If the geometry is
//	altered via a macro command, UpdateGeometry will have to be called to
//	instantiate the change.
void LUXSimDetectorConstruction::UpdateGeometry()
{
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();
	G4LogicalSkinSurface::CleanSurfaceTable();
	G4LogicalBorderSurface::CleanSurfaceTable();
	G4RunManager::GetRunManager()->DefineWorldVolume( Construct() );
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	
	//	I have absolutely no fraking idea why this next line helps, but
	//	it seems to keep the visualization stack from spazzing out and 
	//	core dumping. Remove it at your own peril.
	G4cout << " " << G4endl;
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLXeTeflonRefl()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorConstruction::SetLXeTeflonRefl( G4double r )
{
	materials->SetLXeTeflonRefl(r);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLXeSteelRefl()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorConstruction::SetLXeSteelRefl( G4double r )
{
	materials->SetLXeSteelRefl(r);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLXeAbs()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorConstruction::SetLXeAbs( G4double l )
{
	materials->SetLXeAbs(l);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLXeRayleigh()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorConstruction::SetLXeRayleigh( G4double l )
{
	materials->SetLXeRayleigh(l);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetGXeTeflonRefl()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorConstruction::SetGXeTeflonRefl( G4double r )
{
	materials->SetGXeTeflonRefl(r);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetGXeSteelRefl()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorConstruction::SetGXeSteelRefl( G4double r )
{
	materials->SetGXeSteelRefl(r);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetGXeAbs()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorConstruction::SetGXeAbs( G4double l )
{
	materials->SetGXeAbs(l);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetGXeRayleigh()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorConstruction::SetGXeRayleigh( G4double l )
{
	materials->SetGXeRayleigh(l);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetAlUnoxQuartzRefl()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorConstruction::SetAlUnoxQuartzRefl( G4double l )
{
	materials->SetAlUnoxQuartzRefl(l);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetDetectorZ()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimDetectorConstruction::SetDetectorZ( G4double z )
{
	detZ = z;
}


