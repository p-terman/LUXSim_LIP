////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorG4Decay.hh
*
* This is the header file for the G4Decay generator.
*
********************************************************************************
* Change log
*    06-Oct-2015 - Initial submission (David W)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorG4Decay_HH
#define LUXSimGeneratorG4Decay_HH 1

//
//    GEANT4 includes
//
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "globals.hh"
#include "G4GenericIon.hh"


//
//    LUXSim includes
//
#include "LUXSimBST.hh"
#include "LUXSimIsotope.hh"
#include "LUXSimSource.hh"


//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorG4Decay : public LUXSimSource
{
    public:
        LUXSimGeneratorG4Decay();
        ~LUXSimGeneratorG4Decay();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4int,
			  G4int, G4double);
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);
        //using LUXSimSource::GenerateEvent;
        //void GenerateEvent( G4GeneralParticleSource*, G4Event* );
        std::vector<G4int> GetDecayChainPDGNumbers(G4GeneralParticleSource *particleGun);
        std::vector<G4ParticleDefinition*> ProcessParticleDefs(std::vector<G4ParticleDefinition*> particleDefs, G4GeneralParticleSource
							       *particleGun);
        void GetPDG(int x, std::vector<int> &v);

    protected:
        G4double GetEnergy() {return 0.*keV; };
        G4ParticleDefinition *GetParticleDefinition()
              { return G4GenericIon::Definition(); };

    private:
        G4ParticleDefinition *ion ;
        G4String uiString;
        G4int nucleusA;
        G4int nucleusZ;
};

#endif
