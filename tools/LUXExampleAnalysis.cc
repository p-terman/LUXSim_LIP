////////////////////////////////////////////////////////////////////////////////
//	 LUXExampleAnalysis.cc
//
//	An example of how to access data from the LUXSim TTree.  This script
//	includes code that grabs a PMT lookup table by number, and references
//	the location of that number PMT.  For now, two histograms of the top
//	and bottom PMT response are included.  To run, type the follwing in
//	the command line: 
//	gmake
//	./exampleAnalysis LUXOut837493234.root
//	Two plots for the top and bottom PMT hit maps will be created as .pdf
//	(Melinda)
//
///////////////////////////////////////////////////////////////////////////////
//	Change Log:
//
//	20 Apr 2010 - Initiate from exampleAnalysis.C (Melinda Sweany)
//
//      30 Nov 2011 - Up-to-date volume names and top/bot counting (Matthew S)
//
//      23 Feb 2012 - Same as above (update names again) (Matthew S)
//
//      11 Apr 2012 - Commented out lines which cause seg fault for record
//                    levels > 1 and checked code works for phe (Matthew)
//      
//      18 May 2012 - Made output one line easy to funnel to file (Matthew)
//
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

//
//	ROOT includes
//
#include "Riostream.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"
#include "TRandom3.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TPaveText.h"
#include "TText.h"

//  
//  C/C++ includes
//
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>

using namespace std;

//  Definitions
const int MAXLENGTH = 5000000;

