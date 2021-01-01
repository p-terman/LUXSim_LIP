////////////////////////////////////////////////////////////////////////////////
/*	LUXSimDetectorComponent.hh
*
* This is the header file to define the detector component class. The detector
* components inherit from G4VPhysicalVolumes, but they contain additional
* information and methods related to event generation and output recording.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	20 April 2009 - Added Initialize and AddSource methods (Kareem)
*	22 April 2009 - Added support for radioactive loads / event generation
*					(Kareem)
*	08-May-09 - Added method to record the object's coordinates in the global
*				reference frame (Kareem)
*	27-Aug-09 - Added two more constructor methods in keeping with all the
*				possible ways to create a G4PVPlacement (Kareem)
*	14-Sep-09 - Added the optical photon record level Get and Set methods
*				(Kareem)
*   4-Jan-10  - Extended source structure to include A and Z for Single nucleus
*               decays. (Nick)
*	25-Jan-10 - Added x, y, and z extents for the shape of the component. These
*				values will be used to determine the location of primary events.
*				Also cleaned up variables related to the old way of determining
*				the smallest bounding sphere. (Kareem)
*	2 -Apr-10 - Added a sources reset method (Nick)
*	31-Jan-11 - Added the globalOrientation variable for event generation
*				purposes (Kareem)
*   21-Jul-11 - Added Decay Chain Generating methods (Nick)
*   04-Mar-12 - Added SingleParticle generating methods (Nick)
*	01-May-12 - Added support for the thermal electron record level (Kareem)
*   18-Jun-12 - Added storage for Wimp masses for that source (Daniel)
*   15-Jul-12 - All sources in a binary search tree. Add GenerateEventList
*               and GenerateFromEventList (Nick)
*   26-Sep-13 - Added Get methods for the min and max XYZ of the component
*               (Kareem)
*   03-Sep-14 - Added method declarations for calculating and retrieving the
*               volume and mass of the component (Kareem)
*   14-Oct-14 - Added Set methods for the mass and volume (Kareem)
*   28-Aug-15 - Changed the source stucture and AddSource method to accomodate
*                           point sources (David W)
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimDetectorComponent_HH
#define LUXSimDetectorComponent_HH 1

//
//	C/C++ includes
//
#include <vector>

//
//	GEANT4 includes
//
#include "G4PVPlacement.hh"
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimManager.hh"

//
//	Class forwarding
//
class G4GeneralParticleSource;
class G4Event;
class G4PVPlacement;
class G4Navigator;

class LUXSimSource;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimDetectorComponent : public G4PVPlacement
{
	public:
		struct source {
			LUXSimSource *type;
			G4double activity;
			G4double ratio;
            G4int mass; //single decay
            G4int number;
            G4String parentIsotope; //decay chain
            G4double sourceAge;//in seconds
            G4String particleName; //single particle
            G4double particleEnergy;//doubles for singleparticle and wimpMass
			//G4double wimpMass; //Wimp
	    G4bool pointSource;//is this a point source?  
	    G4ThreeVector posSource;//position of point source
		};

	public:

		LUXSimDetectorComponent( G4RotationMatrix *pRot,
								 const G4ThreeVector &tlate,
								 G4LogicalVolume *pCurrentLogical,
								 const G4String &pName,
								 G4LogicalVolume *pMotherLogical,
								 G4bool pMany,
								 G4int pCopyNo,
								 G4bool pSurfChk=false );
		
		LUXSimDetectorComponent( const G4Transform3D &Transform3D,
								 G4LogicalVolume *pCurrentLogical,
								 const G4String &pName,
								 G4LogicalVolume *pMotherLogical,
								 G4bool pMany,
								 G4int pCopyNo,
								 G4bool pSurfChk=false );

		LUXSimDetectorComponent( G4RotationMatrix *pRot,
								 const G4ThreeVector &tlate,
								 const G4String &pName,
								 G4LogicalVolume *pLogical,
								 G4VPhysicalVolume *pMother,
								 G4bool pMany,
								 G4int pCopyNo,
								 G4bool pSurfChk=false );
		
		LUXSimDetectorComponent( const G4Transform3D &Transform3D,
								 const G4String &pName,
								 G4LogicalVolume *pLogical,
								 G4VPhysicalVolume *pMother,
								 G4bool pMany,
								 G4int pCopyNo,
								 G4bool pSurfChk=false );
								 
		~LUXSimDetectorComponent();
		
		void Initialize();
	
		inline G4int GetRecordLevel() { return recordLevel; };
		inline void SetRecordLevel( G4int level ) { recordLevel = level; };
		
		inline G4int GetRecordLevelOptPhot() { return recordLevelOptPhot; };
		inline void SetRecordLevelOptPhot( G4int level )
				{ recordLevelOptPhot = level; };
		
		inline G4int GetRecordLevelThermElec() { return recordLevelThermElec; };
		inline void SetRecordLevelThermElec( G4int level )
				{ recordLevelThermElec = level; };
		
		void AddDeposition( LUXSimManager::stepRecord aStepRecord )
				{ eventRecord.push_back(aStepRecord);};
		void ClearRecord() { eventRecord.clear(); };
		std::vector<LUXSimManager::stepRecord> GetEventRecord()
				{ return eventRecord; };
		
		void SetID( G4int ID ) { compID = ID; };
		G4int GetID() { return compID; };
		
		void AddSource( LUXSimSource*, G4double, G4int, G4int, G4String, 
				G4double, G4String, G4double, G4bool, G4ThreeVector);
		void ResetSources() {sources.clear(); totalActivity=0;};
		std::vector<source> GetSources() { return sources; };
		void CalculateRatios();
		G4double GetTotalActivity() { return totalActivity; };
		void GenerateFromEventList( G4GeneralParticleSource*, G4Event*,
                decayNode* );
        void GenerateEventList(G4int);
		void DetermineCenterAndExtent( G4PVPlacement* );
		G4ThreeVector GetGlobalCenter() { return globalCenter; };
        G4ThreeVector GetMinXYZ() { return minXYZ; };
        G4ThreeVector GetMaxXYZ() { return maxXYZ; };
    
        void SetVolumePrecision( G4double );
        G4double CalculateVolume( G4bool takeOutDaughters = true );
        G4double CalculateMass();
        G4double GetVolume();
        G4double GetMass();
        void SetVolume( G4double );
        void SetMass( G4double );
		
		G4bool GetCapturePhotons() { return capturePhotons; };
		
	private:
		G4ThreeVector GetEventLocation();

	private:

		G4int recordLevel;
		G4int recordLevelOptPhot;
		G4int recordLevelThermElec;
		std::vector<LUXSimManager::stepRecord> eventRecord;
		G4int compID;
		
		std::vector<source> sources;
		G4double totalActivity;
		G4double maxX, minX;
		G4double maxY, minY;
		G4double maxZ, minZ;
		G4ThreeVector globalCenter;
        G4ThreeVector minXYZ;
        G4ThreeVector maxXYZ;
		G4RotationMatrix globalOrientation;
		G4double xPos, yPos, zPos;
		G4double prob;
    
        G4double volume;
        G4double mass;
        G4int volumePrecision;

		G4bool capturePhotons;
		
		LUXSimManager *luxManager;		
		G4Navigator *navigator;
};

#endif
