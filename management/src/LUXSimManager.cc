////////////////////////////////////////////////////////////////////////////////
/*	LUXSimManager.cc
*
* This is the code file to manage all the workings of LUXSim. This class is a
* clearing house for all internal controls, from geometry to output to event
* generation. All classes can and should have access to this manager class, and
* there should only be one instance of this class in the entire simulation.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	20 April 2009 - Added the SetSource method (Kareem)
*	20 April 2009 - Modified SetRecordLevel to key on any substring in the
*					volume name (Kareem)
*	22 April 2009 - Added support for radioactive loads / event generation
*					(Kareem)
*	27 July 2009 - Added a step in the BeamOn method to turn optical physics on
*				   or off. This is not yet fully implemented, pending a
*				   debugging of either LUXSim or GEANT4 (Kareem)
*	13-Aug-09 - Added a section of code to store the command history in a local
*				file, and later open that file, store it in local memory, and
*				delete the file so that it's not taking up any disk space. Also,
*				add code to record the svn diffs to local memory. (Kareem)
*	13-Aug-09 - Added a method to generate a detector component ID lookup table,
*				which will be recorded in the output file. (Kareem)
*	26-Aug-09 - Modified the code to turn Scintillation and Cerenkov processes
*				on or off depending on the appropriate flag. Also fixed a bug
*				where the history file was being prematurely deleted. Also
*				changed the location of the history and diff files to /tmp so
*				that the LUXSim directory doesn't fill up with old files if the
*				simulation doesn't exit cleanly. (Kareem)
*	28-Aug-09 - Inactivating Scintillation has a bug somewhere in the guts of
*				GEANT4. For now, rather than simply deactivating Scintillation,
*				the yield factor is set to 0 to turn off Scintillation and 1
*				to turn it back on. (Kareem)
*	14-Sep-09 - Added the optical photon record level Get and Set methods. Also
*				add the optical photon record keeping in UpdateGeometry.
*				(Kareem)
*	28-Sept-09 - Added cryoStand selection, default off
*	29-Sept-09 - Added gridWires selection, default off
*   4 -Jan -10 - Single nucleus decays can use SingleDecay (Nick)
*	25-Jan-10 - Changed the call to DetermineCenterAndRadius to
*				DetermineCenterAndExtent (Kareem)
*	25-Jan-10 - Added delete calls to the deconstructor to clean up after itself
*				(Kareem)
*	26-Jan-10 - Added the optical record level to the screen output summary of
*				detector components (Kareem)
*	23-Feb-10 - Removed 1_0Detector as default detector -- no geometry is 
*				created until geometry is selected and either manually updated 
*				or BeamOn is called (Dave)
*	23-Mar-10 - Set the randomization engine to the Mersenne Twister (Kareem)
*	26-Mar-10 - Added the /grdm/allVolumes command to the beamOn method to
*				ensure that all volumes can contain decaying nuclei (Kareem)
*   2 -Apr-10 - Added a ResetSource() method. (Nick)
*   2 -Apr-10 - Modified SetSource() to accept Activity/Mass units.(Nick)
*	27-Apr-10 - Added a call to clear the vector of primary particles after
*				the event has been recorded to the output file (Kareem)
*	19-Aug-10 - Added the reset of randomization seed at the end of each beamOn
*				to avoid the output name overwriting. Fixed the bug of repeating
*				randomization seed for each individual run (Chao)
*	19-Aug-10 - Record the number of events passed to the /LUXSim/beamOn command
*				so that later we can determine if the run ended cleanly (Kareem)
*	27-Nov-10 - Added initialization for alwaysRecordPrimary (Kareem)
*	31-Jan-11 - Added the GetComponentRecordLevel method (Kareem)
*   21-Jul-11 - Added Decay Chain Generating methods (Nick)
*	24-Aug-11 - Added default value for numGNARRLIPMTFlag (Tenzing)
*   21-Sep-11 - ElectricField Set Method (Nick)
*	18-Jan-12 - Added functions for changing material properties (Dave)
*	24-Feb-12 - Added LZFlex controls: total mass, Xe skin thickness, and
*				scintillator thickness (Kareem)
*	04-Mar-12 - Modified SetSource() for single particle sources (Nick)
*	06-Mar-12 - Added functions for setting LZFlex Z position (Dave)
*	08-Mar-12 - SVN diff command is now keyed to the compilation directory so
*				that you can run the executable from anywhere without choking
*				on the "svn diff" command (Kareem)
*	09-Mar-12 - Renamed functions for setting Z position (Dave)
*	09-Mar-12 - Added support for Rayleigh UI hooks (Dave)
*	09-Mar-12 - Changed the randomization input seed from /dev/random to
*				/dev/urandom so that requests aren't ever blocked (Kareem)
*	14-Mar-12 - Added a flag to set either the record level or the optical
*				photon record level of all components to the desired value with
*				a single command. (Kareem)
*   24-Mar-12 - Added support for the event progress report UI hooks (Mike)
*	01-May-12 - Added support for thermal electron record levels (Kareem)
*	01-May-12 - Keep using output file name in previous run unless specify
*				manually in macro file. End the LUXSimOut before next run begin.
*				(Chao)
*	06-May-12 - Fixed a bug that prevented recording of events in volumes where
*				only the thermalelectron record level was set. Thanks to Chao
*				for tracking down this bug. (Kareem)
*   18-Jun-12 - Added support for the Wimp generator, in particular,
*               the ability to pass the mass of the Wimp to the generator.
*               (Daniel)
*   12-Jul-12 - Added multi-region electric field support (Nick and Matthew)
*   15-Jul-12 - Overhaul of generator methods to unify handling. Implement
*               BuildEventList, GenerateEventList for all sources and
*               explicitly defined timing for all sources.
*               Additionally ResetSources and UpdateGeometry have been debugged
*               for errors associated with the two macro commands (Nick)
*   16-Aug-12 - Added handling for PMT numbering scheme command (Rich)
*   22-Aug-12 - Updates to fix time-of-event bug and BST degeneracy (Nick)
*   24-Aug-12 - Modification to speed up multiple volume source setting (Nick)
*   17-Sep-12 - Changed PMT numbering to default to real (Rich)
*	25-Sep-12 - Added initialization for luxSurfaceGeometry switch (Kareem)
*	12-Dec-06 - Replaced E-field commands with commands to set grid voltages
*				and added electric field printing command (Dave)
*	12-Dec-07 - Added switch for field calculations when in gas running mode
*				(Dave)
*   04-Feb-13 - Added default values for the S1 and S2 gains (Kareem)
*	22-Feb-13 - Added default value for the drifting electron attenuation length
*				(Kareem)
*   08-Mar-13 - Bug fixed. Bug only appears when setting a source with 
*               multiply-named volumes (i.e. using PMT_Window for all 01-142 
*               PMTs or FieldRing) and if the activity is set per-mass.(Nick)
*	11-Mar-13 - Added GetComponentByName method (Kareem)
*	18-Mar-13 - Added the SetCollimatorSourceDiameter method, and created
*				default values for the collimator height, collimator bore
*				diameter, and collimator source diameter (Kareem)
*	03-Apr-13 - Kludged up grids to make fields accurate. (Matthew)
*	23-Apr-13 - Set the default value for the neutron calibration tube in the
*				LZ geometry to false (Kareem)
*	26-Apr-13 - Set the default height for the LZSimple calibration source
*				(Kareem)
*	17-Jul-13 - Overhaul of the electric field calculations. Implemented Eric
*				D.'s Matlab script (Kareem)
*	18-Jul-13 - Added functions for LUX neutron collimator tube (Kevin)
*   03-Dec-13 - Replaced the constant values for the voltages on the PMT banks
*               with user-settable values. Also set the default PMT bank and
*               grid voltages to their settings during Run 3. (Kareem)
*   19-Mar-14 - Added commands to use an XYZ-dependent electric field from
*               a text file.
*   24-Mar-14 - Moved functions for extracting electric field from text file
*               from G4S1Light.cc to LUXManager.cc and updated the electric
*               field printout functions.
*	07-Apr-14 - Added support for the 100-keV hack (Kareem)			
*   14-Apr-14 - Added commands to use an XYZ-dependent drift time and radial drift
*               from a file. (Kevin)
*   25-Apr-14 - Plugged a memory leak related to the binary search tree (Kareem)
*   14-May-14 - Added option in LZSimple geometry for the tungsten pig for a YBe
*	        source. (Kevin)
*   16-May-14 - Added commands to change location of LZ DD neutron calibration
*      	       	tube (Kevin)
*   20-May-14 - SetUseOpticalProcesses is now in this code file, and
*               automatically sets the cut length in the physics list to short
*               or long, depending on whether optical processes are on or off.
*               Also, useOpticalProcesses is now default "false". (Kareem)
*   21-May-14 - Altered the 100keVHack discrimination to allow for an arbitrary
*               upper limit (Kareem)
*   28-May-14 - Added flag to turn on/off a new LZVeto system (Scott Ha.)
*   03-Sep-14 - Changed source per mass calculation to use the LUXSim mass
*               calculation rather than the stock Geant4 calculation (Kareem)
*   03-Sep-14 - Added default value for checkVolumeOverlaps flag (Kareem)
*   26-Sep-14 - Added option to change YBe pig height and diameter (Kevin)
*   02-Oct-14 - Removed short cut because new NEST doesn't need it (Matthew)
*   14-Oct-14 - Added SetComponentMass and SetComponentVolume methods (Kareem)
*   25-Jan-15 - Added TwoElectrons generator command (Kevin)
*   02-Feb-15 - Added pencil beam command for the LUX DD generator (Kevin)
*   18-Mar-15 - Changed SetMass and SetVolume methods so that the supplied
*               name is used to match any part of a volume name, so that the
*               behavior is similar to the SetSource method (Kareem)
*   11-May-15 - In LUXSimManager::RecalculateFields(), changed the component 
*				locations to be taken from their definitions in G4S1Light.hh.
*				Previously, the comment said that these values had been copied
*				from G4S1Light, but then they were not updated when G4S1Light.hh
*				was modified.
*   08-Jun-15 - Modified the code so that the LUX and LZ NEST parameters are
*               automatically loaded and do not interfere with each other
*               (Kareem)
*   23-Jul-15 - Print out electric fields after beamOn is called for
*               verification that they are set correctly (Kareem)
*   27-Aug-15 - Fixed bug in upper energy limit cut, formerly known as the "100-
*               keV hack" (Kareem)
*   28-Aug-15 - Changed the way arguments are parsed to accommodate point sources 
*               (David W)
*   28-Sep-15 - The manager now supports the code being in an SVN or Git repo
*               (or no version control) (Kareem)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	C/C++ includes
//
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>

//
//	CLHEP includes
//
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "Randomize.hh"

//
//	GEANT4 includes
//
#include "G4UImanager.hh"
#include "G4GeneralParticleSource.hh"
#include "G4Event.hh"
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimManager.hh"
#include "LUXSimPhysicsList.hh"
#include "LUXSimPhysicsOpticalPhysics.hh"
#include "LUXSimDetectorConstruction.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimPrimaryGeneratorAction.hh"
#include "LUXSimRunAction.hh"
#include "LUXSimEventAction.hh"
#include "LUXSimSteppingAction.hh"
#include "LUXSimOutput.hh"
#include "LUXSimSourceCatalog.hh"
#include "LUXSimSource.hh"
#include "LUXSimBST.hh"
#include "LUXSimMessenger.hh"
#include "LUXSimStand.hh"
#include "LUXSimLZFlex.hh"
#include "G4S1Light.hh"

using namespace std;
using namespace CLHEP;

        extern const int NumPlanesX ;  //placeholder value but should be close

        extern const int NumPlanesY ;  //placeholder value

        extern const int NumPlanesZ ;  //placeholder value

        extern const G4double MinLocationX ;  //placeholder value

        extern const G4double MinLocationY ;  //placeholder value

        extern const G4double MinLocationZ;  //placeholder value, but probably correct

        extern const G4double StepSizeX   ;//Subject to change
	extern const G4double StepSizeY ;
	extern const G4double StepSizeZ ;
        extern const int NumxyzDependentEFieldVars  ;//xi, yi, zi, E, S2x, S2y, drift_time

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimManager()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimManager *LUXSimManager::LUXManager = 0;
LUXSimManager::LUXSimManager()
{
	//	Set the random seed
	CLHEP::HepRandom::setTheEngine( &randomizationEngine );
	
	ifstream devurandom("/dev/urandom");
	G4int seed;
	devurandom.read( (char*)(&seed), sizeof( G4int ) );
	if( seed < 0 ) seed = -seed;
	devurandom.close();
	randomSeed = seed;
	CLHEP::HepRandom::setTheSeed( randomSeed );
	
	UI = G4UImanager::GetUIpointer();
	stringstream historyFileStream;
	historyFileStream << "/tmp/LUXSimCommandHistory_" << randomSeed << ".txt";
	historyFile = historyFileStream.str();
	UI->StoreHistory( historyFile.c_str() );
		
	LUXManager = this;
	LUXMessenger = new LUXSimMessenger( this );
	LUXSimOut = NULL;
	
	luxSimComponents.clear();
    
    recordTree = 0;
	
	outputDir = ".";
	
	alwaysRecordPrimary = true;
    eventProgressFrequency = 100000;

    hasLUXSimSources = false;
    isEventListBuilt = false;
    printEventList = false;
    hasDecayChainSources = false;
    windowEnd = 0.;
    
    use100keVHack = 0;
    liquidXenonTotalEnergy = 0;
	
	//detectorSelection = "1_0Detector";
	detectorSelection = "";
    checkVolumeOverlaps = false;
	muonVetoSelection = "off";
	LZVetoSelection = "off";
	cryoStandSelection = "off";
	gasRun = false;
	gridWiresSelection = "off";
	useOpticalProcesses = false;
	numGNARRLIPMTFlag = false;
	useRealPMTNumberingScheme = true;
	luxSurfaceGeometry = false;
	luxNeutronCollimator = false;
	luxNeutronCollimatorHeight = 25.*cm;
        luxDDPencilBeam = false;
	luxSourceTubeGap = 1.52*cm;
	
	collimator_height = 30.*cm;
	collimator_hole = 1.*mm;
	collimatorSourceDiameter = 5.*mm;
	
	lzFlexMass = -1;
	lzFlexXeSkinThickness = -1;
	lzFlexTeflonThickness = -1;
	lzFlexScintillatorThickness = -1;
	lzSimpleNeutronCal = false;
	lzSimpleNeutronCalHeight = 57.65*cm;
	lzSimpleNeutronCalYPosition = 0.*cm;
	lzSimpleCalSourceHeight = 50.*cm;

	lzYBePig = false;
	lzYBePigDiameter = 24. * cm;
	lzYBePigHeight = 14. * cm;

	eFieldGas = 0.;
	eFieldGasWindow = 0.;
	eFieldGasTop = 0.;
	eFieldGasAnode = 0.;
	eFieldLiq = 0.;
	eFieldLiqSurface = 0.;
	eFieldLiqGate = 0.;
	eFieldLiqCathode = 0.;
	eFieldLiqBottom = 0.;
	
    //  These default values are for Run 3
    topPMTBankVoltage = -1200.*volt;
	topGridVoltage = -1000.*volt;
	anodeGridVoltage = 3500.*volt;
	gateGridVoltage = -1500.*volt;
	cathodeGridVoltage = -10000.*volt;
	bottomGridVoltage = -2000.*volt;
    bottomPMTBankVoltage = -1200.*volt;

    EFieldFromFile = false; // default to calculating e-field from voltages
    EFieldFile = "physicslist/src/FieldFile.dat"; // default to calculating e-field from voltages
    DriftTimeFromFile = false; // default to calculating e-field from voltages
    DriftTimeFile = "physicslist/src/DriftTimeFile.dat"; // default to calculating e-field from voltages
    RadialDriftFromFile = false; // default to calculating e-field from voltages
    RadialDriftFile = "physicslist/src/RadialDriftFile.dat"; // default to calculating e-field from voltages

    luxDoublePheRateFromFile = true;

    luxFastSimSkewGaussianS2 = false;

    s1gain = 1;
    s2gain = 1;
	
	driftElecAttenuation = 1.*m;

     currentEvtN = -1;
	
	LUXSimDetector = NULL;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimManager()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimManager::~LUXSimManager()
{
	if ( LUXSimOut ) delete LUXSimOut;
	if ( LUXSimSourceCat ) delete LUXSimSourceCat;
	
	stringstream rmCommand;
	rmCommand << "rm -rf " << historyFile;
	system( rmCommand.str().c_str() );

	LUXManager = NULL;
	G4cout << "LUXSim manager deleted." << G4endl;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetManager()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimManager *LUXSimManager::GetManager()
{
	return LUXManager;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					BeamOn()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::BeamOn( G4int numOfEvents )
{
	//	Record the total number of events to run
	numEvents = numOfEvents;

	//	Turn on radioactive decay in all volumes
	UI->ApplyCommand( "/grdm/allVolumes");

	// Check if we have created the geometry yet
	if ( !LUXSimDetector->DetectorSpecified() )
	{
		if ( detectorSelection == "" ) // No detector has been selected
		{
			G4cout << G4endl << 
				"*************************************************" << 
				G4endl << "WARNING: no geometry selected." << G4endl << 
				"Using 1_0Detector geometry by default." << G4endl <<
				"No hits will be recorded to output file." << G4endl <<
				"*************************************************" << 
				G4endl << G4endl;
			detectorSelection = "1_0Detector";
		}
		else // A detector has been selected but we haven't updated yet
		{
			G4cout << G4endl << 
				"*************************************************" << 
				G4endl << "WARNING: geometry is updating for the first time, " 
				<< G4endl << "wiping record level settings." << G4endl << 
				"No hits will be recorded to output file." << G4endl << 
				"*************************************************" << 
				G4endl << G4endl;
		}
		
		UpdateGeometry();
	}

	G4cout << "BEAM ON " << G4endl;
	
    //  Set the electric field values according to whether we're running with
    //  the LUX or LZ geometries
    if( detectorSelection == "LZDetector" ) {
        eFieldGas = -10.6*kilovolt/cm;
        eFieldGasWindow = 1.72*kilovolt/cm;
        eFieldGasTop = 1.72*kilovolt/cm;
        eFieldGasAnode = -10.60*kilovolt/cm;
        eFieldLiq = -700.0*volt/cm;
        eFieldLiqSurface = -5.42*kilovolt/cm;
        eFieldLiqGate = -640.0*volt/cm;
        eFieldLiqCathode = 6.68*kilovolt/cm;
        eFieldLiqBottom = 570.0*volt/cm;
        
        LUXSimMat->UpdateElectricField();
        LUXSimPhysicsOptical->GetScintillation()->SetLZGeoValues();
        PrintElectricFields();
    } else {
        eFieldGas = 0.;
        eFieldGasWindow = 0.;
        eFieldGasTop = 0.;
        eFieldGasAnode = 0.;
        eFieldLiq = 0.;
        eFieldLiqSurface = 0.;
        eFieldLiqGate = 0.;
        eFieldLiqCathode = 0.;
        eFieldLiqBottom = 0.;
        
        topPMTBankVoltage = -1200.*volt;
        topGridVoltage = -1000.*volt;
        anodeGridVoltage = 3500.*volt;
        gateGridVoltage = -1500.*volt;
        cathodeGridVoltage = -10000.*volt;
        bottomGridVoltage = -2000.*volt;
        bottomPMTBankVoltage = -1200.*volt;
        RecalculateFields();
        
        LUXSimMat->UpdateElectricField();
        LUXSimPhysicsOptical->GetScintillation()->SetLUXGeoValues();
        PrintElectricFields();
    }
    
	// Create new LUXSimOutput object
	if (LUXSimOut)
		delete LUXSimOut;
	new LUXSimOutput();

	//	Print info to the screen, and assign volume IDs
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ ) {
		
		luxSimComponents[i]->SetID( i+1 );
		
		G4cout << "Volume \"" <<  luxSimComponents[i]->GetName() << "\" "
			   << "assigned ID " << i+1
			   << ", record level " << luxSimComponents[i]->GetRecordLevel()
			   << ", opt record level "
			   << luxSimComponents[i]->GetRecordLevelOptPhot()
			   << ", therm elec record level "
			   << luxSimComponents[i]->GetRecordLevelThermElec()
			   << G4endl;
		luxSimComponents[i]->DetermineCenterAndExtent(
				LUXSimDetector->GetWorldVolume());
	}
	G4cout << G4endl << G4endl;
	
	//	Calculate the final source ratios and print that info to the screen
	sourceByVolume.clear();
	totalSimulationActivity = 0;
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ ) {
		luxSimComponents[i]->CalculateRatios();
		if( luxSimComponents[i]->GetTotalActivity() ) {
			volumeSource tempVolumeSource;
			tempVolumeSource.component = luxSimComponents[i];
			tempVolumeSource.totalVolumeActivity =
					luxSimComponents[i]->GetTotalActivity();
			tempVolumeSource.totalVolumeActivityRatio = 0;
			sourceByVolume.push_back( tempVolumeSource );

			totalSimulationActivity += tempVolumeSource.totalVolumeActivity;
		}
	}
	
	G4cout << "Total activity in the entire simulation = "
		   << totalSimulationActivity << " Bq" << G4endl;
	for( G4int i=0; i<(G4int)sourceByVolume.size(); i++ ) {
		sourceByVolume[i].totalVolumeActivityRatio =
				sourceByVolume[i].totalVolumeActivity / totalSimulationActivity;
		G4cout << "\tActivity ratio for "
			   << sourceByVolume[i].component->GetName() << " = "
			   << sourceByVolume[i].totalVolumeActivityRatio << G4endl;
	}
	for( G4int i=1; i<(G4int)sourceByVolume.size(); i++ )
		sourceByVolume[i].totalVolumeActivityRatio +=
			sourceByVolume[i-1].totalVolumeActivityRatio;
	
	//	Turn the optical processes on or off
	G4cout << "useOpticalProcesses = " << useOpticalProcesses << G4endl;
	if( useOpticalProcesses ) {
		LUXSimPhysicsOptical->GetScintillation()->SetScintillationYieldFactor(
				1.);
//		UI->ApplyCommand( "/process/activate Scintillation" );
		UI->ApplyCommand( "/process/activate Cerenkov" );
	} else {
		LUXSimPhysicsOptical->GetScintillation()->SetScintillationYieldFactor(
				0);
//		UI->ApplyCommand( "/process/inactivate Scintillation" );
		UI->ApplyCommand( "/process/inactivate Cerenkov" );	
	}
	
	//	This next chunk of code opens the stored history of commands, and saves
	//	that list in the local string.
	ifstream commands( historyFile.c_str() );
	commands.seekg( 0, ios::end );
	int length = commands.tellg();
	commands.seekg( 0, ios::beg );
	char *inputBuffer;
	inputBuffer = new char[length];
	commands.read( inputBuffer, length );
	commands.close();
	listOfCommands = inputBuffer;
	delete [] inputBuffer;
	
    //	Next, create a master diff file relative to the current version of
    //	Bacc. Save this diff file in local memory for later access.
    if ( IsSVNRepo ) {
        stringstream diffFileName;
        diffFileName << "/tmp/diff_" << randomSeed << ".txt";
        stringstream diffCommand;
        diffCommand << "svn diff " << compilationDir << " > " << diffFileName.str();
        system( diffCommand.str().c_str() );
        ifstream diffs( diffFileName.str().c_str() );
        diffs.seekg( 0, ios::end );
        length = diffs.tellg();
        diffs.seekg( 0, ios::beg );
        inputBuffer = new char[length];
        diffs.read( inputBuffer, length );
        diffs.close();
        stringstream rmCommand;
        rmCommand.str("");
        rmCommand << "rm -rf " << diffFileName.str();
        system( rmCommand.str().c_str() );
        listOfDiffs = inputBuffer;
        delete [] inputBuffer;
    } else if ( IsGitRepo ) {
        stringstream diffFileName;
        diffFileName << "/tmp/diff_" << randomSeed << ".txt";
        stringstream diffCommand;
        diffCommand << "git diff " << compilationDir << " > " << diffFileName.str();
        system( diffCommand.str().c_str() );
        ifstream diffs( diffFileName.str().c_str() );
        diffs.seekg( 0, ios::end );
        length = diffs.tellg();
        diffs.seekg( 0, ios::beg );
        inputBuffer = new char[length];
        diffs.read( inputBuffer, length );
        diffs.close();
        stringstream rmCommand;
        rmCommand.str("");
        rmCommand << "rm -rf " << diffFileName.str();
        system( rmCommand.str().c_str() );
        listOfDiffs = inputBuffer;
        delete [] inputBuffer;
    } else {
        listOfDiffs = "No diffs available - ";
        listOfDiffs +=  "This was not recognized as either a git repository ";
        listOfDiffs +=  "or an svn repo. \n";
    }
	
    // All sources are added to a binary search tree ordered by time before
    // Geant begins to "generate events"
    if( hasLUXSimSources ) {
        BuildEventList();
        GenerateEventList();
        TrimEventList();
        if( printEventList ) PrintEventList();
    }

	// Record input history before beamOn
	LUXSimOut->RecordInputHistory();

	//	Finally, run the beamOn command
	stringstream command;
	command << "/run/beamOn " << numOfEvents;
	UI->ApplyCommand( command.str() );

	//      Reset randomization seed for next beanOn
        CLHEP::HepRandom::setTheEngine( &randomizationEngine );

        ifstream devrandom("/dev/random");
        G4int seed;
        devrandom.read( (char*)(&seed), sizeof( G4int ) );
        if( seed < 0 ) seed = -seed;
        devrandom.close();
        randomSeed = seed;
        CLHEP::HepRandom::setTheSeed( randomSeed );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetRandomSeed()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetRandomSeed( G4int seed )
{	
	randomSeed = seed;
	G4cout << "Randomization seed = " << randomSeed << G4endl;
	CLHEP::HepRandom::setTheSeed( randomSeed );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetOutputDir()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetOutputDir( G4String dir )
{	
	if( !dir.compare( dir.length()-1, 1, "/" ) )
		outputDir = dir;
	else
		outputDir = dir + "/";
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					UpdateGeometry()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::UpdateGeometry()
{
	//	First, record the current record levels and source types / strengths
	vector<G4String> volNames;
	vector<G4int> recordLevels;
	vector<G4int> recordLevelsOptPhot;
	vector<G4int> recordLevelsThermElec;
	LUXSimDetectorComponent::source tempSource;
	vector<LUXSimDetectorComponent::source> sources;
	vector<G4String> sourceVolNames;

    //G4S1Light update	
	LUXSimMat->UpdateElectricField();

	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ ) {
		volNames.push_back( luxSimComponents[i]->GetName() );
		recordLevels.push_back( luxSimComponents[i]->GetRecordLevel() );
		recordLevelsOptPhot.push_back(
				luxSimComponents[i]->GetRecordLevelOptPhot() );
		recordLevelsThermElec.push_back(
				luxSimComponents[i]->GetRecordLevelThermElec() );

		vector<LUXSimDetectorComponent::source> origSources =
				luxSimComponents[i]->GetSources();
		for( G4int j=0; j<(G4int)origSources.size(); j++ ) {
			tempSource.type = origSources[j].type;
			tempSource.activity = origSources[j].activity;
			tempSource.ratio = 0;
            tempSource.mass = origSources[j].mass;
            tempSource.number = origSources[j].number;
            tempSource.parentIsotope = origSources[j].parentIsotope;
            tempSource.sourceAge = origSources[j].sourceAge;
			sources.push_back( tempSource );
			sourceVolNames.push_back( luxSimComponents[i]->GetName() );
		}
	}	

	//	Next, update the geometry, which wipes out all detector-component-
	//	related info
	luxSimComponents.clear();
	LUXSimDetector->UpdateGeometry();
	
	// reset collimator geometry
	if (LUXSimDetector->GetCryoStand())
	{
		LUXSimDetector->GetCryoStand()->SetCollimatorHeight(
				collimator_height );
		LUXSimDetector->GetCryoStand()->SetCollimatorHoleDiameter(
				collimator_hole );
		LUXSimDetector->GetCryoStand()->SetCollimatorSourceDiameter(
				collimatorSourceDiameter );
	}
	
	//	Finally, reset the recorded record levels and source types / activities
	if( (recordLevels.size() != recordLevelsOptPhot.size()) ||
			(recordLevels.size() != volNames.size()) )
	{
		G4cout << G4endl << G4endl;
		G4cout << "Woah! Found a big bug in the manager. There is not the same"
			   << "number of record" << G4endl
			   << "levels and optical photon record levels. Send an e-mail to"
			   << "Kareem" << G4endl
			   << "(kareem@llnl.gov) and let him know this happened. Please."
			   << G4endl;
		exit(0);
	}
	
	stringstream info;
	
	for ( G4int i=0; i<(G4int)recordLevels.size(); i++ )
	{
		info.str("");
		info << volNames[i] << " " << recordLevels[i];
		SetRecordLevel( info.str() );

		info.str("");
		info << volNames[i] << " " << recordLevelsOptPhot[i];
		SetRecordLevelOptPhot( info.str() );

		info.str("");
		info << volNames[i] << " " << recordLevelsThermElec[i];
		SetRecordLevelThermElec( info.str() );
	}
	
	for ( G4int i=0; i<(G4int)sourceVolNames.size(); i++ )
	{
		info.str("");
	    info << sourceVolNames[i] << " " << sources[i].type->GetName();

		if (sources[i].type->GetName().find("DecayChain") < G4String::npos) 
            info << "_" << sources[i].parentIsotope << " " 
                 << sources[i].activity<< " Bq " << sources[i].sourceAge << " "
                 << " s" ;
		else if (sources[i].type->GetName().find("SingleParticle")
                                                          < G4String::npos)
            info << "_" << sources[i].particleName << " " 
                << sources[i].activity << " Bq " << sources[i].particleEnergy
                << " keV" ;
		else if (sources[i].type->GetName().find("SingleDecay") 
                                                          < G4String::npos) 
		    info << "_" << sources[i].mass << "_" << sources[i].number
				 << " " << sources[i].activity << " Bq";
        else if (sources[i].type->GetName().find("Wimp") < G4String::npos)
            info << " " << sources[i].activity << " Bq " 
                 << sources[i].particleEnergy << " keV";
        else
            info << " " << sources[i].activity << " Bq";
        SetSource( info.str() );
	}
	
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					Deregister()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::Deregister( LUXSimDetectorComponent *component )
{
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		if( luxSimComponents[i] == component ) {
			luxSimComponents.erase( luxSimComponents.begin() + i );
			i = luxSimComponents.size();
		}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetDetectorComponentLookupTable()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4String LUXSimManager::GetDetectorComponentLookupTable()
{
	stringstream tableStream;
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		tableStream << "ID " << luxSimComponents[i]->GetID() << ": "
					<< luxSimComponents[i]->GetName() << G4endl;
	return tableStream.str();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetRecordLevel()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetRecordLevel( G4String info )
{
	//	Start by splitting the G4String info into two parts: the volume name
	//	and the record level.
	
	G4String volName;
	volName = info.substr( 0, info.find_first_of(" ") );
	info = info.substr( info.find_first_of(" ") );
	G4int level = atoi( info.c_str() );
	
	if( volName == "***" ) {
		for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
			luxSimComponents[i]->SetRecordLevel( level );
	} else
		for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
			if( luxSimComponents[i]->GetName().find(volName) < G4String::npos )
				luxSimComponents[i]->SetRecordLevel( level );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetRecordLevels()
//------++++++------++++++------++++++------++++++------++++++------++++++------
std::vector<G4int> LUXSimManager::GetRecordLevels( G4String volName )
{
	std::vector<G4int> levels;

	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		if( luxSimComponents[i]->GetName().find(volName) < G4String::npos )
			levels.push_back( luxSimComponents[i]->GetRecordLevel() );
	
	return levels;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetComponentRecordLevel()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4int LUXSimManager::GetComponentRecordLevel(
		LUXSimDetectorComponent *component )
{
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		if( luxSimComponents[i] == component )
			return( component->GetRecordLevel() );
	
	G4cout << "Warning! Looking for a record level in the"
		   << "\"" << ((LUXSimDetectorComponent*)component)->GetName() << "\" "
		   << "volume, which doesn't seem to" << G4endl
		   << "be a LUXSimDetectorComponent object." << G4endl;
	
	return 0;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetRecordLevelOptPhot()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetRecordLevelOptPhot( G4String info )
{
	//	Start by splitting the G4String info into two parts: the volume name
	//	and the record level.
	
	G4String volName;
	volName = info.substr( 0, info.find_first_of(" ") );
	info = info.substr( info.find_first_of(" ") );
	G4int level = atoi( info.c_str() );
	
	if( volName == "***" ) {
		for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
			luxSimComponents[i]->SetRecordLevelOptPhot( level );
	} else
		for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ ) {
			if( luxSimComponents[i]->GetName().find(volName) < G4String::npos )
				luxSimComponents[i]->SetRecordLevelOptPhot( level );
		}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetRecordLevelsOptPhot()
//------++++++------++++++------++++++------++++++------++++++------++++++------
std::vector<G4int> LUXSimManager::GetRecordLevelsOptPhot( G4String volName )
{
	std::vector<G4int> levels;

	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		if( luxSimComponents[i]->GetName().find(volName) < G4String::npos )
			levels.push_back( luxSimComponents[i]->GetRecordLevelOptPhot() );
	
	return levels;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetComponentRecordLevelOptPhot()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4int LUXSimManager::GetComponentRecordLevelOptPhot(
		LUXSimDetectorComponent *component )
{
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		if( luxSimComponents[i] == component )
			return( component->GetRecordLevelOptPhot() );
	
	G4cout << "Warning! Looking for an optical photon record level in the"
		   << "\"" << ((LUXSimDetectorComponent*)component)->GetName() << "\" "
		   << "volume, which doesn't seem to" << G4endl
		   << "be a LUXSimDetectorComponent object." << G4endl;
	
	return 0;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetRecordLevelThermElec()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetRecordLevelThermElec( G4String info )
{
	//	Start by splitting the G4String info into two parts: the volume name
	//	and the record level.
	
	G4String volName;
	volName = info.substr( 0, info.find_first_of(" ") );
	info = info.substr( info.find_first_of(" ") );
	G4int level = atoi( info.c_str() );
	
	if( volName == "***" ) {
		for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
			luxSimComponents[i]->SetRecordLevelThermElec( level );
	} else
		for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ ) {
			if( luxSimComponents[i]->GetName().find(volName) < G4String::npos )
				luxSimComponents[i]->SetRecordLevelThermElec( level );
		}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetRecordLevelsThermElec()
//------++++++------++++++------++++++------++++++------++++++------++++++------
std::vector<G4int> LUXSimManager::GetRecordLevelsThermElec( G4String volName )
{
	std::vector<G4int> levels;

	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		if( luxSimComponents[i]->GetName().find(volName) < G4String::npos )
			levels.push_back( luxSimComponents[i]->GetRecordLevelThermElec() );
	
	return levels;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetComponentRecordLevelThermElec()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4int LUXSimManager::GetComponentRecordLevelThermElec(
		LUXSimDetectorComponent *component )
{
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		if( luxSimComponents[i] == component )
			return( component->GetRecordLevelThermElec() );
	
	G4cout << "Warning! Looking for a thermal electron record level in the"
		   << "\"" << ((LUXSimDetectorComponent*)component)->GetName() << "\" "
		   << "volume, which doesn't seem to" << G4endl
		   << "be a LUXSimDetectorComponent object." << G4endl;
	
	return 0;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetComponentByName()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimDetectorComponent *LUXSimManager::GetComponentByName( G4String volName )
{
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		if( luxSimComponents[i]->GetName() == volName )
			return( luxSimComponents[i] );
	
	G4cout << "Warning! Could not find component \"" << volName << "\""
		   << G4endl;
	
	return 0;
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetDetectorZ()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetDetectorZ( G4double z )
{	
	detZ = z;
	
	if( LUXSimDetector == NULL )
	{
		G4cout << "WARNING -- activating 1.0 detector with muon veto"
			   << G4endl;
		detectorSelection = "1_0Detector";
		muonVetoSelection = "on";
		cryoStandSelection = "off";
		UpdateGeometry();
	}
	
	LUXSimDetector->SetDetectorZ( z );
	UpdateGeometry();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetComponentMass()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetComponentMass( G4String parameters )
{
    G4String componentName = parameters.substr( 0, parameters.find(" ") );
    parameters = parameters.substr( parameters.find(" ") + 1);
    G4String massString = parameters.substr( 0, parameters.find(" ") );
    parameters = parameters.substr( parameters.find(" ") + 1);
    G4String unitString = parameters.substr( 0, parameters.find(" ") );
    
    G4double mass = atof( massString.c_str() );
    
    mass *= g;
    if( unitString == "ug" ) mass *= 1e-6;
    if( unitString == "mg" ) mass *= 1e-3;
    if( unitString == "kg" ) mass *= 1e3;
    
    for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
        if( luxSimComponents[i]->GetName().find(componentName)<G4String::npos )
            luxSimComponents[i]->SetMass( mass );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetComponentVolume()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetComponentVolume( G4String parameters )
{	
    G4String componentName = parameters.substr( 0, parameters.find(" ") );
    parameters = parameters.substr( parameters.find(" ") + 1);
    G4String volumeString = parameters.substr( 0, parameters.find(" ") );
    parameters = parameters.substr( parameters.find(" ") + 1);
    G4String unitString = parameters.substr( 0, parameters.find(" ") );
    
    G4double volume = atof( volumeString.c_str() );
    
    volume *= cm3;
    if( unitString == "mm3" ) volume *= 1e-3;
    if( unitString == "m3" ) volume *= 1e6;
    
     for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
        if( luxSimComponents[i]->GetName().find(componentName)<G4String::npos )
            luxSimComponents[i]->SetVolume( volume );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetCollimatorHeight()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetCollimatorHeight( G4double height )
{
	collimator_height = height;
	
	if ( LUXSimDetector == NULL || LUXSimDetector->GetCryoStand() == NULL || 
		detectorSelection != "1_0Detector" || cryoStandSelection != "on" )
	{
		G4cout << "WARNING -- activating 1.0 detector with cryo stand"
			   << G4endl;
		detectorSelection = "1_0Detector";
		cryoStandSelection = "on";
		UpdateGeometry();
	}
	
	LUXSimDetector->GetCryoStand()->SetCollimatorHeight( height );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetCollimatorHoleDiameter()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetCollimatorHoleDiameter( G4double diameter )
{
	collimator_hole = diameter;
	
	if ( LUXSimDetector == NULL || LUXSimDetector->GetCryoStand() == NULL || 
		detectorSelection != "1_0Detector" || cryoStandSelection != "on" )
	{
		G4cout << "WARNING -- activating 1.0 detector with cryo stand"
			   << G4endl;
		detectorSelection = "1_0Detector";
		cryoStandSelection = "on";
		UpdateGeometry();
	}
	
	LUXSimDetector->GetCryoStand()->SetCollimatorHoleDiameter( diameter );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetCollimatorSourceDiameter()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetCollimatorSourceDiameter( G4double diameter )
{
	collimatorSourceDiameter = diameter;
	
	if ( LUXSimDetector == NULL || LUXSimDetector->GetCryoStand() == NULL || 
		detectorSelection != "1_0Detector" || cryoStandSelection != "on" )
	{
		G4cout << "WARNING -- activating 1.0 detector with cryo stand"
			   << G4endl;
		detectorSelection = "1_0Detector";
		cryoStandSelection = "on";
		UpdateGeometry();
	}
	
	LUXSimDetector->GetCryoStand()->SetCollimatorSourceDiameter( diameter );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLZFlexMass()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetLZFlexMass( G4double mass )
{	
	lzFlexMass = mass;
	
	if( LUXSimDetector == NULL || LUXSimDetector->GetWaterTank() == NULL || 
			detectorSelection != "LZFlex" || muonVetoSelection != "on" ) {
		
		G4cout << "WARNING -- activating LZFlex detector with muon veto"
			   << G4endl;
		detectorSelection = "LZFlex";
		muonVetoSelection = "on";
		cryoStandSelection = "off";
	}

	UpdateGeometry();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLZFlexXeSkinThickness()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetLZFlexXeSkinThickness( G4double thickness )
{	
	lzFlexXeSkinThickness = thickness;
	
	if( LUXSimDetector == NULL || LUXSimDetector->GetWaterTank() == NULL || 
			detectorSelection != "LZFlex" || muonVetoSelection != "on" ) {
		
		G4cout << "WARNING -- activating LZFlex detector with muon veto"
			   << G4endl;
		detectorSelection = "LZFlex";
		muonVetoSelection = "on";
		cryoStandSelection = "off";
	}

	UpdateGeometry();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLZFlexTeflonThickness()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetLZFlexTeflonThickness( G4double thickness )
{	
	lzFlexTeflonThickness = thickness;
	
	if( LUXSimDetector == NULL || LUXSimDetector->GetWaterTank() == NULL || 
			detectorSelection != "LZFlex" || muonVetoSelection != "on" ) {
		
		G4cout << "WARNING -- activating LZFlex detector with muon veto"
			   << G4endl;
		detectorSelection = "LZFlex";
		muonVetoSelection = "on";
		cryoStandSelection = "off";
	}

	UpdateGeometry();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLZFlexScintThickness()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetLZFlexScintThickness( G4double thickness )
{	
	lzFlexScintillatorThickness = thickness;
	
	if( LUXSimDetector == NULL || LUXSimDetector->GetWaterTank() == NULL || 
			detectorSelection != "LZFlex" || muonVetoSelection != "on" ) {

		G4cout << "WARNING -- activating LZFlex detector with muon veto"
			   << G4endl;
		detectorSelection = "LZFlex";
		muonVetoSelection = "on";
		cryoStandSelection = "off";
	}
	
	UpdateGeometry();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetTopPMTBankVoltage()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetTopPMTBankVoltage( G4double v )
{
	G4cout << "Top PMT bank voltage set to " << v/volt << " V" << G4endl;
	topPMTBankVoltage = v;
	RecalculateFields();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetTopGridVoltage()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetTopGridVoltage( G4double v )
{
	G4cout << "Top grid voltage set to " << v/volt << " V" << G4endl;
	topGridVoltage = v;
	RecalculateFields();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetAnodeGridVoltage()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetAnodeGridVoltage( G4double v )
{
	G4cout << "Anode grid voltage set to " << v/volt << " V" << G4endl;
	anodeGridVoltage = v;
	RecalculateFields();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetGateGridVoltage()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetGateGridVoltage( G4double v )
{
	G4cout << "Gate grid voltage set to " << v/volt << " V" << G4endl;
	gateGridVoltage = v;
	RecalculateFields();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetCathodeGridVoltage()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetCathodeGridVoltage( G4double v )
{
	G4cout << "Cathode grid voltage set to " << v/volt << " V" << G4endl;
	cathodeGridVoltage = v;
	RecalculateFields();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetBottomGridVoltage()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetBottomGridVoltage( G4double v )
{
	G4cout << "Bottom grid voltage set to " << v/volt << " V" << G4endl;
	bottomGridVoltage = v;
	RecalculateFields();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetBottomPMTBankVoltage()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetBottomPMTBankVoltage( G4double v )
{
	G4cout << "Bottom PMT bank voltage set to " << v/volt << " V" << G4endl;
	bottomPMTBankVoltage = v;
	RecalculateFields();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					PrintElectricFields()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::PrintElectricFields()
{

        if (detectorSelection == "1_0Detector" && EFieldFromFile) {
           G4cout<<"-----------------------------------------------------------"<<G4endl;
           G4cout<<"-----------------------------------------------------------"<<G4endl;
           LoadXYZDependentEField(EFieldFile);
    	   G4cout << "Electric field T -> top PMTs: " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 588.))/(volt/cm) << 
		   " V/cm to " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 597.))/(volt/cm) << G4endl;
    	   G4cout << "Electric field A -> T: " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 554.))/(volt/cm) << 
		   " V/cm to " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 587.))/(volt/cm) << G4endl;
    	   G4cout << "Electric field liquid surface -> A: " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 548.))/(volt/cm) << 
		   " V/cm to " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 550.))/(volt/cm) << G4endl;
    	   G4cout << "Electric field G -> liquid surface: " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 543.))/(volt/cm) << 
		   " V/cm to " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 548.))/(volt/cm) << G4endl;
    	   G4cout << "Electric field C -> G: " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 66.))/(volt/cm) << 
		   " V/cm to " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 535.))/(volt/cm) << G4endl;
    	   G4cout << "Electric field B -> C: " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 19.7))/(volt/cm) << 
		   " V/cm to " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 46.))/(volt/cm) << G4endl;
    	   G4cout << "Electric field bottom PMTs -> B: " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 1.))/(volt/cm) << 
		   " V/cm to " << GetXYZDependentElectricField(G4ThreeVector(0., 0., 19.7))/(volt/cm) << G4endl;
           G4cout<<"-----------------------------------------------------------"<<G4endl;
           G4cout<<"-----------------------------------------------------------"<<G4endl;
        }
        else {
    	   G4cout << "Electric field T -> top PMTs: " << eFieldGasWindow/(volt/cm) << 
		   " V/cm" << G4endl;
	   G4cout << "Electric field A -> T: " << eFieldGasTop/(volt/cm) << 
		   " V/cm" << G4endl;
	   G4cout << "Electric field liquid surface -> A: " << eFieldGasAnode/(volt/cm) << 
		   " V/cm" << G4endl;
	   G4cout << "Electric field G -> liquid surface: " << eFieldLiqSurface/(volt/cm) << 
		   " V/cm" << G4endl;
	   G4cout << "Electric field C -> G: " << eFieldLiqGate/(volt/cm) << 
		   " V/cm" << G4endl;
	   G4cout << "Electric field B -> C: " << eFieldLiqCathode/(volt/cm) << 
		   " V/cm" << G4endl;
	   G4cout << "Electric field bottom PMTs -> B: " << eFieldLiqBottom/(volt/cm) << 
		   " V/cm" << G4endl;
        }

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					RecalculateFields()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::RecalculateFields()
{
	//	Reference for calculations:
	//	LUXcode/Trunk/PhysicsTools/MatlabPhysicsTools/Misc/CalculateFields_Generic.m
	
	/* ******************************************************************* */
	/* *** PROPERTIES COPIED FROM G4S1Light CLASS AND MATLAB GRID CODE *** */
	/* ******************************************************************* */
	/*
	G4double topPMTZ = 571.526*mm; //top Cu block (also, quartz window)
	G4double topGridZ = 563.77*mm; //top grid wires
	G4double anodeGridZ = 525.4*mm; //anode mesh
	G4double liquidSurfaceZ = 517.1*mm; //liquid-gas border z-coordinate
	G4double gateGridZ = 517*mm; //gate grid
	G4double cathodeGridZ = 47.656*mm; //cathode grid
	G4double bottomGridZ = 6.451*mm; //bottom PMT grid
	G4double bottomPMTZ = -2.5*mm; //bottom Cu block and PMTs
*/
	
	G4double topPMTZ = LUXSimPhysicsOptical->GetScintillation()->WIN;
            //top Cu block (also, quartz window)
	G4double topGridZ = LUXSimPhysicsOptical->GetScintillation()->TOP;
            //top grid wires
	G4double anodeGridZ = LUXSimPhysicsOptical->GetScintillation()->ANE;
            //anode mesh
	G4double liquidSurfaceZ = LUXSimPhysicsOptical->GetScintillation()->SRF;
            //liquid-gas border z-coordinate
	G4double gateGridZ = LUXSimPhysicsOptical->GetScintillation()->GAT;
            //gate grid
	G4double cathodeGridZ = LUXSimPhysicsOptical->GetScintillation()->CTH;
            //cathode grid
	G4double bottomGridZ = LUXSimPhysicsOptical->GetScintillation()->BOT;
            //bottom PMT grid
	G4double bottomPMTZ = LUXSimPhysicsOptical->GetScintillation()->PMT;
            //bottom Cu block and PMTs

	G4double epsilonRatio = 1.96/1.00126;
	/* ******************************************************************* */
	
	//	Crap that has to be declared outside of switch statement
	//	Case 1
	G4double totalVoltage, d, l, liquidSurfaceVoltage;
	//	Case 2
	
	switch (2) {
		case 1:	//	Dumb calculation, assuming grids are infinite plates
			//	Calculate fields for regions not bordering liquid surface
			eFieldLiqBottom = 
				-(bottomGridVoltage - bottomPMTBankVoltage) / (bottomGridZ - bottomPMTZ);
			eFieldLiqCathode = 
				-(cathodeGridVoltage - bottomGridVoltage) / (cathodeGridZ - bottomGridZ);
			eFieldLiqGate = 
				-(gateGridVoltage - cathodeGridVoltage) / (gateGridZ - cathodeGridZ);
			eFieldGasTop = 
				-(topGridVoltage - anodeGridVoltage) / (topGridZ - anodeGridZ);
			eFieldGasWindow = 
				-(topPMTBankVoltage - topGridVoltage) / (topPMTZ - topGridZ);
			
			//	Calculate fields for regions bordering liquid surface (if !gas)
			if ( GetGasRun() ) {
				eFieldLiqSurface = eFieldGasAnode = 
					-(anodeGridVoltage - gateGridVoltage) / (anodeGridZ - gateGridZ);
			} else {
				totalVoltage = anodeGridVoltage - gateGridVoltage;
				d = anodeGridZ - gateGridZ;
				l = liquidSurfaceZ - gateGridZ;
				liquidSurfaceVoltage = 
					(-l*gateGridVoltage+d*gateGridVoltage + 
						l*(1/epsilonRatio)*anodeGridVoltage) / 
					(d+l*(1/epsilonRatio-1));
				eFieldLiqSurface = 
					 -(liquidSurfaceVoltage - gateGridVoltage) / (liquidSurfaceZ - gateGridZ);
				eFieldGasAnode = 
					-(anodeGridVoltage - liquidSurfaceVoltage) / (anodeGridZ - liquidSurfaceZ);
			}
			
			break;
			
			
		case 2:

			//	These calculations come from a Matlab script written by Eric
			//	Dahl. By pure coincidence, the mathematics behind the script
			//	is from Eric's dissertation (Appendix A).
			
			G4double Varr[] = { bottomPMTBankVoltage/volt,
							   bottomGridVoltage/volt,
							   cathodeGridVoltage/volt,
							   gateGridVoltage/volt,
							   anodeGridVoltage/volt,
							   topGridVoltage/volt,
							   topPMTBankVoltage/volt };

			//G4double liquid_level = 0.57;
			G4double liquid_level = (liquidSurfaceZ - gateGridZ)/(anodeGridZ - gateGridZ);
			G4int liquid_area = 4;
			if( GetGasRun() )
				liquid_area = 0;
			//	The values for the aarr, rarr, and darr arrays come from
			//	LUXSim1_0Detector.cc, and match up as of 15 July 2013. Units are
			//	in cm. (Note that the darr array, which is the distance between
			//	the grid planes, do not match was is in case 1: above.
			G4double aarr[] = { 0, 1.0, 0.5, 0.5, 0.025, 0.5, 0 };
			G4double rarr[] = { 2, 0.0206, 0.0206, 0.01016, 0.00284, 0.00508, 2};
			G4double darr[] = { 1.97, 3.63, 48.54, 1., 3.63, 1. };
			
			G4double e_ratio = 1.96/1.00126;
			
			G4int num_grids = sizeof(Varr)/sizeof(G4double);
			for( G4int i=0; i<(G4int)(sizeof(rarr)/sizeof(G4double)); i++ )
				rarr[i] *= 0.5;
			
			HepVector V(num_grids), a(num_grids), r(num_grids), d(num_grids+1);
			for( G4int i=0; i<num_grids; i++ ) {
				V(i+1) = Varr[i];
				a(i+1) = aarr[i];
				r(i+1) = rarr[i];
				d(i+2) = darr[i];
			}
			d(1) = d(8) = 1e300;	//	1e300 ~ infinity
			
			if( liquid_area > num_grids )
				liquid_area = 0;
			
			//
			//	Setup variables for convenience, and to match the Matlab script
			//
			G4double x = liquid_level;
			G4double er = e_ratio;
			
			G4double *Xarr;
			Xarr = new G4double[num_grids];
			HepVector X(num_grids);
			for( G4int i=0; i<num_grids; i++ ) {
				if( aarr[i] )
					Xarr[i] = (aarr[i]/twopi) * log(aarr[i]/(twopi*rarr[i]));
				else
					Xarr[i] = 0;
				
				X(i+1) = Xarr[i];
			}
			
			HepVector s(num_grids+1);
			for( G4int i=0; i<num_grids; i++ )
				s(i+1) = V(i+1);
			s(8) = 0;
	
			//
			//	Compute the coefficient matrix
			//

			//	coeff is the matrix such that s = coeff * V_sheet, where V_sheet
			//	is a column vector of the equivalent sheet voltages of the
			//	grids,  with the voltage at the liquid level as the last
			//	element.  We invert this matrix to solve for V_sheet and
			//	determine the drift fields.

			//	To compute coeff, we use the equation
			//	V = V_sheet + X*delta_E, where X is defined above, and delta_E
			//	is the change in drift field across the grid.  Writing delta_E
			//	in terms of other sheet voltages will give one row of the coeff
			//	matrix.  When considering dual phases, the voltage at the liquid
			//	level is considered a sheet voltage, and is calculated from the
			//	sheet voltages above and below.  This gives the last row in the
			//	coeff matrix.
			
			HepMatrix coeff(num_grids+1,num_grids+1,0);
			
			//	compute row for liquid level
			coeff(num_grids+1,num_grids+1) = 1.;
			if( liquid_area > 0 ) {
				coeff(num_grids+1,liquid_area) =
						-er*((1.-x)/x) / (1.+er*((1.-x)/x));
				coeff(num_grids+1,liquid_area+1) =
						-1. / (1.+er*((1.-x)/x));
			} else
				liquid_area = -1.;

			//	compute rows for grids bordering the phase boundary region
			if( liquid_area > 0 ) {
				//	compute row for grid below the phase boundary
				G4int ng = liquid_area;
				coeff(ng,ng) = 1. + X(ng) * ((1./d(ng)) + (1./(d(ng+1)*x)));
				coeff(ng,num_grids+1) = -X(ng)/(d(ng+1)*x);
				if( ng>1 )
					coeff(ng,ng-1) = -X(ng)/d(ng);
				
				//	compute row for grid above the phase boundary
				ng = liquid_area+1;
				coeff(ng,ng) =
						1. + X(ng) * ((1./(d(ng)*(1.-x))) + (1./d(ng+1)));
				if( ng<num_grids )
					coeff(ng,ng+1) = -X(ng)/d(ng+1);

				coeff(ng,num_grids+1) = -X(ng)/(d(ng)*(1.-x));
			}

			//	compute rows for grids not bordering the phase boundary region
			for( G4int ng=1; ng<=liquid_area-1; ng++ ) {
				coeff(ng,ng) = 1. + X(ng) * ((1./d(ng)) + (1./d(ng+1)));
				if( ng<num_grids )
					coeff(ng,ng+1) = -X(ng)/d(ng+1);
				if( ng>1 )
					coeff(ng,ng-1) = -X(ng)/d(ng);
			}
			for( G4int ng=liquid_area+2; ng<=num_grids; ng++ ) {
				coeff(ng,ng) = 1. + X(ng) * ((1./d(ng)) + (1./d(ng+1)));
				if( ng<num_grids )
					coeff(ng,ng+1) = -X(ng)/d(ng+1);
				if( ng>1 )
					coeff(ng,ng-1) = -X(ng)/d(ng);
			}
			
			//
			//	calculate v_coeffs and fields
			//
			G4int inv;
			HepMatrix v_coeffs = coeff.inverse(inv);
			HepVector Vs = v_coeffs * s;

			HepVector fields(num_grids,0);
			if( liquid_area > 0 ) {
				//	fields in dual phase detector
				for( G4int nd=1; nd<=liquid_area-1; nd++ )
					fields(nd) = -(Vs(nd+1)-Vs(nd)) / d(nd+1);

				for( G4int nd=liquid_area+2; nd<=num_grids; nd++ )
					fields(nd) = -(Vs(nd)-Vs(nd-1)) / d(nd);
				
				//	field immediately below liquid level
				G4int nd = liquid_area;
				fields(nd) = -(Vs(num_grids+1) - Vs(nd)) / (d(nd+1)*x);

				//	field imediately above liquid level
				nd = liquid_area+1;
				fields(nd) = -(Vs(nd) - Vs(num_grids+1)) / (d(nd)*(1-x));
			} else {
				//	fields in single phase detector
				fields = HepVector(num_grids-1,0);
					for( G4int nd=1; nd<=num_grids-1; nd++ )
						fields(nd) = -(Vs(nd+1)-Vs(nd)) / d(nd+1);
			}
			
			//
			//	Move the calculated fields over to the appropriate variables
			//
			eFieldLiqBottom = fields(1)*volt/cm;
			eFieldLiqCathode = fields(2)*volt/cm;
			eFieldLiqGate = fields(3)*volt/cm;
			eFieldGasAnode = fields(5)*volt/cm;
			if( GetGasRun() )
				eFieldLiqSurface = eFieldGasAnode;
			else
				eFieldLiqSurface = fields(4)*volt/cm;
			eFieldGasTop = fields(6)*volt/cm;
			eFieldGasWindow = fields(7)*volt/cm;
			
			//
			//	Cleanup
			//
			delete [] Xarr;
			
			break;
	}
	
	/* *** deprecated commands for globally setting fields in liquid and gas*** */
	eFieldGas = 0.;
	eFieldLiq = 0.;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetSource()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetSource( G4String spec )
{
    //	First, tokenize the spec string to extract volume, source type, and
    //	activity (and optional arguemnts)
    //	input="volume sourcename activity act.Units" 
    //	  plus optionally ([age units] or [energy]) for decaychain and single
    //	  particle generators.
    G4String origSpec = spec;
    G4String arg1;
    G4String volName, sourceTypeName;
    G4String activityUnit;
    G4double activity;
    G4double posX=0., posY=0., posZ=0.;
    G4ThreeVector posSource = G4ThreeVector(0,0,0);
    G4String volMassUnit = "0";  //optional acitivity unit
    G4String sourceAgeString = "0";//decay chain
    G4String sourceAgeUnit = "s";  //decay chain
    G4String singleParticleName="";//single particle
    G4double singleParticleEnergy = 0; //single particle
    G4String singleParticleEnergyUnit = "0";//single particle
    gammaXFiducialR = 20.1; //fiducial volume radius used by gammaX generator
    gammaXFiducialLoZ = 10; //fiducial volume lower Z bound used by gammaX generator
    gammaXFiducialHiZ = 50; //fiducial volume upper Z bound used by gammaX generator
    gammaXReverseFieldR = 23.5; //reverse field region radius used by gammaX generator
    gammaXReverseFieldLoZ = 1; //reverse field lower Z used by gammaX generator
    gammaXReverseFieldHiZ = 5; //reverse field upper Z used by gammaX generator
    G4String gammaXLengthUnit = "cm"; // length units used for preceeding six variables
    gammaXMaxEnergy = 10*keV; // gammaX scatters are assumed to have energies between 0 and gammaXMaxEnergy
    G4String gammaXEnergyUnit = "keV"; // energy units for preceeding variable
    G4String eventsFile;
    twoElectronsDeltaZMax = 1.;
    G4String twoElectronsLengthUnit = "cm"; // length units used for preceeding six variables
    twoElectronsEmin = 0.;
    twoElectronsEmax = 100.;
    G4String twoElectronsEnergyUnit = "keV"; // energy units for preceeding variable


  	//G4double wimpMass = 0; //Wimp
  	//G4String wimpMassUnit = "0"; //Wimp
    stringstream sspec;
    sspec.str(spec);
    G4cout << "Setting Source = \"" << sspec.str() << "\"" << G4endl;

    //    The source can be set by specifying a position (x,y,z). First 
    //    check to see if the first argument is a LUSim source name 
    //    or a volume name..
    sspec >> arg1;
    G4bool posDefined = false;
    if (arg1.find("SingleParticle") < G4String::npos || arg1.find("DecayChain") < G4String::npos 
	        || arg1.find("SingleDecay") < G4String::npos){
      sourceTypeName = arg1;
      sspec >> posX >> posY >> posZ >> activity >> activityUnit;
      volName = "universe";
      posSource = G4ThreeVector(posX, posY, posZ);
      posDefined = true;
    }
    if(!posDefined){
        for( G4int i=0; i<LUXSimSourceCat->GetNumSourceTypes(); i++ ){
	    if( LUXSimSourceCat->GetSourceType(i)->GetName() == arg1 ) {
	        i = LUXSimSourceCat->GetNumSourceTypes();
		sourceTypeName = arg1;
//		sspec >> posX >> posY >> posZ >> activity >> activityUnit;
	if (sourceTypeName != "MUSUN") // MUSUN is volume independent
		sspec >> posX >> posY >> posZ >> activity >> activityUnit;
		else { activity = 1; activityUnit = "Bq"; }

		volName = "universe";
		posSource = G4ThreeVector(posX, posY, posZ);
		posDefined = true;
	    }
	}
    }    
    //    The first argument is a volume name, not a source name.
    if(!posDefined){
      volName = arg1;
      sspec >> sourceTypeName >> activity >> activityUnit;
    }
    
    if( sourceTypeName.find("SingleParticle") < G4String::npos ) 
        sspec >> singleParticleEnergy >> singleParticleEnergyUnit;
    if( sourceTypeName.find("DecayChain") < G4String::npos ) 
        sspec >> sourceAgeString >> sourceAgeUnit;
	  if( sourceTypeName.find("Wimp") < G4String::npos )
        sspec >> singleParticleEnergy >> singleParticleEnergyUnit;
        //a trick 
		//sspec >> wimpMass >> wimpMassUnit;
    if( sourceTypeName.find("gamma-X") < G4String::npos  ) {
        sspec >> gammaXFiducialR >> gammaXFiducialLoZ >> gammaXFiducialHiZ;
        sspec >> gammaXReverseFieldR >> gammaXReverseFieldLoZ >> gammaXReverseFieldHiZ;
        sspec >> gammaXLengthUnit;
        sspec >> gammaXMaxEnergy >> gammaXEnergyUnit;
    }
    if( sourceTypeName.find("TwoElectrons") < G4String::npos  ) {
        sspec >> twoElectronsDeltaZMax >> twoElectronsLengthUnit;
        sspec >> twoElectronsEmin >> twoElectronsEmax >> twoElectronsEnergyUnit;
    }
    if( sourceTypeName.find("EventsFile") < G4String::npos  ) {
        sspec >> eventsFile;
        G4cout<<eventsFile.data()<<G4endl;
        LoadEventsFile(eventsFile);
    }
    //Error in sspec reading:
    if( sspec.rdstate()!=0 && sspec.fail()!=0 ) { 
        G4cout << "ERROR in SetSource(): " << G4endl; 
        G4cout << "Usage: /LUXSim/source/set VolName SourceName Activity "
               << "ActivityUnits " << G4endl;
        if( sourceTypeName.find("DecayChain")<G4String::npos)
            G4cout << "\tRequired for DecayChain: AgeOfSource AgeUnits" 
                   << G4endl;
        if( sourceTypeName.find("SingleParticle")<G4String::npos)
            G4cout << "\tRequired for SingleParticle: Energy EnergyUnits" 
                   << G4endl;
		if( sourceTypeName.find("Wimp")<G4String::npos)
			G4cout << "\tRequired for Wimp: Mass MassUnits" << G4endl;
        //G4cout << " contact Nick Walsh (niwalsh@ucdavis.edu)" << G4endl;
        exit(0);
    }
    G4cout << " Source parsed as: \"" ;
    G4cout << volName << " " << sourceTypeName << " " << activity << " "
           << activityUnit << " " ;
    if( sourceTypeName.find("SingleParticle") < G4String::npos ||
        sourceTypeName.find("Wimp") < G4String::npos  )
        G4cout << singleParticleEnergy << " " << singleParticleEnergyUnit;
    else if( sourceTypeName.find("DecayChain") < G4String::npos )
        G4cout << sourceAgeString << " " << sourceAgeUnit;
  	//else if( sourceTypeName.find("Wimp") < G4String::npos )
  	//	G4cout << wimpMass << " " << wimpMassUnit;
    G4cout << "\"" << G4endl;
    
    
    //  Check if ActivityUnit is specified per mass
	  spec = activityUnit;
    activityUnit = spec.substr(0,spec.find_first_of('/') );
    if( spec.length() == activityUnit.length() )
        G4cout << "  Total activity for entire mass is specified" << G4endl;
    else {
        spec = spec.substr(activityUnit.length()+1);
        volMassUnit = spec;
        G4cout << "  Activity per unit mass is specified: " << G4endl;
    }

    //  For SingleParticle find and check the specified particle name
    if (sourceTypeName.find("SingleParticle") < G4String::npos ) {
        G4String sourceTypeSingleParticle = sourceTypeName.substr(0,
                               sourceTypeName.find_first_of("_"));
        if( sourceTypeSingleParticle.length() == sourceTypeName.length() ) {
            G4cout << "No Particle Name given with SingleParticle_?: ERROR"
                   << G4endl;
            exit(0);
        }
        singleParticleName = sourceTypeName.substr(
                                sourceTypeSingleParticle.length()+1 );
        sourceTypeName = sourceTypeSingleParticle;
        // Now check against a manually identified list of /gps/particle options
        // excluding ion and genericIon
        G4String gpsmatch;
        G4String gpsParticleList1 = "alpha,anti_nu_e,anti_nu_mu,anti_nu_tau,"
            "chargedgeantino,deuteron,e+,e-,gamma,geantino,mu+,mu-,neutron,nu_e"
            ",nu_mu,nu_tau,opticalphoton,pi+,pi0,pi-,tau+,tau-,thermalelectron";
        stringstream gpsParticlesSS(gpsParticleList1);
        G4bool gpsIsGood = false;
        while ( getline( gpsParticlesSS, gpsmatch, ',') && !gpsIsGood ) {
            if( gpsmatch == singleParticleName )
                gpsIsGood = true;
        }
        if( !gpsIsGood ) {
            //try other matches (without exact match requirement)
            G4String gpsParticleList2 = "kaon,gluon,f0(,f1(,f2(,k0(,k1(,k2(,"
                "b1(,k_star,omega,rho,B+,B0,B-,D+,D0,D-,Ds+,Ds-,J/psi,He3,a0,N("
                ",a0(,a1(,a2(,quark,xi,eta,delta,lambda,sigma,triton";
            gpsParticlesSS.clear();
            gpsParticlesSS.str(gpsParticleList2);
            while ( getline( gpsParticlesSS, gpsmatch, ',') && !gpsIsGood ) {
                if( singleParticleName.find(gpsmatch) < G4String::npos )
                    gpsIsGood = true;
            }
        }
        if( !gpsIsGood ) {
            G4cout << "Error in SetSource: SingleParticle_particleName" 
                   << G4endl;
            G4cout << "\tparticleName: " << singleParticleName << " not found" 
                   << G4endl;
            G4cout << "\tuse names from /gps/List but NOT ion or Genericion"
                   << G4endl;
            exit(0);
        }
    }


    // If DecayChain find the specified isotope chain
    G4String parentIsotope = "0";
    if( sourceTypeName.find("DecayChain") < G4String::npos) {
        G4String sourceTypeDecayChain = sourceTypeName.substr(
                                0, sourceTypeName.find_first_of("_"));
        if( sourceTypeDecayChain.length() == sourceTypeName.length() ) {
            G4cout << "No Parent Isotope specified for DecayChain_?: ERROR1" 
                   << G4endl;
            exit(0);
        } 
        parentIsotope = sourceTypeName.substr(sourceTypeDecayChain.length()+1);
        sourceTypeName = sourceTypeDecayChain;
        hasDecayChainSources=true;
    }    

    // If SingleDecay or G4Decay find the specified A and Z of nucleus to decay
    G4int aa = 0;
    G4int zz = 0;
    if( sourceTypeName.find("SingleDecay") < G4String::npos || 
	    sourceTypeName.find("G4Decay") < G4String::npos ) {
        if( sourceTypeName.find("G4Decay") < G4String::npos ) SetG4DecayBool(true);
        G4String sourceTypeSingleDecay = sourceTypeName.substr(
                         0, sourceTypeName.find_first_of("_") );
        if( sourceTypeSingleDecay.length() == sourceTypeName.length() ) {
            G4cout << "No infomation for A and Z of SingleDecay_A_Z: ERROR!" 
                   << G4endl;
            exit(0);
        }
        sourceTypeName = sourceTypeName.substr( 
                         sourceTypeSingleDecay.length()+1 );
        G4String aaStr = sourceTypeName.substr( 
                         0, sourceTypeName.find_first_of("_") );
        G4String zzStr = sourceTypeName.substr( aaStr.length()+1 );
        if ( zzStr.length() > 0 ) 
            zzStr = sourceTypeName.substr( aaStr.length()+1 );
        else 
            G4cout << "error in assigning SingleDecay or G4Decay, check syntax" << G4endl;
        sourceTypeName = sourceTypeSingleDecay;
        aa = atoi( aaStr.c_str() ) ; 
        zz = atoi( zzStr.c_str() ) ;  
    }

  	//	Check to see that the source type is known
  	G4bool foundSource = false;
  	LUXSimSource *currentSource = 0;
  	for( G4int i=0; i<LUXSimSourceCat->GetNumSourceTypes(); i++ )
  		if( LUXSimSourceCat->GetSourceType(i)->GetName() == sourceTypeName ) {
  			currentSource = LUXSimSourceCat->GetSourceType(i);
  			foundSource = true;
  			i = LUXSimSourceCat->GetNumSourceTypes();
  		}
  	if( !foundSource ) {
  		G4cout << G4endl << G4endl
  			   << "Cannot find source type \"" << sourceTypeName << "\"!"
  			   << G4endl;
  		exit(0);
  	}
    
	  //	Verify that the activity-per-mass mass unit is understood
    vector<G4String> massUnits;
    massUnits.push_back("0");
    massUnits.push_back("g");
    massUnits.push_back("kg");
    massUnits.push_back("mg");
    massUnits.push_back("ug");
    G4bool goodSourceUnit = false;
    for (G4int i=0; i<(G4int)massUnits.size(); i++ )
        //if( volMassUnit.find(massUnits[i]) < G4String::npos )
        if( volMassUnit == massUnits[i] )
            goodSourceUnit = true;
    if( !goodSourceUnit ) {
        G4cout << G4endl << G4endl
               << "Unknown source strength mass unit \"" << volMassUnit 
               << "\"!" << G4endl;
        exit(0);
    }    
    // normalize activity-per-mass mass units
  	if( volMassUnit.find("m") < G4String::npos )
  		activity *= 1e+3;
  	if( volMassUnit.find("u") < G4String::npos )
  		activity *= 1e+6;
  	if( volMassUnit.find("k") < G4String::npos )
  		activity *= 1e-3;
    	
    //  Verify the singleparticle energy is understood
    vector<G4String> eunits;
    eunits.push_back("0"); //for non-singleparticle
    eunits.push_back("eV"); eunits.push_back("*eV");
    eunits.push_back("keV"); eunits.push_back("*keV");
    eunits.push_back("MeV"); eunits.push_back("*MeV");
    eunits.push_back("GeV"); eunits.push_back("*GeV");
    G4bool goodEnergyUnit = true;
    for( G4int i=0; i<(G4int)eunits.size(); i++)
        //if( singleParticleEnergyUnit.find(eunits[i]) < G4String::npos )
        if( singleParticleEnergyUnit == eunits[i] )
            goodEnergyUnit = true;
    if( !goodEnergyUnit) {
        G4cout << G4endl << G4endl;
        G4cout << "Unknown single particle energy unit \"" 
               << singleParticleEnergyUnit
               << "\n Proper Usage for example is 4*keV" << G4endl;
        exit(0);
    }	
    //normalize the energy to keV (from eV, MeV, or GeV)
    if( singleParticleEnergyUnit.find("0") < G4String::npos ) {;}
    else if( singleParticleEnergyUnit.find("keV") < G4String::npos ) {
        singleParticleEnergy = singleParticleEnergy * keV ;
        G4cout << "  SingleParticle energy in keV = " 
               << singleParticleEnergy/keV << G4endl;
    } else if( singleParticleEnergyUnit.find("MeV") < G4String::npos ) {
        singleParticleEnergy = singleParticleEnergy * 1.e3 * keV ; 
        G4cout << "  SingleParticle energy in 'keV' = " 
               << singleParticleEnergy/keV << "*keV " << G4endl;
    } else if( singleParticleEnergyUnit.find("GeV") < G4String::npos ) {
        singleParticleEnergy = singleParticleEnergy * 1.e6 * keV ; 
        G4cout << "  SingleParticle energy in 'keV' = " 
               << singleParticleEnergy/keV << "*keV " << G4endl;
    } else if( singleParticleEnergyUnit=="eV" || 
               singleParticleEnergyUnit=="*eV"      ) {
        singleParticleEnergy = singleParticleEnergy * 1.e-3 * keV; 
        G4cout << "  SingleParticle energy in 'keV' = " 
               << singleParticleEnergy/keV << "*keV " << G4endl;
    } else { 
        G4cout << "No energy scaling found for Single Particle " << G4endl;
        G4cout << " Usage: *eV *keV *MeV or *GeV " << G4endl;
        exit(0);
    }

    //  Verify the gammaX energy is understood
    goodEnergyUnit = false;
    for( G4int i=0; i<(G4int)eunits.size(); i++)
        if( gammaXEnergyUnit == eunits[i] )
            goodEnergyUnit = true;
    if( !goodEnergyUnit) {
        G4cout << G4endl << G4endl;
        G4cout << "Unknown GammaX energy unit \"" 
               << gammaXEnergyUnit
               << "\n Proper Usage for example is 4 keV" << G4endl;
        exit(0);
    }	
    //normalize the energy to keV (from eV, MeV, or GeV)
    if( gammaXEnergyUnit.find("0") < G4String::npos ) {;}
    else if( gammaXEnergyUnit.find("keV") < G4String::npos ) {
        gammaXMaxEnergy = gammaXMaxEnergy * keV ;
        G4cout << "  GammaX energy in keV = " 
               << gammaXMaxEnergy/keV << G4endl;
    } else if( gammaXEnergyUnit.find("MeV") < G4String::npos ) {
        gammaXMaxEnergy = gammaXMaxEnergy * 1e3 * keV ; 
        G4cout << "  GammaX energy in 'keV' = " 
               << gammaXMaxEnergy/keV << "*keV " << G4endl;
    } else if( gammaXEnergyUnit.find("GeV") < G4String::npos ) {
        gammaXMaxEnergy = gammaXMaxEnergy * 1e6 * keV ; 
        G4cout << "  GammaX energy in 'keV' = " 
               << gammaXMaxEnergy/keV << "*keV " << G4endl;
    } else if( gammaXEnergyUnit=="eV" || 
               gammaXEnergyUnit=="*eV"      ) {
        gammaXMaxEnergy = gammaXMaxEnergy * 1e-3 * keV; 
        G4cout << "  GammaX energy in 'keV' = " 
               << gammaXMaxEnergy/keV << "*keV " << G4endl;
    } else { 
        G4cout << "No energy scaling found for GammaX " << G4endl;
        G4cout << " Usage: eV keV MeV or GeV " << G4endl;
        exit(0);
    }

    //  Verify the gammaX positions are understood
    vector<G4String> lunits;
    lunits.push_back("0"); //for non-singleparticle
    lunits.push_back("um"); eunits.push_back("*um");
    lunits.push_back("mm"); eunits.push_back("*mm");
    lunits.push_back("cm"); eunits.push_back("*cm");
    lunits.push_back("m"); eunits.push_back("*m");
    G4bool goodLengthUnit = false;
    for( G4int i=0; i<(G4int)lunits.size(); i++)
        if( gammaXLengthUnit == lunits[i] )
            goodLengthUnit = true;
    if( !goodLengthUnit) {
        G4cout << G4endl << G4endl;
        G4cout << "Unknown GammaX length unit \"" 
               << gammaXLengthUnit
               << "\n Proper Usage for example is 4 cm" << G4endl;
        exit(0);
    }	
    //normalize the energy to keV (from eV, MeV, or GeV)
    if( gammaXLengthUnit.find("0") < G4String::npos ) {;}
    else if( gammaXLengthUnit.find("mm") < G4String::npos ) {
        gammaXFiducialR = gammaXFiducialR * mm ;
        gammaXFiducialLoZ = gammaXFiducialLoZ * mm ;
        gammaXFiducialHiZ = gammaXFiducialHiZ * mm ;
        gammaXReverseFieldR = gammaXReverseFieldR * mm ;
        gammaXReverseFieldLoZ = gammaXReverseFieldLoZ * mm ;
        gammaXReverseFieldHiZ = gammaXReverseFieldHiZ * mm ;
    } else if( gammaXLengthUnit.find("cm") < G4String::npos ) {
        gammaXFiducialR = gammaXFiducialR * cm ;
        gammaXFiducialLoZ = gammaXFiducialLoZ * cm ;
        gammaXFiducialHiZ = gammaXFiducialHiZ * cm ;
        gammaXReverseFieldR = gammaXReverseFieldR * cm ;
        gammaXReverseFieldLoZ = gammaXReverseFieldLoZ * cm ;
        gammaXReverseFieldHiZ = gammaXReverseFieldHiZ * cm ;
    } else if( gammaXLengthUnit.find("m") < G4String::npos ) {
        gammaXFiducialR = gammaXFiducialR * 1e-2 * cm ;
        gammaXFiducialLoZ = gammaXFiducialLoZ * 1e2 * m ;
        gammaXFiducialHiZ = gammaXFiducialHiZ * 1e2 * m ;
        gammaXReverseFieldR = gammaXReverseFieldR * 1e2 * m ;
        gammaXReverseFieldLoZ = gammaXReverseFieldLoZ * 1e2 * m ;
        gammaXReverseFieldHiZ = gammaXReverseFieldHiZ * 1e2 * m ;
    } else if( gammaXLengthUnit=="um" || 
               gammaXLengthUnit=="*um"      ) {
        gammaXFiducialR = gammaXFiducialR * 1e-4 * um ;
        gammaXFiducialLoZ = gammaXFiducialLoZ * 1e-4 * um ;
        gammaXFiducialHiZ = gammaXFiducialHiZ * 1e-4 * um ;
        gammaXReverseFieldR = gammaXReverseFieldR * 1e-4 * um ;
        gammaXReverseFieldLoZ = gammaXReverseFieldLoZ * 1e-4 * um ;
        gammaXReverseFieldHiZ = gammaXReverseFieldHiZ * 1e-4 * um ;
    } else { 
        G4cout << "No length scaling found for GammaX " << G4endl;
        G4cout << " Usage: um mm cm or m " << G4endl;
        exit(0);
    }

    //  Verify the two electron energy is understood
    goodEnergyUnit = false;
    for( G4int i=0; i<(G4int)eunits.size(); i++)
        if( twoElectronsEnergyUnit == eunits[i] )
            goodEnergyUnit = true;
    if( !goodEnergyUnit) {
        G4cout << G4endl << G4endl;
        G4cout << "Unknown Two Electrons energy unit \"" 
               << twoElectronsEnergyUnit
               << "\n Proper Usage for example is 4 keV" << G4endl;
        exit(0);
    }	
    //normalize the energy to keV (from eV, MeV, or GeV)
    if( twoElectronsEnergyUnit.find("0") < G4String::npos ) {;}
    else if( twoElectronsEnergyUnit.find("keV") < G4String::npos ) {
        twoElectronsEmin = twoElectronsEmin * keV ;
        twoElectronsEmax = twoElectronsEmax * keV ;
        G4cout << "  Two Electrons energy in keV = " 
               << twoElectronsEmax/keV << G4endl;
    } else if( twoElectronsEnergyUnit.find("MeV") < G4String::npos ) {
        twoElectronsEmin = twoElectronsEmin * 1e3 * keV ;
        twoElectronsEmax = twoElectronsEmax * 1e3 * keV ;
        G4cout << "  Two Electrons energy in 'keV' = " 
               << twoElectronsEmax/keV << "*keV " << G4endl;
    } else if( twoElectronsEnergyUnit.find("GeV") < G4String::npos ) {
        twoElectronsEmin = twoElectronsEmin * 1e6 * keV ;
        twoElectronsEmax = twoElectronsEmax * 1e6 * keV ;
        G4cout << "  Two Electrons energy in 'keV' = " 
               << twoElectronsEmax/keV << "*keV " << G4endl;
    } else if( twoElectronsEnergyUnit=="eV" || 
               twoElectronsEnergyUnit=="*eV"      ) {
        twoElectronsEmin = twoElectronsEmin * 1e-3 * keV ;
        twoElectronsEmax = twoElectronsEmax * 1e-3 * keV ;
        G4cout << "  Two Electrons energy in 'keV' = " 
               << twoElectronsEmax/keV << "*keV " << G4endl;
    } else { 
        G4cout << "No energy scaling found for Two Electrons " << G4endl;
        G4cout << " Usage: eV keV MeV or GeV " << G4endl;
        exit(0);
    }

    //  Verify the twoElectrons positions are understood
    goodLengthUnit = false;
    for( G4int i=0; i<(G4int)lunits.size(); i++)
        if( twoElectronsLengthUnit == lunits[i] )
            goodLengthUnit = true;
    if( !goodLengthUnit) {
        G4cout << G4endl << G4endl;
        G4cout << "Unknown Two Electrons length unit \"" 
               << twoElectronsLengthUnit
               << "\n Proper Usage for example is 4 cm" << G4endl;
        exit(0);
    }	
    //normalize the energy to keV (from eV, MeV, or GeV)
    if( twoElectronsLengthUnit.find("0") < G4String::npos ) {;}
    else if( twoElectronsLengthUnit.find("mm") < G4String::npos ) {
        twoElectronsDeltaZMax = twoElectronsDeltaZMax * mm;
    } else if( gammaXLengthUnit.find("cm") < G4String::npos ) {
        twoElectronsDeltaZMax = twoElectronsDeltaZMax * cm;
    } else if( gammaXLengthUnit.find("m") < G4String::npos ) {
        twoElectronsDeltaZMax = twoElectronsDeltaZMax * m;
    } else if( gammaXLengthUnit=="um" || 
               gammaXLengthUnit=="*um"      ) {
        twoElectronsDeltaZMax = twoElectronsDeltaZMax * um;
    } else { 
        G4cout << "No length scaling found for Two Electrons " << G4endl;
        G4cout << " Usage: um mm cm or m " << G4endl;
        exit(0);
    }

  	// Verify that the wimp mass is understood and normalize to GeV
  	//if (wimpMassUnit.find("0") < G4String::npos ) {;}
  	//else if (wimpMassUnit.find("keV") < G4String::npos ||
  	//	wimpMassUnit.find("*keV") < G4String::npos){
  	//	wimpMass = wimpMass * 1.e-6;
  	//}
  	//else if (wimpMassUnit.find("MeV") < G4String::npos ||
  	//	wimpMassUnit.find("*MeV") < G4String::npos){
  	//	wimpMass = wimpMass * 1.e-3;
  	//}
  	//else if (wimpMassUnit.find("GeV") < G4String::npos ||
  	//	wimpMassUnit.find("*GeV") < G4String::npos){
  	//	;//No conversion necessary
  	//}
  	//else if (wimpMassUnit.find("eV") < G4String::npos ||
  	//	wimpMassUnit.find("*eV") < G4String::npos){
  	//	wimpMass = wimpMass * 1.e-9;
  	//}
  	//else {
  	//	G4cout << "No or unknown mass unit for Wimp" << G4endl;
  	//	G4cout << "Usage: *eV *keV *MeV *GeV " << G4endl;
  	//	exit(0);
  	//}


  	//	Verify that the source strength unit is understood
  	vector<G4String> units;
  	units.push_back("Ci");
  	units.push_back("mCi");
  	units.push_back("uCi");
  	units.push_back("nCi");
  	units.push_back("kBq");
  	units.push_back("Bq");
  	units.push_back("mBq");
  	units.push_back("uBq");
  	units.push_back("nBq");
  	goodSourceUnit = false;
  	for( G4int i=0; i<(G4int)units.size(); i++ )
  		if( activityUnit == units[i] )
  			goodSourceUnit = true;
  	if( !goodSourceUnit ) {
  		G4cout << G4endl << G4endl
  			   << "Unknown source strength unit \"" << activityUnit << "\"!"
  			   << G4endl;
  		exit(0);
  	}
  	//	Next, normalize the source activity
  	//G4double activity = atof( activityString.c_str() );
  	if( activityUnit.find("Ci") < G4String::npos )
  		activity *= 3.7e10;
  	if( activityUnit.find("m") < G4String::npos )
  		activity *= 1e-3;
  	if( activityUnit.find("u") < G4String::npos )
  		activity *= 1e-6;
  	if( activityUnit.find("n") < G4String::npos )
  		activity *= 1e-9;
  	if( activityUnit.find("k") < G4String::npos )
  		activity *= 1e3;

    //  Source Age checks
    vector<G4String> timeUnits;
    timeUnits.push_back("s");
    timeUnits.push_back("Ms");
    timeUnits.push_back("Gs");
    timeUnits.push_back("h");
    timeUnits.push_back("d");
    timeUnits.push_back("y");
    timeUnits.push_back("yr");
    timeUnits.push_back("ky");
    timeUnits.push_back("kyr");
    timeUnits.push_back("Myr");
    timeUnits.push_back("My");
    timeUnits.push_back("Gyr");
    timeUnits.push_back("Gy");
    goodSourceUnit = false;
    for( G4int i=0; i<(G4int)timeUnits.size(); i++ )
        if( sourceAgeUnit == timeUnits[i] )
            goodSourceUnit = true;
    if( !goodSourceUnit ) {
        G4cout << G4endl << G4endl << "Unknown source age units \""
               << sourceAgeUnit << "\"!" << G4endl;
        exit(0);
    }
    //  normalize source age to seconds
    G4double sourceAge = atof( sourceAgeString.c_str() );
    if( sourceAgeUnit.find("k") < G4String::npos )
        sourceAge *= 1e+3 ;
    if( sourceAgeUnit.find("M") < G4String::npos )
        sourceAge *= 1e+6 ;
    if( sourceAgeUnit.find("G") < G4String::npos )
        sourceAge *= 1e+9 ;
    if( sourceAgeUnit.find("d") < G4String::npos )
        sourceAge *= 3600*24 ;
    if( sourceAgeUnit.find("h") < G4String::npos )
        sourceAge *= 3600 ;
    if( sourceAgeUnit.find("y") < G4String::npos )
        sourceAge *= 3600*24*365.25 ;


    G4bool volumeFound = false;
    // the following variable is identical to activity, 
    // but allows for the case of multiply named
    // volumes set with activity-per-mass to be scale
    G4double activityToSet = activity ;
		for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ ) {
		    if( luxSimComponents[i]->GetName().find(volName) < G4String::npos ) {
            if( volMassUnit != "0" ){
                G4double volMass = luxSimComponents[i]->GetMass();
                activityToSet = activity * volMass ;
                G4cout << "   " << luxSimComponents[i]->GetName() 
                       << " Mass = " << volMass << " g yields Total Activity of " 
                       << activityToSet << " Bq" << G4endl;
                //G4cout << "mass: " << volMass ; 
            }

            luxSimComponents[i]->AddSource( currentSource, activityToSet, 
                aa, zz, parentIsotope, sourceAge, 
		singleParticleName, singleParticleEnergy, posDefined, posSource);//, wimpMass);
            volumeFound = true;
        }
    }
    if( !volumeFound ) {
        G4cout << "Error: Did not recognize the volume in which you wanted"
               << " to set the source, \"" << volName << "\"" << G4endl;
        G4cout << "Usage: /LUXSim/source/set volName sourceName activity"
               << " activityUnits" << G4endl;
        exit(0);
    }
    G4cout << G4endl;

    hasLUXSimSources = true;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					ResetSources()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::ResetSources()
{
    for( G4int i=0; i<(G4int)sourceByVolume.size(); i++ )
      sourceByVolume[i].component->ResetSources();
    sourceByVolume.clear();
    totalSimulationActivity=0;
    hasLUXSimSources=false;
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetUseOpticalProcesses()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetUseOpticalProcesses( G4bool use )
{
	useOpticalProcesses = use;
    
	//if( useOpticalProcesses )
        //LUXSimPhysics->SetCutsShort();
	//else
        LUXSimPhysics->SetCutsLong();
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLXeTeflonRefl()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetLXeTeflonRefl( G4double r )
{
	LUXSimDetector->SetLXeTeflonRefl(r);
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLXeSteelRefl()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetLXeSteelRefl( G4double r )
{
	LUXSimDetector->SetLXeSteelRefl(r);
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLXeAbs()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetLXeAbs( G4double l )
{
	LUXSimDetector->SetLXeAbs(l);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetLXeRayleigh()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetLXeRayleigh( G4double l )
{
	LUXSimDetector->SetLXeRayleigh(l);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetGXeTeflonRefl()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetGXeTeflonRefl( G4double r )
{
	LUXSimDetector->SetGXeTeflonRefl(r);
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetGXeSteelRefl()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetGXeSteelRefl( G4double r )
{
	LUXSimDetector->SetGXeSteelRefl(r);
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetGXeAbs()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetGXeAbs( G4double l )
{
	LUXSimDetector->SetGXeAbs(l);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetGXeRayleigh()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetGXeRayleigh( G4double l )
{
	LUXSimDetector->SetGXeRayleigh(l);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetAlUnoxQuartzRefl()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetAlUnoxQuartzRefl( G4double r )
{
	LUXSimDetector->SetAlUnoxQuartzRefl(r);
}



//------++++++------++++++------++++++------++++++------++++++------++++++------
//					BuildEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::BuildEventList()
{
    // Builds a binary search tree ordered by time of the parent particle event
    // time. Begins at 0*ns and runs to a time determined by the total activity
    // and number of events requested. The purpose of the window width is to
    // create empty nodes in the BST to avoid quasi-degeneracy, but we won't
    // actually cut on the window end time when inserting events. Note that the 
    // window end time has a 10% pad on it, just to add a bit of overhead space
    G4int numEvts = GetNumEvents();
    G4double initialActivity = GetTotalSimulationActivity();

    //G4double windowStart, windowEnd; //units=seconds
    windowStart = 0;
    windowEnd   = 1e10;

    G4int numVols = (G4int)sourceByVolume.size();
    G4int recordTreeDepth = 20; 
    // For numEvents >> 1e7 increase depth 
    // and numVols > 1 increase depth and widen window
    if( numVols < 1 || initialActivity==0 ) {
        G4cout << "no activity registered"<<G4endl;
    }
    else {
        if(numEvents > 100) windowEnd = 2.*numEvts/initialActivity ;
        else                windowEnd = 4.*numEvts/initialActivity ;
       // if(numEvents > 1000)    windowEnd = 1.1*numEvts*numVols/initialActivity;
       // else if(numEvents > 50) windowEnd = 2.*numEvts*numVols/initialActivity ;
       // else                    windowEnd = 4.*numEvts*numVols/initialActivity ;
    }
    // The initialization levels(depth) created in the BST is defaulted to 20.
    // These levels contain empty nodes to subdivide the space to reduce
    // degeneracy of the BST. The number of nodes is 2^level-1 so be careful
    // about using many levels (i.e., 10 level will create 1023 empty nodes).
    // By experience 20 is reasonable for O(1e7) events. The number of events
    // added to the BST is trickier when multiple sources in multiple volumes
    // are requested. When using "PMT_Window" or similar each volume creates
    // numEvent of entries. 

    if( numEvts > 2.e6 ) {
        G4int npower = 1;
        while( numEvts/(2.e6*pow(2.,npower)) > 1. && recordTreeDepth < 25) { 
            npower++;
            recordTreeDepth++;
        }
    } 
    if( hasDecayChainSources ) recordTreeDepth++ ;//10-14 more decays/activity

    G4cout << "\n============================================================="
           << "========" << G4endl;
    G4cout << "Building binary search tree (BST) with depth " 
           << recordTreeDepth << G4endl; 
    G4cout << "  The BST time window runs from " << windowStart << " to " 
           << windowEnd << " s" << G4endl;

    if( recordTree )
        delete recordTree;
    recordTree = new LUXSimBST(recordTreeDepth, windowStart, windowEnd, numEvts);

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::GenerateEventList()
{
    // Each component and each source need to have population calculated
    // and the events generated
    for( G4int i=0; i<(G4int)sourceByVolume.size(); i++ ) 
        if( sourceByVolume[i].component ) 
            sourceByVolume[i].component->GenerateEventList(i);

    // Do this every so often, because PMT_Window has between 122-142 volumes!
    // Same as TrimEventList(); but with out terminating "======..."
    while( recordTree->GetNumNonemptyNodes()>numEvents ) recordTree->PopLast();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LoadEventsFile()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::LoadEventsFile(G4String eventsFile)
{
    std::ifstream file;
    file.open(eventsFile);
    if (!file.is_open()) {
        G4cout<<G4endl<<G4endl<<G4endl;
        G4cout<<"Events File Not Found!"<<G4endl;
        G4cout<<G4endl<<G4endl<<G4endl;
        exit(0);
    }
    G4int evtN;
    G4int particleID;
    G4double energyDep_keV;
    G4double positionX_cm;
    G4double positionY_cm;
    G4double positionZ_cm;
    while( 1 ) {
        file >> evtN >> particleID >> energyDep_keV >> positionX_cm >> positionY_cm >> positionZ_cm;
        if( file.fail() ) {
             break;
        }
        qEvtN.push(evtN);
        qParticleID.push(particleID);
        qEnergyDep_keV.push(energyDep_keV*keV);
        qPositionX_cm.push(positionX_cm*cm);
        qPositionY_cm.push(positionY_cm*cm);
        qPositionZ_cm.push(positionZ_cm*cm);
    }
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					NextEventToGenerate()
//------++++++------++++++------++++++------++++++------++++++------++++++------

G4int LUXSimManager::NextEventToGenerate()
{

    currentEvtN = qEvtN.front();
    while ( qEvtN.front() == currentEvtN && 
          !qEvtN.empty() ) {
        qEvtN.pop();
        currentParticleID.push(qParticleID.front());
        qParticleID.pop();
        currentEnergyDep_keV.push(qEnergyDep_keV.front());
        qEnergyDep_keV.pop();
        currentX_cm.push(qPositionX_cm.front());
        qPositionX_cm.pop();
        currentY_cm.push(qPositionY_cm.front());
        qPositionY_cm.pop();
        currentZ_cm.push(qPositionZ_cm.front());
        qPositionZ_cm.pop();
        
    }

    return currentParticleID.size();

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					NextParticleIDToGenerate()
//------++++++------++++++------++++++------++++++------++++++------++++++------

G4int LUXSimManager::NextParticleIDToGenerate()
{

    G4int partilceID = currentParticleID.front();
    currentParticleID.pop();
    return partilceID;

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					NextEnergyDepToGenerate()
//------++++++------++++++------++++++------++++++------++++++------++++++------

G4double LUXSimManager::NextEnergyDepToGenerate()
{

    G4double energyDep = currentEnergyDep_keV.front();
    currentEnergyDep_keV.pop();
    return energyDep;

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					NextPositionToGenerate()
//------++++++------++++++------++++++------++++++------++++++------++++++------

G4ThreeVector LUXSimManager::NextPositionToGenerate()
{

    G4ThreeVector pos;
    pos[0] = currentX_cm.front();
    currentX_cm.pop();
    pos[1] = currentY_cm.front();
    currentY_cm.pop();
    pos[2] = currentZ_cm.front();
    currentZ_cm.pop();
    return pos;

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					TrimEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::TrimEventList()
{
    // At the end of this process there will be more events in the decay record
    // than requested, so the excess can be reduced.
    G4cout << "Reducing the number of events in the tree to " << numEvents
           << "..." ;
    while( recordTree->GetNumNonemptyNodes()>numEvents ) recordTree->PopLast();
    G4cout << "Done. " << recordTree->GetNumNonemptyNodes() <<" events remain."
           << "\n============================================================="
           << "========" << G4endl;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					RecordTreeInsert()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::RecordTreeInsert(Isotope *iso, G4double t, G4ThreeVector p,
                        G4int sourceByVolumeID, G4int sourcesID)
{
    // Time sent and received in *ns. RecordTree window is set in seconds.
    if( t/ns < windowEnd*1.e9 )
        recordTree->Insert(iso, t, p, sourceByVolumeID, sourcesID);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateEvent()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::GenerateEvent( G4GeneralParticleSource *particleGun,
        G4Event *event )
{
    if( hasLUXSimSources && recordTree->GetNumNonemptyNodes()>0 ) {
        decayNode* firstNode;
        G4bool searchingNodes = true;
        while(searchingNodes){
            firstNode = recordTree->GetEarliest();
            if(firstNode->Z) searchingNodes = false;
            else recordTree->PopEarliest();
        }

        sourceByVolume[firstNode->sourceByVolumeID].component->
              GenerateFromEventList(particleGun, event, firstNode);
        recordTree->PopEarliest();
    }
    else { 
        G4cout << "No more events found OR using macro command sources" << G4endl;

    }
}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					AddDeposition()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::AddDeposition( LUXSimDetectorComponent* component,
				stepRecord aStep )
{
	//	Go through all the detector components, and if the volume being recorded
	//	is a registered detector component, pass that info to the correct object
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		if( luxSimComponents[i] == component ) {
			luxSimComponents[i]->AddDeposition( aStep );
			i = (G4int)luxSimComponents.size();
		}
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					SetPMTNumberingScheme()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::SetPMTNumberingScheme( G4String sel ) 
{
    if(sel == "real")
        useRealPMTNumberingScheme = true;
    if(sel == "linear")
        useRealPMTNumberingScheme = false;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					CapturePhotons()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4bool LUXSimManager::CapturePhotons( LUXSimDetectorComponent* component )
{
	//	Go through all the detector components, and if the volume with the
	//	optical photon is set to capture optical photons, return true so that
	//	the optical photon track can be killed
	G4bool response = false;
	
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
		if( luxSimComponents[i] == component ) {
			if( luxSimComponents[i]->GetCapturePhotons() )
				response = true;
			i = (G4int)luxSimComponents.size();
		}
	
	return response;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					RecordValues()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::RecordValues( G4int eventNum )
{
	//	Go through all the detector components, and if any have an record
	//	level greater than one, send the vector of steps to LUXSimOutput for
	//	recording.
    if( use100keVHack == 0 ) {
        for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
            if( (luxSimComponents[i]->GetRecordLevel() ||
                    luxSimComponents[i]->GetRecordLevelOptPhot() ||
                    luxSimComponents[i]->GetRecordLevelThermElec() )
                    && luxSimComponents[i]->GetEventRecord().size() )
                LUXSimOut->RecordEventByVolume( luxSimComponents[i], eventNum );
    } else if( liquidXenonTotalEnergy > 0.1*keV &&
            liquidXenonTotalEnergy < use100keVHack ) {
        for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
            if( (luxSimComponents[i]->GetRecordLevel() ||
                    luxSimComponents[i]->GetRecordLevelOptPhot() ||
                    luxSimComponents[i]->GetRecordLevelThermElec() )
                    && luxSimComponents[i]->GetEventRecord().size() )
                LUXSimOut->RecordEventByVolume( luxSimComponents[i], eventNum );
    }
    
    liquidXenonTotalEnergy = 0;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					ClearRecords()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimManager::ClearRecords()
{
	//	Go through all the detector components and clear the information
	//	vectors.
	for( G4int i=0; i<(G4int)luxSimComponents.size(); i++ )
			luxSimComponents[i]->ClearRecord();
	
	//	Now clear all the primary particles
	primaryParticles.clear();

	//close the output file before next run
	if ( GetRunEndedCleanly() ){
		if ( LUXSimOut ) delete LUXSimOut;
		LUXSimOut = NULL;
	}
}
/*
void LUXSimManager::LoadXYZDependentEField (G4String eFieldFile) {
  std::ifstream gridFile;
  gridFile.open(eFieldFile);
  if (!gridFile.is_open()) {
      G4cout<<G4endl<<G4endl<<G4endl;
      G4cout<<"E-Field File Not Found!"<<G4endl;
      G4cout<<G4endl<<G4endl<<G4endl;
      exit(0);      
  }
  for (int j = 0; j < 598; j++) {
    for (int i = 0; i < 251; i++) {
      gridFile >> xyzDependentEField[i][j];
    }
  }
  gridFile.close();
}

G4double LUXSimManager::GetXYZDependentElectricField (G4ThreeVector x) {
    G4double R = sqrt(x[0]*x[0] + x[1]*x[1]);
    G4double Z = x[2];

    // for now exploit the fact that we know the grid has 1 mm spacing
    // i.e. if R is in mm then the index for R is between floor(R) and ceil(R)
    // this needs to be made more flexible in the future
    G4int R1 = floor(R);
    G4int R2 = ceil(R);
    G4int Z1 = floor(Z);
    G4int Z2 = ceil(Z);

    // need to deal with cases were an RZ position is on one of the grid points

    G4double E3;

    if (R1 == R2 && Z1 == Z2) {
        E3 = xyzDependentEField[R1][Z1];
    }
    else {
        if (R1 == R2) {
            G4double E1 = xyzDependentEField[R1][Z1];
            G4double E2 = xyzDependentEField[R1][Z2];
            G4double M3 = (E2 - E1)/(Z2 - Z1);
            G4double B3 = E1 - M3*Z1;
            E3 = M3*Z + B3;
        }
        else if (Z1 == Z2) {
            G4double E1 = xyzDependentEField[R1][Z1];
            G4double E2 = xyzDependentEField[R2][Z1];
            G4double M3 = (E2 - E1)/(R2 - R1);
            G4double B3 = E1 - M3*R1;
            E3 = M3*R + B3;
        }
        else {
            // calculate slope from (R1, Z1) to (R2, Z1) and (R1, Z2) to (R2, Z2)
            // so we will have two slopes: M1 and M2 and two offsets B1 and B2

            G4double M1 = (xyzDependentEField[R2][Z1] - xyzDependentEField[R1][Z1])/(R2 - R1);
            G4double M2 = (xyzDependentEField[R2][Z2] - xyzDependentEField[R1][Z2])/(R2 - R1);

            G4double B1 = xyzDependentEField[R1][Z1] - M1*R1;
            G4double B2 = xyzDependentEField[R2][Z2] - M2*R2;

            G4double E1 = M1*R + B1;
            G4double E2 = M2*R + B2;

            // we now calculate the slope, M3, and intercept, B3, for these two points

            G4double M3 = (E2 - E1)/(Z2 - Z1);
            G4double B3 = E1 - M3*Z1;

            // we now can calculate the field at our point (R,Z)

            E3 = M3*Z + B3;
      }
  }

  E3 = -1.*E3/1e9; // converter V/m to... V/nm?

  return E3;
}

*/
void LUXSimManager::LoadXYZDependentDriftTime (G4String driftTimeFile) {
  std::ifstream gridFile;

  gridFile.open(driftTimeFile);
  if (!gridFile.is_open()) {
      G4cout<<G4endl<<G4endl<<G4endl;
      G4cout<<"Drift Time File Not Found!"<<G4endl;
      G4cout<<G4endl<<G4endl<<G4endl;
      exit(0);      
  }
  for (int j = 0; j < 598; j++) {
    for (int i = 0; i < 251; i++) {
      gridFile >> xyzDependentDriftTime[i][j];
    }
  }
  gridFile.close();
}
/*
G4double LUXSimManager::GetXYZDependentDriftTime (G4ThreeVector x) {
    G4double R = sqrt(x[0]*x[0] + x[1]*x[1]);
    G4double Z = x[2];

    // for now exploit the fact that we know the grid has 1 mm spacing
    // i.e. if R is in mm then the index for R is between floor(R) and ceil(R)
    // this needs to be made more flexible in the future
    G4int R1 = floor(R);
    G4int R2 = ceil(R);
    G4int Z1 = floor(Z);
    G4int Z2 = ceil(Z);

    G4double dT3;

    // ok, so now we have four points between which is interpolate
    // (R1, Z1), (R1, Z2), (R2, Z1), and (R2, Z2)
    // if the drift time from any of these points is -1 we assume
    // the drift time is -1 at (R, Z) for simplicity

    if (xyzDependentDriftTime[R1][Z1] < 0 ||
        xyzDependentDriftTime[R1][Z2] < 0 ||
        xyzDependentDriftTime[R2][Z1] < 0 ||
        xyzDependentDriftTime[R2][Z2] < 0) {
        return -1;
    }
    else {
	// need to deal with cases were an RZ position is on one of the grid points
        if (R1 == R2 && Z1 == Z2) {
            dT3 = xyzDependentDriftTime[R1][Z1];
        }
	else {
            if (R1 == R2) {
                G4double dT1 = xyzDependentDriftTime[R1][Z1];
                G4double dT2 = xyzDependentDriftTime[R1][Z2];
                G4double M3 = (dT2 - dT1)/(Z2 - Z1);
                G4double B3 = dT1 - M3*Z1;
                dT3 = M3*Z + B3;
            }
            else if (Z1 == Z2) {
                G4double dT1 = xyzDependentDriftTime[R1][Z1];
                G4double dT2 = xyzDependentDriftTime[R2][Z1];
                G4double M3 = (dT2 - dT1)/(R2 - R1);
                G4double B3 = dT1 - M3*R1;
                dT3 = M3*R + B3;
            }
            else {
                // calculate slope from (R1, Z1) to (R2, Z1) and (R1, Z2) to (R2, Z2)
                // so we will have two slopes: M1 and M2 and two offsets B1 and B2
                G4double M1 = (xyzDependentDriftTime[R2][Z1] - xyzDependentDriftTime[R1][Z1])/(R2 - R1);
                G4double M2 = (xyzDependentDriftTime[R2][Z2] - xyzDependentDriftTime[R1][Z2])/(R2 - R1);

                G4double B1 = xyzDependentDriftTime[R1][Z1] - M1*R1;
                G4double B2 = xyzDependentDriftTime[R2][Z2] - M2*R2;

                G4double dT1 = M1*R + B1;
                G4double dT2 = M2*R + B2;

                // we now calculate the slope, M3, and intercept, B3, for these two points

                G4double M3 = (dT2 - dT1)/(Z2 - Z1);
                G4double B3 = dT1 - M3*Z1;

                // we now can calculate the field at our point (R,Z)

                dT3 = M3*Z + B3;
            }
	}
    }
    dT3 = dT3 * 1000.; // LUXSim wants drift time in units of ns, not us
    return dT3;
}
*/
void LUXSimManager::LoadXYZDependentRadialDrift (G4String radialDriftFile) {
  std::ifstream gridFile;
  gridFile.open(radialDriftFile);
  if (!gridFile.is_open()) {
      G4cout<<G4endl<<G4endl<<G4endl;
      G4cout<<"Radial Drift File Not Found!"<<G4endl;
      G4cout<<G4endl<<G4endl<<G4endl;
      exit(0);
  }
  for (int j = 0; j < 598; j++) {
    for (int i = 0; i < 251; i++) {
      gridFile >> xyzDependentRadialDrift[i][j];
    }
  }
  gridFile.close();
}

G4double LUXSimManager::GetXYZDependentRadialDrift (G4ThreeVector x1) {
    G4double R = sqrt(x1[0]*x1[0] + x1[1]*x1[1]);
    G4double Z = x1[2];

    // for now exploit the fact that we know the grid has 1 mm spacing
    // i.e. if R is in mm then the index for R is between floor(R) and ceil(R)
    // this needs to be made more flexible in the future
    G4int R1 = floor(R);
    G4int R2 = ceil(R);
    G4int Z1 = floor(Z);
    G4int Z2 = ceil(Z);

    G4double tR3;

    // ok, so now we have four points between which is interpolate
    // (R1, Z1), (R1, Z2), (R2, Z1), and (R2, Z2)
    // if the drift time from any of these points is -1 we assume
    // the drift time is -1 at (R, Z) for simplicity

    if (xyzDependentRadialDrift[R1][Z1] < 0 ||
        xyzDependentRadialDrift[R1][Z2] < 0 ||
        xyzDependentRadialDrift[R2][Z1] < 0 ||
        xyzDependentRadialDrift[R2][Z2] < 0) {
        return -1;
    }
    else {
	// need to deal with cases were an RZ position is on one of the grid points
        if (R1 == R2 && Z1 == Z2) {
            tR3 = xyzDependentRadialDrift[R1][Z1];
        }
	else {
            if (R1 == R2) {
                G4double tR1 = xyzDependentRadialDrift[R1][Z1];
                G4double tR2 = xyzDependentRadialDrift[R1][Z2];
                G4double M3 = (tR2 - tR1)/(Z2 - Z1);
                G4double B3 = tR1 - M3*Z1;
                tR3 = M3*Z + B3;
            }
            else if (Z1 == Z2) {
                G4double tR1 = xyzDependentRadialDrift[R1][Z1];
                G4double tR2 = xyzDependentRadialDrift[R2][Z1];
                G4double M3 = (tR2 - tR1)/(R2 - R1);
                G4double B3 = tR1 - M3*R1;
                tR3 = M3*R + B3;
            }
            else {
                // calculate slope from (R1, Z1) to (R2, Z1) and (R1, Z2) to (R2, Z2)
                // so we will have two slopes: M1 and M2 and two offsets B1 and B2
                G4double M1 = (xyzDependentRadialDrift[R2][Z1] - xyzDependentRadialDrift[R1][Z1])/(R2 - R1);
                G4double M2 = (xyzDependentRadialDrift[R2][Z2] - xyzDependentRadialDrift[R1][Z2])/(R2 - R1);

                G4double B1 = xyzDependentRadialDrift[R1][Z1] - M1*R1;
                G4double B2 = xyzDependentRadialDrift[R2][Z2] - M2*R2;

                G4double tR1 = M1*R + B1;
                G4double tR2 = M2*R + B2;

                // we now calculate the slope, M3, and intercept, B3, for these two points

                G4double M3 = (tR2 - tR1)/(Z2 - Z1);
                G4double B3 = tR1 - M3*Z1;

                // we now can calculate the field at our point (R,Z)

                tR3 = M3*Z + B3;
            }
	}
    }
    tR3 = tR3 * 10.; // convert cm to mm
    return tR3;
}


void LUXSimManager::LoadXYZDependentEField (G4String eFieldFile) {
      G4cout<<G4endl<<G4endl<<G4endl;
      G4cout<<"Load Efieldfile!"<<G4endl;
      G4cout<<G4endl<<G4endl<<G4endl;

  for (int aa=0; aa<NumPlanesX; aa++) {

    for (int bb=0; bb<NumPlanesY; bb++) {

      for (int cc=0; cc<NumPlanesZ; cc++) {

  /*      xyzDependentEField[aa][bb][cc][0] = G4double(round(double(aa) -

            double(MinLocationX)));

        xyzDependentEField[aa][bb][cc][1] = G4double(round(double(bb) -

            double(MinLocationY)));

        xyzDependentEField[aa][bb][cc][2] = G4double(round(double(cc) -

            double(MinLocationZ)));*/

        for (int dd=3; dd<NumxyzDependentEFieldVars; dd++) {

          xyzDependentEField[aa][bb][cc][dd] = -100000.0;  //know when outside or if somehow a point is missing from the field map

        }

      }

    }

  }



  std::ifstream gridFile;

  gridFile.open(eFieldFile);

  if (!gridFile.is_open()) {

      G4cout<<G4endl<<G4endl<<G4endl;
      G4cout<<"E-Field File Not Found!"<<G4endl;
      G4cout<<G4endl<<G4endl<<G4endl;

      exit(0);

  }
	string sLine;

	getline(gridFile, sLine);         
//	G4cout << sLine.data() << G4endl;
  char readchar[20];
    
  G4double tempArray[NumxyzDependentEFieldVars];

  G4int tempX;

  G4int tempY;

  G4int tempZ;

  while (!gridFile.eof()) {

    for (int aa=0; aa<NumxyzDependentEFieldVars; aa++) {

      //read from EFieldFile

      //7 values: xi, yi, zi, E, x, y, drift_time
	
      gridFile >> readchar;
	if (strcmp(readchar,"nan")!=0) {
	tempArray[aa] = atof(readchar);
	//G4cout<<tempArray[aa]<<G4endl<<G4endl;
	}
	
   }

    //find array location to save in which to save info

    tempX = (int)(round(double((tempArray[0]*10. - MinLocationX)/StepSizeX)));

    tempY = (int)(round(double((tempArray[1]*10.  - MinLocationY)/StepSizeY)));

    tempZ = (int)(round(double((tempArray[2]*10.  - MinLocationZ)/StepSizeZ)));


    for (int aa=0; aa<NumxyzDependentEFieldVars; aa++) {

      //save the info
	if(tempX>-1&&tempX<NumPlanesX&&tempY>-1&&tempY<NumPlanesY&&tempZ>-1&&tempZ<NumPlanesZ){
	if(aa==0||aa==1||aa==4||aa==5)      xyzDependentEField[tempX][tempY][tempZ][aa] = tempArray[aa]*10.;//convert unit to mm
	else if (aa==2)	xyzDependentEField[tempX][tempY][tempZ][aa] = tempArray[aa]*10.;//56 mm is height of cathode.
	else if (aa==6) xyzDependentEField[tempX][tempY][tempZ][aa] = tempArray[aa]*1000.;//convert unit to ns.

	else 	{xyzDependentEField[tempX][tempY][tempZ][aa] = tempArray[aa];       
}
	}

    }

  }

  gridFile.close();

}

G4double LUXSimManager::GetXYZDependentElectricField (G4ThreeVector x) {

    // for now exploit the fact that we know the grid has 1 mm spacing
    // i.e. if R is in mm then the index for R is between floor(R) and ceil(R)
    // this needs to be made more flexible in the future

	int value = 3; //EField is the [3]component of xyzDependentEField map.

	int Z_index = (int) ((x[2]-MinLocationZ)/StepSizeZ); //56mm is height of cathode
	int Y_index = (int) ((x[1]-MinLocationY)/StepSizeY);
	int X_index = (int) ((x[0]-MinLocationX)/StepSizeX);
	// Not in the liquid, ignore this event.
	if (Z_index < 0 || Z_index >  (NumPlanesZ-2) ||
		Y_index < 0 || Y_index > (NumPlanesY-2) ||
		X_index < 0 || X_index > (NumPlanesX-2)){
        return -100000.;
	}
	

	//8 index that is closest to the points we gave.
	int id[8];
	id[0] = (((X_index)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[1] = (((X_index+1)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[2] = (((X_index)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[3] = (((X_index+1)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[4] = (((X_index)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[5] = (((X_index+1)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[6] = (((X_index)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[7] = (((X_index+1)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;

	G4double E3 = -100000.;
	

	// Not in the fiducial region, ignore this event.
	for(int k=0;k<8;k++){
		if(*(&xyzDependentEField[0][0][0][0]+id[k]+7) ==0) {return -100000.;}

	}
	
	// gradient of zyx direction
	G4double z_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)-*(&xyzDependentEField[0][0][0][0]+id[1]+value)-*(&xyzDependentEField[0][0][0][0]+id[2]+value)-*(&xyzDependentEField[0][0][0][0]+id[3]+value)+*(&xyzDependentEField[0][0][0][0]+id[4]+value)+*(&xyzDependentEField[0][0][0][0]+id[5]+value)+*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeZ; 
	G4double y_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)-*(&xyzDependentEField[0][0][0][0]+id[1]+value)+*(&xyzDependentEField[0][0][0][0]+id[2]+value)+*(&xyzDependentEField[0][0][0][0]+id[3]+value)-*(&xyzDependentEField[0][0][0][0]+id[4]+value)-*(&xyzDependentEField[0][0][0][0]+id[5]+value)+*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeY; 
	G4double x_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)+*(&xyzDependentEField[0][0][0][0]+id[1]+value)-*(&xyzDependentEField[0][0][0][0]+id[2]+value)+*(&xyzDependentEField[0][0][0][0]+id[3]+value)-*(&xyzDependentEField[0][0][0][0]+id[4]+value)+*(&xyzDependentEField[0][0][0][0]+id[5]+value)-*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeX; 
	//interpolation from the closest point.
	//find the closest point of the 8 first.
	int i=0;
	for (int j=1;j<8;j++){

		if((pow(x[0]-*(&xyzDependentEField[0][0][0][0]+id[j]+0),2)+ pow(x[1]-*(&xyzDependentEField[0][0][0][0]+id[j]+1),2)+ pow(x[2]-*(&xyzDependentEField[0][0][0][0]+id[j]+2),2))<(pow(x[0]-*(&xyzDependentEField[0][0][0][0]+id[i]+0),2)+ pow(x[1]-*(&xyzDependentEField[0][0][0][0]+id[i]+1),2)+ pow(x[2]-*(&xyzDependentEField[0][0][0][0]+id[i]+2),2))) i=j;
	}

	E3 = *(&xyzDependentEField[0][0][0][0]+id[i]+value)+x_gradient*(x[0]-*(&xyzDependentEField[0][0][0][0]+id[i]+0))+y_gradient*(x[1]-*(&xyzDependentEField[0][0][0][0]+id[i]+1))+z_gradient*(x[2]-*(&xyzDependentEField[0][0][0][0]+id[i]+2));
	E3 = -1.*E3/1e9; // converter V/m to... V/nm?
	//G4cout<<"E="<<E3<<G4endl<<G4endl;
	return E3;

}


G4double LUXSimManager::GetXYZDependentS2_X (G4ThreeVector x) {

    // for now exploit the fact that we know the grid has 1 mm spacing
    // i.e. if R is in mm then the index for R is between floor(R) and ceil(R)
    // this needs to be made more flexible in the future

	int value = 4; //s2x is the [4]component of xyzDependentEField map.

	int Z_index = (int) ((x[2]-MinLocationZ)/StepSizeZ); 
	int Y_index = (int) ((x[1]-MinLocationY)/StepSizeY);
	int X_index = (int) ((x[0]-MinLocationX)/StepSizeX);
	// Not in the liquid, ignore this event.
	if (Z_index < 0 || Z_index >  (NumPlanesZ-2) ||
		Y_index < 0 || Y_index > (NumPlanesY-2) ||
		X_index < 0 || X_index > (NumPlanesX-2)){
        return -100000.;
	}
	

	//8 index that is closest to the points we gave.
	int id[8];
	id[0] = (((X_index)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[1] = (((X_index+1)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[2] = (((X_index)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[3] = (((X_index+1)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[4] = (((X_index)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[5] = (((X_index+1)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[6] = (((X_index)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[7] = (((X_index+1)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;

	G4double E3 = -100000.;
	

	// Not in the fiducial region, ignore this event.
	for(int k=0;k<8;k++){
		if(*(&xyzDependentEField[0][0][0][0]+id[k]+7) !=1) {return -100000.;}
	}
	
	// gradient of zyx direction
	G4double z_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)-*(&xyzDependentEField[0][0][0][0]+id[1]+value)-*(&xyzDependentEField[0][0][0][0]+id[2]+value)-*(&xyzDependentEField[0][0][0][0]+id[3]+value)+*(&xyzDependentEField[0][0][0][0]+id[4]+value)+*(&xyzDependentEField[0][0][0][0]+id[5]+value)+*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeZ; 
	G4double y_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)-*(&xyzDependentEField[0][0][0][0]+id[1]+value)+*(&xyzDependentEField[0][0][0][0]+id[2]+value)+*(&xyzDependentEField[0][0][0][0]+id[3]+value)-*(&xyzDependentEField[0][0][0][0]+id[4]+value)-*(&xyzDependentEField[0][0][0][0]+id[5]+value)+*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeY; 
	G4double x_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)+*(&xyzDependentEField[0][0][0][0]+id[1]+value)-*(&xyzDependentEField[0][0][0][0]+id[2]+value)+*(&xyzDependentEField[0][0][0][0]+id[3]+value)-*(&xyzDependentEField[0][0][0][0]+id[4]+value)+*(&xyzDependentEField[0][0][0][0]+id[5]+value)-*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeX; 
	//interpolation from the closest point.
	//find the closest point of the 8 first.
	int i=0;
	for (int j=1;j<8;j++){

		if((pow(x[0]-*(&xyzDependentEField[0][0][0][0]+id[j]+0),2)+ pow(x[1]-*(&xyzDependentEField[0][0][0][0]+id[j]+1),2)+ pow(x[2]-*(&xyzDependentEField[0][0][0][0]+id[j]+2),2))<(pow(x[0]-*(&xyzDependentEField[0][0][0][0]+id[i]+0),2)+ pow(x[1]-*(&xyzDependentEField[0][0][0][0]+id[i]+1),2)+ pow(x[2]-*(&xyzDependentEField[0][0][0][0]+id[i]+2),2))) i=j;
	}

	E3 = *(&xyzDependentEField[0][0][0][0]+id[i]+value)+x_gradient*(x[0]-*(&xyzDependentEField[0][0][0][0]+id[i]+0))+y_gradient*(x[1]-*(&xyzDependentEField[0][0][0][0]+id[i]+1))+z_gradient*(x[2]-*(&xyzDependentEField[0][0][0][0]+id[i]+2));
	//G4cout<<"x="<<x[0]<<"y="<<x[1]<<"z="<<x[2]<<", s2x="<<E3<<G4endl<<G4endl;
	return E3;
}

G4double LUXSimManager::GetXYZDependentS2_Y (G4ThreeVector x) {


    // for now exploit the fact that we know the grid has 1 mm spacing
    // i.e. if R is in mm then the index for R is between floor(R) and ceil(R)
    // this needs to be made more flexible in the future

	int value = 5; //s2y is the [4]component of xyzDependentEField map.
	int Z_index = (int) ((x[2]-MinLocationZ)/StepSizeZ); 
	int Y_index = (int) ((x[1]-MinLocationY)/StepSizeY);
	int X_index = (int) ((x[0]-MinLocationX)/StepSizeX);
	// Not in the liquid, ignore this event.
	if (Z_index < 0 || Z_index >  (NumPlanesZ-2) ||
		Y_index < 0 || Y_index > (NumPlanesY-2) ||
		X_index < 0 || X_index > (NumPlanesX-2)){
        return -100000.;
	}
	

	//8 index that is closest to the points we gave.
	int id[8];
	id[0] = (((X_index)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[1] = (((X_index+1)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[2] = (((X_index)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[3] = (((X_index+1)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[4] = (((X_index)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[5] = (((X_index+1)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[6] = (((X_index)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[7] = (((X_index+1)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;

	G4double E3 = -100000.;
	

	// Not in the fiducial region, ignore this event.
	for(int k=0;k<8;k++){
		if(*(&xyzDependentEField[0][0][0][0]+id[k]+7) != 1) {return -100000.;}
	}
	
	// gradient of zyx direction
	G4double z_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)-*(&xyzDependentEField[0][0][0][0]+id[1]+value)-*(&xyzDependentEField[0][0][0][0]+id[2]+value)-*(&xyzDependentEField[0][0][0][0]+id[3]+value)+*(&xyzDependentEField[0][0][0][0]+id[4]+value)+*(&xyzDependentEField[0][0][0][0]+id[5]+value)+*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeZ; 
	G4double y_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)-*(&xyzDependentEField[0][0][0][0]+id[1]+value)+*(&xyzDependentEField[0][0][0][0]+id[2]+value)+*(&xyzDependentEField[0][0][0][0]+id[3]+value)-*(&xyzDependentEField[0][0][0][0]+id[4]+value)-*(&xyzDependentEField[0][0][0][0]+id[5]+value)+*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeY; 
	G4double x_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)+*(&xyzDependentEField[0][0][0][0]+id[1]+value)-*(&xyzDependentEField[0][0][0][0]+id[2]+value)+*(&xyzDependentEField[0][0][0][0]+id[3]+value)-*(&xyzDependentEField[0][0][0][0]+id[4]+value)+*(&xyzDependentEField[0][0][0][0]+id[5]+value)-*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeX; 
	//interpolation from the closest point.
	//find the closest point of the 8 first.
	int i=0;
	for (int j=1;j<8;j++){

		if((pow(x[0]-*(&xyzDependentEField[0][0][0][0]+id[j]+0),2)+ pow(x[1]-*(&xyzDependentEField[0][0][0][0]+id[j]+1),2)+ pow(x[2]-*(&xyzDependentEField[0][0][0][0]+id[j]+2),2))<(pow(x[0]-*(&xyzDependentEField[0][0][0][0]+id[i]+0),2)+ pow(x[1]-*(&xyzDependentEField[0][0][0][0]+id[i]+1),2)+ pow(x[2]-*(&xyzDependentEField[0][0][0][0]+id[i]+2),2))) i=j;
	}

	E3 = *(&xyzDependentEField[0][0][0][0]+id[i]+value)+x_gradient*(x[0]-*(&xyzDependentEField[0][0][0][0]+id[i]+0))+y_gradient*(x[1]-*(&xyzDependentEField[0][0][0][0]+id[i]+1))+z_gradient*(x[2]-*(&xyzDependentEField[0][0][0][0]+id[i]+2));
	return E3;
}

G4double LUXSimManager::GetXYZDependentDriftTime (G4ThreeVector x) {
//      G4cout<<G4endl<<G4endl<<G4endl;
//      G4cout<<"Get Drift time from file!"<<G4endl;
//      G4cout<<G4endl<<G4endl<<G4endl;

    // for now exploit the fact that we know the grid has 1 mm spacing
    // i.e. if R is in mm then the index for R is between floor(R) and ceil(R)
    // this needs to be made more flexible in the future

	int value = 6; //DriftTime is the [6]component of xyzDependentEField map.

	int Z_index = (int) ((x[2]-MinLocationZ)/StepSizeZ); 
	int Y_index = (int) ((x[1]-MinLocationY)/StepSizeY);
	int X_index = (int) ((x[0]-MinLocationX)/StepSizeX);
	// Not in the liquid, ignore this event.
	if (Z_index < 0 || Z_index >  (NumPlanesZ-2) ||
		Y_index < 0 || Y_index > (NumPlanesY-2) ||
		X_index < 0 || X_index > (NumPlanesX-2)){
        return -100000.;
	}
	

	//8 index that is closest to the points we gave.
	int id[8];
	id[0] = (((X_index)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[1] = (((X_index+1)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[2] = (((X_index)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[3] = (((X_index+1)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index)*NumxyzDependentEFieldVars;
	id[4] = (((X_index)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[5] = (((X_index+1)*NumPlanesY + (Y_index)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[6] = (((X_index)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;
	id[7] = (((X_index+1)*NumPlanesY + (Y_index+1)) * NumPlanesZ+ Z_index+1)*NumxyzDependentEFieldVars;

	G4double E3 = -100000.;
	

	// Not in the fiducial region, ignore this event.
	for(int k=0;k<8;k++){
		if(*(&xyzDependentEField[0][0][0][0]+id[k]+7) != 1) {return -100000.;}
	}
	
	// gradient of zyx direction
	G4double z_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)-*(&xyzDependentEField[0][0][0][0]+id[1]+value)-*(&xyzDependentEField[0][0][0][0]+id[2]+value)-*(&xyzDependentEField[0][0][0][0]+id[3]+value)+*(&xyzDependentEField[0][0][0][0]+id[4]+value)+*(&xyzDependentEField[0][0][0][0]+id[5]+value)+*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeZ; 
	G4double y_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)-*(&xyzDependentEField[0][0][0][0]+id[1]+value)+*(&xyzDependentEField[0][0][0][0]+id[2]+value)+*(&xyzDependentEField[0][0][0][0]+id[3]+value)-*(&xyzDependentEField[0][0][0][0]+id[4]+value)-*(&xyzDependentEField[0][0][0][0]+id[5]+value)+*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeY; 
	G4double x_gradient = (-*(&xyzDependentEField[0][0][0][0]+id[0]+value)+*(&xyzDependentEField[0][0][0][0]+id[1]+value)-*(&xyzDependentEField[0][0][0][0]+id[2]+value)+*(&xyzDependentEField[0][0][0][0]+id[3]+value)-*(&xyzDependentEField[0][0][0][0]+id[4]+value)+*(&xyzDependentEField[0][0][0][0]+id[5]+value)-*(&xyzDependentEField[0][0][0][0]+id[6]+value)+*(&xyzDependentEField[0][0][0][0]+id[7]+value))/4./StepSizeX; 
	//interpolation from the closest point.
	//find the closest point of the 8 first.
	int i=0;
	for (int j=1;j<8;j++){

		if((pow(x[0]-*(&xyzDependentEField[0][0][0][0]+id[j]+0),2)+ pow(x[1]-*(&xyzDependentEField[0][0][0][0]+id[j]+1),2)+ pow(x[2]-*(&xyzDependentEField[0][0][0][0]+id[j]+2),2))<(pow(x[0]-*(&xyzDependentEField[0][0][0][0]+id[i]+0),2)+ pow(x[1]-*(&xyzDependentEField[0][0][0][0]+id[i]+1),2)+ pow(x[2]-*(&xyzDependentEField[0][0][0][0]+id[i]+2),2))) i=j;
	}

	E3 = *(&xyzDependentEField[0][0][0][0]+id[i]+value)+x_gradient*(x[0]-*(&xyzDependentEField[0][0][0][0]+id[i]+0))+y_gradient*(x[1]-*(&xyzDependentEField[0][0][0][0]+id[i]+1))+z_gradient*(x[2]-*(&xyzDependentEField[0][0][0][0]+id[i]+2));
	return E3;
}





