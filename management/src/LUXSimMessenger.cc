////////////////////////////////////////////////////////////////////////////////
/*	LUXSimMessenger.cc
*
* This is the code file to define the LUXSim messenger commands. The idea
* behind this class is that all commands get routed and stored in this
* management class, whether those commands are related to the geometry, physics
* list, output specification, or whatever. In this way, all parts of the
* simulation have access to all simulation settings without having to re-write
* any code.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	7 April 2009 - Added the command for the LUX1.0Detector as well as the muon
*				   veto system. Currently the muon veto command can only be used
*				   with the LUX1.0Detector; this will be remedied shortly.
*				   (Melinda)
*	20 April 2009 - Added the LUXSimSourceSet command (Kareem)
*	27 July 2009 - Added the physicsList directory and the command to turn
*				   optical photons on and off (Kareem)
*	28 July 2009 - Added messenger commands to set the output directory (Kareem)
*	14-Sep-09 - Added commands to control the optical photon record level
*				(Kareem)
*	28-Sept-09 - Added cryoStand command to turn on/off the cryostat stand
*                (Melinda)
*	28-Sept-09 - Added 0_1Detector option to detector/select command
*	29-Sept-09 - Added gridWires command: wires makes the individual wires,
*               plane puts a thin sheet in place of the wires, off doesn't place
*               anything (Melinda)
*	19-Jan-10 - Added LZ20Detector option to detector/select command (Melinda)
*   2 -Apr-10 - Added a source/reset method (Nick)
*	19-May-10 - Added NMDetector (Melinda)
*	26-May-10 - Added LZ20Simple detector (Monica)
*	26-Jul-10 - Added the output name command (Chao)
*	20-Sep-10 - Added an optical debug command (Kareem)
*	27-Nov-10 - Added always record primary command (Kareem)
*   21-Jul-11 - Added Decay Chain Print option (Nick)
*	24-Aug-11 - Added reference to GNARRLIDetector and added command for
*               controlling 1 or 4 PMT geometry for GNARRLI detector (Tenzing)
*   21-Sep-11 - ElectricField options. And added delete call to geometry
*               optns(Nick)
*	18-Jan-12 - Added commands for altering materials properties (Dave)
*	24-Feb-12 - Added LZFlex to the list of detectors (Kareem)
*	24-Feb-12 - Added LZFlex controls: total mass, Xe skin thickness, and
*				scintillator thickness (Kareem)
*	26-Feb-12 - Changed the documentation of the LZFlex scintillator thickness
*               command because 75 cm is now default (Kareem)
*	06-Mar-12 - Added LZFlex control for detector Z position (Dave)
*	09-Mar-12 - Renamed Z position controls and variables, to indicate
*				that they work with all detectors (Dave)
*	09-Mar-12 - Added Rayleigh scattering UI hooks (Dave)
*   24-Mar-12 - Added support for the event progress report UI hooks (Mike)
*	01-May-12 - Added a command to set the thermal electron record level
*				(Kareem)
*	26-May-12 - Added the LZDetector to the list of available detectors (Kareem)
*   12-Jul-12 - New field commands (Nick and Matthew)
*   15-Jul-12 - Modification to put all generators into a binary search tree 
*               using methods GenerateEventList and GenerateFromEventList(Nick)
*   16-Aug-12 - Added command to switch PMT numbering scheme (Rich)
*	25-Sep-12 - Added switch to toggle between the surface and underground LUX
*				geometries (Kareem)
*	25-Sep-12 - Added switch to toggle gas-only mode (Dave)
*	12-Dec-06 - Replaced E-field commands with commands to set grid voltages
*				and added electric field printing command (Dave)
*	12-Dec-07 - Gas running command disappeared! Put it back. (Dave)
*   04-Feb-13 - Added command for setting the S1 and S2 gains (Kareem)
*	22-Feb-13 - Added command for setting the drifting electron attenuation
*				distance (Kareem)
*	18-Mar-13 - Added the command for controlling the active diameter of the
*				collimated sources (Kareem)
*	14-Apr-13 - Added LZSimple as a detector selection choice (Kareem)
*	23-Apr-13 - Added the LUXSimLZNeutronCalibrationTubeCommand support (Kareem)
*	26-Apr-13 - Added the LUXSimLZCalibrationSourceHeightCommand support
*				(Kareem)
*   23-Jun-13 - Added the LUXSimLZNeutronCalibrationTubeTopCommand support
*               (Kareem)
*   18-Jul-13 - Added the LUXSimLUXNeutronCollimatorCommand and the
*               LUXSimLUXNeutronCollimatorHeightCommand (Kevin)
*   03-Dec-13 - Added the commands for setting the top and bottom PMT bank
*               voltages (Kareem)
*   07-Apr-14 - Added 100-keV hack switch (Kareem)
*   14-Apr-14 - Added commands to use an XYZ-dependent drift time and radial drift
*               from a file. (Kevin)
*   14-May-14 - Added option in LZSimple geometry for the tungsten pig for a YBe
*	        source. (Kevin)
*   16-May-14 - Added commands to change location of LZ DD neutron calibration
*               tube (Kevin)
*   21-May-14 - Changed the 100keVHack command to take a variable upper energy
*               limit, and updated the documentation (Kareem)
*   21-May-14 - Changed the 100keVHack command to "upperEnergyHack" to make it
*               more intuitive (Kareem)
*   28-May-14 - Added flag to turn on/off a new LZVeto system (Scott Ha.)
*   03-Sep-14 - Added flag for checking overlaps for all volumes (automatic
*               checking off by default) (Kareem)
*   26-Sep-14 - Added option to change YBe pig height and diameter (Kevin)
*   14-Oct-14 - Added component-wise SetMass and SetVolume commands (Kareem)
*   02-Feb-15 - Added pencil beam command for the LUX DD generator (Kevin)
*   15-Jul-15 - Added option to turn on/off the cavern rock geometry (David W)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
//#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
//#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithADouble.hh"
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimMessenger.hh"
#include "LUXSimManager.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimMessenger()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimMessenger::LUXSimMessenger( LUXSimManager *man )
: luxManager( man )
{
	//	General commands
	LUXSimDir = new G4UIdirectory( "/LUXSim/" );
	LUXSimDir->SetGuidance( "Commands specific to LUXSim" );
	
	LUXSimBeamOnCommand = new G4UIcmdWithAnInteger( "/LUXSim/beamOn", this );
	LUXSimBeamOnCommand->SetGuidance( "This command should be used rather than the stanard \"/run/beamOn\" command." );
	LUXSimBeamOnCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimRandomSeedCommand = new G4UIcmdWithAnInteger( "/LUXSim/randomSeed", this );
	LUXSimRandomSeedCommand->SetGuidance( "Use this command to explicitly set the randomization seed used in the" );
	LUXSimRandomSeedCommand->SetGuidance( "simulation. By default, the randomization seed is itself random, so this" );
	LUXSimRandomSeedCommand->SetGuidance( "command is used for reproducing earlier data (e.g., for debugging)." );
	LUXSimRandomSeedCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	//	Input/output commands
	LUXSimFileDir = new G4UIdirectory( "/LUXSim/io/" );
	LUXSimFileDir->SetGuidance( "Controls the LUXSim input and output files" );
	
	LUXSimOutputDirCommand = new G4UIcmdWithAString( "/LUXSim/io/outputDir", this );
	LUXSimOutputDirCommand->SetGuidance( "Use this to set the name of the output directory. By default, this is set to the" );
	LUXSimOutputDirCommand->SetGuidance( "directory in which the executable is launched. If the directory does not end" );
	LUXSimOutputDirCommand->SetGuidance( "with a \"/\", one will be added." );

	LUXSimOutputNameCommand = new G4UIcmdWithAString( "/LUXSim/io/outputName", this );
	LUXSimOutputNameCommand->SetGuidance( "This command sets the base name of the output file. The randomization seed is" );
	LUXSimOutputNameCommand->SetGuidance( "still incorporated into the file name so that files don't get overwritten." );
	
	LUXSimAlwaysRecordPrimaryCommand = new G4UIcmdWithABool( "/LUXSim/io/alwaysRecordPrimary", this );
	LUXSimAlwaysRecordPrimaryCommand->SetGuidance( "Setting this command to true will record the primary particle information, even" );
	LUXSimAlwaysRecordPrimaryCommand->SetGuidance( "if no other hit or energy information is recorded. The default value is true." );

    LUXSimEventProgressCommand = new G4UIcmdWithAnInteger( "/LUXSim/io/updateFrequency", this );
    LUXSimEventProgressCommand->SetGuidance( "This controls after how many events a progress report is printed to screen that" );
    LUXSimEventProgressCommand->SetGuidance( "includes the current event number and the total number of seconds that have" );
    LUXSimEventProgressCommand->SetGuidance( "elapsed since the begininning of Event 1." );
    
    LUXSim100keVHackCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/io/upperEnergyHack", this );
    LUXSim100keVHackCommand->SetGuidance( "Sets the upper energy cut for the active liquid xenon. If the total energy" );
    LUXSim100keVHackCommand->SetGuidance( "deposition exceeds the given value, the entire event will not be recorded to" );
    LUXSim100keVHackCommand->SetGuidance( "disk. Set to 0 to turn it off (which is the default)." );
    LUXSim100keVHackCommand->SetGuidance( "active liquid xenon will be recorded. Default is off.");
    LUXSim100keVHackCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

    // User defined variables commands.
	LUXSimUserVar1Command = new G4UIcmdWithADouble( "/LUXSim/io/userVar1", this);
	LUXSimUserVar1Command->SetGuidance("Set userVar1 in the manager class. Useful for passing arbitrary parameters from macros." );
	LUXSimUserVar2Command = new G4UIcmdWithADouble( "/LUXSim/io/userVar2", this);
	LUXSimUserVar2Command->SetGuidance("Set userVar2 in the manager class. Useful for passing arbitrary parameters from macros." );
	LUXSimUserVar3Command = new G4UIcmdWithADouble( "/LUXSim/io/userVar3", this);
	LUXSimUserVar3Command->SetGuidance("Set userVar3 in the manager class. Useful for passing arbitrary parameters from macros." );
	LUXSimUserVar4Command = new G4UIcmdWithADouble( "/LUXSim/io/userVar4", this);
	LUXSimUserVar4Command->SetGuidance("Set userVar4 in the manager class. Useful for passing arbitrary parameters from macros." );
	LUXSimUserVar5Command = new G4UIcmdWithADouble( "/LUXSim/io/userVar5", this);
	LUXSimUserVar5Command->SetGuidance("Set userVar5 in the manager class. Useful for passing arbitrary parameters from macros." );

	//	Geometry commands
	LUXSimDetectorDir = new G4UIdirectory( "/LUXSim/detector/" );
	LUXSimDetectorDir->SetGuidance( "Commands to control the LUXSim geometry" );
	
	LUXSimUpdateCommand = new G4UIcmdWithoutParameter( "/LUXSim/detector/update", this );
	LUXSimUpdateCommand->SetGuidance( "Update LUXSim detector - used if the detector has been altered on-the-fly (i.e.," );
	LUXSimUpdateCommand->SetGuidance( "between runs, but without re-executing the simulation" );
	LUXSimUpdateCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimDetectorSelectCommand = new G4UIcmdWithAString( "/LUXSim/detector/select", this );
	LUXSimDetectorSelectCommand->SetGuidance( "Selects between using the simple and complex example cryostats. The default" );
	LUXSimDetectorSelectCommand->SetGuidance( "choice is \"simple\"." );
	LUXSimDetectorSelectCommand->SetCandidates( "simpleExample complexExample 1_0Detector 0_1Detector emptyCryostat LZDetector LZSimple LZ20Detector NMDetector LZ20Simple LZFlex LLNLSinglePhase GNARRLIDetector" );
	LUXSimDetectorSelectCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
    
    LUXSimVolumeOverlapCheckCommand = new G4UIcmdWithABool( "/LUXSim/detector/checkForOverlaps", this );
    LUXSimVolumeOverlapCheckCommand->SetGuidance( "Forces a volume overlap check for all components" );
    LUXSimVolumeOverlapCheckCommand->SetGuidance( "Default value = false" );
    LUXSimVolumeOverlapCheckCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimDetectorZCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/detectorZ", this );
	LUXSimDetectorZCommand->SetGuidance( "Sets the absolute Z position of any detector" );
	LUXSimDetectorZCommand->SetGuidance( "relative to water shield or universe center" );
	LUXSimDetectorZCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
    LUXSimComponentSetMassCommand = new G4UIcmdWithAString( "/LUXSim/detector/setComponentMass", this );
	LUXSimComponentSetMassCommand->SetGuidance( "Sets the mass of a detector component." );
	LUXSimComponentSetMassCommand->SetGuidance( "Usage: /LUXSim/detector/setComponentMass <component name> <mass> <units>" );
	LUXSimComponentSetMassCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

    LUXSimComponentSetVolumeCommand = new G4UIcmdWithAString( "/LUXSim/detector/setComponentVolume", this );
	LUXSimComponentSetVolumeCommand->SetGuidance( "Sets the mass of a detector component." );
	LUXSimComponentSetVolumeCommand->SetGuidance( "Usage: /LUXSim/detector/setComponentVolume <component name> <volume> <units>" );
	LUXSimComponentSetVolumeCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimMuonVetoCommand = new G4UIcmdWithAString( "/LUXSim/detector/muonVeto", this );
	LUXSimMuonVetoCommand->SetGuidance( "Turns the muon veto system on or off. The default" );
	LUXSimMuonVetoCommand->SetGuidance( "choice is \"off\"." );
	LUXSimMuonVetoCommand->SetCandidates( "on off" );
	LUXSimMuonVetoCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimLZVetoCommand = new G4UIcmdWithAString( "/LUXSim/detector/LZVeto", this );
	LUXSimLZVetoCommand->SetGuidance( "Turns the LZ water veto system on or off. The default" );
	LUXSimLZVetoCommand->SetGuidance( "choice is \"off\"." );
	LUXSimLZVetoCommand->SetCandidates( "on off" );
	LUXSimLZVetoCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimPMTNumberingCommand = new G4UIcmdWithAString( "/LUXSim/detector/usePMTNumbering", this);
	LUXSimPMTNumberingCommand->SetGuidance( "Selects the PMT numbering scheme between the \"old\", linear numbering");
	LUXSimPMTNumberingCommand->SetGuidance( "and the real numbering.  Default is linear");
	LUXSimPMTNumberingCommand->SetCandidates( "linear real" );
	LUXSimPMTNumberingCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimCryoStandCommand = new G4UIcmdWithAString( "/LUXSim/detector/cryoStand", this );
	LUXSimCryoStandCommand->SetGuidance( "Turns the cryostat stand on or off. The default" );
	LUXSimCryoStandCommand->SetGuidance( "choice is \"off\"." );
	LUXSimCryoStandCommand->SetCandidates( "on off" );
	LUXSimCryoStandCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimGridWiresCommand = new G4UIcmdWithAString( "/LUXSim/detector/gridWires", this );
	LUXSimGridWiresCommand->SetGuidance( "Selects grid wire option. The default" );
	LUXSimGridWiresCommand->SetGuidance( "choice is \"off\"." );
	LUXSimGridWiresCommand->SetCandidates( "on off" );
	LUXSimGridWiresCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimRecordLevelCommand = new G4UIcmdWithAString( "/LUXSim/detector/recordLevel", this );
	LUXSimRecordLevelCommand->SetGuidance( "Sets the record level of a volume according to the volume name." );
	LUXSimRecordLevelCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimRecordLevelOptPhotCommand = new G4UIcmdWithAString( "/LUXSim/detector/recordLevelOptPhot", this );
	LUXSimRecordLevelOptPhotCommand->SetGuidance( "Sets the optical photon record level of a volume according to the volume name." );
	LUXSimRecordLevelOptPhotCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimRecordLevelThermElecCommand = new G4UIcmdWithAString( "/LUXSim/detector/recordLevelThermElec", this );
	LUXSimRecordLevelThermElecCommand->SetGuidance( "Sets the thermal electron record level of a volume according to the volume name." );
	LUXSimRecordLevelThermElecCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimCollimatorHeightCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/collimatorHeight", this );
	LUXSimCollimatorHeightCommand->SetGuidance( "Sets the height of the collimator relative to detector center." );
	LUXSimCollimatorHeightCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimCollimatorHoleCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/collimatorHole", this );
	LUXSimCollimatorHoleCommand->SetGuidance( "Sets the collimator hole diameter." );
	LUXSimCollimatorHoleCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimCollimatorSourceDiameterCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/collimatorSourceDiameter", this );
	LUXSimCollimatorSourceDiameterCommand->SetGuidance( "Sets the diameter of the active source in the collimators" );
	LUXSimCollimatorSourceDiameterCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimLUXSurfaceGeometryCommand = new G4UIcmdWithABool( "/LUXSim/detector/LUXSurfaceGeometry", this );
	LUXSimLUXSurfaceGeometryCommand->SetGuidance( "Sets the LUX detector to the surface (vs. underground) geometry." );
	LUXSimLUXSurfaceGeometryCommand->SetGuidance( "In the underground geometry, the detector has a different orientation" );
	LUXSimLUXSurfaceGeometryCommand->SetGuidance( "with respect to the calibration source tubes, and the two lowest grids" );
	LUXSimLUXSurfaceGeometryCommand->SetGuidance( "are slightly changed." );
	LUXSimLUXSurfaceGeometryCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLUXNeutronCollimatorCommand = new G4UIcmdWithABool( "/LUXSim/detector/LUXNeutronCollimator", this );
	LUXSimLUXNeutronCollimatorCommand->SetGuidance( "When true, as the neutron collimator tube to the geometry." );
	LUXSimLUXNeutronCollimatorCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLUXNeutronCollimatorHeightCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LUXNeutronCollimatorHeight", this );
	LUXSimLUXNeutronCollimatorHeightCommand->SetGuidance( "Sets the height of the collimator relative to detector center." );
	LUXSimLUXNeutronCollimatorHeightCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLUXDDPencilBeamCommand = new G4UIcmdWithABool( "/LUXSim/detector/LUXDDPencilBeam", this );
	LUXSimLUXDDPencilBeamCommand->SetGuidance( "When true, the DD neutron source is treated as a pencil beam." );
	LUXSimLUXDDPencilBeamCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLUXSourceTubeGapCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LUXSourceTubeGap", this );
	LUXSimLUXSourceTubeGapCommand->SetGuidance( "Sets the water gap between the outer titanium and the LUX acrylicc source tubes." );
	LUXSimLUXSourceTubeGapCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimGNARRLIPMTCommand = new G4UIcmdWithABool( "/LUXSim/detector/fourPMTs", this );
	LUXSimGNARRLIPMTCommand->SetGuidance( "Selects 1 or 4 PMT geometry of GNARRLI detector." );
	LUXSimGNARRLIPMTCommand->SetGuidance( "true for 4 PMT geometry, false for 1 PMT geometry (default)" );
	LUXSimGNARRLIPMTCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimLZFlexMassCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LZFlexMass", this );
	LUXSimLZFlexMassCommand->SetGuidance( "Sets the total mass of the active xenon volume for a simple LZ detector" );
	LUXSimLZFlexMassCommand->SetGuidance( "Using a negative value sets the default value of 10 tonnes" );
	LUXSimLZFlexMassCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLZFlexXeSkinCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LZFlexXeSkin", this );
	LUXSimLZFlexXeSkinCommand->SetGuidance( "Sets the thickness of the xenon skin for a simple LZ detector" );
	LUXSimLZFlexXeSkinCommand->SetGuidance( "Using a negative value sets the default value of 4 cm" );
	LUXSimLZFlexXeSkinCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLZFlexTeflonThicknessCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LZFlexTeflonThickness", this );
	LUXSimLZFlexTeflonThicknessCommand->SetGuidance( "Sets the thickness of the Teflon wall for a simple LZ detector" );
	LUXSimLZFlexTeflonThicknessCommand->SetGuidance( "Using a negative value sets the default value of 3 cm" );
	LUXSimLZFlexTeflonThicknessCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLZFlexScintThicknessCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LZFlexScintThickness", this );
	LUXSimLZFlexScintThicknessCommand->SetGuidance( "Sets the thickness of the scintillator for a simple LZ detector" );
	LUXSimLZFlexScintThicknessCommand->SetGuidance( "Using a negative value sets the default value of 75 cm" );
	LUXSimLZFlexScintThicknessCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimLZNeutronCalibrationTubeCommand = new G4UIcmdWithABool( "/LUXSim/detector/LZNeutronCalTube", this );
	LUXSimLZNeutronCalibrationTubeCommand->SetGuidance( "Places or removes a neutron calibration tube to the LZSimple geometry" );
	LUXSimLZNeutronCalibrationTubeCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimLZNeutronCalibrationTubeTopCommand = new G4UIcmdWithABool( "/LUXSim/detector/LZNeutronCalTopTube", this );
	LUXSimLZNeutronCalibrationTubeTopCommand->SetGuidance( "Places or removes a top, vertical neutron calibration tube to the LZSimple");
    LUXSimLZNeutronCalibrationTubeTopCommand->SetGuidance( "geometry" );
	LUXSimLZNeutronCalibrationTubeTopCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLZNeutronCalibrationTubeHeightCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LZNeutronCalTubeHeight", this );
	LUXSimLZNeutronCalibrationTubeHeightCommand->SetGuidance( "Changes height of LZ neutron calibration tube in LZSimple" );
	LUXSimLZNeutronCalibrationTubeHeightCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLZNeutronCalibrationTubeYPositionCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LZNeutronCalTubeYPosition", this );
	LUXSimLZNeutronCalibrationTubeYPositionCommand->SetGuidance( "Changes Y position of LZ neutron calibration tube in LZSimple" );
	LUXSimLZNeutronCalibrationTubeYPositionCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimLZCalibrationSourceHeightCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LZSimpleCalSourceHeight", this );
	LUXSimLZCalibrationSourceHeightCommand->SetGuidance( "Sets the height of the calibration source above the height of the bottom PMTs" );
	LUXSimLZCalibrationSourceHeightCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimCavernRockCommand = new G4UIcmdWithAString( "/LUXSim/detector/cavRock", this );
        LUXSimCavernRockCommand->SetGuidance( "Turns the cavern rock on or off. The default" );
        LUXSimCavernRockCommand->SetGuidance( "choice is \"off\"." );
        LUXSimCavernRockCommand->SetCandidates( "on off" );
	LUXSimCavernRockCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

        // YBe Pig

	LUXSimLZYBePigCommand = new G4UIcmdWithABool( "/LUXSim/detector/LZYBePig", this );
	LUXSimLZYBePigCommand->SetGuidance( "Places a tungsten pig on top of the cryostat for YBe calibration simulations." );
	LUXSimLZYBePigCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLZYBePigDiameterCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LZYBePigDiameter", this );
	LUXSimLZYBePigDiameterCommand->SetGuidance( "Set diameter of YBe tungsten shield." );
	LUXSimLZYBePigDiameterCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLZYBePigHeightCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/LZYBePigHeight", this );
	LUXSimLZYBePigHeightCommand->SetGuidance( "Set height of YBe tungsten shield." );
	LUXSimLZYBePigHeightCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	//Electric Field
	LUXSimTopPMTBankVoltageCommand			= new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/topPMTBankVoltage", this );
	LUXSimTopPMTBankVoltageCommand->SetGuidance( "Sets the voltage on the top PMT bank for the purpose of calculating the electric fields." );
	LUXSimTopPMTBankVoltageCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimTopGridVoltageCommand			= new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/topGridVoltage", this );
	LUXSimTopGridVoltageCommand->SetGuidance( "Sets the voltage on the top grid." );
	LUXSimTopGridVoltageCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimAnodeGridVoltageCommand		= new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/anodeGridVoltage", this );
	LUXSimAnodeGridVoltageCommand->SetGuidance( "Sets the voltage on the anode grid." );
	LUXSimAnodeGridVoltageCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimGateGridVoltageCommand		= new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/gateGridVoltage", this );
	LUXSimGateGridVoltageCommand->SetGuidance( "Sets the voltage on the gate grid." );
	LUXSimGateGridVoltageCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimCathodeGridVoltageCommand		= new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/cathodeGridVoltage", this );
	LUXSimCathodeGridVoltageCommand->SetGuidance( "Sets the voltage on the cathode grid." );
	LUXSimCathodeGridVoltageCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimBottomGridVoltageCommand		= new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/bottomGridVoltage", this );
	LUXSimBottomGridVoltageCommand->SetGuidance( "Sets the voltage on the bottom grid." );
	LUXSimBottomGridVoltageCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimBottomPMTBankVoltageCommand			= new G4UIcmdWithADoubleAndUnit( "/LUXSim/detector/bottomPMTBankVoltage", this );
	LUXSimBottomPMTBankVoltageCommand->SetGuidance( "Sets the voltage on the bottom PMT bank for the purpose of calculating the electric fields." );
	LUXSimBottomPMTBankVoltageCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimPrintElectricFieldsCommand	= new G4UIcmdWithoutParameter( "/LUXSim/detector/printEFields", this );
	LUXSimPrintElectricFieldsCommand->SetGuidance( "Print electric fields calculated from grid voltages." );
	LUXSimPrintElectricFieldsCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimGasRunCommand = new G4UIcmdWithABool( "/LUXSim/detector/gasRun", this );
	LUXSimGasRunCommand->SetGuidance( "Sets LUX1_0Detector to gas-only mode" );
	LUXSimGasRunCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimEFieldFromFileCommand = new G4UIcmdWithABool( "/LUXSim/detector/EFieldFromFile", this );
	LUXSimEFieldFromFileCommand->SetGuidance( "When true, use a an electric field map from a file instead of a constant drift field." );
	LUXSimEFieldFromFileCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimEFieldFileCommand = new G4UIcmdWithAString( "/LUXSim/detector/EFieldFile", this );
	LUXSimEFieldFileCommand->SetGuidance( "Sets path to electric field map." );
	LUXSimEFieldFileCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimDriftTimeFromFileCommand = new G4UIcmdWithABool( "/LUXSim/detector/DriftTimeFromFile", this );
	LUXSimDriftTimeFromFileCommand->SetGuidance( "When true, use a an electric field map from a file instead of a constant drift field." );
	LUXSimDriftTimeFromFileCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimDriftTimeFileCommand = new G4UIcmdWithAString( "/LUXSim/detector/DriftTimeFile", this );
	LUXSimDriftTimeFileCommand->SetGuidance( "Sets path to electric field map." );
	LUXSimDriftTimeFileCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimRadialDriftFromFileCommand = new G4UIcmdWithABool( "/LUXSim/detector/RadialDriftFromFile", this );
	LUXSimRadialDriftFromFileCommand->SetGuidance( "When true, use a an electric field map from a file instead of a constant drift field." );
	LUXSimRadialDriftFromFileCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimRadialDriftFileCommand = new G4UIcmdWithAString( "/LUXSim/detector/RadialDriftFile", this );
	LUXSimRadialDriftFileCommand->SetGuidance( "Sets path to electric field map." );
	LUXSimRadialDriftFileCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimLUXDoublePheRateFromFileCommand = new G4UIcmdWithABool( "/LUXSim/detector/LUXDoublePheRateFromFile", this );
	LUXSimLUXDoublePheRateFromFileCommand->SetGuidance( "When true, uses a text file to determine double phe rate for each PMT." );
	LUXSimLUXDoublePheRateFromFileCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXFastSimSkewGaussianS2Command = new G4UIcmdWithABool( "/LUXSim/detector/LUXFastSimSkewGaussianS2", this );
	LUXFastSimSkewGaussianS2Command->SetGuidance( "Sets whether or not to use a skew gaussian model for the single electrons size in fast sims." );
	LUXFastSimSkewGaussianS2Command->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	//	Source commands
	LUXSimSourceDir = new G4UIdirectory( "/LUXSim/source/" );
	LUXSimSourceDir->SetGuidance( "Commands to control the LUXSim sources" );
	
	LUXSimSourceSetCommand = new G4UIcmdWithAString( "/LUXSim/source/set", this );
	LUXSimSourceSetCommand->SetGuidance( "(with String) Adds a radioactive load to a LUXSimDetectorComponent. This command works only" );
	LUXSimSourceSetCommand->SetGuidance( "with geometry volumes that have been defined at LUXSimDetectorComponents." );
	LUXSimSourceSetCommand->SetGuidance( "The format of the string is <volume name> <source type> <activity>. The source" );
	LUXSimSourceSetCommand->SetGuidance( "type can only be one of Th, U, K, AmBe, Cf, or spallation. The activity is given" );
	LUXSimSourceSetCommand->SetGuidance( "in units of nCi, uCi, mCi, Bq, and kBq. If no unit is specified, Bq is assumed." );
	LUXSimSourceSetCommand->SetGuidance( "Example: /LUXSim/source/set Top_PMT_Window_2 AmBe 0.5 kBq" );
	LUXSimSourceSetCommand->SetGuidance( "If using the GammaX generator, the fiducial volume radial and Z bounds, the reverse field" );
	LUXSimSourceSetCommand->SetGuidance( "radial and Z bounds, and the maximum energy of the scatters must be specified." );
	LUXSimSourceSetCommand->SetGuidance( "In that case the string format would become");
	LUXSimSourceSetCommand->SetGuidance( "'LiquidXenon gamma-X <activity> <fiducial radius> <lower fiducial Z> <upper fiducial Z>");
	LUXSimSourceSetCommand->SetGuidance( "<reverse field region (rfr) radius> <rfr lower Z> <rfr upper Z> <units> <max Energy> <units>");
	LUXSimSourceSetCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

    //print decay Chain
	LUXSimSourcePrintCommand = new G4UIcmdWithABool( "/LUXSim/source/print", this );
    LUXSimSourcePrintCommand->SetGuidance("(Boolean) Prints Decay Chain Binary Tree to standard output; lines starts with NodeDecayChain");
    LUXSimSourcePrintCommand->AvailableForStates( G4State_PreInit, G4State_Idle);
    //reset
    LUXSimSourceResetCommand = new G4UIcmdWithoutParameter( "/LUXSim/source/reset", this );
	LUXSimSourceResetCommand->SetGuidance( "Clears all previously set sources" );
	LUXSimSourceResetCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	//	Physics list commands
	LUXSimPhysicsListDir = new G4UIdirectory( "/LUXSim/physicsList/" );
	LUXSimPhysicsListDir->SetGuidance( "Commands to control the physics list" );
	
	LUXSimOpticalPhotonsCommand = new G4UIcmdWithABool( "/LUXSim/physicsList/useOpticalProcesses", this );
	LUXSimOpticalPhotonsCommand->SetGuidance( "Turns optical processes (Scintillation and Cerenkov) on and off" );
	LUXSimOpticalPhotonsCommand->AvailableForStates( G4State_PreInit, G4State_Idle );

	LUXSimOpticalDebugCommand = new G4UIcmdWithABool( "/LUXSim/physicsList/opticalDebug", this );
	LUXSimOpticalDebugCommand->SetGuidance( "Turns on simplified optical parameters for debugging purposes" );
	LUXSimOpticalDebugCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
    
    LUXSimS1GainCommand = new G4UIcmdWithADouble( "/LUXSim/physicsList/s1gain", this );
 	LUXSimS1GainCommand->SetGuidance( "Sets the gain for S1 light generation" );
	LUXSimS1GainCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
    LUXSimS2GainCommand = new G4UIcmdWithADouble( "/LUXSim/physicsList/s2gain", this );
 	LUXSimS2GainCommand->SetGuidance( "Sets the gain for S2 light generation" );
	LUXSimS2GainCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
    LUXSimDriftingElectronAttenuationCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/physicsList/driftElecAttenuation", this );
 	LUXSimDriftingElectronAttenuationCommand->SetGuidance( "Sets the attenuation length for drifting electrons" );
	LUXSimDriftingElectronAttenuationCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	//	Materials commands
	LUXSimMaterialsDir = new G4UIdirectory( "/LUXSim/materials/" );
	LUXSimMaterialsDir->SetGuidance( "Commands to control material properties" );
	
	LUXSimLXeTeflonReflCommand = new G4UIcmdWithADouble( "/LUXSim/materials/LXeTeflonRefl", this );
	LUXSimLXeTeflonReflCommand->SetGuidance( "Sets LXe/Teflon reflectivity (value between 0 and 1)" );
	LUXSimLXeTeflonReflCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimLXeSteelReflCommand = new G4UIcmdWithADouble( "/LUXSim/materials/LXeSteelRefl", this );
	LUXSimLXeSteelReflCommand->SetGuidance( "Sets LXe/steel (i.e. grid) reflectivity (value between 0 and 1)" );
	LUXSimLXeSteelReflCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimLXeAbsCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/materials/LXeAbsorption", this );
	LUXSimLXeAbsCommand->SetGuidance( "Sets the LXe photon absorption length" );
	LUXSimLXeAbsCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimLXeRayleighCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/materials/LXeRayleigh", this );
	LUXSimLXeRayleighCommand->SetGuidance( "Sets the LXe Rayleigh scattering length" );
	LUXSimLXeRayleighCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimGXeTeflonReflCommand = new G4UIcmdWithADouble( "/LUXSim/materials/GXeTeflonRefl", this );
	LUXSimGXeTeflonReflCommand->SetGuidance( "Sets GXe/Teflon reflectivity (value between 0 and 1)" );
	LUXSimGXeTeflonReflCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimGXeSteelReflCommand = new G4UIcmdWithADouble( "/LUXSim/materials/GXeSteelRefl", this );
	LUXSimGXeSteelReflCommand->SetGuidance( "Sets GXe/steel (i.e. grid) reflectivity (value between 0 and 1)" );
	LUXSimGXeSteelReflCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimGXeAbsCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/materials/GXeAbsorption", this );
	LUXSimGXeAbsCommand->SetGuidance( "Sets the GXe photon absorption length" );
	LUXSimGXeAbsCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimGXeRayleighCommand = new G4UIcmdWithADoubleAndUnit( "/LUXSim/materials/GXeRayleigh", this );
	LUXSimGXeRayleighCommand->SetGuidance( "Sets the GXe Rayleigh scattering length" );
	LUXSimGXeRayleighCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
	
	LUXSimAlUnoxQuartzReflCommand = new G4UIcmdWithADouble( "/LUXSim/materials/AlUnoxidizedQuartzRefl", this );
	LUXSimAlUnoxQuartzReflCommand->SetGuidance( "Sets the unoxidized Al / quartz reflectivity (value between 0 and 1)" );
	LUXSimAlUnoxQuartzReflCommand->AvailableForStates( G4State_PreInit, G4State_Idle );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimMessenger()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimMessenger::~LUXSimMessenger()
{
	//	General commands
	delete LUXSimDir;
	delete LUXSimBeamOnCommand;
	delete LUXSimRandomSeedCommand;

	//	Input/output commands
	delete LUXSimFileDir;
	delete LUXSimOutputDirCommand;
	delete LUXSimOutputNameCommand;
	delete LUXSimAlwaysRecordPrimaryCommand;
    delete LUXSimEventProgressCommand;
    delete LUXSim100keVHackCommand;

	// User defined variables commands.
	delete LUXSimUserVar1Command;
	delete LUXSimUserVar2Command;
	delete LUXSimUserVar3Command;
	delete LUXSimUserVar4Command;
	delete LUXSimUserVar5Command;

	//	Geometry commands
	delete LUXSimDetectorDir;
	delete LUXSimUpdateCommand;
	delete LUXSimDetectorSelectCommand;
    delete LUXSimVolumeOverlapCheckCommand;
	delete LUXSimDetectorZCommand;
    delete LUXSimComponentSetMassCommand;
    delete LUXSimComponentSetVolumeCommand;
	delete LUXSimMuonVetoCommand;
	delete LUXSimLZVetoCommand;
	delete LUXSimCryoStandCommand;
	delete LUXSimGridWiresCommand;
	delete LUXSimRecordLevelCommand;
	delete LUXSimRecordLevelOptPhotCommand;
	delete LUXSimRecordLevelThermElecCommand;
	delete LUXSimCollimatorHeightCommand;
    delete LUXSimCollimatorHoleCommand;
	delete LUXSimCollimatorSourceDiameterCommand;
	delete LUXSimLUXSurfaceGeometryCommand;
	delete LUXSimLUXNeutronCollimatorCommand;
	delete LUXSimLUXNeutronCollimatorHeightCommand;
	delete LUXSimLUXDDPencilBeamCommand;
	delete LUXSimLUXSourceTubeGapCommand;
    delete LUXSimGNARRLIPMTCommand;
	delete LUXSimLZFlexMassCommand;
	delete LUXSimLZFlexXeSkinCommand;
	delete LUXSimLZFlexTeflonThicknessCommand;
	delete LUXSimLZFlexScintThicknessCommand;
	delete LUXSimLZNeutronCalibrationTubeCommand;
    delete LUXSimLZNeutronCalibrationTubeTopCommand;
	delete LUXSimLZNeutronCalibrationTubeHeightCommand;
	delete LUXSimLZNeutronCalibrationTubeYPositionCommand;
	delete LUXSimLZCalibrationSourceHeightCommand;
	delete LUXSimLZYBePigCommand;
	delete LUXSimLZYBePigDiameterCommand;
	delete LUXSimLZYBePigHeightCommand;
	delete LUXSimGasRunCommand;
	delete LUXSimCavernRockCommand;

	//Field commands
    delete LUXSimTopPMTBankVoltageCommand;
	delete LUXSimTopGridVoltageCommand;
	delete LUXSimAnodeGridVoltageCommand;
	delete LUXSimGateGridVoltageCommand;
	delete LUXSimCathodeGridVoltageCommand;
	delete LUXSimBottomGridVoltageCommand;
    delete LUXSimBottomPMTBankVoltageCommand;
	delete LUXSimPrintElectricFieldsCommand;
    delete LUXSimEFieldFromFileCommand;
    delete LUXSimEFieldFileCommand;
    delete LUXSimDriftTimeFromFileCommand;
    delete LUXSimDriftTimeFileCommand;
    delete LUXSimRadialDriftFromFileCommand;
    delete LUXSimRadialDriftFileCommand;

    delete LUXSimLUXDoublePheRateFromFileCommand;

    delete LUXFastSimSkewGaussianS2Command;

	//	Source commands
	delete LUXSimSourceDir;
	delete LUXSimSourceSetCommand;
	delete LUXSimSourceResetCommand;
	delete LUXSimSourcePrintCommand;

	//	Physics list commands
	delete LUXSimPhysicsListDir;
	delete LUXSimOpticalPhotonsCommand;
	delete LUXSimOpticalDebugCommand;
    delete LUXSimS1GainCommand;
    delete LUXSimS2GainCommand;
	delete LUXSimDriftingElectronAttenuationCommand;
	
	//	Materials commands
	delete LUXSimMaterialsDir;	
	delete LUXSimLXeTeflonReflCommand;
	delete LUXSimLXeSteelReflCommand;
	delete LUXSimLXeAbsCommand;
	delete LUXSimLXeRayleighCommand;
	delete LUXSimGXeTeflonReflCommand;
	delete LUXSimGXeSteelReflCommand;
	delete LUXSimGXeAbsCommand;
	delete LUXSimGXeRayleighCommand;
	delete LUXSimAlUnoxQuartzReflCommand;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetNewValue()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimMessenger::SetNewValue( G4UIcommand *command, G4String newValue )
{
	//	General commands
	if( command == LUXSimBeamOnCommand )
		luxManager->BeamOn( LUXSimBeamOnCommand->GetNewIntValue(newValue) );
		
	else if( command == LUXSimRandomSeedCommand )
		luxManager->SetRandomSeed( LUXSimRandomSeedCommand->GetNewIntValue(newValue) );
		
	//	Input/output commands
	else if( command == LUXSimOutputDirCommand )
		luxManager->SetOutputDir( newValue );
	else if( command == LUXSimOutputNameCommand )
		luxManager->SetOutputName( newValue );
	else if( command == LUXSimAlwaysRecordPrimaryCommand )
		luxManager->SetAlwaysRecordPrimary( LUXSimAlwaysRecordPrimaryCommand->GetNewBoolValue(newValue) );
	else if( command == LUXSimEventProgressCommand )
		luxManager->SetEventProgressFrequency( LUXSimEventProgressCommand->GetNewIntValue(newValue) );
	else if( command == LUXSim100keVHackCommand )
		luxManager->Set100keVHack( LUXSim100keVHackCommand->GetNewDoubleValue( newValue.data() ) );

	// User defined variables commands
	else if( command == LUXSimUserVar1Command )
		luxManager->SetUserVar1( LUXSimUserVar1Command->GetNewDoubleValue( newValue.data() ) );
	else if( command == LUXSimUserVar2Command )
		luxManager->SetUserVar2( LUXSimUserVar2Command->GetNewDoubleValue( newValue.data() ) );
	else if( command == LUXSimUserVar3Command )
		luxManager->SetUserVar3( LUXSimUserVar3Command->GetNewDoubleValue( newValue.data() ) );
	else if( command == LUXSimUserVar4Command )
		luxManager->SetUserVar4( LUXSimUserVar4Command->GetNewDoubleValue( newValue.data() ) );
	else if( command == LUXSimUserVar5Command )
		luxManager->SetUserVar5( LUXSimUserVar5Command->GetNewDoubleValue( newValue.data() ) );

	//	Geometry commands
	else if( command == LUXSimUpdateCommand )
		luxManager->UpdateGeometry();
	
	else if( command == LUXSimDetectorSelectCommand )
		luxManager->SetDetectorSelection( newValue );
    
    else if( command == LUXSimVolumeOverlapCheckCommand )
        luxManager->SetCheckVolumeOverlaps( LUXSimVolumeOverlapCheckCommand->GetNewBoolValue(newValue) );
	
	else if( command == LUXSimDetectorZCommand )
		luxManager->SetDetectorZ( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
    
    else if( command == LUXSimComponentSetMassCommand )
        luxManager->SetComponentMass( newValue );
	
    else if( command == LUXSimComponentSetVolumeCommand )
        luxManager->SetComponentVolume( newValue );
	
	else if( command == LUXSimMuonVetoCommand )
		luxManager->SetMuonVetoSelection( newValue );

	else if( command == LUXSimLZVetoCommand )
		luxManager->SetLZVetoSelection( newValue );

	else if( command == LUXSimCryoStandCommand )
		luxManager->SetCryoStandSelection( newValue );
	
	else if( command == LUXSimGridWiresCommand )
		luxManager->SetGridWiresSelection( newValue );

	else if( command == LUXSimPMTNumberingCommand )
	  luxManager->SetPMTNumberingScheme( newValue );

	else if( command == LUXSimRecordLevelCommand )
		luxManager->SetRecordLevel( newValue );
	
	else if( command == LUXSimRecordLevelOptPhotCommand )
		luxManager->SetRecordLevelOptPhot( newValue );
	
	else if( command == LUXSimRecordLevelThermElecCommand )
		luxManager->SetRecordLevelThermElec( newValue );
	
	else if( command == LUXSimCollimatorHeightCommand )
		luxManager->SetCollimatorHeight( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if( command == LUXSimCollimatorHoleCommand )
		luxManager->SetCollimatorHoleDiameter( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if( command == LUXSimCollimatorSourceDiameterCommand )
		luxManager->SetCollimatorSourceDiameter( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if( command == LUXSimLUXSurfaceGeometryCommand )
		luxManager->SetLUXSurfaceGeometry( LUXSimLUXSurfaceGeometryCommand->GetNewBoolValue(newValue) );

	else if( command == LUXSimLUXNeutronCollimatorCommand )
		luxManager->SetLUXNeutronCollimator( LUXSimLUXNeutronCollimatorCommand->GetNewBoolValue(newValue) );

	else if( command == LUXSimLUXNeutronCollimatorHeightCommand )
		luxManager->SetLUXNeutronCollimatorHeight( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );

	else if( command == LUXSimLUXDDPencilBeamCommand )
		luxManager->SetLUXDDPencilBeam( LUXSimLUXDDPencilBeamCommand->GetNewBoolValue(newValue) );

	else if( command == LUXSimLUXSourceTubeGapCommand )
		luxManager->SetLUXSourceTubeGap( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if( command == LUXSimLZFlexMassCommand )
		luxManager->SetLZFlexMass( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if( command == LUXSimLZFlexXeSkinCommand )
		luxManager->SetLZFlexXeSkinThickness( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if( command == LUXSimLZFlexTeflonThicknessCommand )
		luxManager->SetLZFlexTeflonThickness( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if( command == LUXSimLZFlexScintThicknessCommand )
		luxManager->SetLZFlexScintThickness( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if( command == LUXSimLZNeutronCalibrationTubeCommand )
		luxManager->SetLZSimpleNeutronCalTube( LUXSimLZNeutronCalibrationTubeCommand->GetNewBoolValue(newValue) );
	
	else if( command == LUXSimLZNeutronCalibrationTubeTopCommand )
		luxManager->SetLZSimpleNeutronCalTopTube( LUXSimLZNeutronCalibrationTubeTopCommand->GetNewBoolValue(newValue) );

	else if( command == LUXSimLZNeutronCalibrationTubeHeightCommand )
		luxManager->SetLZSimpleNeutronCalTubeHeight( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );

	else if( command == LUXSimLZNeutronCalibrationTubeYPositionCommand )
		luxManager->SetLZSimpleNeutronCalTubeYPosition( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if( command == LUXSimLZCalibrationSourceHeightCommand )
		luxManager->SetLZSimpleCalSourceHeight( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );

	else if( command == LUXSimLZYBePigCommand )
		luxManager->SetLZYBePig( LUXSimLZYBePigCommand->GetNewBoolValue(newValue) );

	else if( command == LUXSimLZYBePigDiameterCommand )
		luxManager->SetLZYBePigDiameter( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );

	else if( command == LUXSimLZYBePigHeightCommand )
		luxManager->SetLZYBePigHeight( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
		
	else if( command == LUXSimGasRunCommand )
		luxManager->SetGasRun( LUXSimGasRunCommand->GetNewBoolValue(newValue) );
	
	else if( command == LUXSimTopPMTBankVoltageCommand )
	  luxManager->SetTopPMTBankVoltage( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	else if( command == LUXSimTopGridVoltageCommand )
	  luxManager->SetTopGridVoltage( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	else if( command == LUXSimAnodeGridVoltageCommand )
	  luxManager->SetAnodeGridVoltage( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	else if( command == LUXSimGateGridVoltageCommand )
	  luxManager->SetGateGridVoltage( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	else if( command == LUXSimCathodeGridVoltageCommand )
	  luxManager->SetCathodeGridVoltage( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	else if( command == LUXSimBottomGridVoltageCommand )
	  luxManager->SetBottomGridVoltage( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	else if( command == LUXSimBottomPMTBankVoltageCommand )
	  luxManager->SetBottomPMTBankVoltage( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if( command == LUXSimPrintElectricFieldsCommand )
	  luxManager->PrintElectricFields();

	else if( command == LUXSimEFieldFromFileCommand )
		luxManager->SetEFieldFromFile( LUXSimEFieldFromFileCommand->GetNewBoolValue(newValue) );

	else if( command == LUXSimEFieldFileCommand )
		luxManager->SetEFieldFile( newValue );

	else if( command == LUXSimDriftTimeFromFileCommand )
		luxManager->SetDriftTimeFromFile( LUXSimDriftTimeFromFileCommand->GetNewBoolValue(newValue) );

	else if( command == LUXSimDriftTimeFileCommand )
		luxManager->SetDriftTimeFile( newValue );

	else if( command == LUXSimRadialDriftFromFileCommand )
		luxManager->SetRadialDriftFromFile( LUXSimRadialDriftFromFileCommand->GetNewBoolValue(newValue) );

	else if( command == LUXSimRadialDriftFileCommand )
		luxManager->SetRadialDriftFile( newValue );

	else if( command == LUXSimLUXDoublePheRateFromFileCommand )
		luxManager->SetLUXDoublePheRateFromFile( LUXSimLUXDoublePheRateFromFileCommand->GetNewBoolValue(newValue) );

	else if( command == LUXFastSimSkewGaussianS2Command )
		luxManager->SetLUXFastSimSkewGaussianS2( LUXFastSimSkewGaussianS2Command->GetNewBoolValue(newValue) );

	else if( command == LUXSimCavernRockCommand )
	  luxManager->SetCavernRockSelection( newValue );

	//	Source commands
	else if( command == LUXSimSourceSetCommand )
		luxManager->SetSource( newValue );
        
	else if( command == LUXSimSourceResetCommand )
		luxManager->ResetSources();

	else if( command == LUXSimSourcePrintCommand )
		luxManager->SetPrintEventList( newValue );

	//	Physics list commands
	else if( command == LUXSimOpticalPhotonsCommand )
		luxManager->SetUseOpticalProcesses( LUXSimOpticalPhotonsCommand->GetNewBoolValue(newValue) );

	else if( command == LUXSimOpticalDebugCommand )
		luxManager->SetOpticalDebugging( LUXSimOpticalDebugCommand->GetNewBoolValue(newValue) );
    
    else if( command == LUXSimS1GainCommand )
        luxManager->SetS1Gain( G4UIcmdWithADouble::GetNewDoubleValue(newValue.data()) );
	
    else if( command == LUXSimS2GainCommand )
        luxManager->SetS2Gain( G4UIcmdWithADouble::GetNewDoubleValue(newValue.data()) );
	
	else if( command == LUXSimDriftingElectronAttenuationCommand )
		luxManager->SetDriftElecAttenuation( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	//	Materials commands
	else if ( command == LUXSimLXeTeflonReflCommand )
		luxManager->SetLXeTeflonRefl( G4UIcmdWithADouble::GetNewDoubleValue( newValue.data() ) );
	
	else if ( command == LUXSimLXeSteelReflCommand )
		luxManager->SetLXeSteelRefl( G4UIcmdWithADouble::GetNewDoubleValue( newValue.data() ) );
	
	else if ( command == LUXSimLXeAbsCommand )
		luxManager->SetLXeAbs( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if ( command == LUXSimLXeRayleighCommand )
		luxManager->SetLXeRayleigh( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
	
	else if ( command == LUXSimGXeTeflonReflCommand )
		luxManager->SetGXeTeflonRefl( G4UIcmdWithADouble::GetNewDoubleValue( newValue.data() ) );
	
	else if ( command == LUXSimGXeSteelReflCommand )
		luxManager->SetGXeSteelRefl( G4UIcmdWithADouble::GetNewDoubleValue( newValue.data() ) );
	
	else if ( command == LUXSimGXeAbsCommand )
		luxManager->SetGXeAbs( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
		
	else if ( command == LUXSimGXeRayleighCommand )
		luxManager->SetGXeRayleigh( G4UIcmdWithADoubleAndUnit::GetNewDoubleValue( newValue.data() ) );
		
	else if ( command == LUXSimAlUnoxQuartzReflCommand )
		luxManager->SetAlUnoxQuartzRefl( G4UIcmdWithADouble::GetNewDoubleValue( newValue.data() ) );
	
}
