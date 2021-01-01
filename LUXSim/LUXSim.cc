////////////////////////////////////////////////////////////////////////////////
/*	LUXSim.cc
*
* This is the main code file for LUXSim.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	27-Aug-09 - Now properly deletes the LUXSimManager class at the end of the
*				program (Kareem)
*	22-Jan-10 - Added code to look for the "exit" command as the last command
*				in the commands file, to allow for proper closing of the
*				executable (Kareem)
*	23-Feb-10 - Restructured macro-executing code to not need an input macro 
*				file (i.e. no longer generates an error when it doesn't find
*				vis.mac) (Dave)
*				Removed LUXSimOutput creation -- will be created on each call
*				to LUXSimManager::BeamOn (Dave)
*	27-Sep-10 - Minor code cleanup, and added LUXSimMaterials as a standalone
*				singleton class (Kareem)
*	08-Mar-12 - Now sets the compilation directory at compilation time (i.e.,
*				not run time) (Kareem)
*	09-Nov-12 - Added checks for Geant4 version, and if you're running prior to
*				4.9.4.p00, it automatically quits. If you're running something
*				other than 4.9.4.p04, 4.9.5.p01, or 4.9.5.p02, you have to
*				include a "-f" flag to force the sim to run (Kareem)
*	14-Nov-12 - Fixed a bug in the flag logic, where a file named "-f" would
*				try to load if a second parameter weren't included (Kareem)
*   28-Sep-15 - Added checks for SVN or Git repos (Kareem)
*       06-Oct-15 - Added StackingAction class to the run manager (David W)
*       18-Dec-15 - Added check to see if MUSUN generator is defined in the macro, if it is
*                               the Shielding physics list is built (David W) (merged into Git by Doug T)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4Version.hh"
#include "G4MonopolePhysics.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE_XM
#include "G4UIXm.hh"
#endif

//
//	LUXSim includes
//
#include "LUXSimMaterials.hh"
#include "LUXSimPhysicsList.hh"
#include "LUXSimDetectorConstruction.hh"
#include "LUXSimPrimaryGeneratorAction.hh"
#include "LUXSimRunAction.hh"
#include "LUXSimEventAction.hh"
#include "LUXSimSteppingAction.hh"
#include "LUXSimStackingAction.hh"
#include "LUXSimOutput.hh"
#include "LUXSimSourceCatalog.hh"
#include "LUXSimManager.hh"

#include <fstream>
#include <sstream>
G4String GetCommandValue(const char * macro_name, std::string cmd_name){
  ifstream infile(macro_name);
  if(!infile.is_open()){
    G4cerr<<macro_name<<" can not be opened."<<G4endl;
    return G4String("");
  }

  G4String val("");
  std::string line;
  size_t space = 0, content = 0;
  while(!infile.eof()){
    getline(infile, line);
    if(line.length()<2) continue;
    content=line.find_first_not_of(" \t");
    if(content==string::npos) continue;
    if(line[content]=='#') continue; //we ignore the case there is another # later in the line
    content=line.find(cmd_name, content);
    if(content==string::npos) continue;
    content=line.find_first_not_of(" \t", content+cmd_name.length());
    if(content==string::npos) continue;
    val.assign(line.substr(content));
    break;
  }//endwhile()
  infile.close();
  return val;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					main()
//------++++++------++++++------++++++------++++++------++++++------++++++------
int main( int argc, char **argv )
{
	cout << "\n\tProcessing eventA ";
		G4cout.flush();

	if( argc > 3 ) {
		G4cout << G4endl << G4endl << G4endl;
		G4cout << "You have supplied " << argc-1 << " arguments, but LUXSim "
			   << "can only handle two." << G4endl;
		G4cout << G4endl << G4endl << G4endl;
		
		exit(0);
	}

	if( G4VERSION_NUMBER < 940 ) {
		stringstream version;
		version << G4VERSION_NUMBER;
		G4String majorVersion = version.str().substr(0,1);
		G4String minorVersion = version.str().substr(1,1);
		G4String patchLevel = version.str().substr(2,1);

		G4cout << G4endl << G4endl << G4endl;
		G4cout << "You are running Geant4." << majorVersion << "."
			   << minorVersion << ".p0" << patchLevel << G4endl;
		G4cout << "LUXSim no longer operates with versions of Geant4 prior to "
			   << "4.9.4.p00" << G4endl;
		G4cout << G4endl << G4endl << G4endl;
		
		exit(0);
	}
	
	if( argc == 3 && !strcmp(argv[2],"-f") ) {
		strcpy( argv[2], argv[1] );
		strcpy( argv[1], "-f" );
	}
	
	if( G4VERSION_NUMBER != 944 && G4VERSION_NUMBER != 951 &&
			G4VERSION_NUMBER != 952 ) {
		//	The logic in this next line is aesthetically displeasing, but it's
		//	actually the most concise way to perform the required check.
		if( argc>1 && !strcmp(argv[1], "-f") )
			;
		else {
			
			G4cout << G4endl << G4endl << G4endl;
			G4cout << "You should run LUXSim with one of the following "
				   << "versions of Geant4:" << G4endl;
			G4cout << "\t4.9.4.p04" << G4endl;
			G4cout << "\t4.9.5.p01" << G4endl;
			G4cout << "\t4.9.5.p02" << G4endl;
			G4cout << G4endl;
			
			stringstream version;
			version << G4VERSION_NUMBER;
			G4String majorVersion = version.str().substr(0,1);
			G4String minorVersion = version.str().substr(1,1);
			G4String patchLevel = version.str().substr(2,1);
			G4cout << "If you insist on running with Geant4." << majorVersion
				   << "." << minorVersion << ".p0" << patchLevel
				   << ", you can force execution with the" << G4endl;
			G4cout << "'-f' flag." << G4endl;
			G4cout << G4endl << G4endl << G4endl;
			
			exit(0);
		}
	}

	// Check to see if the muon generator has been specified in the macro
	// If it is, tell LUXSim to build the shielding physics w/ muon-nuclear interactions
	// Otherwise, build the 'standard' LUXSim physics list
	G4bool useShielding = false;
	if (argc == 1 ) { // There's no macro file, so no chance for someone to turn on useShielding
		useShielding = false;
	}
	else {
		G4String commandFile = argv[1];
		//      Read in the commands file, and look for the MUSUN generator command
		ifstream commands(commandFile.c_str());
		commands.seekg(0, ios::end);
		int length = commands.tellg();
		commands.seekg(0, ios::beg);
		char *inputBuffer;
		inputBuffer = new char[length + 1];
		commands.read(inputBuffer, length);
		inputBuffer[length] = '\0';
		commands.close();
		G4String commandList = inputBuffer;
		delete[] inputBuffer;
		int iterator = 0;
		int strFirst = 0;
		int strLast = 0;
		while (iterator < commandList.size()) {
			iterator++;
			if (commandList.substr(iterator, 1) == "\n") {
				strLast = iterator;
				if (commandList.substr(strFirst, strLast - strFirst) == "/LUXSim/source/set MUSUN")
					useShielding = true;
				strFirst = strLast + 1;
			}
		}
	}
	if (useShielding == true && G4VERSION_NUMBER < 951)
	{
		if (argc >= 2 && !strcmp(argv[1], "-f")) {
			G4cout << endl << endl << "Geant4.9.4 cannot run the Shielding physics list that is optomized for Muons" << endl;
			cout << "Forcing it to run may produce unrealistic results, use at your own risk!" << endl;
			useShielding = false;
			cout << "Physics list reset to default... good luck!" << endl;
		}
		else {
			cout << endl << endl<<"Attempting to run MUSUN without the Shielding physics list is not advised!" << endl;
			cout << "Please upgrade to Geant4.9.5 or force it to run using the -f flag (not advised!!!!)" << endl;
			cout << "Exiting..." << endl;
			exit(0);
		}
	}

	//	Construct the default run manager
	G4RunManager* runManager = new G4RunManager;

	//	Instantiate and create all the required classes
	LUXSimManager *LUXManager = new LUXSimManager();
	
	//	For this next bit of code, COMPDIR is set at compilation time using the
	//	compilation switch -DCOMPDIR=\"`pwd`\". It is therefore a hard-coded
	//	string of where the executable was compiled. This is relevant for
	//	generating the SVN diffs if you're running the executable anywhere other
	//	than the top-level LUXSim directory.
	G4String compDir = COMPDIR;
	if( compDir.substr(compDir.length()-1,1) == "/" )
		compDir = compDir.substr( 0, compDir.length()-1 );
	compDir = compDir.substr( 0, compDir.find_last_of( "/" ) );
	LUXManager->SetCompilationDirectory( compDir );
	
    char repoTestCommand[100];
    sprintf(repoTestCommand, "ls -a %s/../ | grep svn", COMPDIR);
    LUXManager->SetIsSVNRepo( (bool) !bool( system( repoTestCommand ) ) );
    sprintf(repoTestCommand, "ls -a %s/../ | grep git", COMPDIR);
    LUXManager->SetIsGitRepo( (bool) !bool( system( repoTestCommand ) ) );

	LUXSimMaterials *LUXMaterials = new LUXSimMaterials();
	
	LUXSimPhysicsList *LUXSimPhysics = new LUXSimPhysicsList(useShielding);
	G4MonopolePhysics * theMonopole = new G4MonopolePhysics();
	//	G4cout<<"G4MonopolePhysics constructed."<<G4endl;
	//we can only update monople setup if there is a macro
	if(argc>1){
	  G4String monople_setup = GetCommandValue(argv[1], "/monopole/setup");
	  if(monople_setup.length()){
	    G4double q, m;
	    G4double mass;
	    G4String unts;
	    std::istringstream is(monople_setup);
	    is >> m >> q >> mass >> unts;
	    G4String unit = unts;
	    G4double vUnit = G4UIcommand::ValueOf(unit);
	    theMonopole->SetMagneticCharge(m);
	    theMonopole->SetElectricCharge(q);
	    theMonopole->SetMonopoleMass(mass*vUnit);
	    G4cout<<"*** Set Monopole charges (m, q, mass): "<<monople_setup<<G4endl;
	  }
	LUXSimPhysics->RegisterPhysics(theMonopole);
	//	G4cout<<"G4MonopolePhysics registered."<<G4endl;
	}
	else{
	  G4cerr<<"/monopole/setup command not found in"<<argv[1]<<G4endl;
	}
	runManager->SetUserInitialization( LUXSimPhysics );
	G4cout<<"Run Manager has set the physics initialization."<<G4endl;

	LUXSimDetectorConstruction *LUXSimDetector =
			new LUXSimDetectorConstruction();
	runManager->SetUserInitialization( LUXSimDetector );

	LUXSimPrimaryGeneratorAction *LUXSimGenerator =
			new LUXSimPrimaryGeneratorAction();
	runManager->SetUserAction( LUXSimGenerator );

	LUXSimRunAction *LUXSimRun = new LUXSimRunAction();
	runManager->SetUserAction( LUXSimRun );
	G4cout << "\tB" << G4endl; G4cout.flush();

	LUXSimEventAction *LUXSimEvent = new LUXSimEventAction();
	runManager->SetUserAction( LUXSimEvent );

	LUXSimSteppingAction *LUXSimStep = new LUXSimSteppingAction();
	runManager->SetUserAction( LUXSimStep );
	
	LUXSimStackingAction *LUXSimStack = new LUXSimStackingAction();
	runManager->SetUserAction( LUXSimStack );

	LUXSimSourceCatalog *LUXSimSourceCat = new LUXSimSourceCatalog();
	
	//	This next lines are kludges so that the compiler doesn't complain about
	//	unused variables.
	LUXManager = LUXManager;
	LUXSimSourceCat = LUXSimSourceCat;
	LUXMaterials = LUXMaterials;

	//	Set up the visualization
#ifdef G4VIS_USE
	G4VisManager *visManager = new G4VisExecutive;
	visManager->Initialize();
#endif

	// Initialize G4 kernel
	//	runManager->Initialize();
	
	//	Get the user interface manager
	G4UImanager *UI = G4UImanager::GetUIpointer();

	// Define (G)UI terminal for interactive mode
	G4UIsession *session = 0;
#ifdef G4UI_USE_TCSH
	session = new G4UIterminal(new G4UItcsh);      
#else
	session = new G4UIterminal();
#endif
	
	// Execute input macro, if specified
	if( argc == 1 || (argc == 2 && !strcmp(argv[1],"-f")) ) {
		session->SessionStart();
		delete session;	
	} else {
		G4String commandFile = argv[1];
		if( argc > 2 )
			commandFile = argv[2];
		G4String command = "/control/execute " + commandFile;
		UI->ApplyCommand(command);
	
		//	Read in the commands file, and if the last command is "exit", don't
		//	start the command session.
		ifstream commands( commandFile.c_str() );
		commands.seekg( 0, ios::end );
		int length = commands.tellg();
		commands.seekg( 0, ios::beg );
		char *inputBuffer;
		inputBuffer = new char[length+1];
		commands.read( inputBuffer, length );
		inputBuffer[length] = '\0';
		commands.close();
		G4String commandList = inputBuffer;
		delete [] inputBuffer;
		while( commandList.substr( commandList.length()-1 ) == "\n" ||
			   commandList.substr( commandList.length()-1 ) == "\t" ||
			   commandList.substr( commandList.length()-1 ) == " " )
			commandList = commandList.substr( 0, commandList.length()-1 );
		if( commandList.find_last_of("\n") < G4String::npos )
			commandList = commandList.substr( commandList.find_last_of("\n") );
		while( commandList.substr( 0,1 ) == "\n" ||
			   commandList.substr( 0,1 ) == "\t" ||
			   commandList.substr( 0,1 ) == " " )
			commandList = commandList.substr( 1 );
		if( commandList != "exit" ) {
			session->SessionStart();
			delete session;
		}
	}

	//	Clean up
#ifdef G4VIS_USE
	delete visManager;
#endif
	
	delete runManager;
	delete LUXManager;

	return 0;
}


