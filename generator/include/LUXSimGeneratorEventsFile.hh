////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorEventsFile.hh
*
* This is the header file for the EventsFile generator.
*
********************************************************************************
* Change log
*   12-May-14 - Initial submission (Kevin)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorEventsFile_HH
#define LUXSimGeneratorEventsFile_HH 1

//
//    GEANT4 includes
//
#include "G4ParticleDefinition.hh"
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorEventsFile : public LUXSimSource
{
    public:
        LUXSimGeneratorEventsFile();
        ~LUXSimGeneratorEventsFile();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);

    private:
        G4ParticleDefinition *ionDef;
        G4ParticleDefinition *electronDef;
        G4ParticleDefinition *gammaDef;
        
};

#endif
