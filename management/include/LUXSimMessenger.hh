////////////////////////////////////////////////////////////////////////////////
/*	LUXSimMessenger.hh
*
* This is the header file to define the LUXSim messenger commands. The idea
* behind this class is that all commands get routed and stored in this
* management class, whether those commands are related to the geometry, physics
* list, output specification, or whatever. In this way, all parts of the
* simulation have access to all simulation settings without having to re-write
* any code.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	7 April 2009 - Added the LUXSimMuonVeto command (Melinda)
*	20 April 2009 - Added the LUXSimSourceSet command (Kareem)
*	27 July 2009 - Added the physics list directory and the photons command
*				   (Kareem)
*	28 July 2009 - Added variables for the output directory (Kareem)
*	14-Sep-09 - Added commands to control the optical photon record level
*				(Kareem)
*	28-Sept-09 -  Added the LUXSimCryoStand command (Melinda)
*	29-Sept-09 - Added the LUXSimGridWires command (Melinda)
*	2 -Apr -10 - Added the sources reset command (Nick)
*	26-Jul-10 - Added a command to set the basename of the output file (Chao)
*	20-Sep-10 - Added an optical debug command (Kareem)
*	27-Nov-10 - Added always record primary command (Kareem)
*   21-Jul-11 - Added Decay Chain Print option (Nick)
*	24-Aug-11 - Added LUXSimGNARRLIPMTCommand (Tenzing)
*   21-Sep-11 - Electric field set methods(Nick)
*	18-Jan-12 - Added commands for altering materials properties (Dave)
*	24-Feb-12 - Added LZFlex controls: total mass, Xe skin thickness, and
*				scintillator thickness (Kareem)
*	06-Mar-12 - Added functions for setting LZFlex Z position (Dave)
*	09-Mar-12 - Renamed Z position variables, to indicate that they 
*				work with all detectors (Dave)
*	09-Mar-12 - Added Rayleigh scattering UI hooks (Dave)
*   24-Mar-12 - Added support for the event progress report UI hooks (Mike)
*	01-May-12 - Added a command to set the thermal electron record level
*				(Kareem)
*	12-Jul-12 - Added separate E-fields for all regions (Nick&Matt)
*	13-Aug-12 - Added switch to use old v. real PMT numberings (Rich)
*	25-Sep-12 - Added switch to toggle between the surface and underground LUX
*				geometries (Kareem)
*	25-Sep-12 - Added switch to toggle gas-only mode (Dave)
*	12-Dec-06 - Replaced E-field commands with commands to set grid voltages
*				and added electric field printing command (Dave)
*   04-Feb-13 - Added command for setting the S1 and S2 gains (Kareem)
*	22-Feb-13 - Added command for setting the drifting electron attenuation
*				distance (Kareem)
*	18-Mar-13 - Added the LUXSimCollimatorSourceDiameterCommand variable
*				(Kareem)
*	23-Apr-13 - Added the LUXSimLZNeutronCalibrationTubeCommand variable
*				(Kareem)
*	26-Apr-13 - Added a command to control the height of the calibration source
*				in the LZSimple geometry (Kareem)
*   23-Jun-13 - Added the LUXSimLZNeutronCalibrationTubeTopCommand variable
*               (Kareem)
*   18-Jul-13 - Added the LUXSimLUXNeutronCollimatorCommand and
*               the LUXSimNeutronCollimatorHeightCommand (Kevin)
*   03-Dec-13 - Added commands for setting the voltage on the top and bottom
*               PMT banks (Kareem)
*   07-Apr-14 - Added 100keVHack switch (Kareem)
*   14-Apr-14 - Added commands to use an XYZ-dependent drift time and radial
*               drift from a file. (Kevin)
*   16-May-14 - Added commands to change location of LZ DD neutron calibration
*      	       	tube (Kevin)
*   21-May-14 - Changed the variable type of the 100keVHack command (Kareem)
*   28-May-14 - Added flag to turn on/off a new LZVeto system (Scott Ha.)
*   03-Sep-14 - Added flag for checking overlaps for all volumes (automatic
*               checking off by default) (Kareem)
*   26-Sep-14 - Added option to change YBe pig height and diameter (Kevin)
*   14-Oct-14 - Added component-wise SetMass and SetVolume commands (Kareem)
*   02-Feb-15 - Added pencil beam command for the LUX DD generator (Kevin)
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimMessenger_HH
#define LUXSimMessenger_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4UImessenger.hh"

//
//	Class forwarding
//
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
//class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithADoubleAndUnit;
//class G4UIcmdWith3Vector;
class G4UIcmdWithADouble;

class LUXSimManager;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimMessenger : public G4UImessenger
{
	public:
		LUXSimMessenger( LUXSimManager* );
		~LUXSimMessenger();
		
		void SetNewValue( G4UIcommand*, G4String );
	
	private:
		LUXSimManager *luxManager;
	
		//	General commands
		G4UIdirectory				*LUXSimDir;
		G4UIcmdWithAnInteger		*LUXSimBeamOnCommand;
		G4UIcmdWithAnInteger		*LUXSimRandomSeedCommand;
		
		//	Input/output commands
		G4UIdirectory				*LUXSimFileDir;
		G4UIcmdWithAString			*LUXSimOutputDirCommand;
		G4UIcmdWithAString			*LUXSimOutputNameCommand;
		G4UIcmdWithABool			*LUXSimAlwaysRecordPrimaryCommand;
        G4UIcmdWithAnInteger        *LUXSimEventProgressCommand;
        G4UIcmdWithADoubleAndUnit   *LUXSim100keVHackCommand;

        // User defined variables commands.
		G4UIcmdWithADouble			*LUXSimUserVar1Command;
		G4UIcmdWithADouble			*LUXSimUserVar2Command;
		G4UIcmdWithADouble			*LUXSimUserVar3Command;
		G4UIcmdWithADouble			*LUXSimUserVar4Command;
		G4UIcmdWithADouble			*LUXSimUserVar5Command;
	
		//	Geometry commands
		G4UIdirectory				*LUXSimDetectorDir;
		G4UIcmdWithoutParameter		*LUXSimUpdateCommand;
		G4UIcmdWithAString	       	*LUXSimDetectorSelectCommand;
        G4UIcmdWithABool            *LUXSimVolumeOverlapCheckCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimDetectorZCommand;
        G4UIcmdWithAString          *LUXSimComponentSetMassCommand;
        G4UIcmdWithAString          *LUXSimComponentSetVolumeCommand;
		G4UIcmdWithAString	    	*LUXSimMuonVetoCommand;
		G4UIcmdWithAString	    	*LUXSimLZVetoCommand;
		G4UIcmdWithAString   		*LUXSimCryoStandCommand;
		G4UIcmdWithAString			*LUXSimGridWiresCommand;
		G4UIcmdWithAString			*LUXSimPMTNumberingCommand;
		G4UIcmdWithAString			*LUXSimRecordLevelCommand;
		G4UIcmdWithAString			*LUXSimRecordLevelOptPhotCommand;
		G4UIcmdWithAString			*LUXSimRecordLevelThermElecCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimCollimatorHeightCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimCollimatorHoleCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimCollimatorSourceDiameterCommand;
		G4UIcmdWithABool			*LUXSimLUXSurfaceGeometryCommand;
		G4UIcmdWithABool			*LUXSimLUXNeutronCollimatorCommand;
		G4UIcmdWithABool			*LUXSimLUXDDPencilBeamCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLUXNeutronCollimatorHeightCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLUXSourceTubeGapCommand;
		G4UIcmdWithABool	       	*LUXSimGNARRLIPMTCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLZFlexMassCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLZFlexXeSkinCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLZFlexTeflonThicknessCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLZFlexScintThicknessCommand;
		G4UIcmdWithABool			*LUXSimLZNeutronCalibrationTubeCommand;
		G4UIcmdWithABool			*LUXSimLZNeutronCalibrationTubeTopCommand;
		G4UIcmdWithADoubleAndUnit   *LUXSimLZNeutronCalibrationTubeHeightCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLZNeutronCalibrationTubeYPositionCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLZCalibrationSourceHeightCommand;
		G4UIcmdWithABool			*LUXSimLZYBePigCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLZYBePigDiameterCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLZYBePigHeightCommand;
        G4UIcmdWithADoubleAndUnit   *LUXSimTopPMTBankVoltageCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimTopGridVoltageCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimAnodeGridVoltageCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimGateGridVoltageCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimCathodeGridVoltageCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimBottomGridVoltageCommand;
        G4UIcmdWithADoubleAndUnit   *LUXSimBottomPMTBankVoltageCommand;
		G4UIcmdWithoutParameter		*LUXSimPrintElectricFieldsCommand;
		G4UIcmdWithABool			*LUXSimEFieldFromFileCommand;
		G4UIcmdWithAString			*LUXSimEFieldFileCommand;
		G4UIcmdWithABool			*LUXSimDriftTimeFromFileCommand;
		G4UIcmdWithAString			*LUXSimDriftTimeFileCommand;
		G4UIcmdWithABool			*LUXSimRadialDriftFromFileCommand;
		G4UIcmdWithAString			*LUXSimRadialDriftFileCommand;
		G4UIcmdWithABool			*LUXSimGasRunCommand;

		G4UIcmdWithABool			*LUXSimLUXDoublePheRateFromFileCommand;

		G4UIcmdWithABool			*LUXFastSimSkewGaussianS2Command;
                G4UIcmdWithAString                      *LUXSimCavernRockCommand;
  
		//	Source commands
		G4UIdirectory				*LUXSimSourceDir;
		G4UIcmdWithAString			*LUXSimSourceSetCommand;
		G4UIcmdWithoutParameter		*LUXSimSourceResetCommand;
		G4UIcmdWithABool         	*LUXSimSourcePrintCommand;
		
		//	Physics list commands
		G4UIdirectory				*LUXSimPhysicsListDir;
		G4UIcmdWithABool			*LUXSimOpticalPhotonsCommand;
		G4UIcmdWithABool			*LUXSimOpticalDebugCommand;
        G4UIcmdWithADouble          *LUXSimS1GainCommand;
        G4UIcmdWithADouble          *LUXSimS2GainCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimDriftingElectronAttenuationCommand;
		
		//	Materials commands
		G4UIdirectory				*LUXSimMaterialsDir;	
		G4UIcmdWithADouble			*LUXSimLXeTeflonReflCommand;
		G4UIcmdWithADouble			*LUXSimLXeSteelReflCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLXeAbsCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimLXeRayleighCommand;
		G4UIcmdWithADouble			*LUXSimGXeTeflonReflCommand;
		G4UIcmdWithADouble			*LUXSimGXeSteelReflCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimGXeAbsCommand;
		G4UIcmdWithADoubleAndUnit	*LUXSimGXeRayleighCommand;
		G4UIcmdWithADouble			*LUXSimAlUnoxQuartzReflCommand;

};

#endif
