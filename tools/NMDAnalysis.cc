////////////////////////////////////////////////////////////////////////////////
/*	NMDAnalysis.cc
*
* Very basic analysis for NMD Detector.
*
********************************************************************************
* Change log
*	15 June 2010 - Initial submission (Melinda Sweany): 
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
#include "TF1.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TRandom3.h"
//
//	C/C++ includes
//
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
//
//	Definitions
//
#define DEBUGGING 0

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------

bool PmtHitsToPhotoelectrons(double photonWavelength){
	double rand = gRandom->Rndm();
	int numEntries = 239;
	double wavelength[] = {270.152,		271.501,	272.85,		274.199,	275.548,
							276.897,	278.246,	279.595,	282.293,	283.642,
							286.341,	289.039,	291.737,	294.435,	299.831,
							305.228,	306.577,	309.275,	310.624,	313.322,
							314.671,	317.369,	318.718,	320.067,	321.417,
							322.766,	324.115,	325.464,	326.813,	328.162,
							329.511,	330.859,	332.209,	333.558,	334.907,
							336.256,	337.605,	338.954,	340.304,	341.653,
							343.002,	344.351,	345.7,		347.049,	348.398,
							349.746,	351.096,	352.445,	353.794,	355.143,
							356.492,	357.841,	359.191,	360.54,		361.889,
							363.238,	364.587,	365.936,	367.285,	368.634,
							369.983,	371.332,	372.681,	374.03,		375.379,
							376.728,	378.078,	379.427,	380.776,	382.125,
							383.474,	384.823,	386.172,	387.521,	388.87,
							390.219,	391.568,	392.917,	394.266,	395.616,
							396.964,	398.314,	399.663,	401.012,	402.361,
							403.71,		405.059,	406.408,	407.757,	409.106,
							410.455,	411.804,	413.153,	414.503,	415.851,
							417.201,	418.55,		419.899,	421.248,	422.597,
							423.946,	425.295,	426.644,	427.992,	429.342,
							430.69,		432.04,		433.39,		434.738,	436.088,
							437.437,	438.786,	440.135,	441.484,	442.833,
							444.182,	445.531,	446.88,		448.229,	449.577,
							450.927,	452.277,	453.626,	454.975,	456.324,
							457.673,	459.022,	460.371,	461.72,		463.069,
							464.418,	465.767,	467.116,	468.465,	469.815,
							471.164,	472.513,	473.862,	475.211,	476.56,
							477.909,	479.258,	480.607,	481.956,	483.305,
							484.654,	486.003,	487.351,	488.702,	502.192,
							504.89,		507.589,	510.287,	512.984,	515.683,
							518.381,	521.079,	522.428,	525.126,	526.476,
							527.825,	530.523,	533.221,	534.569,	535.919,
							537.268,	538.617,	541.315,	542.664,	545.363,
							546.712,	548.061,	549.41,		552.108,	553.457,
							554.806,	558.853,	561.551,	562.901,	564.25,
							565.599,	568.297,	569.646,	572.343,	575.042,
							576.391,	579.089,	580.438,	583.137,	584.486,
							587.184,	588.533,	589.882,	591.231,	592.58,
							595.278,	596.627,	597.976,	599.325,	600.675,
							603.373,	607.42,		608.769,	611.467,	614.165,
							616.863,	618.212,	619.562,	622.26,		623.609,
							624.958,	626.307,	627.656,	630.354,	631.702,
							633.052,	635.75,		637.099,	638.449,	639.798,
							641.147,	642.496,	643.845,	645.194,	646.543,
							647.892,	650.589,	651.939,	653.288,	654.637,
							655.987,	657.336,	658.685,	661.383,	662.732,
							664.081,	665.43,		666.779,	669.476};
	double efficiency[] = {0.00313,		0.00397,	0.00504,	0.00641,	0.00814,
							0.01033,	0.01313,	0.01667,	0.02117,	0.0269,
							0.03416,	0.04339,	0.05511,	0.07,		0.08891,
							0.11293,	0.12229,	0.13244,	0.13783,	0.1406,
							0.14632,	0.14926,	0.15227,	0.15533,	0.15846,
							0.16165,	0.1649,		0.16822,	0.17161,	0.17161,
							0.17506,	0.17506,	0.17506,	0.17858,	0.17858,
							0.18218,	0.18218,	0.18584,	0.18584,	0.18584,
							0.18958,	0.18958,	0.18958,	0.18958,	0.18958,
							0.1934,		0.1934,		0.1934,		0.1934,		0.1934,
							0.1934,		0.19729,	0.19729,	0.19729,	0.19729,
							0.19729,	0.19729,	0.19729,	0.19729,	0.19729,
							0.19729,	0.19729,	0.19729,	0.19729,	0.19729,
							0.19729,	0.19729,	0.19729,	0.19729,	0.19729,
							0.19729,	0.19729,	0.19729,	0.19729,	0.19729,
							0.19729,	0.19729,	0.19729,	0.19729,	0.19729,
							0.19729,	0.19729,	0.19729,	0.19729,	0.19729,
							0.19729,	0.19729,	0.19729,	0.19729,	0.19729,
							0.19729,	0.1934,		0.1934,	 	0.1934,		0.1934,
							0.18958,	0.18958,	0.18958,	0.18958,	0.18584,
							0.18584,	0.18584,	0.18584,	0.18218,	0.18218,
							0.18218,	0.17858,	0.17858,	0.17858,	0.17506,
							0.17506,	0.17506,	0.17161,	0.17161,	0.16822,
							0.16822,	0.16822,	0.16822,	0.1649,		0.1649,
							0.16165,	0.16165,	0.15846,	0.15533,	0.15533,
							0.15227,	0.15227,	0.14926,	0.14926,	0.14632,
							0.14343,	0.14343,	0.1406,		0.1406,		0.13783,
							0.13783,	0.13511,	0.13511,	0.13244,	0.13244,
							0.12983,	0.12983,	0.12726,	0.12726,	0.12726,
							0.12476,	0.12229,	0.11988,	0.11988,	0.10222,
							0.1002,		0.09628,	0.09438,	0.0907,		0.08891,
							0.08714,	0.08375,	0.08048,	0.07889,	0.07581,
							0.07431,	0.07141,	0.07,		0.06862,	0.06593,
							0.06464,	0.06211,	0.05968,	0.05734,	0.05621,
							0.05402,	0.05191,	0.04988,	0.04794,	0.04605,
							0.04339,	0.04087,	0.0385,		0.03627,	0.03484,
							0.03283,	0.03154,	0.02971,	0.02799,	0.02637,
							0.02484,	0.02339,	0.02204,	0.02076,	0.01955,
							0.01842,	0.01734,	0.01634,	0.01569,	0.01479,
							0.01393,	0.01313,	0.01236,	0.01165,	0.01097,
							0.00954,	0.00847,	0.00751,	0.00667,	0.00592,
					 		0.00525,	0.00466,	0.00413,	0.00367,	0.00325,
							0.00289,	0.00256,	0.00227,	0.00202,	0.00179,
							0.00159,	0.00141,	0.00125,	0.00111,	0.0009842,
							0.0008733,	0.0007749,	0.0006875,	0.0006101,	0.0005413,
							0.0004803,	0.0004348,	0.0003858,	0.0003423,	0.0003037,
							0.0002695,	0.0002391,	0.0002122,	0.0001883,	0.0001671,
							0.0001482,	0.0001315,	0.0001167,	0.0001035};
	//	find the closest wavelenght entry
	//	use that as the quantum efficiency
	double difference = 1000000.;
	int index = 0;
	for(int i=0; i<numEntries; i++){
		if(fabs(wavelength[i]-photonWavelength) < difference){
			difference = fabs(wavelength[i]-photonWavelength);
			index = i;
		}
	}
	//	if the random number is less
	//	than the efficiency, we got a hit
	if(rand < efficiency[index]) 
		return true;
	else 
		return false;
}
int main( int argc, char** argv){

	//	set the random number seed
	//	from /dev/random
	ifstream devrandom("/dev/random");
	int seed;
	devrandom.read( (char*)(&seed), sizeof(int) );
	if(seed < 0) seed = -seed;
	devrandom.close();

	gRandom->SetSeed(seed);	

	//	read in the binary file 
	ifstream fin;
	char * filename = argv[1];
	fin.open(filename,ios::binary|ios::in);
	if(  !fin.is_open() ) {
		cout << "Couldn't find the file "<< filename << endl;
		exit( 0 );
	}

	TFile* fFile;
	
	const int kMaxCharacter = 30;
	int cParticleNamePos;
	int iRecordSize;	
	int iEvtNb;
	int iVolume;
	int iTotalOptPhotNumber=0;

	int iPrimaryParNum;
	int cPrimaryParNamePos;
	char * cPrimaryParName;
	double * fPrimaryParEnergy_keV;
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

	char * outfile = strtok(filename,".");
	strcat(outfile,"_Plots.root");
	fFile = TFile::Open(outfile, "RECREATE");

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
	int Size1;
	
	//	read in header info from binary file
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


	//  create the PMT lookup tables from the detector component 
	//  basket in the header, the index is the pmt number -1
	int numPmts = 2;
	int northPmtLookup[numPmts];
	int southPmtLookup[numPmts];
	int northPmtHits[numPmts];
	int southPmtHits[numPmts];
	for(int p=0; p<numPmts; p++){
		northPmtLookup[p]=0;
		southPmtLookup[p]=0;
		northPmtHits[p]=0;
		southPmtHits[p]=0;
	}
	char * str1 = strtok (DetCompo," ");
	char * str2 = strtok (NULL," ");
	char * str3 = strtok (NULL,"\n");
	while(strcmp(str3,"Source")!=0){
		str1 = strtok (NULL," ");
		str2 = strtok (NULL," ");
		str3 = strtok (NULL,"\n");
		char * northName = (char*)"North_PMT_";
		char * southName = (char*)"South_PMT_";
		if(strncmp(str3,northName,strlen(northName)) == 0){
			size_t length;
			char pmtNumber[5];
			string componentName = (string)str3;
			length = componentName.copy(pmtNumber,
					strlen(str3)-strlen(northName),strlen(northName));
			pmtNumber[length] = '\0';
			char volumeNumber[5];
			strncpy(volumeNumber,str2,strlen(str2)-1);
			volumeNumber[strlen(str2)-1] = '\0';
			northPmtLookup[atoi(pmtNumber)-1] = atoi(volumeNumber);
		}
		else if(strncmp(str3,southName,strlen(southName)) == 0){
			size_t length;
			char pmtNumber[5];
			string componentName = (string)str3;
			length = componentName.copy(pmtNumber,
					strlen(str3)-strlen(southName),strlen(southName));
			pmtNumber[length] = '\0';
			char volumeNumber[5];
			strncpy(volumeNumber,str2,strlen(str2)-1);
			volumeNumber[strlen(str2)-1] = '\0';
			southPmtLookup[atoi(pmtNumber)-1] = atoi(volumeNumber);
		}
	}

	//	declare histograms
	TH1F * NORTH_PHOTOELECTRONS[numPmts];
	TH1F * SOUTH_PHOTOELECTRONS[numPmts];
	for(int p=0; p<numPmts; p++){
		char northName[kMaxCharacter];
		char southName[kMaxCharacter];
		sprintf(northName,"NORTH_PHOTOELECTRONS_%2d",p+1);
		sprintf(southName,"SOUTH_PHOTOELECTRONS_%2d",p+1);	
		NORTH_PHOTOELECTRONS[p] = new TH1F(northName,"Number of Photoelectrons;photoelectrons;counts",500,0.,10000.);
		SOUTH_PHOTOELECTRONS[p] = new TH1F(southName,"Number of Photoelectrons;photoelectrons;counts",500,0.,10000.);	
	}

	//	clear the header variables
	delete[] productionTime;
	delete[] geant4Version;
	delete[] svnVersion;
	delete[] uName;
	delete[] InputCommands;
	delete[] diffs;
	delete[] DetCompo;

	//	currentEvent is a counter, initialized at the first event,
	//	that is only incremented when it is not equal to iEvtNb.
	//	This is necessary to get all the records from one event in
	//	the same histogram.
	int currentEvent = 1;

	//	start the loop over records
	for(int i=0; i<iNumRecords; i++) {
		//	read in the primary particle info
		fin.read((char *)(&iPrimaryParNum),sizeof(int));		
		fPrimaryParEnergy_keV = new double[iPrimaryParNum];
		fPrimaryParPosX_mm = new double[iPrimaryParNum];	
		fPrimaryParPosY_mm = new double[iPrimaryParNum];
		fPrimaryParPosZ_mm = new double[iPrimaryParNum];
		fPrimaryParDirX = new double[iPrimaryParNum];
		fPrimaryParDirY = new double[iPrimaryParNum];
		fPrimaryParDirZ = new double[iPrimaryParNum];
		cPrimaryParName = new char[iPrimaryParNum*(kMaxCharacter +1) +2];

		cPrimaryParNamePos = 0;
		for (int i = 0; i< iPrimaryParNum; i++){
			fin.read((char *)(&Size1),sizeof(int));
			strName = new char [Size1+1];
			particleName = new char [Size1+1];
			fin.read((char *)(particleName),Size1);
			strncpy(strName, particleName, Size1);
			if (Size1 > kMaxCharacter){
				cout<<"WARNING: the particle name has exceeded: "<<kMaxCharacter<<endl;
				cout<<"Truncating name...."<<endl;
				Size1 = kMaxCharacter;
			}
			particleName[Size1] = '&';
			strncpy(&(cPrimaryParName[cPrimaryParNamePos]), particleName, Size1+1);
			cPrimaryParNamePos += Size1+1;
			strName[Size1] = '\0';
			fin.read((char *)(&fPrimaryParEnergy_keV[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParPosX_mm[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParPosY_mm[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParPosZ_mm[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParDirX[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParDirY[i]),sizeof(double));
			fin.read((char *)(&fPrimaryParDirZ[i]),sizeof(double));
		}

		//	read in event level info
		fin.read((char *)(&recordLevel),sizeof(int));
		fin.read((char *)(&optPhotRecordLevel),sizeof(int));
		fin.read((char *)(&iVolume),sizeof(int));
		fin.read((char *)(&iEvtNb),sizeof(int));
		if (recordLevel>0) fin.read((char *)(&fTotEnergyDep_keV),sizeof(double));
		if (optPhotRecordLevel>0) fin.read((char *)(&iTotalOptPhotNumber),sizeof(int));
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
		cParticleNamePos = 0;		
		
		//	start the loop over entires in record
		for( int i = 0; i<iRecordSize; i++ ) {
			fin.read((char *)(&Size1),sizeof(int));
		   	strName = new char [Size1+1];			
			particleName = new char [Size1+1];
			fin.read((char *)(particleName),Size1);
			strncpy(strName, particleName, Size1);
			if (Size1 > kMaxCharacter){
				cout<<"WARNING: the particle name has exceeded: "<<kMaxCharacter<<endl;
				cout<<"Truncating name...."<<endl;
				Size1 = kMaxCharacter;
			}
			particleName[Size1] = '&';
			strncpy(&(cParticleName[cParticleNamePos]), particleName, Size1+1);
			cParticleNamePos += Size1+1;
			strName[Size1] = '\0';

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
			}
			else {
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
	
			delete[] particleName;
			delete[] strName;
	
			//	if you pass the quantum efficiency cut,
			//	register a hit
			if(PmtHitsToPhotoelectrons(fOptPhoWaveLength_nm[i])) {
				for(int p=0; p<numPmts; p++){
					if(northPmtLookup[p] == iVolume){
						northPmtHits[p]++;
					}
					else if(southPmtLookup[p] == iVolume){
						southPmtHits[p]++;
					}
				}
			}
		}	//	end of loop over recordSize
		//	if iEvtNb is not the current event: fill the histograms,
		//	zero the arrays, and increment currentEvent
		if(iEvtNb != currentEvent){
			currentEvent = iEvtNb;
			for(int p=0; p<numPmts; p++){
				NORTH_PHOTOELECTRONS[p]->Fill(northPmtHits[p]);
				SOUTH_PHOTOELECTRONS[p]->Fill(southPmtHits[p]);
				northPmtHits[p]=0;
				southPmtHits[p]=0;
			}
		}
		//	free all the memeory, clear the names, etc.
		if (iRecordSize == 0) {
			cParticleName[cParticleNamePos] = ' ';
			cParticleNamePos++;
		}
		cPrimaryParName[cPrimaryParNamePos] = '\0';			
		cParticleName[cParticleNamePos]='\0';

		delete[] fPrimaryParEnergy_keV;
		delete[] fPrimaryParPosX_mm;
		delete[] fPrimaryParPosY_mm;
		delete[] fPrimaryParPosZ_mm;
		delete[] fPrimaryParDirX;
		delete[] fPrimaryParDirY;
		delete[] fPrimaryParDirZ;
		delete[] cPrimaryParName; 

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

	}	//	end of loop over numRecords
	
	fFile->cd();
	fFile->Write();
	fFile->Close();
	delete fFile;
	fin.close();
}
