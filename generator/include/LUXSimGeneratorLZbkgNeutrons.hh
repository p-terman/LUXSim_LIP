////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorLZbkgNeutrons.hh
*
* This is the header file for the LZ bkg neutron generator.
*
********************************************************************************
* Change log
*    31 March 2015 - Initial submission (Scott Haselschwardt)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorLZbkgNeutrons_HH
#define LUXSimGeneratorLZbkgNeutrons_HH 1

//
//    GEANT4 includes
//
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorLZbkgNeutrons : public LUXSimSource
{
    public:
        LUXSimGeneratorLZbkgNeutrons();
        ~LUXSimGeneratorLZbkgNeutrons();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);
        //using LUXSimSource::GenerateEvent;
        //void GenerateEvent( G4GeneralParticleSource*, G4Event* );

    private:
        G4double GetNeutronEnergy();
        void SetPosition();
        void LoadWeights( G4String fileName );
        void LoadNeutronEnergies( G4String fileName );
        
    private:
        G4ParticleDefinition *ionDef;
        G4ParticleDefinition *neutronDef;
        G4ParticleDefinition *geantinoDef;
        
        //num bins for input temp plots
        G4int numZbins;
        G4int numR2bins;
        G4int numWeights;
        //bin widths
        G4double zbinWidth;
        G4double r2binWidth;
        //final positions
        G4double z;
        G4double r2;
        G4double x;
        G4double y;
        //final neutron energy
        G4double energy;
        
        //weights from temp plot
        //G4double weights[numWeights];
        
        G4double weights2[1125][180];
        
        G4double zbinCenters[180];
        G4double r2binCenters[1125];
        
        G4double energyWeights[100];
        G4double neutronEnergies[100];
        
};

#endif
