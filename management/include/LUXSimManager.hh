////////////////////////////////////////////////////////////////////////////////
/*	LUXSimManager.hh
*
* This is the header file to manage all the workings of LUXSim. This class is a
* clearing house for all internal controls, from geometry to output to event
* generation. All classes can and should have access to this manager class, and
* there should only be one instance of this class in the entire simulation.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	20 April 2009 - Added the SetSource method (Kareem)
*	20 April 2009 - Added support for the LUXSimSourceCatalog object (Kareem)
*	22 April 2009 - Added support for radioactive loads / event generation
*					(Kareem)
*	27 July 2009 - Added Get/SetuseOpticalProcesses method (Kareem)
*	28 July 2009 - Added Get/SetOutputDir methods (Kareem)
*	13-Aug-09 - Added the variables to store the list of commands and the svn
*				diffs, as well as methods to retrieve each of those. (Kareem)
*	13-Aug-09 - Added GetDetectorComponentTable declaration. (Kareem)
*	14-Sep-09 - Added the optical photon record level Get and Set methods
*				(Kareem)
*	28-Sept-09 - Added CryoStand Get/Set methods (Melinda)
*	29-Sept-09 - Added grid Get/Set methods (Melinda)
*	23-Mar-10 - Added a variable for the Mersenne Twister randomization engine
*				(Kareem)
*	27-Apr-10 - Added variables and methods for recording and accessing the
*				primary particles in any given event (Kareem)
*	26-Jul-10 - Added Get and Set methods for controlling the output name (Chao)
*	19-Aug-10 - Implemented a "run ended cleanly" flag to prevent the .tmp
*				file being renamed a .bin file if the run was ended prematurely
*				(Kareem)
*	20-Sep-10 - Added registration method for the LUXSimMaterials class (Kareem)
*	20-Sep-10 - Added a flag to turn on simple optical parameters for debugging
*				purposes (Kareem)
*	27-Sep-10 - Removed "using namespace" line (Kareem)
*	27-Nov-10 - Added Get/Set methods for always record primary (Kareem)
*	31-Jan-11 - Added the GetComponentRecordLevel declaration (Kareem)
*   21-Jul-11 - Added Decay Chain Generating methods (Nick)
*	23-Aug-11 - Added Set/Get NumGNARRLIPMTs to control whether 1 or 4 PMTs
*				are built, uses the bool numGNARRLIPMTFlag (Tenzing)
*   21-Sep-11 - Electric field in gas/liquid methods (Nick)
*	02-Dec-11 - Added the creator process to the stepRecord struct (Kareem)
*	18-Jan-12 - Added methods for setting material properties (Dave)
*	24-Feb-12 - Added LZFlex controls: total mass, Xe skin thickness, and
*				scintillator thickness (Kareem)
*	06-Mar-12 - Added functions for setting LZFlex Z position (Dave)
*	09-Mar-12 - Renamed Z position functions (Dave)
*	09-Mar-12 - Added support for Rayleigh UI hooks (Dave)
*   24-Mar-12 - Added support for the event progress report UI hooks (Mike)
*	01-May-12 - Added support for thermal electron record levels (Kareem)
*   12-Jul-12 - Added support for different E-fields (Nick & Matt)
*   15-Jul-12 - All events are time-ordered in a binary search tree. (Nick)
*   16-Aug-12 - Added support for switching PMT numbering schemes (Rich)
*   24-Aug-12 - Modification to speed up multiple volume source setting (Nick)
*	25-Sep-12 - Added hook to switch back and forth between the slight changes
*				to the LUX geometry between the surface and underground
*				installations (Kareem)
*	25-Sep-12 - Added flag for gas running (Dave)
*	12-Dec-06 - Replaced E-field commands with commands to set grid voltages
*				and added electric field printing command (Dave)
*   04-Feb-13 - Added commands and hooks for setting the S1 and S2 gain (Kareem)
*	22-Feb-13 - Added commands and hooks for setting the drifting electron
*				attenuation length (Kareem)
*	11-Mar-13 - Added GetComponentByName method (Kareem)
*	18-Mar-13 - Added the hooks for setting the collimator source diameter
*				(Kareem)
*	23-Apr-13 - Added hooks for the LZSimple neutron calibration tube (Kareem)
*	26-Apr-13 - Added hooks for the LZSimple calibration source height (Kareem)
*	06-May-13 - Added an UpdateGeometry call for when the height of the LZSimple
*				source is set (Kareem)
*   23-Jun-13 - Added hooks for the LZSimple neutron calibration top tube
*               (Kareem)
*   18-Jul-13 - Added functions for the LUX neutron collimator tube (Kevin)
*   03-Dec-13 - Added Get/Set methods for the top and bottom PMT bank voltages
*               (Kareem)
*   07-Apr-13 - Added 100-keV hack setting (Kareem)
*   14-Apr-14 - Added commands to use an XYZ-dependent drift time and radial drift
*               from a file. (Kevin)
*   14-May-14 - Added option in LZSimple geometry for the tungsten pig for a YBe
*               source. (Kevin)
*   12-May-14 - Added commands for the EventsFile generator, which allows
*               for energy deposits to be generated based on an input text file
*               (Kevin)
*   16-May-14 - Added commands to change location of LZ DD neutron calibration
*      	       	tube (Kevin)
*   20-May-14 - Spun off the SetUseOpticalProcesses method to the code file
*               (Kareem)
*   28-May-14 - Added flag to turn on/off a new LZVeto system (Scott Ha.)
*   03-Sep-14 - Added flag for checking overlaps for all volumes (automatic
*               checking off by default) (Kareem)
*   26-Sep-14 - Added option to change YBe pig height and diameter (Kevin)
*   14-Oct-14 - Added SetComponentMass and SetComponentVolume declarations
*               (Kareem)
*   25-Jan-15 - Added TwoElectrons generator command
*               (Kevin)
*   02-Feb-15 - Added pencil beam command for the LUX DD generator (Kevin)
*   20-Aug-15 - Added the current process name to the step record (Kareem)
*   27-Aug-15 - Added GetLiquidXenonEnergy method (Kareem)
*   28-Sep-15 - Added SVN/Git repo check support (Kareem)
*   06-Oct-15 - Added methods for G4Decay generator (David W)
*   18-Dec-2015 - Added muon-nuclear interaction physics (David W, merged in by Doug T)
*   
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimManager_HH
#define LUXSimManager_HH 1

//
//	C/C++ includes
//
#include <vector>
#include <queue>
#include <map>

//
//	GEANT4 includes
//
#include "G4ThreeVector.hh"
#include "globals.hh"

//
//	CLHEP includes
//
#include "CLHEP/Random/MTwistEngine.h"

//
//	LUXSim includes
//
#include "LUXSimMaterials.hh"
#include "LUXSimBST.hh"
//#include "G4S1Light.hh"
//#include "LUXSimIsotope.hh"

//
//	Class forwarding
//
class G4UImanager;
class G4GeneralParticleSource;
class G4Event;

class LUXSimPhysicsList;
class LUXSimPhysicsOpticalPhysics;
class LUXSimPhysicsMuonNuclear;
class LUXSimDetectorConstruction;
class LUXSimDetectorComponent;
class LUXSimPrimaryGeneratorAction;
class LUXSimRunAction;
class LUXSimEventAction;
class LUXSimSteppingAction;
class LUXSimOutput;
class LUXSimMessenger;
class LUXSimSourceCatalog;

        static const int NumPlanesX = 49;  //placeholder value but should be close

        static const int NumPlanesY = 49;  //placeholder value

        static const int NumPlanesZ = 49;  //placeholder value

        static const G4double MinLocationX = -240.0 ;  //placeholder value

        static const G4double MinLocationY = -240.0;  //placeholder value

        static const G4double MinLocationZ = 58.0;  //placeholder value, but probably correct

        static const G4double StepSizeX = 10.;  //Subject to change
	static const G4double StepSizeY = 10.; 
	static const G4double StepSizeZ = 10.; 
        static const int NumxyzDependentEFieldVars = 8;  //xi, yi, zi, E, S2x, S2y, drift_time

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimManager
{
	public:
		LUXSimManager();
		~LUXSimManager();



		//	General methods
		static LUXSimManager *GetManager();
		
		//	Overloaded registration methods
		void Register( LUXSimPhysicsList* phys ) { LUXSimPhysics = phys; };
		void Register( LUXSimPhysicsOpticalPhysics *optPhys )
				{ LUXSimPhysicsOptical = optPhys; }
		void Register(LUXSimPhysicsMuonNuclear *muNuc)
				{LUXSimPhysicsMuNuc = muNuc;}

		void Register( LUXSimDetectorConstruction *con )
				{ LUXSimDetector = con; };
		void Register( LUXSimMaterials *mat ) { LUXSimMat = mat; };
		void Register( LUXSimPrimaryGeneratorAction *gen )
				{ LUXSimGenerator = gen; };
		void Register( LUXSimRunAction *run ) { LUXSimRun = run; };
		void Register( LUXSimEventAction *evt ) { LUXSimEvent = evt; };
		void Register( LUXSimSteppingAction *step ) { LUXSimStep = step; };
		void Register( LUXSimOutput *out ) { LUXSimOut = out; };
		void Register( LUXSimSourceCatalog *cat ) { LUXSimSourceCat = cat; };
		
		LUXSimPhysicsList *GetPhysicsList() { return LUXSimPhysics; };
		LUXSimDetectorConstruction *GetDetectorConstruction() {
				return LUXSimDetector; };
		LUXSimPrimaryGeneratorAction *GetGenerator() {
				return LUXSimGenerator; };
		LUXSimRunAction *GetRun() { return LUXSimRun; };
		LUXSimEventAction *GetEvent() { return LUXSimEvent; };
		LUXSimSteppingAction *GetStep() { return LUXSimStep; };
		LUXSimOutput *GetOutput() { return LUXSimOut; };
		LUXSimSourceCatalog *GetSourceCatalog() { return LUXSimSourceCat; };
		
		//	General-purpose methods
		void BeamOn( G4int );
		inline G4int GetRandomSeed() { return randomSeed; };
		void SetRandomSeed( G4int );
		
		//	Input/output methods
        void SetIsSVNRepo( G4bool isSVN ) { IsSVNRepo = isSVN; }
        G4bool GetIsSVNRepo() { return IsSVNRepo; }
        void SetIsGitRepo( G4bool isGit ) { IsGitRepo = isGit; }
        G4bool GetIsGitRepo () { return IsGitRepo; }
		void SetCompilationDirectory( G4String dir ) { compilationDir = dir; };
		G4String GetCompilationDirectory() { return compilationDir; };
		void SetOutputDir( G4String );
		G4String GetOutputDir() { return outputDir; };
		void SetOutputName( G4String name ) { outputName = name; };
		G4String GetOutputName() { return outputName; };
		G4String GetHistoryFile() { return historyFile; };
		G4String GetInputCommands() { return listOfCommands; };
		G4String GetDiffs() { return listOfDiffs; };
		
		void SetRunEndedCleanly( G4bool val ) { runEndedCleanly = val; };
		G4bool GetRunEndedCleanly() { return runEndedCleanly; };
		G4int GetNumEvents() { return numEvents; };
		
		void SetAlwaysRecordPrimary( G4bool val ) { alwaysRecordPrimary = val;};
		G4bool GetAlwaysRecordPrimary() { return alwaysRecordPrimary; };

                void InitialiseEventCount(){ eventCount=0; };
                void IterateEventCount(){ eventCount++; };
                G4int GetEventCount(){ return eventCount; };
  

       	// the next block of functions are for loading XYZ dependent electric field strength,
        // drift time, and radial drift from charging teflon from Scott's COMSOL simulations
//        void LoadXYZDependentEField (G4String eFieldFile);
//        double GetXYZDependentElectricField (G4ThreeVector x1);
       	void LoadXYZDependentDriftTime (G4String driftTimeFile);
//        double GetXYZDependentDriftTime (G4ThreeVector x1);
       	void LoadXYZDependentRadialDrift (G4String radialDriftFile);
        double GetXYZDependentRadialDrift (G4ThreeVector x1);

	void LoadXYZDependentEField (G4String eFieldFile);
	G4double GetXYZDependentElectricField (G4ThreeVector x);
	G4double GetXYZDependentS2_X(G4ThreeVector x);
	G4double GetXYZDependentS2_Y (G4ThreeVector x);
	G4double GetXYZDependentDriftTime (G4ThreeVector x);
/**/
        void SetEventProgressFrequency( G4int val ) { eventProgressFrequency = val;};
        G4int GetEventProgressFreqnecy() { return eventProgressFrequency; };
    
        inline G4double Get100keVHack() { return use100keVHack; };
        inline void Set100keVHack( G4double val ) { use100keVHack = val; };
        inline void AddLiquidXenonEnergy( G4double en )
                { liquidXenonTotalEnergy += en; };
        inline G4double GetLiquidXenonEnergy() {return liquidXenonTotalEnergy;};

        // User defined variables methods
        void SetUserVar1( G4double var ) { userVar1 = var;};
        void SetUserVar2( G4double var ) { userVar2 = var;};
        void SetUserVar3( G4double var ) { userVar3 = var;};
        void SetUserVar4( G4double var ) { userVar4 = var;};
        void SetUserVar5( G4double var ) { userVar5 = var;};
        G4double GetUserVar1() { return userVar1;};
        G4double GetUserVar2() { return userVar2;};
        G4double GetUserVar3() { return userVar3;};
        G4double GetUserVar4() { return userVar4;};
        G4double GetUserVar5() { return userVar5;};

		//	Geometry methods
		void UpdateGeometry();
		
		void Register( LUXSimDetectorComponent *com )
				{ luxSimComponents.push_back( com ); };
		void Deregister( LUXSimDetectorComponent* );
		
		G4String GetDetectorComponentLookupTable();

		void SetDetectorSelection( G4String sel ) {
				detectorSelection = sel; /*UpdateGeometry();*/ };
		G4String GetDetectorSelection() { return detectorSelection; };
    
        void SetCheckVolumeOverlaps( G4bool val ) {
                checkVolumeOverlaps = val; }
        G4bool GetCheckVolumeOverlaps() { return checkVolumeOverlaps; };
    
        void SetComponentMass( G4String );
        void SetComponentVolume( G4String );

		void SetMuonVetoSelection( G4String sel ) {
				muonVetoSelection = sel; /*UpdateGeometry();*/ };
		G4String GetMuonVetoSelection() { return muonVetoSelection; };
		
		void SetLZVetoSelection( G4String sel ) {
				LZVetoSelection = sel; /*UpdateGeometry();*/ };
		G4String GetLZVetoSelection() { return LZVetoSelection; };
		
		void SetCryoStandSelection( G4String sel ) {
				cryoStandSelection = sel; /*UpdateGeometry();*/ };
		G4String GetCryoStandSelection() { return cryoStandSelection; };

		void SetGridWiresSelection( G4String sel ) {
				gridWiresSelection = sel; /*UpdateGeometry();*/ };
		G4String GetGridWiresSelection() { return gridWiresSelection; };

                void SetPMTNumberingScheme( G4String sel );

                G4bool GetPMTNumberingScheme() { return useRealPMTNumberingScheme; };

		G4bool CapturePhotons( LUXSimDetectorComponent* );

		void SetRecordLevel( G4String );
		std::vector<G4int> GetRecordLevels( G4String );
		G4int GetComponentRecordLevel( LUXSimDetectorComponent* );
		
		void SetRecordLevelOptPhot( G4String );
		std::vector<G4int> GetRecordLevelsOptPhot( G4String );
		G4int GetComponentRecordLevelOptPhot( LUXSimDetectorComponent* );
		
		void SetRecordLevelThermElec( G4String );
		std::vector<G4int> GetRecordLevelsThermElec( G4String );
		G4int GetComponentRecordLevelThermElec( LUXSimDetectorComponent* );
		
		LUXSimDetectorComponent *GetComponentByName( G4String );
		
		void SetCollimatorHeight( G4double );
		void SetCollimatorHoleDiameter( G4double );
		void SetCollimatorSourceDiameter( G4double );

		void SetNumGNARRLIPMTs( G4bool  val ) { numGNARRLIPMTFlag = val; };
		G4bool GetNumGNARRLIPMTs() { return numGNARRLIPMTFlag; };

		void SetDetectorZ( G4double );
		G4double GetDetectorZ()
				{ return detZ; };
		
		void SetLUXSurfaceGeometry( G4bool val ) { luxSurfaceGeometry = val; };
		G4bool GetLUXSurfaceGeometry() { return luxSurfaceGeometry; };
		
		void SetGasRun( G4bool val ) { gasRun = val; 
			G4cout << "Gas running set to " << gasRun << G4endl; };
		G4bool GetGasRun() { return gasRun; };

		void SetLUXNeutronCollimator( G4bool val )
				{ luxNeutronCollimator = val; };
		G4bool GetLUXNeutronCollimator() { return luxNeutronCollimator; };
	        void SetLUXNeutronCollimatorHeight( G4double val )
				{ luxNeutronCollimatorHeight = val; };
		G4double GetLUXNeutronCollimatorHeight() { return luxNeutronCollimatorHeight; };
	        void SetLUXSourceTubeGap( G4double val )
				{ luxSourceTubeGap = val; };
		G4double GetLUXSourceTubeGap() { return luxSourceTubeGap; };

		G4bool GetLUXDDPencilBeam() {return luxDDPencilBeam;};
		void SetLUXDDPencilBeam( G4bool val ) {luxDDPencilBeam = val;};

		void SetLZFlexMass( G4double );
		G4double GetLZFlexMass() { return lzFlexMass; };
		void SetLZFlexXeSkinThickness( G4double );
		G4double GetLZFlexXeSkinThickness() { return lzFlexXeSkinThickness; };
		void SetLZFlexTeflonThickness( G4double );
		G4double GetLZFlexTeflonThickness() { return lzFlexTeflonThickness; };
		void SetLZFlexScintThickness( G4double );
		G4double GetLZFlexScintThickness()
				{ return lzFlexScintillatorThickness; };
		void SetLZSimpleNeutronCalTube( G4bool val )
				{ lzSimpleNeutronCal = val; };
		G4bool GetLZSimpleNeutronCalTube() { return lzSimpleNeutronCal; };
        void SetLZSimpleNeutronCalTopTube( G4bool val )
                { lzSimpleNeutronCalTop = val; };
		G4bool GetLZSimpleNeutronCalTopTube() { return lzSimpleNeutronCalTop; };
		void SetLZSimpleNeutronCalTubeHeight( G4double val )
				{ lzSimpleNeutronCalHeight = val; };
		G4double GetLZSimpleNeutronCalTubeHeight() { return lzSimpleNeutronCalHeight; };
		void SetLZSimpleNeutronCalTubeYPosition( G4double val )
				{ lzSimpleNeutronCalYPosition = val; };
		G4double GetLZSimpleNeutronCalTubeYPosition() { return lzSimpleNeutronCalYPosition; };
		void SetLZSimpleCalSourceHeight( G4double val )
				{ lzSimpleCalSourceHeight = val; UpdateGeometry(); };
		G4double GetLZSimpleCalSourceHeight()
				{ return lzSimpleCalSourceHeight; };

                void SetCavernRockSelection( G4String sel )
                                { cavernRockSelection = sel; /*UpdateGeometry();*/ };
                G4String GetCavernRockSelection() { return cavernRockSelection; };

                // For YBe pig
                void SetLZYBePig( G4bool val )
				{ lzYBePig = val; };
                G4bool GetLZYBePig() { return lzYBePig; };
	        void SetLZYBePigDiameter( G4double val )
				{ lzYBePigDiameter = val; };
		G4double GetLZYBePigDiameter() { return lzYBePigDiameter; };
	        void SetLZYBePigHeight( G4double val )
				{ lzYBePigHeight = val; };
		G4double GetLZYBePigHeight() { return lzYBePigHeight; };

		G4double GetElectricFieldGas() {return eFieldGas;};
		G4double GetElectricFieldGasWindow() {return eFieldGasWindow;};
		G4double GetElectricFieldGasTop() {return eFieldGasTop;};
		G4double GetElectricFieldGasAnode() {return eFieldGasAnode;};
		G4double GetElectricFieldLiquid() {return eFieldLiq;};
		G4double GetElectricFieldLiquidSurface() {return eFieldLiqSurface;};
		G4double GetElectricFieldLiquidGate() {return eFieldLiqGate;};
		G4double GetElectricFieldLiquidCathode() {return eFieldLiqCathode;};
		G4double GetElectricFieldLiquidBottom() {return eFieldLiqBottom;};
		
		void SetTopPMTBankVoltage( G4double v );
		G4double GetTopPMTBankVoltage() { return topPMTBankVoltage; };
		void SetTopGridVoltage( G4double v );
		G4double GetTopGridVoltage() { return topGridVoltage; };
		void SetAnodeGridVoltage( G4double v );
		G4double GetAnodeGridVoltage() { return anodeGridVoltage; };
		void SetGateGridVoltage( G4double v );
		G4double GetGateGridVoltage() { return gateGridVoltage; };
		void SetCathodeGridVoltage( G4double v );
		G4double GetCathodeGridVoltage() { return cathodeGridVoltage; };
		void SetBottomGridVoltage( G4double v );
		G4double GetBottomGridVoltage() { return bottomGridVoltage; };
		void SetBottomPMTBankVoltage( G4double v );
		G4double GetBottomPMTBankVoltage() { return bottomPMTBankVoltage; };
		
		void PrintElectricFields();
		
		void RecalculateFields();

                void SetEFieldFromFile ( G4bool v ) { EFieldFromFile = v; };
                G4bool GetEFieldFromFile () { return EFieldFromFile; };
                void SetEFieldFile ( G4String v ) { EFieldFile = v; };
                G4String GetEFieldFile () { return EFieldFile; };

                void SetDriftTimeFromFile ( G4bool v ) { DriftTimeFromFile = v; };
                G4bool GetDriftTimeFromFile () { return DriftTimeFromFile; };
                void SetDriftTimeFile ( G4String v ) { DriftTimeFile = v; };
                G4String GetDriftTimeFile () { return DriftTimeFile; };

                void SetRadialDriftFromFile ( G4bool v ) { RadialDriftFromFile = v; };
                G4bool GetRadialDriftFromFile () { return RadialDriftFromFile; };
                void SetRadialDriftFile ( G4String v ) { RadialDriftFile = v; };
                G4String GetRadialDriftFile () { return RadialDriftFile; };

		void SetLUXDoublePheRateFromFile( G4bool val )
				{ luxDoublePheRateFromFile = val; };
		G4bool GetLUXDoublePheRateFromFile()
				{ return luxDoublePheRateFromFile; };

		void SetLUXFastSimSkewGaussianS2( G4bool val )
				{ luxFastSimSkewGaussianS2 = val; };
		G4bool GetLUXFastSimSkewGaussianS2()
				{ return luxFastSimSkewGaussianS2; };
  
         //	Source methods
        void SetSource( G4String );
        void SetPrintEventList( G4bool sel ) { printEventList = sel; };
        void ResetSources();
        void BuildEventList();
        void TrimEventList();
        void PrintEventList() { recordTree->PrintNodes(); };
        void GenerateEvent( G4GeneralParticleSource*, G4Event* );
        void GenerateEventList();
      	G4double GetTotalSimulationActivity() { return totalSimulationActivity;};
        G4bool GetLUXSimSources() { return hasLUXSimSources;};
        void RecordTreeInsert(Isotope*, G4double, G4ThreeVector, G4int, G4int);
        G4double GetWindowEndTime() {return windowEnd;};

        G4double GetGammaXFiducialR() {return gammaXFiducialR;}
        G4double GetGammaXFiducialLoZ() {return gammaXFiducialLoZ;}
        G4double GetGammaXFiducialHiZ() {return gammaXFiducialHiZ;}
        G4double GetGammaXReverseFieldR() {return gammaXReverseFieldR;}
        G4double GetGammaXReverseFieldLoZ() {return gammaXReverseFieldLoZ;}
        G4double GetGammaXReverseFieldHiZ() {return gammaXReverseFieldHiZ;}
        G4double GetGammaXMaxEnergy() {return gammaXMaxEnergy;}

        G4double GetTwoElectronsDeltaZMax() {return twoElectronsDeltaZMax;}
        G4double GetTwoElectronsEmin() {return twoElectronsEmin;}
        G4double GetTwoElectronsEmax() {return twoElectronsEmax;}

        void LoadEventsFile(G4String);
        G4int NextEventToGenerate();
        G4int NextParticleIDToGenerate();
        G4double NextEnergyDepToGenerate();
        G4ThreeVector NextPositionToGenerate();

        G4bool GetG4DecayBool(){ return g4decaybool; };
        void SetG4DecayBool(G4bool val){ g4decaybool = val; };

        std::vector<G4int> GetRadioIsotopeList(){ return radioIsotopeList; };
        void SetRadioIsotopeList(std::vector<G4int> val){ radioIsotopeList=val; SetRadioIsotopeMap(val); };
        std::map<G4int, G4bool> GetRadioIsotopeMap(){ return radioIsotopeMap; };
        void SetRadioIsotopeMap(std::vector<G4int> val){ for(unsigned int ii=0; ii<val.size(); ii++) 
	    radioIsotopeMap[val[ii]]=false; };
        void UpdateRadioIsotopeMap(std::map<G4int, G4bool> val){ radioIsotopeMap=val; };


		//	Data methods

		//	Simulation data methods and variables
		struct stepRecord {
			G4int stepNumber;
			G4int particleID;
			G4String particleName;
			G4String creatorProcess;
            G4String stepProcess;
			G4int trackID;
			G4int parentID;
			G4double particleEnergy;
			G4double particleDirection[3];
			G4double energyDeposition;
			G4double position[3];
			G4double stepTime;
		};
		void AddDeposition( LUXSimDetectorComponent*, stepRecord );
		G4bool KillPhoton( LUXSimDetectorComponent* );
		void RecordValues( G4int );
		void RecordValuesOptPhot( G4int );
		void RecordValuesThermElec( G4int );
		void ClearRecords();
		
		//	Primary particle information
		struct primaryParticleInfo {
			G4String id;
			G4double energy;
			G4double time;
			G4ThreeVector position;
			G4ThreeVector direction;
		};
		void AddPrimaryParticle( primaryParticleInfo particle )
				{ primaryParticles.push_back( particle );}; 
		std::vector<primaryParticleInfo> GetPrimaryParticles()
				{ return primaryParticles; };

		//	Physics list methods
		inline G4bool GetUseOpticalProcesses() { return useOpticalProcesses; };
        void SetUseOpticalProcesses( G4bool );
        
        inline void SetS1Gain( G4double val ) { s1gain = val; };
        inline G4double GetS1Gain() { return s1gain; };
        inline void SetS2Gain( G4double val ) { s2gain = val; };
        inline G4double GetS2Gain() { return s2gain; };
		
		inline void SetDriftElecAttenuation( G4double val )
				{ driftElecAttenuation = val; };
		inline G4double GetDriftElecAttenuation()
				{ return driftElecAttenuation; };
		
		//	Materials methods
		void SetLXeTeflonRefl( G4double r );
		void SetLXeSteelRefl( G4double r );
		void SetLXeAbs( G4double l );
		void SetLXeRayleigh( G4double l );
		void SetGXeTeflonRefl( G4double r );
		void SetGXeSteelRefl( G4double r );
		void SetGXeAbs( G4double l );
		void SetGXeRayleigh( G4double l );
		void SetAlUnoxQuartzRefl( G4double r );

		inline G4bool GetOpticalDebugging()
				{ return LUXSimMat->GetOpticalDebugging(); };
		void SetOpticalDebugging( G4bool debug )
				{ LUXSimMat->SetOpticalDebugging(debug); };
		

	private:
		static LUXSimManager *LUXManager;
		LUXSimMessenger *LUXMessenger;
		
		LUXSimPhysicsList *LUXSimPhysics;
		LUXSimPhysicsOpticalPhysics *LUXSimPhysicsOptical;
		LUXSimPhysicsMuonNuclear *LUXSimPhysicsMuNuc;
		LUXSimDetectorConstruction *LUXSimDetector;
		LUXSimMaterials *LUXSimMat;
		LUXSimPrimaryGeneratorAction *LUXSimGenerator;
		LUXSimRunAction *LUXSimRun;
		LUXSimEventAction *LUXSimEvent;
		LUXSimSteppingAction *LUXSimStep;
		LUXSimOutput *LUXSimOut;
		LUXSimSourceCatalog *LUXSimSourceCat;
	
		G4UImanager *UI;
		
		CLHEP::MTwistEngine randomizationEngine;
		G4int randomSeed;
		
		//	Input/output variables
        G4bool   IsSVNRepo;
        G4bool   IsGitRepo;
		G4String compilationDir;
		G4String outputDir;
		G4String outputName;
		G4String historyFile;
		G4String listOfCommands;
		G4String listOfDiffs;
		G4bool runEndedCleanly;
		G4int numEvents;
		G4bool alwaysRecordPrimary;
		G4int eventProgressFrequency;
        G4double use100keVHack;
        G4double liquidXenonTotalEnergy;
        G4int eventCount;

        // User defined variables
        G4double userVar1;
        G4double userVar2;
        G4double userVar3;
        G4double userVar4;
        G4double userVar5;
		
		//	Geometry variables
		std::vector<LUXSimDetectorComponent*> luxSimComponents;
		G4String detectorSelection;
        G4bool checkVolumeOverlaps;
		G4String muonVetoSelection;
		G4String LZVetoSelection;
		G4String cryoStandSelection;
		G4String gridWiresSelection;
		G4bool useRealPMTNumberingScheme;
		
		G4double collimator_height;
		G4double collimator_hole;
		G4double collimatorSourceDiameter;
		G4double detZ;
		G4bool gasRun;
		G4bool luxSurfaceGeometry;
		G4bool luxNeutronCollimator;
		G4double luxNeutronCollimatorHeight;
		G4double luxSourceTubeGap;
                G4bool luxDDPencilBeam;
		G4bool numGNARRLIPMTFlag;
		G4double lzFlexMass;
		G4double lzFlexXeSkinThickness;
		G4double lzFlexTeflonThickness;
		G4double lzFlexScintillatorThickness;
		G4bool lzSimpleNeutronCal;
        G4bool lzSimpleNeutronCalTop;
                G4double lzSimpleNeutronCalHeight;
                G4double lzSimpleNeutronCalYPosition;
		G4double lzSimpleCalSourceHeight;
                G4bool lzYBePig;
                G4double lzYBePigDiameter;
                G4double lzYBePigHeight;

		G4double eFieldGas, eFieldGasWindow, eFieldGasTop, eFieldGasAnode;
        G4double eFieldLiq, eFieldLiqSurface, eFieldLiqGate, eFieldLiqCathode;
        G4double eFieldLiqBottom;
		
        G4double topPMTBankVoltage;
        G4double topGridVoltage;
        G4double anodeGridVoltage;
        G4double gateGridVoltage;
        G4double cathodeGridVoltage;
        G4double bottomGridVoltage;
        G4double bottomPMTBankVoltage;


        G4bool EFieldFromFile;

        G4String EFieldFile;

        double xyzDependentEField[NumPlanesX][NumPlanesY][NumPlanesZ][NumxyzDependentEFieldVars];

