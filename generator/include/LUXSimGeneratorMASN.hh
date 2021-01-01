////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorMASN.hh
*
* This is the header file for the AmBe generator.
*
********************************************************************************
* Change log
*    20 April 2009 - Initial submission (Kareem)
*    27-May-2009 - This generator now works (Kareem)
*    14-Jul-2012 - GenerateEvent changed to use binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorMASN_HH
#define LUXSimGeneratorMASN_HH 1

//
//    GEANT4 includes
//
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorMASN : public LUXSimSource
{
    public:
        LUXSimGeneratorMASN();
        ~LUXSimGeneratorMASN();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);
        //using LUXSimSource::GenerateEvent;
        //void GenerateEvent( G4GeneralParticleSource*, G4Event* );

    private:
        G4double GetMuonEnergy();
        G4double GetMuonAngle();
        G4double GetNeutronEnergy();
        G4double GetNeutronMultiplicity();
        G4double GetNeutronAngle();

    private:
        G4ParticleDefinition *neutronDef;
        G4ParticleDefinition *muonDef;
        
        G4double muonAngleCDF[1001];
        G4double muonAngle[1001];
        G4double muonEnergyCDF[1001];
        G4double muonEnergy[1001];

        G4double neutronAngleCDF[1001];
        G4double neutronAngle[1001];
        G4double neutronMultCDF[1001];
        G4double neutronMult[1001];
        G4double neutronEnergyCDF[1001];
        G4double neutronEnergy[1001];
};

#endif
