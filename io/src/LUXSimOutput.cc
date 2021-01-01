////////////////////////////////////////////////////////////////////////////////
/*	LUXSimOutput.cc
*
* This is the code file to control the LUXSim output. This output is solely to
* a general-purpose binary format, and should never be geared specifically
* toward either ROOT or Matlab. There will be separate projects to create ROOT-
* and Matlab-based readers for this binary format.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	10 April 2009 - added binary output (Chao Zhang)
*	14 April 2009 - Modified code to perform record checking on the call to
*					RecordEventByVolume, rather than in the method itself.
*					Consolidated common sections of code to outside the if()
*					statements. (Kareem)
*	27-Apr-09 - Removed the unit normalization from the output recording,
*				because this normlization is already handled in
*				LUXSimUserSteppingAction (Kareem)
*	27-May-09 - Changed the default DEBUGGING flag to "0", changed debugging
*				screen output to be dependent on the DEBUGGING flag, and basic
*				code maintenance (Kareem)
*	2-Jul-09  - 1. Random number is added to output file name
*				2. Added production date and geant4 version information to
*				output file (Chao)
*
*	12-Aug-09 - 1. Output directory is set to handle all the output files
*				2. Save the version of LUXSim and the name of the computer it's
*				being running to output file(Chao)  
*
*	28-Aug-09 - 1. For the variable is a string, write them as the exact size
*				into binary file
*				2. added Input-Commands, Code-Modification-Log and Detector-
*				Component-Lookup-Table into the output file (Chao)
*
*	1 -Sep-09 - 1. move the temporary file to directory io/temp/ and delete them
*				after using
*				2. replace and write all strings as c_str() instead of charactor
*				(Chao)
*
*	30-Sep-09 - Added optical photons in the output, convert photon's energy to
*				wavelength (Chao)
*	13-Oct-09 - Keep optical photons recordLevel independent from normal
*				recordLevel. Remove the convertion of photon's energy to
*				wavelength (Chao)
*	23-Feb-10 - Making file output name newly randomized (no longer gets same
*				random seed from LUXSimManager every time) -- will allow
*				creation of new binary file with each call to LUXSimOutput
*				(Dave)
*	26-Feb-10 - Making output write to a temporary file with ".bin.tmp" ending,
*				which is moved to ".bin" during destruction of LUXSimOutput
*				instance -- this identifies the file as still-being-written-to.
*				(Dave)
*	12-Mar-10 - Fixed bug in if statement that caused particles other than
*				optical photons to be saved when no recordLevel = 0 and
*				opticalRecordLevel = 3 (Melinda)
*	17-Mar-10 - Added field for number of records stored in file. (Dave)
*	5 -May-10 - Added primary particle information (Chao)
*	25-Jul-10 - Modified the output filename to use the random seed set within
*				the manager class, rather than creating its own random seed.
*				Also did a minor code format cleanup (Kareem)
*	26-Jul-10 - Added support for changing the base filename from "LUXOut" to
*				something specified by the user (Chao)
*	19-Aug-10 - Added a check to see if the run ended cleanly, and if not, do
*				not remove the ".tmp" extension. Minor code formatting. (Kareem)
*	28-Nov-10 - Added the record control for primary particle information. If
*				AlwaysRecordPrimary() is set to false and no energy depostion in
*				the volume, no records for primaries (Chao)
*	29-Nov-10 - Improved the record control for primary particle information
*				(Chao)
*	31-Jan-11 - Added support for record level 4 (Kareem)
*	02-Dec-11 - Output now records the creator process for record levels 2, 3,
*				and 4, and optical photon record levels 3 and 4 (Kareem)
*	08-Mar-12 - Removed the leading "io" from the "svn info" output file. Also
*				changed the command to acquire the svn info to reference the
*				compilation directory (Kareem)
*   01-May-12 - Added support for thermal electrons. (Chao)
*	02-May-12 - Fixed the issue of no record of the input history for an empty run. (Chao) 
*	20-May-13 - Added the emission time for primaries. (Chao)
*   02-Aug-13 - Cleaned up the output directory handling to avoid crashes (Kareem)
*   29-Apr-14 - The time stamp now records in local time instead of GMT (Kareem)
*   28-Sep-15 - Handle the case of the code being in an SVN or Git repo (Kareem)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	C/C++ includes
//
#include <sstream>

//
//	LUXSim includes
//
#include "LUXSimOutput.hh"
#include "LUXSimManager.hh"
#include "LUXSimDetectorComponent.hh"
#include "G4Version.hh"

#define DEBUGGING 0
//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimOutput()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimOutput::LUXSimOutput()
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
	
	std::stringstream RandSeed, TimeDate;
	stringstream TempName1;
	G4String SeedStr, TempName, TempNameTmp, OutDir, TempName2,
			 TempName3;
	char* OutName, * OutNameTmp;

	OutDir = luxManager->GetOutputDir();	 //get output directory
	if( OutDir.substr( OutDir.length() - 1, 1 ) == "/" )
	  OutDir = OutDir.substr( 0, OutDir.length() - 1 );
	struct stat st;
	if ( stat(OutDir.c_str(), &st) == -1 ) mkdir(OutDir.c_str(), 0777);	//	check, if not exist,
													//	create the folder
	
	// Create file name with the random number in it
	RandSeed << luxManager->GetRandomSeed();
	SeedStr = RandSeed.str();

	if( (luxManager->GetOutputName().length() > 0) &&
			(luxManager->GetOutputName() != "0") )
		TempName = OutDir + "/" + luxManager->GetOutputName() + SeedStr +".bin";
	else 
		TempName = OutDir + "/LUXOut" + SeedStr + ".bin";

	TempNameTmp = TempName + ".tmp"; // set name 
	OutName = new char[TempName.length()+1];
	TempName.copy(OutName,TempName.length(),0);
	OutName[TempName.length()]='\0';
	OutNameTmp = new char[TempNameTmp.length()+1];
	TempNameTmp.copy(OutNameTmp,TempNameTmp.length(),0);
	OutNameTmp[TempNameTmp.length()]='\0';
	
	// Set global file name
	fName = TempName;
	
	fLUXOutput.open(OutNameTmp, ios::out | ios::binary);
	delete[] OutName;
	
	// Set record size placeholder
	int placeholder = 0;
	fLUXOutput.write((char *)(&placeholder), sizeof(int));

	struct tm *gm;
	time_t t;
    char timeBuffer[20];
	t = time(NULL);
	gm = localtime(&t);						   //find production time
    strftime( timeBuffer, 20, "%Z", gm );
	TimeDate << asctime(gm);
	G4String gmt_head = timeBuffer;
    gmt_head += ": ";
	GMT = gmt_head + TimeDate.str();
	Size = GMT.length();
	fLUXOutput.write((char *)(&Size), sizeof(int));
	fLUXOutput.write((char *)(GMT.c_str()), Size);	

	G4Ver = G4Version;								// find G4 Version
	G4Ver = G4Ver.substr( G4Ver.find("Name:") + 6 );
	G4Ver = G4Ver.substr( 0, G4Ver.find(" $") );
	Size = G4Ver.length();
	fLUXOutput.write((char *)(&Size), sizeof(int));
	fLUXOutput.write((char *)(G4Ver.c_str()), Size);

	char * temp1;
	char * temp2;
	ifstream is;
	
    if ( luxManager->GetIsSVNRepo() ) {
        TempName = "/tmp/LUXSimInfo_" + SeedStr + ".txt";
        TempName1 << "svn info " << luxManager->GetCompilationDirectory() << " > "
        << TempName;
        //  TempName1 = "svn info > " + TempName;
        system(TempName1.str().c_str());
        is.open(TempName.c_str(), ios::binary );
        is.seekg (0, ios::end);
        Size = is.tellg();
        is.seekg (0, ios::beg);
        temp1 = new char [Size];
        is.read(temp1, Size);
        SimVer = temp1;
        SimVer = SimVer.substr(SimVer.find("Revision:"));  // find revision number
        SimVer = SimVer.substr(0,13);
        Size = SimVer.length();
        fLUXOutput.write((char *)(&Size), sizeof(int));
        fLUXOutput.write((char *)(SimVer.c_str()), Size);
        is.close();
        delete[] temp1;
    } else if ( luxManager->GetIsGitRepo() ) {
        TempName = "/tmp/LUXSimInfo_" + SeedStr + ".txt";
        TempName1 << "git rev-parse HEAD " << " > "
        << TempName;
        //  TempName1 = "svn info > " + TempName;
        system(TempName1.str().c_str());
        is.open(TempName.c_str(), ios::binary );
        is.seekg (0, ios::end);
        Size = is.tellg();
        is.seekg (0, ios::beg);
        temp1 = new char [Size];
        is.read(temp1, Size);
        SimVer = temp1;
        //          SimVer = SimVer.substr(SimVer.find("Revision:"));  // find revision number
        //          SimVer = SimVer.substr(0,13);
        Size = SimVer.length();
        fLUXOutput.write((char *)(&Size), sizeof(int));
        fLUXOutput.write((char *)(SimVer.c_str()), Size);
        is.close();
        delete[] temp1;
    } else {
        Size = 0;
        fLUXOutput.write((char *)(&Size), sizeof(int));
    }
    

	TempName2 = "uname -n > " + TempName;
	system(TempName2.c_str());	  // find name of computer
	is.open(TempName, ios::binary);
	is.seekg (0, ios::end);
	Size = is.tellg();
	is.seekg (0, ios::beg);
	temp2 = new char [Size+1];
	is.read(temp2, Size);
	temp2[Size] = '\0';
	is.close();
	uname = temp2;
	Size = uname.length();
	fLUXOutput.write((char *)(&Size), sizeof(int));
	fLUXOutput.write((char *)(uname.c_str()), Size);
	delete[] temp2;
	TempName3 = "rm -f " + TempName;
	system(TempName3.c_str());

	numRecords = 0;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimOutput()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimOutput::~LUXSimOutput()
{
	fLUXOutput.seekp(0, std::ios_base::beg);
	fLUXOutput.write((char *)(&numRecords), sizeof(int));
	
	fLUXOutput.close();
	
	// We're done writing to the file -- remove the .tmp suffix if the run
	// ended cleanly.
	if( luxManager->GetRunEndedCleanly() ) {
		G4String OutDir = luxManager->GetOutputDir();
		G4String command = "mv " + fName + ".tmp " + fName + " ";
		system( command.c_str() );
		
		G4cout << "\nOutput saved to " << fName << G4endl << G4endl;
	} else
		G4cout << "\nRun did not end cleanly, file name remains " << fName
			   << ".tmp" << G4endl;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                                      RecordInputHistory()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimOutput::RecordInputHistory()
{
		// this part should be done before the beamOn
                commands = luxManager->GetInputCommands();
                Size = commands.length();
                fLUXOutput.write((char *)(&Size), sizeof(int));
                fLUXOutput.write((char *)(commands.c_str()), Size);
                fLUXOutput.flush();

                differ = luxManager->GetDiffs();
                Size = differ.length();
                fLUXOutput.write((char *)(&Size), sizeof(int));
                fLUXOutput.write((char *)(differ.c_str()), Size);
                fLUXOutput.flush();

                DetCompo = luxManager->GetDetectorComponentLookupTable();
                Size = DetCompo.length();
                fLUXOutput.write((char *)(&Size), sizeof(int));
                fLUXOutput.write((char *)(DetCompo.c_str()), Size);
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					RecordEventByVolume()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimOutput::RecordEventByVolume( LUXSimDetectorComponent* component,
		G4int eventNum )
{
	//	Record level definitions
	//	0 - Do not record energy depositions
	//	1 - Record just total energy in the volume for each event
	//	2 - Record just the steps where energy was deposited
	//	3 - Record all steps, including steps where no energy was deposited
	//	4 - Record just the first step in the volume, but then kill the track.
	//		The processing for this step is done within UserSteppingAction, but
	//		the record level still needs to be taken into account here.

	//	Optical photons level definitions
	//	0 - Don't record any info (default)
	//	1 - Record only the total number of optical photons entering the 
	//		volume AND kill the track so the photons don't propagate
	//	2 - Record the total number of optical photons entering the volume, 
	//		but don't kill the tracks
	//	3 - Record all the info on the optical photons entering the volume AND 
	//		kill the track
	//	4 - Record all the info on the optical photons, but don't kill the 
	//		tracks

        //      Thermal electrons level definitions
        //      0 - Don't record any info (default)
        //      1 - Record only the total number of thermal electrons entering the 
        //              volume AND kill the track so the electron don't propagate
        //      2 - Record the total number of thermal electrons entering the volume, 
        //              but don't kill the tracks
        //      3 - Record all the info on the theraml electrons entering the volume AND 
        //              kill the track
        //      4 - Record all the info on the thermal electrons, but don't kill the 
        //              tracks

	///////////////////////calculate record size

        std::vector<LUXSimManager::stepRecord> eventRecord =
                        component->GetEventRecord();

        std::vector<LUXSimManager::primaryParticleInfo> primaryPar =
                        luxManager->GetPrimaryParticles();

        totalVolumeEnergy = 0.;
        totalOptPhotNumber = 0;
	totalThermElecNumber = 0;
        G4int recordsize2 = 0;
        G4int recordsize3 = 0;
        for( G4int i=0; i<(G4int)eventRecord.size(); i++ ){
                totalVolumeEnergy += eventRecord[i].energyDeposition;
                if (eventRecord[i].particleName == "opticalphoton"){
                        totalOptPhotNumber ++;
                }else if(eventRecord[i].particleName == "thermalelectron"){
                        totalThermElecNumber ++; 
		}else{
                        if ( eventRecord[i].energyDeposition > 0. ){
                                recordsize2 ++;
                        }
                        recordsize3 ++;
                }
        }

        //      if primary record set to false and there is no energy depositon
        //      in the interested volume, no primary information recorded.
   if ( totalVolumeEnergy > 0 || component->GetRecordLevel() > 2 
				|| luxManager->GetAlwaysRecordPrimary()){	
        ++numRecords;
	////  Primary particle information
	primaryParSize = (int) primaryPar.size();

        fLUXOutput.write((char *)(&primaryParSize),sizeof(int));
	if ( DEBUGGING ) G4cout<< "\n primaryParSize = "<< primaryParSize <<G4endl;
	for (int m = 0; m < primaryParSize; m++ ) {
		Size = primaryPar[m].id.length();
		fLUXOutput.write((char *)(&Size),sizeof(int));
		primaryParName = primaryPar[m].id;
		fLUXOutput.write((char *)(primaryParName.c_str()),Size);
		primaryParEnergy_keV = primaryPar[m].energy / keV;
		fLUXOutput.write((char *)(&primaryParEnergy_keV),sizeof(double));		
		primaryParTime_ns = primaryPar[m].time / ns;
                fLUXOutput.write((char *)(&primaryParTime_ns),sizeof(double));
		primaryParPos_mm[0] = primaryPar[m].position[0] / mm;
		fLUXOutput.write((char *)(&primaryParPos_mm[0]),sizeof(double));	
		primaryParPos_mm[1] = primaryPar[m].position[1] / mm;
		fLUXOutput.write((char *)(&primaryParPos_mm[1]),sizeof(double));
		primaryParPos_mm[2] = primaryPar[m].position[2] / mm;
		fLUXOutput.write((char *)(&primaryParPos_mm[2]),sizeof(double));
		primaryParDir[0] = primaryPar[m].direction[0];
		fLUXOutput.write((char *)(&primaryParDir[0]),sizeof(double));
		primaryParDir[1] = primaryPar[m].direction[1];
		fLUXOutput.write((char *)(&primaryParDir[1]),sizeof(double));
		primaryParDir[2] = primaryPar[m].direction[2];
		fLUXOutput.write((char *)(&primaryParDir[2]),sizeof(double));

		if( DEBUGGING ) {
			G4cout<<"primary_ID = "<< primaryParName <<G4endl;
			G4cout<<"primary_energy = "<< primaryParEnergy_keV <<" keV"
				  <<G4endl;
			G4cout<<"primary_time = "<< primaryParTime_ns<<" ns" <<G4endl;
			G4cout<<"primary_positionX = "<< primaryParPos_mm[0] <<" mm"
				  <<G4endl;
			G4cout<<"primary_positionY = "<< primaryParPos_mm[1] <<" mm"
				  <<G4endl;
			G4cout<<"primary_positionZ = "<< primaryParPos_mm[2] <<" mm"
				  <<G4endl;
			G4cout<<"primary_directionX = "<< primaryParDir[0] <<G4endl;
			G4cout<<"primary_directionY = "<< primaryParDir[1] <<G4endl;
			G4cout<<"primary_directionZ = "<< primaryParDir[2] <<G4endl;
		}
	}
	
	//	First handle information recording that is independent of the specific
	//	record level.
	optPhotRecordLevel = component->GetRecordLevelOptPhot();
	thermElecRecordLevel = component->GetRecordLevelThermElec();
	recordLevel = component->GetRecordLevel();
	fLUXOutput.write((char *)(&recordLevel),sizeof(int));
	fLUXOutput.write((char *)(&optPhotRecordLevel),sizeof(int));
        fLUXOutput.write((char *)(&thermElecRecordLevel),sizeof(int));
	volume = component->GetID();
	fLUXOutput.write((char *)(&volume),sizeof(int));
	fLUXOutput.write((char *)(&eventNum),sizeof(int));

	//	record steping information according to the specified record level
	//
	if( recordLevel>0) fLUXOutput.write((char *)(&totalVolumeEnergy),
			sizeof(double));
	if( optPhotRecordLevel >0) fLUXOutput.write((char *)(&totalOptPhotNumber),
			sizeof(int));
        if( thermElecRecordLevel >0) fLUXOutput.write((char *)(&totalThermElecNumber),
                        sizeof(int));
	fLUXOutput.flush();
	if( DEBUGGING ) {
		G4cout << G4endl;
		G4cout << "OpticalLevel, thermElecLevel, recordLevel, volume, evtN, Edep, NOptPho, NthermEle= "
			   << optPhotRecordLevel<<", "<<thermElecRecordLevel<<", "<<recordLevel << ", "
			   <<volume << ", " << eventNum << ", " << totalVolumeEnergy
			   <<", "<<totalOptPhotNumber<<", "<<totalThermElecNumber <<G4endl;
	}
	//	Record Level 1
	recordSize = 0;
	//	Record Level 2
	if( recordLevel == 2){
		recordSize = recordsize2;
	} else if( recordLevel > 2 ){
	//	  Record Level 3
		recordSize = recordsize3;
	}
	if (optPhotRecordLevel > 2 ) recordSize += totalOptPhotNumber;
	if (thermElecRecordLevel >2 ) recordSize += totalThermElecNumber;

	fLUXOutput.write((char *)(&recordSize),sizeof(int));
	fLUXOutput.flush();
	
	if( recordSize > 0 ) {
		for( G4int i=0; i<(G4int)eventRecord.size(); i++ ) {
			if( ( (eventRecord[i].particleName != "opticalphoton") && 
				(eventRecord[i].particleName != "thermalelectron") &&
				  ( (eventRecord[i].energyDeposition > 0 && recordLevel == 2) ||
					recordLevel >2 ) ) ||
				(optPhotRecordLevel > 2 &&
						eventRecord[i].particleName == "opticalphoton") ||
				(thermElecRecordLevel > 2 &&
                                                eventRecord[i].particleName == "thermalelectron") ) {

				particleName = eventRecord[i].particleName;
				particleNameSize = particleName.length();
				fLUXOutput.write((char *)(&particleNameSize), sizeof(int));
				fLUXOutput.write((char *)(particleName.c_str()),
						particleNameSize);

				creatorProcess = eventRecord[i].creatorProcess;
				creatorProcessSize = creatorProcess.length();
				fLUXOutput.write((char *)(&creatorProcessSize), sizeof(int));
				fLUXOutput.write((char *)(creatorProcess.c_str()),
						creatorProcessSize);
                
                stepProcess = eventRecord[i].stepProcess;
                stepProcessSize = stepProcess.length();
                fLUXOutput.write((char *)(&stepProcessSize), sizeof(int));
                fLUXOutput.write((char *)(stepProcess.c_str()),
                        stepProcessSize);
						
				data.stepNumber = eventRecord[i].stepNumber;
				data.particleID = eventRecord[i].particleID;
				data.trackID = eventRecord[i].trackID;
				data.parentID = eventRecord[i].parentID;
				data.particleEnergy = eventRecord[i].particleEnergy;
				data.particleDirection[0]=eventRecord[i].particleDirection[0];
				data.particleDirection[1]=eventRecord[i].particleDirection[1];
				data.particleDirection[2]=eventRecord[i].particleDirection[2];
				data.energyDeposition = eventRecord[i].energyDeposition;
				data.position[0]=eventRecord[i].position[0];
				data.position[1]=eventRecord[i].position[1];
				data.position[2]=eventRecord[i].position[2];
				data.stepTime= eventRecord[i].stepTime;
				
				fLUXOutput.write((char *)(&data),sizeof(data));


				fLUXOutput.flush();

				if (DEBUGGING) {
					G4cout << "sizeof(data) = " << sizeof(data) << G4endl;
					G4cout << "particleName, Size= " << particleName << ", "
						   << particleNameSize << G4endl;
					G4cout << "data.stepNumber= " << data.stepNumber << G4endl;
					G4cout << "data.particleID= " << data.particleID << G4endl;
					G4cout << "data.trackID= " << data.trackID << G4endl;
					G4cout << "data.parentID= " << data.parentID << G4endl;
					G4cout << "data.particleEnergy= " << data.particleEnergy
						   << G4endl;
					G4cout << "data.particleDirection= "
						   << data.particleDirection[0] << ", "
						   << data.particleDirection[1] << ", "
						   << data.particleDirection[2] << G4endl;
					G4cout << "data.energyDeposition= " << data.energyDeposition
						   << G4endl;
					G4cout << "data.position= " << data.position[0] << ", "
						   << data.position[1] << ", " << data.position[2]
						   << G4endl;
					G4cout << "data.stepTime= " << data.stepTime << G4endl;
					G4cout << "creatorProcess, Size= " << creatorProcess << ", "
						   << creatorProcessSize << G4endl;
					G4cout << G4endl << G4endl;
				}	
			}
		}
	}
	}
}