// XYZ-dependent electric field calculated by Lucie


  //      G4bool EFieldFromFile;
   //     G4String EFieldFile;
 //       G4double xyzDependentEField[251][598]; // XYZ-dependent electric field calculated from COMSOL

        G4bool DriftTimeFromFile;
        G4String DriftTimeFile;
        G4double xyzDependentDriftTime[251][598]; // XYZ-dependent drift time calculated from COMSOL

        G4bool RadialDriftFromFile;
        G4String RadialDriftFile;
        G4double xyzDependentRadialDrift[251][598]; // XYZ-dependent radial drift calculated from COMSOL

        G4bool luxDoublePheRateFromFile;

        G4bool luxFastSimSkewGaussianS2;

        G4String cavernRockSelection;

		//	Source variables
        struct volumeSource {
          LUXSimDetectorComponent *component;
          G4double totalVolumeActivity;
          G4double totalVolumeActivityRatio;
        };
        std::vector<volumeSource> sourceByVolume;
        G4double totalSimulationActivity;
        G4double windowStart, windowEnd;
        G4bool hasLUXSimSources, isEventListBuilt;
        G4bool hasDecayChainSources, printEventList;
        LUXSimBST* recordTree;

        G4double gammaXFiducialR;
        G4double gammaXFiducialLoZ;
        G4double gammaXFiducialHiZ;
        G4double gammaXReverseFieldR;
        G4double gammaXReverseFieldLoZ;
        G4double gammaXReverseFieldHiZ;
        G4double gammaXMaxEnergy;

        G4double twoElectronsDeltaZMax;
        G4double twoElectronsEmin;
        G4double twoElectronsEmax;

		std::vector<primaryParticleInfo> primaryParticles;

        G4bool g4decaybool;
        std::vector<G4int> radioIsotopeList;
        std::map<G4int,G4bool> radioIsotopeMap;
  
		//	Physics list variables
		G4bool useOpticalProcesses;
		G4bool opticalDebugging;
        G4double s1gain;
        G4double s2gain;
		G4double driftElecAttenuation;

        // for evnets file generator
        std::queue<G4int> qEvtN;
        std::queue<G4int> qParticleID;
        std::queue<G4double> qEnergyDep_keV;
        std::queue<G4double> qPositionX_cm;
        std::queue<G4double> qPositionY_cm;
        std::queue<G4double> qPositionZ_cm;
        G4int currentEvtN;
        std::queue<G4int> currentParticleID;
        std::queue<G4double> currentEnergyDep_keV;
        std::queue<G4double> currentX_cm;
        std::queue<G4double> currentY_cm;
        std::queue<G4double> currentZ_cm;


};

#endif
