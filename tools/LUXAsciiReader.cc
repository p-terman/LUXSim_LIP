////////////////////////////////////////////////////////////////////////////////
/*	LUXAsciiReader.cc
*
* A reader which extracts ASCII light and charge yields from LUXSim .bin files.
*
********************************************************************************
* Change log
*	14 Feb 2012 - Initial submission (Matthew) based on LUXRootReader.cc
*       23 Feb 2012 - Adapted for use in PMT hitmaps and S2/S1 plots (Matthew)
*       26 Feb 2012 - Fixed event# to start @1 and changed %d->%i (Matthew)
*       07 Mar 2012 - Changed some variables from long to double (Matthew)
*       11 Apr 2012 - Added support for case of muonVeto+cryoStand on (Matthew)
*       14 Apr 2012 - Changed hard-coded grid coordinates (Matthew)
*       11 May 2012 - Added support for the new thermal e- recording (Matthew)
*       22 Jun 2012 - New output: log(S2/S1) analogues + variances (Matthew)
*       27 Jul 2012 - Corrected for vol new num after Pb backing (Matthew)
*       30 Dec 2012 - Compensated for growing time in S1 v S2 cut (Matthew)
*       24 Aug 2015 - Added support for the primary particle time (Kareem)
*       24 Aug 2015 - Added support for the step process name (Kareem)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	ROOT includes
//
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TText.h"
#include "TH1F.h"
#include "TF1.h"
//
//	C/C++ includes
//
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <cstdlib>
//
//	Definitions
//
#define DEBUGGING 1

using namespace std;

#define maxNumEvts 5000000
double TotalS1Hits[maxNumEvts], TotalS2Hits[maxNumEvts];


//------++++++------++++++------++++++------++++++------++++++------++++++------
int main( int argc, char** argv){
	

	ifstream fin;
	char * filename = argv[1];

	int sourceTubes;
	if ( argc >= 3 ) sourceTubes = atoi(argv[2]);
	else 
	  sourceTubes = 0;

	fin.open(filename,ios::binary|ios::in);
	if(  !fin.is_open() ) {
		cout << "Couldn't find the file "<< filename << endl;
		exit( 0 );
	}
	
	const int kMaxCharacter = 25;
	int cParticleNamePos, cCreatorProcessPos, cStepProcessPos;
	int iRecordSize;	
	int iEvtNb;
	int iVolume;
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
	
	double TotalpHits = 0, TotaleHits = 0, rad, exx, why, zee; int fiducial;
	double fTotalOptPhotNumber[iNumRecords], fTotalIonElecNumber[iNumRecords];
	TH1F *TimeHistogram = new TH1F("TimeHistogram","Steptimes",40,30,150);
	char ParentParticle[80]; double Energy, delta, pos[3]; double S1,S2;
	for(int i=0; i<iNumRecords; i++) {
		fin.read((char *)(&iPrimaryParNum),sizeof(int));
		fiducial = 1; Energy = 0.0; pos[0]=0;pos[1]=0;pos[2]=0; delta=-1.0; S1=0;S2=0;
		fPrimaryParEnergy_keV = new double[iPrimaryParNum];
        fPrimaryParTime_ns = new double[iPrimaryParNum];
		fPrimaryParPosX_mm = new double[iPrimaryParNum];
        fPrimaryParPosY_mm = new double[iPrimaryParNum];
        fPrimaryParPosZ_mm = new double[iPrimaryParNum];
        fPrimaryParDirX = new double[iPrimaryParNum];
        fPrimaryParDirY = new double[iPrimaryParNum];
        fPrimaryParDirZ = new double[iPrimaryParNum];
		cPrimaryParName = new char[iPrimaryParNum*(kMaxCharacter +1) +2];
		
        cPrimaryParNamePos = 0;
        for (int j = 0; j< iPrimaryParNum; j++){
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
            fin.read((char *)(&fPrimaryParEnergy_keV[j]),sizeof(double));
            if( has_emission_time )
                fin.read((char *)(&fPrimaryParTime_ns[i]),sizeof(double));
            fin.read((char *)(&fPrimaryParPosX_mm[j]),sizeof(double));
            fin.read((char *)(&fPrimaryParPosY_mm[j]),sizeof(double));
            fin.read((char *)(&fPrimaryParPosZ_mm[j]),sizeof(double));
            fin.read((char *)(&fPrimaryParDirX[j]),sizeof(double));
            fin.read((char *)(&fPrimaryParDirY[j]),sizeof(double));
            fin.read((char *)(&fPrimaryParDirZ[j]),sizeof(double));
            if (DEBUGGING) {
                cout << "iPrimaryParNum, ParName, KEn = " << j << ", "
                     << strName << ", " << fPrimaryParEnergy_keV[j] << endl;
                cout << "PosX,PosY,PosZ = " << fPrimaryParPosX_mm[j]
                     << ", " << fPrimaryParPosY_mm[j]
                     << ", " << fPrimaryParPosZ_mm[j] << endl;
                cout << "DirX,DirY,DirZ = " << fPrimaryParDirX[j] << ", "
                     << fPrimaryParDirY[j] << ", " << fPrimaryParDirZ[j] <<endl;
            }
        }

		fin.read((char *)(&recordLevel),sizeof(int));
		fin.read((char *)(&optPhotRecordLevel),sizeof(int));
		fin.read((char *)(&thermElecRecordLevel),sizeof(int));
		fin.read((char *)(&iVolume),sizeof(int));
		fin.read((char *)(&iEvtNb),sizeof(int));
		if (recordLevel>0) fin.read((char *)(&fTotEnergyDep_keV),sizeof(double));
		if (optPhotRecordLevel>0) fin.read((char *)(&iTotalOptPhotNumber),sizeof(int));
		if (thermElecRecordLevel>0) fin.read((char *)(&iTotalThermElecNumber),sizeof(int));
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
		
		if( DEBUGGING ) {
		  cout << "RecordLevel = " << recordLevel << endl;
		  cout << "OptRecordLevel = " << optPhotRecordLevel << endl;
		  cout << "ThermElecRecordLevel = " << thermElecRecordLevel << endl;
		  cout << "volume, evtN, RecordSize = "<< iVolume << ", " << iEvtNb
		       << ", " << iRecordSize << endl;
		  cout << "TotEdep, TotalOptPhotNumber, TotalThermElecNumber= " << fTotEnergyDep_keV
		       <<", "<<iTotalOptPhotNumber<<", "<< iTotalThermElecNumber<< "\n" << endl;
		}
		if(sourceTubes) iVolume -= 126;
		if ( iVolume < 188 || iVolume > 793 ) {
		  TotalpHits += iTotalOptPhotNumber; fTotalOptPhotNumber[i] = iTotalOptPhotNumber;
		  TotaleHits += iTotalThermElecNumber; fTotalIonElecNumber[i] = iTotalThermElecNumber;
		  fprintf(stderr,"%f\t%i\t%i\t",fTotEnergyDep_keV,iTotalOptPhotNumber,iTotalThermElecNumber);
		}
		
		cParticleNamePos = cCreatorProcessPos = cStepProcessPos = 0;
		for( int k = 0; k<iRecordSize; k++ ) {
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
			
			fin.read((char *)(&Size2),sizeof(int));
            creatorProcName = new char[Size2+1];
            creatorProcessName = new char[Size2+1];
            fin.read((char *)(creatorProcessName), Size2 );
            strncpy(creatorProcName, creatorProcessName, Size2 );
            if( Size2 > kMaxCharacter ) {
                cout << "WARNING: the process name has exceeded: "
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
                cout << "WARNING: the process name has exceeded: "
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
			iStepNum[k] = (data.stepNumber);
			iParticleID[k] = (data.particleID);
			iTrackID[k] = (data.trackID);
			iParentID[k] = (data.parentID);
			fKEnergy_keV[k] = (data.particleEnergy);
			if (string(strName) == "opticalphoton") {
				if (data.particleEnergy > 0 ){
					fOptPhoWaveLength_nm[k] = (1.24/data.particleEnergy);
				}
			}else{
				fOptPhoWaveLength_nm[k] = (0.);
			}
			fDirectionX[k] = (data.particleDirection[0]);
			fDirectionY[k] = (data.particleDirection[1]);
			fDirectionZ[k] = (data.particleDirection[2]);
			fEnergyDep_keV[k] = (data.energyDeposition);
			fPositionX_cm[k] = (data.position[0]);
			fPositionY_cm[k] = (data.position[1]);
			fPositionZ_cm[k] = (data.position[2]);
			fStepTime[k] = (data.stepTime);
	
			if( DEBUGGING ) {
				cout<<"k = "<<k<<endl;
				cout<<"particleName, size= "<<strName<<"\t"<<Size1<<endl;
				cout<<"creatorProcessName, size= "<<creatorProcName<<"\t"<<Size2<<endl;
                cout<<"stepProcessName, size= "<<stepProcName<<"\t"<<Size3<<endl;
				cout<<"stepNumber= "<<iStepNum[k]<<endl;
				cout<<"particleID= "<<iParticleID[k]<<endl;
				cout<<"trackID= "<<iTrackID[k]<<endl;
				cout<<"parentID= "<<iParentID[k]<<endl;
				cout<<"particleEnergy= "<<fKEnergy_keV[k]<<endl;
				cout<<"fOptPhoWaveLength_nm= "<<fOptPhoWaveLength_nm[k]<<endl;
				cout<<"particleDirection= "<<fDirectionX[k];
				cout<<", "<<fDirectionY[k];
				cout<<", "<<fDirectionZ[k]<<endl;
				cout<<"fEnergyDep_keV= "<<fEnergyDep_keV[k]<<endl;
				cout<<"position= "<<fPositionX_cm[k];
				cout<<", "<<fPositionY_cm[k];
				cout<<", "<<fPositionZ_cm[k]<<endl;
				cout<<"stepTime = "<<fStepTime[k]<<endl;
			}
			
			if ( fEnergyDep_keV[k] > 0 && string(strName) != "thermalelectron" && string(strName) != "opticalphoton" ) {
                          if ( pos[0] == 0 && pos[1] == 0 && pos[2] == 0 ) {
                            pos[0] = fPositionX_cm[k];
                            pos[1] = fPositionY_cm[k];
                            pos[2] = fPositionZ_cm[k];
                          }
                          else {
                            if ( sqrt(pow(fPositionX_cm[k]-pos[0],2.)+pow(fPositionY_cm[k]-pos[1],2.)+pow(fPositionZ_cm[k]-pos[2],2.)) > delta )
                              delta = sqrt(pow(fPositionX_cm[k]-pos[0],2.)+pow(fPositionY_cm[k]-pos[1],2.)+pow(fPositionZ_cm[k]-pos[2],2.));
                          }
                        }
			
			rad = (23.495-0.005)/cos(15.*M_PI/180.);
                        zee = fPositionZ_cm[k];
                        exx = fabs(fPositionX_cm[k]);
                        if(exx<(rad/2.)) why=(sqrt(3)-2.)*exx+rad;
                        else if (exx>=(rad/2.) && exx < (rad*sqrt(3)/2.)) why=-(exx-(rad/2.))+rad*(sqrt(3)/2.);
                        else if (exx>=(rad*sqrt(3)/2.) && exx <= rad) why=(exx-rad)/(sqrt(3)-2.);
                        else why = rad;
			if ( zee < 10. || zee > 50. || exx > rad || fabs(fPositionY_cm[k]) > why ) {
			  if ( string(strName) != "thermalelectron" && string(strName) != "opticalphoton" ) fiducial = 0;
			}
			
			if(string(strName) == "opticalphoton") TimeHistogram->Fill(fStepTime[k]);
			if(iParentID[k]==0) {
			  strcpy(ParentParticle,strName);
			  //Energy = fKEnergy_keV[0];
			} Energy += fEnergyDep_keV[k];
			
			//if counting phe in the PMTs
			if ( string(strName) == "thermalelectron" ) {
			  if ( (fStepTime[k]-fStepTime[0]) > 1000 ) S2++;
                          else S1++;
                        }
			
			delete[] particleName;
			delete[] strName;
			delete[] creatorProcessName;
            delete[] creatorProcName;
            delete[] stepProcessName;
            delete[] stepProcName;
		}
		
		if ( iVolume >= 188 && iVolume <= 793 ) {
		  
		  int pmtIndx[123]; pmtIndx[0]=0;
		  pmtIndx[1] = 1  ;
		  pmtIndx[2] = 54 ;
		  pmtIndx[3] = 53 ;
		  pmtIndx[4] = 52 ;
		  pmtIndx[5] = 51 ;
		  pmtIndx[6] = 2  ;
		  pmtIndx[7] = 5  ;
		  pmtIndx[8] = 57 ;
		  pmtIndx[9] = 56 ;
		  pmtIndx[10] = 55;
		  pmtIndx[11] = 44;
		  pmtIndx[12] = 3 ;
		  pmtIndx[13] = 6 ;
		  pmtIndx[14] = 8 ;
		  pmtIndx[15] = 59;
		  pmtIndx[16] = 58;
		  pmtIndx[17] = 47;
		  pmtIndx[18] = 43;
		  pmtIndx[19] = 4 ;
		  pmtIndx[20] = 7 ;
		  pmtIndx[21] = 9 ;
		  pmtIndx[22] = 10;
		  pmtIndx[23] = 60;
		  pmtIndx[24] = 49;
		  pmtIndx[25] = 46;
		  pmtIndx[26] = 42;
		  pmtIndx[27] = 11;
		  pmtIndx[28] = 15;
		  pmtIndx[29] = 18;
		  pmtIndx[30] = 20;
		  pmtIndx[31] = 121;
		  pmtIndx[32] = 50;
		  pmtIndx[33] = 48;
		  pmtIndx[34] = 45;
		  pmtIndx[35] = 41;
		  pmtIndx[36] = 12;
		  pmtIndx[37] = 16;
		  pmtIndx[38] = 19;
		  pmtIndx[39] = 30;
		  pmtIndx[40] = 40;
		  pmtIndx[41] = 39;
		  pmtIndx[42] = 37;
		  pmtIndx[43] = 34;
		  pmtIndx[44] = 13;
		  pmtIndx[45] = 17;
		  pmtIndx[46] = 28;
		  pmtIndx[47] = 29;
		  pmtIndx[48] = 38;
		  pmtIndx[49] = 36;
		  pmtIndx[50] = 33;
		  pmtIndx[51] = 14;
		  pmtIndx[52] = 25;
		  pmtIndx[53] = 26;
		  pmtIndx[54] = 27;
		  pmtIndx[55] = 35;
		  pmtIndx[56] = 32;
		  pmtIndx[57] = 21;
		  pmtIndx[58] = 22;
		  pmtIndx[59] = 23;
		  pmtIndx[60] = 24;
		  pmtIndx[61] = 31;
		  pmtIndx[62] = 61;
		  pmtIndx[63] = 114;
		  pmtIndx[64] = 113;
		  pmtIndx[65] = 112;
		  pmtIndx[66] = 111;
		  pmtIndx[67] = 62;
		  pmtIndx[68] = 65;
		  pmtIndx[69] = 117;
		  pmtIndx[70] = 116;
		  pmtIndx[71] = 115;
		  pmtIndx[72] = 104;
		  pmtIndx[73] = 63;
		  pmtIndx[74] = 66;
		  pmtIndx[75] = 68;
		  pmtIndx[76] = 119;
		  pmtIndx[77] = 118;
		  pmtIndx[78] = 107;
		  pmtIndx[79] = 103;
		  pmtIndx[80] = 64;
		  pmtIndx[81] = 67;
		  pmtIndx[82] = 69;
		  pmtIndx[83] = 70;
		  pmtIndx[84] = 120;
		  pmtIndx[85] = 109;
		  pmtIndx[86] = 106;
		  pmtIndx[87] = 102;
		  pmtIndx[88] = 71;
		  pmtIndx[89] = 75;
		  pmtIndx[90] = 78;
		  pmtIndx[91] = 80;
		  pmtIndx[92] = 122;
		  pmtIndx[93] = 110;
		  pmtIndx[94] = 108;
		  pmtIndx[95] = 105;
		  pmtIndx[96] = 101;
		  pmtIndx[97] = 72;
		  pmtIndx[98] = 76;
		  pmtIndx[99] = 79;
		  pmtIndx[100] = 90;
		  pmtIndx[101] = 100;
		  pmtIndx[102] = 99;
		  pmtIndx[103] = 97;
		  pmtIndx[104] = 94;
		  pmtIndx[105] = 73;
		  pmtIndx[106] = 77;
		  pmtIndx[107] = 88;
		  pmtIndx[108] = 89;
		  pmtIndx[109] = 98;
		  pmtIndx[110] = 96;
		  pmtIndx[111] = 93;
		  pmtIndx[112] = 74;
		  pmtIndx[113] = 85;
		  pmtIndx[114] = 86;
		  pmtIndx[115] = 87;
		  pmtIndx[116] = 95;
		  pmtIndx[117] = 92;
		  pmtIndx[118] = 81;
		  pmtIndx[119] = 82;
		  pmtIndx[120] = 83;
		  pmtIndx[121] = 84;
		  pmtIndx[122] = 91;
		  
		  int vol, pmt; //don't override iVolume
		  iVolume++;
		  if(iVolume <= 488) pmt = int(0.2*((double)iVolume)-36.4+0.1)+61;
		  if(iVolume >= 493) pmt = int(0.2*((double)iVolume)-36.4+0.1)-61;
		  vol = pmtIndx[pmt]; fiducial = -1;
		  if ( vol < 10 )
		    printf("%i\tPMT_#00%i\t%.0f\t%.0f\n",iEvtNb,vol,S1,S2);
		  else if ( vol >= 10 && vol < 100 )
		    printf("%i\tPMT_#0%i\t%.0f\t%.0f\n",iEvtNb,vol,S1,S2);
		  else
		    printf("%i\tPMT_#%i\t%.0f\t%.0f\n",iEvtNb,vol,S1,S2);
		  TotalS1Hits[iEvtNb-1] += S1;
		  TotalS2Hits[iEvtNb-1] += S2;
		}
		else {
		  fprintf(stderr,"f=%i\t%e\n",fiducial,delta);
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

		delete[] fPrimaryParEnergy_keV;
        delete[] fPrimaryParTime_ns;
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
		delete[] cCreatorProcess;
        delete[] cStepProcess;
	}

	if ( fiducial >= 0 ) {
	  TF1 *exponential = new TF1("exponential","expo");
	  TimeHistogram->Fit("exponential","q"); double tau = -1/exponential->GetParameter(1);
	  double NumPhotons, NumElectrons, NumQuanta;
	  NumPhotons = TotalpHits/(double)iNumRecords; NumElectrons = TotaleHits/(double)iNumRecords;
	  TH1F *PhotonHistogram = new TH1F("PhotonHistogram","Photons",60,0.8*NumPhotons,1.2*NumPhotons);
	  TH1F *ElectronHistogram = new TH1F("ElectronHistogram","Electrons",60,0.2*NumElectrons,1.8*NumElectrons);
	  TH1F *Log10rHistogram = new TH1F("Log10rHistogram","log10((1-r)/r)",400,-2,2);
	  TH1F *LgS2S1Histogram = new TH1F("LgS2S1Histogram","log10(Ne/N_ph)",400,-2,2);
	  for(int i=0;i<iNumRecords;i++) {
	    PhotonHistogram->Fill(fTotalOptPhotNumber[i]);
	    ElectronHistogram->Fill(fTotalIonElecNumber[i]);
	    Log10rHistogram->Fill(log10(((fTotalIonElecNumber[i]*1.06)/(fTotalIonElecNumber[i]+fTotalOptPhotNumber[i]))/
					(1-((fTotalIonElecNumber[i]*1.06)/(fTotalIonElecNumber[i]+fTotalOptPhotNumber[i])))));
	    LgS2S1Histogram->Fill(log10(fTotalIonElecNumber[i]/fTotalOptPhotNumber[i]));
	  }
	  TF1 *gaussian1 = new TF1("gaussian1","gaus");
	  TF1 *gaussian2 = new TF1("gaussian2","gaus");
	  TF1 *gaussian3 = new TF1("gaussian3","gaus");
	  TF1 *gaussian4 = new TF1("gaussian4","gaus");
	  PhotonHistogram->Fit("gaussian1","q");
	  ElectronHistogram->Fit("gaussian2","q");
	  Log10rHistogram->Fit("gaussian3","q");
	  LgS2S1Histogram->Fit("gaussian4","q");
	  double pmean = gaussian1->GetParameter(1);
	  double pwidth = gaussian1->GetParameter(2);
	  double emean = gaussian2->GetParameter(1);
	  double ewidth = gaussian2->GetParameter(2);
	  double rmean = gaussian3->GetParameter(1);
	  double rwidth = gaussian3->GetParameter(2);
	  double lmean = gaussian4->GetParameter(1);
          double lwidth = gaussian4->GetParameter(2);
	  NumQuanta = NumPhotons + NumElectrons;
	  //printf("Type\tS1 (Photons)\tS2 (Electrons)\tQuanta (S1+S2)\t1/e Time (ns)\tEnergy (keV)\tphot/keV\telec/keV\tquant/keV\tlog10(Ne/Nph)\tEscapeProb\tphot/keVG\tPhotRes (%)\telec/keVG\tElecRes (%)\n");
	  //printf("%s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",ParentParticle,NumPhotons,NumElectrons,
	  //       NumQuanta,tau,Energy,NumPhotons/Energy,NumElectrons/Energy,NumQuanta/Energy,
	  //       log10(NumElectrons/NumPhotons),NumElectrons*1.06/NumQuanta,pmean/Energy,(pwidth/pmean)*100,emean/Energy,(ewidth/emean)*100);
	  
	  printf("%s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",ParentParticle,NumPhotons,NumElectrons,
		 NumQuanta,tau,Energy,NumPhotons/Energy,NumElectrons/Energy,NumQuanta/Energy,
		 log10(NumElectrons/NumPhotons),NumElectrons*1.06/NumQuanta,pmean/Energy,(pwidth/pmean)*100,emean/Energy,(ewidth/emean)*100,
		 rmean,pow(rwidth,2.),lmean,pow(lwidth,2.));
	}
	else {
	  for(int i=0; i<iEvtNb; i++) {
	    if ( TotalS1Hits[i] || TotalS2Hits[i] )
	      fprintf(stderr,"%i\t%.0f\t%.0f\t%f\n",i+1,TotalS1Hits[i],TotalS2Hits[i],log10(TotalS2Hits[i]/TotalS1Hits[i]));
	  }
	}
	fin.close();
}
