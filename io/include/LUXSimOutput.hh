////////////////////////////////////////////////////////////////////////////////
/*	LUXSimOutput.hh
*
* This is the header file to control the LUXSim output. This output is solely to
* a general-purpose binary format, and should never be geared specifically
* toward either ROOT or Matlab. There will be separate projects to create ROOT-
* and Matlab-based readers for this binary format.
*
********************************************************************************
* Change log
*	13 Mar 2009 - Initial submission (Kareem)
*	10 Apr 2009 - Added binary output (Chao Zhang)
*	26 Feb 2010 - Added fName variable, to keep record of file name (Dave)
*	17 Mar 2010 - Added numRecords variable, to keep track of number of records
*				  written to file. (Dave)
*	5  May 2010 - Added primary particle information (Chao)
*	02 Dec 2011 - The output class now records the creator process for record
*				  levels 2-4 and optical record levels 3 and 4 (Kareem)
*   02 Aug 2013 - Superstitiously changed the order of includes (Kareem)
*   20 Aug 2015 - Added the step process to the output file (Kareem)
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimOutput_HH
#define LUXSimOutput_HH 1

//
//	C/C++ includes
//
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//
//	GEANT4 includes
//
#include "globals.hh"

//
//	Class forwarding
//
class LUXSimManager;
class LUXSimDetectorComponent;

using namespace std;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimOutput
{
	public:
		LUXSimOutput();
		~LUXSimOutput();
		
	public:
		void RecordEventByVolume( LUXSimDetectorComponent*, G4int );
		void RecordInputHistory();	
	
	private:
		LUXSimManager *luxManager;
		
		G4String fName;
		ofstream fLUXOutput;

		G4int Size;
		G4int particleNameSize;
		G4int creatorProcessSize;
        G4int stepProcessSize;
		G4String GMT; // Time & Date
		G4String G4Ver; // G4 version & Date
		G4String SimVer; // SVN version
		G4String uname;  // Name of computer	
		G4String DetCompo;
		G4String commands;
		G4String differ;

		G4int recordLevel;
		G4int volume;
		G4int eventNumber;
		G4int recordSize;  // Total number recorded for the volume/event 
		G4int numRecords;

		G4String particleName;
		struct datalevel {
			G4int stepNumber;
			G4int particleID;
			G4int trackID;
			G4int parentID;
			G4double particleEnergy;
			G4double particleDirection[3];
			G4double energyDeposition;
			G4double position[3];
			G4double stepTime;
		} data;
		G4String creatorProcess;
        G4String stepProcess;

		G4double totalVolumeEnergy;
		G4int optPhotRecordLevel;
		G4int totalOptPhotNumber;
		
		G4int thermElecRecordLevel;		
		G4int totalThermElecNumber;

		G4int    primaryParSize;
		G4String primaryParName;
		G4double primaryParEnergy_keV;
		G4double primaryParTime_ns;
		G4double primaryParPos_mm[3];
		G4double primaryParDir[3];

};

#endif
