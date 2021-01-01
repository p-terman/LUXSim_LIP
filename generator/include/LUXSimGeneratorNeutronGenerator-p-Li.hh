////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorAmBe.hh
*
* This is the header file for the AmBe generator.
*
********************************************************************************
* Change log
*    25 August 2011 - Initial submission (Mike)
*    14-Jul-2012 - GenerateEvent changed to use binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorpLithium_HH
#define LUXSimGeneratorpLithium_HH 1

//
//    GEANT4 includes
//
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorpLithium : public LUXSimSource
{
    public:
        LUXSimGeneratorpLithium();
        ~LUXSimGeneratorpLithium();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);
        //using LUXSimSource::GenerateEvent;
        //void GenerateEvent( G4GeneralParticleSource*, G4Event* );

    private:
        void GetNeutronEnergy();


    private:
        G4ParticleDefinition *ionDef;
        G4ParticleDefinition *neutronDef;
        G4ParticleDefinition *gammaDef;
        
        G4double neutronAnglePDF[28000];
        G4double neutronEnergyPDF[28000];
        G4double neutronCountPDF[28000];
    
        G4double energy;
        G4double angle;
    
        G4double neutronEnergy;
        G4double neutronAngle;
    
        G4double testcount;
    
        G4double lowang;
        G4double highang;
        G4double lowangDegrees;
        G4double highangDegrees;

        G4double phistartDegrees;
        G4double phitotalDegrees;
        G4double phistart;
        G4double phitotal;
    
};

#endif