int main( int argc, char** argv){

	gStyle->SetOptStat(0);
	gStyle->SetPalette(1);
	gStyle->SetFrameFillColor(kWhite);
	gStyle->SetCanvasColor(kWhite);
	gStyle->SetTitleFillColor(kWhite);
	
	char * inFilename = argv[1];
	//cout << "Reading in " << inFilename << " ..." << endl;
	TFile InFile(inFilename);

	//	Grab the header tree
	TTree * headerTree = (TTree*) InFile.Get("header");
	Char_t DetCompo[MAXLENGTH];
	headerTree->SetBranchAddress("DetCompo", DetCompo); 
	headerTree->GetEntry(0);

	int numPmts = 61;	//	numPmts in top/bottom array
	int topPmtLookup[numPmts];
	int bottomPmtLookup[numPmts];


	//	create the PMT lookup tables from the detector component 
	//	basket in the header, the index is the pmt number -1
	char * str1 = strtok (DetCompo," ");
	char * str2 = strtok (NULL," ");
	char * str3 = strtok (NULL,"\n");
	while(strcmp(str3,"VacuumVessel")!=0){
		str1 = strtok (NULL," ");
		str2 = strtok (NULL," ");
		str3 = strtok (NULL,"\n");
		char * topName = (char*)"Top_PMT_PhotoCathode_";
		char * bottomName = (char*)"Bottom_PMT_PhotoCathode_";
		if(strncmp(str3,topName,strlen(topName)) == 0){
			size_t length;
			char pmtNumber[5];
			string componentName = (string)str3;
			length = componentName.copy(pmtNumber,
					strlen(str3)-strlen(topName),strlen(topName));
			pmtNumber[length] = '\0';
			char volumeNumber[5];	
			strncpy(volumeNumber,str2,strlen(str2)-1);
			volumeNumber[strlen(str2)-1] = '\0';
			topPmtLookup[atoi(pmtNumber)-1] = atoi(volumeNumber);
		}
		else if(strncmp(str3,bottomName,strlen(bottomName)) == 0){
			size_t length;
			char pmtNumber[5];
			string componentName = (string)str3;
			length = componentName.copy(pmtNumber,
					strlen(str3)-strlen(bottomName),strlen(bottomName));
			pmtNumber[length] = '\0';
			char volumeNumber[5];
			strncpy(volumeNumber,str2,strlen(str2)-1);
			volumeNumber[strlen(str2)-1] = '\0';
			bottomPmtLookup[atoi(pmtNumber)-1] = atoi(volumeNumber);
		}
	}


	Double_t pmtSpace = 5.9;
	
	//	pmtX and pmtY are the x,y coordinates of the pmts
	//	the index is the pmt number -1
	Double_t topPmtHits[numPmts];
	Double_t bottomPmtHits[numPmts];
	double pmtX[numPmts];
	double pmtY[numPmts];
	for(int p=0; p<numPmts; p++){
		topPmtHits[p]=0.;
		bottomPmtHits[p]=0.;
		if(p < 5) {
			pmtY[p] = 4 * sqrt(3.)/2 * pmtSpace;
			pmtX[p] = (p - 2) * pmtSpace;
		}
		else if(p < 5+6) {
			pmtY[p] = 3 * sqrt(3.)/2 * pmtSpace;
			pmtX[p] = (p - 7.5) * pmtSpace;
		}
		else if(p < 5+6+7) {
			pmtY[p] = 2 * sqrt(3.)/2 * pmtSpace;
			pmtX[p] = (p - 14) * pmtSpace;
		}
		else if(p < 5+6+7+8) {
			pmtY[p] = 1 * sqrt(3.)/2 * pmtSpace;
			pmtX[p] = (p - 21.5) * pmtSpace;
		}
		else if(p < 5+6+7+8+9) {
			pmtY[p] = 0;
			pmtX[p] = (p - 30) * pmtSpace;
		}
		else if(p < 5+6+7+8+9+8) {
			pmtY[p] = -1 * sqrt(3.)/2 * pmtSpace;
			pmtX[p] = (p - 38.5) * pmtSpace;
		}
		else if(p < 5+6+7+8+9+8+7) {
			pmtY[p] = -2 * sqrt(3.)/2 * pmtSpace;
			pmtX[p] = (p - 46) * pmtSpace;
		}
		else if(p < 5+6+7+8+9+8+7+6) {
			pmtY[p] = -3 * sqrt(3.)/2 * pmtSpace;
			pmtX[p] = (p - 52.5) * pmtSpace;
		}
		else if(p < 5+6+7+8+9+8+7+6+5) {
			pmtY[p] = -4 * sqrt(3.)/2 * pmtSpace;
			pmtX[p] = (p - 58) * pmtSpace;
		}
	}

	//	Grab the event tree 	
	TTree * eventTree = (TTree*) InFile.Get("tree");

	Int_t recordSize, eventNumber, volumeName, stepNumber; 
	Int_t particleID, trackID, parentID, totalNumPhotons;
	Char_t particleName;
	Double_t kineticEnergy, photonWavelength, xDirection, yDirection, zDirection;
	Double_t energyDeposit, xPosition, yPosition, zPosition, stepTime;
	Double_t totalEnergyDeposit;

	//	Set the branch address of the tree to the variables
	//	previously declared
	eventTree->SetBranchAddress("iRecordSize", &recordSize);
	eventTree->SetBranchAddress("iEvtN", &eventNumber); 
	eventTree->SetBranchAddress("iV_name", &volumeName);
	/*eventTree->SetBranchAddress("iStepNum", &stepNumber);
	eventTree->SetBranchAddress("iParticleID", &particleID);
	eventTree->SetBranchAddress("cParticleName", &particleName);
	eventTree->SetBranchAddress("iTrackID", &trackID);
	eventTree->SetBranchAddress("iParentID", &parentID);
	eventTree->SetBranchAddress("fKEnergy_keV", &kineticEnergy);
	eventTree->SetBranchAddress("fOptPhoWaveLength_nm", &photonWavelength);
	eventTree->SetBranchAddress("fDirectionX", &xDirection);
	eventTree->SetBranchAddress("fDirectionY", &yDirection);
	eventTree->SetBranchAddress("fDirectionZ", &zDirection);
	eventTree->SetBranchAddress("fEnergyDep_keV", &energyDeposit);
	eventTree->SetBranchAddress("fPositionX_cm", &xPosition);
	eventTree->SetBranchAddress("fPositionY_cm", &yPosition);
	eventTree->SetBranchAddress("fPositionZ_cm", &zPosition);
	eventTree->SetBranchAddress("fStepTime", &stepTime);
	eventTree->SetBranchAddress("fTotEDep", &totalEnergyDeposit); 
	eventTree->SetBranchAddress("iTotOptNum", &totalNumPhotons);*/

	//	Declare histograms
	TH2D * BOTTOM_HITS = new TH2D("BOTTOM_HITS","Bottom PMT Hit Pattern",17,-23.,23.,17,-26.5,26.5);
	TH2D * TOP_HITS = new TH2D("TOP_HITS","Top PMT Hit Pattern",17,-23.,23.,17,-26.5,26.5);

	Int_t numEntries = eventTree->GetEntries();
	//	Loop over entries in the event tree
	for(int e=0; e<numEntries; e++){
		eventTree->GetEntry(e);
		if(1) {
			for(int p=0; p<numPmts; p++) {
				if(bottomPmtLookup[p] == volumeName){
					bottomPmtHits[p]++;
				}
				else if(topPmtLookup[p] == volumeName) {
					topPmtHits[p]++;
				}
			}
		}
	}

	Int_t topPmtHitsTotal = 0;
	Int_t bottomPmtHitsTotal = 0;
	for(int p=0; p<numPmts; p++) {
	  topPmtHitsTotal += (Int_t)topPmtHits[p];
	  bottomPmtHitsTotal += (Int_t)bottomPmtHits[p];
	}
	printf("%s\t%i\t%i\t%i\t%f\n",inFilename,numEntries,topPmtHitsTotal,bottomPmtHitsTotal,
	       (double(topPmtHitsTotal)-double(bottomPmtHitsTotal))/(double(topPmtHitsTotal)+double(bottomPmtHitsTotal)));
	
	//	Fill the histograms
	for(int p=0; p<numPmts; p++){
		BOTTOM_HITS->Fill(pmtX[p],pmtY[p],bottomPmtHits[p]);
		TOP_HITS->Fill(pmtX[p],pmtY[p],topPmtHits[p]);		
	}

	TCanvas * c1 = new TCanvas();
	TCanvas * c2 = new TCanvas();
	c1->cd();
	TOP_HITS->Draw("LEGO2");
	//c1->Print("TOP_PMT_HITMAP.pdf");
	c2->cd();
	BOTTOM_HITS->Draw("LEGO2");
	//c2->Print("BOTTOM_PMT_HITMAP.pdf");

	InFile.Close();
}
