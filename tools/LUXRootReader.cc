////////////////////////////////////////////////////////////////////////////////
/*	LUXRootReader.cc
*
* A reader which convert binary file into root file.
*
********************************************************************************
* Change log
*	15 March 2010 - Initial submission (Melinda Sweany): Made LUXRootReader.C
*				(Chao Zhang) a stand along program that accepts an argument:
*				First compile with gmake, and then run with filename as
*				parameter:
*				./LUXRootReader LUXOut2345436.bin
*				The .root file will appear in the same directory, with the same
*				filename
*				(Melinda)
*	31 March 2010 - Added new variable "inumRecords" to the header (Chao)
*	 1 April 2010 - Corrected the format of storage for the particle names
*					(Chao)
*	20 April 2010 - Changed to take .bin extention in input
*	 5 May	 2010 - Added primary particle information (Chao) 
*	17 Sep   2010 - Changed the output file name to be keyed on the extension
*					of the input file name, rather than the first period it
*					comes to (Kareem)
*	02 Dec   2011 - Added support for the creator process to the output file
*					(Kareem)
*	09 Feb   2012 - Upped the character limit from 15 to 25 (Kareem)
*	01 May	 2012 - Added support for thermal electrons. (Chao)
*   11 May   2012 - Added plugin line to prevent crash on certain systems.
*					(Matthew)
*	20 May	 2013 - Added emission time for primaries. (Chao)
*	23 May	 2013 - Fixed a bug where the creator process string was getting
*					truncated to the length of the particle name string (Kareem)
*   20 Aug   2015 - Added the step process to the reader (Kareem)
*   26 Aug   2015 - Added the volume name to each event in the output file. Note
*                   this was done by parsing the header, rather than by adding
*                   another field to the binary output file (Kareem)
*   28 Sep   2015 - Changed an SVN version check to a default value to avoid
*                   incompatability if the code isn't under SVN control (Kareem)
*   10 Feb   2016 - Set the has_emission_time flag permanently to true (Kareem)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	ROOT includes
//
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TText.h"
#include "TPluginManager.h"
//
//	C/C++ includes
//
#include <fstream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdlib>
//
//	Definitions
//
#define DEBUGGING 0

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
int main( int argc, char** argv){
	
	gROOT->GetPluginManager()->AddHandler( "TVirtualStreamerInfo", "*",
            "TStreamerInfo", "RIO", "TStreamerInfo()" );
	ifstream fin;
	char * filename = argv[1];
	fin.open(filename,ios::binary|ios::in);
	if(  !fin.is_open() ) {
		cout << "Couldn't find the file "<< filename << endl;
		exit( 0 );
	}

	TFile* fFile;
	TTree* fTree;
	TTree* fTree0;
	
	const int kMaxCharacter = 50;
	int cParticleNamePos, cCreatorProcessPos, cStepProcessPos;
	int iRecordSize;	
	int iEvtNb;
	int iVolume;
    char * cVolumeName;
	int iTotalOptPhotNumber=0;
	int iTotalThermElecNumber=0;

	int iPrimaryParNum;
	int cPrimaryParNamePos;
	char * cPrimaryParName;
	double * fPrimaryParEnergy_keV;
	double * fPrimaryParTime_ns;
	double * fPrimaryParPosX_mm;
	double * fPrimaryParPosY_mm;
	double * fPrimaryParPosZ_mm;
	double * fPrimaryParDirX;
	double * fPrimaryParDirY;
	double * fPrimaryParDirZ;

	int iNumRecords;
	char * productionTime;
	char * geant4Version;
	char * svnVersion;
	char * uName;
	char * InputCommands;
	char * diffs;
	char * DetCompo;

	int arraySize=1;

	int * iStepNum;
	int * iParticleID;
	char * cParticleName;
	char * particleName;
	char * strName;
	char * cCreatorProcess;
	char * creatorProcessName;
	char * creatorProcName;
    char * cStepProcess;
    char * stepProcessName;
    char * stepProcName;
	
	int * iTrackID;
	int * iParentID;
	double * fKEnergy_keV;
	double * fOptPhoWaveLength_nm;
	double * fDirectionX;
	double * fDirectionY;
	double * fDirectionZ;
	double * fEnergyDep_keV;
	double * fPositionX_cm;
	double * fPositionY_cm;
	double * fPositionZ_cm;
	double * fStepTime;
	double fTotEnergyDep_keV=0;

	string outfileString = filename;
	outfileString = outfileString.substr( 0, outfileString.find_last_of('.') );
	outfileString += ".root";
	fFile = TFile::Open(outfileString.c_str(), "RECREATE");
	fTree =  new TTree("tree", "tree");
	fTree0 =  new TTree("header", "header");

	fTree->Branch("iPrimaryParNum", &iPrimaryParNum, "iPrimaryParNum/I");
			// Total number of primary particles
	fTree->Branch("cPrimaryParName", cPrimaryParName, "cPrimaryParName/C");
			// The name of primary particles
	fTree->Branch("fPrimaryParEnergy_keV", fPrimaryParEnergy_keV,
			"fPrimaryParEnergy_keV[iPrimaryParNum]/D");
			// Kinetic Energy of primary particles
	fTree->Branch("fPrimaryParTime_ns", fPrimaryParTime_ns,
			"fPrimaryParTime_ns[iPrimaryParNum]/D");
			// Emission time of primary particles
	fTree->Branch("fPrimaryParPosX_mm", fPrimaryParPosX_mm,
			"fPrimaryParPosX_mm[iPrimaryParNum]/D");
			// PositionX of primary particles
	fTree->Branch("fPrimaryParPosY_mm", fPrimaryParPosY_mm,
			"fPrimaryParPosY_mm[iPrimaryParNum]/D");
			// PositionY of primary particles
	fTree->Branch("fPrimaryParPosZ_mm", fPrimaryParPosZ_mm,
			"fPrimaryParPosZ_mm[iPrimaryParNum]/D");
			// PositionZ of primary particles
	fTree->Branch("fPrimaryParDirX", fPrimaryParDirX,
			"fPrimaryParDirX[iPrimaryParNum]/D");
			// DirectionX of primary particles
	fTree->Branch("fPrimaryParDirY", fPrimaryParDirY,
			"fPrimaryParDirY[iPrimaryParNum]/D");
			// DirectionY of primary particles
	fTree->Branch("fPrimaryParDirZ", fPrimaryParDirZ,
			"fPrimaryParDirZ[iPrimaryParNum]/D");
			// DirectionZ of primary particles

	fTree->Branch("iRecordSize", &iRecordSize, "iRecordSize/I");
			// the total step recorded for the volume/event 
	fTree->Branch("iEvtN", &iEvtNb, "EvtN/I");  // Event Number: starts with 1 
	fTree->Branch("iV_name", &iVolume, "V_name/I"); // Volume name
    fTree->Branch("cVolumeName", &cVolumeName, "cVolumeName/C" );

	fTree->Branch("iStepNum", iStepNum, "iStepNum[iRecordSize]/I");
			// Step Number
	fTree->Branch("iParticleID", iParticleID, "iParticleID[iRecordSize]/I");
			// Particle ID
	fTree->Branch("cParticleName", &cParticleName, "cParticleName/C");
			// Particle Name

	fTree->Branch("iTrackID", iTrackID, "iTrackID[iRecordSize]/I"); // Track ID
	fTree->Branch("iParentID", iParentID, "iParentID[iRecordSize]/I");
			// Parent ID
	fTree->Branch("cCreatorProcess", &cCreatorProcess, "cCreatorProcess/C");
			// Creator process
    fTree->Branch("cStepProcess", &cStepProcess, "cStepProcess/C");
            //  Step process
	fTree->Branch("fKEnergy_keV", fKEnergy_keV, "fKEnergy_keV[iRecordSize]/D");
			// Kinetic Energy keV
	fTree->Branch("fOptPhoWaveLength_nm", fOptPhoWaveLength_nm,
			"fOptPhoWaveLength_nm[iRecordSize]/D");
			// OptPhoton Wavelength nm
	fTree->Branch("fDirectionX", fDirectionX, "fDirectionX[iRecordSize]/D");
			// Particle Direction X
	fTree->Branch("fDirectionY", fDirectionY, "fDirectionY[iRecordSize]/D");
			// Particle Direction Y
	fTree->Branch("fDirectionZ", fDirectionZ, "fDirectionZ[iRecordSize]/D");
			// Particle Direction Z
	fTree->Branch("fEnergyDep_keV", fEnergyDep_keV,
			"fEnergyDep_keV[iRecordSize]/D");
			// Energy Deposition keV
	fTree->Branch("fPositionX_cm", fPositionX_cm,
			"fPositionX_cm[iRecordSize]/D");
			// Particle Position X_cm
	fTree->Branch("fPositionY_cm", fPositionY_cm,
			"fPositionY_cm[iRecordSize]/D");
			// Particle Position Y_cm
	fTree->Branch("fPositionZ_cm", fPositionZ_cm,
			"fPositionZ_cm[iRecordSize]/D");
			// Particle Position Z_cm
	fTree->Branch("fStepTime", fStepTime, "fStepTime[iRecordSize]/D");
			// Step Time

	fTree->Branch("fTotEDep", &fTotEnergyDep_keV, "TotEDep/D");
			// Total energy deposition in the volume/event 
	fTree->Branch("iTotOptNum", &iTotalOptPhotNumber, "TotOptNum/I");
			// Total number of optical photons in the volume 
	fTree->Branch("iTotThermElecNum", &iTotalThermElecNumber, "TotThermElecNum/I");
			// Total number of thermal electrons in the volume

	struct datalevel {
		int stepNumber;
		int particleID;
		int trackID;
		int parentID;
		double particleEnergy;
		double particleDirection[3];
		double energyDeposition;
		double position[3];
		double stepTime;
	} data;

	int recordLevel;
	int optPhotRecordLevel;
	int thermElecRecordLevel;
	int Size1, Size2, Size3;
	
	fin.read((char *)(&iNumRecords), sizeof(int));
	if( DEBUGGING ) cout<<"numRecords= "<<iNumRecords<<endl;

	fin.read((char *)(&Size1),sizeof(int));
	productionTime = new char [Size1+1];
	fin.read((char *)(productionTime),Size1);
	productionTime[Size1] = '\0';

	fin.read((char *)(&Size1),sizeof(int));
	geant4Version = new char [Size1+1];
	fin.read((char *)(geant4Version),Size1);
	geant4Version[Size1] = '\0';

	fin.read((char *)(&Size1),sizeof(int));
	svnVersion = new char [Size1+1];
	fin.read((char *)(svnVersion),Size1);
	svnVersion[Size1] = '\0';
	int svnVersion_int = atoi(svnVersion+10);
	bool has_emission_time = (svnVersion_int > 606) ? true : false;
    //  This next line is a total hack, because it simply checks to see if the
    //  code is under svn versioning. But since it's highly unlikely that
    //  anyone's using LUXSim version 606 or earlier (which was created in May
    //  of 2013).
//    if( svnVersion_int == 0 )
        has_emission_time = true;

	fin.read((char *)(&Size1),sizeof(int));
	uName = new char [Size1+1];
	fin.read((char *)(uName),Size1);
	uName[Size1] = '\0';

	fin.read((char *)(&Size1),sizeof(int));
	InputCommands = new char [Size1+1];
	fin.read((char *)(InputCommands),Size1);
	InputCommands[Size1] = '\0';

	fin.read((char *)(&Size1),sizeof(int));
	diffs = new char [Size1+1];
	fin.read((char *)(diffs),Size1);
	diffs[Size1] = '\0';

	fin.read((char *)(&Size1),sizeof(int));
	DetCompo = new char [Size1+1];
	fin.read((char *)(DetCompo),Size1);
	DetCompo[Size1] = '\0';

	fTree0->Branch("iNumRecords", &iNumRecords, "iNumRecords/I");
			// number of Records
	fTree0->Branch("productionTime", productionTime, "sPT/C");
			// the time producting these data
	fTree0->Branch("geant4Version", geant4Version, "sG4V/C"); // Geant4 Version
	fTree0->Branch("svnVersion", svnVersion, "sSV/C");
			// SVN Version of simulation code 
	fTree0->Branch("uName", uName, "sUN/C"); // Name of the computer
	fTree0->Branch("InputCommands", InputCommands, "sIC/C"); // Input Commands 
	fTree0->Branch("diffs", diffs, "sDI/C");
			// Code differences before/after modification 
	fTree0->Branch("DetCompo", DetCompo, "sDC/C");
			// Detector component lookup table 

	fTree0->Fill();
    
    //  Parse DetCompo to create a vector of volume names to act as a lookup
    //  table later in this converter
    string lookupTableStr = DetCompo;
    vector<string> componentLookupTable;
    string singleLine;
    int index = 1;
    while( lookupTableStr.length() ) {
        singleLine = lookupTableStr.substr( 0, lookupTableStr.find("\n") );
        singleLine = singleLine.substr( singleLine.find(": ")+2 );
        componentLookupTable.push_back( singleLine );
        lookupTableStr = lookupTableStr.substr( lookupTableStr.find("\n") + 1 );
    }
    
	if( DEBUGGING ) {
		cout<<"generationTime= "<<productionTime<<endl;
		cout<<"g4version= "<<geant4Version<<endl;
		cout<<"svnVersion= "<<svnVersion<<endl;
		cout<<"uName= "<<uName<<endl;
		cout<<"commands= "<<InputCommands<<endl;
		cout<<"diffs= "<<diffs<<endl;
		cout<<"component= "<<DetCompo<<endl;
	}
	delete[] productionTime;
	delete[] geant4Version;
	delete[] svnVersion;
	delete[] uName;
	delete[] InputCommands;
	delete[] diffs;
	delete[] DetCompo;

	for(int i=0; i<iNumRecords; i++) {
		fin.read((char *)(&iPrimaryParNum),sizeof(int));
		
		fPrimaryParEnergy_keV = new double[iPrimaryParNum];
		fPrimaryParTime_ns = new double[iPrimaryParNum];
		fPrimaryParPosX_mm = new double[iPrimaryParNum];	
		fPrimaryParPosY_mm = new double[iPrimaryParNum];
		fPrimaryParPosZ_mm = new double[iPrimaryParNum];
		fPrimaryParDirX = new double[iPrimaryParNum];
		fPrimaryParDirY = new double[iPrimaryParNum];
		fPrimaryParDirZ = new double[iPrimaryParNum];
		cPrimaryParName = new char[iPrimaryParNum*(kMaxCharacter +1) +2];
        cVolumeName = new char[kMaxCharacter+1];
		fTree->SetBranchAddress("fPrimaryParEnergy_keV", fPrimaryParEnergy_keV);
		fTree->SetBranchAddress("fPrimaryParTime_ns", fPrimaryParTime_ns);
		fTree->SetBranchAddress("fPrimaryParPosX_mm", fPrimaryParPosX_mm);
		fTree->SetBranchAddress("fPrimaryParPosY_mm", fPrimaryParPosY_mm);
		fTree->SetBranchAddress("fPrimaryParPosZ_mm", fPrimaryParPosZ_mm);
		fTree->SetBranchAddress("fPrimaryParDirX", fPrimaryParDirX);
		fTree->SetBranchAddress("fPrimaryParDirY", fPrimaryParDirY);
		fTree->SetBranchAddress("fPrimaryParDirZ", fPrimaryParDirZ);
		fTree->SetBranchAddress("cPrimaryParName", cPrimaryParName);
        fTree->SetBranchAddress("cVolumeName", cVolumeName);

		cPrimaryParNamePos = 0;
		for (int i = 0; i< iPrimaryParNum; i++){
			fin.read((char *)(&Size1),sizeof(int));
			strName = new char [Size1+1];
			particleName = new char [Size1+1];
			fin.read((char *)(particleName),Size1);
			strncpy(strName, particleName, Size1);
			if (Size1 > kMaxCharacter){
					cout << "WARNING: the particle name has "
						 << "exceeded: " << kMaxCharacter << endl;
					cout<<"Truncating name...."<<endl;
					Size1 = kMaxCharacter;
			}
			particleName[Size1] = '&';
			strncpy(&(cPrimaryParName[cPrimaryParNamePos]), particleName,
					Size1+1);
			cPrimaryParNamePos += Size1+1;
			strName[Size1] = '\0';
			fin.read((char *)(&fPrimaryParEnergy_keV[i]),sizeof(double));
			if(has_emission_time)
				fin.read((char *)(&fPrimaryParTime_ns[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParPosX_mm[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParPosY_mm[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParPosZ_mm[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParDirX[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParDirY[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParDirZ[i]),sizeof(double));
			if (DEBUGGING) {
                cout << "iPrimaryParNum, ParName, KEn = " << i << ", "
                     << strName << ", " << fPrimaryParEnergy_keV[i] << endl;
                cout << "PosX,PosY,PosZ = " << fPrimaryParPosX_mm[i]
                     << ", " << fPrimaryParPosY_mm[i]
                     << ", " << fPrimaryParPosZ_mm[i] << endl;
                cout << "DirX,DirY,DirZ = " << fPrimaryParDirX[i] << ", "
                     << fPrimaryParDirY[i] << ", " << fPrimaryParDirZ[i] <<endl;
			}
		}

		fin.read((char *)(&recordLevel),sizeof(int));
		fin.read((char *)(&optPhotRecordLevel),sizeof(int));
		fin.read((char *)(&thermElecRecordLevel),sizeof(int));
		fin.read((char *)(&iVolume),sizeof(int));
        strncpy( cVolumeName, componentLookupTable[iVolume-1].c_str(),
                componentLookupTable[iVolume-1].length() );
        cVolumeName[componentLookupTable[iVolume-1].length()] = '\0';
		fin.read((char *)(&iEvtNb),sizeof(int));
		if (recordLevel>0) fin.read((char *)(&fTotEnergyDep_keV),
				sizeof(double));
		if (optPhotRecordLevel>0) fin.read((char *)(&iTotalOptPhotNumber),
				sizeof(int));
		if (thermElecRecordLevel>0) fin.read((char *)(&iTotalThermElecNumber),
				sizeof(int));
		fin.read((char *)(&iRecordSize),sizeof(int));
		iEvtNb += 1; //starts from 1

		iStepNum = new int[iRecordSize];
		iParticleID = new int[iRecordSize];
		iTrackID = new int[iRecordSize];
		iParentID = new int[iRecordSize];
		fKEnergy_keV = new double[iRecordSize]; 
		fOptPhoWaveLength_nm = new double[iRecordSize];
		fDirectionX = new double[iRecordSize]; 
		fDirectionY = new double[iRecordSize];
		fDirectionZ = new double[iRecordSize];
		fEnergyDep_keV = new double[iRecordSize];
		fPositionX_cm = new double[iRecordSize];
		fPositionY_cm = new double[iRecordSize];
		fPositionZ_cm = new double[iRecordSize];
		fStepTime = new double[iRecordSize];
		cParticleName = new char[iRecordSize*(kMaxCharacter +1) +2];
		cCreatorProcess = new char[iRecordSize*(kMaxCharacter +1) +2];
        cStepProcess = new char[iRecordSize*(kMaxCharacter +1) +2];
				
		fTree->SetBranchAddress("iStepNum", iStepNum);
		fTree->SetBranchAddress("iParticleID",iParticleID);
		fTree->SetBranchAddress("iTrackID",iTrackID);
		fTree->SetBranchAddress("iParentID",iParentID);
		fTree->SetBranchAddress("fKEnergy_keV",fKEnergy_keV);
		fTree->SetBranchAddress("fOptPhoWaveLength_nm", fOptPhoWaveLength_nm);
		fTree->SetBranchAddress("fDirectionX",fDirectionX);
		fTree->SetBranchAddress("fDirectionY",fDirectionY);
		fTree->SetBranchAddress("fDirectionZ",fDirectionZ);
		fTree->SetBranchAddress("fEnergyDep_keV",fEnergyDep_keV);
		fTree->SetBranchAddress("fPositionX_cm",fPositionX_cm);
		fTree->SetBranchAddress("fPositionY_cm",fPositionY_cm);
		fTree->SetBranchAddress("fPositionZ_cm",fPositionZ_cm);
		fTree->SetBranchAddress("fStepTime",fStepTime);
		fTree->SetBranchAddress("cParticleName",cParticleName);
		fTree->SetBranchAddress("cCreatorProcess",cCreatorProcess);
        fTree->SetBranchAddress("cStepProcess",cStepProcess);
		
		if( DEBUGGING ) {
			cout << "RecordLevel = " << recordLevel << endl;
			cout << "OptRecordLevel = " << optPhotRecordLevel << endl;
			cout << "ThermElecRecordLevel = " << thermElecRecordLevel << endl;
			cout << "volume, evtN, RecordSize = "<< iVolume << ", " << iEvtNb
				 << ", " << iRecordSize << endl;
			cout << "TotEdep, TotalOptPhotNumber, TotalThermElecNumber= " << fTotEnergyDep_keV 
				 <<", "<<iTotalOptPhotNumber<<", "<< iTotalThermElecNumber<< "\n" << endl;
		}

		cParticleNamePos = cCreatorProcessPos = cStepProcessPos = 0;
		for( int i = 0; i<iRecordSize; i++ ) {
			fin.read((char *)(&Size1),sizeof(int));
		 	strName = new char [Size1+1];			
			particleName = new char [Size1+1];
			fin.read((char *)(particleName),Size1);
			strncpy(strName, particleName, Size1);
			if (Size1 > kMaxCharacter){
				cout<<"WARNING: the particle name has exceeded: "<<kMaxCharacter
					<<endl;
				cout<<"Truncating name...."<<endl;
				Size1 = kMaxCharacter;
			}
			particleName[Size1] = '&';
			strncpy(&(cParticleName[cParticleNamePos]), particleName, Size1+1);
			cParticleNamePos += Size1+1;
			strName[Size1] = '\0';
			
			fin.read((char *)(&Size2),sizeof(int));
			creatorProcName = new char[Size2+1];
			creatorProcessName = new char[Size2+1];
			fin.read((char *)(creatorProcessName), Size2 );
			strncpy(creatorProcName, creatorProcessName, Size2 );
			if( Size2 > kMaxCharacter ) {
				cout << "WARNING: the creator process name has exceeded: "
					 << kMaxCharacter << endl;
				cout << "Truncating process name..." << endl;
				Size2 = kMaxCharacter;
			}
			creatorProcessName[Size2] = '&';
			strncpy(&(cCreatorProcess[cCreatorProcessPos]), creatorProcessName,
					Size2+1);
			cCreatorProcessPos += Size2+1;
			creatorProcName[Size2] = '\0';

            fin.read((char *)(&Size3),sizeof(int));
            stepProcName = new char[Size3+1];
            stepProcessName = new char[Size3+1];
            fin.read((char *)(stepProcessName), Size3 );
            strncpy(stepProcName, stepProcessName, Size3 );
            if( Size3 > kMaxCharacter ) {
                cout << "WARNING: the step process name has exceeded: "
                << kMaxCharacter << endl;
                cout << "Truncating process name..." << endl;
                Size3 = kMaxCharacter;
            }
            stepProcessName[Size3] = '&';
            strncpy(&(cStepProcess[cStepProcessPos]), stepProcessName,
                    Size3+1);
            cStepProcessPos += Size3+1;
            stepProcName[Size3] = '\0';
            
			fin.read((char *)(&data),sizeof(data));
			iStepNum[i] = (data.stepNumber);
			iParticleID[i] = (data.particleID);
			iTrackID[i] = (data.trackID);
			iParentID[i] = (data.parentID);
			fKEnergy_keV[i] = (data.particleEnergy);
			if (string(strName) == "opticalphoton") {
				if (data.particleEnergy > 0 ){
					fOptPhoWaveLength_nm[i] = (1.24/data.particleEnergy);
				}
			}else{
				fOptPhoWaveLength_nm[i] = (0.);
			}
			fDirectionX[i] = (data.particleDirection[0]);
			fDirectionY[i] = (data.particleDirection[1]);
			fDirectionZ[i] = (data.particleDirection[2]);
			fEnergyDep_keV[i] = (data.energyDeposition);
			fPositionX_cm[i] = (data.position[0]);
			fPositionY_cm[i] = (data.position[1]);
			fPositionZ_cm[i] = (data.position[2]);
			fStepTime[i] = (data.stepTime);
	
			if( DEBUGGING ) {
				cout<<"i = "<<i<<endl;
				cout<<"particleName, size= "<<strName<<"\t"<<Size1<<endl;
				cout<<"creatorProcessName, size= "<<creatorProcessName<<"\t"<<Size2<<endl;
                cout<<"stepProcessName, size= "<<stepProcessName<<"\t"<<Size3<<endl;
				cout<<"stepNumber= "<<iStepNum[i]<<endl;
				cout<<"particleID= "<<iParticleID[i]<<endl;
				cout<<"trackID= "<<iTrackID[i]<<endl;
				cout<<"parentID= "<<iParentID[i]<<endl;
				cout<<"particleEnergy= "<<fKEnergy_keV[i]<<endl;
				cout<<"fOptPhoWaveLength_nm= "<<fOptPhoWaveLength_nm[i]<<endl;
				cout<<"particleDirection= "<<fDirectionX[i];
				cout<<", "<<fDirectionY[i];
				cout<<", "<<fDirectionZ[i]<<endl;
				cout<<"fEnergyDep_keV= "<<fEnergyDep_keV[i]<<endl;
				cout<<"position= "<<fPositionX_cm[i];
				cout<<", "<<fPositionY_cm[i];
				cout<<", "<<fPositionZ_cm[i]<<endl;
				cout<<"stepTime = "<<fStepTime[i]<<endl;
				cout<<endl<<endl;
			}

			delete[] particleName;
			delete[] strName;
            delete[] creatorProcessName;
            delete[] creatorProcName;
			delete[] stepProcessName;
			delete[] stepProcName;
		}	
		if (iRecordSize == 0) {
			cParticleName[cParticleNamePos] = ' ';
			cParticleNamePos++;
			cCreatorProcess[cCreatorProcessPos] = ' ';
			cCreatorProcessPos++;
            cStepProcess[cStepProcessPos] = ' ';
            cStepProcessPos++;
		}
		cPrimaryParName[cPrimaryParNamePos] = '\0';			
		cParticleName[cParticleNamePos]='\0';
		cCreatorProcess[cCreatorProcessPos]='\0';
        cStepProcess[cStepProcessPos]='\0';
		fTree->Fill();

		delete[] fPrimaryParEnergy_keV;
		delete[] fPrimaryParTime_ns;
		delete[] fPrimaryParPosX_mm;
		delete[] fPrimaryParPosY_mm;
		delete[] fPrimaryParPosZ_mm;
		delete[] fPrimaryParDirX;
		delete[] fPrimaryParDirY;
		delete[] fPrimaryParDirZ;
		delete[] cPrimaryParName;
        delete[] cVolumeName;

		delete[] iStepNum;
		delete[] iParticleID;
		delete[] iTrackID;
		delete[] iParentID;
		delete[] fKEnergy_keV; 
		delete[] fOptPhoWaveLength_nm;
		delete[] fDirectionX; 
		delete[] fDirectionY;
		delete[] fDirectionZ;
		delete[] fEnergyDep_keV;
		delete[] fPositionX_cm;
		delete[] fPositionY_cm;
		delete[] fPositionZ_cm;
		delete[] fStepTime;
		delete[] cParticleName;
		delete[] cCreatorProcess;
        delete[] cStepProcess;
	}
	
	fFile->cd();
	fTree->Write();
	fTree0->Write();
	fFile->Close();
	delete fFile;
	fin.close();
}
