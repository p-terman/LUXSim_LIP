//	LUXSimStand.hh

//	This is the header file to define the cryostat.

//	2009-09-28 - Initial submission (Melinda)
//	2010-01-07 - Added pieces (Michael)
//  2010-04-04 - Fixed position of support rods and position of
//                stand relative to cryostat. Individual rods are now
//                LUXSimDetectorComponents.
//	2010-04-16 - Added Set* functions for collimators and private
//               collimator variable (Dave)
//	2012-01-16 - Added variable containers for a more realistic collimator
//				 geometry (Kareem)
//	2012-02-06 - Created separate logical volumes for all the collimator and
//				 source parts (Kareem)
//	2012-07-18 - Added variables to handle the lead plate backing in the
//				 collimator (Kareem)
//	2013-03-01 - Added LUXSimManager pointer variable (Kareem)
//	2013-03-18 - Added the SetCollimatorSourceDiameter declaration (Kareem)
//	2013-03-18 - Added LUX neutron collimator tube (Kevin)

#ifndef LUXSimStand_HH
#define LUXSimStand_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimManager.hh"

//
//	Class forwarding
//
class G4Box;
class G4Tubs;
class G4UnionSolid;
class G4SubtractionSolid;
class G4LogicalVolume;
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimStand
{
		
	public:
		LUXSimStand( LUXSimDetectorComponent *standMother, 
					G4double detX, 
					G4double detY, 
					G4double detZ );
		~LUXSimStand();
		
		inline LUXSimDetectorComponent *GetStandColumn() { return standColumn;};
		inline LUXSimDetectorComponent *GetStandBeams()  { return standBeams; };
		inline LUXSimDetectorComponent *GetStandRadial() { return standRadial;};
		inline LUXSimDetectorComponent *GetTower()       { return standTower; };
        inline LUXSimDetectorComponent *GetLevelingRing(){return levelingRing;};
        inline LUXSimDetectorComponent *GetSupportRods() { return rods; };
        
        void SetCollimatorHeight(G4double);
        void SetCollimatorHoleDiameter(G4double);
		void SetCollimatorSourceDiameter(G4double);


	private:
		LUXSimManager *luxManager;
		G4bool underground;

                G4bool useNeutronCollimator;
	
		LUXSimDetectorComponent *standColumn;
		LUXSimDetectorComponent *standBeams;
        LUXSimDetectorComponent *standRadial;
        LUXSimDetectorComponent *standTower;
        LUXSimDetectorComponent *rods;
        LUXSimDetectorComponent *levelingRing;

    public:
		inline LUXSimDetectorComponent *GetX() { return xaxis; };
		inline LUXSimDetectorComponent *GetY() { return yaxis; };
		inline LUXSimDetectorComponent *GetZ() { return zaxis; };

    private:
		LUXSimDetectorComponent *xaxis;
		LUXSimDetectorComponent *yaxis;
        LUXSimDetectorComponent *zaxis;
        
		G4double collimatorCavityOffset;
		G4double sourceCavityDiameter;
		G4double sourceCavityHeight;
		G4double collimatorBoreDepth;
		G4double collimatorBoreOffset;
		G4double boreHoleXOffset;
		G4double boreHoleZOffset;
		G4double mainblockBoreDepth;
		G4double sourceCavityOffset;
		G4double leadPlateDepth;
		G4double leadPlateOffset;
		G4double collimatorHousing_z;
		G4double collimator_z;
		G4double sourceHousingOffset;
		G4double sourceHousing_z;
		G4double activeSourceDiameter;
		G4double neutronCollimatorDiamInner;
		G4double neutronCollimatorDiamOuter;
		G4double neutronCollimatorLength;
		G4double neutronCollimatorHeight;
		G4double sourceTubeGap;
		
		G4Box *mainblock_solid1;
		G4Tubs *mainblock_solid2;
		G4VSolid *mainblock_solid;
		G4Box *collimatorCavity1;
		G4Tubs *sourceCavity1;
		G4Tubs *sourceCavity2;
		G4UnionSolid *sourceCavity_solid;
		G4SubtractionSolid *collimator_solid;
		G4Tubs *source_solid;
		G4Tubs *neutronCollimator_solid;
		G4Tubs *neutronCollimatorInside_solid;
		
		G4LogicalVolume *collimatorVolume_log[6];
		LUXSimDetectorComponent *collimator_Volume[6];
		G4LogicalVolume *collimatorHousing_log[6];
		LUXSimDetectorComponent *collimatorHousing[6];
		G4LogicalVolume *collimator_log[6];
		LUXSimDetectorComponent *collimator[6];
		G4LogicalVolume *leadPlate_log[6];
		LUXSimDetectorComponent *leadPlate[6];
		G4LogicalVolume *sourceHousing_log[6];
		LUXSimDetectorComponent *sourceHousing[6];
		G4LogicalVolume *collimatorSource_log[6];
		LUXSimDetectorComponent *collimatorSource[6];
		G4LogicalVolume *neutronCollimator_log;
		LUXSimDetectorComponent *neutronCollimator;
		G4LogicalVolume *neutronCollimatorInside_log;
		LUXSimDetectorComponent *neutronCollimatorInside;

};

#endif
