////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorSingleDecay.hh
*
* This is the header file for the SingleDecay generator.
*
********************************************************************************
* Change log
*   04-Jan-10 - Initial submission (Nick)
*   04-Mar-12 - Fixed bug where primary particle was both the LUXSource default
*               and the user specified SingleDecay (Nick)
*   14-Jul-12 - GenerateEvent method changed to use binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorSingleDecay_HH
#define LUXSimGeneratorSingleDecay_HH 1

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
class LUXSimGeneratorSingleDecay : public LUXSimSource
{
    public:
        LUXSimGeneratorSingleDecay();
        ~LUXSimGeneratorSingleDecay();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4int, 
                            G4int, G4double);
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);

        //using LUXSimSource::GenerateEvent;
        //void GenerateEvent( G4GeneralParticleSource*, G4Event*, G4int, G4int );

    protected:
        G4double GetEnergy() {return 0.*keV; };
        G4ParticleDefinition *GetParticleDefinition() 
            { return G4GenericIon::Definition(); };

    private:
        G4ParticleDefinition *ion ;
        //std::stringstream uiStream;
        G4String uiString;
        G4int nucleusA;
        G4int nucleusZ;


};

#endif
